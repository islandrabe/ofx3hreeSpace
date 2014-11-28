ofx3hreeSpace
=====================================

Introduction
------------
3hreeSpace offers a method for 3D tracking. It uses two cameras capturing the tracked area from front view and top view. The tracking area is devided into areas of movement (i.e. as shown in the ofx3hreeSpace-sketch.png). For each tracking area the amount of changed pixels get meassured by comparing the current RGB values with the previous ones as a sign for movement. If the amount of changed pixels in the corresponding areas of movement from front view and top view are above a given threshold, the area of movement in 3D (= an intersection of front view and top view) gets triggered as active. 
The code of example-3hreeSpace could be used to give this tracking method a try by setting up two webcams each with a distance of about half a meter to the center of the tracking area. Both cameras need to focus the center of the tracking area. Using cameras with 60° field of view results in a size of the tracking area of about 15-20cm. 
An example for usage is the "Klangkubus" ("sound cube"), an interactive emptiness that acts like an invisible sample player; two videos – [sound performance](http://vimeo.com/109898249) and [dance performance](http://vimeo.com/111193290) – show different approaches of playing the work and [klangkubus.islandrabe.com](http://klangkubus.islandrabe.com/) offers a documentation in german.

License
-------
The code in this repository is available under the [MIT License](https://secure.wikimedia.org/wikipedia/en/wiki/Mit_license).
Copyright (c) 11/2014 Richard Schwarz, [islandrabe](http://islandrabe.com)

Installation
------------
Copy it to your openFrameworks/addons folder.

Dependencies
------------
none

Compatibility
------------
OF 0.8.3

Known issues
------------
v 0.1 only works with 3 areas of movement per dimension resulting in 27 areas that could be used as triggers in space; for adding the option for more or less areas of movement the function turnValues() needs to be adapted.

Version history
------------

### Version 0.1 (28/11/2014):
The first version of 3hreeSpace is put online.


