[![Build Status (master)](https://img.shields.io/travis/com/denisacostaq/DAQs/master.svg?label=Build%20Status%20%28master%29)](https://api.travis-ci.com/denisacostaq/DAQs)
[![Build Status (last)](https://img.shields.io/travis/com/denisacostaq/DAQs.svg?label=Build%20Status%20%28last%29)](https://travis-ci.com/denisacostaq/DAQs/pull_requests)
[![Coverage Status](https://coveralls.io/repos/github/denisacostaq/DAQs/badge.svg?branch=master)](https://coveralls.io/github/denisacostaq/DAQs)

[![Build History](https://buildstats.info/travisci/chart/denisacostaq/DAQs?showStats=true&branch=master)](https://travis-ci.com/denisacostaq/DAQs/builds?branch=master)
[![Build history](https://buildstats.info/travisci/chart/denisacostaq/DAQs?showStats=true)](https://travis-ci.com/denisacostaq/DAQs/builds)

[![Issues](https://img.shields.io/github/issues/denisacostaq/DAQs.svg)](https://github.com/denisacostaq/DAQs/issues)
[![Bugs](https://img.shields.io/github/issues/denisacostaq/DAQs/bug.svg)](https://github.com/denisacostaq/DAQs/issues?q=is%3Aissue+is%3Aopen+label%3Abug)
[![Pulls](https://img.shields.io/github/issues-pr/denisacostaq/DAQs.svg)](https://github.com/denisacostaq/DAQs/pulls)


## Table of contents

<!-- MarkdownTOC levels="1,2,3" autolink="true" bracket="round" -->

- [DAQs](#daqs)
- [Build instructions](#build-instructions)
  - [Build for android](#build-for-android)
    - [Build android for arm](#build-android-for-arm)
    - [Build android for x86_64](#build-android-for-x86_64)
  - [Build for your host](#build-for-your-host)
- [API documentation](#api-documentation)

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

### Build android for arm.

```bash
> cmake -DCMAKE_TOOLCHAIN_FILE=<ndk_path>/build/cmake/android.toolchain.cmake -DProtobuf_DIR=<protobuf_install_dir>/lib/cmake/protobuf -DQt5SDK_DIR=<your_path> -DANDROID_SDK_DIR=<your_path> ..
> cmake --build . --target apk_debug -j $(nproc)
```

### Build android for x86_64.

```bash
> cmake -DANDROID_ABI=x86_64 -DCMAKE_TOOLCHAIN_FILE=<ndk_path>/build/cmake/android.toolchain.cmake -DProtobuf_DIR=<protobuf_install_dir>/lib/cmake/protobuf -DQt5SDK_DIR=<your_path> -DANDROID_SDK_DIR=<your_path> ..
> cmake --build . --target apk_debug -j $(nproc)
```


## Build for your host.

```bash
> cmake -DQt5_DIR=<your_path>/lib/cmake/Qt5 ..
> cmake --build . --target all -j $(nproc)
```

# API documentation.

A detailed API documentation can be found on [github pages](https://denisacostaq.github.io/DAQs/index.html).
