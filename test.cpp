#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <asio.hpp>

using asio::ip::tcp;
using namespace std;

// Determine content type by file extension
string get_content_type(const string& path) {
    static unordered_map<string, string> mime_types = {
        {".html", "text/html"},
        {".css", "text/css"},
        {".js", "application/javascript"},
        {".png", "image/png"},
        {".jpg", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".txt", "text/plain"},
        {".json", "application/json"}
    };
    size_t dot_pos = path.rfind('.');
    if (dot_pos != string::npos) {
        string ext = path.substr(dot_pos);
        if (mime_types.count(ext)) return mime_types[ext];
    }
    return "application/octet-stream";
}

// Read full content of file into a string
string read_file(const string& filepath) {
    ifstream file(filepath, ios::binary);
    if (!file) return "";
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Client handler
void handle_client(tcp::socket socket) {
    try {
        cout << "Client connected on thread: " << this_thread::get_id() << "\n";

        asio::streambuf request_buf;
        asio::read_until(socket, request_buf, "\r\n");

        istream request_stream(&request_buf);
        string method, path, http_version;
        request_stream >> method >> path >> http_version;

        string response;
        if (method == "GET") {
            if (path.find("/public/") == 0) {
                string file_path = "public" + path.substr(7); 
                string file_content = read_file(file_path);

                if (!file_content.empty()) {
                    string content_type = get_content_type(file_path);
                    response = "HTTP/1.1 200 OK\r\nContent-Type: " + content_type +
                               "\r\nContent-Length: " + to_string(file_content.size()) +
                               "\r\n\r\n" + file_content;
                } else {
                    string not_found = "404 Not Found";
                    response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: " +
                               to_string(not_found.size()) + "\r\n\r\n" + not_found;
                }
            }
            else if (path == "/api/data") {
                std::ostringstream thread_id_stream;
                thread_id_stream << std::this_thread::get_id();
    
                std::string json = "{ \"message\": \"Hello from the server\", \"thread\": \"" + thread_id_stream.str() + "\" }";
    
                std::ostringstream resp;
                resp << "HTTP/1.1 200 OK\r\n"
                     << "Content-Type: application/json\r\n"
                     << "Content-Length: " << json.size() << "\r\n"
                     << "Connection: close\r\n\r\n"
                     << json;
    
                response = resp.str();
            }
             else {
                string msg = "Welcome! Try /public/index.html";
                response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " +
                           to_string(msg.size()) + "\r\n\r\n" + msg;
            }
        }
        

        asio::write(socket, asio::buffer(response));
        socket.shutdown(tcp::socket::shutdown_send);
    } catch (exception& e) {
        cerr << "Error: " << e.what() << "\n";
    }
}

// Main server
int main() {
    try {
        asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));
        cout << "Server running at http://localhost:8080\n";

        while (true) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);
            thread(handle_client, move(socket)).detach();
        }
    } catch (exception& e) {
        cerr << "Server failed: " << e.what() << "\n";
    }

    return 0;
}
