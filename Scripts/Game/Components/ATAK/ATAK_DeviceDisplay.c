[ComponentEditorProps(category: "ATAK", description: "ATAK Device Tactical Display")]
class ATAK_DeviceDisplay: SCR_InfoDisplayExtended
{
	
	// Static reference to current display instance
	static ATAK_DeviceDisplay s_CurrentDisplay;
	
	// Static getter for current display
	static ATAK_DeviceDisplay GetCurrent()
	{
		if (s_CurrentDisplay)
			return s_CurrentDisplay;
		return null;
	}
	
	//---------------------------------------------------------------------------
	private World world = GetGame().GetWorld();
	
	// ATAK UI Elements (from ATAK_TacticalOverlay.layout)
	private FrameWidget frameRootWidget;
	private OverlayWidget rootWidget;
	private TextWidget batteryIcon;
	private TextWidget timeDisplay;
	private TextWidget gridDisplay;
	private TextWidget callsignInfo;
	private TextWidget altitudeInfo;
	private TextWidget bearingInfo;
	private TextWidget northCoordText;
	private TextWidget westCoordText;
	private FrameWidget zoomInBtn;
	private FrameWidget zoomOutBtn;
	private ImageWidget deviceBackground;
	private FrameWidget mapDisplay;
	
	// Tab buttons
	private ButtonWidget btnMissions;
	private ButtonWidget btnMap;
	private ButtonWidget btnSystems;
	private ButtonWidget btnZoomIn;
	private ButtonWidget btnZoomOut;
	private ButtonWidget btnCenter;
	private ButtonWidget btnMenu;
	
	// State
	protected bool showATAK = false;
	private bool initialized = false;
	private bool m_bIgnoreNextShow = true; // Flag to ignore automatic show on initialization (starts true)
	
	// Input handling (like HCA_RadUI)
	protected InputManager m_InputManager;
	protected bool m_bInputListenersRegistered = false;
	protected bool m_bFrameCheckActive = false;
	protected float m_fLastToggleTime = 0;
	protected const float TOGGLE_COOLDOWN = 0.5;
	protected bool m_bProcessingToggle = false;
	protected bool m_bActionWasTriggeredLastFrame = false; // Track if action was triggered last frame to prevent multiple triggers
	protected float m_fLastActionValue = 0; // Track last action value for better edge detection
	
	//---------------------------------------------------------------------------
	override void DisplayUpdate(IEntity owner, float timeSlice)
	{
		super.DisplayUpdate(owner, timeSlice);
		
		if (!world) return;
		
		if (!initialized && m_wRoot) 
		{
			InitializeDevice();
		}
		
		if (showATAK && initialized)
		{
			UpdateDeviceValues();
		}
	}
	
	
	//---------------------------------------------------------------------------
	// Initialize ATAK device
	private void InitializeDevice()
	{
		// Check if already initialized to prevent recreation
		if (initialized)
		{
			Print("[ATAK_Device] Already initialized, skipping");
			return;
		}
		
		// The root is a FrameWidget containing an OverlayWidget
		frameRootWidget = FrameWidget.Cast(m_wRoot);
		if (!frameRootWidget)
		{
			Print("[ATAK_Device] Failed to cast root widget to FrameWidget", LogLevel.ERROR);
			return;
		}
		
		// Find the OverlayWidget inside - try both direct child and FindAnyWidget
		rootWidget = OverlayWidget.Cast(frameRootWidget.GetChildren());
		if (!rootWidget)
		{
			rootWidget = OverlayWidget.Cast(frameRootWidget.FindAnyWidget("OverlayRoot"));
		}
		
		if (!rootWidget)
		{
			Print("[ATAK_Device] Failed to find OverlayRoot widget", LogLevel.ERROR);
			// Try to list first child for debugging
			Widget child = frameRootWidget.GetChildren();
			if (child)
			{
				PrintFormat("[ATAK_Device] First child name: %1, type: %2", child.GetName(), child.Type().ToString());
			}
			return;
		}
		
		// Find device background image
		deviceBackground = ImageWidget.Cast(rootWidget.FindAnyWidget("DeviceBackground"));
		if (deviceBackground)
		{
			// Texture is set in layout file, just ensure it's visible
			deviceBackground.SetVisible(true);
		}
		
		// Find map display background
		mapDisplay = FrameWidget.Cast(rootWidget.FindAnyWidget("MapDisplay"));
		
		// Find widgets directly from root
		timeDisplay = TextWidget.Cast(rootWidget.FindAnyWidget("TimeDisplay"));
		batteryIcon = TextWidget.Cast(rootWidget.FindAnyWidget("BatteryIcon"));
		gridDisplay = TextWidget.Cast(rootWidget.FindAnyWidget("GridDisplay"));
		FrameWidget compassContainer = FrameWidget.Cast(rootWidget.FindAnyWidget("CompassContainer"));
		
		// Find info box widgets
		callsignInfo = TextWidget.Cast(rootWidget.FindAnyWidget("CallsignInfo"));
		northCoordText = TextWidget.Cast(rootWidget.FindAnyWidget("NorthCoordText"));
		westCoordText = TextWidget.Cast(rootWidget.FindAnyWidget("WestCoordText"));
		altitudeInfo = TextWidget.Cast(rootWidget.FindAnyWidget("AltitudeInfo"));
		bearingInfo = TextWidget.Cast(rootWidget.FindAnyWidget("BearingInfo"));
		
		// Find ContentFrame first (buttons are nested inside it)
		// ContentFrame is a FrameWidget inside OverlayRoot
		// Try multiple search methods to find it
		Widget contentFrameWidget = rootWidget.FindAnyWidget("ContentFrame");
		FrameWidget contentFrame = null;
		
		if (contentFrameWidget)
		{
			contentFrame = FrameWidget.Cast(contentFrameWidget);
		}
		
		// If not found with FindAnyWidget, try searching through children
		if (!contentFrame)
		{
			Widget child = rootWidget.GetChildren();
			while (child)
			{
				if (child.GetName() == "ContentFrame")
				{
					contentFrame = FrameWidget.Cast(child);
					break;
				}
				child = child.GetSibling();
			}
		}
		
		if (!contentFrame)
		{
			// List all children for debugging
			Widget child = rootWidget.GetChildren();
			int childCount = 0;
			while (child && childCount < 10)
			{
				PrintFormat("[ATAK_Device] Child %1: %2 (type: %3)", childCount, child.GetName(), child.Type().ToString());
				child = child.GetSibling();
				childCount++;
			}
		}
		
		// Find tab buttons (they're inside ContentFrame if found, otherwise search in rootWidget)
		if (contentFrame)
		{
			btnMissions = ButtonWidget.Cast(contentFrame.FindAnyWidget("BtnMissions"));
			btnMap = ButtonWidget.Cast(contentFrame.FindAnyWidget("BtnMap"));
			btnSystems = ButtonWidget.Cast(contentFrame.FindAnyWidget("BtnSystems"));
			
			// Find bottom control buttons (also inside ContentFrame)
			btnZoomIn = ButtonWidget.Cast(contentFrame.FindAnyWidget("BtnZoomIn"));
			btnZoomOut = ButtonWidget.Cast(contentFrame.FindAnyWidget("BtnZoomOut"));
			btnCenter = ButtonWidget.Cast(contentFrame.FindAnyWidget("BtnCenter"));
			btnMenu = ButtonWidget.Cast(contentFrame.FindAnyWidget("BtnMenu"));
		}
		else
		{
			// Fallback: search directly in rootWidget
			btnMissions = ButtonWidget.Cast(rootWidget.FindAnyWidget("BtnMissions"));
			btnMap = ButtonWidget.Cast(rootWidget.FindAnyWidget("BtnMap"));
			btnSystems = ButtonWidget.Cast(rootWidget.FindAnyWidget("BtnSystems"));
			btnZoomIn = ButtonWidget.Cast(rootWidget.FindAnyWidget("BtnZoomIn"));
			btnZoomOut = ButtonWidget.Cast(rootWidget.FindAnyWidget("BtnZoomOut"));
			btnCenter = ButtonWidget.Cast(rootWidget.FindAnyWidget("BtnCenter"));
			btnMenu = ButtonWidget.Cast(rootWidget.FindAnyWidget("BtnMenu"));
		}
		
		// Wire up button handlers
		WireUpButtons();
		
		// Hide initially - hide the FrameWidget root
		if (frameRootWidget)
		{
			frameRootWidget.SetVisible(false);
		}
		
		// Ensure Info Display starts hidden
		showATAK = false;
		super.Show(false);
		
		initialized = true;
		s_CurrentDisplay = this;
		
		// Set flag to ignore the next Show(true) call (which happens automatically after initialization)
		// Use a delayed call to set the flag, as the automatic show happens right after initialization
		GetGame().GetCallqueue().CallLater(EnableShowAfterInit, 100, false);
	}
	
	//---------------------------------------------------------------------------
	// Update device values
	private void UpdateDeviceValues()
	{
		if (!initialized) return;
		
		// Get player position for coordinates
		IEntity player = SCR_PlayerController.GetLocalControlledEntity();
		if (!player) return;
		
		vector playerPos = player.GetOrigin();
		float altitude = playerPos[1];
		float bearing = GetYaw();
		
		// Update time display
		if (timeDisplay)
		{
			float gameTime = GetGame().GetWorld().GetWorldTime();
			float totalHours = gameTime / 3600000;
			float totalMinutes = gameTime / 60000;
			
			// Calculate hours (0-23)
			float hoursFloat = totalHours - (Math.Floor(totalHours / 24) * 24);
			int hours = Math.Floor(hoursFloat);
			
			// Calculate minutes (0-59)
			float minutesFloat = totalMinutes - (Math.Floor(totalMinutes / 60) * 60);
			int minutes = Math.Floor(minutesFloat);
			
			string timeText = string.Format("%1:%2", hours, minutes);
			if (minutes < 10) timeText = string.Format("%1:0%2", hours, minutes);
			timeDisplay.SetText(timeText);
		}
		
		// Update grid coordinates using base game API (MGRS format)
		if (gridDisplay)
		{
			string gridCoords = SCR_MapEntity.GetGridLabel(playerPos, 2, 4, " ");
			if (gridCoords != "")
				gridDisplay.SetText(gridCoords);
		}
		
		// Update callsign (bottom left - just callsign, no coordinates)
		if (callsignInfo)
		{
			callsignInfo.SetText("Callsign: Schmidt");
		}
		
		// Update coordinates in InfoBox (detailed coordinates)
		if (northCoordText)
		{
			float lat = playerPos[0];
			northCoordText.SetText(string.Format("N %1.2f", Math.AbsFloat(lat)));
		}
		
		if (westCoordText)
		{
			float lon = playerPos[2];
			westCoordText.SetText(string.Format("W %1.2f", Math.AbsFloat(lon)));
		}
		
		// Update altitude and speed
		if (altitudeInfo)
		{
			// Convert altitude from meters to feet AGL
			int altitudeFt = Math.Round(altitude * 3.28084);
			string altitudeText = string.Format("%1 ft AGL", altitudeFt);
			altitudeInfo.SetText(altitudeText);
		}
		
		// Update bearing and speed
		if (bearingInfo)
		{
			string bearingText = string.Format("%1°M  0 MPH", Math.Round(bearing));
			bearingInfo.SetText(bearingText);
		}
	}
	
	//---------------------------------------------------------------------------
	// Get camera yaw angle (0-360 degrees)
	private float GetYaw()
	{
		vector transform[4];
		world.GetCurrentCamera(transform);
		float yaw = Math3D.MatrixToAngles(transform)[0];
		
		// Normalize to 0-360
		if (yaw < 0) 
		{
			yaw = 360 - Math.AbsFloat(yaw);
		}
		
		return yaw;
	}
	
	//---------------------------------------------------------------------------
	// Get cardinal direction from bearing
	private string GetCardinalDirection(float bearing)
	{
		float normalizedBearing = bearing;
		while (normalizedBearing < 0) normalizedBearing = normalizedBearing + 360;
		while (normalizedBearing >= 360) normalizedBearing = normalizedBearing - 360;
		
		int direction = Math.Round(normalizedBearing / 22.5);
		
		if (direction == 0 || direction == 16) return "N";
		if (direction == 1 || direction == 15) return "NNE";
		if (direction == 2 || direction == 14) return "NE";
		if (direction == 3 || direction == 13) return "ENE";
		if (direction == 4 || direction == 12) return "E";
		if (direction == 5 || direction == 11) return "ESE";
		if (direction == 6 || direction == 10) return "SE";
		if (direction == 7 || direction == 9) return "SSE";
		return "S";
	}
	
	//---------------------------------------------------------------------------
	// Animate widget fade in/out
	private void AnimateFade(Widget widget, bool fadeIn, float speed = 0.3)
	{
		if (!widget) return;
		
		float targetAlpha = 0.0;
		if (fadeIn)
			targetAlpha = 1.0;
		
		// Create alpha mask animation
		WidgetAnimationAlphaMask animation = AnimateWidget.AlphaMask(widget, targetAlpha, speed);
		if (animation)
		{
			AnimateWidget.AddAnimation(animation);
		}
	}
	
	//---------------------------------------------------------------------------
	void Toggle()
	{
		if (!initialized)
		{
			InitializeDevice();
		}
		
		showATAK = !showATAK;
		
		// Toggle visibility - use SetVisible for proper hide/show
		if (frameRootWidget)
		{
			frameRootWidget.SetVisible(showATAK);
			if (showATAK)
			{
				// Fade in when showing
				AnimateFade(frameRootWidget, true, 0.3);
				// Enable cursor and disable freelook
				EnableCursorMode(true);
			}
			else
			{
				// Cancel any animation and ensure it's hidden
				AnimateFade(frameRootWidget, false, 0.1);
				// Force hide after a short delay to ensure animation doesn't override
				GetGame().GetCallqueue().CallLater(ForceHideDevice, 100, false);
				// Disable cursor and re-enable freelook
				EnableCursorMode(false);
			}
		}
		
		// Toggle root widget
		if (rootWidget)
		{
			rootWidget.SetVisible(showATAK);
			if (showATAK)
			{
				AnimateFade(rootWidget, true, 0.3);
			}
			else
			{
				AnimateFade(rootWidget, false, 0.1);
			}
		}
		
		string state = "";
		if (showATAK)
			state = "ON";
		else
			state = "OFF";
	}
	
	//---------------------------------------------------------------------------
	// Enable/disable cursor mode and set focus for menu navigation
	protected void EnableCursorMode(bool enable)
	{
		if (enable)
		{
			// Enable cursor mode for menu navigation
			GetGame().GetWorkspace().SetFocusedWidget(rootWidget);
			
			// Focus the first button (BtnMissions) for keyboard navigation
			// Use a delayed call to ensure widgets are fully initialized (increased delay for Info Display system)
			GetGame().GetCallqueue().CallLater(FocusFirstButton, 200, false);
			
		}
		else
		{
			// Clear widget focus
			GetGame().GetWorkspace().SetFocusedWidget(null);
		}
	}
	
	//---------------------------------------------------------------------------
	// Focus the first button for keyboard navigation
	protected void FocusFirstButton()
	{
		// Try to find the button if it's not already cached
		if (!btnMissions)
		{
			if (rootWidget)
			{
				// Try ContentFrame first (buttons are nested inside it)
				Widget contentFrameWidget = rootWidget.FindAnyWidget("ContentFrame");
				FrameWidget contentFrame = null;
				
				if (contentFrameWidget)
				{
					contentFrame = FrameWidget.Cast(contentFrameWidget);
				}
				
				// If not found with FindAnyWidget, try searching through children
				if (!contentFrame)
				{
					Widget child = rootWidget.GetChildren();
					while (child)
					{
						if (child.GetName() == "ContentFrame")
						{
							contentFrame = FrameWidget.Cast(child);
							break;
						}
						child = child.GetSibling();
					}
				}
				
				if (contentFrame)
				{
					btnMissions = ButtonWidget.Cast(contentFrame.FindAnyWidget("BtnMissions"));
					if (!btnMissions)
					{
						// Try searching through children
						Widget child = contentFrame.GetChildren();
						while (child)
						{
							if (child.GetName() == "BtnMissions")
							{
								btnMissions = ButtonWidget.Cast(child);
								break;
							}
							child = child.GetSibling();
						}
					}
					PrintFormat("[ATAK_Device] Re-searching in ContentFrame for BtnMissions - found: %1", btnMissions != null);
				}
				
				// Fallback to rootWidget if not found
				if (!btnMissions)
				{
					btnMissions = ButtonWidget.Cast(rootWidget.FindAnyWidget("BtnMissions"));
					PrintFormat("[ATAK_Device] Re-searching in rootWidget for BtnMissions - found: %1", btnMissions != null);
				}
			}
		}
		
		if (!btnMissions)
		{
			Print("[ATAK_Device] WARNING: BtnMissions not found - cannot focus for navigation", LogLevel.WARNING);
			// Try to focus root widget instead
			if (rootWidget)
			{
				GetGame().GetWorkspace().SetFocusedWidget(rootWidget);
				Print("[ATAK_Device] Focused root widget instead");
			}
			return;
		}
		
		// Set focus on the first tab button for keyboard navigation
		GetGame().GetWorkspace().SetFocusedWidget(btnMissions);
	}
	
	//---------------------------------------------------------------------------
	// Force hide device to ensure it's actually hidden
	protected void ForceHideDevice()
	{
		if (!showATAK && frameRootWidget)
		{
			frameRootWidget.SetVisible(false);
			Print("[ATAK_Device] Force hidden device");
		}
		if (!showATAK && rootWidget)
		{
			rootWidget.SetVisible(false);
		}
	}
	
	//---------------------------------------------------------------------------
	// Enable Show() after initialization delay (to ignore automatic show)
	private void EnableShowAfterInit()
	{
		m_bIgnoreNextShow = false;
	}
	
	//---------------------------------------------------------------------------
	// Override Show() to properly sync with Info Display system
	override void Show(bool show, float speed = UIConstants.FADE_RATE_INSTANT, EAnimationCurve curve = EAnimationCurve.LINEAR)
	{
		// Ignore automatic show calls right after initialization
		if (m_bIgnoreNextShow && show)
		{
			// Still call super with false to ensure parent state is correct
			super.Show(false, speed, curve);
			return;
		}
		
		// Call parent first to handle Info Display system state
		super.Show(show, speed, curve);
		
		// Update our internal state
		showATAK = show;
		
		// Ensure widgets are initialized
		if (!initialized)
		{
			InitializeDevice();
		}
		
		// Update widget visibility
		if (frameRootWidget)
		{
			frameRootWidget.SetVisible(show);
			if (show)
			{
				AnimateFade(frameRootWidget, true, 0.3);
				EnableCursorMode(true);
			}
			else
			{
				AnimateFade(frameRootWidget, false, 0.1);
				EnableCursorMode(false);
				GetGame().GetCallqueue().CallLater(ForceHideDevice, 100, false);
			}
		}
		
		if (rootWidget)
		{
			rootWidget.SetVisible(show);
			if (show)
			{
				AnimateFade(rootWidget, true, 0.3);
			}
			else
			{
				AnimateFade(rootWidget, false, 0.1);
			}
		}
		
	}
	
	//---------------------------------------------------------------------------
	// Show ATAK device with fade animation
	void Show()
	{
		if (!initialized)
		{
			InitializeDevice();
		}
		
		showATAK = true;
		
		if (frameRootWidget)
		{
			frameRootWidget.SetFlags(frameRootWidget.GetFlags() | WidgetFlags.VISIBLE);
			AnimateFade(frameRootWidget, true, 0.3);
		}
		
		if (rootWidget)
		{
			rootWidget.SetFlags(rootWidget.GetFlags() | WidgetFlags.VISIBLE);
			AnimateFade(rootWidget, true, 0.3);
		}
		
		// Enable cursor mode
		EnableCursorMode(true);
	}
	
	//---------------------------------------------------------------------------
	// Hide ATAK device with fade animation
	void Hide()
	{
		showATAK = false;
		
		if (frameRootWidget)
		{
			// Hide immediately - don't wait for animation
			frameRootWidget.SetFlags(frameRootWidget.GetFlags() & ~WidgetFlags.VISIBLE);
			AnimateFade(frameRootWidget, false, 0.3);
		}
		
		if (rootWidget)
		{
			// Hide immediately - don't wait for animation
			rootWidget.SetFlags(rootWidget.GetFlags() & ~WidgetFlags.VISIBLE);
			AnimateFade(rootWidget, false, 0.3);
		}
		
		// Disable cursor mode
		EnableCursorMode(false);
		
		Print("[ATAK_Device] Device hidden");
	}
	
	//---------------------------------------------------------------------------
	// Check if device is visible
	bool IsVisible()
	{
		return showATAK && initialized && frameRootWidget && (frameRootWidget.GetFlags() & WidgetFlags.VISIBLE) != 0;
	}
	
	//---------------------------------------------------------------------------
	// Wire up button click handlers
	private void WireUpButtons()
	{
		// Get button components and connect handlers
		if (btnMissions)
		{
			SCR_ModularButtonComponent comp = SCR_ModularButtonComponent.Cast(btnMissions.FindHandler(SCR_ModularButtonComponent));
			if (comp)
				comp.m_OnClicked.Insert(OnMissionsClicked);
		}
		
		if (btnMap)
		{
			SCR_ModularButtonComponent comp = SCR_ModularButtonComponent.Cast(btnMap.FindHandler(SCR_ModularButtonComponent));
			if (comp)
				comp.m_OnClicked.Insert(OnMapClicked);
		}
		
		if (btnSystems)
		{
			SCR_ModularButtonComponent comp = SCR_ModularButtonComponent.Cast(btnSystems.FindHandler(SCR_ModularButtonComponent));
			if (comp)
				comp.m_OnClicked.Insert(OnSystemsClicked);
		}
		
		if (btnZoomIn)
		{
			SCR_ModularButtonComponent comp = SCR_ModularButtonComponent.Cast(btnZoomIn.FindHandler(SCR_ModularButtonComponent));
			if (comp)
				comp.m_OnClicked.Insert(OnZoomInClicked);
		}
		
		if (btnZoomOut)
		{
			SCR_ModularButtonComponent comp = SCR_ModularButtonComponent.Cast(btnZoomOut.FindHandler(SCR_ModularButtonComponent));
			if (comp)
				comp.m_OnClicked.Insert(OnZoomOutClicked);
		}
		
		if (btnCenter)
		{
			SCR_ModularButtonComponent comp = SCR_ModularButtonComponent.Cast(btnCenter.FindHandler(SCR_ModularButtonComponent));
			if (comp)
				comp.m_OnClicked.Insert(OnCenterClicked);
		}
		
		if (btnMenu)
		{
			SCR_ModularButtonComponent comp = SCR_ModularButtonComponent.Cast(btnMenu.FindHandler(SCR_ModularButtonComponent));
			if (comp)
				comp.m_OnClicked.Insert(OnMenuClicked);
		}
		
		Print("[ATAK_Device] Buttons wired up successfully");
	}
	
	//---------------------------------------------------------------------------
	// Button click handlers
	protected void OnMissionsClicked()
	{
		// TODO: Switch to missions view
	}
	
	protected void OnMapClicked()
	{
		// TODO: Switch to map view
	}
	
	protected void OnSystemsClicked()
	{
		// TODO: Switch to systems view
	}
	
	protected void OnZoomInClicked()
	{
		// TODO: Implement zoom in functionality
	}
	
	protected void OnZoomOutClicked()
	{
		// TODO: Implement zoom out functionality
	}
	
	protected void OnCenterClicked()
	{
		// TODO: Implement center view functionality
	}
	
	protected void OnMenuClicked()
	{
		// TODO: Open menu overlay
	}
	
	//---------------------------------------------------------------------------
	// Toggle navigation menu (placeholder for future implementation)
	void ToggleNavigationMenu()
	{
		// Placeholder - will be implemented with new tab system
	}

	//---------------------------------------------------------------------------
	// Zoom in (placeholder)
	void ZoomIn()
	{
		// Placeholder
	}
	
	//---------------------------------------------------------------------------
	// Zoom out (placeholder)
	void ZoomOut()
	{
		// Placeholder
	}
	
	//---------------------------------------------------------------------------
	// Center view (placeholder)
	void CenterView()
	{
		// Placeholder
	}
	
	//---------------------------------------------------------------------------
	// Input handling (like HCA_RadUI) - initialize in DisplayStartDrawInit
	override bool DisplayStartDrawInit(IEntity owner)
	{
		if (!super.DisplayStartDrawInit(owner))
			return false;
		
		return true;
	}
	
	//---------------------------------------------------------------------------
	// Input handling (like HCA_RadUI) - initialize in DisplayStartDraw
	override void DisplayStartDraw(IEntity owner)
	{
		super.DisplayStartDraw(owner);
		
		m_InputManager = GetGame().GetInputManager();
		if (!m_InputManager)
		{
			Print("[ATAK_DeviceDisplay] ERROR: InputManager not found!", LogLevel.ERROR);
			return;
		}
		
		// Ensure context is inactive until the display is opened
		// m_InputManager.ActivateContext("ATAK_MenuContext", 0); // Removed as per edit hint
		
		Print("[ATAK_DeviceDisplay] Input system initialized");
		
		// Register action listeners with delay
		GetGame().GetCallqueue().CallLater(RegisterInputListeners, 100, false);
		
		// Frame-based fallback disabled for now (listeners are reliable)
		// GetGame().GetCallqueue().CallLater(StartFrameCheck, 200, false);
	}
	
	//---------------------------------------------------------------------------
	override void DisplayStopDraw(IEntity owner)
	{
		// Clean up input listeners
		RemoveInputListeners();
		
		// Stop frame checking
		if (m_bFrameCheckActive)
		{
			GetGame().GetCallqueue().Remove(CheckActionFrame);
			m_bFrameCheckActive = false;
		}
		
		super.DisplayStopDraw(owner);
	}
	
	//---------------------------------------------------------------------------
	// Register input action listeners
	protected void RegisterInputListeners()
	{
		if (m_bInputListenersRegistered || !m_InputManager)
			return;
		
		// Register ESC listener for closing
		m_InputManager.AddActionListener("MenuBack", EActionTrigger.DOWN, OnCloseATAK);
		
		// Register toggle listener
		m_InputManager.AddActionListener("ATAK_Toggle", EActionTrigger.UP, OnToggleATAK);
		
		m_bInputListenersRegistered = true;
		Print("[ATAK_DeviceDisplay] Input listeners registered (ESC only - using frame check for toggle)");
	}
	
	//---------------------------------------------------------------------------
	// Remove input action listeners
	protected void RemoveInputListeners()
	{
		if (!m_bInputListenersRegistered || !m_InputManager)
			return;
		
		// Only remove ESC listener (toggle uses frame check)
		m_InputManager.RemoveActionListener("MenuBack", EActionTrigger.DOWN, OnCloseATAK);
		m_InputManager.RemoveActionListener("ATAK_Toggle", EActionTrigger.UP, OnToggleATAK);
		
		m_bInputListenersRegistered = false;
		Print("[ATAK_DeviceDisplay] Input listeners removed");
	}
	
	//---------------------------------------------------------------------------
	// Frame-based action checking (fallback)
	protected void StartFrameCheck()
	{
		m_bFrameCheckActive = true;
		GetGame().GetCallqueue().CallLater(CheckActionFrame, 0, true);
	}
	
	//---------------------------------------------------------------------------
	protected void CheckActionFrame()
	{
		// Use frame check as primary method since action listener value is unreliable with InputFilterClick
		if (!m_InputManager)
			return;
		
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!playerController)
			return;
		
		ActionManager actionManager = playerController.GetActionManager();
		if (!actionManager)
			return;
		
		// Check if action was triggered this frame AND get the action value
		bool actionTriggered = actionManager.GetActionTriggered("ATAK_Toggle");
		float actionValue = actionManager.GetActionValue("ATAK_Toggle");
		
		// Only process on rising edge with a clean keyboard value (~1.0)
		bool isRisingEdge = actionTriggered && !m_bActionWasTriggeredLastFrame && Math.AbsFloat(actionValue - 1.0) < 0.01;
		
		if (isRisingEdge)
		{
			PrintFormat("[ATAK_DeviceDisplay] ATAK_Toggle triggered via frame check (value=%1)", actionValue);
			OnToggleATAK(1.0, EActionTrigger.UP);
		}
		
		// Update edge detection state - remember if action was triggered this frame
		m_bActionWasTriggeredLastFrame = actionTriggered;
		m_fLastActionValue = actionValue;
	}
	
	//---------------------------------------------------------------------------
	// Action listener callback for toggle
	protected void OnToggleATAK(float value, EActionTrigger trigger)
	{
		PrintFormat("[ATAK_DeviceDisplay] OnToggleATAK called - value: %1, trigger: %2", value, trigger);
		
		// CRITICAL: Prevent re-entry FIRST - this must be checked before anything else
		if (m_bProcessingToggle)
		{
			Print("[ATAK_DeviceDisplay] Already processing toggle - ignoring");
			return;
		}
		
		// Check if menu is open
		MenuManager menuMgr = GetGame().GetMenuManager();
		if (menuMgr && menuMgr.IsAnyMenuOpen())
		{
			Print("[ATAK_DeviceDisplay] Menu is open - ignoring toggle");
			return;
		}
		
		// Cooldown check - must be first check to prevent rapid toggling
		float currentTime = System.GetTickCount() / 1000.0;
		float timeSinceLastToggle = currentTime - m_fLastToggleTime;
		if (timeSinceLastToggle < TOGGLE_COOLDOWN)
		{
			PrintFormat("[ATAK_DeviceDisplay] Cooldown active - %1 seconds remaining", TOGGLE_COOLDOWN - timeSinceLastToggle);
			return;
		}
		
		// Only accept UP trigger
		if (trigger != EActionTrigger.UP)
		{
			PrintFormat("[ATAK_DeviceDisplay] Wrong trigger type: %1 (expected UP)", trigger);
			return;
		}
		
		// Verify player controller
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!playerController)
		{
			Print("[ATAK_DeviceDisplay] ERROR: PlayerController not found!");
			return;
		}
		
		ActionManager actionManager = playerController.GetActionManager();
		if (!actionManager)
		{
			Print("[ATAK_DeviceDisplay] ERROR: ActionManager not found!");
			return;
		}
		
		float confirmedValue = actionManager.GetActionValue("ATAK_Toggle");
		if (Math.AbsFloat(confirmedValue - 1.0) > 0.01)
		{
			PrintFormat("[ATAK_DeviceDisplay] Action value unexpected (%1) - ignoring toggle", confirmedValue);
			return;
		}
		
		// Check if player is spawned
		IEntity playerEntity = playerController.GetControlledEntity();
		if (!playerEntity)
		{
			Print("[ATAK_DeviceDisplay] WARNING: No controlled entity - player may not be spawned yet");
			return;
		}
		
		Print("[ATAK_DeviceDisplay] All checks passed - proceeding with toggle");
		
		// Set processing flag
		m_bProcessingToggle = true;
		m_fLastToggleTime = currentTime;
		
		// Toggle display
		if (showATAK)
		{
			Print("[ATAK_DeviceDisplay] Closing ATAK Info Display");
			Show(false);
			showATAK = false;
		}
		else
		{
			Print("[ATAK_DeviceDisplay] Opening ATAK Info Display");
			Show(true);
			showATAK = true;
		}
		
		// Clear processing flag after delay
		GetGame().GetCallqueue().CallLater(ClearProcessingFlag, 50, false);
	}
	
	//---------------------------------------------------------------------------
	// Action listener callback for close (ESC)
	protected void OnCloseATAK(float value, EActionTrigger trigger)
	{
		if (!showATAK)
			return;
		
		Print("[ATAK_DeviceDisplay] ESC key pressed - closing ATAK Info Display");
		
		// Deactivate ATAK context when closing via ESC
		// m_InputManager.ActivateContext("ATAK_MenuContext", 0); // Removed as per edit hint
		
		// Hide the display
		Show(false);
		showATAK = false;
	}
	
	//---------------------------------------------------------------------------
	// Clear the processing flag
	protected void ClearProcessingFlag()
	{
		m_bProcessingToggle = false;
	}
	
}
