sealed class GunBuilderUI_XboxNoteComponent: ScriptedWidgetComponent {
	override void HandlerAttached(Widget w) {
		super.HandlerAttached(w);
		
//		if (System.IsConsoleApp())
//			return;
//		
//		if (Replication.IsServer() && System.GetPlatform() != EPlatform.WINDOWS) {
//			w.SetVisible(true);
//		}
		
		w.SetVisible(!Bacon_GunBuilderUI_Helpers.IsFileSavingEnabled());
	}
}