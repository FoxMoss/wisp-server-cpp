import bindings from 'bindings';
import { parse, stringify } from 'yaml' // yaml because i think its rlly funny to not do json
import { WebSocketServer } from 'ws';
import { response } from 'express';
export const wisp = bindings('wispservercpp');

const socketMaps = {};
let path;

const sendCallback = (exit, msg, id) => {
  let socket = socketMaps[id];
  socket.send(msg);
  if (exit) {
    socket.close();
  }
}

const wsServer = new WebSocketServer({ noServer: true });

export const routeUpgrade = (req, socket, head) => {
  wsServer.handleUpgrade(req, socket, head, socket => {
    wsServer.emit('connection', socket, req);
  });
}
export const init = (pathPass = "/wisp") => {
  path = pathPass;
  wisp.Init(sendCallback);
  wsServer.on('connection', socket => {
    let id = wisp.Open(sendCallback)

    socketMaps[id] = socket;

    socket.on('message', message => {
      wisp.Message(id, message, sendCallback);
    });
    socket.on('close', () => wisp.Close(id));
  });

}
export const shouldRoute = (req) => {
  if (path == undefined) {
    console.error("Wisp not init'ed");
    return false;
  }
  return req.url.startsWith(path);
}
const createResp = (body, res, status = 200) => {
  res.writeHead(status, { 'Content-Type': "text/plain" });
  res.write(body);
  res.end();
}
export const routeRequest = (req, res) => {
  let reqPath = req.url.slice(path.length);
  switch (reqPath) {
    case "/":
      res = createResp(stringify({
        info: {
          name: "wisp-server-cpp",
          repo: "https://github.com/FoxMoss/wisp-server-cpp",
          author: "FoxMoss"
        }
      }), res);
      break;
    default:
      res = createResp(stringify({
        error: "Route not found."
      }), res, 404);
      break;
  }
}
