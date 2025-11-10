// NOTE: Menu preset enum removed - using Info Display system instead
// modded enum ChimeraMenuPreset
// {
// 	ATAK_DeviceMenu
// }

//------------------------------------------------------------------------------------------------
//! ATAK Device Menu - DEPRECATED: Using Info Display system (ATAK_DeviceDisplay) instead
// This class is kept for reference but is no longer used
class ATAK_DeviceMenu : ChimeraMenuBase
{
	static ATAK_DeviceMenu m_Instance;
	
	private World m_World;
	private Widget m_Root;
	
	// UI Elements
	private TextWidget m_GridDisplay;
	private TextWidget m_TimeDisplay;
	private TextWidget m_BatteryIcon;
	private TextWidget m_NorthCoordText;
	private TextWidget m_WestCoordText;
	private TextWidget m_AltitudeInfo;
	private TextWidget m_BearingInfo;
	
	// Buttons
	private ButtonWidget m_BtnMissions;
	private ButtonWidget m_BtnMap;
	private ButtonWidget m_BtnSystems;
	
	// Prevent multiple close calls
	private bool m_bClosing = false;
	private float m_fMenuOpenTime = 0;
	private const float MENU_OPEN_DELAY = 1.0; // Ignore N key for 1 second after opening
	private bool m_bConsumedOpeningTrigger = false; // Track if we've consumed the trigger that opened the menu
	
	//------------------------------------------------------------------------------------------------
	static ATAK_DeviceMenu GetInstance()
	{
		return m_Instance;
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnMenuInit()
	{
		super.OnMenuInit();
		m_Instance = this;
		m_World = GetGame().GetWorld();
		Print("[ATAK_Menu] Menu initialized");
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		// Reset closing flag and record open time
		m_bClosing = false;
		m_fMenuOpenTime = System.GetTickCount() / 1000.0;
		m_bConsumedOpeningTrigger = false; // Reset flag - we need to consume the trigger that opened the menu
		PrintFormat("[ATAK_Menu] Menu opened at time %1 - will ignore N key for %.1fs", m_fMenuOpenTime, MENU_OPEN_DELAY);
		
		// Set the action context for this menu (important for proper input handling)
		SetActionContext("ATAK_MenuContext");
		
		// Manually activate ATAK_MenuContext with indefinite duration
		InputManager inputMgr = GetGame().GetInputManager();
		if (inputMgr)
		{
			// -1 duration = stays active until manually deactivated
			inputMgr.ActivateContext("ATAK_MenuContext", -1);
			Print("[ATAK_Menu] ATAK_MenuContext activated (Priority 501, Flags 0 - non-blocking)");
			
			// CRITICAL: Consume the trigger that opened the menu immediately
			// This prevents GetActionTriggered from returning true in the first OnMenuUpdate call
			inputMgr.GetActionTriggered("ATAK_Toggle");
			m_bConsumedOpeningTrigger = true;
			Print("[ATAK_Menu] Consumed opening trigger immediately");
		}
		
		m_Root = GetRootWidget();
		if (!m_Root)
		{
			Print("[ATAK_Menu] ERROR: Root widget not found!", LogLevel.ERROR);
			return;
		}
		
		// Make sure root widget is visible and enabled
		m_Root.SetVisible(true);
		m_Root.SetEnabled(true);
		m_Root.SetOpacity(1.0);
		
		// Also ensure OverlayRoot is visible
		Widget overlayRoot = m_Root.FindAnyWidget("OverlayRoot");
		if (overlayRoot)
		{
			overlayRoot.SetVisible(true);
			overlayRoot.SetEnabled(true);
			overlayRoot.SetOpacity(1.0);
			Print("[ATAK_Menu] OverlayRoot found and made visible");
		}
		else
		{
			Print("[ATAK_Menu] WARNING: OverlayRoot widget not found!", LogLevel.WARNING);
		}
		
		Print(string.Format("[ATAK_Menu] Root widget: Name=%1, Visible=%2, Enabled=%3, Opacity=%4", 
			m_Root.GetName(), m_Root.IsVisible(), m_Root.IsEnabled(), m_Root.GetOpacity()));
		
		// Check screen position and size
		float screenX, screenY, screenW, screenH;
		m_Root.GetScreenPos(screenX, screenY);
		m_Root.GetScreenSize(screenW, screenH);
		PrintFormat("[ATAK_Menu] Screen position: X=%1, Y=%2, Width=%3, Height=%4", screenX, screenY, screenW, screenH);
		
		// Get workspace screen size for comparison
		float wsWidth, wsHeight;
		GetGame().GetWorkspace().GetScreenSize(wsWidth, wsHeight);
		PrintFormat("[ATAK_Menu] Workspace size: %1 x %2", wsWidth, wsHeight);
		
		// FORCE UPDATE - Widget size is 0 until UI updates
		m_Root.Update();
		GetGame().GetWorkspace().Update();
		
		// Check size AFTER update
		m_Root.GetScreenSize(screenW, screenH);
		PrintFormat("[ATAK_Menu] Screen size AFTER update: Width=%1, Height=%2", screenW, screenH);
		
		if (screenW == 0 || screenH == 0)
		{
			Print("[ATAK_Menu] WARNING: Widget still reports zero size, but layout has explicit pixel dimensions", LogLevel.WARNING);
		}
		else
		{
			Print("[ATAK_Menu] ✓ Widget sized successfully!");
		}
		
		// Find all UI widgets
		m_GridDisplay = TextWidget.Cast(m_Root.FindAnyWidget("GridDisplay"));
		m_TimeDisplay = TextWidget.Cast(m_Root.FindAnyWidget("TimeDisplay"));
		m_BatteryIcon = TextWidget.Cast(m_Root.FindAnyWidget("BatteryIcon"));
		m_NorthCoordText = TextWidget.Cast(m_Root.FindAnyWidget("NorthCoordText"));
		m_WestCoordText = TextWidget.Cast(m_Root.FindAnyWidget("WestCoordText"));
		m_AltitudeInfo = TextWidget.Cast(m_Root.FindAnyWidget("AltitudeInfo"));
		m_BearingInfo = TextWidget.Cast(m_Root.FindAnyWidget("BearingInfo"));
		
		// Find buttons
		m_BtnMissions = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnMissions"));
		m_BtnMap = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnMap"));
		m_BtnSystems = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnSystems"));
		
		// Wire up button handlers
		WireUpButtons();
		
        // Register ESC key handler for closing menu
        // NOTE: We handle N key in OnMenuUpdate to avoid conflicts with input component
        if (inputMgr)
        {
            inputMgr.AddActionListener("MenuBack", EActionTrigger.DOWN, OnCloseMenu);
            Print("[ATAK_Menu] ESC key listener registered for closing menu");
        }
        
        Print("[ATAK_Menu] Menu opened - UI visible with cursor!");
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnMenuUpdate(float tDelta)
	{
		super.OnMenuUpdate(tDelta);
		UpdateDeviceValues();
		
		// Don't allow closing immediately after opening
		float currentTime = System.GetTickCount() / 1000.0;
		float timeSinceOpen = currentTime - m_fMenuOpenTime;
		
		// Check delay - ignore ALL N key input for MENU_OPEN_DELAY seconds
		if (timeSinceOpen < MENU_OPEN_DELAY)
		{
			// Still in delay period - ignore all N key input
			return;
		}
		
		// Only check for N key if delay has passed and not already closing
		if (m_bClosing)
			return;
		
		// DISABLED: Input handling moved to ATAK_InputComponent (Info Display system)
		// Use GetActionTriggered which only returns true once per key press (consumes the trigger)
		// InputManager inputMgr = GetGame().GetInputManager();
		// if (inputMgr)
		// {
		// 	// GetActionTriggered returns true only once per key press (consumes the trigger)
		// 	if (inputMgr.GetActionTriggered("ATAK_Toggle"))
		// 	{
		// 		PrintFormat("[ATAK_Menu] N key triggered (%.2fs after open) - closing menu", timeSinceOpen);
		// 		OnCloseMenu(1.0, EActionTrigger.DOWN);
		// 	}
		// }
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnMenuClose()
	{
        Print("[ATAK_Menu] OnMenuClose called - cleaning up");
        
        // Clean up input listeners
        InputManager inputMgr = GetGame().GetInputManager();
        if (inputMgr)
        {
            inputMgr.RemoveActionListener("MenuBack", EActionTrigger.DOWN, OnCloseMenu);
            
            // Deactivate ATAK_MenuContext
            inputMgr.ActivateContext("ATAK_MenuContext", 0);
            Print("[ATAK_Menu] ATAK_MenuContext deactivated and listeners removed");
        }
        
		super.OnMenuClose();
		m_Instance = null;
        Print("[ATAK_Menu] Menu closed - controls restored");
    }
    
    //------------------------------------------------------------------------------------------------
    // Input handler for closing the menu (ESC and N keys)
    protected void OnCloseMenu(float value, EActionTrigger reason)
    {
        // Prevent multiple close calls
        if (m_bClosing)
        {
            Print("[ATAK_Menu] Already closing - ignoring duplicate close request");
            return;
        }
        
        // Log which action triggered this
        PrintFormat("[ATAK_Menu] Close menu triggered - value: %1, reason: %2", value, reason);
        
        // Only process DOWN events
        if (reason != EActionTrigger.DOWN)
    {
            PrintFormat("[ATAK_Menu] Ignoring non-DOWN trigger: %1", reason);
            return;
        }
        
        m_bClosing = true;
        Print("[ATAK_Menu] Closing menu...");
        
        // Note: No longer needed - we're using Info Display system now
        // Input component handles toggling directly through HUD Manager
        
        Close();
	}
	
	//------------------------------------------------------------------------------------------------
	// Wire up button click handlers
	private void WireUpButtons()
	{
		if (m_BtnMissions)
		{
			SCR_ModularButtonComponent comp = SCR_ModularButtonComponent.Cast(m_BtnMissions.FindHandler(SCR_ModularButtonComponent));
			if (comp)
				comp.m_OnClicked.Insert(OnMissionsClicked);
		}
		
		if (m_BtnMap)
		{
			SCR_ModularButtonComponent comp = SCR_ModularButtonComponent.Cast(m_BtnMap.FindHandler(SCR_ModularButtonComponent));
			if (comp)
				comp.m_OnClicked.Insert(OnMapClicked);
		}
		
		if (m_BtnSystems)
		{
			SCR_ModularButtonComponent comp = SCR_ModularButtonComponent.Cast(m_BtnSystems.FindHandler(SCR_ModularButtonComponent));
			if (comp)
				comp.m_OnClicked.Insert(OnSystemsClicked);
		}
		
		Print("[ATAK_Menu] Buttons wired up");
	}
	
	//------------------------------------------------------------------------------------------------
	// Update device display values
	private void UpdateDeviceValues()
	{
		if (!m_World) return;
		
		// Get player position
		IEntity player = SCR_PlayerController.GetLocalControlledEntity();
		if (!player) return;
		
		vector playerPos = player.GetOrigin();
		float altitude = playerPos[1];
		
		// Update grid coordinates using base game API (MGRS format)
		if (m_GridDisplay)
		{
			string gridCoords = SCR_MapEntity.GetGridLabel(playerPos, 2, 4, " ");
			if (gridCoords != "")
				m_GridDisplay.SetText(gridCoords);
		}
		
		// Update time display
		if (m_TimeDisplay)
		{
			float gameTime = m_World.GetWorldTime();
			float totalHours = gameTime / 3600000;
			float totalMinutes = gameTime / 60000;
			
			int hours = Math.Floor(totalHours - (Math.Floor(totalHours / 24) * 24));
			int minutes = Math.Floor(totalMinutes - (Math.Floor(totalMinutes / 60) * 60));
			
			string timeText = string.Format("%1:%2", hours, minutes);
			if (minutes < 10) timeText = string.Format("%1:0%2", hours, minutes);
			m_TimeDisplay.SetText(timeText);
		}
		
		// Update coordinates
		if (m_NorthCoordText)
		{
			float lat = playerPos[0];
			m_NorthCoordText.SetText(string.Format("N %1.2f", Math.AbsFloat(lat)));
		}
		
		if (m_WestCoordText)
		{
			float lon = playerPos[2];
			m_WestCoordText.SetText(string.Format("W %1.2f", Math.AbsFloat(lon)));
		}
		
		// Update altitude
		if (m_AltitudeInfo)
		{
			int altitudeFt = Math.Round(altitude * 3.28084);
			m_AltitudeInfo.SetText(string.Format("%1 ft AGL", altitudeFt));
		}
		
		// Update bearing
		if (m_BearingInfo)
		{
			float bearing = GetYaw();
			m_BearingInfo.SetText(string.Format("%1°M  0 MPH", Math.Round(bearing)));
		}
	}
	
	//------------------------------------------------------------------------------------------------
	// Get camera yaw
	private float GetYaw()
	{
		vector transform[4];
		m_World.GetCurrentCamera(transform);
		float yaw = Math3D.MatrixToAngles(transform)[0];
		
		if (yaw < 0) 
			yaw = 360 - Math.AbsFloat(yaw);
		
		return yaw;
	}
	
	//------------------------------------------------------------------------------------------------
	// Button handlers
	protected void OnMissionsClicked()
	{
		Print("[ATAK_Menu] MISSIONS tab clicked");
	}
	
	protected void OnMapClicked()
	{
		Print("[ATAK_Menu] MAP tab clicked");
	}
	
	protected void OnSystemsClicked()
	{
		Print("[ATAK_Menu] SYSTEMS tab clicked");
	}
}

