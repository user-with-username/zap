#pragma once

//  zap — a lightweight C++ HTTP framework
//
//  Include this single header in your project.
//  Everything lives in the `zap` namespace; the `app` macro gives you
//  convenient access to the global App singleton.
//
//  Quick start:
//
//    #include "zap/zap.h"
//
//    int main() {
//        app.get("/hello")([](){ return "Hello, world!"; });
//        app.listen(8080);
//    }

#include "app.h"

using zap::get_app;
#define app get_app()
