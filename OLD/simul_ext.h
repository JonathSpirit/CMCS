#ifndef SIMUL_EXT_H_INCLUDED
#define SIMUL_EXT_H_INCLUDED

#include <SFML/Graphics.hpp>

void simulext_update(sf::RenderWindow* ren_window, sf::RenderWindow* ren_oscillo);
bool simulext_setcible(unsigned char _pcible , unsigned char _bcible);
bool simulext_delcible(unsigned char _pcible , unsigned char _bcible);
bool simulext_iscible(unsigned char _pcible , unsigned char _bcible);
sf::Color simulext_signalcolour(unsigned char _pcible , unsigned char _bcible);

#endif // SIMUL_EXT_H_INCLUDED
