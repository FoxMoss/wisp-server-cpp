import { init, routeRequest, routeUpgrade, shouldRoute } from "./index.js"
import http from 'node:http'
import express from "express"

init();
const app = express();
const server = http.createServer();

server.on('request', (request, response) => {
  if (shouldRoute(request)) {
    response.setHeader
    routeRequest(request, response);
    return;
  }
  app(request, response);
});
server.on('upgrade', (request, socket, head) => {
  if (shouldRoute(request)) {
    routeUpgrade(request, socket, head);
    return;
  }
  socket.end();
});

server.listen(6001)
console.log("Runnning Wisp Server on 6001");
