# qt-pc-viewer
Qt ("cute") Point Cloud Viewer of PLY file format of point clouds.

## Description

Cross platform Qt based OpenGL application written in C++. 
The PLY format point cloud is loaded from "File" menu. If the color attributes are present in the PLY file, those will be displayed with each point.
In case of no color information the color will be genarted according to the look-up-table. 
It uses arcball OpenGL camera model for convienient dispaly of the model.

## Getting Started

### Dependencies
You need have any Qt version installed in your system and compiler supporting C++11 standard or higher.
* I.e. on Ubuntu 20.04:
```
sudo apt-get install qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools
```

### Building
```
mkdir build && cd build
qmake ..
make
```

Alternatively open qt-pc-viewer.pro in QtCreator and build and run.

### Executing program
```
cd ../bin 
./qt-pc-viewer
```
Check example pointcloud of myself near desk: 
```
./qt-pc-viewer ../resources/pointclouds/me.ply 
```

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

* [Yuki Koyama, tinycolormap](https://github.com/yuki-koyama/tinycolormap)
* [Dimitri Diakopoulos, tinyply](https://github.com/ddiakopoulos/tinyply)
* [Will Usher, Arcball camera model](https://github.com/Twinklebear/arcball-cpp)
