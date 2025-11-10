# ATAK Tactical Overlay - Widget Configuration Guide

## Widget Structure Overview

Based on the tactical overlay, here's a comprehensive list of widgets you need:

### **Core Container Widgets**

1. **ATAK_Root** (FrameWidgetClass) - Main root container

    - Name: "ATAK_Root"
    - Contains: OverlayRoot

2. **OverlayRoot** (OverlayWidgetClass) - Main overlay container

    - Name: "OverlayRoot"
    - Contains: All tactical widgets

3. **MapDisplay** (FrameWidgetClass) - Map background

    - Name: "MapDisplay"
    - Background color for map area

4. **MapContainer** (FrameWidgetClass) - Interactive map area
    - Name: "MapContainer"
    - For map interactions (drag, zoom, click)

---

### **Tactical Information Widgets** (Top-Left Area)

5. **TacticalStatusBox** (FrameWidgetClass) - Main info box container

    - Name: "TacticalStatusBox"
    - Background for all tactical info
    - Contains: All status widgets below

6. **StatusText** (TextWidgetClass) - Main status text

    - Name: "StatusText"
    - Displays: "STATUS: GREEN", "FATISSIAN ACTIVE", "RANGE LOOMBIS"
    - Multi-line format

7. **CallsignText** (TextWidgetClass) - Callsign display

    - Name: "CallsignText"
    - Displays: "Callsign: Schmidt"

8. **NorthCoordText** (TextWidgetClass) - North coordinate

    - Name: "NorthCoordText"
    - Displays: "N 1345.35.05f"

9. **WestCoordText** (TextWidgetClass) - West coordinate

    - Name: "WestCoordText"
    - Displays: "W 2941.6.05f"

10. **AltitudeText** (TextWidgetClass) - Altitude above ground

    - Name: "AltitudeText"
    - Displays: "125 ft AGL"

11. **BearingSpeedText** (TextWidgetClass) - Bearing and speed

    - Name: "BearingSpeedText"
    - Displays: "324°M 0 MPH"

12. **GPSAccuracyText** (TextWidgetClass) - GPS accuracy

    - Name: "GPSAccuracyText"
    - Displays: "+/-8m GPS ACC"

13. **RangeText** (TextWidgetClass) - Range information
    - Name: "RangeText"
    - Displays: "0000089RANGE" (or similar)

---

### **Compass & Navigation Widgets**

14. **CompassIndicator** (FrameWidgetClass) - Small compass with N indicator

    -   Name: "CompassIndicator"
    -   Red background with white N
    -   Contains number indicator (08)

15. **CompassNorthLabel** (TextWidgetClass) - Compass N label

    -   Name: "CompassNorthLabel"
    -   Displays: "N"

16. **CompassNumber** (TextWidgetClass) - Compass number
    -   Name: "CompassNumber"
    -   Displays: "08" (or current bearing)

---

### **Map Information Widgets**

17. **GridDisplay** (TextWidgetClass) - MGRS grid coordinates

    -   Name: "GridDisplay"
    -   Displays: Grid reference (e.g., "31U 12345 67890")

18. **ScaleBar** (TextWidgetClass) - Map scale
    -   Name: "ScaleBar"
    -   Displays: Scale (e.g., "1.33 mi")

---

### **Control Widgets**

19. **ZoomControls** (FrameWidgetClass) - Zoom button container

    -   Name: "ZoomControls"
    -   Contains: Zoom buttons

20. **BtnZoomIn** (ButtonWidgetClass) - Zoom in button

    -   Name: "BtnZoomIn"
    -   Text: "+"

21. **BtnZoomOut** (ButtonWidgetClass) - Zoom out button

    -   Name: "BtnZoomOut"
    -   Text: "−"

22. **BtnCenter** (ButtonWidgetClass) - Center view button

    -   Name: "BtnCenter"
    -   Icon: "⌂"

23. **BtnMenu** (ButtonWidgetClass) - Menu button

    -   Name: "BtnMenu"
    -   Icon: "☰"

24. **NavigationMenu** (FrameWidgetClass) - Navigation menu (hidden initially)

    -   Name: "NavigationMenu"
    -   "Is Visible": 0

25. **BtnCloseMenu** (ButtonWidgetClass) - Close menu button
    -   Name: "BtnCloseMenu"
    -   Text: "✕"

---

### **Additional Display Widgets**

26. **TimeDisplay** (TextWidgetClass) - Current time

    -   Name: "TimeDisplay"
    -   Displays: "10:00"

27. **BatteryIcon** (TextWidgetClass) - Battery status

    -   Name: "BatteryIcon"
    -   Displays: "⚡ 100%"

28. **MissionIDDisplay** (TextWidgetClass) - Mission identifier

    -   Name: "MissionIDDisplay"
    -   Displays: Mission code (e.g., "012008")

29. **StatusDisplay** (TextWidgetClass) - System status

    -   Name: "StatusDisplay"
    -   Displays: "RADIO: ONLINE\nGPS: LOCKED\nMISSION: ACTIVE"

30. **FriendlyDisplay** (TextWidgetClass) - Friendly units info
    -   Name: "FriendlyDisplay"
    -   Displays: Friendly count and status

---

## Widget Configuration Template

### Text Widget Template:

```
TextWidgetClass "{UNIQUE_ID}" {
 Name "WidgetName"
 Slot FrameWidgetSlot "{SLOT_ID}" {
  Anchor 0.5 0.5 0.5 0.5
  OffsetLeft -X
  OffsetTop -Y
  SizeX WIDTH
  OffsetRight X
  SizeY HEIGHT
  OffsetBottom Y
 }
 Text "Default Text"
 "Font Size" 12
 "Horizontal Alignment" Left
 "Vertical Alignment" Top
 Color 1.0 1.0 1.0 1.0
}
```

### Frame Widget Template:

```
FrameWidgetClass "{UNIQUE_ID}" {
 Name "FrameName"
 Slot FrameWidgetSlot "{SLOT_ID}" {
  Anchor 0.5 0.5 0.5 0.5
  OffsetLeft -X
  OffsetTop -Y
  SizeX WIDTH
  OffsetRight X
  SizeY HEIGHT
  OffsetBottom Y
 }
 Color 0.1 0.1 0.1 0.9
 "Ignore Cursor" 0
 {
  // Child widgets here
 }
}
```

### Button Widget Template:

```
ButtonWidgetClass "{UNIQUE_ID}" {
 Name "ButtonName"
 Slot FrameWidgetSlot "{SLOT_ID}" {
  Anchor 0.5 0.5 0.5 0.5
  OffsetLeft -X
  OffsetTop -Y
  SizeX WIDTH
  OffsetRight X
  SizeY HEIGHT
  OffsetBottom Y
 }
 Color 0.2 0.2 0.2 0.9
 "Ignore Cursor" 0
 {
  TextWidgetClass "{TEXT_ID}" {
   Name "ButtonText"
   Slot FrameWidgetSlot "{TEXT_SLOT}" {
    Anchor 0.5 0.5 0.5 0.5
    // Position and size
   }
   Text "Button"
   "Font Size" 14
   "Horizontal Alignment" Center
   "Vertical Alignment" Center
   Color 1.0 1.0 1.0 1.0
  }
 }
}
```

---

## Recommended Layout Structure

```
ATAK_Root (FrameWidget)
 └── OverlayRoot (OverlayWidget)
     ├── MapDisplay (FrameWidget) - Map background
     ├── MapContainer (FrameWidget) - Interactive map
     ├── TacticalStatusBox (FrameWidget) - Main info box
     │   ├── StatusText (TextWidget)
     │   ├── CallsignText (TextWidget)
     │   ├── NorthCoordText (TextWidget)
     │   ├── WestCoordText (TextWidget)
     │   ├── AltitudeText (TextWidget)
     │   ├── BearingSpeedText (TextWidget)
     │   ├── GPSAccuracyText (TextWidget)
     │   └── RangeText (TextWidget)
     ├── CompassIndicator (FrameWidget)
     │   ├── CompassNorthLabel (TextWidget)
     │   └── CompassNumber (TextWidget)
     ├── GridDisplay (TextWidget)
     ├── ScaleBar (TextWidget)
     ├── TimeDisplay (TextWidget)
     ├── BatteryIcon (TextWidget)
     ├── MissionIDDisplay (TextWidget)
     ├── StatusDisplay (TextWidget)
     ├── FriendlyDisplay (TextWidget)
     ├── ZoomControls (FrameWidget)
     │   ├── BtnZoomIn (ButtonWidget)
     │   ├── BtnZoomOut (ButtonWidget)
     │   └── BtnCenter (ButtonWidget)
     ├── BtnMenu (ButtonWidget)
     └── NavigationMenu (FrameWidget)
         └── BtnCloseMenu (ButtonWidget)
```

---

## Next Steps

1. Review this widget list
2. Let me know which widgets you want to add/modify
3. I'll help you set up the layout structure with proper positioning
4. We can iterate on the design together

