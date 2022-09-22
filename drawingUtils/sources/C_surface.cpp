#include "C_surface.hpp"

namespace cmcs::draw
{

Surface::Surface() = default;

Surface::Surface(SDL_Surface* newSurface)
{
    this->set(newSurface);
}

bool Surface::setPixel(int x, int y, uint32_t pixel)
{
    SDL_Surface* surface = this->g_surface.get();
    if (surface == nullptr)
    {
        return false;
    }
    if (x<0 || y<0 || x>=surface->w || y>=surface->h)
    {
        return false;
    }

    auto* const target_pixel = reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(surface->pixels)
                                                                + y * surface->pitch
                                                                + x * surface->format->BytesPerPixel);
    *target_pixel = pixel;
    return true;
}
bool Surface::setPixel(int x, int y, SDL_Color color)
{
    SDL_Surface* surface = this->g_surface.get();
    if (surface == nullptr)
    {
        return false;
    }
    if (x<0 || y<0 || x>=surface->w || y>=surface->h)
    {
        return false;
    }

    auto* const target_pixel = reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(surface->pixels)
                                                           + y * surface->pitch
                                                           + x * surface->format->BytesPerPixel);
    *target_pixel = SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a);
    return true;
}

bool Surface::fillRect(const SDL_Rect* rect, SDL_Color color)
{
    SDL_Surface* surface = this->g_surface.get();
    if (surface == nullptr)
    {
        return false;
    }
    return SDL_FillRect(surface, rect, SDL_MapRGBA(surface->format,color.r, color.g, color.b, color.a)) == 0;
}

void Surface::set(SDL_Surface* surface)
{
    this->g_surface.reset(surface, Surface::surfaceDeleter);
}
SDL_Surface* Surface::get() const
{
    return this->g_surface.get();
}

void Surface::surfaceDeleter(SDL_Surface* surface)
{
    if (surface != nullptr)
    {
        SDL_FreeSurface(surface);
    }
}

}//end cmcs::draw