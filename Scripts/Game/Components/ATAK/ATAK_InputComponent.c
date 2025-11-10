// ATAK Input Component - Handles toggling the ATAK Info Display with N key
// Uses action listeners like VPAD and other mods

class ATAK_InputComponentClass : ScriptGameComponentClass
{
}

class ATAK_InputComponent : ScriptGameComponent
{
    protected const float TOGGLE_COOLDOWN = 0.5; // Cooldown to prevent rapid toggling (increased to prevent double-trigger)
    protected float m_fLastToggleTime = 0;
    protected InputManager m_InputManager;
    protected bool m_bListenersRegistered = false;
    protected bool m_bDisplayIsOpen = false; // Track display state internally
    protected bool m_bProcessingToggle = false; // Prevent re-entry during toggle processing
    protected float m_fLastFrameCheckDebugTime = 0; // For frame check debug logging
    
    override void OnPostInit(IEntity owner)
    {
        super.OnPostInit(owner);
        
        m_InputManager = GetGame().GetInputManager();
        if (!m_InputManager)
        {
            Print("[ATAK_Input] ERROR: InputManager not found!", LogLevel.ERROR);
            return;
        }
        
        // Keep ATAK_MenuContext active at medium priority (accessible but doesn't interfere with movement)
        // Priority 100 = medium priority, accessible for toggle but won't block movement contexts
        m_InputManager.ActivateContext("ATAK_MenuContext", 100);
        
        Print("[ATAK_Input] Component initialized on player controller");
        Print("[ATAK_Input] Using action listeners (like VPAD)");
        
        // Register action listeners with a small delay to ensure input system is ready
        // This ensures Chimera config is fully loaded
        GetGame().GetCallqueue().CallLater(RegisterActionListeners, 100, false);
        
        // Also start frame-based checking as fallback (like vPad does)
        GetGame().GetCallqueue().CallLater(StartFrameCheck, 200, false);
    }
    
    // Frame-based action checking (fallback if listeners don't work)
    protected void StartFrameCheck()
    {
        GetGame().GetCallqueue().CallLater(CheckActionFrame, 0, true);
    }
    
    protected void CheckActionFrame()
    {
        if (!m_InputManager)
            return;
            
        // Check if action was triggered this frame (like vPad's GetActionTriggered)
        // This works even if action listeners don't fire
        SCR_PlayerController playerController = SCR_PlayerController.Cast(GetOwner());
        if (!playerController)
            return;
            
        ActionManager actionManager = playerController.GetActionManager();
        if (!actionManager)
        {
            Print("[ATAK_Input] WARNING: ActionManager not found in frame check");
            return;
        }
        
        // Check action value and triggered state
        float actionValue = actionManager.GetActionValue("ATAK_Toggle");
        bool actionTriggered = actionManager.GetActionTriggered("ATAK_Toggle");
        
        // Debug log every few seconds to verify frame check is running
        float currentTime = System.GetTickCount() / 1000.0;
        if (currentTime - m_fLastFrameCheckDebugTime > 3.0)
        {
            PrintFormat("[ATAK_Input] Frame check running - actionValue: %1, triggered: %2", actionValue, actionTriggered);
            m_fLastFrameCheckDebugTime = currentTime;
        }
        
        // Check if action was triggered (fires on key release for InputFilterClick)
        if (actionTriggered)
        {
            Print("[ATAK_Input] ATAK_Toggle triggered via frame check");
            // Call toggle handler directly
            OnToggleATAK(1.0, EActionTrigger.UP);
        }
        
        // Also check if action value changed (for debugging)
        if (actionValue > 0.1)
        {
            PrintFormat("[ATAK_Input] Action value detected: %1", actionValue);
        }
    }
    
    override void OnDelete(IEntity owner)
    {
        // Clean up listeners on component deletion
        RemoveActionListeners();
        
        // Stop frame checking
        GetGame().GetCallqueue().Remove(CheckActionFrame);
        
        super.OnDelete(owner);
    }
    
    // Register action listeners (like VPAD)
    protected void RegisterActionListeners()
    {
        if (m_bListenersRegistered || !m_InputManager)
            return;
        
        // Test if action exists
        float testValue = m_InputManager.GetActionValue("ATAK_Toggle");
        PrintFormat("[ATAK_Input] Testing ATAK_Toggle action - current value: %1", testValue);
        
        // Register toggle listener - use UP trigger only (InputFilterClick fires on key release)
        // If action doesn't exist, listener registration will fail silently (config not loaded)
        m_InputManager.AddActionListener("ATAK_Toggle", EActionTrigger.UP, OnToggleATAK);
        
        // Register ESC listener for closing (only when display is open)
        m_InputManager.AddActionListener("MenuBack", EActionTrigger.DOWN, OnCloseATAK);
        
        m_bListenersRegistered = true;
        Print("[ATAK_Input] Action listeners registered (UP trigger for toggle, DOWN for ESC)");
    }
    
    // Remove action listeners
    protected void RemoveActionListeners()
    {
        if (!m_bListenersRegistered || !m_InputManager)
            return;
        
        m_InputManager.RemoveActionListener("ATAK_Toggle", EActionTrigger.UP, OnToggleATAK);
        m_InputManager.RemoveActionListener("MenuBack", EActionTrigger.DOWN, OnCloseATAK);
        
        m_bListenersRegistered = false;
        Print("[ATAK_Input] Action listeners removed");
    }
    
    // Action listener callback for toggle (like VPAD's gOS_KeyDown_* methods)
    protected void OnToggleATAK(float value, EActionTrigger trigger)
    {
        // DEBUG: Log that the callback was triggered
        PrintFormat("[ATAK_Input] OnToggleATAK called - value: %1, trigger: %2", value, trigger);
        
        // CRITICAL: Don't process if any menu or dialog is open (prevents interference with keybinding assignment)
        MenuManager menuMgr = GetGame().GetMenuManager();
        if (menuMgr && menuMgr.IsAnyMenuOpen())
        {
            Print("[ATAK_Input] Menu is open - ignoring toggle (keybinding dialog active?)");
            return;
        }
        
        // NOTE: Device type check removed - GetLastUsedInputDevice() is unreliable (returns last device used globally, not for this action)
        // The config already restricts ATAK_Toggle to keyboard only ("keyboard:KC_N"), so no script-side check needed
        
        // CRITICAL: Prevent re-entry - if we're already processing a toggle, ignore this call
        if (m_bProcessingToggle)
        {
            Print("[ATAK_Input] Already processing toggle - ignoring");
            return;
        }
        
        // Only accept UP trigger - InputFilterClick fires on key release (UP)
        if (trigger != EActionTrigger.UP)
        {
            PrintFormat("[ATAK_Input] Wrong trigger type: %1 (expected UP)", trigger);
            return;
        }
        
        // For UP trigger with InputFilterClick, value might be 0 (key was just released)
        // This is normal for InputFilterClick - we process it regardless of value
        
        // CRITICAL: Verify the action value is valid (should be > 0 for UP trigger on digital action)
        // This helps filter out spurious triggers
        float actionValue = m_InputManager.GetActionValue("ATAK_Toggle");
        
        // For UP trigger on a digital action that was just released, the value might be 0
        // But we should still process it if the trigger is UP and cooldown passed
        // The action config should already filter to keyboard only
        
        // Cooldown check - prevent rapid-fire toggling
        float currentTime = System.GetTickCount() / 1000.0;
        float timeSinceLastToggle = currentTime - m_fLastToggleTime;
        if (timeSinceLastToggle < TOGGLE_COOLDOWN)
        {
            PrintFormat("[ATAK_Input] Cooldown active - %1 seconds remaining", TOGGLE_COOLDOWN - timeSinceLastToggle);
            return;
        }
        
        // CRITICAL: Verify player controller is valid and player is alive
        SCR_PlayerController playerController = SCR_PlayerController.Cast(GetOwner());
        if (!playerController)
        {
            Print("[ATAK_Input] ERROR: Owner is not SCR_PlayerController!");
            return;
        }
        
        // Check if player is in a valid state (not dead, not in menu, etc.)
        IEntity playerEntity = playerController.GetControlledEntity();
        if (!playerEntity)
        {
            Print("[ATAK_Input] WARNING: No controlled entity - player may not be spawned yet");
            return;
        }
        
        // Get the Info Display
        ATAK_DeviceDisplay atakDisplay = GetATAKDisplay();
        if (!atakDisplay)
        {
            Print("[ATAK_Input] ERROR: Could not get ATAK_DeviceDisplay!");
            return;
        }
        
        Print("[ATAK_Input] All checks passed - proceeding with toggle");
        
        // Set processing flag to prevent re-entry
        m_bProcessingToggle = true;
        
        // Update cooldown FIRST to prevent rapid toggling
        m_fLastToggleTime = currentTime;
        
        // CRITICAL: Use ONLY internal state tracking - don't check IsShown() as it causes race conditions
        // The internal state is the source of truth
        if (m_bDisplayIsOpen)
        {
            // Display is open - close it
            Print("[ATAK_Input] N key released - closing ATAK Info Display");
            
            // Hide the Info Display FIRST
            atakDisplay.Show(false);
            m_bDisplayIsOpen = false;
            
            // Lower context priority (doesn't interfere with movement, but action still available)
            if (m_InputManager)
            {
                m_InputManager.ActivateContext("ATAK_MenuContext", 100);
            }
        }
        else
        {
            // Display is closed - open it
            Print("[ATAK_Input] N key released - opening ATAK Info Display");
            
            // Raise context priority for menu navigation (higher priority when menu is open)
            if (m_InputManager)
            {
                m_InputManager.ActivateContext("ATAK_MenuContext", 1000);
            }
            
            // Show the Info Display AFTER activating context
            atakDisplay.Show(true);
            m_bDisplayIsOpen = true;
        }
        
        // Clear processing flag after a small delay to ensure state is stable
        GetGame().GetCallqueue().CallLater(ClearProcessingFlag, 50, false);
    }
    
    // Clear the processing flag
    protected void ClearProcessingFlag()
    {
        m_bProcessingToggle = false;
    }
    
    // Action listener callback for close (ESC)
    protected void OnCloseATAK(float value, EActionTrigger trigger)
    {
        if (!m_bDisplayIsOpen)
            return;
        
        // Get the Info Display
        ATAK_DeviceDisplay atakDisplay = GetATAKDisplay();
        if (!atakDisplay)
            return;
        
        Print("[ATAK_Input] ESC key pressed - closing ATAK Info Display");
        
        // Lower context priority (doesn't interfere with movement, but action still available)
        if (m_InputManager)
        {
            m_InputManager.ActivateContext("ATAK_MenuContext", 100);
        }
        
        // Hide the Info Display
        atakDisplay.Show(false);
        m_bDisplayIsOpen = false;
    }
    
    //------------------------------------------------------------------------------------------------
    // Helper to get ATAK_DeviceDisplay from HUD Manager
    protected ATAK_DeviceDisplay GetATAKDisplay()
    {
        // Get player controller and HUD Manager
        SCR_PlayerController playerController = SCR_PlayerController.Cast(GetOwner());
        if (!playerController)
        {
            Print("[ATAK_Input] ERROR: Owner is not SCR_PlayerController!", LogLevel.ERROR);
            return null;
        }
        
        // Get HUD Manager Component
        SCR_HUDManagerComponent hudManager = SCR_HUDManagerComponent.Cast(playerController.FindComponent(SCR_HUDManagerComponent));
        if (!hudManager)
        {
            Print("[ATAK_Input] ERROR: SCR_HUDManagerComponent not found!", LogLevel.ERROR);
            return null;
        }
        
        // Find the ATAK_DeviceDisplay Info Display
        ATAK_DeviceDisplay atakDisplay = ATAK_DeviceDisplay.Cast(hudManager.FindInfoDisplay(ATAK_DeviceDisplay));
        if (!atakDisplay)
        {
            Print("[ATAK_Input] ERROR: ATAK_DeviceDisplay Info Display not found in HUD Manager!", LogLevel.ERROR);
            return null;
        }
        
        return atakDisplay;
    }
}
