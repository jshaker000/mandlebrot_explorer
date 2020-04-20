#include <algorithm>
#include <chrono>
#include <cmath>
#include <complex>
#include <iostream>
#include <iomanip>
#include <thread>
#include <vector>

#include "SDL.h"

#include "config.h"
#include "rendering.h"

static const std::string program_name = "Mandlebrot Explorer";

// Take in as conditions what part of the fractal to calculate,
// then dump it into its alotted segment of &iterations
void calculate_iterations(const int i_start,    const int i_end,
                          const int j_start,    const int j_end,
                          const int pixelWidth, const int order, const size_t nIter,
                          const double x_min,   const double y_max, const double x_inc, const double y_inc,
                          std::vector<double> &iterations)
{
    for (int i = i_start; i < i_end; i++)
    {
        for(int j = j_start; j < j_end; j++)
        {
            const std::complex<double> currentPoint(x_min+(j-j_start)*x_inc, y_max-(i-i_start)*y_inc);
            size_t k = 0;
            // cardiod improvement for 2nd order
            if (order == 2)
            {
                const double q = (currentPoint.real() - 1.0/4.0)* ( currentPoint.real() - 1.0/4.0)
                                + currentPoint.imag() * currentPoint.imag();
                if (4 * q*(q+(currentPoint.real() - 1.0/4.0)) <= currentPoint.imag() * currentPoint.imag())
                {
                    k = nIter;
                }
            }

            std::complex<double> cp_iterate(currentPoint);
            while(k < nIter && abs(cp_iterate) < mandlebrot::BAILOUT_RADIUS)
            {
                cp_iterate = std::pow(cp_iterate, order) + currentPoint;
                k += 1;
            }

            if (k == nIter)
            {
                iterations[i * pixelWidth + j] = k;
            }
            // compute fractional iterations
            else
            {
                const double log_zn = log(abs(cp_iterate));
                const double nu     = log(log_zn/log(mandlebrot::BAILOUT_RADIUS))/log(order);
                iterations[i * pixelWidth + j] = k + 1 - nu;
            }
        }
    }
}

int main()
{

    bool recalculate = true;
    bool redraw      = true;
    bool quit        = false;

    double modulo_blending = mandlebrot::modulo_blending_def;
    if (modulo_blending < 1)
    {
        modulo_blending = 1;
    }

    bool histogram_color = mandlebrot::histogram_color_def;

    int order      = mandlebrot::order_def;
    size_t nIter   = static_cast<size_t>(mandlebrot::nIter_def);

    double x_min   =  mandlebrot::x_min_def;
    double x_max   =  mandlebrot::x_max_def;
    double y_min   =  mandlebrot::y_min_def;
    double y_max   =  mandlebrot::y_max_def;
    double x_width =  x_max - x_min;
    double y_width =  y_max - y_min;

    std::vector<double> iterations(mandlebrot::pixelWidth * mandlebrot::pixelWidth);
    std::vector<std::vector <unsigned char>> current_colors;
    {
        size_t current_map = static_cast<size_t>(mandlebrot::colorscheme_def);
        if (current_map >= mandlebrot::color_maps.size())
        {
            current_map = 0;
        }
        current_colors = mandlebrot::color_maps[current_map];
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL_Init(SDL_INIT_VIDEO)\n";
        return -1;
    }

    SDL_Window   *window;
    SDL_Renderer *renderer;
    SDL_Event    e;

    SDL_CreateWindowAndRenderer(mandlebrot::pixelWidth,
                                mandlebrot::pixelWidth,
                                SDL_WINDOW_OPENGL,
                                &window,
                                &renderer);

    if (window == nullptr || renderer == nullptr)
    {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_SetWindowTitle(window, program_name.c_str());

    SDL_SetWindowPosition(window, 10, 10);

    while (!quit)
    {
        if (recalculate)
        {

            const double y_inc = static_cast <double>(y_width) / mandlebrot::pixelWidth;
            const double x_inc = static_cast <double>(x_width) / mandlebrot::pixelWidth;

            // calculate iterations for the new mandlebrot
            // send off threads to calculate iterations for each slice of the fractal and then return back
            std::vector<std::thread> t;
            t.reserve(mandlebrot::NUM_THREADS);
            for (int i = 0; i < mandlebrot::NUM_THREADS; i++)
            {
                t.push_back(std::thread(calculate_iterations,
                                        i * mandlebrot::pixelWidth / mandlebrot::NUM_THREADS,       // i start
                                        (i + 1) * mandlebrot::pixelWidth / mandlebrot::NUM_THREADS, // i end
                                        0, // j start
                                        mandlebrot::pixelWidth, // j end
                                        mandlebrot::pixelWidth, // num_pixels
                                        order, // order
                                        nIter, //iter
                                        x_min, // xmin
                                        y_max - (i*y_width)/mandlebrot::NUM_THREADS, // y max
                                        x_inc, // x increment
                                        y_inc, // y increment
                                        std::ref(iterations)) // iterations vector
                );
            }

            SDL_SetWindowTitle(window, (std::string("~ ") + program_name + std::string( ",   calculating ~") ).c_str());
            SDL_PumpEvents();

            std::for_each(t.begin(), t.end(),
            [](auto &th)
            { 
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                th.join();
            });

            recalculate = false;
            redraw = true;
        }
        if (redraw)
        {
            SDL_RenderClear(renderer);

            SDL_SetWindowTitle(window, (std::string("~ ") + program_name + std::string( ",    rendering   ~")).c_str());
            SDL_PumpEvents();

            if (histogram_color)
            {
                mandlebrot::histogram_render(current_colors, iterations, nIter, renderer);
            }
            // else use modulo color
            else
            {
                mandlebrot::modulo_render(current_colors, iterations, nIter, modulo_blending, renderer);
            }
            SDL_RenderPresent(renderer);
            redraw = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        SDL_SetWindowTitle(window, (program_name + std::string(",    idle        ~")).c_str());
        SDL_PumpEvents();

        while (SDL_PollEvent(&e))
        {
            // If user closes the window
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }

            else if(e.type == SDL_MOUSEBUTTONDOWN)
            {
                // recenter window over mouse
                if (e.button.button == SDL_BUTTON_LEFT)
                {
                    x_min += x_width * (static_cast<double>(e.button.x)/ mandlebrot::pixelWidth - 1.0 / 2.0);
                    x_max += x_width * (static_cast<double>(e.button.x)/ mandlebrot::pixelWidth - 1.0 / 2.0);

                    y_min -= y_width * (static_cast<double>(e.button.y)/ mandlebrot::pixelWidth - 1.0 / 2.0);
                    y_max -= y_width * (static_cast<double>(e.button.y)/ mandlebrot::pixelWidth - 1.0 / 2.0);

                    x_width  = x_max - x_min;
                    y_width  = y_max - y_min;

                    recalculate = true;

                }
            }

            // If user presses any key
            else if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                    // quit
                    case SDLK_q:
                        quit = true;
                        break;

                    // print current state
                    case SDLK_p:
                        std::cout.precision(20);
                        std::cout << "----------------------------------------------------------------------\n"
                                  << "x_min = "   << std::setw(25) << x_min << ", x_max = " << std::setw(25) << x_max << "\n"
                                  << "y_min = "   << std::setw(25) << y_min << ", y_max = " << std::setw(25) << y_max << "\n"
                                  << "x_width = " << std::setw(25 - 2) << x_width << "\n"
                                  << "y_width = " << std::setw(25 - 2) << y_width << "\n";

                        std::cout << "current color_scheme:\n"
                                  << "{\n"
                                  << std::hex << std::setfill ('0');

                        std::for_each(current_colors.begin(), current_colors.end(),
                        [] (const auto &cc)
                        {
                            std::cout << "    "
                                      << "{0x"  << std::setw(2) << static_cast<int>(cc[0])
                                      << ", 0x" << std::setw(2) << static_cast<int>(cc[1])
                                      << ", 0x" << std::setw(2) << static_cast<int>(cc[2]) << "}," << "\n";
                        });
                        std::cout << "};" << "\n"
                                  << std::dec << std::setfill(' ');

                        std::cout << "histogram_coloring? = " << histogram_color << "\n"
                                  << "number iterations   = " << nIter << "\n"
                                  << "modulo_blending     = " << modulo_blending << std::endl;
                        break;

                    // print controls
                    case SDLK_t:
                        std::cout << "----------------------------------------------------------------------\n"
                                  << "Controls:\n"
                                  << "Left Click: recenter the view on the mouse location\n"
                                  << "Arrow Keys: Coarse Pan\n"
                                  << "hjkl      : Fine Pan\n"
                                  << "+ / -     : Coarse Zoom\n"
                                  << "y / u     : Fine Zoom\n"
                                  << "[ / ]     : increase/decrease number of iterations.\n"
                                  << "            low iterations are easier to render but lack sharpness\n"
                                  << "m/n       : modulo/histogram coloring\n"
                                  << "i/o       : toggle the amount of modulo blending\n"
                                  << "Nums 1-4  : toggle between precoded color maps in src/config.cpp\n"
                                  << "r         : reset to default view\n"
                                  << "p         : print current state\n"
                                  << "q         : quit\n"
                                  << "t         : pull up this menu" << std::endl;
                        break;

                    // reset
                    case SDLK_r:
                        x_max = mandlebrot::x_max_def;
                        x_min = mandlebrot::x_min_def;

                        y_max = mandlebrot::y_max_def;
                        y_min = mandlebrot::y_min_def;

                        x_width  = x_max - x_min;
                        y_width  = y_max - y_min;
                        nIter    = mandlebrot::nIter_def;
                        modulo_blending = mandlebrot::modulo_blending_def;

                        recalculate = true;
                        break;

                    // modulo coloring
                    case SDLK_m:
                        if (histogram_color)
                        {
                            histogram_color = 0;
                            redraw = true;
                        }
                        break;

                    // histogram coloring
                    case SDLK_n:
                        if (!histogram_color)
                        {
                            histogram_color = 1;
                            redraw = true;
                        }
                        break;

                    // change color profile
                    case SDLK_1:
                        if(mandlebrot::color_maps.size() >= 1)
                        {
                            current_colors = mandlebrot::color_maps[1 - 1];
                            redraw = true;
                        }
                        break;

                    case SDLK_2:
                        if(mandlebrot::color_maps.size() >= 2)
                        {
                            current_colors = mandlebrot::color_maps[2 - 1];
                            redraw = true;
                        }
                        break;

                    case SDLK_3:
                        if(mandlebrot::color_maps.size() >= 3)
                        {
                            current_colors = mandlebrot::color_maps[3 - 1];
                            redraw = true;
                        }
                        break;

                    case SDLK_4:
                        if(mandlebrot::color_maps.size() >= 4)
                        {
                            current_colors = mandlebrot::color_maps[4 - 1];
                            redraw = true;
                        }
                        break;

                    // change # of iterations
                    // more
                    case SDLK_LEFTBRACKET:
                        nIter  *= (1.0+(mandlebrot::COARSE_ZOOM_FACTOR - 1.0));
                        recalculate = true;
                        break;
                    // less
                    case SDLK_RIGHTBRACKET:
                        nIter  *= (1.0/(1.0 +(mandlebrot::COARSE_ZOOM_FACTOR - 1.0)));
                        if (nIter <= current_colors.size() + 5)
                            nIter   = current_colors.size() + 5;
                        recalculate = true;
                        break;

                    // change amount of modulo blending
                    // more
                    case SDLK_i:
                        modulo_blending *= mandlebrot::modulo_blending_scroll;
                        redraw = true;
                        break;
                    // less
                    case SDLK_o:
                        modulo_blending /= mandlebrot::modulo_blending_scroll;
                        if (modulo_blending < mandlebrot::modulo_blending_min)
                            modulo_blending = mandlebrot::modulo_blending_min;
                        redraw = true;
                        break;

                    // zoom in
                    // coarse
                    case SDLK_PLUS: case SDLK_EQUALS:
                        x_max -= 0.5 * (mandlebrot::COARSE_ZOOM_FACTOR - 1.0) * (x_width);
                        x_min += 0.5 * (mandlebrot::COARSE_ZOOM_FACTOR - 1.0) * (x_width);

                        y_max -= 0.5 * (mandlebrot::COARSE_ZOOM_FACTOR - 1.0) * (y_width);
                        y_min += 0.5 * (mandlebrot::COARSE_ZOOM_FACTOR - 1.0) * (y_width);

                        x_width = x_max - x_min;
                        y_width = y_max - y_min;
                        nIter   *= (1.0 + (mandlebrot::COARSE_ZOOM_FACTOR - 1.0) * (mandlebrot::COARSE_ZOOM_FACTOR - 1.0));
                        recalculate   = true;
                        break;

                    // fine
                    case SDLK_y:
                        x_max -= 0.5 * (mandlebrot::FINE_ZOOM_FACTOR - 1.0) * (x_width);
                        x_min += 0.5 * (mandlebrot::FINE_ZOOM_FACTOR - 1.0) * (x_width);

                        y_max -= 0.5 * (mandlebrot::FINE_ZOOM_FACTOR - 1.0) * (y_width);
                        y_min += 0.5 * (mandlebrot::FINE_ZOOM_FACTOR - 1.0) * (y_width);

                        x_width = x_max - x_min;
                        y_width = y_max - y_min;
                        nIter   *= (1.0 + (mandlebrot::FINE_ZOOM_FACTOR - 1.0) * (mandlebrot::FINE_ZOOM_FACTOR - 1.0));
                        recalculate   = true;
                        break;

                    // zoom out
                    // coarse
                    case SDLK_MINUS: case SDLK_UNDERSCORE:
                        x_max += 0.5 * (mandlebrot::COARSE_ZOOM_FACTOR - 1.0) * (x_width);
                        x_min -= 0.5 * (mandlebrot::COARSE_ZOOM_FACTOR - 1.0) * (x_width);

                        y_max += 0.5 * (mandlebrot::COARSE_ZOOM_FACTOR - 1.0) * (y_width);
                        y_min -= 0.5 * (mandlebrot::COARSE_ZOOM_FACTOR - 1.0) * (y_width);

                        x_width = x_max - x_min;
                        y_width = y_max - y_min;
                        nIter   *= (1.0 / ( 1.0 + (mandlebrot::COARSE_ZOOM_FACTOR - 1.0) * (mandlebrot::COARSE_ZOOM_FACTOR - 1.0)));
                        if ( nIter <= current_colors.size() + 5)
                            nIter   = current_colors.size() + 5;
                        recalculate = true;
                        break;

                    // fine
                    case SDLK_u:
                        x_max += 0.5*(mandlebrot::FINE_ZOOM_FACTOR - 1.0) * (x_width);
                        x_min -= 0.5*(mandlebrot::FINE_ZOOM_FACTOR - 1.0) * (x_width);

                        y_max += 0.5*(mandlebrot::FINE_ZOOM_FACTOR - 1.0) * (y_width);
                        y_min -= 0.5*(mandlebrot::FINE_ZOOM_FACTOR - 1.0) * (y_width);

                        x_width = x_max - x_min;
                        y_width = y_max - y_min;
                        nIter   *= ( 1.0 / ( 1.0 + (mandlebrot::FINE_ZOOM_FACTOR - 1.0) * (mandlebrot::FINE_ZOOM_FACTOR - 1.0) ) );
                        if ( nIter <= current_colors.size() + 5)
                            nIter   = current_colors.size() + 5;
                        recalculate = true;
                        break;

                    // move left
                    // coarse
                    case SDLK_LEFT:
                        x_max  -= (mandlebrot::COARSE_SCROLL_FACTOR - 1.0) * x_width;
                        x_min  -= (mandlebrot::COARSE_SCROLL_FACTOR - 1.0) * x_width;
                        x_width = x_max - x_min;
                        recalculate = true;
                        break;
                    // fine
                    case SDLK_h:
                        x_max  -= (mandlebrot::FINE_SCROLL_FACTOR - 1.0) * x_width;
                        x_min  -= (mandlebrot::FINE_SCROLL_FACTOR - 1.0) * x_width;
                        x_width = x_max - x_min;
                        recalculate = true;
                        break;

                    // move right
                    // coarse
                    case SDLK_RIGHT:
                        x_max  += (mandlebrot::COARSE_SCROLL_FACTOR - 1.0) * x_width;
                        x_min  += (mandlebrot::COARSE_SCROLL_FACTOR - 1.0) * x_width;
                        x_width = x_max - x_min;
                        recalculate = true;
                        break;
                    // fine
                    case SDLK_l:
                        x_max  += (mandlebrot::FINE_SCROLL_FACTOR - 1.0) * x_width;
                        x_min  += (mandlebrot::FINE_SCROLL_FACTOR - 1.0) * x_width;
                        x_width = x_max - x_min;
                        recalculate = true;
                        break;

                    // move up
                    // coarse
                    case SDLK_UP:
                        y_max  += (mandlebrot::COARSE_SCROLL_FACTOR - 1.0) * y_width;
                        y_min  += (mandlebrot::COARSE_SCROLL_FACTOR - 1.0) * y_width;
                        y_width = y_max - y_min;
                        recalculate = true;
                        break;
                    // fine
                    case SDLK_k:
                        y_max  += (mandlebrot::FINE_SCROLL_FACTOR - 1.0) * y_width;
                        y_min  += (mandlebrot::FINE_SCROLL_FACTOR - 1.0) * y_width;
                        y_width = y_max - y_min;
                        recalculate = true;
                        break;

                    // move down
                    // coarse
                    case SDLK_DOWN:
                        y_max  -= (mandlebrot::COARSE_SCROLL_FACTOR - 1.0) * y_width;
                        y_min  -= (mandlebrot::COARSE_SCROLL_FACTOR - 1.0) * y_width;
                        y_width = y_max - y_min;
                        recalculate = true;
                        break;
                    // fine
                    case SDLK_j:
                        y_max  -= (mandlebrot::FINE_SCROLL_FACTOR - 1.0) * y_width;
                        y_min  -= (mandlebrot::FINE_SCROLL_FACTOR - 1.0) * y_width;
                        y_width = y_max - y_min;
                        recalculate = true;
                        break;
                    default:
                        break;
                }
            }
        }
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
