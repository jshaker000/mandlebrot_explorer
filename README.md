# mandlebrot_explorer

## Purpose:
The purpose of this program is to be an easy to use fractal explorer and a demo of interactivity using the SDL2 library.

## DependancieS:
To install this program, one must have *SDL2* (available from their website or most package managers) and must have lpthreads (default on most POSIX systems)

## Compile & Install:
Clone the repository and run **make && make install** from the *src/* directory. 
This will compile the program and copy it to your **$PATH**, and install the man page.
Depending on your system, *make install* may require administrative privileges.


    $ make && make install

To uninstall the program, run:

    $ make clean && make uninstall

## Usage:

Run the program, you can then use the:

arrow keys to pan 

+/- to zoom. 

[ / ] to increase or decrease the amount of iterations done. Decreasing the iterations reduces sharpness but greatly speeds up rendering
It could be nice to allow the image to be blurry while you find an area of interest, then sharpen for desired effect

Q to quit

H toggles to 'histogram view'

M toggles to 'modulo view'

Numbers 1 - 4 allow you to toggle between preloaded color maps from *src/config.cpp* (from this repo
1 and 2 are loaded with data, 3 and 4 are not).

R to reset your view and scale back to the main fractal

Currently Histogram view looks inconsistent. In some view it looks great and in others it looks iffy. 
But I think the concept is good and efforts will be made to improve it.

Efforts will also be made to have seperate keybindings for smooth / coarse zoom and pan

### Customization:

To customize, copy *src/config.cpp.def* into *src/config.cpp* and make your edits there

*src/config.cpp* is in the .gitignore, so you can edit without worrying about syncing with the repo.

However, beware when you pull if there are changes that will cause your *src/config.cpp* to become incompatable.

In the file, you can modify the color palette, num threads, pixelWidth, etc.

It should be fairly self explanatory.

After customizing, you must re-run make and make install, but it should be fairly quick because only a few cosntants were changed.

##  TODO:

1)  Add seperate controls for fine and coarse zoom/pan
2)  More intellegently setup a color gradient for historgram. Now histogram looks great at some zoom 
    levels but garbage at others. Also more intellegent "bucket" placing for histogram
3)  More intellegently split work between threads, rather than having each take a stripe.
    Maybe guess where the most work would need to be done?
4)  Have a better interface to scroll multiple steps at once, 
    perhaps click and drag and/or boxes to type in Xmin/Xmax Ymin/Ymax
5)  Add GUI elements to ajust the colormap
6)  Add a second window as a "HUD" to show where you are on the fractal
7)  Investigate bug where at high zoom levels thread panes get out of sync

## Examples Images

Depending on what you are looking for, both the Historgram and Modulo renderings can look nice.

However, the Modulo rendering tends to be more consistently nice. It also has zoom invariance

The Histogram renderings are a bit finicky in my experience. It tends to look very nice on certain structures and
confused on others. Playing with the gradient, and how much automtic vs manual shading can give you some interesting results.

Here are some comparisons:

Modulo:

![Modulo 1]( /screenshots/modulo1.png?raw=true) 

Histogram:

![Histogram 1]( /screenshots/histogram1.png?raw=true) 

Modulo:

![Modulo 2](/screenshots/modulo2.png?raw=true)

Histogram:

![Histogram 2](/screenshots/histogram2.png?raw=true)

Here are some extras:

Wikipedia Color Scheme (modulo):

x_min =    -1.7475429838556857387, x_max =    -1.7475429812841090094

y_min = 6.0249427805793403368e-07, y_max = 6.0506585439303890429e-07

![Wikipedia ColorSceme]( /screenshots/wikipedia_color_scheme.png?raw=true) 

Spiral (histogram):

![Spiral 1]( /screenshots/spiral1.png?raw=true) 

Minibrot (histogram):

![Minibrot 1]( /screenshots/minibrot1.png?raw=true) 

Minibrot (histogram):

![Minibrot 2](/screenshots/minibrot2.png?raw=true)