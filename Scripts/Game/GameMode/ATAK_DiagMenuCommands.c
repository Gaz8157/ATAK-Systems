// ATAK DiagMenu Debug Commands
// Note: DiagMenuAttribute is not available in the current API
// These methods can be called manually via console or bound to DiagMenu using DiagMenu.RegisterItem()

class ATAK_DiagMenuCommands
{
    // Toggle ATAK Menu - can be registered manually if needed
    static void ToggleATAKMenu()
    {
        Print("[ATAK_DiagMenu] Manual toggle triggered from DiagMenu");
        
        MenuManager menuMgr = GetGame().GetMenuManager();
        if (!menuMgr)
        {
            Print("[ATAK_DiagMenu] ERROR: MenuManager not found!", LogLevel.ERROR);
            return;
        }
        
        // DISABLED: Using Info Display system now, not ChimeraMenuBase
        // ATAK_DeviceMenu existingMenu = ATAK_DeviceMenu.GetInstance();
        // if (existingMenu)
        // {
        //     Print("[ATAK_DiagMenu] Menu is open - closing");
        //     existingMenu.Close();
        // }
        // else
        // {
        //     Print("[ATAK_DiagMenu] Opening ATAK menu");
        //     MenuBase opened = menuMgr.OpenMenu(ChimeraMenuPreset.ATAK_DeviceMenu);
        //     PrintFormat("[ATAK_DiagMenu] OpenMenu returned: %1", opened);
        // }
        
        // Use Info Display system instead
        Print("[ATAK_DiagMenu] Toggle ATAK Info Display (use N key in-game)");
    }
    
    // Check Input State - can be registered manually if needed
    static void CheckInputState()
    {
        Print("[ATAK_DiagMenu] ===== ATAK Input State Check =====");
        
        InputManager inputMgr = GetGame().GetInputManager();
        if (!inputMgr)
        {
            Print("[ATAK_DiagMenu] ERROR: InputManager is NULL!");
            return;
        }
        
        // Check action value
        float atakToggleValue = inputMgr.GetActionValue("ATAK_Toggle");
        PrintFormat("[ATAK_DiagMenu] ATAK_Toggle action value: %1", atakToggleValue);
        
        // Check if context is active
        bool menuContextActive = inputMgr.IsContextActive("ATAK_MenuContext");
        PrintFormat("[ATAK_DiagMenu] ATAK_MenuContext active: %1", menuContextActive);
        
        // List all active contexts
        Print("[ATAK_DiagMenu] =====================================");
    }
}

