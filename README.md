[![Build Status (master)](https://api.travis-ci.com/denisacostaq/DAQs.svg?branch=master)](https://api.travis-ci.com/denisacostaq/DAQs)
[![Build Status (last)](https://api.travis-ci.com/denisacostaq/DAQs.svg)](https://api.travis-ci.com/denisacostaq/DAQs)

## Table of contents

<!-- MarkdownTOC levels="1,2,3" autolink="true" bracket="round" -->

- [DAQs](#daqs)
- [Build instructions](#build-instructions)
  - [Build for android](#build-for-android)
  - [Build for your host](#build-for-your-host)

<!-- /MarkdownTOC -->

# DAQs
Data Acquisition System (basic demo).

# Build instructions

Get the DAQs source code:
`git clone --depth=10 https://github.com/denisacostaq/DAQs.git`

Move to a build folder:
`mkdir DAQs/build ; pushd DAQs/build`

The build system is based on CMake, for specific platforms follow the next instructions.

## Build for android.

At the moment of calling cmake you need to set the following variables:

 - `ANDROID_SDK_DIR` Path to the android sdk installation.
 - `CMAKE_TOOLCHAIN_FILE` The path to the Android toolchain file distributed with the NDK ([r18b](https://dl.google.com/android/repository/android-ndk-r18b-linux-x86_64.zip) version is recommended).
 - `Protobuf_DIR` Path where a protobuf install dir found, it's important to have a protobuf with libraries build for android and compiler build for your current host ([more details](https://github.com/denisacostaq/DAQs/wiki/Protobuf)).
 - `Qt5_SDK_DIR` Path where a Qt5 for android installation can be found.

Example:

 - `cmake -DCMAKE_TOOLCHAIN_FILE=<ndk_path>/build/cmake/android.toolchain.cmake -DProtobuf_DIR=<protobuf_install_dir>/lib/cmake/protobuf -DQt5SDK_DIR=<your_path> -DANDROID_SDK_DIR=<your_path> ..`
 - `cmake --build . --target apk_debug -j $(nproc)`

## Build for your host.

`cmake -DQt5_DIR=<your_path>/lib/cmake/Qt5 ..`

`cmake --build . --target all -j $(nproc)`
