#include "zap.h"

int main() {
    // Simple route
    app.get("/")([]() {
        return zap::json{{"message", "Hello, World!"}};
    });
    
    // Route with parameter
    app.get("/hello/:name")([](std::string name) {
        return zap::json{{"message", "Hello, " + name + "!"}};
    });
    
    // Start server
    app.listen(8080);
    
    return 0;
}