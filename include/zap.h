#pragma once

//  zap — a lightweight C++ HTTP framework
//
//    #include "zap.h"
//
//    int main() {
//        app.get("/hello")([](){ return "Hello, world!"; });
//        app.listen(8080);
//    }

#include "app.h"

using zap::get_app;
#define app get_app()
