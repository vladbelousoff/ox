# Ox Game Engine with GTK Editor

A simple game engine with a GTK-based editor interface.

## Components

### Game Engine (`ox`)
- **ECS (Entity Component System)**: Core architecture for game objects
- **Rendering**: Raylib-based rendering system
- **Memory Management**: Custom memory allocator with tracking
- **Logging**: Debug logging system
- **Data Structures**: Dynamic lists and utilities

### Editor (`ox_editor`)
- **GTK3 Interface**: Modern GUI with panels and toolbars
- **Scene Hierarchy**: Tree view of game entities
- **Properties Panel**: Component editing interface
- **Viewport**: Integrated game scene preview with animation
- **Entity Management**: Create, delete, and manipulate entities
- **Menu System**: File operations and entity management

## Building

### Prerequisites
```bash
sudo apt update
sudo apt install -y libgtk-3-dev pkg-config build-essential cmake
```

### Build Instructions
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

This will create two executables:
- `ox` - The game engine demo (bouncing balls simulation)
- `ox_editor` - The GTK-based game editor

## Running

### Game Engine Demo
```bash
./ox
```
Shows a physics simulation with bouncing balls and collision detection.

### Game Editor
```bash
./ox_editor
```
Opens the GTK editor with:
- **Scene Hierarchy Panel** (left): Shows entities in tree view
- **Viewport Panel** (center): Animated preview with grid and sample entities
- **Properties Panel** (bottom): Entity component editing
- **Menu Bar**: File and Entity operations
- **Toolbar**: Quick access buttons

## Features

### Editor Features
- **Entity Creation**: Add new entities to the scene
- **Entity Selection**: Click entities in hierarchy to view properties
- **Component Editing**: Transform, Render components with property fields
- **Animated Viewport**: Real-time preview with moving entities
- **Modern UI**: GTK3 with proper panels, toolbars, and responsive layout

### Engine Features
- **Component System**: Extensible ECS architecture
- **Spatial Partitioning**: Optimized collision detection using grid
- **Memory Tracking**: Debug memory allocations
- **Cross-Platform**: Linux, with potential for Windows/macOS

## Architecture

The editor integrates with the game engine by:
1. Sharing the core ECS system
2. Using the same memory management
3. Leveraging engine components for entity properties
4. Providing a visual interface for engine data

The GTK editor provides a professional game development environment while maintaining the lightweight nature of the core engine.