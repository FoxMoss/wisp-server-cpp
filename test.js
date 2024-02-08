import { init, routeUpgrade } from "./index.js"
import express from "express"

init();
console.log("Runnning Wisp Server on 6001");
const app = express();
const server = app.listen(4000);
server.on('upgrade', (request, socket, head) => {
  routeUpgrade(request, socket, head);
});
