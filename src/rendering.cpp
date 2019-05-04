#include <algorithm>
#include <cmath>
#include <vector>

#include "config.h"
#include "rendering.h"

//sort all pixels by iteration, so we can group them into buckets
//similar iterations will be similar color and we will shade the difference
//this way, in theory each 1/NUM_BUCKETS group will have a similar color
//In theory, each Bucket should then have TOTAL_PIXELS/NUM_BUCKETS in it
void mandlebrot::histogram_render ( const std::vector<double> &iterations, int nIter, SDL_Renderer *renderer )
{
    std::vector<double> temp(iterations);
    std::sort( temp.begin(), temp.end() );
    double buckets[ mandlebrot::colors.size() ];

    for ( int i = 0; i < mandlebrot::colors.size(); i++ )

    {
        buckets[ i ] = temp[ ( ( temp.size() * ( i + 1 ) ) / mandlebrot::colors.size() ) - 1 ];
    }

    //ensure no 2 buckets have the same iteration count
    //this is very rudimentary and should be improved for smoother coloring
    for ( int i = 0; i < mandlebrot::colors.size() - 1; i++ )
    {
        if ( buckets[ i ] == buckets[ i + 1 ] )
        {
            if ( i == 0 )
            {
                buckets[ i ] *= 0.2;
            }
            else
            {
                buckets[ i ] = buckets[ i - 1 ] + 1 + 0.2 * ( buckets[ i ] - buckets[ i - 1 ] );
            }
        }
    }

    for ( int i = 0; i < mandlebrot::pixelWidth; i++ )
    {
        for ( int j = 0; j < mandlebrot::pixelWidth; j++ )
        {
            int bucket_index;
            for ( bucket_index = 0; bucket_index < mandlebrot::colors.size(); bucket_index++ )
            {
                if ( iterations[ i * mandlebrot::pixelWidth + j ] <= buckets[ bucket_index ] )
                    break;
            }

            char red, green, blue;
            
            if ( iterations[ i * mandlebrot::pixelWidth + j ] == nIter )
            {
                red   = 0x00;
                green = 0x00;
                blue  = 0x00;
            }
            else if ( bucket_index == 0 )
            {
                red   = mandlebrot::colors[ 0 ][ mandlebrot::RED   ];
                green = mandlebrot::colors[ 0 ][ mandlebrot::GREEN ];
                blue  = mandlebrot::colors[ 0 ][ mandlebrot::BLUE  ];
            }
            else
            {

                double blend = static_cast<double>( iterations[ i * mandlebrot::pixelWidth + j ] - buckets[ bucket_index - 1 ] ) 
                        / static_cast<double>( buckets[ bucket_index ] - buckets[ bucket_index - 1 ] );

                red   = static_cast<char>( ( 1 - blend ) * mandlebrot::colors[ bucket_index - 1 ][ mandlebrot::RED   ] 
                        + blend * mandlebrot::colors[ bucket_index ][ mandlebrot::RED   ] );
                green = static_cast<char>( ( 1 - blend ) * mandlebrot::colors[ bucket_index - 1 ][ mandlebrot::GREEN ] 
                        + blend * mandlebrot::colors[ bucket_index ][ mandlebrot::GREEN ] );
                blue  = static_cast<char>( ( 1 - blend ) * mandlebrot::colors[ bucket_index - 1 ][ mandlebrot::BLUE  ] 
                        + blend * mandlebrot::colors[ bucket_index ][ mandlebrot::BLUE  ] );
            }
            SDL_SetRenderDrawColor( renderer, red, green, blue, 0xFF );
            SDL_RenderDrawPoint   (renderer, j, i);
        }
    }
}

//color each pixel by the modulo of the iterations it took
//this has the advantage of being zoom invariant, but can get messy
void mandlebrot::modulo_render ( const std::vector<double> &iterations, int nIter, SDL_Renderer *renderer )
{
    for ( int i = 0; i < mandlebrot::pixelWidth; i++ )
    {
        for ( int j = 0; j < mandlebrot::pixelWidth; j++ )
        {
            char red, green, blue;
            if ( iterations[ i * mandlebrot::pixelWidth + j ] == nIter )
            {
                red   = 0x00;
                green = 0x00;
                blue  = 0x00;
            }
            else
            {
                
                int bucket_index  = static_cast<int>( std::floor( iterations[ i * mandlebrot::pixelWidth + j ] ) ) % mandlebrot::colors.size();
                int bucket2_index = ( bucket_index + 1 ) % mandlebrot::colors.size();

                double blend = static_cast<double>( iterations[ i * mandlebrot::pixelWidth + j ] 
                               - std::floor ( iterations[ i * mandlebrot::pixelWidth + j ] ) );

                red   = static_cast<char>( ( 1 - blend ) * mandlebrot::colors[ bucket_index ][ mandlebrot::RED   ] 
                      + blend * mandlebrot::colors[ bucket2_index ][ mandlebrot::RED   ] );
                
                green = static_cast<char>( ( 1 - blend ) * mandlebrot::colors[ bucket_index ][ mandlebrot::GREEN ] 
                      + blend * mandlebrot::colors[ bucket2_index ][ mandlebrot::GREEN ] );
                
                blue  = static_cast<char>( ( 1 - blend ) * mandlebrot::colors[ bucket_index ][ mandlebrot::BLUE  ] 
                      + blend * mandlebrot::colors[ bucket2_index ][ mandlebrot::BLUE  ] );
            }
            SDL_SetRenderDrawColor( renderer, red, green, blue, 0xFF );
            SDL_RenderDrawPoint   (renderer, j, i);
        }
    }
}