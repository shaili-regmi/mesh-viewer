# mesh-viewer

OPenGL applications for viewing meshes

## How to build

*Windows*

Open git bash to the directory containing this repository.

```
mesh-viewer $ mkdir build
mesh-viewer $ cd build
mesh-viewer/build $ cmake -G "Visual Studio 16 2019" ..
mesh-viewer/build $ start CS312-MeshViewer.sln
```

Your solution file should contain multiple projects, such as `mesh-viewer`.
To run from the git bash command shell, 

```
mesh-viewer/build $ ../bin/Debug/mesh-viewer
```

*macOS*

Open terminal to the directory containing this repository.

```
mesh-viewer $ mkdir build
mesh-viewer $ cd build
mesh-viewer/build $ cmake ..
mesh-viewer/build $ make
```

To run each program from build, you would type

```
mesh-viewer/build $ ../bin/mesh-viewer
```

# TODO: Screenshots and writeup


# Required Features Implemented:


## Perspective Projection and Camera Controls/Rotations


*NOTE: Rotations have been clamped (Azimuth -180 to 180 and Elevation -90 to 90)*


![pers_cam](https://github.com/shaili-regmi/mesh-viewer/blob/main/pers_view%20and%20camera%20control.png)


## Zoom in and out with Shift


*Cursor position towards the right - zoom out*

*Cursor position towards the left - zoom in*

*For maximum effect, also change cursor position from top to bottom*


![zoom](https://github.com/shaili-regmi/mesh-viewer/blob/main/zoom.png)


# Mesh Viewer


*Implemented Mesh class that is capable of loading PLY files. Examples are unlit shaders:*

*NOTE: Each model will be scaled and translated to fit the screen, but you may need to left-click when it first loads for this transformation to occur and to be able to see the model or center it.*


![unlit1](https://github.com/shaili-regmi/mesh-viewer/blob/main/unlit1.png)


![unlit2](https://github.com/shaili-regmi/mesh-viewer/blob/main/unlit2.png)


# Phong shading


![phong](https://github.com/shaili-regmi/mesh-viewer/blob/main/phong.png)


# Unique Feature


*Per-pixel lighting*


![vertex vs pixel1](https://github.com/shaili-regmi/mesh-viewer/blob/main/Per-vertex%20vs%20Per-pixel.png)


![vertex vs pixel2](https://github.com/shaili-regmi/mesh-viewer/blob/main/Per-vertex%20vs%20Per-pixel%202.png)


![vertex vs pixel3](https://github.com/shaili-regmi/mesh-viewer/blob/main/Per-vertex%20vs%20Per-pixel%203.png)

