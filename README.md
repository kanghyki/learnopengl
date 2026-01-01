# Learn OpenGL Study Project

This project is a personal study repository for learning OpenGL. It implements modern OpenGL rendering techniques based on the `Learn OpenGL`.

## 🔗 References

- **Learn OpenGL**: [https://learnopengl.com/](https://learnopengl.com/)

## 🛠 Tech Stack

- **Language**: C++ 17
- **Graphics API**: OpenGL 3.3+
- **Window Management**: GLFW
- **OpenGL Loader**: GLAD
- **Mathematics**: GLM
- **Model Loading**: Assimp
- **UI**: Dear ImGui
- **Logging**: spdlog
- **Image Loading**: stb_image

## 🚀 Getting Started

### Prerequisites

- CMake 3.13 or higher
- C++ 17 compatible compiler (Clang, GCC, MSVC, etc.)

### Build and Run

You can build the project using the `build.sh` script included in the root folder.

```bash
# Grant execution permission (if necessary)
chmod +x build.sh

# Execute build
./build.sh

# Run the application (macOS/Linux)
./output
```

## 📂 Project Structure

- `src/`: Core source code for engine and rendering logic (.cpp, .hpp)
- `shader/`: GLSL shader files (Vertex/Fragment/Geometry Shaders)
- `imgui/`: Dear ImGui library for GUI implementation
- `image/`: Textures and resource images

## ✨ Key Features

- **Camera System**: Interactive camera class for free-look navigation.
- **Lighting**: Phong and Blinn-Phong lighting models, Shadow Mapping.
- **Model Loading**: Complex 3D model loading and rendering using Assimp.
- **Post Processing**: Effects like Inversion, Grayscale, and Gaussian Blur using Framebuffers.
- **Environment Mapping**: Skyboxes and reflection effects using Cubemaps.
- **Input Management**: Mouse and keyboard input handling.
- **UI**: Real-time parameter adjustment and debug info display via ImGui.
