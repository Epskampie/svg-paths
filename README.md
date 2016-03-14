svg-paths is a linux commad line utility that takes all the paths from an SVG file and makes all the points absolute (so all transformations, viewboxes etc are collapsed). It then exports the data in easily readable JSON format.

Useful for making visualisations.

Usage
=====

* Use your package manager to install the json-c library.
    * On debian/ubuntu linux: `sudo apt-get install libjson0`
* Download svg-paths, and make the file executable:
    * `wget https://raw.githubusercontent.com/Epskampie/svg-paths/master/bin/linux-x64/svg-paths`
    * `chmod u+x svg-paths`
* Run it:
    * `./svg-paths PATH/TO/SVG/FILE.svg`

Build instructions
==================

* Make sure you installed the json-c library.
    * On debian/ubuntu linux: `sudo apt-get install libjson0-dev`
* Then simply run `make`.
    
Acknowledgments
===============

SVG parsing by Nano SVG: https://github.com/memononen/nanosvg
