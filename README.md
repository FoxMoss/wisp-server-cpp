# wisp-server-cpp
`wisp-server-cpp` is the poorly named NodeJS binding of WispServerCpp it is still written largely in C++ and contains the same pompous attitude of the original with little evidence to back up its claims. If you so choose you can also call this project Wisp C++ Node to distinguish it from the original.

All actual info about the project implementation is stored on [WispServerCpp](https://github.com/FoxMoss/WispServerCpp).

## Usage


> [!IMPORTANT]  
> You may want to use `--force-node-api-uncaught-exceptions-policy=trueto` when running node.


Include and install wisp-server-cpp
```js
import { init, routeRequest, routeUpgrade, shouldRoute } from "wisp-server-cpp"
```

Call init to setup the internal callbacks before using routeUpgrade anywhere.
```js
init();
```
Optionally choose the prefix for shouldRoute, default is `/wisp/`
```js
init("/non-suspicious-path/");
```


Add express and HTTP server boilerplate.
```js
import http from 'node:http'
import express from "express"

const app = express();
const server = http.createServer();
```

Hook in express app routes and Wisp Server routing.
```js
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
```

Then run it;
```js
server.listen(6001)
```
## Credit
- bare-server-node for userspace arch
