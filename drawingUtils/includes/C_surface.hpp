#ifndef _CMCS_DRAW_C_SURFACE_HPP_INCLUDED
#define _CMCS_DRAW_C_SURFACE_HPP_INCLUDED

#include "drawingUtilsExtern.hpp"
#include <SDL_render.h>
#include <memory>

namespace cmcs::draw
{

class CMCS_DRAWINGUTILS_API Surface
{
public:
    Surface();
    explicit Surface(SDL_Surface* newSurface);

    bool setPixel(int x, int y, uint32_t pixel);
    bool setPixel(int x, int y, SDL_Color color);

    bool fillRect(const SDL_Rect* rect, SDL_Color color);

    void set(SDL_Surface* surface);
    [[nodiscard]] SDL_Surface* get() const;

private:
    static void surfaceDeleter(SDL_Surface* surface);
    std::shared_ptr<SDL_Surface> g_surface;
};

}//end cmcs::draw

#endif //_CMCS_DRAW_C_SURFACE_HPP_INCLUDED
