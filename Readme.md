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