# Project Name

## Overview

This application is written in C++20 and built using Qt 6.8.0. These instructions will guide you through building a release version and packaging the application for Windows deployment.

---

## Prerequisites

* **Qt 6.8.0** installed and available in your PATH (or accessible via Qt Creator).
* A C++20–capable compiler:

  * MSVC 2019 or 2022 (MinGW-w64 is also supported).
* **windeployqt** tool (bundled with your Qt installation).

---

## Directory Structure

```text
/Eldara
├── src/               # Source files
├── include/           # Header files
├── resources/         # Icons, QML, other assets
├── build/             # Build output (ignored by VCS)
...
└── README.md          # This file
```

---

## Building a Release

1. **Open a Qt-enabled Shell**
   Launch the "Qt 6.8.0 for Desktop" command prompt (or the Visual Studio Developer Prompt if using MSVC).

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
   After a successful build, the release binaries will reside in `build/release/` (or `build-*/release/`).

---

## Packaging for Windows

1. **Change to the Release Directory**

   ```bash
   cd build/release
   ```

2. **Run windeployqt**

   ```bash
   windeployqt YourApp.exe
   ```

   * This command scans `YourApp.exe` and copies all required Qt runtime libraries, plugins, and QML modules into the current folder.
   * If your application uses QML from a custom directory, add:

     ```bash
     windeployqt --qmldir path\to\resources\qml YourApp.exe
     ```

3. **Clean Up Unneeded Files**

   * Remove any debug DLLs (e.g., `Qt6Cored.dll`, etc.).
   * Delete unused plugins or modules to keep the package lean.

4. **Create a Distribution Archive**

   ```bash
   cd ..
   zip -r YourApp-v1.0.zip release/
   ```

   * Alternatively, use an installer generator (e.g., NSIS, Inno Setup) for a professional installer.

---

## Verifying the Package

* **Test on a Clean Windows VM**
  Ensure all dependencies are present and that the application launches without errors.
* **Dependency Inspection**
  Use tools like [Dependency Walker](http://www.dependencywalker.com/) or `ldd` (for MinGW) to verify linked libraries.

---

## Troubleshooting

* **Missing DLL Errors**
  Rerun `windeployqt` with `--qmldir` if using QML or check the `platforms/` and `imageformats/` plugin folders.
* **High Memory Usage on Startup**
  This is normal: Qt and the MSVC runtime reserve memory pools and thread stacks. Windows will free pages under pressure (see official docs).
* **Build Failures**
  * Confirm your Qt installation path is correct.
  * Run `qmake -v` to verify the correct Qt version is being used.

---

## License

This project is released under the \[Your License Name] License. See `LICENSE` for details.

---
