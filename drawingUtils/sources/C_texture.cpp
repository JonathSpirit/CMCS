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

bool Texture::updateTextureFromSurface(SDL_Surface* surface)
{
    SDL_Surface* lockedSurface;
    if ( SDL_LockTextureToSurface(this->g_texture.get(), nullptr, &lockedSurface) != 0 )
    {
        return false;
    }

    if ( SDL_BlitSurface(surface, nullptr, lockedSurface, nullptr) != 0 )
    {
        SDL_UnlockTexture(this->g_texture.get());
        return false;
    }
    SDL_UnlockTexture(this->g_texture.get());
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