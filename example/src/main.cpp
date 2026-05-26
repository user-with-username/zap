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