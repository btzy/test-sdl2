#include "colorwindow.hpp"

#include <stdexcept>
#include <vector>
#include <SDL.h>


ColorWindow::ColorWindow( Uint32 flags )
{
    if ( SDL_Init( flags ) != 0 )
        throw std::runtime_error("SDL_Init() failed.");

    if ( SDL_CreateWindowAndRenderer( 640, 480, SDL_WINDOW_SHOWN,
                                      &m_window, &m_renderer ) != 0 )
        throw std::runtime_error("SDL_CreateWindowAndRenderer() failed.");
}

ColorWindow::~ColorWindow()
{
    SDL_DestroyWindow( m_window );
    SDL_DestroyRenderer( m_renderer );
    SDL_Quit();
}


void ColorWindow::draw()
{
    // Clear the window with a black background
    SDL_SetRenderDrawColor( m_renderer, 0, 0, 0, 255 );
    SDL_RenderClear( m_renderer );

    // Show the window
    SDL_RenderPresent( m_renderer );


    //std::map<int,int> mp;
    std::vector<SDL_Color> colours{
        {203, 203, 203, 255}, // Gray
        {255, 255,   0, 255}, // Yellow
        {  0, 255, 255, 255}, // Cyan
        {  0, 255,   0, 255}, // Green
        {255,   0, 255, 255}, // Magenta
        {255,   0,   0, 255}, // Red
        {  0,   0, 255, 255}, // Blue
        {  0,   0,   0, 255} // Black
    };


    SDL_Rect colorBar;
    colorBar.x = 0; colorBar.y = 0; colorBar.w = 90; colorBar.h = 480;

    // Render a new color bar every 0.5 seconds
    for ( auto it = colours.begin(); it != colours.end(); ++it, colorBar.x += 90 )
    {
        SDL_SetRenderDrawColor( m_renderer, it->r, it->g, it->b, it->a );
        SDL_RenderFillRect( m_renderer, &colorBar );
        SDL_RenderPresent( m_renderer );
        SDL_Delay( 500 );
    }
}
