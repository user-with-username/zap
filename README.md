# zap

A lightweight C++ HTTP framework — single header, zero fuss.

## Quick Start

```cpp
#include "zap/zap.h"

int main() {
    app.get("/")([]() {
        return zap::json{{"message", "Hello, World!"}};
    });

    app.get("/hello/:name")([](std::string name) {
        return zap::json{{"message", "Hello, " + name + "!"}};
    });

    app.listen(8080);
}
```

## Features

- **Single header** — `#include "zap/zap.h"`, you're done
- **Path parameters** — `/users/:id` just works
- **Route groups** — prefix and group middleware
- **Middleware** — global or per-route, with `next()` chaining
- **Built-in CORS** — `app.enable_cors()`
- **JSON** — powered by nlohmann/json
- **Thread pool** — auto-sized to your hardware

## Building

Zap uses [Crow](https://github.com/user-with-username/crow) as its build system

```bash
crow build
```

## License

MIT
