#include <exception>
#include <iostream>
#include <string>
#include <utility>
#include <SDL.h>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "colorwindow.hpp"

using namespace std::literals::string_literals;


void test_http_request() {
    const char* host = "www.example.com";
    const char* port = "80";
    const char* target = "/";

    // The io_context is required for all I/O
    boost::asio::io_context ioc;

    using boost::asio::ip::tcp;
    namespace http = boost::beast::http;

    // These objects perform our I/O
    tcp::resolver resolver{ioc};
    tcp::socket socket{ioc};

    // Look up the domain name
    auto const results = resolver.resolve(host, port);

    // Make the connection on the IP address we get from a lookup
    boost::asio::connect(socket, results.begin(), results.end());

    // Set up an HTTP GET request message
    http::request<http::string_body> req{http::verb::get, target, 11}; // 11 = HTTP/1.1
    req.set(http::field::host, host);

    // Send the HTTP request to the remote host
    http::write(socket, req);

    // This buffer is used for reading and must be persisted
    boost::beast::flat_buffer buffer;

    // Declare a container to hold the response
    http::response<http::string_body> res;

    // Receive the HTTP response
    http::read(socket, buffer, res);

    // Gracefully close the socket
    boost::system::error_code ec;
    socket.shutdown(tcp::socket::shutdown_both, ec);

    // not_connected happens sometimes
    // so don't bother reporting it.
    if(ec && ec != boost::system::errc::not_connected)
        throw boost::system::system_error{ec};

    // Show the message in a message box
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, ("Received from "s + host + ":" + port + " some data:").c_str(), std::move(res).body().c_str(), nullptr);
}

int main( int argc, char * argv[] ) {
    try {
        ColorWindow cw( SDL_INIT_VIDEO | SDL_INIT_TIMER );
        cw.draw();
    }
    catch (const std::exception& err) {
        std::cerr << "Error while initializing SDL:  " << err.what() << std::endl;
        return 1;
    }

    try {
        test_http_request();
    }
    catch (const std::exception& err) {
        std::cerr << "Error when making HTTP request:  " << err.what() << std::endl;
        return 2;
    }

    return 0;
}
