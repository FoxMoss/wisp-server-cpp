#include "nodeBinding.hpp"
#include <condition_variable>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <napi.h>
#include <thread>
#include <vector>

uint32_t nextId = 0;
std::mutex mtx;
std::condition_variable cv;
Napi::ThreadSafeFunction tsfn;
std::thread nativeThread;
std::vector<uint32_t> socketMap;
struct sendMessage {
  char *data;
  size_t size;
  void *id;
  bool exit;
};
std::vector<struct sendMessage> messageStack;

uint32_t NextID() { return nextId++; }

void sendCallback(void *data, size_t size, void *id, bool exit) {
  std::unique_lock<std::mutex> lock(mtx);

  struct sendMessage message;

  message.data = (char *)malloc(size);

  memcpy(message.data, data, size);
  message.size = size;
  message.id = id;
  message.exit = exit;
  messageStack.push_back(message);

  lock.unlock();
  cv.notify_one();
}

// sendCallback
Napi::Value Init(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  tsfn = Napi::ThreadSafeFunction::New(env, info[0].As<Napi::Function>(),
                                       "Callback Watcher", 0, 1,
                                       [](Napi::Env) { nativeThread.join(); });

  nativeThread = std::thread([env] {
    auto callback = [](Napi::Env env, Napi::Function jsCallback,
                       struct sendMessage *value) {
      if (value->data != NULL && value->id != NULL) {
        char *data = value->data;
        size_t size = value->size;
        uint32_t id = *((uint32_t *)value->id);
        bool exit = value->exit;

        Napi::TypedArrayOf<uint8_t> message =
            Napi::TypedArrayOf<uint8_t>::New(env, size);
        memcpy(message.Data(), data, size);

        Napi::Value exitVal = Napi::Boolean::New(env, exit);
        Napi::Value idVal = Napi::Number::New(env, id);

        std::vector<Napi::Value> argv = {exitVal, message, idVal};

        jsCallback.Call(argv);
        free(value);
      }
    };
    while (true) {
      std::unique_lock<std::mutex> lock(mtx);

      cv.wait(lock, [] { return !messageStack.empty(); });

      struct sendMessage *messageQueued =
          (struct sendMessage *)malloc(sizeof(struct sendMessage));

      memcpy(messageQueued, messageStack.begin().base(),
             sizeof(struct sendMessage));

      tsfn.BlockingCall(messageQueued, callback);
      messageStack.erase(messageStack.begin());
    }
  });

  Napi::Promise::Deferred promise(env);

  return promise.Promise();
}

// args: id
Napi::Value Open(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  uint32_t id = NextID();
  socketMap.push_back(id);

  for (auto needle = socketMap.begin(); needle != socketMap.end(); needle++) {
    if (*needle.base() == id) {
      open_interface(sendCallback, needle.base());
    }
  }

  return Napi::Number::New(env, id);
}

// args: id, message
Napi::Value Message(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  uint32_t id = info[0].As<Napi::Number>().Uint32Value();

  Napi::Buffer<uint8_t> buffer = info[1].As<Napi::Buffer<uint8_t>>();

  unsigned char *cString = buffer.Data();
  size_t size = buffer.Length();

  if (size < 5) {
    // prolly invalid
    return info[0];
  }

  std::string cppString;

  for (size_t x = 0; x < size; x++) {
    cppString.push_back(cString[x]);
  }

  for (auto needle = socketMap.begin(); needle != socketMap.end(); needle++) {
    if (*needle.base() == id) {
      message_interface(sendCallback, cppString, needle.base());
    }
  }
  return info[0];
}

// args: id
Napi::Value Close(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  uint32_t id = info[0].As<Napi::Number>().Uint32Value();
  std::cout << "Exit\n";

  for (auto needle = socketMap.begin(); needle != socketMap.end(); needle++) {
    if (*needle.base() == id) {
      close_interface(sendCallback, needle.base());
    }
  }
  return info[0];
}

Napi::Object Initialize(Napi::Env env, Napi::Object exports) {
  exports.Set("Open", Napi::Function::New(env, Open));
  exports.Set("Close", Napi::Function::New(env, Close));
  exports.Set("Message", Napi::Function::New(env, Message));
  exports.Set("Init", Napi::Function::New(env, Init));

  return exports;
}

NODE_API_MODULE(wispservercpp, Initialize)
