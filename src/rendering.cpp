#include <algorithm>
#include <cmath>
#include <vector>

#include <iostream>

#include "config.h"
#include "rendering.h"

//sort all pixels by iteration, so we can group them into buckets
//similar iterations will be similar color and we will shade the difference
//this way, in theory each 1/NUM_BUCKETS group will have a similar color
//In theory, each Bucket should then have TOTAL_PIXELS/NUM_BUCKETS in it
void mandlebrot::histogram_render (const std::vector< std::vector <unsigned char> > &current_colors,
                                    const std::vector<double> &iterations, int nIter,
                                    SDL_Renderer *renderer)
{

    std::vector<double> temp(iterations);
    std::sort(temp.begin(), temp.end());
    const auto nIterIt = std::find(temp.begin(), temp.end(), nIter);
    const size_t nIterIdx = nIterIt - temp.begin() - (nIterIt == temp.end() ? 1 : 0);

    std::vector<double>buckets(current_colors.size());
    for (size_t i = 0; i < current_colors.size(); i++)
    {
        size_t calc_idx = (nIterIdx*(i+1))/(current_colors.size()) - 1;
        calc_idx = calc_idx + 1 == 0 ? 0 : calc_idx;
        buckets[i] = temp[calc_idx];
    }

    //ensure no 2 buckets have the same iteration count
    //this is very rudimentary and should be improved for smoother coloring
    for (size_t i = 0; i < current_colors.size() - 1; i++)
    {
        if (buckets[i] == buckets[i + 1])
        {
            if (i == 0)
            {
                buckets[i] *= 0.2;
            }
            else
            {
                buckets[i] = buckets[i - 1] + 1 + 0.2 * (buckets[i] - buckets[i - 1]);
            }
        }
    }

    #pragma omp parallel for collapse(2)
    for (int i = 0; i < mandlebrot::pixelWidth; i++)
    {
        for (int j = 0; j < mandlebrot::pixelWidth; j++)
        {
            char red, green, blue;
            if (iterations[i * mandlebrot::pixelWidth + j] == nIter)
            {
                red   = 0x00;
                green = 0x00;
                blue  = 0x00;
            }
            else
            {

                //find which bucket each pixel belongs to
                size_t bucket_index, bucket2_index;
                for (bucket_index = 0; bucket_index < buckets.size() - 1; bucket_index++)
                {
                    if (iterations[i * mandlebrot::pixelWidth + j] <= buckets[bucket_index])
                        break;
                }

                bucket2_index = bucket_index == 0 ? buckets.size() -1 : bucket_index - 1;

                double blend = static_cast<double>(iterations[i * mandlebrot::pixelWidth + j] - buckets[bucket_index - 1])
                        / static_cast<double>(buckets[bucket_index] - buckets[bucket_index - 1]);

                red   = static_cast<char>((1 - blend) * current_colors[bucket2_index][mandlebrot::RED]
                        + blend * current_colors[bucket_index][mandlebrot::RED]);
                green = static_cast<char>((1 - blend) * current_colors[bucket2_index][mandlebrot::GREEN]
                        + blend * current_colors[bucket_index][mandlebrot::GREEN]);
                blue  = static_cast<char>((1 - blend) * current_colors[bucket2_index][mandlebrot::BLUE]
                        + blend * current_colors[bucket_index][mandlebrot::BLUE]);
            }
            #pragma omp critical
            {
                SDL_SetRenderDrawColor(renderer, red, green, blue, 0xFF);
                SDL_RenderDrawPoint   (renderer, j, i);
            }
        }
    }
}

//color each pixel by the modulo of the iterations it took
//this has the advantage of being zoom invariant, but can get messy
void mandlebrot::modulo_render (const std::vector< std::vector <unsigned char> > &current_colors,
                                 const std::vector<double> &iterations, int nIter, double modulo_blend,
                                 SDL_Renderer *renderer)
{
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < mandlebrot::pixelWidth; i++)
    {
        for (int j = 0; j < mandlebrot::pixelWidth; j++)
        {
            char red, green, blue;
            if (iterations[i * mandlebrot::pixelWidth + j] == nIter)
            {
                red   = 0x00;
                green = 0x00;
                blue  = 0x00;
            }
            else
            {

                int bucket_index  = static_cast<int>(std::floor(iterations[i * mandlebrot::pixelWidth + j]
                                    / modulo_blend)) % current_colors.size();

                int bucket2_index = (bucket_index + 1) % current_colors.size();

                double tmp = iterations[i * mandlebrot::pixelWidth + j];
                double tmp2;

                for (tmp2 = 0; tmp2 < tmp - modulo_blend; tmp2 += modulo_blend)
                {
                    ;
                }

                double blend = static_cast<double>(iterations[i * mandlebrot::pixelWidth + j] - tmp2) / modulo_blend;

                red   = static_cast<char>((1 - blend) * current_colors[bucket_index][mandlebrot::RED]
                      + blend * current_colors[bucket2_index][mandlebrot::RED]);

                green = static_cast<char>((1 - blend) * current_colors[bucket_index][mandlebrot::GREEN]
                      + blend * current_colors[bucket2_index][mandlebrot::GREEN]);

                blue  = static_cast<char>((1 - blend) * current_colors[bucket_index][mandlebrot::BLUE]
                      + blend * current_colors[bucket2_index][mandlebrot::BLUE]);
            }
            #pragma omp critical
            {
                SDL_SetRenderDrawColor(renderer, red, green, blue, 0xFF);
                SDL_RenderDrawPoint   (renderer, j, i);
            }
        }
    }
}
