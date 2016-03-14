/* Hello World program */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#define NANOSVG_IMPLEMENTATION  // Expands implementation
#include "nanosvg.h"
#include <json/json.h>

json_object * color2json(unsigned int color) {
    char res[9 + 1];
    if (color <= 0xFFFFFFFF)
    {
        sprintf(&res[0], "#%02x%02x%02x%02x", 
            (color >> 0) & 0x000000FF,
            (color >> 8) & 0x000000FF,
            (color >> 16) & 0x000000FF,
            (color >> 24) & 0x000000FF
        );
    }
    return json_object_new_string(res);
}

json_object * gradientStop2json(NSVGgradientStop * stop) {
    json_object * jStop = json_object_new_object();
    json_object_object_add(jStop, "color", color2json(stop->color));
    json_object_object_add(jStop, "offset", json_object_new_double(stop->offset));
    return jStop;
}

json_object * gradient2json(NSVGgradient * gradient) {
    json_object * jGradient = json_object_new_object();
    
    json_object * jStops = json_object_new_array();
    json_object_object_add(jGradient, "stops", jStops);
    for (int i = 0; i < gradient->nstops; i++) {
        json_object_array_add(jStops, gradientStop2json(&gradient->stops[i]));
    }
    
    return jGradient;
}

json_object * paint2json(NSVGpaint * paint) {
    json_object * jPaint = json_object_new_object();
    
    const char * types[] = {
        "none",
        "color",
        "linear gradient",
        "radial gradient",
    };
    json_object_object_add(jPaint, "type", json_object_new_string(types[(int)paint->type]));
    
    if (paint->type == NSVG_PAINT_COLOR) {
        json_object_object_add(jPaint, "color", color2json(paint->color));
    } else if (paint->type == NSVG_PAINT_LINEAR_GRADIENT) {
        json_object_object_add(jPaint, "gradient", gradient2json(paint->gradient));
    }
    
    return jPaint;
}

json_object * floatArray2Json(float * floatArray, int count) {
    json_object * jFloatArray = json_object_new_array();
    for (int i = 0; i < count; i++) {
        json_object_array_add(jFloatArray, json_object_new_double(floatArray[i]));
    }
    return jFloatArray;
}

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
    
    json_object * jImage = json_object_new_object();
    json_object_object_add(jImage, "width", json_object_new_double(image->width));
    json_object_object_add(jImage, "height", json_object_new_double(image->height));
    
    json_object * jShapes = json_object_new_array();
    json_object_object_add(jImage, "shapes", jShapes);
    
    // Convert to JSON
    for (NSVGshape *shape = image->shapes; shape != NULL; shape = shape->next) {
        
        json_object * jShape = json_object_new_object();
        json_object_array_add(jShapes, jShape);
        json_object_object_add(jShape, "id", json_object_new_string(shape->id));
        json_object_object_add(jShape, "fill", paint2json(&shape->fill));
        json_object_object_add(jShape, "stroke", paint2json(&shape->stroke));
        json_object_object_add(jShape, "opacity", json_object_new_double(shape->opacity));
        json_object_object_add(jShape, "strokeWidth", json_object_new_double(shape->strokeWidth));
        if (shape->strokeDashCount) {
            json_object_object_add(jShape, "strokeDashOffset", json_object_new_double(shape->strokeDashOffset));
            json_object_object_add(jShape, "strokeDashArray", floatArray2Json(shape->strokeDashArray, shape->strokeDashCount));
        }
        const char * fillRules[] = { "nonzero", "evenodd" };
        json_object_object_add(jShape, "fillRule", json_object_new_string(fillRules[(int)shape->fillRule]));
        
        json_object * jPaths = json_object_new_array();
        // json_object_object_add(jShape, "paths", jPaths);
        
        for (NSVGpath *path = shape->paths; path != NULL; path = path->next) {
            
            json_object * jPath = json_object_new_object();
            json_object_array_add(jPaths, jPath);
            json_object_object_add(jPath, "closed", json_object_new_boolean(path->closed));
            
            if (path->npts > 0) {
                float * p = &path->pts[0];
                json_object * jStart = json_object_new_array();
                json_object_array_add(jStart, json_object_new_double(p[0]));
                json_object_array_add(jStart, json_object_new_double(p[1]));
                json_object_object_add(jPath, "start", jStart);
            }
            
            json_object * jBezierCurves = json_object_new_array();
            json_object_object_add(jPath, "bezierCurves", jBezierCurves);
            
            for (int i = 1; i < path->npts - 1; i += 3) {
                float* p = &path->pts[i*2];
                
                json_object * jBezierCurve = json_object_new_object();
                json_object_array_add(jBezierCurves, jBezierCurve);
                
                // printf("%f,%f %f,%f %f,%f %f,%f \n", p[0],p[1], p[2],p[3], p[4],p[5], p[6],p[7]);
                
                for (int i = 0; i < 6; i += 2) {
                    json_object * jPoint = json_object_new_array();
                    json_object_array_add(jPoint, json_object_new_double(p[i]));
                    json_object_array_add(jPoint, json_object_new_double(p[i + 1]));
                    const char * name = "controlPoint1";
                    switch (i) {
                        case 2:
                            name = "controlPoint2";
                            break;
                        case 4:
                            name = "end";
                            break;
                    }
                    json_object_object_add(jBezierCurve, name, jPoint);
                }
            }
        }
    }
    
    // Delete
    nsvgDelete(image);
    
    // Print
    printf("%s\n",json_object_to_json_string_ext(jImage, JSON_C_TO_STRING_PRETTY));
          
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
        return 1;
    }
    
    return init(argv[1]);
}