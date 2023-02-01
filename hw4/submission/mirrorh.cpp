/*
* CSF Assignment 4
* Image Processing with Plugins
* Mirrorh Plugin - generates a mirror image of the input image reflected horizontally. 
* Simone Bliss & Danny Lee
* sbliss5@jhu.edu, jlee692@jhu.edu
*/

#include <stdlib.h>
#include "image_plugin.h"

struct Arguments {
	// This plugin doesn't accept any command line arguments;
	// just define a single dummy field.
	int dummy;
};

const char *get_plugin_name(void) {
	return "mirrorh";
}

const char *get_plugin_desc(void) {
	return "mirror image horizontally";
}

void *parse_arguments(int num_args, char *args[]) {
	(void) args; // this is just to avoid a warning about an unused parameter

	if (num_args != 0) {
		return NULL;
	}
	return calloc(1, sizeof(struct Arguments));
}

struct Image *transform_image(struct Image *source, void *arg_data) {
	struct Arguments *args = (Arguments *) arg_data;

	// Allocate a result Image
	struct Image *out = img_create(source->width, source->height);
	if (!out) {
		free(args);
		return NULL;
	}

    unsigned left = 0;
    unsigned right = source->width - 1;

    for (unsigned i = 0; i <= source->height; i++) {
        while (left <= right) {
            out->data[left] = source->data[right];
            out->data[right] = source->data[left];
            left++;
            right--;
        }
        left = i * source->width;
        right = (i+1) * source->width - 1;
    }

	free(args);

	return out;
}