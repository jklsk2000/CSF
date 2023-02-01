/*
* CSF Assignment 4
* Image Processing with Plugins
* Expose Plugin - adjusts the intensity of each pixel
* Simone Bliss & Danny Lee
* sbliss5@jhu.edu, jlee692@jhu.edu
*/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include "image_plugin.h"

using std::string; using std::cout; using std::endl;
using std::cerr;

struct Arguments {
	float factor;
};

const char *get_plugin_name(void) {
	return "expose";
}

const char *get_plugin_desc(void) {
	return "adjust the intensity of all pixels";
}

void *parse_arguments(int num_args, char *args[]) {
	if (num_args != 1) {
		return NULL;
	}

    string sFactor = args[0];
    float factor = 0;

	try {
        factor = stof(sFactor);
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

int multiplyFactor(uint8_t color, float factor) {
    int result = color * factor;
    if (result > 255) {
        result = 255;
    }
    return result;
}

// Helper function 
static uint32_t expose_pixel(uint32_t pix, float factor) {
	uint8_t r, g, b, a;
	img_unpack_pixel(pix, &r, &g, &b, &a);
    r = multiplyFactor(r, factor);
    g = multiplyFactor(g, factor);
    b = multiplyFactor(b, factor);
	return img_pack_pixel(r, g, b, a);
}

struct Image *transform_image(struct Image *source, void *arg_data) {
	struct Arguments *args = (Arguments *) arg_data;
    float factor = args->factor;

	// Allocate a result Image
	struct Image *out = img_create(source->width, source->height);
	if (!out) {
		free(args);
		return NULL;
	}

    unsigned num_pixels = source->width * source->height;
	for (unsigned i = 0; i < num_pixels; i++) {
		out->data[i] = expose_pixel(source->data[i], factor);
	}

	free(args);
	return out;
}