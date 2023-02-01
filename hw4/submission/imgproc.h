/*
* CSF Assignment 4
* Image Processing with Plugins
* Function prototypes for Driver functions（headers）
* Simone Bliss & Danny Lee
* sbliss5@jhu.edu, jlee692@jhu.edu
*/

#ifndef IMGPROC_H
#define IMGPROC_H

#include <vector>
#include <string>

using std::vector; using std::string;

struct Plugin {
    void *handle;
    const char *(*get_plugin_name)(void);
    const char *(*get_plugin_desc)(void);
    void *(*parse_arguments)(int num_args, char *args[]);
    struct Image *(*transform_image)(struct Image *source, void *arg_data);
};

//helper functions
bool loadPlugins(Plugin &plugin, string command, string effect);
bool checkArgs(string effect, string sfactor);
bool checkFactor(string effect, string sfactor);

#endif /* IMGPROC_H */