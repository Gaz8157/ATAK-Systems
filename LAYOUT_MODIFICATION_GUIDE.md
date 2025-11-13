# ATAK Systems - Layout Modification Guide

This guide provides step-by-step instructions for modifying the `ATAK_LayoutMain.layout` file in Arma Reforger Workbench to implement the UI enhancements based on research.

**Important:** Layout files (`.layout`) are binary files that must be edited in Arma Reforger Workbench Resource Manager, not in a text editor.

---

## Prerequisites

1. Arma Reforger Workbench installed
2. ATAK Systems project loaded in Workbench
3. Basic familiarity with Workbench Resource Manager and Layout Editor

---

## Phase 1: Add Enhanced Status Indicators

### 1.1 Add Zoom Level Display

**Widget to Add:** `ZoomLevel` (TextWidget)

**Location:** Near map controls (right side, below zoom buttons)

**Properties:**
```
Name: "ZoomLevel"
Position: X=700, Y=380
Size: 80x20
Font Size: 11
Color: 0.95 0.95 0.95 1 (white)
Text: "1:1000"
Horizontal Alignment: Right
Vertical Alignment: Center
```

**Steps in Workbench:**
1. Open `UI/Layouts/atak/ATAK_LayoutMain.layout`
2. Navigate to `OverlayRoot > ContentFrame`
3. Right-click `ContentFrame` → Add Widget → TextWidget
4. Name it `ZoomLevel`
5. Set position and size using slot properties
6. Set text properties as specified above

### 1.2 Add Connection Status

**Widget to Add:** `ConnectionStatus` (TextWidget)

**Location:** Top-right corner, near time display

**Properties:**
```
Name: "ConnectionStatus"
Position: X=620, Y=10
Size: 160x20
Font Size: 11
Color: 0 1 0 1 (green - online)
Text: "ONLINE"
Horizontal Alignment: Right
Vertical Alignment: Center
```

### 1.3 Add Battery Gauge (Visual)

**Widget to Add:** `BatteryGauge` (ImageWidget)

**Location:** Next to BatteryIcon text

**Properties:**
```
Name: "BatteryGauge"
Position: X=560, Y=35
Size: 40x15
Texture: (use existing battery texture or create simple rectangle)
Color: 0 1 0 1 (green)
Opacity: 1.0 (will be controlled by code based on percentage)
```

### 1.4 Add Signal Strength Indicator

**Widget to Add:** `SignalStrength` (ImageWidget)

**Location:** Top-left, near status box

**Properties:**
```
Name: "SignalStrength"
Position: X=20, Y=340
Size: 30x20
Texture: (use signal bars icon)
Color: 0 1 0 1 (green - good signal)
Opacity: 1.0
```

### 1.5 Add GPS Status Icon

**Widget to Add:** `GPSStatusIcon` (ImageWidget)

**Location:** Near GPS accuracy text in InfoBox

**Properties:**
```
Name: "GPSStatusIcon"
Position: X=220, Y=225 (inside InfoBox)
Size: 20x20
Texture: (use satellite icon)
Color: 0 1 0 1 (green - locked)
Opacity: 1.0
```

---

## Phase 2: Add Container Frames for Organization

### 2.1 Add HeaderBar Container

**Widget to Add:** `HeaderBar` (FrameWidget)

**Purpose:** Group tab buttons together

**Properties:**
```
Name: "HeaderBar"
Position: X=0, Y=0
Size: 800x60
Color: 0.15 0.15 0.15 0.5 (semi-transparent dark gray background)
Z Order: 50
```

**Steps:**
1. Add FrameWidget under `ContentFrame`
2. Move existing tab buttons (`BtnMissions`, `BtnMap`, `BtnSystems`) into `HeaderBar`
3. Adjust button positions relative to new parent

### 2.2 Add MainContent Container

**Widget to Add:** `MainContent` (FrameWidget)

**Purpose:** Container for all tab content

**Properties:**
```
Name: "MainContent"
Position: X=0, Y=60
Size: 800x330
Color: 0 0 0 0 (transparent)
Z Order: 60
```

**Steps:**
1. Add FrameWidget under `ContentFrame`
2. Move tab-specific content into this container
3. Tab switching will show/hide content within this frame

### 2.3 Add FooterBar Container

**Widget to Add:** `FooterBar` (FrameWidget)

**Purpose:** Group bottom controls

**Properties:**
```
Name: "FooterBar"
Position: X=0, Y=390
Size: 800x60
Color: 0.15 0.15 0.15 0.5 (semi-transparent dark gray background)
Z Order: 50
```

### 2.4 Add StatusOverlay Container

**Widget to Add:** `StatusOverlay` (FrameWidget)

**Purpose:** Group left-side status widgets

**Properties:**
```
Name: "StatusOverlay"
Position: X=0, Y=60
Size: 280x330
Color: 0 0 0 0 (transparent - layout only)
Z Order: 70
```

**Steps:**
1. Add FrameWidget under `ContentFrame`
2. Move `InfoBox` and related status widgets into this container
3. This provides logical grouping without visual change

### 2.5 Add ControlsOverlay Container

**Widget to Add:** `ControlsOverlay` (FrameWidget)

**Purpose:** Group right-side controls

**Properties:**
```
Name: "ControlsOverlay"
Position: X=680, Y=60
Size: 120x330
Color: 0 0 0 0 (transparent - layout only)
Z Order: 70
```

---

## Phase 3: Add Missions Content Area

### 3.1 Add MissionsContent Container

**Widget to Add:** `MissionsContent` (FrameWidget)

**Location:** Inside `MainContent` container

**Properties:**
```
Name: "MissionsContent"
Position: X=280, Y=0 (relative to MainContent)
Size: 400x330
Color: 0.1 0.1 0.1 0.3 (semi-transparent background)
Z Order: 61
Visible: False (will be shown when Missions tab is active)
```

### 3.2 Add ScrollMissions (Optional - Advanced)

**Widget to Add:** `ScrollMissions` (ScrollLayoutWidget)

**Purpose:** Enable scrolling for mission list

**Properties:**
```
Name: "ScrollMissions"
Parent: MissionsContent
Position: X=10, Y=10
Size: 380x310
Clipping: True
```

**Notes:**
- ScrollLayoutWidget is more complex to set up
- May require scrollbar widgets
- Can be implemented in future phase
- For now, use regular FrameWidget with fixed mission entries

### 3.3 Add Mission Entry Placeholders

**Widgets to Add:** `MissionEntry1`, `MissionEntry2`, `MissionEntry3` (FrameWidget)

**Purpose:** Individual mission display containers

**Properties for MissionEntry1:**
```
Name: "MissionEntry1"
Parent: MissionsContent (or ScrollMissions if implemented)
Position: X=10, Y=10
Size: 380x90
Color: 0.2 0.2 0.2 0.7 (dark gray background)
```

**Repeat for MissionEntry2 (Y=110) and MissionEntry3 (Y=210)**

### 3.4 Add Mission Text Widgets

**For each MissionEntry, add these child widgets:**

**MissionTitle:**
```
Name: "MissionTitle1" (2, 3 for other entries)
Parent: MissionEntry1 (or 2, 3)
Position: X=10, Y=10
Size: 360x20
Font Size: 14
Color: 0 1 1 1 (cyan - highlighted)
Text: "MISSION: ALPHA"
Horizontal Alignment: Left
```

**MissionStatus:**
```
Name: "MissionStatus1" (2, 3 for other entries)
Parent: MissionEntry1 (or 2, 3)
Position: X=10, Y=35
Size: 360x15
Font Size: 11
Color: 0 1 0 1 (green - active)
Text: "STATUS: ACTIVE"
Horizontal Alignment: Left
```

**MissionDescription:**
```
Name: "MissionDescription1" (2, 3 for other entries)
Parent: MissionEntry1 (or 2, 3)
Position: X=10, Y=55
Size: 360x30
Font Size: 10
Color: 0.9 0.9 0.9 1 (light gray)
Text: "Secure objective at grid 31U 12345 67890"
Horizontal Alignment: Left
Vertical Alignment: Top
```

---

## Phase 4: Visual Polish

### 4.1 Add Separator Lines

**Horizontal Separator (between header and content):**

**Widget:** `SeparatorTop` (FrameWidget)

**Properties:**
```
Name: "SeparatorTop"
Position: X=0, Y=59
Size: 800x2
Color: 0.5 0.5 0.5 1 (gray line)
Z Order: 80
```

**Horizontal Separator (between content and footer):**

**Widget:** `SeparatorBottom` (FrameWidget)

**Properties:**
```
Name: "SeparatorBottom"
Position: X=0, Y=389
Size: 800x2
Color: 0.5 0.5 0.5 1 (gray line)
Z Order: 80
```

### 4.2 Enhance Button Visual States

**For each tab button (BtnMissions, BtnMap, BtnSystems):**

1. Adjust normal color to: `0.2 0.2 0.2 0.8` ✓ (already set)
2. Add hover component if not present (SCR_ModularButtonComponent handles this)
3. Ensure text color is: `0.7 0.7 0.7 1` ✓ (already set)

**Active state colors are controlled by code in widget handler**

### 4.3 Add Padding to InfoBox

**Adjust InfoBox properties:**
```
Background Color: 0.1 0.1 0.1 0.7 (slightly more opaque)
```

**Adjust child widget positions to have consistent 10px margins**

---

## Phase 5: Testing Checklist

After making layout modifications:

- [ ] Load layout in Workbench Preview mode
- [ ] Verify all widgets are visible
- [ ] Check widget hierarchy in tree view
- [ ] Verify no overlapping widgets
- [ ] Check text readability
- [ ] Verify button positions and sizes
- [ ] Test with different screen resolutions (if possible)
- [ ] Save layout file
- [ ] Test in game via Workbench → Game → Play

---

## Common Issues and Solutions

### Widget Not Showing Up in Code

**Problem:** Added widget to layout but `FindAnyWidget()` returns null

**Solutions:**
1. Check widget name exactly matches (case-sensitive)
2. Ensure widget is under correct parent in hierarchy
3. Verify widget is not hidden (`Is Visible` = 1)
4. Try searching from root widget instead of sub-container

### Widget Positioned Incorrectly

**Problem:** Widget appears in wrong location

**Solutions:**
1. Check if parent uses anchors or absolute positioning
2. Verify OffsetLeft/Right and SizeX are consistent
3. Check Z Order (higher value = on top)
4. Ensure parent container is sized correctly

### Text Not Readable

**Problem:** Text is too small or wrong color

**Solutions:**
1. Increase font size (11-14 for body text)
2. Adjust text color for better contrast
3. Check parent background color
4. Ensure text widget is large enough for content

### Performance Issues

**Problem:** UI lags or FPS drops

**Solutions:**
1. Reduce number of widgets
2. Don't update hidden widgets
3. Use simpler textures
4. Disable debug prints in production

---

## Widget Naming Reference

### Status Indicators
- `ZoomLevel` - Zoom level display
- `ConnectionStatus` - Network connection status
- `BatteryGauge` - Visual battery indicator
- `SignalStrength` - Signal bars image
- `GPSStatusIcon` - GPS lock icon

### Containers
- `HeaderBar` - Top tab button container
- `MainContent` - Main content area container
- `FooterBar` - Bottom controls container
- `StatusOverlay` - Left status widgets container
- `ControlsOverlay` - Right controls container

### Mission Widgets
- `MissionsContent` - Missions tab content container
- `ScrollMissions` - Scrollable missions list (optional)
- `MissionEntry1-3` - Individual mission containers
- `MissionTitle1-3` - Mission name displays
- `MissionStatus1-3` - Mission status displays
- `MissionDescription1-3` - Mission description text

### Visual Elements
- `SeparatorTop` - Header separator line
- `SeparatorBottom` - Footer separator line

---

## Next Steps

1. **Start with Phase 1** - Add enhanced status indicators
2. **Test after each phase** - Don't add everything at once
3. **Save frequently** - Layout file corruption can happen
4. **Keep backups** - Copy `ATAK_LayoutMain.layout` before major changes
5. **Iterate** - Add features incrementally and test each one

---

## Additional Resources

- **Arma Reforger Layout Editor**: https://community.bistudio.com/wiki/Arma_Reforger:Resource_Manager:_Layout_Editor
- **Widget Reference**: See `ATAK_Widget_Guide.md` for existing widget details
- **UI Research**: See `ATAK_UI_Enhancement_Research.md` for design patterns

---

**Document Version:** 1.0  
**Last Updated:** 2025-11-13  
**For:** ATAK Systems UI Enhancement Project
