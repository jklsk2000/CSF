/*
* CSF Assignment 4
* Image Processing with Plugins
* Mirrorv Plugin - generates a mirror image of the input image reflected vertically
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
	return "mirrorv";
}

const char *get_plugin_desc(void) {
	return "mirror image vertically";
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

    unsigned top = 0;
    unsigned bot = source->width * (source->height - 1);

    for (unsigned i = 0; i < source->width; i++) {
        while (top <= bot) {
            out->data[top] = source->data[bot];
            out->data[bot] = source->data[top];
            top += source->width;
            bot -= source->width;
        }
        top = i + 1;
        bot = source->width * (source->height - 1) + i + 1;
    }

	free(args);

	return out;
}