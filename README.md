# wisp-server-cpp
`wisp-server-cpp` is the poorly named NodeJS binding of WispServerCpp it is still written largely in C++ and contains the same pompous attitude of the original with little evidence to back up its claims. If you so choose you can also call this project Wisp C++ Node to distinguish it from the original.

All actual info about the project implementation is stored on [WispServerCpp](https://github.com/FoxMoss/WispServerCpp).

## Usage

Include and install wisp-server-cpp
```js
import { init, routeUpgrade } from "wisp-server-cpp"
```

Call init to setup the internal callbacks before using routeUpgrade anywhere.
```js
init();
```

Then you can use routeUpgrade for any socket upgrade. Express is not necessarily required you can use the [NodeJS http server](https://nodejs.org/api/http.html#class-httpserver), it's the same.
```js
import express from "express"

const app = express();
const server = app.listen(6001);
server.on('upgrade', (request, socket, head) => {
  routeUpgrade(request, socket, head);
});
```
