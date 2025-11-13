# ATAK Systems

Advanced Tactical Awareness Kit (ATAK) device mod for Arma Reforger.

## Overview

ATAK Systems provides an interactive tactical device with real-time GPS coordinates, compass, map display, and mission coordination features for multiplayer gameplay.

**Project ID**: `ATAKSystems` (GUID: `66A7337947673249`)  
**Engine**: Enfusion / Enforce Script  
**Game**: Arma Reforger

## Features

- 📱 **Interactive Tactical Device** - Menu-based UI with cursor/gamepad navigation
- 🗺️ **Real-time Map Display** - GPS coordinates, MGRS grid reference, compass
- 📡 **Mission Coordination** - Mission briefings and tactical planning (planned)
- 🧭 **Compass System** - Standalone compass with bearing and direction
- 🎮 **Multi-tab Interface** - Missions, Map, and Systems tabs
- 🌐 **Multiplayer Ready** - Network synchronization for mission data
- 🔄 **Network Retry System** - Automatic retry with exponential backoff for failed RPC calls

## Quick Start

### Requirements
- Arma Reforger Workbench
- Basic knowledge of Enfusion scripting

### Installation
1. Clone or download this repository to your Arma Reforger addons folder
2. Open `addon.gproj` in Workbench
3. Compile the mod (Workbench > Game > Compile)

### Usage
- Press **N** key in-game to toggle ATAK device
- Use cursor/gamepad to navigate tabs and controls
- ESC to close the device

## Development

### Project Structure
```
ATAK Systems/
├── Scripts/          # Enforce Script source files
├── UI/Layouts/       # Widget layout files
├── Configs/          # Input and system configs
├── Prefabs/          # Entity prefabs
├── Assets/           # Models, textures, materials
└── .github/          # Documentation and guides
```

### Key Documentation
- **[Copilot Instructions](.github/copilot-instructions.md)** - Complete AI agent development guide
- **[Widget Guide](ATAK_Widget_Guide.md)** - UI widget structure and templates
- **[Network Retry Guide](ATAK_NetworkRetry_Guide.md)** - Network retry system documentation
- **API Documentation** - `.github/docs/api/` (Enfusion & Arma Reforger)
- **Reference Mods** - `.github/docs/reference-mods/` (BLE, vPad, Tactical-Data-Link)

### Building
Open in Workbench and use:
- **Compile**: Workbench > Game > Compile Mod
- **Test**: Workbench > Game > Play
- **Logs**: `%USERPROFILE%\Documents\My Games\ArmaReforgerWorkbench\logs\script.log`

### Contributing
See [.github/copilot-instructions.md](.github/copilot-instructions.md) for:
- Architecture overview
- Coding conventions
- Component patterns
- Input handling
- Networking patterns

## Architecture Highlights

### Display System
- **Primary**: `ATAK_DeviceMenu` (ChimeraMenuBase) - Interactive menu with real-time data display
- **Experimental**: `ATAK_DeviceDisplay` (SCR_InfoDisplayExtended) - HUD overlay testing
- **Compass**: `ATAK_CompassDisplay` (SCR_InfoDisplay) - Standalone compass widget

### Input Handling
- `ATAK_InputComponent` - N key toggle handler
- `ATAK_DeviceMenu` - Cursor/gamepad navigation
- Input action: `ATAK_Toggle` (keyboard:KC_N)

### Network Reliability
- `ATAK_NetworkRetryComponent` - Automatic retry with exponential backoff
- `ATAK_MissionSyncComponent` - Mission data synchronization with retry
- Configurable retry attempts, delays, and timeouts

### Planned Features
- **Map Rendering** - Real-time terrain and satellite imagery
- **Missions Tab** - Command coordination and mission briefings (components ready)
- **Network Sync** - Multiplayer mission data synchronization (implemented)

## Technical Notes

### Enforce Script Quirks
- No modulo operator (`%`) - use manual loops
- No ternary operator (`? :`) - use if/else blocks
- CallLater safety - use EntityID, not IEntity references

### Performance
- Optimized for multiplayer gameplay
- Throttled diagnostics and logging
- Efficient widget updates (OnMenuUpdate)

## License

[Add your license here]

## Credits

- **Developer**: [Your Name]
- **Engine**: Bohemia Interactive Enfusion
- **Game**: Arma Reforger

## Support

For issues, questions, or contributions, please refer to the documentation in `.github/copilot-instructions.md`.
