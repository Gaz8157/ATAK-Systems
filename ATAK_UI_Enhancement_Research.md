# ATAK Systems UI Enhancement Research

**Date:** 2025-11-13  
**Purpose:** Comprehensive research on Arma Reforger UI best practices and reference mods to enhance ATAK Systems main UI layout

---

## 1. Executive Summary

This document compiles findings from:
- Arma Reforger UI Wiki and official documentation
- BLE (Bacon Loadout Editor) - Interactive menu system reference
- vPad - HUD and input handling reference
- Tactical Data Link - Network sync and tactical UI reference

### Key Findings:
1. **Widget Organization**: Use clear hierarchical structures with descriptive naming
2. **Layout Patterns**: Grid-based layouts with ScrollLayoutWidget for scrollable content
3. **Button Components**: SCR_ModularButtonComponent and SCR_InputButtonComponent patterns
4. **Visual Feedback**: Opacity and color changes for active/inactive states
5. **Performance**: Minimize per-frame operations, cache widget references

---

## 2. Arma Reforger UI Best Practices

### 2.1 Widget Types and Usage

**FrameWidget** - Container for organizing groups of widgets
- Use for logical sections (status box, control panels, tab content)
- Set background colors for visual grouping: `Color 0.1 0.1 0.1 0.9`
- Can be transparent: `Color 0 0 0 0` for layout-only containers

**OverlayWidget** - Layer widgets over other elements
- Ideal for HUD overlays and device screens
- Use with proper z-ordering: `"Z Order" 1` to `"Z Order" 100`
- Set alignment: `HorizontalAlign 3` (center), `VerticalAlign 3` (center)

**TextWidget** - Display text content
- Font sizes: 10-16 for body text, 18-24 for headers
- Alignment options: Left, Center, Right (horizontal), Top, Center, Bottom (vertical)
- Color format: `Color R G B A` (0.0-1.0 range)
- Common colors: White `1 1 1 1`, Gray `0.7 0.7 0.7 1`, Green `0 1 0 1`

**ImageWidget** - Display textures and icons
- Texture path format: `"{GUID}UI/Textures/filename.edds"`
- Size in pixels: `Size 800 450`
- Tiling options: None, Horizontal, Vertical, Both
- Blend modes: Blend (standard), Add, Multiply

**ButtonWidget** - Interactive buttons
- Must have child TextWidget for label
- Components: `SCR_ModularButtonComponent` or `SCR_InputButtonComponent`
- Focus behavior: `m_bFocusOnMouseEnter 1` for hover highlighting
- Color states: Normal (0.2 0.2 0.2 0.8), Hover (brighter), Pressed (darker)

**ScrollLayoutWidget** - Scrollable content areas
- Single-direction scrolling recommended: vertical OR horizontal, not both
- Set scroll amount: `scroll_amount_y` for vertical, `scroll_amount_x` for horizontal
- Include scrollbar widgets for user feedback

### 2.2 Layout Positioning

**Anchor-based Positioning**
```
Slot FrameWidgetSlot "{SLOT_ID}" {
  Anchor 0.5 0.5 0.5 0.5  // Center anchor point
  OffsetLeft -400          // Left offset from anchor
  OffsetTop -225           // Top offset from anchor
  SizeX 800                // Width
  OffsetRight 400          // Right offset (should match OffsetLeft + SizeX)
  SizeY 450                // Height
  OffsetBottom 225         // Bottom offset (should match OffsetTop + SizeY)
  Alignment 0.5 0.5        // Widget's own alignment point
}
```

**Anchor Values:**
- `0 0 0 0` - Top-left corner
- `0.5 0.5 0.5 0.5` - Center (most common for fullscreen overlays)
- `1 1 1 1` - Bottom-right corner

**Absolute Positioning** (for fixed-position elements):
```
Slot FrameWidgetSlot "{SLOT_ID}" {
  PositionX 50
  OffsetLeft 50
  PositionY 10
  OffsetTop 10
  SizeX 150
  OffsetRight -200
  SizeY 40
  OffsetBottom -50
}
```

### 2.3 Widget Naming Conventions

**Recommended Prefixes:**
- `Btn` - ButtonWidget (BtnMissions, BtnZoomIn)
- `Txt` - TextWidget (TxtCallsign, TxtAltitude)
- `Img` - ImageWidget (ImgBackground, ImgIcon)
- `Frame` - FrameWidget (FrameContent, FrameStatus)
- `Scroll` - ScrollLayoutWidget (ScrollMissions, ScrollList)

**Descriptive Names:**
- Use clear, self-documenting names: `GridDisplay`, `CompassIndicator`, `BatteryIcon`
- Avoid generic names: `Widget1`, `Text2`, `Button3`
- Group related widgets: `TacticalData1`, `TacticalData2`, `TacticalData3`

---

## 3. BLE Reference Patterns

### 3.1 Menu Structure

BLE uses `ChimeraMenuBase` for interactive menu system with these key features:

**Menu Lifecycle:**
```enscript
override void OnMenuInit()
{
    super.OnMenuInit();
    m_Instance = this;
    // Initialize references
}

override void OnMenuOpen()
{
    super.OnMenuOpen();
    m_Root = GetRootWidget();
    // Find widgets, wire up events
    // Enable cursor for interaction
}

override void OnMenuUpdate(float tDelta)
{
    super.OnMenuUpdate(tDelta);
    // Update dynamic values (time, position, etc.)
    // Update component states
}

override void OnMenuClose()
{
    // Clean up listeners
    // Deactivate contexts
    super.OnMenuClose();
    m_Instance = null;
}
```

### 3.2 Button Event Wiring

BLE uses `SCR_InputButtonComponent.GetInputButtonComponent()` helper:
```enscript
Widget footer = m_root.FindAnyWidget("Footer");
m_swapButton = SCR_InputButtonComponent.GetInputButtonComponent("SwapButton", footer);
m_editButton = SCR_InputButtonComponent.GetInputButtonComponent("EditButton", footer);

// Connect events
m_swapButton.m_OnActivated.Insert(OnButtonPressed_Swap);
m_editButton.m_OnActivated.Insert(OnButtonPressed_Edit);
```

### 3.3 UI State Management

**Waiting/Loading States:**
```enscript
void SetUIWaiting(bool state)
{
    m_bIsActionInProgress = state;
    
    // Disable/enable widgets based on state
    m_wSlotChoices.SetEnabled(!state);
    m_SlotCategoryWidgetComponent.SetEnabled(!state);
    m_swapButton.SetEnabled(!state);
    m_editButton.SetEnabled(!state);
}
```

**Visual Feedback:**
- Disabled widgets: Lower opacity (0.5-0.7)
- Active tab: Higher opacity (1.0), bright color
- Inactive tab: Lower opacity (0.7), gray color

### 3.4 Component Architecture

BLE separates UI into specialized components:
- `GunBuilderUI_PreviewUIComponent` - 3D preview rendering
- `GunBuilderUI_CategoryButtonsUIComponent` - Category selection
- `GunBuilderUI_InventoryPanelUIComponent` - Inventory management
- `GunBuilderUI_RankAndSupplyInfoComponent` - Status display

**Key Pattern:**
```enscript
// Component attached to widget via components array in layout
m_wPreviewWidgetComponent = GunBuilderUI_PreviewUIComponent.Cast(
    m_root.FindAnyWidget("ItemPreviewContainer").FindHandler(GunBuilderUI_PreviewUIComponent)
);

// Connect component events
m_SlotCategoryWidgetComponent.m_OnCategoryChangedInvoker.Insert(OnSlotCategoryChanged);
m_InventoryPanelWidgetComponent.m_OnItemClicked.Insert(OnItemAddRequested);
```

---

## 4. Widget Handler Best Practices

### 4.1 ScriptedWidgetComponent Pattern

From ATAK's current implementation and BLE reference:

**Initialization:**
```enscript
class ATAK_TacticalOverlayWidgetHandler : ScriptedWidgetComponent
{
    protected Widget m_RootWidget;
    protected TextWidget m_StatusText;
    // ... other widget references
    
    override protected void HandlerAttached(Widget w)
    {
        m_RootWidget = w.FindAnyWidget("OverlayRoot");
        InitializeWidgets();
        ConnectEvents();
        UpdateDisplay();
        m_bInitialized = true;
    }
}
```

**Widget Finding:**
```enscript
protected void InitializeWidgets()
{
    // Find container first, then children
    FrameWidget infoBox = FrameWidget.Cast(m_RootWidget.FindAnyWidget("InfoBox"));
    if (infoBox)
    {
        m_StatusText = TextWidget.Cast(infoBox.FindAnyWidget("StatusText"));
        m_NorthCoordText = TextWidget.Cast(infoBox.FindAnyWidget("NorthCoordText"));
    }
    
    // Direct finding for top-level widgets
    m_GridDisplay = TextWidget.Cast(m_RootWidget.FindAnyWidget("GridDisplay"));
}
```

### 4.2 Event Connection

**SCR_InputButtonComponent** (preferred for buttons):
```enscript
protected void ConnectEvents()
{
    if (m_BtnZoomIn)
    {
        SCR_InputButtonComponent inputButton = SCR_InputButtonComponent.Cast(
            m_BtnZoomIn.FindHandler(SCR_InputButtonComponent)
        );
        if (inputButton)
            inputButton.m_OnActivated.Insert(OnZoomIn);
    }
}
```

**SCR_ModularButtonComponent** (alternative):
```enscript
SCR_ModularButtonComponent comp = SCR_ModularButtonComponent.Cast(
    m_BtnMissions.FindHandler(SCR_ModularButtonComponent)
);
if (comp)
    comp.m_OnClicked.Insert(OnMissionsClicked);
```

### 4.3 Dynamic Updates

**Per-Frame Updates:**
```enscript
void UpdateDisplay()
{
    if (!m_bInitialized) return;
    
    // Update time display
    if (m_TimeDisplay)
    {
        float gameTime = GetGame().GetWorld().GetWorldTime();
        float totalHours = gameTime / 3600000.0;
        int hours = Math.Floor(totalHours);
        // Calculate hours/minutes (manual modulo)
        hours = hours - (Math.Floor(hours / 24.0) * 24);
        if (hours < 0) hours = hours + 24;
        
        string timeStr = string.Format("%1:%2", hours, minutes);
        m_TimeDisplay.SetText(timeStr);
    }
}
```

**Performance Tip:** Don't update every widget every frame - only update what's changed or visible.

---

## 5. UI Enhancement Recommendations for ATAK

### 5.1 Layout Improvements

**Current Status:**
- Main layout: `ATAK_LayoutMain.layout` (800x450px device overlay)
- Widget handler: `ATAK_TacticalOverlayWidgetHandler`
- Tab system: Missions, Map, Systems tabs

**Recommended Enhancements:**

1. **Add ScrollLayoutWidget for Missions Tab**
   - Enable scrollable mission briefings
   - Show multiple missions with scroll capability
   - Add scrollbar indicators

2. **Improve Visual Hierarchy**
   - Group related widgets in FrameWidgets with subtle backgrounds
   - Add separator lines between sections
   - Use consistent spacing (10px margins, 5px padding)

3. **Enhanced Button Styling**
   - Add hover states with opacity changes
   - Visual feedback for button presses
   - Icons for common actions (zoom, center, menu)

4. **Status Indicators**
   - Color-coded status: Green (online), Yellow (warning), Red (error)
   - Animated indicators for active connections
   - Battery level with visual gauge

5. **Map Display Improvements**
   - Add grid overlay option
   - Scale indicator with multiple units (meters, miles, km)
   - Zoom level indicator (1:1000, 1:5000, etc.)

### 5.2 Widget Organization

**Proposed Structure:**
```
rootFrame (FrameWidget)
└── OverlayRoot (OverlayWidget)
    ├── DeviceBackground (ImageWidget) - Device texture
    ├── ContentFrame (FrameWidget) - Main content area
    │   ├── HeaderBar (FrameWidget) - Top bar
    │   │   ├── BtnMissions (ButtonWidget)
    │   │   ├── BtnMap (ButtonWidget)
    │   │   └── BtnSystems (ButtonWidget)
    │   ├── MainContent (FrameWidget) - Tab content area
    │   │   ├── MissionsContent (FrameWidget + ScrollLayoutWidget)
    │   │   ├── MapContent (FrameWidget)
    │   │   └── SystemsContent (FrameWidget)
    │   └── FooterBar (FrameWidget) - Bottom controls
    ├── StatusOverlay (FrameWidget) - Left side status
    │   ├── InfoBox (FrameWidget)
    │   ├── CompassIndicator (FrameWidget)
    │   └── GridDisplay (TextWidget)
    └── ControlsOverlay (FrameWidget) - Right side controls
        ├── ZoomControls (FrameWidget)
        └── NavigationMenu (FrameWidget)
```

### 5.3 New Widget Additions

**Status Panel Enhancement:**
- `BatteryGauge` (ImageWidget) - Visual battery level bar
- `SignalStrength` (ImageWidget) - Radio signal indicator
- `GPSStatusIcon` (ImageWidget) - GPS lock status icon
- `ConnectionStatus` (TextWidget) - Network connection state

**Mission Panel:**
- `ScrollMissions` (ScrollLayoutWidget) - Scrollable mission list
- `MissionEntry1-5` (FrameWidget) - Individual mission containers
- `MissionTitle` (TextWidget) - Mission name/code
- `MissionStatus` (TextWidget) - Mission state (Active, Complete, Failed)
- `MissionDescription` (TextWidget) - Mission details

**Map Controls:**
- `ZoomLevel` (TextWidget) - Current zoom level display
- `MapModeToggle` (ButtonWidget) - Switch between satellite/topo
- `MarkerControls` (FrameWidget) - Add/remove marker buttons

### 5.4 Color Scheme

**Tactical Device Theme:**
- Background: Dark gray `0.1 0.1 0.1 0.9`
- Primary text: White `1 1 1 1`
- Secondary text: Light gray `0.7 0.7 0.7 1`
- Active/highlighted: Green `0 1 0 1` or Cyan `0 1 1 1`
- Warning: Yellow `1 1 0 1`
- Error: Red `1 0 0 1`
- Disabled: Dark gray `0.5 0.5 0.5 0.7`

**Button States:**
- Normal: `Color 0.2 0.2 0.2 0.8`, Text `0.7 0.7 0.7 1`
- Hover: `Color 0.3 0.3 0.3 0.9`, Text `0.9 0.9 0.9 1`
- Active: `Color 0.4 0.4 0.4 1.0`, Text `1 1 1 1`
- Disabled: `Color 0.15 0.15 0.15 0.6`, Text `0.5 0.5 0.5 0.7`

---

## 6. Implementation Guidelines

### 6.1 Widget Component Pattern

When attaching components to widgets in layout:
```
FrameWidgetClass "{FRAME_ID}" {
  Name "FrameName"
  components {
    ATAK_CustomWidgetHandler "{HANDLER_ID}" {
      // Component properties
    }
  }
  {
    // Child widgets
  }
}
```

### 6.2 Button Component Pattern

For interactive buttons:
```
ButtonWidgetClass "{BTN_ID}" {
  Name "BtnName"
  Slot FrameWidgetSlot "{SLOT_ID}" {
    // Position and size
  }
  Color 0.2 0.2 0.2 0.8
  components {
    SCR_InputButtonComponent "{COMP_ID}" {
      m_bFocusOnMouseEnter 1
    }
    ButtonActionComponent "{ACTION_ID}" {
    }
  }
  {
    TextWidgetClass "{TEXT_ID}" {
      Name "BtnText"
      Text "LABEL"
      "Font Size" 14
      "Horizontal Alignment" Center
      "Vertical Alignment" Center
      Color 0.7 0.7 0.7 1
    }
  }
}
```

### 6.3 ScrollLayoutWidget Pattern

For scrollable content:
```
ScrollLayoutWidgetClass "{SCROLL_ID}" {
  Name "ScrollContent"
  Slot FrameWidgetSlot "{SLOT_ID}" {
    // Position and size
  }
  "Z Order" 10
  Clipping True
  {
    FrameWidgetClass "{CONTENT_FRAME}" {
      Name "ScrollableContent"
      {
        // Content widgets
      }
    }
  }
}
```

### 6.4 Performance Considerations

**Cache Widget References:**
```enscript
// Do this ONCE in initialization
m_GridDisplay = TextWidget.Cast(m_RootWidget.FindAnyWidget("GridDisplay"));

// Then reuse:
if (m_GridDisplay)
    m_GridDisplay.SetText(gridCoords);
```

**Minimize Per-Frame Updates:**
```enscript
void UpdateDisplay()
{
    // Only update what changes frequently
    UpdateTime();
    UpdatePlayerPosition();
    
    // Don't update static widgets every frame
}
```

**Use CallLater for Delayed Updates:**
```enscript
// Delay non-critical updates
GetGame().GetCallqueue().CallLater(UpdateMissionList, 1000, false);
```

---

## 7. Reference Sources

### Official Documentation
- **Arma Reforger Wiki**: https://community.bistudio.com/wiki/Arma_Reforger
  - Layout Creation: https://community.bistudio.com/wiki/Arma_Reforger:Layout_Creation
  - Widget Tooltip Setup: https://community.bistudio.com/wiki/Arma_Reforger:Widget_Tooltip_Setup
  - Resource Manager Layout Editor: https://community.bistudio.com/wiki/Arma_Reforger:Resource_Manager:_Layout_Editor

- **Enfusion Script API**: 
  - UI Group: https://community.bistudio.com/wikidata/external-data/arma-reforger/EnfusionScriptAPIPublic/group__UI.html
  - ScrollLayoutWidget: https://community.bistudio.com/wikidata/external-data/arma-reforger/EnfusionScriptAPIPublic/interfaceScrollLayoutWidget.html
  - SizeLayoutWidget: https://community.bistudio.com/wikidata/external-data/arma-reforger/EnfusionScriptAPIPublic/interfaceSizeLayoutWidget.html

- **Modding Boot Camp #4**: User Interface and HUD
  - https://reforger.armaplatform.com/news/modding-boot-camp-4-user-interface-and-hud
  - Video: https://www.youtube.com/watch?v=EE1iqCqrlq4

### Community Guides
- **Steam Community Guide**: How to create HUD/UI/GUI Addons
  - https://steamcommunity.com/sharedfiles/filedetails/?id=2814814735

- **GamePretty Guide**: Arma Reforger How to Create HUD/UI/GUI Addons
  - https://gamepretty.com/arma-reforger-how-to-create-hud-ui-gui-addons/

- **Arma Explorer**: Class references
  - ChimeraMenuBase: https://arexplorer.zeroy.com/class_chimera_menu_base.html

### Reference Mods (Local)
- **BLE** (Bacon Loadout Editor): `.github/docs/reference-mods/BLE/`
  - Interactive menu system with tabs, categories, preview
  - Complex button handling and state management
  - Multi-component architecture

- **vPad**: `.github/docs/reference-mods/vpad/`
  - HUD overlay systems
  - Input handling patterns
  - Minimap integration

- **Tactical Data Link**: `.github/docs/reference-mods/Tactical-Data-Link-main/`
  - Network synchronization
  - Tactical map markers
  - Device UI patterns

---

## 8. Next Steps

### Phase 1: Layout Enhancement
1. ✅ Complete research documentation
2. Add ScrollLayoutWidget to Missions tab
3. Improve visual hierarchy with container frames
4. Enhance button styling with better visual states
5. Add status indicators with color coding

### Phase 2: Widget Handler Enhancement
1. Implement cached widget references pattern
2. Add performance optimizations (throttled updates)
3. Improve tab switching with smooth transitions
4. Add scroll event handling

### Phase 3: New Features
1. Implement scrollable mission list
2. Add zoom level indicator
3. Create visual battery gauge
4. Add GPS/radio status icons
5. Implement map mode toggle

### Phase 4: Polish
1. Add hover effects to all interactive elements
2. Implement consistent color scheme
3. Add tooltips to buttons
4. Test in multiplayer environment
5. Document all changes in widget guide

---

## 9. Conclusion

This research provides a solid foundation for enhancing the ATAK Systems UI layout. Key takeaways:

1. **Structure matters**: Proper widget hierarchy and naming makes code maintainable
2. **Component architecture**: Separate concerns with specialized widget components
3. **Visual feedback**: Users need clear indication of interactive elements and states
4. **Performance**: Cache references, minimize per-frame updates, use CallLater wisely
5. **Reference patterns**: BLE demonstrates excellent menu architecture to learn from

The next phase involves implementing these findings into actual layout and code improvements, starting with the most impactful changes (scroll support, visual hierarchy) and progressing to polish and new features.

---

**Document Version:** 1.0  
**Last Updated:** 2025-11-13  
**Author:** GitHub Copilot Research Agent
