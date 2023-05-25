# Basic Server C++
This is a very basic C++ server library.

## Goals
The goals of this project are:
  * Getting practical experience working with sockets/networking in C++
  * Getting practical experience working in a multithreaded environment in C++
  * Comparing networking via threads vs via coroutines

## Server features
Features have to include:
  * Ability to listen, handle and respond to HTTP requests
  * Ability to serve files(images/videos/GIFs)
  * Server should be able to take a configuration parameter that enables us to switch between threads vs coroutines for the asynchronous behavior 

## Building
    # cd to the source folder
    mkdir build
    cmake ..
    make
    sudo make install


The generated folder called 'include' should be copied to your project and included via target_include_directories.

## Integration

    cmake_minimum_required(VERSION 3.12)
    
    project(MyProject)

    # Add the executable
    add_executable(test main.cpp)

	# Find the library
    find_library(CPPSERV_LIB cppserv_lib)
    
    # Link the shared library
    cmake_minimum_required(VERSION 3.12)
    project(MyProject)

    # Add the executable
    add_executable(test main.cpp)
    
    find_library(CPPSERV_LIB cppserv_lib)
    
    # Link the shared library
    target_link_libraries(test PRIVATE ${CPPSERV_LIB})
    target_include_directories(test PRIVATE include)
## Usage

    #include "exports.hpp"
    
    class  HelloWorld : public  BaseRouteHandler {
	    public:
	    void  Get(HttpRequest&  req) override {
		    HttpResponse  resp;
		    resp.setHtmlData("<h3>Helloso World!</h3>");
		    
		    sendResponse(resp);
	    }
    };

    int  main() {
	    Router  router;
	    router.addRoute("/hello", new  HelloWorld());
	    
	    Config  config; 
	    config.server_runner_type = ServerRunnerTypes::SELECT;
	    config.port = 3000;
	    config.router  =  std::make_shared<Router>(router);
	    
	    Server  server(config);
	    server.start();
    }

    
   
    
