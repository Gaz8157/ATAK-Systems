// ATAK Input System - Disabled to prevent conflicts with ATAK_InputComponent
// This system was causing input conflicts and has been disabled
// Input handling is now done through ATAK_InputComponent on the player controller

class ATAK_InputSystemClass : ScriptGameComponentClass
{
}

class ATAK_InputSystem : ScriptGameComponent
{
    // DISABLED: This system is no longer used
    // Input handling is done through ATAK_InputComponent on the player controller
    
    override void OnPostInit(IEntity owner)
	{
        super.OnPostInit(owner);
        
        // DISABLED: Input listener registration removed to prevent conflicts
        // InputManager inputManager = GetGame().GetInputManager();
        // if (inputManager)
        // {
        //     inputManager.AddActionListener("Gesture", EActionTrigger.PRESSED, OnToggle);
        // }
        
        Print("[ATAK_InputSystem] System initialized (disabled - using ATAK_InputComponent instead)");
    }
    
    // DISABLED: Toggle method removed to prevent conflicts
    // void OnToggle(float value, EActionTrigger trigger)
    // {
    //     // This was causing conflicts with ATAK_InputComponent
    // }
}
