# ATAK Systems - AI Agent Guide

## Project Overview
**ATAK Systems** is an Arma Reforger mod implementing a tactical Android Team Awareness Kit (ATAK) device for in-game players. Built using Enfusion Engine's Enforce Script language, it provides a HUD overlay with GPS coordinates, compass, map display, and tactical information.

**Project ID**: `ATAKSystems` (GUID: `66A7337947673249`)  
**Engine**: Enfusion / Enforce Script (similar to C++ syntax, `.c` files)  
**Game**: Arma Reforger (dependency GUID: `58D0FB3206B6F859`)

## Architecture Overview

### Display System Architecture
The mod uses a **hybrid Menu + Info Display approach** for the interactive tactical device:
- `ATAK_DeviceMenu` (extends `ChimeraMenuBase`) - **Primary interactive menu** for cursor navigation
  - Real-time tactical data display (GPS, compass, coordinates, mission briefings)
  - Interactive controls (zoom, pan, tab switching, button clicks)
  - Multi-tab interface (Missions, Map, Systems)
  - Cursor/gamepad navigation support
- `ATAK_DeviceDisplay` (extends `SCR_InfoDisplayExtended`) - Alternative/experimental HUD overlay
- `ATAK_CompassDisplay` (extends `SCR_InfoDisplay`) - Standalone compass widget
- Registered in `DefaultPlayerController.et` via `SCR_HUDManagerComponent`

**Critical**: For an **interactive device with both data display and user interaction** (like BLE's loadout menu), use `ChimeraMenuBase`:
- Displays real-time information while allowing interaction
- Enables cursor/gamepad navigation
- Supports button clicks, input fields, and input focus
- Proper menu stacking with MenuManager
- Can disable player movement while open
- Updates display values in `OnMenuUpdate()` every frame

**Menu vs Info Display Decision**:
- **Use ChimeraMenuBase**: Interactive UI with data display, cursor navigation, button clicks, multi-tab navigation
- **Use SCR_InfoDisplay**: Passive HUD overlays, non-interactive real-time data (like compass only)

### Component Hierarchy
```
DefaultPlayerController.et (entity prefab)
├── SCR_HUDManagerComponent
│   ├── ATAK_DeviceDisplay (Info Display) ← Experimental HUD overlay
│   └── ATAK_CompassDisplay (Info Display) ← Standalone compass
└── ATAK_InputComponent (ScriptGameComponent) ← N key toggle handler

Menu System (via MenuManager):
└── ATAK_DeviceMenu (ChimeraMenuBase) ← Primary interactive device UI
```

### UI Widget System
Layouts use Enfusion's widget tree (`.layout` files with special syntax):
- `ATAK_LayoutMain.layout` - Main device UI (tactical overlay)
- `ATAK_LayoutSystems.layout` - Systems tab content
- Widgets: `FrameWidget`, `OverlayWidget`, `TextWidget`, `ButtonWidget`, `ImageWidget`
- Widget handlers: `ATAK_TacticalOverlayWidgetHandler` (extends `ScriptedWidgetComponent`)

**Widget Naming Convention**: CamelCase with descriptive names (`GridDisplay`, `BtnZoomIn`, `CompassIndicator`)

### Input Handling Pattern
**Two-layer approach** (mirrors BLE loadout menu and vPad patterns):
1. **ATAK_InputComponent** - ScriptGameComponent attached to player controller
   - Registers action listeners: `m_InputManager.AddActionListener("ATAK_Toggle", EActionTrigger.UP, OnToggleATAK)`
   - Opens `ATAK_DeviceMenu` via `MenuManager.OpenMenu(ChimeraMenuPreset.ATAK_DeviceMenu)`
   - Cooldown protection (0.5s) to prevent rapid toggling
   - **Input contexts**: `ATAK_MenuContext` with appropriate priority

2. **Menu Input Handling** - `ATAK_DeviceMenu.OnMenuOpen()`
   - Enables cursor for button clicks and navigation
   - Registers ESC/close handlers
   - Manages focus for keyboard/gamepad navigation
   - Closes via `Close()` method

**Critical Pattern**: Use `EActionTrigger.UP` with `InputFilterClick` (fires on key release, not press)

## Key Conventions

### Enforce Script Patterns
```enscript
// Component initialization
override void OnPostInit(IEntity owner)
{
    super.OnPostInit(owner);  // ALWAYS call super first
    // ... your init code
}

// Widget handler attachment
override protected void HandlerAttached(Widget w)
{
    m_RootWidget = w;
    // ... find widgets, connect events
}

// Info Display updates
override void DisplayUpdate(IEntity owner, float timeSlice)
{
    super.DisplayUpdate(owner, timeSlice);
    // ... update logic
}
```

### Logging Convention
Use `Print()` with prefixes: `Print("[ATAK_ClassName] Message")`  
Log levels: `Print("...", LogLevel.ERROR)`, `LogLevel.WARNING`

### Widget Finding Pattern
```enscript
// Find widget by name (searches entire tree)
TextWidget widget = TextWidget.Cast(m_RootWidget.FindAnyWidget("WidgetName"));

// Check for null before use
if (widget)
    widget.SetText("Value");
```

### CallQueue for Delayed Execution
```enscript
// One-time delayed call (100ms delay)
GetGame().GetCallqueue().CallLater(MethodName, 100, false);

// Repeating call (200ms interval)
GetGame().GetCallqueue().CallLater(UpdateMethod, 200, true);

// Remove repeating call
GetGame().GetCallqueue().Remove(UpdateMethod);
```

### Button Event Wiring
Uses `SCR_ModularButtonComponent` or `SCR_InputButtonComponent`:
```enscript
SCR_ModularButtonComponent comp = SCR_ModularButtonComponent.Cast(button.FindHandler(SCR_ModularButtonComponent));
if (comp)
    comp.m_OnClicked.Insert(OnButtonClicked);
```

## Critical Development Rules

### 1. Menu Lifecycle (ChimeraMenuBase)
- **OnMenuInit()**: Initialize references, set up singleton instance
- **OnMenuOpen()**: Create widgets, enable cursor, register input listeners
- **OnMenuUpdate()**: Update dynamic values every frame
- **OnMenuClose()**: Clean up listeners, restore player controls
- Always call `super.OnMethod()` first in override methods

### 2. Info Display Lifecycle (for HUD overlays)
- **DisplayStartDraw()**: Initialize input listeners, set up systems
- **DisplayUpdate()**: Update widget values every frame
- **DisplayStopDraw()**: Clean up listeners, remove CallQueue callbacks
- **Show(bool show)**: Override to sync visibility with Info Display system

### 2. Widget Visibility Management
```enscript
// Proper visibility toggle
widget.SetVisible(true);  // or widget.SetFlags(widget.GetFlags() | WidgetFlags.VISIBLE);

// Fade animations
AnimateWidget.AlphaMask(widget, targetAlpha, speed);
```

### 3. Coordinate System Usage
- World position: `vector playerPos = player.GetOrigin()` (meters)
- MGRS grid: `SCR_MapEntity.GetGridLabel(playerPos, precision, padding, separator)`
- Camera heading: `Math3D.MatrixToAngles(transform)[0]` (yaw in degrees)

### 4. Player Entity Access
```enscript
// Get local player
IEntity player = SCR_PlayerController.GetLocalControlledEntity();

// Get player controller
SCR_PlayerController controller = SCR_PlayerController.Cast(GetGame().GetPlayerController());
```

### 5. Math Helpers (No Modulo or Ternary Operators)
Enforce Script lacks `%` (modulo) and `? :` (ternary) operators - use manual loops and if/else:
```enscript
// Normalize to 0-360 (no modulo)
while (angle < 0) angle = angle + 360;
while (angle >= 360) angle = angle - 360;

// No ternary operator - use if/else instead
// BAD: int value = (condition) ? trueValue : falseValue;
// GOOD:
int value;
if (condition)
    value = trueValue;
else
    value = falseValue;
```

### 6. CallLater with Entity Safety (Prevent Use-After-Free)
**Never pass live IEntity pointers to CallLater** - entities may be deleted before the callback executes:
```enscript
// ❌ UNSAFE - causes UAF crashes
GetGame().GetCallqueue().CallLater(MyCallback, 100, false, myEntity);

// ✅ SAFE - use EntityID and resolve inside callback
static void MyCallback_ByID(EntityID entityId)
{
    IEntity entity = GetGame().GetWorld().FindEntityByID(entityId);
    if (!entity) return; // Entity may have been deleted
    
    // Now safe to use resolved entity
    MyOriginalLogic(entity);
}

// Usage:
GetGame().GetCallqueue().CallLater(MyCallback_ByID, 100, false, myEntity.GetID());
```

**Pattern**: Create `*_ByID` static helper functions that accept EntityID parameters and resolve entities inside the callback.

## File Organization

### Directory Structure
- `Scripts/Game/Components/ATAK/` - Component classes (UI, Map, Input)
- `Scripts/Game/UI/` - Info Displays and widget handlers
- `Scripts/Game/Systems/` - Game systems (deprecated ATAK_InputSystem)
- `UI/Layouts/atak/` - Widget layout files
- `Prefabs/Characters/Core/` - Entity prefabs with component configs
- `Configs/System/` - Input configuration files

### Naming Patterns
- Classes: `ATAK_ClassName` (prefix with `ATAK_`)
- Components: `ATAK_ComponentNameComponent` (suffix with `Component`)
- Displays: `ATAK_DisplayName` (extends Info Display classes)
- Handlers: `ATAK_HandlerName` (widget event handlers)

## Performance Best Practices

**Critical for multiplayer gameplay**:
- **Avoid per-frame heavy work**: Cache results, delay operations, spread across frames
- **Reduce raycast counts**: Prefer center + few offsets; spread extra rays across frames
- **Break large loops**: Use `CallLater()` to slice array processing across frames
- **Cache expensive results**: Menu presets, resource loads - invalidate on change
- **Stop early in searches**: Don't iterate entire collections when one result suffices
- **Minimize global calls**: Store `GetGame()` references locally in tight loops
- **Reuse objects**: Single `TraceParam` for repeated raycasts instead of allocating new ones
- **Gate diagnostics**: Use flags like `m_bEnableDiagnostics` to disable heavy logging in production

```enscript
// Example: Throttled diagnostics
protected bool m_bEnableDiagnostics = false;
protected float m_fLastDiagTime = 0;

void UpdateWithDiagnostics()
{
    // Only log every 2 seconds when diagnostics enabled
    float currentTime = System.GetTickCount() / 1000.0;
    if (m_bEnableDiagnostics && currentTime - m_fLastDiagTime > 2.0)
    {
        Print("[ATAK_Component] Diagnostic info");
        m_fLastDiagTime = currentTime;
    }
}
```

## Common Workflows

### Adding New Widget to Layout
1. Edit `.layout` file (binary format - use Workbench UI Editor)
2. Add widget with unique GUID and descriptive name
3. In widget handler: `m_Widget = WidgetType.Cast(m_Root.FindAnyWidget("WidgetName"))`
4. Wire up events if button: `comp.m_OnClicked.Insert(OnMethodName)`

### Implementing Network Synchronization
**For Missions Tab** (see reference: Tactical-Data-Link RPC patterns):
1. Create component with `RplComponent` attached
2. Add `[RplRpc]` attribute to sync methods
3. Use `Rpc()` to call methods across network
4. Server broadcasts mission updates to all clients

```enscript
// Example: Mission briefing sync
[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
void RPC_UpdateMissionBriefing(string briefing)
{
    // All clients receive and display briefing
    UpdateMissionsTab(briefing);
}

// Server calls this when mission changes
void SetMissionBriefing(string briefing)
{
    if (Replication.IsServer())
        Rpc(RPC_UpdateMissionBriefing, briefing);
}
```

### Debugging Input Issues
1. Check `Print()` output in console (F1 in-game)
2. Verify action exists: `float val = inputMgr.GetActionValue("ATAK_Toggle")`
3. Check context activation: `inputMgr.IsContextActive("ATAK_MenuContext")`
4. Enable frame check fallback if listeners fail
5. Reference vPad mod for working input patterns

### Testing Changes
- **Workbench**: Load `addon.gproj`, use Workbench > Game > Play
- **Diag Menu**: Register debug commands in `ATAK_DiagMenuCommands` (currently manual only)
- **Console**: Direct script execution (limited without DiagMenu)
- **Multiplayer**: Test with multiple clients - host server and connect as client

## External Dependencies

### Base Game Systems
- `SCR_HUDManagerComponent` - HUD and Info Display management
- `SCR_PlayerController` - Player control and entity access
- `SCR_MapEntity` - Map coordinate conversion (MGRS)
- `SCR_InfoDisplayExtended` - Extended Info Display with Show/Hide animations
- `InputManager` - Input action system
- `ActionManager` - Action state queries (per-controller)

### Widget Components
- `SCR_ModularButtonComponent` - Button click events (`m_OnClicked`)
- `SCR_InputButtonComponent` - Input-driven button events (`m_OnActivated`)
- `ScriptedWidgetComponent` - Base for custom widget handlers

## Known Quirks

1. **InputFilterClick fires on key UP, not DOWN** - Use `EActionTrigger.UP` for toggle actions
2. **Info Display auto-shows after init** - Set `m_bIgnoreNextShow = true` initially
3. **Widget size reports 0 until Update()** - Call `widget.Update()` after creation
4. **No % or ?: operators** - Use manual loops for modulo, if/else for conditionals
5. **ActionManager per controller** - Get via `playerController.GetActionManager()`, not InputManager
6. **Context priority doesn't block lower priorities** - Use Flags for exclusive contexts
7. **FindAnyWidget may fail on freshly created widgets** - Try searching children manually
8. **Switch statements need default** - Always include default case or break to avoid parser errors
9. **Map iteration** - Use index-based loops, not iterators: `for (int i = 0; i < map.Count(); i++)`
10. **Maps with classes** - Use `ref map<string, ref T>` when storing class instances

## Documentation References

### Project Documentation
- Widget Guide: `ATAK_Widget_Guide.md` (comprehensive widget structure and templates)
- Input Config: `Configs/System/chimeraInputCommon.conf`
- Player Prefab: `Prefabs/Characters/Core/DefaultPlayerController.et`
- Main Layout: `UI/Layouts/atak/ATAK_LayoutMain.layout`

### API Documentation (Update 1.6)
- Enfusion API: `.github/docs/api/EnfusionScriptAPIPublic/` (Widget, Input, Core systems)
- Arma Reforger API: `.github/docs/api/ArmaReforgerScriptAPIPublic/` (Game-specific classes)
- Base Game Scripts: `.github/docs/BaseGameScripts/` (Vanilla implementation examples)

### Reference Mods (Pattern Examples)
- **BLE**: `.github/docs/reference-mods/BLE/` - **Interactive menu system** (Bacon_GunBuilderUI), cursor navigation, button handling
- **vPad**: `.github/docs/reference-mods/vpad/` - Input handling, HUD systems, RPC patterns
- **Tactical-Data-Link**: `.github/docs/reference-mods/Tactical-Data-Link-main/` - Network sync, tactical UI

**IMPORTANT**: Reference mods are for **learning patterns only** - do NOT:
- Copy their naming conventions (use `ATAK_` prefix consistently)
- Directly rip code verbatim (adapt patterns to ATAK's architecture)
- Use their class/variable names (maintain ATAK's naming style)
- Import their components wholesale (implement equivalent functionality in ATAK style)

**DO**: Study their implementation approaches, architectural patterns, and problem-solving techniques, then implement similar solutions using ATAK conventions.

**Key API Classes to Reference**:
- `BaseInfoDisplay` / `SCR_InfoDisplayExtended` - Info Display system
- `ChimeraMenuBase` - Interactive menu system (primary for ATAK)
- `InputManager` / `ActionManager` - Input handling
- `Widget` hierarchy - UI components (TextWidget, ButtonWidget, FrameWidget, etc.)
- `SCR_HUDManagerComponent` - HUD management
- `SCR_MapEntity` - Map coordinate conversion (MGRS)
- `World` - Terrain/world data access
- `RplComponent` - Replication/networking

### External Resources
- **Arma Reforger Wiki**: https://community.bistudio.com/wiki/Arma_Reforger - Official documentation, scripting guides, and API references
- AI agents can search the wiki for additional context on Enfusion engine features, scripting patterns, and best practices

**Essential Scripting References**:
- [Keywords](https://community.bistudio.com/wiki/Arma_Reforger:Scripting:_Keywords) - Enforce Script keywords and syntax
- [Operators](https://community.bistudio.com/wiki/Arma_Reforger:Scripting:_Operators) - Available operators (NOTE: no modulo `%` operator)
- [Values](https://community.bistudio.com/wiki/Arma_Reforger:Scripting:_Values) - Data types and value handling
- [Conventions](https://community.bistudio.com/wiki/Arma_Reforger:Scripting:_Conventions) - Official naming and coding conventions
- [Do's and Don'ts](https://community.bistudio.com/wiki/Arma_Reforger:Scripting:_Do's_and_Don'ts) - Best practices and pitfalls
- [OOP Basics](https://community.bistudio.com/wiki/Arma_Reforger:Object_Oriented_Programming_Basics) - Object-oriented programming fundamentals
- [OOP Advanced](https://community.bistudio.com/wiki/Arma_Reforger:Object_Oriented_Programming_Advanced_Usage) - Advanced OOP patterns and usage

## Migration Notes

**Menu System Architecture** (current approach):
- **Primary**: `ATAK_DeviceMenu` extending `ChimeraMenuBase` for interactive UI
- **Experimental**: `ATAK_DeviceDisplay` extending `SCR_InfoDisplayExtended` for HUD overlay testing
- **Reason**: Interactive device with cursor navigation requires full menu system (like BLE loadout menu)
- **Reference**: Study BLE's `Bacon_GunBuilderUI` for cursor navigation, button handling, and menu management patterns

## Planned Features & Roadmap

### Map Rendering (In Development)
`ATAK_MapComponent` will implement real-time terrain and satellite imagery rendering:
- Render world terrain data using `World` API
- Display satellite/topographical overlays
- Interactive map controls (pan, zoom, markers)
- Integration with `RenderTargetWidget` for efficient rendering

### Missions Tab (Planned)
Mission planning and briefing system for command coordination:
- **Purpose**: Chat-like area where command sets mission objectives
- **Use case**: Keep soldiers synchronized with current mission goals
- **Features**: Mission briefings, objectives, tactical directives
- **Implementation**: Text display widget with scrollable mission log

### Multiplayer Considerations
**This mod operates in multiplayer environments:**
- Info Displays are client-side only (no network sync needed for UI)
- Player position/coordinates are local to each client
- Mission briefings will require network synchronization (RPC or replicated components)
- Test with multiple clients to ensure UI doesn't interfere with network performance

**Network Synchronization Pattern** (see reference mods: vPad, Tactical-Data-Link):
```enscript
// Mark RPC method with attribute
[RplRpc(RplChannel.Reliable, RplRcver.Server)]
private void RPC_SendMissionData(string missionText)
{
    // Server receives mission data
}

// Call RPC from client
Rpc(RPC_SendMissionData, "Mission: Capture Objective Alpha");
```

**RPC Channels**:
- `RplChannel.Reliable` - Guaranteed delivery (use for mission data)
- `RplChannel.Unreliable` - Fast, no guarantee (use for position updates)

**RPC Receivers**:
- `RplRcver.Server` - Client to server
- `RplRcver.Broadcast` - Server to all clients
- `RplRcver.Owner` - Server to specific client owner

## Quick Start for New Features

1. **New widget in tactical overlay**: Edit `ATAK_LayoutMain.layout` in Workbench, add to `ATAK_TacticalOverlayWidgetHandler.InitializeWidgets()`
2. **New input action**: Add to `chimeraInputCommon.conf`, register listener in `ATAK_InputComponent`
3. **New tab content**: Create layout, add to `ATAK_UIComponent.EnsureSystemsLayout()` pattern
4. **Map rendering**: Extend `ATAK_MapComponent.UpdateMapDisplay()` to query `World` terrain/texture data
5. **Missions system**: Create networked component for mission briefing sync across clients

---

**Remember**: This is an Enfusion mod for multiplayer Arma Reforger. Always test in Workbench, check console logs with F1, and verify multiplayer behavior with multiple clients.
