#ifndef  RENDERING_H
#define RENDERING_H

#include "SDL.h"
#include <vector>

namespace mandlebrot
{
    void histogram_render ( const std::vector< std::vector <unsigned char> > &current_colors,
                            const std::vector<double> &iterations, int nIter,
                            SDL_Renderer *renderer );

    void modulo_render    ( const std::vector< std::vector <unsigned char> > &current_colors,
                            const std::vector<double> &iterations, int nIter, double modulo_blend,
                            SDL_Renderer *renderer );
}

#endif
