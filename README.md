# Eldara

## Overview

This application is written in C++20 and built using Qt 6.8.0. These instructions will guide you through building a release version and packaging the application for Windows deployment.


## Prerequisites

* **Qt 6.8.0** installed and available (or accessible via Qt Creator).
* A C++20–capable compiler:
  * MSVC 2019 or 2022 (MinGW-w64 is also supported).
* **windeployqt** tool (bundled with your Qt installation).

## Building a Release

1. **Open a Qt-enabled Shell**
2. **Configure with qmake**
   From the project root:
   ```bash
   qmake CONFIG+=release
   ```
3. **Compile the Project**
   * Using MSVC (nmake):
     ```bash
     nmake
     ```
   * Using MinGW (mingw32-make):
     ```bash
     mingw32-make
     ```
   * Or open the `.pro` file in Qt Creator, switch to Release mode, and click Build.
4. **Locate the Executable**
   After a successful build, the release binaries will reside in `build/`, based on your configuration, you should find the release directory.

## Packaging for Windows
1. **Change to the Release Directory**
2. **Run windeployqt**
   ```bash
   windeployqt Eldara.exe
   ```
   * This command scans `Eldara.exe` and copies all required Qt runtime libraries, plugins, and QML modules into the current directory.
