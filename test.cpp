#include <iostream>
#include <thread>
#include <asio.hpp>

using namespace std;
using asio::ip::tcp;

void handle_client(tcp::socket socket) { 
try {
    cout << "Client connected on thread: " << this_thread::get_id() <<endl;

    string response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 45\r\n"
        "\r\n"
        "Hello, world!This is running on thread " + ([]() -> string {
            ostringstream oss;
            oss << this_thread::get_id();
            return oss.str();
        })() + "\n";
    
    asio::write(socket , asio::buffer(response));
    socket.shutdown(tcp::socket::shutdown_send);

}
catch (exception& e) {
    cerr << "Error handling client: " << e.what() << "\n";
};
}

int main() {
    try {
        asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));
        cout << "Server is running on port 8080\n";

        while(true){

            tcp::socket socket(io_context);
            acceptor.accept(socket);
            thread(handle_client, move(socket)).detach();

        }
    }
    catch (exception& e) {
        cerr << "Server error: " << e.what() << "\n";
    }

    return 0;
}
// This code is a simple threaded HTTP server using ASIO. It accepts incoming connections and handles each client in a separate thread. The server responds with a "Hello, world!" message and the thread ID of the handling thread.