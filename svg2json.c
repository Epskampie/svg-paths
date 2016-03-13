/* Hello World program */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#define NANOSVG_IMPLEMENTATION  // Expands implementation
#include "nanosvg.h"
#include <json/json.h>

int indent = 0;

int init(char *filename) {
    if (access(filename, F_OK) == -1) {
        printf("File doesn't exist or isn't accessible: %s \n", filename);
        return 1;
    }
    
    // Load
    struct NSVGimage* image;
    image = nsvgParseFromFile(filename, "px", 96);
    if (!image) {
        printf("Cannot read from file: %s \n", filename);
        return 1;
    }
    
    // Use...
    for (NSVGshape *shape = image->shapes; shape != NULL; shape = shape->next) {
        
        printf("\n");
        printf("shape: %s\n", shape->id);
        
        for (NSVGpath *path = shape->paths; path != NULL; path = path->next) {
            
            printf("=== path with %d points ===\n", path->npts);
            
            for (int i = 0; i < path->npts - 1; i += 3) {
                float* p = &path->pts[i*2];
                printf("%f,%f %f,%f %f,%f %f,%f \n", p[0],p[1], p[2],p[3], p[4],p[5], p[6],p[7]);
            }
        }
    }
    // Delete
    nsvgDelete(image);
    
    /*Creating a json object*/
    json_object * jobj = json_object_new_object();

    /*Creating a json string*/
    json_object *jstring = json_object_new_string("Joys of Programming");

    /*Creating a json integer*/
    json_object *jint = json_object_new_int(10);

    /*Creating a json boolean*/
    json_object *jboolean = json_object_new_boolean(1);

    /*Creating a json double*/
    json_object *jdouble = json_object_new_double(2.14);

    /*Creating a json array*/
    json_object *jarray = json_object_new_array();

    /*Creating json strings*/
    json_object *jstring1 = json_object_new_string("c");
    json_object *jstring2 = json_object_new_string("c++");
    json_object *jstring3 = json_object_new_string("php");

    /*Adding the above created json strings to the array*/
    json_object_array_add(jarray,jstring1);
    json_object_array_add(jarray,jstring2);
    json_object_array_add(jarray,jstring3);

    /*Form the json object*/
    /*Each of these is like a key value pair*/
    json_object_object_add(jobj,"Site Name", jstring);
    json_object_object_add(jobj,"Technical blog", jboolean);
    json_object_object_add(jobj,"Average posts per day", jdouble);
    json_object_object_add(jobj,"Number of posts", jint);
    json_object_object_add(jobj,"Categories", jarray);

    /*Now printing the json object*/
    printf ("The json object created: %sn",json_object_to_json_string(jobj));
          
    return 0;
}


int main(int argc, char **argv)
{
    if (argc != 2) {
        printf(
"Usage: %s INPUT \n\
Reads the INPUT svg file, and outputs path points as JSON. \n\
All output coordinates are absolute.", 
            argv[0]
        );
        return 0;
    }
    
    return init(argv[1]);
}