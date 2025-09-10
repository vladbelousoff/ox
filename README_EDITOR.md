# OX Game Engine Editor

A simple GTK-based editor for the OX Game Engine, built with C and GTK3.

## Features

- **Scene Management**: Create, save, and load game scenes
- **Entity System**: Add, remove, and manage game entities
- **Component System**: Support for Transform, Render, and Name components
- **Visual Editor**: Scene view with real-time rendering
- **Property Panel**: Edit entity properties
- **Hierarchy Panel**: Manage entity hierarchy
- **File Operations**: Save/load scenes in custom .oxs format

## Building

### Prerequisites

- CMake 4.0 or higher
- GTK3 development libraries
- Raylib 5.5
- C compiler with C17 support

### Dependencies

On Ubuntu/Debian:
```bash
sudo apt-get install libgtk-3-dev pkg-config cmake build-essential
```

On Fedora/RHEL:
```bash
sudo dnf install gtk3-devel pkgconfig cmake gcc
```

### Build Instructions

```bash
mkdir build
cd build
cmake ..
make
```

This will create two executables:
- `ox`: The original game engine demo
- `ox_editor`: The GTK-based editor

## Usage

### Running the Editor

```bash
./ox_editor
```

### Editor Interface

The editor consists of several panels:

1. **Menu Bar**: File operations (New, Open, Save, Save As)
2. **Toolbar**: Play/Stop controls
3. **Hierarchy Panel**: Entity management and creation
4. **Scene View**: Visual representation of the game scene
5. **Property Panel**: Edit selected entity properties
6. **Status Bar**: Shows current operation status

### Creating Entities

1. Click "Add Entity" in the Hierarchy panel
2. The entity will appear in the scene view
3. Select the entity to edit its properties

### Scene Operations

- **New Scene**: File → New Scene (clears current scene)
- **Open Scene**: File → Open Scene (loads .oxs file)
- **Save Scene**: File → Save Scene (saves to current file)
- **Save As**: File → Save Scene As... (saves to new file)

## Scene File Format

The editor uses a custom binary format (.oxs) for scene files:

- **Header**: Magic number, version, entity count
- **Entity Data**: Transform, Render, and Name components for each entity

## Architecture

### Core Components

- `ox_editor.h/c`: Main editor application and UI
- `ox_editor_scene.h/c`: Scene management and I/O
- `ox_editor_components.h/c`: ECS component definitions
- `main.c`: Application entry point

### Integration with Engine

The editor integrates with the existing OX engine systems:
- Uses the same ECS architecture
- Leverages the memory management system
- Integrates with the rendering system
- Maintains compatibility with the engine's component system

## Development

### Adding New Components

1. Define the component structure in `ox_editor_components.h`
2. Register the component in `ox_editor_components_register()`
3. Add serialization support in `ox_editor_scene.c`
4. Update the property panel to edit the component

### Extending the UI

The editor uses GTK3 widgets and follows a modular design:
- Each panel is created in its own function
- Callbacks are handled in the main editor file
- UI updates are managed through the editor state

## Limitations

- Basic ECS implementation (simplified for demonstration)
- Limited component types (Transform, Render, Name)
- No undo/redo system
- No asset management
- Scene view rendering is basic (no 3D support)

## Future Enhancements

- Full ECS implementation with proper archetypes
- More component types (Physics, Audio, etc.)
- Asset browser and management
- 3D scene view
- Undo/redo system
- Plugin system for custom components
- Scripting support
- Animation timeline
- Prefab system

## License

This editor is part of the OX Game Engine project.