/*
* CSF Assignment 4
* Image Processing with Plugins
* Driver Program
* Simone Bliss & Danny Lee
* sbliss5@jhu.edu, jlee692@jhu.edu
*/

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <stdlib.h>
#include <dirent.h>
#include <dlfcn.h>
#include "imgproc.h"
#include "image.h"

using std::cout; using std::endl; using std::cerr;
using std::string; using std::vector;

int main(int argc, char *argv[]) {

    // No CL arguments
    if (argc == 1) {
        cout << "Usage: imgproc <command> [<command args...>]" << endl
        << "Commands are:" <<endl
        << "\tlist" << endl
        << "\texec <plugin> <input img> <output img> [<plugin args...>]" << endl;
        return 1;
    }

    // At least one argument (guaranteed)
    string command = argv[1];
    Plugin plugin;

    //imageproc + list
    if (command.compare("list") == 0) {
        loadPlugins(plugin, command, "");
    }

    //well formatted plugin requests (exec + effect)
    else if (command.compare("exec") == 0) {
        Image *sourceImage;
        Image *destinationImage;

        //guaranteed arguments
        string effect = argv[2];
        const char *sourceFile = argv[3];
        const char *destinationFile = argv[4];
        loadPlugins(plugin, command, effect);

        int num_args = 0;
        char *arg_data[10];

        if (argc >= 6) {
            num_args = argc - 5;
            for (int i = 0; i < num_args; i++) {
                arg_data[i] = argv[i + 5];
            }
        }

        // if (argc == 5) {
        //     num_args = 0;
        // } else {
        //     num_args = 1;
        //     arg_data[0] = argv[5];
        // }

        sourceImage = img_read_png(sourceFile);
        if (sourceImage == NULL) {
            cerr << "Error: invalid source file" << endl; 
            return 1;
        }
        void *args = plugin.parse_arguments(num_args, arg_data);

        if (args == NULL) {
            cerr << "Error: invalid command argument or number of arguments" << endl;
            return 1;
        }

        destinationImage = plugin.transform_image(sourceImage, args);
        img_write_png(destinationImage, destinationFile);
        img_destroy(sourceImage);
        img_destroy(destinationImage);
        dlclose(plugin.handle);
        return 0;

        //only gets hit if cl args are invalid
        cerr << "Error: invalid effect argument or factor argument" << endl;
        return 1;
    }
    //invalid commands given
    else {
        cerr << "Error: invalid command argument or number of arguments" << endl;
        return 1;
    }
}

//helper functions

/*
 * Purpose: load plugins
 * Parameters:
 * 		&pluginArr - reference to a vector of Plugin structs (struct of function pointers for a plugin)
 *      command - string, the command argument given ('list' or 'exec')
 * Returns:
 * 		boolean, whether or not it was successful
 */
bool loadPlugins(Plugin &plugin, string command, string effect) {
    string pluginDir;
    const char * envDir = getenv("PLUGIN_DIR");

    //check if enironnment variable is present, else hard code it
    if (envDir != NULL) {
        pluginDir = envDir;
    } else {
        pluginDir = "./plugins";
    }

    // Open the plugin directory
    DIR *d;
    d = opendir(pluginDir.c_str());

    if (d == NULL) {
        cerr << "Error: could not open plugin directory" << endl;
        return false;
    }

    struct dirent *de;

    while ((de = readdir(d)) != NULL) {
        
        string filename = de->d_name;
        string filepath = pluginDir + "/" + de->d_name;
        int fpLen = filepath.length();

        //skip all non .so files
        if ( !(fpLen > 3) || (filepath[fpLen - 3] != '.') || (filepath[fpLen - 2] != 's') || (filepath[fpLen - 1] != 'o')) {
            continue;
        }

        if (!(filename.compare(effect + ".so") == 0) && command.compare("exec") == 0) {
            continue;
        }
        //load the dynamic library file and set "handle" for the dynamic library
        plugin.handle = dlopen(filepath.c_str(), RTLD_LAZY);

        if (plugin.handle == NULL)  {
            cerr << "Error: plugin not found" << endl;
            dlclose(plugin.handle);
            closedir(d);
            return 1;
        }

        //error handling for the wrong shared libraries? i think that's rude and probs unnecessary
        *((void **) &plugin.get_plugin_name) = dlsym(plugin.handle, "get_plugin_name");
        *((void **) &plugin.get_plugin_desc) = dlsym(plugin.handle, "get_plugin_desc");
        *((void **) &plugin.parse_arguments) = dlsym(plugin.handle, "parse_arguments");
        *((void **) &plugin.transform_image) = dlsym(plugin.handle, "transform_image");

        if (command.compare("list") == 0) {
            cout << plugin.get_plugin_name() << ": " << plugin.get_plugin_desc() << endl;
        }
    }
    
    closedir(d);
    return true;
}