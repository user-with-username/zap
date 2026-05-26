# zap

A lightweight C++ HTTP framework — single header, zero fuss

## Quick Start

```cpp
#include "zap.h"

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

## Building

Zap uses [Crow](https://github.com/user-with-username/crow) as its build system
If you want to check how it works, do:

```bash
cd example
crow run
```

## License

MIT
