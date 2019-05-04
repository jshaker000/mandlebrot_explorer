#ifndef CONFIG_H
#define CONFIG_H

#include <vector>

namespace mandlebrot
{
    extern const int order_def;

    extern const double COARSE_ZOOM_FACTOR;
    extern const double FINE_ZOOM_FACTOR;
    
    extern const double COARSE_SCROLL_FACTOR;
    extern const double FINE_SCROLL_FACTOR;

    extern const double colorscheme_def;
    extern const bool   histogram_color_def;
    extern const int    modulo_blending_def;

    extern const int pixelWidth;
    extern const int NUM_THREADS;

    extern const int    nIter_def;
    extern const double x_min_def;
    extern const double x_max_def;
    extern const double y_min_def;
    extern const double y_max_def;

    extern const int BAILOUT_RADIUS;

    //Color palette in RGB
    //Top row is min iterations to escape
    //Bottom row is max iterations to escape
    //will either be rendered with a histogram or modulo depening on view
    //each row is an RGB triple
    extern const int RED;
    extern const int GREEN;
    extern const int BLUE;

    extern const std::vector< std::vector< std::vector < unsigned char> > > color_maps;
}

#endif