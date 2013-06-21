#ifndef TANK_WINDOW_HPP
#define TANK_WINDOW_HPP

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "IWindow.hpp"
#include "../Utility/Vector.hpp"

namespace tank {

class Window : public IWindow
{
public:
    Window(int width, int height, int flags = SDL_HWSURFACE);
    virtual ~Window();

    virtual Vectori const& getSize();
    virtual std::string const& getCaption();

    virtual void setWidth(int width, int height);   //Doesn't work
    virtual void setCaption(std::string&& caption);
    virtual void setIcon(std::string&& path);
private:
    std::string caption_;
    Vectori size_;

    //Is this window instance the current window?
    bool valid_;

    //Unfortunately we can only have one window to stop SDL from shitting itself
    static bool windowExists_;
};

}

#endif /* WINDOW_H */
