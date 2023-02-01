/*
* CSF Assignment 4
* Image Processing with Plugins
* Tile Plugin - generates an image containing an N x N arrangement of tiles, 
*               each tile being a smaller version of the original image
* Simone Bliss & Danny Lee
* sbliss5@jhu.edu, jlee692@jhu.edu
*/
#include <stdlib.h>
#include <iostream>
#include <string>
#include <math.h>
#include "image_plugin.h"

using std::string; using std::cout; using std::endl;

struct Arguments {
	int factor;
};

const char *get_plugin_name(void) {
	return "tile";
}

const char *get_plugin_desc(void) {
	return "tile source image in an NxN arrangement";
}

void *parse_arguments(int num_args, char *args[]) {
	if (num_args != 1) {
		return NULL;
	}

    string sFactor = args[0];
    int factor = 0;
    
    try {
        factor = stoi(sFactor);;
    } catch (const std::invalid_argument& e) {
		return NULL;
    }

    if (factor < 0) {
        return NULL;
    }

    Arguments *arguments = (Arguments *) malloc(sizeof(Arguments));
    arguments->factor = factor;
	return arguments;
}

unsigned determineIter(unsigned h, unsigned v, unsigned hiter, unsigned viter, 
    unsigned hSpread, unsigned vSpread, unsigned width) {

    unsigned iter = 0;
    unsigned vNum = 0;

    if (v < vSpread) {
        vNum = v * viter * width;
    } else {
        vNum = (vSpread * viter * width) + (v - vSpread) * (viter - 1) * width;
    }
    if (h < hSpread) {
        iter = vNum + h * hiter;
    } else {
        iter = vNum + hSpread * hiter + (h - hSpread) * (hiter - 1);
    }

    return iter;
}

struct Image *transform_image(struct Image *source, void *arg_data) {
	struct Arguments *args = (Arguments *) arg_data;
    int factor = args->factor;

	// Allocate a result Image
	struct Image *out = img_create(source->width, source->height);
	if (!out) {
		free(args);
		return NULL;
	}

    unsigned iter = 0;
    unsigned hiter = ceil((double) source->width / factor);
    unsigned viter = ceil((double) source->height / factor);
    static int arrLen = hiter * viter;
    uint32_t *sample = (uint32_t *) malloc(sizeof(uint32_t) * (arrLen));
    int counter = 0;

    // Sample every Nth pixel horizontally and vertically
    for (unsigned v = 0; v < viter; v++) {
        for (unsigned h = 0; h < hiter; h++) {
            sample[counter] = source->data[iter];
            iter += factor;
            counter++;
        }
        iter = (v + 1) * factor * source->width;
    }

    unsigned hSpread = source->width % factor;
    unsigned vSpread = source->height % factor;

    if (hSpread == 0) {
        hSpread = factor;
    }
    if (vSpread == 0) {
        vSpread = factor;
    }

    // Fill each of the tile using the sample array
    for (unsigned v = 0; v < (unsigned) factor; v++) {
        for (unsigned h = 0; h < (unsigned) factor; h++) {
            iter = determineIter(h, v, hiter, viter, hSpread, vSpread, source->width);
            counter = 0;
            for (unsigned c = 0; c < viter; c++) {
                if (v < vSpread || c != viter - 1) {
                    for (unsigned r = 0; r < hiter; r++) {
                        if (h < hSpread || r != hiter - 1) {
                            out->data[iter] = sample[counter];
                        }
                        counter++;
                        iter++;
                    }
                    iter += source->width - hiter;
                }
            }
        }
    }

    free(sample);
	free(args);
	return out;
}