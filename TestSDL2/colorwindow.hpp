#include <SDL.h>

class ColorWindow
{
    SDL_Window * m_window;
    SDL_Renderer * m_renderer;
public:
    ColorWindow( Uint32 flags = 0 );
    virtual ~ColorWindow();
    void draw();
};

