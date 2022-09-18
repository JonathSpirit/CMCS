#ifndef _CMCS_DRAW_C_TEXTURE_HPP_INCLUDED
#define _CMCS_DRAW_C_TEXTURE_HPP_INCLUDED

#include "drawingUtilsExtern.hpp"
#include <SDL_render.h>
#include <memory>

namespace cmcs::draw
{

class CMCS_DRAWINGUTILS_API Texture
{
public:
    Texture();
    explicit Texture(SDL_Texture* newTexture);

    bool loadBmp(SDL_Renderer* renderer, const char* path);

    void set(SDL_Texture* texture);
    [[nodiscard]] SDL_Texture* get() const;

private:
    static void textureDeleter(SDL_Texture* texture);
    std::shared_ptr<SDL_Texture> g_texture;
};

}//end cmcs::draw

#endif //_CMCS_DRAW_C_TEXTURE_HPP_INCLUDED
