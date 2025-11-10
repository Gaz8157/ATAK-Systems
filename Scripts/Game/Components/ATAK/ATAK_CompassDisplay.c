class ATAK_CompassDisplay: SCR_InfoDisplay
{
	//---------------------------------------------------------------------------
	private World world = GetGame().GetWorld();
	
	// Compass UI Elements
	private FrameWidget compassFrame;
	private TextWidget bearingText;
	private TextWidget directionText;
	
	// State
	private bool showCompass = false; // Hidden on spawn
	private bool initialized = false;
	
	override protected void UpdateValues(IEntity owner, float timeSlice)
	{
		super.UpdateValues(owner, timeSlice);
		
		if (!world) return;
		
		// Initialize on first update
		if (!initialized) 
		{
			InitializeCompass();
		}
		
		// Update compass display if visible
		if (showCompass && initialized)
		{
			UpdateCompassValues();
		}
	}
	
	//---------------------------------------------------------------------------
	// Initialize compass system
	private void InitializeCompass()
	{
		// Hide root widget initially
		if (m_wRoot)
		{
			m_wRoot.SetVisible(false);
		}
		
		// Fetch widgets
		compassFrame = FrameWidget.Cast(m_wRoot.FindAnyWidget("CompassFrame"));
		if (!compassFrame)
		{
			return;
		}
		
		bearingText = TextWidget.Cast(m_wRoot.FindAnyWidget("BearingText"));
		directionText = TextWidget.Cast(m_wRoot.FindAnyWidget("DirectionText"));
		
		// Register input listeners
		InputManager inputMgr = GetGame().GetInputManager();
		if (inputMgr)
		{
			inputMgr.AddActionListener("ATAKCompassToggle", EActionTrigger.DOWN, Toggle);
		}
		
		// Hide compass initially
		if (compassFrame)
		{
			compassFrame.SetVisible(false);
		}
		
		initialized = true;
	}
	
	//---------------------------------------------------------------------------
	// Update the compass bearing and direction
	private void UpdateCompassValues()
	{
		if (!bearingText || !directionText) return;
		
		// Get current yaw (compass bearing)
		float yaw = GetYaw();
		
		// Format bearing with leading zeros
		string bearingStr = FormatBearing(yaw);
		
		// Convert yaw to cardinal direction
		string direction = YawToDirection(yaw);
		
		// Update text
		bearingText.SetText(bearingStr);
		directionText.SetText(direction);
	}
	
	//---------------------------------------------------------------------------
	// Convert yaw to 8 cardinal directions (N, NE, E, SE, S, SW, W, NW)
	private string YawToDirection(int bearing)
	{
		string directions[8] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};
		int result = Math.Round(bearing / 45);
		if (result == 8) 
		{
			return directions[0];
		}
		else
			return directions[result];
	}
	
	//---------------------------------------------------------------------------
	// Format bearing with leading zeros (e.g., "045", "180")
	private string FormatBearing(float bearing)
	{
		int bearingInt = Math.Round(bearing);
		string prefix = "";
		
		if (bearingInt < 10)
		{
			prefix = "00";
		}
		else if (bearingInt < 100)
		{
			prefix = "0";
		}
		
		return prefix + bearingInt.ToString();
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
	// Toggle compass visibility
	void Toggle()
	{
		showCompass = !showCompass;
		
		if (compassFrame)
		{
			compassFrame.SetVisible(showCompass);
		}
		
		// Also toggle root visibility
		if (m_wRoot)
		{
			m_wRoot.SetVisible(showCompass);
		}
	}
	
	//---------------------------------------------------------------------------
	// Show compass
	void Show()
	{
		showCompass = true;
		if (compassFrame)
		{
			compassFrame.SetVisible(true);
		}
		if (m_wRoot)
		{
			m_wRoot.SetVisible(true);
		}
	}
	
	//---------------------------------------------------------------------------
	// Hide compass
	void Hide()
	{
		showCompass = false;
		if (compassFrame)
		{
			compassFrame.SetVisible(false);
		}
		if (m_wRoot)
		{
			m_wRoot.SetVisible(false);
		}
	}
}

