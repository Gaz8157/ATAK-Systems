[BaseContainerProps()]
class ATAK_HUD : SCR_InfoDisplayExtended
{
    // Simple HUD component - just creates UI, no complex input handling
    
    override bool DisplayStartDrawInit(IEntity owner)
    {
        Print("[ATAK_HUD] Simple HUD initialized - no input handling");
        return true;
    }

    override void DisplayStopDraw(IEntity owner)
    {
        Print("[ATAK_HUD] HUD cleanup completed");
    }

    // Helpers
    protected ATAK_UIComponent GetUIComp()
    {
        // Get the PlayerController directly
        SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
        if (playerController)
        {
            ATAK_UIComponent uiComp = ATAK_UIComponent.Cast(playerController.FindComponent(ATAK_UIComponent));
            if (uiComp)
            {
                Print("[ATAK_HUD] Found ATAK_UIComponent on PlayerController");
                return uiComp;
            }
        }
        
        // Fallback: try the controlled entity
        IEntity controlled = SCR_PlayerController.GetLocalControlledEntity();
        if (controlled)
        {
            ATAK_UIComponent uiComp = ATAK_UIComponent.Cast(controlled.FindComponent(ATAK_UIComponent));
            if (uiComp)
            {
                Print("[ATAK_HUD] Found ATAK_UIComponent on controlled entity");
                return uiComp;
            }
        }
        
        Print("[ATAK_HUD] ATAK_UIComponent not found anywhere", LogLevel.WARNING);
        return null;
    }
}


