class ATAK_TacticalOverlayWidgetHandler : ScriptedWidgetComponent
{
    // Root widget reference
    protected Widget m_RootWidget;
    
    // Tactical Status Box widgets
    protected TextWidget m_StatusText;
    protected TextWidget m_CallsignText;
    protected TextWidget m_NorthCoordText;
    protected TextWidget m_WestCoordText;
    protected TextWidget m_AltitudeText;
    protected TextWidget m_BearingSpeedText;
    protected TextWidget m_GPSAccuracyText;
    protected TextWidget m_RangeText;
    
    // Compass widgets
    protected FrameWidget m_CompassIndicator;
    protected TextWidget m_CompassNorthLabel;
    protected TextWidget m_CompassNumber;
    protected TextWidget m_CompassDisplay;
    
    // Map widgets
    protected FrameWidget m_MapDisplay;
    protected FrameWidget m_MapContainer;
    protected TextWidget m_GridDisplay;
    protected TextWidget m_ScaleBar;
    
    // Display widgets
    protected TextWidget m_TimeDisplay;
    protected TextWidget m_BatteryIcon;
    protected TextWidget m_MissionIDDisplay;
    protected TextWidget m_StatusDisplay;
    protected TextWidget m_FriendlyDisplay;
    
    // Tactical overlay widgets
    protected TextWidget m_TacticalData1;
    protected TextWidget m_TacticalData2;
    protected TextWidget m_TacticalData3;
    protected TextWidget m_TacticalData4;
    protected TextWidget m_TacticalBearing;
    
    // Control widgets
    protected ButtonWidget m_BtnZoomIn;
    protected ButtonWidget m_BtnZoomOut;
    protected ButtonWidget m_BtnCenter;
    protected ButtonWidget m_BtnMenu;
    protected FrameWidget m_NavigationMenu;
    protected ButtonWidget m_BtnCloseMenu;
    
    // Tab buttons
    protected ButtonWidget m_BtnMissions;
    protected ButtonWidget m_BtnMap;
    protected ButtonWidget m_BtnSystems;
    protected TextWidget m_MissionsTabText;
    protected TextWidget m_MapTabText;
    protected TextWidget m_SystemsTabText;
    
    // Tab content widgets
    protected FrameWidget m_MapContent;
    protected FrameWidget m_SystemsContent;
    protected FrameWidget m_MissionsContent;
    
    // State
    protected bool m_bInitialized = false;
    protected bool m_bMenuVisible = false;
    protected int m_iActiveTab = 1; // 0=Missions, 1=Map, 2=Systems
    protected ATAK_UIComponent m_UIComponent;
    
    //------------------------------------------------------------------------------------------------
    // Widget handler attached - called automatically when component is attached to widget
    override protected void HandlerAttached(Widget w)
    {
        // The handler is attached to rootFrame, but we need OverlayRoot as the actual root
        m_RootWidget = w.FindAnyWidget("OverlayRoot");
        if (!m_RootWidget)
        {
            // Fallback: use the widget we're attached to
            m_RootWidget = w;
            Print("[ATAK_Handler] Warning: OverlayRoot not found, using rootFrame");
        }
        
        if (!m_RootWidget)
        {
            Print("[ATAK_Handler] Error: HandlerAttached received null/invalid widget", LogLevel.ERROR);
            return;
        }
            
        InitializeWidgets();
        ConnectEvents();
        UpdateDisplay();
        UpdateTabButtons(); // Initialize tab button states
        
        m_bInitialized = true;
        Print("[ATAK_Handler] Widget handler attached and initialized");
    }
    
    //------------------------------------------------------------------------------------------------
    // Public initialization method (for backward compatibility if needed)
    bool Initialize(Widget w)
    {
        HandlerAttached(w);
        return m_bInitialized;
    }
    
    //------------------------------------------------------------------------------------------------
    // Find and store all widget references
    protected void InitializeWidgets()
    {
        // Info Box (Tactical Status Box)
        FrameWidget infoBox = FrameWidget.Cast(m_RootWidget.FindAnyWidget("InfoBox"));
        if (infoBox)
        {
            m_StatusText = TextWidget.Cast(infoBox.FindAnyWidget("StatusText"));
            m_NorthCoordText = TextWidget.Cast(infoBox.FindAnyWidget("NorthCoordText"));
            m_WestCoordText = TextWidget.Cast(infoBox.FindAnyWidget("WestCoordText"));
            m_AltitudeText = TextWidget.Cast(infoBox.FindAnyWidget("AltitudeInfo"));
            m_BearingSpeedText = TextWidget.Cast(infoBox.FindAnyWidget("BearingInfo"));
            m_GPSAccuracyText = TextWidget.Cast(infoBox.FindAnyWidget("GPSAccuracyText"));
            m_RangeText = TextWidget.Cast(infoBox.FindAnyWidget("RangeText"));
        }
        
        // Callsign is now at bottom left, not in InfoBox
        m_CallsignText = TextWidget.Cast(m_RootWidget.FindAnyWidget("CallsignInfo"));
        
        // Compass Indicator
        m_CompassIndicator = FrameWidget.Cast(m_RootWidget.FindAnyWidget("CompassIndicator"));
        if (m_CompassIndicator)
        {
            m_CompassNorthLabel = TextWidget.Cast(m_CompassIndicator.FindAnyWidget("CompassNorthLabel"));
            m_CompassNumber = TextWidget.Cast(m_CompassIndicator.FindAnyWidget("CompassNumber"));
        }
        
        m_CompassDisplay = TextWidget.Cast(m_RootWidget.FindAnyWidget("CompassDisplay"));
        
        // Map widgets
        m_MapDisplay = FrameWidget.Cast(m_RootWidget.FindAnyWidget("MapDisplay"));
        m_MapContainer = FrameWidget.Cast(m_RootWidget.FindAnyWidget("MapContainer"));
        m_GridDisplay = TextWidget.Cast(m_RootWidget.FindAnyWidget("GridDisplay"));
        m_ScaleBar = TextWidget.Cast(m_RootWidget.FindAnyWidget("ScaleBar"));
        
        // Display widgets
        m_TimeDisplay = TextWidget.Cast(m_RootWidget.FindAnyWidget("TimeDisplay"));
        m_BatteryIcon = TextWidget.Cast(m_RootWidget.FindAnyWidget("BatteryIcon"));
        m_MissionIDDisplay = TextWidget.Cast(m_RootWidget.FindAnyWidget("MissionIDDisplay"));
        m_StatusDisplay = TextWidget.Cast(m_RootWidget.FindAnyWidget("StatusDisplay"));
        m_FriendlyDisplay = TextWidget.Cast(m_RootWidget.FindAnyWidget("FriendlyDisplay"));
        
        // Tactical overlay
        FrameWidget tacticalOverlay = FrameWidget.Cast(m_RootWidget.FindAnyWidget("TacticalOverlay"));
        if (tacticalOverlay)
        {
            m_TacticalData1 = TextWidget.Cast(tacticalOverlay.FindAnyWidget("TacticalData1"));
            m_TacticalData2 = TextWidget.Cast(tacticalOverlay.FindAnyWidget("TacticalData2"));
            m_TacticalData3 = TextWidget.Cast(tacticalOverlay.FindAnyWidget("TacticalData3"));
            m_TacticalData4 = TextWidget.Cast(tacticalOverlay.FindAnyWidget("TacticalData4"));
            m_TacticalBearing = TextWidget.Cast(tacticalOverlay.FindAnyWidget("TacticalBearing"));
        }
        
        // Control buttons
        m_BtnZoomIn = ButtonWidget.Cast(m_RootWidget.FindAnyWidget("BtnZoomIn"));
        m_BtnZoomOut = ButtonWidget.Cast(m_RootWidget.FindAnyWidget("BtnZoomOut"));
        m_BtnCenter = ButtonWidget.Cast(m_RootWidget.FindAnyWidget("BtnCenter"));
        m_BtnMenu = ButtonWidget.Cast(m_RootWidget.FindAnyWidget("BtnMenu"));
        m_NavigationMenu = FrameWidget.Cast(m_RootWidget.FindAnyWidget("NavigationMenu"));
        if (m_NavigationMenu)
            m_BtnCloseMenu = ButtonWidget.Cast(m_NavigationMenu.FindAnyWidget("BtnCloseMenu"));
        
        // Tab buttons
        m_BtnMissions = ButtonWidget.Cast(m_RootWidget.FindAnyWidget("BtnMissions"));
        m_BtnMap = ButtonWidget.Cast(m_RootWidget.FindAnyWidget("BtnMap"));
        m_BtnSystems = ButtonWidget.Cast(m_RootWidget.FindAnyWidget("BtnSystems"));
        
        // Tab text widgets
        if (m_BtnMissions)
            m_MissionsTabText = TextWidget.Cast(m_BtnMissions.FindAnyWidget("MissionsText"));
        if (m_BtnMap)
            m_MapTabText = TextWidget.Cast(m_BtnMap.FindAnyWidget("MapTabText"));
        if (m_BtnSystems)
            m_SystemsTabText = TextWidget.Cast(m_BtnSystems.FindAnyWidget("SystemsTabText"));
        
        // Tab content (use existing widgets as content)
        m_MapContent = m_MapDisplay;
        // Systems content will be from Systems layout
        // Missions content can be added later
            
        Print("[ATAK_Handler] Widgets initialized");
    }
    
    //------------------------------------------------------------------------------------------------
    // Connect button events using SCR_InputButtonComponent pattern
    protected void ConnectEvents()
    {
        // Connect zoom and control buttons
        if (m_BtnZoomIn)
        {
            SCR_InputButtonComponent inputButton = SCR_InputButtonComponent.Cast(m_BtnZoomIn.FindHandler(SCR_InputButtonComponent));
            if (inputButton)
                inputButton.m_OnActivated.Insert(OnZoomIn);
        }
        
        if (m_BtnZoomOut)
        {
            SCR_InputButtonComponent inputButton = SCR_InputButtonComponent.Cast(m_BtnZoomOut.FindHandler(SCR_InputButtonComponent));
            if (inputButton)
                inputButton.m_OnActivated.Insert(OnZoomOut);
        }
        
        if (m_BtnCenter)
        {
            SCR_InputButtonComponent inputButton = SCR_InputButtonComponent.Cast(m_BtnCenter.FindHandler(SCR_InputButtonComponent));
            if (inputButton)
                inputButton.m_OnActivated.Insert(OnCenter);
        }
        
        if (m_BtnMenu)
        {
            SCR_InputButtonComponent inputButton = SCR_InputButtonComponent.Cast(m_BtnMenu.FindHandler(SCR_InputButtonComponent));
            if (inputButton)
                inputButton.m_OnActivated.Insert(OnToggleMenu);
        }
        
        if (m_BtnCloseMenu)
        {
            SCR_InputButtonComponent inputButton = SCR_InputButtonComponent.Cast(m_BtnCloseMenu.FindHandler(SCR_InputButtonComponent));
            if (inputButton)
                inputButton.m_OnActivated.Insert(OnCloseMenu);
        }
        
        // Connect tab buttons
        if (m_BtnMissions)
        {
            SCR_InputButtonComponent inputButton = SCR_InputButtonComponent.Cast(m_BtnMissions.FindHandler(SCR_InputButtonComponent));
            if (inputButton)
                inputButton.m_OnActivated.Insert(OnTabMissions);
        }
        
        if (m_BtnMap)
        {
            SCR_InputButtonComponent inputButton = SCR_InputButtonComponent.Cast(m_BtnMap.FindHandler(SCR_InputButtonComponent));
            if (inputButton)
                inputButton.m_OnActivated.Insert(OnTabMap);
        }
        
        if (m_BtnSystems)
        {
            SCR_InputButtonComponent inputButton = SCR_InputButtonComponent.Cast(m_BtnSystems.FindHandler(SCR_InputButtonComponent));
            if (inputButton)
                inputButton.m_OnActivated.Insert(OnTabSystems);
        }
        
        Print("[ATAK_Handler] Events connected");
    }
    
    //------------------------------------------------------------------------------------------------
    // Button event handlers
    void OnZoomIn()
    {
        Print("[ATAK_Handler] Zoom In pressed");
        if (m_UIComponent)
            m_UIComponent.ZoomIn();
    }
    
    void OnZoomOut()
    {
        Print("[ATAK_Handler] Zoom Out pressed");
        if (m_UIComponent)
            m_UIComponent.ZoomOut();
    }
    
    void OnCenter()
    {
        Print("[ATAK_Handler] Center pressed");
        if (m_UIComponent)
            m_UIComponent.CenterView();
    }
    
    void OnToggleMenu()
    {
        m_bMenuVisible = !m_bMenuVisible;
        
        if (m_NavigationMenu)
            m_NavigationMenu.SetVisible(m_bMenuVisible);
        
        string status = "closed";
        if (m_bMenuVisible)
            status = "opened";
        PrintFormat("[ATAK_Handler] Menu %1", status);
        
        if (m_UIComponent)
            m_UIComponent.ToggleNavigationMenu();
    }
    
    void OnCloseMenu()
    {
        m_bMenuVisible = false;
        
        if (m_NavigationMenu)
            m_NavigationMenu.SetVisible(false);
            
        Print("[ATAK_Handler] Menu closed");
    }
    
    //------------------------------------------------------------------------------------------------
    // Tab button handlers
    void OnTabMissions()
    {
        Print("[ATAK_Handler] Missions tab pressed");
        SetActiveTab(0);
    }
    
    void OnTabMap()
    {
        Print("[ATAK_Handler] Map tab pressed");
        SetActiveTab(1);
    }
    
    void OnTabSystems()
    {
        Print("[ATAK_Handler] Systems tab pressed");
        SetActiveTab(2);
    }
    
    //------------------------------------------------------------------------------------------------
    // Switch active tab
    protected void SetActiveTab(int tabIndex)
    {
        m_iActiveTab = tabIndex;
        
        // Update button appearances
        UpdateTabButtons();
        
        // Show/hide content based on active tab
        if (m_MapContent)
        {
            if (tabIndex == 1) // Map tab
                m_MapContent.SetVisible(true);
            else
                m_MapContent.SetVisible(false);
        }
        
        PrintFormat("[ATAK_Handler] Tab switched to: %1 (0=Missions, 1=Map, 2=Systems)", tabIndex);
        
        // Notify UI component if needed
        if (m_UIComponent)
        {
            if (tabIndex == 2) // Systems tab
            {
                m_UIComponent.ShowSystemsLayout();
                Print("[ATAK_Handler] Systems layout shown");
            }
            else // Missions or Map tab
            {
                m_UIComponent.HideSystemsLayout();
                Print("[ATAK_Handler] Systems layout hidden");
            }
        }
        else
        {
            Print("[ATAK_Handler] Warning: UI Component not set - cannot toggle systems layout");
        }
    }
    
    //------------------------------------------------------------------------------------------------
    // Update tab button visual states
    protected void UpdateTabButtons()
    {
        // Update Missions button - active: brighter opacity, inactive: dimmer
        if (m_BtnMissions)
        {
            if (m_iActiveTab == 0)
                m_BtnMissions.SetOpacity(1.0);
            else
                m_BtnMissions.SetOpacity(0.7);
        }
        if (m_MissionsTabText)
        {
            if (m_iActiveTab == 0)
            {
                m_MissionsTabText.SetOpacity(1.0);
                m_MissionsTabText.SetColor(Color.White); // Active: white
            }
            else
            {
                m_MissionsTabText.SetOpacity(0.7);
                m_MissionsTabText.SetColor(new Color(0.7, 0.7, 0.7, 1.0)); // Inactive: gray
            }
        }
        
        // Update Map button - active: brighter opacity, inactive: dimmer
        if (m_BtnMap)
        {
            if (m_iActiveTab == 1)
                m_BtnMap.SetOpacity(1.0);
            else
                m_BtnMap.SetOpacity(0.7);
        }
        if (m_MapTabText)
        {
            if (m_iActiveTab == 1)
            {
                m_MapTabText.SetOpacity(1.0);
                m_MapTabText.SetColor(Color.White); // Active: white
            }
            else
            {
                m_MapTabText.SetOpacity(0.7);
                m_MapTabText.SetColor(new Color(0.7, 0.7, 0.7, 1.0)); // Inactive: gray
            }
        }
        
        // Update Systems button - active: brighter opacity, inactive: dimmer
        if (m_BtnSystems)
        {
            if (m_iActiveTab == 2)
                m_BtnSystems.SetOpacity(1.0);
            else
                m_BtnSystems.SetOpacity(0.7);
        }
        if (m_SystemsTabText)
        {
            if (m_iActiveTab == 2)
            {
                m_SystemsTabText.SetOpacity(1.0);
                m_SystemsTabText.SetColor(Color.White); // Active: white
            }
            else
            {
                m_SystemsTabText.SetOpacity(0.7);
                m_SystemsTabText.SetColor(new Color(0.7, 0.7, 0.7, 1.0)); // Inactive: gray
            }
        }
    }
    
    //------------------------------------------------------------------------------------------------
    // Update all display widgets with current data
    void UpdateDisplay()
    {
        if (!m_bInitialized)
            return;
            
        // Update tactical status box
        if (m_StatusText)
            m_StatusText.SetText("0000089RANGE LOOMBIS FATISSIAN ACTIVE\nSTATUS: GREEN");
            
        if (m_CallsignText)
            m_CallsignText.SetText("Callsign: Schmidt");
            
        if (m_NorthCoordText)
            m_NorthCoordText.SetText("N 1345.35.05f");
            
        if (m_WestCoordText)
            m_WestCoordText.SetText("W 2941.6.05f");
            
        if (m_AltitudeText)
            m_AltitudeText.SetText("125 ft AGL");
            
        if (m_BearingSpeedText)
            m_BearingSpeedText.SetText("324°M 0 MPH");
            
        if (m_GPSAccuracyText)
            m_GPSAccuracyText.SetText("+/-8m GPS ACC");
        
        // Update compass
        if (m_CompassNorthLabel)
            m_CompassNorthLabel.SetText("N");
            
        if (m_CompassNumber)
            m_CompassNumber.SetText("08");
            
        if (m_CompassDisplay)
            m_CompassDisplay.SetText("N 271°");
        
        // Update map info
        if (m_GridDisplay)
            m_GridDisplay.SetText("31U 12345 67890");
            
        if (m_ScaleBar)
            m_ScaleBar.SetText("1.33 mi");
        
        // Update time
        if (m_TimeDisplay)
        {
            float gameTime = GetGame().GetWorld().GetWorldTime();
            float totalHours = gameTime / 3600000.0;
            int hours = Math.Floor(totalHours);
            hours = hours - (Math.Floor(hours / 24.0) * 24);
            if (hours < 0) hours = hours + 24;
            
            float totalMinutes = gameTime / 60000.0;
            int minutes = Math.Floor(totalMinutes);
            minutes = minutes - (Math.Floor(minutes / 60.0) * 60);
            if (minutes < 0) minutes = minutes + 60;
            
            string timeStr = string.Format("%1:%2", hours, minutes);
            if (minutes < 10)
                timeStr = string.Format("%1:0%2", hours, minutes);
            m_TimeDisplay.SetText(timeStr);
        }
        
        // Update battery
        if (m_BatteryIcon)
            m_BatteryIcon.SetText("⚡ 100%");
        
        // Update mission ID
        if (m_MissionIDDisplay)
            m_MissionIDDisplay.SetText("012008");
        
        // Update status display
        if (m_StatusDisplay)
            m_StatusDisplay.SetText("RADIO: ONLINE\nGPS: LOCKED\nMISSION: ACTIVE");
        
        // Update friendly display
        if (m_FriendlyDisplay)
            m_FriendlyDisplay.SetText("FRIENDLIES: 3\nRANGE: 1.2KM\nSTATUS: ACTIVE");
        
        // Update tactical overlay
        if (m_TacticalData1)
            m_TacticalData1.SetText("012008");
            
        if (m_TacticalData2)
            m_TacticalData2.SetText("156");
            
        if (m_TacticalData3)
            m_TacticalData3.SetText("51m");
            
        if (m_TacticalData4)
            m_TacticalData4.SetText("78m");
            
        if (m_TacticalBearing)
            m_TacticalBearing.SetText("271° W");
    }
    
    //------------------------------------------------------------------------------------------------
    // Set reference to UI component
    void SetUIComponent(ATAK_UIComponent component)
    {
        m_UIComponent = component;
    }
    
    //------------------------------------------------------------------------------------------------
    // Update player position and heading
    void UpdatePlayerData(vector position, float heading, float altitude)
    {
        if (!m_bInitialized)
            return;
            
        // Update compass
        int bearing = Math.Round(heading);
        if (m_CompassNumber)
            m_CompassNumber.SetText(string.Format("%1", bearing));
            
        if (m_CompassDisplay)
        {
            string dir = GetCardinalDirection(heading);
            m_CompassDisplay.SetText(string.Format("%1 %2°", dir, bearing));
        }
        
        // Update bearing/speed
        if (m_BearingSpeedText)
            m_BearingSpeedText.SetText(string.Format("%1°M 0 MPH", bearing));
        
        // Update tactical bearing
        if (m_TacticalBearing)
        {
            string dir = GetCardinalDirection(heading);
            m_TacticalBearing.SetText(string.Format("%1° %2", bearing, dir));
        }
        
        // Update altitude
        if (m_AltitudeText)
        {
            int altFt = Math.Round(altitude * 3.28084);
            m_AltitudeText.SetText(string.Format("%1 ft AGL", altFt));
        }
    }
    
    //------------------------------------------------------------------------------------------------
    // Update coordinates
    void UpdateCoordinates(float lat, float lon)
    {
        if (!m_bInitialized)
            return;
            
        if (m_NorthCoordText)
            m_NorthCoordText.SetText(string.Format("N %1.05f°", Math.AbsFloat(lat)));
            
        if (m_WestCoordText)
            m_WestCoordText.SetText(string.Format("W %1.05f°", Math.AbsFloat(lon)));
    }
    
    //------------------------------------------------------------------------------------------------
    // Update grid coordinates
    void UpdateGrid(string gridRef)
    {
        if (!m_bInitialized)
            return;
            
        if (m_GridDisplay)
            m_GridDisplay.SetText(gridRef);
    }
    
    //------------------------------------------------------------------------------------------------
    // Helper: Get cardinal direction from heading
    protected string GetCardinalDirection(float heading)
    {
        int dir = Math.Round(heading / 45.0);
        // Manual modulo 8 without % operator
        while (dir < 0)
            dir = dir + 8;
        while (dir >= 8)
            dir = dir - 8;
        string directions[8] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};
        return directions[dir];
    }
}
