import bindings from 'bindings';
import { WebSocketServer } from 'ws';
export const wisp = bindings('wispservercpp');

const socketMaps = {};

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
export const init = () => {
  wisp.Init(sendCallback);
  wsServer.on('connection', socket => {
    let id = wisp.NextID();
    socketMaps[id] = socket;
    wisp.Open(id, sendCallback)

    socket.on('message', message => wisp.Message(id, message.toString(), sendCallback));
  });

}
