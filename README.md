# MultiThreaded Webserver

A concurrent HTTP/1.1 web server built in C++ using Asio, 
spawning a new thread per connection to handle requests in parallel.

## What it does

- Accepts incoming TCP connections on port 8080
- Spawns a dedicated thread per client using `std::thread`
- Handles GET requests for static files from the `/public/` directory
- Serves a JSON API endpoint at `/api/data` that returns the 
  handling thread ID
- Infers MIME types from file extensions (HTML, CSS, JS, JSON, 
  images)
- Returns proper HTTP/1.1 responses with Content-Type and 
  Content-Length headers

## Stack

- C++17
- Asio (standalone, non-Boost) for async TCP networking
- POSIX threads via std::thread

## How to run

```bash
g++ -std=c++17 test.cpp -o server -lpthread
./server
```

Visit `http://localhost:8080` or `http://localhost:8080/api/data`
