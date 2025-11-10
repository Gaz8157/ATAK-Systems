sealed class Bacon_GunBuilderOpenAction : ScriptedUserAction
{
	override bool HasLocalEffectOnlyScript() { return true; }
	override bool CanBroadcastScript() { return false; }
	
	override bool CanBePerformedScript(IEntity user) {
//		if (System.GetPlatform() == EPlatform.PS5 || System.GetPlatform() == EPlatform.PS5_PRO) {
//			m_sCannotPerformReason = "Crashes on PS5";
//			return false;
//		}
		
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{	
		MenuManager menuManager = GetGame().GetMenuManager();
		if (!menuManager) return;
		
		Bacon_GunBuilderUI menu = Bacon_GunBuilderUI.Cast(menuManager.OpenMenu(ChimeraMenuPreset.Bacon_GunBuilderUI));
		if (!menu) {
			Print("Bacon_GunBuilderOpenAction.PerformAction | Failed to open Bacon_GunBuilderUI menu (preset missing?)", LogLevel.ERROR);
			return;
		}
		
		menu.Init(pOwnerEntity, pUserEntity);
	}
};
