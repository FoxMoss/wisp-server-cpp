import { init, routeUpgrade } from "./index"
import http from 'node:http'

init();
const server = http.createServer();
server.on('upgrade', (request, socket, head) => {
  routeUpgrade(request, socket, head);
});
server.listen(6001)
console.log("Runnning Wisp Server on 6001");

