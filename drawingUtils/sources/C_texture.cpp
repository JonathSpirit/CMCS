#include "C_texture.hpp"

namespace cmcs::draw
{

Texture::Texture() = default;

Texture::Texture(SDL_Texture* newTexture)
{
    this->set(newTexture);
}

bool Texture::loadBmp(SDL_Renderer* renderer, const char* path)
{
    SDL_Surface* surface = SDL_LoadBMP(path);
    if (surface == nullptr)
    {
        return false;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == nullptr)
    {
        SDL_FreeSurface(surface);
        return false;
    }
    SDL_FreeSurface(surface);
    this->set(texture);
    return true;
}

void Texture::set(SDL_Texture* texture)
{
    this->g_texture.reset(texture, Texture::textureDeleter);
}
SDL_Texture* Texture::get() const
{
    return this->g_texture.get();
}

void Texture::textureDeleter(SDL_Texture* texture)
{
    if (texture != nullptr)
    {
        SDL_DestroyTexture(texture);
    }
}

}//end cmcs::draw