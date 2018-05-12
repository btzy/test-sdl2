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

using namespace std::literals::string_literals;

class SDL
{
    SDL_Window * m_window;
    SDL_Renderer * m_renderer;
public:
    SDL( Uint32 flags = 0 );
    virtual ~SDL();
    void draw();
};

SDL::SDL( Uint32 flags )
{
    if ( SDL_Init( flags ) != 0 )
        throw std::runtime_error("SDL_Init() failed.");

    if ( SDL_CreateWindowAndRenderer( 640, 480, SDL_WINDOW_SHOWN,
                                      &m_window, &m_renderer ) != 0 )
        throw std::runtime_error("SDL_CreateWindowAndRenderer() failed.");
}

SDL::~SDL()
{
    SDL_DestroyWindow( m_window );
    SDL_DestroyRenderer( m_renderer );
    SDL_Quit();
}

void SDL::draw()
{
    // Clear the window with a black background
    SDL_SetRenderDrawColor( m_renderer, 0, 0, 0, 255 );
    SDL_RenderClear( m_renderer );

    // Show the window
    SDL_RenderPresent( m_renderer );

    int rgb[] = { 203, 203, 203, // Gray
                  254, 254,  31, // Yellow
                    0, 255, 255, // Cyan
                    0, 254,  30, // Green
                  255,  16, 253, // Magenta
                  253,   3,   2, // Red
                   18,  14, 252, // Blue
                    0,   0,   0  // Black
                };

    SDL_Rect colorBar;
    colorBar.x = 0; colorBar.y = 0; colorBar.w = 90; colorBar.h = 480;

    // Render a new color bar every 0.5 seconds
    for ( int i = 0; i != sizeof rgb / sizeof *rgb; i += 3, colorBar.x += 90 )
    {
        SDL_SetRenderDrawColor( m_renderer, rgb[i], rgb[i + 1], rgb[i + 2], 255 );
        SDL_RenderFillRect( m_renderer, &colorBar );
        SDL_RenderPresent( m_renderer );
        SDL_Delay( 500 );
    }
}

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
        SDL sdl( SDL_INIT_VIDEO | SDL_INIT_TIMER );
        sdl.draw();
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
