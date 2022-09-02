# Mod Maker

This is a tool for developing mods for either Factorio or Minecraft

## Status

This project is very early in development, currently is not capable of creating any mods.

## Building

This is a standard visual studio c++ project, therefore the easiest way to build it is to install the dependencies with vcpkg, and open the project in visual studio 2022. This app should be build as a 64 bit app, and is configured to use c++20.

### Dependencies
The following dependencies are written how they should be written for the vcpkg commands (without triplet)

- imgui[glfw-binding,opengl3-binding,freetype,docking-experimental]
- rapidjson
- glad[extensions,gl-api-latest]
- glfw3
- nativefiledialog
