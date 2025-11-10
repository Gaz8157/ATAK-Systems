

class gOS_SCR_MiniMap_HUD : SCR_InfoDisplayExtended {

    static gOS_SCR_MiniMap_HUD s_CurrentHud;

	// gOS_SCR_MiniMap_HUD.gOS_GetCurrent()
    static gOS_SCR_MiniMap_HUD gOS_GetCurrent () {
        if (s_CurrentHud) {
            return s_CurrentHud;
        };
        return null;
    };

	//protected SCR_CharacterControllerComponent m_CharacterController;
    protected const ResourceName gOS_DEFAULT_LAYOUT = "{FE7FBE5223A1B87D}UI/GOS_Minimap_Menu_3d.layout";


	override bool DisplayStartDrawInit (IEntity owner) {
		gOS_RadUI_Init();

		// gOS_OnDisplayStartDraw_InitSettings();

        // Print("gOS::gOS_SCR_MiniMap_HUD::DisplayStartDrawInit::OWN:" + owner);

		// Register weapon related weapon event handlers
		gOS_AddKeyEvents();

		m_MenuManager = GetGame().GetMenuManager();
		if (!m_MenuManager)
			return false;
		
		
		// Fallback in case the layout is not provided as an InfoDisplay attribute
		if (m_LayoutPath == string.Empty)
			m_LayoutPath = gOS_DEFAULT_LAYOUT;
		

		// SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(owner);
		// if (!character)
		// 	return false;
			
		// // // Store character contoller
		// m_CharacterController = SCR_CharacterControllerComponent.Cast(character.FindComponent(SCR_CharacterControllerComponent));
		// if (!m_CharacterController)
		// 	return false;


		return true;
	};
    
	protected float gOS_m_fOpacity_Desired = 1;

	//------------------------------------------------------------------------------------------------	
	protected void gOS_UpdateOpacity (float opacity, float sceneBrightness, float sceneBrightnessRaw) {
		gOS_m_fOpacity_Desired = sceneBrightness * 1;
		if (!m_wRoot) {
			return;
		};
    };

	//------------------------------------------------------------------------------------------------
	// override void UpdateOpacity (float opacity, float sceneBrightness, float sceneBrightnessRaw) {
		
	// 	gOS_m_fOpacity_Desired = 0.25 + sceneBrightness * 0.5;
	// 	if (gOS_m_fOpacity_Desired < 0.2) {
	// 		gOS_m_fOpacity_Desired = 0.2;
	// 	};
		
	// 	return;
	// };

	//------------------------------------------------------------------------------------------------
	override void DisplayStartDraw (IEntity owner) {
		// Do default actions such as UI setups
		gOS_OnDisplayStartDraw();

		if (!m_wRoot)
			return;
		
		// m_Widgets = new SCR_gOS_Driving_HUDWidgets();
		// m_Widgets.Init(m_wRoot);
		
		
        gOS_SCR_MiniMap_HUD.s_CurrentHud = this;


		if (!GetGame().GetHUDManager() || !GetGame().GetHUDManager().GetSceneBrightnessChangedInvoker()) {
			return;
		};



	};


	//------------------------------------------------------------------------------------------------
	override void DisplayStopDraw (IEntity owner) {
		gOS_OnDisplayStopDraw();
		
		// // Un-register event handlers
		gOS_RemoveKeyEvents();
		
		// Print("DisplayStopDraw::DisplayStopDraw::DisplayStopDraw");
		// gOS_RadUI_Exit();
		
        gOS_SCR_MiniMap_HUD.s_CurrentHud = null;
		m_wRoot = null;


		// Maybe I should run the susper?
		// super.DisplayStopDraw(owner);

		if (!GetGame().GetHUDManager() || !GetGame().GetHUDManager().GetSceneBrightnessChangedInvoker()) {
			return;
		};

	};

	//--- Init function to save commonly used widgets
	protected void gOS_OnDisplayStopDraw () {
        // Print("gOS::gOS_SCR_MiniMap_HUD::DisplayStopDraw");
		gOS_OnDisplayStopDraw_DeleteSettings();
		gOS_OnDisplayStopDraw_Delete_MapHandlers();

		
		// disable if able?
		gOS_m_bIsPlacing = false;
		if (gOS_m_IEnt_Place_Test) {
			SCR_EntityHelper.DeleteEntityAndChildren(gOS_m_IEnt_Place_Test);
		};
	};


	override void DisplayUpdate (IEntity owner, float timeSlice) {
		super.DisplayUpdate(owner, timeSlice);

		// lets go
		gOS_OnDisplayUpdate(owner, timeSlice);

		// gOS_Mny_OnDisplayUpdate(owner, timeSlice);
	};

	private float gOS_m_fControl_Zoom = 0;
	private float gOS_m_fControl_PanX = 0;
	private float gOS_m_fControl_PanY = 0;
	private bool gOS_m_bControl_POI_Requested = false;

	private float gOS_m_fApplied_Zoom = 0;
	private float gOS_m_fApplied_PanX = 0;
	private float gOS_m_fApplied_PanY = 0;

	private float gOS_m_fPOI_Time_Started = 0;


	// private float gOS_m_fPanAppliedY = 0;

    void gOS_OnDisplayUpdate (IEntity f_Owner, float f_TimePassed) {
        // Print("gOS_OnDisplayUpdate:" + f_Owner);

	


		if (!m_wFrame0) {
			return;
		};

		float t_fOpacity_Current = m_wFrame0.GetOpacity();
		
		if (t_fOpacity_Current == gOS_m_fOpacity_Desired) {
			return;
		};
		

		float t_fOpacity_Missing = gOS_m_fOpacity_Desired - t_fOpacity_Current;
		float t_fOpacity_Change = (0.25 * f_TimePassed);

		if (gOS_m_fOpacity_Desired > t_fOpacity_Current) {

		} else {
			t_fOpacity_Change = 0 - t_fOpacity_Change;
		};
		float t_fOpacity_New = t_fOpacity_Current + t_fOpacity_Change;
		
		if (gOS_m_fOpacity_Desired > t_fOpacity_Current && t_fOpacity_New > gOS_m_fOpacity_Desired) {
			// Print("New bigger than desired");
			t_fOpacity_New = gOS_m_fOpacity_Desired;
		};
		
		if (gOS_m_fOpacity_Desired < t_fOpacity_Current && t_fOpacity_New < gOS_m_fOpacity_Desired) {
			// Print("New smaller than desired");

			t_fOpacity_New = gOS_m_fOpacity_Desired;
		};
		// Print("C:" +t_fOpacity_Current + " D:" + gOS_m_fOpacity_Desired + " new:" +t_fOpacity_New + " change:" +  t_fOpacity_Change +  " m:" + t_fOpacity_Missing);

		//! TEST
		//	- Does this function okay?
		// m_wFrame0.SetOpacity(t_fOpacity_New);



	};


	private void gOS_OnDisplayStartDraw_InitSettings () {
		// Print("gOS_OnDisplayStartDraw_InitSettings");
		gOS_LoadSettings();
		GetGame().OnUserSettingsChangedInvoker().Insert(gOS_LoadSettings);
	};
	private void gOS_OnDisplayStopDraw_DeleteSettings () {
		// Print("gOS_OnDisplayStopDraw_DeleteSettings");
		GetGame().OnUserSettingsChangedInvoker().Remove(gOS_LoadSettings);
	};
	private void gOS_LoadSettings () {
		// Print("gOS_LoadSettings");
		// Print(this);
		BaseContainer t_Settings_gOS = GetGame().GetGameUserSettings().GetModule("gOS_SCR_Settings");
		// bool t_bIsTest;
		// if (t_Settings_gOS.Get("m_bIsTest1", t_bIsTest)) {
		// 	// Print(t_bIsTest);
        // };

		if (!t_Settings_gOS.Get("m_bIsSettingsShortcut", gOS_m_bIsSettingsShortcut)) {
            gOS_m_bIsSettingsShortcut = true;
        };

		if (!t_Settings_gOS.Get("m_bIsHoldForMap", gOS_m_bIsHoldForMap)) {
            gOS_m_bIsHoldForMap = true;
        };

		if (!t_Settings_gOS.Get("m_bEnableGPadVehPan", gOS_m_bEnableGPadVehPan)) {
            gOS_m_bEnableGPadVehPan = false;
        };
		
		if (!t_Settings_gOS.Get("m_bEnableGPadVehZoom", gOS_m_bEnableGPadVehZoom)) {
            gOS_m_bEnableGPadVehZoom = true;
        };
		
		if (!t_Settings_gOS.Get("m_bGPBlockGadgetAway", gOS_m_bGPBlockGadgetAway)) {
            gOS_m_bGPBlockGadgetAway = true;
        };

		if (!t_Settings_gOS.Get("m_iBrightness", gOS_m_iBrightness)) {
            gOS_m_iBrightness = 100;
        };
		// Print("gOS_LoadSettings:" + gOS_m_iBrightness);
		gOS_Brigthness_Set(gOS_m_iBrightness);
		// Print(gOS_m_iBrightness);
	};
	protected int gOS_m_iBrightness = 100;
    protected bool gOS_m_bIsSettingsShortcut = false;
    protected bool gOS_m_bIsHoldForMap = false;

    protected bool gOS_m_bEnableGPadVehPan = false;
    protected bool gOS_m_bEnableGPadVehZoom = false;
    protected bool gOS_m_bGPBlockGadgetAway = false;


	//--- fixing vanila map zoom bug
	
	private void gOS_OnDisplayStartDraw_InitMapHandlers () {
		// Print("gOS_OnDisplayStartDraw_InitMapHandlers");
		if (!SCR_MapEntity.GetOnMapOpen()) {
			return;
		};
        SCR_MapEntity.GetOnMapOpen().Insert(gOS_OnMapOpened);
        SCR_MapEntity.GetOnMapClose().Insert(gOS_OnMapClosed);

	};
	private void gOS_OnDisplayStopDraw_Delete_MapHandlers () {
		// Print("gOS_OnDisplayStopDraw_Delete_MapHandlers");
		if (!SCR_MapEntity.GetOnMapOpen()) {
			return;
		};
        SCR_MapEntity.GetOnMapOpen().Remove(gOS_OnMapOpened);
        SCR_MapEntity.GetOnMapClose().Remove(gOS_OnMapClosed);
	};




	float gOS_m_fFullMap_LastZoom = -1;
	vector gOS_m_vFullMap_LastPan;

	void gOS_OnMapClosed_Delayed (MapConfiguration f_MapCfg) {
	};
	void gOS_OnMapClosed (MapConfiguration f_MapCfg) {

		// if we are closing the minimap, ignore
		if (f_MapCfg.MapEntityMode != EMapEntityMode.FULLSCREEN) {
			return;
		};

        SCR_MapEntity t_EntMap = SCR_MapEntity.GetMapInstance();
		if (!t_EntMap) {
            return;
        };
        float t_fZoomVal = t_EntMap.GetCurrentZoom();
		vector t_vPan = t_EntMap.GetCurrentPan();

		gOS_m_fFullMap_LastZoom = t_fZoomVal;
		gOS_m_vFullMap_LastPan = t_vPan;

		// Print(f_MapCfg);
		// Print(t_fZoomVal);
		// Print(gOS_m_MapCfg);
	};
	void gOS_OnMapOpened_Delayed (MapConfiguration f_MapCfg) {
		if (f_MapCfg.MapEntityMode != EMapEntityMode.FULLSCREEN) {
			return;
		};
        SCR_MapEntity t_EntMap = SCR_MapEntity.GetMapInstance();
		if (!t_EntMap) {
            return;
        };
        float t_fZoomVal = gOS_m_fFullMap_LastZoom;
		if (t_fZoomVal == -1) {
			return;
		};
		float t_fPanX = gOS_m_vFullMap_LastPan[0];
		float t_fPanY = gOS_m_vFullMap_LastPan[1];

		WorkspaceWidget t_wWorkspace = GetGame().GetWorkspace();
		if (t_wWorkspace) {
			t_fPanX = t_wWorkspace.DPIUnscale(t_fPanX);
			t_fPanY = t_wWorkspace.DPIUnscale(t_fPanY);
		};
		t_EntMap.SetPan(
			t_fPanX
			, t_fPanY
			, true
			, false
		);
		if (t_fZoomVal == 0) {
			t_fZoomVal = 1;
		};
		t_EntMap.SetZoom(t_fZoomVal, false);
		Print("gOS_OnMapOpened_Delayed");
	};
	void gOS_OnMapOpened (MapConfiguration f_MapCfg) {
		
		// if we are opening the minimap, ignore
		if (f_MapCfg.MapEntityMode != EMapEntityMode.FULLSCREEN) {
			return;
		};

        SCR_MapEntity t_EntMap = SCR_MapEntity.GetMapInstance();
		if (!t_EntMap) {
            return;
        };

        // IEntity t_IEnt_Player = SCR_PlayerController.GetLocalControlledEntity();
        // if (!t_IEnt_Player) {
        //     return;
        // };
        
        // SCR_GadgetManagerComponent t_Comp_GMan = SCR_GadgetManagerComponent.GetGadgetManager(t_IEnt_Player);
        // if (!t_Comp_GMan) {
        //     return;
        // };
                
        // IEntity t_GadgetMap = t_Comp_GMan.GetGadgetByType(EGadgetType.MAP);
        // if (!t_GadgetMap) {
        //     Print("gOS_SCR_MiniMap_HUD::gOS_OnMapOpened: Missing map!");
        //     return;
        // };

        float t_fZoomVal = gOS_m_fFullMap_LastZoom;

		if (t_fZoomVal == -1) {
			return;
			// t_fZoomVal = gOS_m_fFullMap_LastZoom;
		};
		float t_fPanX = gOS_m_vFullMap_LastPan[0];
		float t_fPanY = gOS_m_vFullMap_LastPan[1];

		WorkspaceWidget t_wWorkspace = GetGame().GetWorkspace();
		if (t_wWorkspace) {
			t_fPanX = t_wWorkspace.DPIUnscale(t_fPanX);
			t_fPanY = t_wWorkspace.DPIUnscale(t_fPanY);
		};



		// t_EntMap.SetZoom(t_fZoomVal - 0.1, true);
		t_EntMap.SetZoom(t_fZoomVal + 0.5, true);
		t_EntMap.SetPan(
			t_fPanX
			, t_fPanY
			, true
			, false
		);

		if (t_fZoomVal == 0) {
			t_fZoomVal = 1;
		};
		t_EntMap.SetZoom(t_fZoomVal, false);

		// this should fix the stuff not loading when opening map(weird bug)
		t_EntMap.Pan(EMapPanMode.HORIZONTAL, 0);
		// t_EntMap.SetZoom(t_fZoomVal + 0.00001, false);

		// t_EntMap.CenterMap();
	
		// Print("gOS_SCR_MiniMap_HUD::gOS_OnMapOpened::X:" + gOS_m_vFullMap_LastPan[0]);
		// Print("gOS_SCR_MiniMap_HUD::gOS_OnMapOpened::Y:" + gOS_m_vFullMap_LastPan[1]);
		// Print("gOS_SCR_MiniMap_HUD::gOS_OnMapOpened::ZoomTo:" + t_fZoomVal);

		// GetGame().GetCallqueue().Remove(gOS_OnMapOpened_Delayed);
		// GetGame().GetCallqueue().CallLater(gOS_OnMapOpened_Delayed, 500, false, f_MapCfg);

	};

	


	//--- Init function to save commonly used widgets
	protected void gOS_OnDisplayStartDraw () {
		
		gOS_OnDisplayStartDraw_InitSettings();
		gOS_OnDisplayStartDraw_InitMapHandlers();

		if (!gOS_SCR_MiniMap_HUD.gOS_s_mWidgetData) {
			return;
		};

		// this will apply all required values
		foreach (string t_sName, int t_iValue: gOS_SCR_MiniMap_HUD.gOS_s_mWidgetData) {
			gOS_ApplyWidgetValue(t_sName, t_iValue);
		};


		m_wFrame0 = m_wRoot.FindAnyWidget("Frame0");
        if (!m_wFrame0) {
            return;
        };
		// Print("gOS::gOS_SCR_MiniMap_HUD::gOS_OnDisplayStartDraw::Complete");

	};


	protected Widget m_wFrame0;


	void gOS_AddKeyEvents () {
        // Print("gOS_AddKeyEvents");
		// GetGame().GetInputManager().AddActionListener(gOS_ACTION_TOGGLE, EActionTrigger.DOWN, gOS_ToggleKeyDown);
		// GetGame().GetInputManager().AddActionListener("MouseWheel", EActionTrigger.PRESSED, gOS_KeyDown_OpenMenu);

		GetGame().GetInputManager().AddActionListener("gOS_MiniMap_PanX_Left", EActionTrigger.PRESSED, gOS_KeyDown_PanX_Left);
		GetGame().GetInputManager().AddActionListener("gOS_MiniMap_PanX_Right", EActionTrigger.PRESSED, gOS_KeyDown_PanX_Right);
		GetGame().GetInputManager().AddActionListener("gOS_MiniMap_PanY_Up", EActionTrigger.PRESSED, gOS_KeyDown_PanY_Up);
		GetGame().GetInputManager().AddActionListener("gOS_MiniMap_PanY_Down", EActionTrigger.PRESSED, gOS_KeyDown_PanY_Down);

		GetGame().GetInputManager().AddActionListener("gOS_MiniMap_Zoom_In", EActionTrigger.PRESSED, gOS_KeyDown_Zoom_In);
		GetGame().GetInputManager().AddActionListener("gOS_MiniMap_Zoom_Out", EActionTrigger.PRESSED, gOS_KeyDown_Zoom_Out);

		GetGame().GetInputManager().AddActionListener("gOS_MiniMap_POI", EActionTrigger.UP, gOS_KeyDown_POI);
		GetGame().GetInputManager().AddActionListener("gOS_MiniMap_Settings", EActionTrigger.UP, gOS_KeyDown_Settings);
		GetGame().GetInputManager().AddActionListener("gOS_MiniMap_Options", EActionTrigger.UP, gOS_KeyDown_Options);
		GetGame().GetInputManager().AddActionListener("gOS_MiniMap_Brightness", EActionTrigger.UP, gOS_KeyDown_Brigthness);

		// GetGame().GetInputManager().AddActionListener("TacticalPing", EActionTrigger.DOWN, gOS_OnToggleKey);
	};

	void gOS_RemoveKeyEvents () {
		// GetGame().GetInputManager().RemoveActionListener(gOS_ACTION_TOGGLE, EActionTrigger.DOWN, gOS_ToggleKeyDown);
		// GetGame().GetInputManager().RemoveActionListener("MouseWheel", EActionTrigger.PRESSED, gOS_KeyDown_OpenMenu);

		GetGame().GetInputManager().RemoveActionListener("gOS_MiniMap_PanX_Left", EActionTrigger.PRESSED, gOS_KeyDown_PanX_Left);
		GetGame().GetInputManager().RemoveActionListener("gOS_MiniMap_PanX_Right", EActionTrigger.PRESSED, gOS_KeyDown_PanX_Right);
		GetGame().GetInputManager().RemoveActionListener("gOS_MiniMap_PanY_Up", EActionTrigger.PRESSED, gOS_KeyDown_PanY_Up);
		GetGame().GetInputManager().RemoveActionListener("gOS_MiniMap_PanY_Down", EActionTrigger.PRESSED, gOS_KeyDown_PanY_Down);

		GetGame().GetInputManager().RemoveActionListener("gOS_MiniMap_Zoom_In", EActionTrigger.PRESSED, gOS_KeyDown_Zoom_In);
		GetGame().GetInputManager().RemoveActionListener("gOS_MiniMap_Zoom_Out", EActionTrigger.PRESSED, gOS_KeyDown_Zoom_Out);
		
		GetGame().GetInputManager().RemoveActionListener("gOS_MiniMap_POI", EActionTrigger.UP, gOS_KeyDown_POI);
		GetGame().GetInputManager().RemoveActionListener("gOS_MiniMap_Settings", EActionTrigger.UP, gOS_KeyDown_Settings);
		GetGame().GetInputManager().RemoveActionListener("gOS_MiniMap_Options", EActionTrigger.UP, gOS_KeyDown_Options);
		GetGame().GetInputManager().RemoveActionListener("gOS_MiniMap_Brightness", EActionTrigger.UP, gOS_KeyDown_Brigthness);
	};

	private float gOS_m_fPanScale = 1;
	private float gOS_m_fZoomScale = 1;

	void gOS_KeyDown_PanX_Left (float value, EActionTrigger trigger) {
		if (gOS_m_bIsPlacing) {
			gOS_m_fPos_Place = gOS_m_fPos_Place + "0 0 0.01";

			return;
		};


		gOS_m_fControl_PanX = gOS_m_fControl_PanX - gOS_m_fPanScale;

        gOS_m_bControl_POI_Requested = false;
	};
	void gOS_KeyDown_PanX_Right (float value, EActionTrigger trigger) {
		if (gOS_m_bIsPlacing) {
			gOS_m_fPos_Place = gOS_m_fPos_Place + "0 0 -0.01";

			return;
		};


		gOS_m_fControl_PanX = gOS_m_fControl_PanX + gOS_m_fPanScale;

        gOS_m_bControl_POI_Requested = false;
	};
	void gOS_KeyDown_PanY_Up (float value, EActionTrigger trigger) {
		if (gOS_m_bIsPlacing) {
			gOS_m_fPos_Place = gOS_m_fPos_Place + "0 0.01 0.0";

			return;
		};


		gOS_m_fControl_PanY = gOS_m_fControl_PanY - gOS_m_fPanScale;

        gOS_m_bControl_POI_Requested = false;
	};
	void gOS_KeyDown_PanY_Down (float value, EActionTrigger trigger) {
		if (gOS_m_bIsPlacing) {
			gOS_m_fPos_Place = gOS_m_fPos_Place + "0 -0.01 0.0";

			return;
		};


		gOS_m_fControl_PanY = gOS_m_fControl_PanY + gOS_m_fPanScale;

        gOS_m_bControl_POI_Requested = false;

		// Print("gOS_KeyDown_PanY_Down::Y:" + gOS_m_fControl_PanY + " val:" + value);
	};

	void gOS_KeyDown_Zoom_In (float value, EActionTrigger trigger) {
		if (gOS_m_bIsPlacing) {
			gOS_m_fPos_Place = gOS_m_fPos_Place + "0.01 0 0";

			return;
		};


		// gOS_m_fControl_Zoom = gOS_m_fControl_Zoom + 1;
		// gOS_m_fControl_Zoom = gOS_m_fControl_Zoom + (10 * System.GetFrameTimeS());
		gOS_m_fControl_Zoom = gOS_m_fControl_Zoom + gOS_m_fZoomScale;
	};
	void gOS_KeyDown_Zoom_Out (float value, EActionTrigger trigger) {
		if (gOS_m_bIsPlacing) {
			gOS_m_fPos_Place = gOS_m_fPos_Place + "-0.01 0 0";

			return;
		};


		// gOS_m_fControl_Zoom = gOS_m_fControl_Zoom - 1;
		// gOS_m_fControl_Zoom = gOS_m_fControl_Zoom - (10 * System.GetFrameTimeS());
		gOS_m_fControl_Zoom = gOS_m_fControl_Zoom - gOS_m_fZoomScale;

		// Print("gOS_KeyDown_Zoom_Out::Y:" + gOS_m_fControl_Zoom + " val:" + value);
	};

	void gOS_KeyDown_POI (float value, EActionTrigger trigger) {
		if (gOS_m_bIsPlacing && gOS_m_IEnt_Place_Test) {
			Print("gOS_KeyDown_POI::placing start");
			
			if (!m_Comp_MiniMap) {
				Print("gOS_KeyDown_POI::Missing minimap comp");
				return;
			};

			// generate relative position:
			vector t_aMat_Place[4];
			gOS_m_IEnt_Place_Test.GetWorldTransform(t_aMat_Place);

			// check if player is in car:
			IEntity player = SCR_PlayerController.GetLocalControlledEntity();
			if (!player) {
				Print("gOS_KeyDown_POI::Missing player");
				return;
			};
			SCR_ChimeraCharacter t_Char = SCR_ChimeraCharacter.Cast(player);
			if (!t_Char) {
				Print("gOS_KeyDown_POI::Missing char");
				return;
			};
			CompartmentAccessComponent t_Comp_CompartmentAccess = t_Char.GetCompartmentAccessComponent();
			if (
				!t_Comp_CompartmentAccess
				|| !t_Comp_CompartmentAccess.GetCompartment()
				|| !t_Char.IsInVehicle()
			) {

				Print("gOS_KeyDown_POI::Missing compartment");
				return;
			};	
			Vehicle t_Vehicle = Vehicle.Cast(CompartmentAccessComponent.GetVehicleIn(t_Char));
			RplId t_RplID_Veh = Replication.FindId(t_Vehicle);
			RplId t_RplID_Player = Replication.FindId(t_Char);

			// get relative position:
			vector t_aMat_Veh[4];
			t_Vehicle.GetWorldTransform(t_aMat_Veh);

			Math3D.MatrixMultiply4(t_aMat_Veh, t_aMat_Place, t_aMat_Place);
			
			
			if (gOS_m_bIsPlaced_Map_Enabled) {
				gOS_m_bIsPlaced_Map_Enabled = false;

				// flip the map on-off
				gOS_MiniMap_SetEnabled(false);
				gOS_MiniMap_SetEnabled(true);
			};
				
			// remove old if it exists
			m_Comp_MiniMap.gOS_C2S_Place_Remove();

			m_Comp_MiniMap.gOS_C2S_Place(
				t_aMat_Place[0]
				, t_aMat_Place[1]
				, t_aMat_Place[2]
				, t_aMat_Place[3]
				, t_Vehicle
				, m_bIsPaperMap
			);

			//	enable the placed map immediately for conviencenes
			gOS_MiniMap_SetEnabled_Placed_Map();


			// m_Comp_MiniMap.Rpc(
			// 	m_Comp_MiniMap.gOS_C2S_Place
			// 	, t_aMat_Place[0]
			// 	, t_aMat_Place[1]
			// 	, t_aMat_Place[2]
			// 	, t_aMat_Place[3]
			// 	, t_RplID_Veh
			// 	, m_bIsPaperMap
			// 	// , t_RplID_Player
			// 	// , rplComponent.Id(), characterRplComp.Id()
			// );

			gOS_m_bIsPlacing = false;

			return;
		};


		// gOS_m_bControl_POI_Requested = true;
		gOS_m_bControl_POI_Requested = !gOS_m_bControl_POI_Requested;
		// Print("gOS_m_bControl_POI_Requested");
	};
	void gOS_KeyDown_Brigthness (float value, EActionTrigger trigger) {

		BaseContainer t_Settings_gOS = GetGame().GetGameUserSettings().GetModule("gOS_SCR_Settings");
		if (!t_Settings_gOS.Get("m_iBrightness", gOS_m_iBrightness)) {
			gOS_m_iBrightness = 100;
		};

		gOS_m_iBrightness = gOS_m_iBrightness + 50;
		if (gOS_m_iBrightness > 100) {
			gOS_m_iBrightness = 0;
		};
		// Print(gOS_m_iBrightness);
		// Print("gOS_KeyDown_Brigthness:" + gOS_m_iBrightness);
		t_Settings_gOS.Set("m_iBrightness", gOS_m_iBrightness);

		// gOS_Brigthness_Set(gOS_m_iBrightness);
		
		// SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		// if (playerController) {
			// playerController.SetGameUserSettings();
        // };
		// GetGame().ApplySettingsPreset();
		// GetGame().UserSettingsChanged();

		GetGame().UserSettingsChanged();
		GetGame().SaveUserSettings();
	};


	//! TODO:
	//	- edit the texture of the SPECIFIC map only, currently changes global material
	void gOS_Brigthness_Set (float f_fValue) {
		// Print("gOS_Brigthness_Set");
		ResourceName mat = "{25BBE0B8CE47F38F}Assets/Data/Material_RT.emat";

		// if paper map:
		if (m_bIsPaperMap) {
			mat = "{1A053271B1A6AA74}Assets/Data/Material_RT_Paper.emat";
			// return;
		};


		Material t_Mat = Material.GetOrLoadMaterial(mat, 0);
		if (!t_Mat) {
			return;
		};
		
        float t_fBrightness = Math.InverseLerp(0, 100, f_fValue);
        t_fBrightness = Math.Clamp(t_fBrightness, 0, 1);
        t_fBrightness = Math.Lerp(-12, -3, t_fBrightness);	// old
        // t_fBrightness = Math.Lerp(-12, 1, t_fBrightness);	// new, better realistic screen
		if (m_bIsPaperMap) {
			t_fBrightness = Math.InverseLerp(0, 100, f_fValue);
			t_fBrightness = Math.Clamp(t_fBrightness, 0, 1);
			t_fBrightness = Math.Lerp(-12, 0, t_fBrightness);	// old

		};

		// * either the max is -3 LV
		// * or +6 LV with the COLOR disabled

		// Print(t_fBrightness);
		t_Mat.SetParam("EmissiveLV", t_fBrightness);
		//t_Mat.Release();
	};






























































































	void gOS_RadUI_Exit () {

		HCA_RadUI_HUD t_RadUI_HUD = HCA_RadUI_HUD.HCA_GetCurrent();
		if (!t_RadUI_HUD) {
			return;
		};

		t_RadUI_HUD.HCA_RadUI_Close();
	};

	void gOS_RadUI_Init () {

		// return;
		HCA_RadUI_Data t_Data;
		t_Data = new HCA_RadUI_Data;
		t_Data.HCA_m_bOnExecute_Close = false;
		t_Data.m_bIsMainMenu = false;
		t_Data.m_sID = "vPad_Category";
		t_Data.m_sName = "vPad Options";
		t_Data.m_sDesc = "vPad related options";
		t_Data.m_rImg = "{53E7AB5887CFFE00}UI/Textures/Editor/ContentBrowser/ContentBrowser_Trait_Armed.edds";
		HCA_RadUI_Man.SAdd(t_Data);
		t_Data.HCA_m_Invoker_OnExecute.Insert(gOS_RadUI_Cat);

		t_Data = new HCA_RadUI_Data;
		t_Data.HCA_m_bOnExecute_Close = false;
		t_Data.m_sID = "gOS_RadUI_Category_Brightness";
		t_Data.m_sName = "BackLight";
		t_Data.m_sDesc = "Brightness of the screen";
		t_Data.m_rImg = "{1B9DD6FBA753EC1C}UI/Textures/Editor/Toolbar/ToolbarAction_Flashlight.edds";
		t_Data.m_rImgSet = "{8705013792245F5F}UI/vPad_Icons.imageset";
		t_Data.m_sImageSet = "light_settings";
		t_Data.m_bIsImageSet = true;
		HCA_RadUI_Man.SAdd(t_Data);
		t_Data.Parent_Set("vPad_Category");
		t_Data.HCA_m_Invoker_OnExecute.Insert(gOS_RadUI_Cat);


		
		t_Data = new HCA_RadUI_Data;
		t_Data.HCA_m_bOnExecute_Close = false;
		t_Data.m_sID = "gOS_RadUI_Bright_0";
		t_Data.m_sName = "0 %";
		t_Data.m_sDesc = "Lowest and darkest backlight setting.";
		t_Data.m_rImg = "{B6289DB4FF8C3EEA}UI/Textures/Editor/Toolbar/ToolbarAction_Flashlight_Off.edds";
		t_Data.m_rImgSet = "{8705013792245F5F}UI/vPad_Icons.imageset";
		t_Data.m_sImageSet = "light_off";
		t_Data.m_bIsImageSet = true;
		HCA_RadUI_Man.SAdd(t_Data);
		t_Data.Parent_Set("gOS_RadUI_Category_Brightness");
		t_Data.HCA_m_Invoker_OnExecute.Insert(gOS_RadUI_Bright_0);

		t_Data = new HCA_RadUI_Data;
		t_Data.HCA_m_bOnExecute_Close = false;
		t_Data.m_sID = "gOS_RadUI_Bright_100";
		t_Data.m_sName = "100 %";
		t_Data.m_sDesc = "Highest and brightest backlight setting.";
		t_Data.m_rImg = "{120C61A156A70EDA}UI/Textures/Editor/Toolbar/ToolbarAction_Flashlight_On.edds";
		t_Data.m_rImgSet = "{8705013792245F5F}UI/vPad_Icons.imageset";
		t_Data.m_sImageSet = "light";
		t_Data.m_bIsImageSet = true;
		HCA_RadUI_Man.SAdd(t_Data);
		t_Data.Parent_Set("gOS_RadUI_Category_Brightness");
		t_Data.HCA_m_Invoker_OnExecute.Insert(gOS_RadUI_Bright_100);



		t_Data = new HCA_RadUI_Data;
		t_Data.HCA_m_bOnExecute_Close = false;
		t_Data.m_sID = "gOS_RadUI_Bright_Current";
		t_Data.m_sName = "Current";
		t_Data.m_sDesc = "Current Value";
		t_Data.m_rImg = "{27D94704CC78A113}UI/Textures/Editor/ContextMenu/ContextAction_SnapToSurface.edds";
		t_Data.m_rImgSet = "{8705013792245F5F}UI/vPad_Icons.imageset";
		t_Data.m_sImageSet = "light_settings";
		t_Data.m_bIsImageSet = true;
		HCA_RadUI_Man.SAdd(t_Data);
		t_Data.Parent_Set("gOS_RadUI_Category_Brightness");
		t_Data.HCA_m_Invoker_OnDraw.Insert(gOS_RadUI_Bright_Current_OnDraw);
		// t_Data.HCA_m_Invoker_OnExecute.Insert(gOS_RadUI_Bright_Down);

		t_Data = new HCA_RadUI_Data;
		t_Data.HCA_m_bOnExecute_Close = false;
		t_Data.m_sID = "gOS_RadUI_Bright_Up";
		t_Data.m_sName = "Increase";
		t_Data.m_sDesc = "Raise the backlight setting by 10%.";
		t_Data.m_rImg = "{53B084C9E0486C1E}UI/Textures/Editor/ContextMenu/ContextAction_MoveToFlightAlt.edds";
		t_Data.m_rImgSet = "{8705013792245F5F}UI/vPad_Icons.imageset";
		t_Data.m_sImageSet = "light_up";
		t_Data.m_bIsImageSet = true;
		HCA_RadUI_Man.SAdd(t_Data);
		t_Data.Parent_Set("gOS_RadUI_Category_Brightness");
		t_Data.HCA_m_Invoker_OnExecute.Insert(gOS_RadUI_Bright_Up);

		t_Data = new HCA_RadUI_Data;
		t_Data.HCA_m_bOnExecute_Close = false;
		t_Data.m_sID = "gOS_RadUI_Bright_Down";
		t_Data.m_sName = "Decrease";
		t_Data.m_sDesc = "Lower the backlight setting by 10%.";
		t_Data.m_rImg = "{27D94704CC78A113}UI/Textures/Editor/ContextMenu/ContextAction_SnapToSurface.edds";
		t_Data.m_rImgSet = "{8705013792245F5F}UI/vPad_Icons.imageset";
		t_Data.m_sImageSet = "light_down";
		t_Data.m_bIsImageSet = true;
		HCA_RadUI_Man.SAdd(t_Data);
		t_Data.Parent_Set("gOS_RadUI_Category_Brightness");
		t_Data.HCA_m_Invoker_OnExecute.Insert(gOS_RadUI_Bright_Down);



		// t_Data = new HCA_RadUI_Data;
		// t_Data.HCA_m_bOnExecute_Close = false;
		// t_Data.m_sID = "gOS_RadUI_Clean_To_10";
		// t_Data.m_sName = "Clean To 10";
		// t_Data.m_sDesc = "Clean all except the 10 newest hits";
		// t_Data.m_rImg = "{9164E45B9A237FE9}UI/Textures/Editor/EditableEntities/Characters/EditableEntity_Character_Unarmed.edds";
		// HCA_RadUI_Man.SAdd(t_Data);
		// t_Data.Parent_Set("gOS_RadUI_Category_Brightness");
		// t_Data.HCA_m_Invoker_OnExecute.Insert(gOS_RadUI_Clean_To_10);


		// t_Data = new HCA_RadUI_Data;
		// t_Data.HCA_m_bOnExecute_Close = false;
		// t_Data.m_sID = "gOS_RadUI_Toggle";
		// t_Data.m_sName = "Toggle";
		// t_Data.m_sDesc = "Toggle the display of server/client hits on off. Server hits have text on the right, Character hits are red.";
		// t_Data.m_rImg = "{9164E45B9A237FE9}UI/Textures/Editor/EditableEntities/Characters/EditableEntity_Character_Unarmed.edds";
		// HCA_RadUI_Man.SAdd(t_Data);
		// t_Data.Parent_Set("vPad_Category");
		// t_Data.HCA_m_Invoker_OnExecute.Insert(gOS_RadUI_Toggle);
		// t_Data.HCA_m_Invoker_OnDraw.Insert(gOS_RadUI_Toggle_OnDraw);



		
		// t_Data = new HCA_RadUI_Data;
		// t_Data.HCA_m_bOnExecute_Close = false;
		// t_Data.m_sID = "gOS_RadUI_Toggle_Damage";
		// t_Data.m_sName = "Toggle Damage";
		// t_Data.m_sDesc = "Toggle the display of damage dealt via dbg HUD.";
		// t_Data.m_rImg = "{9164E45B9A237FE9}UI/Textures/Editor/EditableEntities/Characters/EditableEntity_Character_Unarmed.edds";
		// HCA_RadUI_Man.SAdd(t_Data);
		// t_Data.Parent_Set("vPad_Category");

		// t_Data.HCA_m_Invoker_OnExecute.Insert(gOS_RadUI_Toggle_Damage);
		// t_Data.HCA_m_Invoker_OnDraw.Insert(gOS_RadUI_Toggle_Damage_OnDraw);

		
		t_Data = new HCA_RadUI_Data;
		t_Data.HCA_m_bOnExecute_Close = false;
		t_Data.m_sID = "gOS_RadUI_Category_MiniMap";
		t_Data.m_sName = "MiniMap";
		t_Data.m_sDesc = "Options related to the minimap, such as markers and stuff";
		t_Data.m_rImg = "{1B9DD6FBA753EC1C}UI/Textures/Editor/Toolbar/ToolbarAction_Flashlight.edds";
		t_Data.m_rImgSet = "{8705013792245F5F}UI/vPad_Icons.imageset";
		t_Data.m_sImageSet = "marker_settings";
		t_Data.m_bIsImageSet = true;
		HCA_RadUI_Man.SAdd(t_Data);
		t_Data.Parent_Set("vPad_Category");
		t_Data.HCA_m_Invoker_OnExecute.Insert(gOS_RadUI_Cat);

		t_Data = new HCA_RadUI_Data;
		t_Data.HCA_m_bOnExecute_Close = false;
		t_Data.m_sID = "gOS_RadUI_Markers_Unit_Toggle";
		t_Data.m_sName = "Markers: unit";
		t_Data.m_sDesc = "Toggle Unit markers on/off";
		t_Data.m_rImg = "{02714C672E89A0C7}UI/Textures/Editor/Toolbar/ToolbarAction_AttributesBase.edds";
		t_Data.m_rImgSet = "{8705013792245F5F}UI/vPad_Icons.imageset";
		t_Data.m_sImageSet = "marker_unit";
		t_Data.m_bIsImageSet = true;
		HCA_RadUI_Man.SAdd(t_Data);
		t_Data.Parent_Set("gOS_RadUI_Category_MiniMap");
		t_Data.HCA_m_Invoker_OnExecute.Insert(gOS_RadUI_Markers_Unit_Toggle);
		t_Data.HCA_m_Invoker_OnDraw.Insert(gOS_RadUI_Markers_Unit_Toggle_OnDraw);

		t_Data = new HCA_RadUI_Data;
		t_Data.HCA_m_bOnExecute_Close = false;
		t_Data.m_sID = "gOS_RadUI_Markers_Unit_Toggle_Fade";
		t_Data.m_sName = "Markers: unit FADING";
		t_Data.m_sDesc = "Toggle Unit markers FADING on/off. Fading will make the unit markers invisible when zooming out the minimap.";
		t_Data.m_rImg = "{02714C672E89A0C7}UI/Textures/Editor/Toolbar/ToolbarAction_AttributesBase.edds";
		HCA_RadUI_Man.SAdd(t_Data);
		t_Data.Parent_Set("gOS_RadUI_Category_MiniMap");
		t_Data.HCA_m_Invoker_OnExecute.Insert(gOS_RadUI_Markers_Unit_Toggle_Fade);
		t_Data.HCA_m_Invoker_OnDraw.Insert(gOS_RadUI_Markers_Unit_Toggle_Fade_OnDraw);

		t_Data = new HCA_RadUI_Data;
		t_Data.HCA_m_bOnExecute_Close = false;
		t_Data.m_sID = "gOS_RadUI_Markers_Group_Toggle";
		t_Data.m_sName = "Markers: group";
		t_Data.m_sDesc = "Toggle group markers on/off.";
		t_Data.m_rImg = "{02714C672E89A0C7}UI/Textures/Editor/Toolbar/ToolbarAction_AttributesBase.edds";
		t_Data.m_rImgSet = "{8705013792245F5F}UI/vPad_Icons.imageset";
		t_Data.m_sImageSet = "marker_group";
		t_Data.m_bIsImageSet = true;
		HCA_RadUI_Man.SAdd(t_Data);
		t_Data.Parent_Set("gOS_RadUI_Category_MiniMap");
		t_Data.HCA_m_Invoker_OnExecute.Insert(gOS_RadUI_Markers_Group_Toggle);
		t_Data.HCA_m_Invoker_OnDraw.Insert(gOS_RadUI_Markers_Group_Toggle_OnDraw);
		

		t_Data = new HCA_RadUI_Data;
		t_Data.HCA_m_bOnExecute_Close = false;
		t_Data.m_sID = "gOS_RadUI_Markers_Group_Toggle_Fade";
		t_Data.m_sName = "Markers: group Fading";
		t_Data.m_sDesc = "Toggle fading group markers on/off. This makes the group text smaller when zooming in";
		t_Data.m_rImg = "{02714C672E89A0C7}UI/Textures/Editor/Toolbar/ToolbarAction_AttributesBase.edds";
		HCA_RadUI_Man.SAdd(t_Data);
		t_Data.Parent_Set("gOS_RadUI_Category_MiniMap");
		t_Data.HCA_m_Invoker_OnExecute.Insert(gOS_RadUI_Markers_Group_Toggle_Fade);
		t_Data.HCA_m_Invoker_OnDraw.Insert(gOS_RadUI_Markers_Group_Toggle_Fade_OnDraw);

		t_Data = new HCA_RadUI_Data;
		t_Data.HCA_m_bOnExecute_Close = false;
		t_Data.m_sID = "gOS_RadUI_Exp_Action_Enable";
		t_Data.m_sName = "Exp: Enable interaction action";
		t_Data.m_sDesc = "enables the interaction on map objects";
		t_Data.m_rImg = "{02714C672E89A0C7}UI/Textures/Editor/Toolbar/ToolbarAction_AttributesBase.edds";
		HCA_RadUI_Man.SAdd(t_Data);
		t_Data.Parent_Set("gOS_RadUI_Category_MiniMap");
		t_Data.HCA_m_Invoker_OnExecute.Insert(gOS_RadUI_Exp_Action_Enable);
		t_Data.HCA_m_Invoker_OnDraw.Insert(gOS_RadUI_Exp_Action_Enable_OnDraw);








		t_Data = new HCA_RadUI_Data;
		t_Data.HCA_m_bOnExecute_Close = true;
		t_Data.m_sID = "gOS_RadUI_ToSettings";
		t_Data.m_sName = "Settings";
		t_Data.m_sDesc = "Go into the settings panel for adjusting the user settings and options.";
		t_Data.m_rImg = "{02714C672E89A0C7}UI/Textures/Editor/Toolbar/ToolbarAction_AttributesBase.edds";
		HCA_RadUI_Man.SAdd(t_Data);
		t_Data.Parent_Set("vPad_Category");
		t_Data.HCA_m_Invoker_OnExecute.Insert(gOS_RadUI_ToSettings);


		t_Data = new HCA_RadUI_Data;
		t_Data.HCA_m_bOnExecute_Close = false;
		t_Data.m_sID = "gOS_RadUI_Place";
		t_Data.m_sName = "Hook To Any Hoist";
		t_Data.m_sDesc = "Hook the map to the vehicle";
		t_Data.m_rImg = "{02714C672E89A0C7}UI/Textures/Editor/Toolbar/ToolbarAction_AttributesBase.edds";
		HCA_RadUI_Man.SAdd(t_Data);
		t_Data.Parent_Set("vPad_Category");
		// t_Data.HCA_m_Invoker_OnExecute.Insert(gOS_RadUI_Place);

		t_Data = new HCA_RadUI_Data;
		t_Data.HCA_m_bOnExecute_Close = true;
		t_Data.m_sID = "gOS_RadUI_Place_Setup";
		t_Data.m_sName = "Place";
		t_Data.m_sDesc = "Placement mode, allows placing the minimap. YOU MUST BE IN A VEHICLE";
		t_Data.m_rImg = "{02714C672E89A0C7}UI/Textures/Editor/Toolbar/ToolbarAction_AttributesBase.edds";
		HCA_RadUI_Man.SAdd(t_Data);
		t_Data.Parent_Set("gOS_RadUI_Place");
		t_Data.HCA_m_Invoker_OnExecute.Insert(gOS_RadUI_Place);

		t_Data = new HCA_RadUI_Data;
		t_Data.HCA_m_bOnExecute_Close = true;
		t_Data.m_sID = "gOS_RadUI_Place_Delete";
		t_Data.m_sName = "Delete";
		t_Data.m_sDesc = "Delete the minimap placed in vehicle";
		t_Data.m_rImg = "{02714C672E89A0C7}UI/Textures/Editor/Toolbar/ToolbarAction_AttributesBase.edds";
		HCA_RadUI_Man.SAdd(t_Data);
		t_Data.Parent_Set("gOS_RadUI_Place");
		t_Data.HCA_m_Invoker_OnExecute.Insert(gOS_RadUI_Place_Delete);

		t_Data = new HCA_RadUI_Data;
		t_Data.HCA_m_bOnExecute_Close = false;
		t_Data.m_sID = "gOS_RadUI_Place_Enable";
		t_Data.m_sName = "Enable";
		t_Data.m_sDesc = "Switch the minimap to the placed minimap";
		t_Data.m_rImg = "{02714C672E89A0C7}UI/Textures/Editor/Toolbar/ToolbarAction_AttributesBase.edds";
		HCA_RadUI_Man.SAdd(t_Data);
		t_Data.Parent_Set("gOS_RadUI_Place");
		t_Data.HCA_m_Invoker_OnExecute.Insert(gOS_RadUI_Place_Enable);
		t_Data.HCA_m_Invoker_OnDraw.Insert(gOS_RadUI_Place_Enable_OnDraw);

		t_Data = new HCA_RadUI_Data;
		t_Data.HCA_m_bOnExecute_Close = false;
		t_Data.m_sID = "gOS_RadUI_Place_Disable_Keys";
		t_Data.m_sName = "UnBlock";
		t_Data.m_sDesc = "Block(Disable) keybinds for minimap (except this one and open/close map)";
		t_Data.m_rImg = "{02714C672E89A0C7}UI/Textures/Editor/Toolbar/ToolbarAction_AttributesBase.edds";
		HCA_RadUI_Man.SAdd(t_Data);
		t_Data.Parent_Set("gOS_RadUI_Place");
		t_Data.HCA_m_Invoker_OnExecute.Insert(gOS_RadUI_Place_Disable_Keys);
		t_Data.HCA_m_Invoker_OnDraw.Insert(gOS_RadUI_Place_Disable_Keys_OnDraw);

		// t_Data.HCA_m_Invoker_OnDraw.Insert(gOS_RadUI_Toggle_OnDraw);
	};
	void gOS_RadUI_Bright_0 (HCA_RadUI_Data f_Option) {
		gOS_m_iBrightness = 0;
		gOS_Brigthness_Set(gOS_m_iBrightness);
	};
	void gOS_RadUI_Bright_100 (HCA_RadUI_Data f_Option) {
		gOS_m_iBrightness = 100;
		gOS_Brigthness_Set(gOS_m_iBrightness);
	};
	void gOS_RadUI_Bright_Up (HCA_RadUI_Data f_Option) {
		gOS_m_iBrightness = gOS_m_iBrightness + 10;
		if (gOS_m_iBrightness > 100) {
			gOS_m_iBrightness = 100;
		};
		gOS_Brigthness_Set(gOS_m_iBrightness);
	};
	void gOS_RadUI_Bright_Down (HCA_RadUI_Data f_Option) {
		gOS_m_iBrightness = gOS_m_iBrightness - 10;
		if (gOS_m_iBrightness < 0) {
			gOS_m_iBrightness = 0;
		};
		gOS_Brigthness_Set(gOS_m_iBrightness);		
	};

	void gOS_RadUI_Bright_Current_OnDraw (HCA_RadUI_Data f_Option) {
		if (!f_Option.m_wItem) {
			return;
		};
		// Print("gOS_RadUI_Bright_Current_OnDraw");
		//! TODO:
		//	- Maybe add a universal function in the option to mess with the text / icon
		TextWidget t_wText = TextWidget.Cast(f_Option.m_wItem.FindAnyWidget("m_Text"));
		ImageWidget t_wImg = ImageWidget.Cast(f_Option.m_wItem.FindAnyWidget("m_Icon"));
		if (!t_wText || !t_wImg) {
			return;
		};
		// t_wText.SetText(m_sName);
		string t_sText;

		t_sText = "Brightness:" + gOS_m_iBrightness;

		t_wText.SetText(t_sText);
	};



	void gOS_RadUI_Clean_To_10 (HCA_RadUI_Data f_Option) {
		
	};
	void gOS_RadUI_ToSettings (HCA_RadUI_Data f_Option) {
		gOS_Settings_JumpTo();
	};
	void gOS_RadUI_Place (HCA_RadUI_Data f_Option) {
		if (gOS_m_bIsPlacing) {
			gOS_m_bIsPlacing = false;
			return;
		};

		// check if player is in car:
		IEntity player = SCR_PlayerController.GetLocalControlledEntity();
		if (!player) {
			Print("gOS_RadUI_Place::Missing player");
			return;
		};
		SCR_ChimeraCharacter t_Char = SCR_ChimeraCharacter.Cast(player);
		if (!t_Char) {
			Print("gOS_RadUI_Place::Missing char");
			return;
		};
		CompartmentAccessComponent t_Comp_CompartmentAccess = t_Char.GetCompartmentAccessComponent();
		if (
			!t_Comp_CompartmentAccess
			|| !t_Comp_CompartmentAccess.GetCompartment()
			|| !t_Char.IsInVehicle()
		) {

			Print("gOS_RadUI_Place::Missing compartment");
			return;
		};








		gOS_RadUI_Exit();
		gOS_m_bIsPlacing = true;
		gOS_m_fPos_Place = "0 0 0";
		GetGame().GetCallqueue().CallLater(gOS_Place_Loop, 1, false);
	};
	void gOS_RadUI_Place_Delete (HCA_RadUI_Data f_Option) {
		if (gOS_m_bIsPlacing) {
			gOS_m_bIsPlacing = false;
			return;
		};
		gOS_RadUI_Exit();
		gOS_m_bIsPlacing = false;
		// gOS_m_fPos_Place = "0 0 0";
		// GetGame().GetCallqueue().CallLater(gOS_Place_Loop, 1, false);

		
		if (gOS_m_bIsPlaced_Map_Enabled) {
			gOS_m_bIsPlaced_Map_Enabled = false;

			// // hack to override allow SetEnabled to run again from scratch
			// if (gOS_m_bIsEnabled) {
			// 	gOS_m_bIsEnabled = false;
			// };

			//! TEST: can this disable and enable the map on the same frame?
			//	- Need delay or sth?
			gOS_MiniMap_SetEnabled(false);
			gOS_MiniMap_SetEnabled(true);
		};
		if (!m_Comp_MiniMap) {
			Print("gOS_RadUI_Place_Delete::Missing minimap comp");
			return;
		};
		m_Comp_MiniMap.gOS_C2S_Place_Remove();
	};
	static bool gOS_m_bIsPlaced_Map_Enabled = false;
	void gOS_RadUI_Place_Enable (HCA_RadUI_Data f_Option) {
		

		// toggle if already enbabled
		if (gOS_m_bIsPlaced_Map_Enabled) {
			gOS_m_bIsPlaced_Map_Enabled = false;

			// // hack to override allow SetEnabled to run again from scratch
			// if (gOS_m_bIsEnabled) {
			// 	gOS_m_bIsEnabled = false;
			// };

			//! TEST: can this disable and enable the map on the same frame?
			//	- Need delay or sth?
			gOS_MiniMap_SetEnabled(false);
			gOS_MiniMap_SetEnabled(true);
			return;
		};


		// gOS_RadUI_Exit();
		// gOS_m_bIsPlacing = true;
		// gOS_m_fPos_Place = "0 0 0";
		// GetGame().GetCallqueue().CallLater(gOS_Place_Loop, 1, false);

		gOS_MiniMap_SetEnabled_Placed_Map();
	};
	void gOS_RadUI_Place_Enable_OnDraw (HCA_RadUI_Data f_Option) {
		if (!f_Option.m_wItem) {
			return;
		};
		// Print("gOS_RadUI_Bright_Current_OnDraw");
		//! TODO:
		//	- Maybe add a universal function in the option to mess with the text / icon
		TextWidget t_wText = TextWidget.Cast(f_Option.m_wItem.FindAnyWidget("m_Text"));
		ImageWidget t_wImg = ImageWidget.Cast(f_Option.m_wItem.FindAnyWidget("m_Icon"));
		if (!t_wText || !t_wImg) {
			return;
		};
		// t_wText.SetText(m_sName);
		string t_sText = "Enable";
		if (gOS_m_bIsPlaced_Map_Enabled) {
			t_sText = "Disable";
		};

		t_wText.SetText(t_sText);
	};
	static bool gOS_m_bIsPlaced_Block_Keys = false;
	void gOS_RadUI_Place_Disable_Keys (HCA_RadUI_Data f_Option) {
		
		gOS_m_bIsPlaced_Block_Keys = !gOS_m_bIsPlaced_Block_Keys;

		// // toggle if already enbabled
		// if (gOS_m_bIsPlaced_Block_Keys) {
		// 	gOS_m_bIsPlaced_Block_Keys = false;
		// };
	};
	void gOS_RadUI_Place_Disable_Keys_OnDraw (HCA_RadUI_Data f_Option) {
		if (!f_Option.m_wItem) {
			return;
		};
		// Print("gOS_RadUI_Bright_Current_OnDraw");
		//! TODO:
		//	- Maybe add a universal function in the option to mess with the text / icon
		TextWidget t_wText = TextWidget.Cast(f_Option.m_wItem.FindAnyWidget("m_Text"));
		ImageWidget t_wImg = ImageWidget.Cast(f_Option.m_wItem.FindAnyWidget("m_Icon"));
		if (!t_wText || !t_wImg) {
			return;
		};
		// t_wText.SetText(m_sName);
		string t_sText = "Enable Keys";
		if (!gOS_m_bIsPlaced_Block_Keys) {
			t_sText = "Disable Keys";
		};

		t_wText.SetText(t_sText);
	};



	void gOS_Place_Loop () {
		if (!gOS_m_bIsPlacing) {
			if (gOS_m_IEnt_Place_Test) {
				SCR_EntityHelper.DeleteEntityAndChildren(gOS_m_IEnt_Place_Test);
			};
			return;
		};

		vector t_aMat_Cam[4];
		GetGame().GetWorld().GetCurrentCamera(t_aMat_Cam);
		// vector t_vDir = 
		
		// if (!GetGame() || !GetGame().GetCameraManager()) {
		// 	Print("gOS_Place_Loop::no cam man");			
		// 	return;
		// };
		// CameraBase t_Cam = GetGame().GetCameraManager().CurrentCamera();
		// if (!t_Cam) {
		// 	Print("gOS_Place_Loop::no cam");
		// 	return;
		// };
		// vector t_aMat_Cam_World[4];
		// t_Cam.GetWorldCameraTransform(t_aMat_Cam_World);
		if (!m_IEnt_Map_Box) {
			Print("gOS_Place_Loop::no map box");

			// if no box found and map is supposedly enabled:
			if (gOS_m_bIsEnabled) {
				gOS_MiniMap_SetEnabled(false);
				gOS_MiniMap_SetEnabled(true);
			};
			return;
		};

		if (!gOS_m_IEnt_Place_Test) {
			IEntity t_IEnt_Map = m_IEnt_Map_Box.GetParent();
			if (!t_IEnt_Map) {
				Print("gOS_Place_Loop::no map");
				return;
			};
			EntityPrefabData t_EntPrefabData = t_IEnt_Map.GetPrefabData();
			if (!t_EntPrefabData) {
				Print("gOS_Place_Loop::no prefab data");
				return;
			};
			ResourceName t_sRsc_Map = t_EntPrefabData.GetPrefabName();
			// Print(t_sRsc_Map);
			Resource t_rMap = Resource.Load(t_sRsc_Map);
			// Resource t_rMap = Resource.Load("{03B44EA7652D0D17}Prefabs/Props/Military/Radios/RadioStation_R123M_01.et");
			// "{4E63B2F6FAE7CE11}Prefabs/Items/Equipment/Maps/vPad_Paper.et"
			gOS_m_IEnt_Place_Test = GetGame().SpawnEntityPrefabLocal(t_rMap, GetGame().GetWorld());

			IEntity t_IEnt_Player = SCR_PlayerController.GetLocalControlledEntity();
			if (!t_IEnt_Player) {
				return;
			};
			// gOS_m_IEnt_Place_Test.SetScale(2);

			// gOS_m_IEnt_Place_Test.SetOrigin(t_IEnt_Player.GetOrigin());
			// Print(t_aMat_Cam[3]);
		};

		// Print(t_aMat_Cam);
		// Print(t_aMat_Cam_World);

		vector t_vPos = t_aMat_Cam[3];
		vector t_vDir = t_aMat_Cam[2];

		// t_vPos = t_vPos + (t_vDir * 0.05);
		t_vPos = t_vPos + (t_vDir * 0.5);
		t_vPos = t_vPos + (gOS_m_fPos_Place);


		// vector t_vPos_Hit_Dir_Rel_2 = t_vPos_Hit_Dir.InvMultiply3(t_vMat_Bone_2);
		// gOS_m_fPos_Place = "0 0 0";
		vector t_vPlacement = Vector(
			// t_vDir[0] * (gOS_m_fPos_Place[0] * 100)
			t_vDir[0] + (gOS_m_fPos_Place[0] * 1)
			, t_vDir[1] + gOS_m_fPos_Place[1]
			, t_vDir[2] + gOS_m_fPos_Place[2]

		);
		// t_vPos = t_vPos + t_vPlacement;

		// TODO:
		// - radui action to reset positon
		// - radui action to "plug in" or sth



		t_aMat_Cam[3] = t_vPos;
		// Print(t_aMat_Cam_World);

		vector m_PreviewRotation = "0 90 0";
		vector t_aMat_Rotate[4];
		// Math3D.AnglesToMatrix(m_PreviewRotation, t_aMat_Rotate);

		vector yawPitchRoll = Math3D.MatrixToAngles(t_aMat_Cam);
		// Print(yawPitchRoll);
		// yawPitchRoll = yawPitchRoll - yawPitchRoll;
		// yawPitchRoll = yawPitchRoll + m_PreviewRotation;
		// Print(yawPitchRoll);
		// yawPitchRoll[0] = 0;
		yawPitchRoll[1] = yawPitchRoll[1] + 90;
		// yawPitchRoll[2] = 0;
		Math3D.AnglesToMatrix(yawPitchRoll, t_aMat_Cam);

		// t_aMat_Cam = t_aMat_Cam * t_aMat_Rotate;
		// Print(yawPitchRoll);

		// t_vDir = t_vDir * t_aMat_Rotate[2];
		// t_aMat_Cam[2] = t_vDir;
		
		// Math3D.MatrixMultiply4(t_aMat_Rotate, t_aMat_Cam, t_aMat_Cam);
		// Math3D.MatrixMultiply3(t_aMat_Cam, t_aMat_Rotate, t_aMat_Cam);
		// Math3D.MatrixMultiply3(t_aMat_Rotate, t_aMat_Cam, t_aMat_Cam);
		// Math3D.MatrixInvMultiply3(t_aMat_Rotate, t_aMat_Cam, t_aMat_Cam);
		t_aMat_Cam[3] = t_vPos;

		// t_aMat_Cam[2] = t_aMat_Cam[2] * t_vDir;
		// t_vDir = t_aMat_Cam.Multiply3(t_aMat_Rotate);
		// t_vDir = t_aMat_Rotate.Multiply3(t_aMat_Cam);
		// t_vDir = t_aMat_Cam.InvMultiply3(t_aMat_Rotate);
		// t_vDir = t_aMat_Rotate.InvMultiply3(t_aMat_Cam);
		// t_aMat_Cam[2] = t_vDir;

		
		IEntity t_IEnt_Player = SCR_PlayerController.GetLocalControlledEntity();
		if (!t_IEnt_Player) {
			return;
		};
		// vector t_aMat_Player[4];
		// t_IEnt_Player.GetTransform(t_aMat_Player);
		// t_aMat_Cam[1] = t_aMat_Player[1];
		
		// Math3D.MatrixMultiply4(t_aMat_Rotate, t_aMat_Player, t_aMat_Player);
		// t_vDir = t_aMat_Cam.Multiply3(t_aMat_Rotate);
		// t_vDir = t_aMat_Rotate.Multiply3(t_aMat_Cam);
		// t_vDir = t_aMat_Cam.InvMultiply3(t_aMat_Rotate);
		// t_vDir = t_aMat_Rotate.InvMultiply3(t_aMat_Cam);
		// t_aMat_Cam[2] = t_vDir;
		// t_aMat_Player[2] = t_vDir;
		// t_aMat_Player[3] = t_vPos;

		// gOS_m_IEnt_Place_Test.SetTransform(t_aMat_Player);
		gOS_m_IEnt_Place_Test.SetTransform(t_aMat_Cam);
		// gOS_m_IEnt_Place_Test.SetScale(2);
		gOS_m_IEnt_Place_Test.Update();
		// gOS_m_IEnt_Place_Test.SetOrigin(t_vPos);


		// gOS_m_IEnt_Place_Test.SetOrigin(t_IEnt_Player.GetOrigin() + "0 2 0");
		// Print(gOS_m_IEnt_Place_Test);


		GetGame().GetCallqueue().CallLater(gOS_Place_Loop, 1, false);
	};



	IEntity gOS_m_IEnt_Place_Test;

	bool gOS_m_bIsPlacing = false;
	vector gOS_m_fPos_Place = "0 0 0";



	void gOS_RadUI_Toggle (HCA_RadUI_Data f_Option) {
		// gOS_RadUI_m_bIsEnabled = !gOS_RadUI_m_bIsEnabled;
		// gOS_RadUI_Toggle_OnDraw(f_Option);

	};
	bool gOS_RadUI_m_bIsEnabled = false;
	void gOS_RadUI_Toggle_OnDraw (HCA_RadUI_Data f_Option) {
		if (!f_Option.m_wItem) {
			return;
		};
		//! TODO:
		//	- Maybe add a universal function in the option to mess with the text / icon
		TextWidget t_wText = TextWidget.Cast(f_Option.m_wItem.FindAnyWidget("m_Text"));
		ImageWidget t_wImg = ImageWidget.Cast(f_Option.m_wItem.FindAnyWidget("m_Icon"));
		if (!t_wText || !t_wImg) {
			return;
		};
		// t_wText.SetText(m_sName);
		string t_sText;
		if (gOS_RadUI_m_bIsEnabled) {
			t_sText = "Disable xxx";
		} else {
			t_sText = "Enable xxx";
		};
		t_wText.SetText(t_sText);
	};
	void gOS_RadUI_Cat (HCA_RadUI_Data f_Option) {

	};


	void gOS_RadUI_Markers_Unit_Toggle (HCA_RadUI_Data f_Option) {
		gOS_m_bMarkers_ShowPlayers = !gOS_m_bMarkers_ShowPlayers;
		gOS_RadUI_Markers_Unit_Toggle_OnDraw(f_Option);

	};
	bool gOS_m_bMarkers_ShowPlayers = true;
	void gOS_RadUI_Markers_Unit_Toggle_OnDraw (HCA_RadUI_Data f_Option) {
		if (!f_Option.m_wItem) {
			return;
		};
		//! TODO:
		//	- Maybe add a universal function in the option to mess with the text / icon
		TextWidget t_wText = TextWidget.Cast(f_Option.m_wItem.FindAnyWidget("m_Text"));
		ImageWidget t_wImg = ImageWidget.Cast(f_Option.m_wItem.FindAnyWidget("m_Icon"));
		if (!t_wText || !t_wImg) {
			return;
		};
		// t_wText.SetText(m_sName);
		string t_sText;
		if (gOS_m_bMarkers_ShowPlayers) {
			t_sText = "Disable Player Markers";
		} else {
			t_sText = "Enable Player Markers";
		};
		t_wText.SetText(t_sText);
	};



	void gOS_RadUI_Markers_Unit_Toggle_Fade (HCA_RadUI_Data f_Option) {
		gOS_m_bMarkers_ShowPlayers_IsFade = !gOS_m_bMarkers_ShowPlayers_IsFade;
		gOS_RadUI_Markers_Unit_Toggle_Fade_OnDraw(f_Option);

	};
	bool gOS_m_bMarkers_ShowPlayers_IsFade = true;
	void gOS_RadUI_Markers_Unit_Toggle_Fade_OnDraw (HCA_RadUI_Data f_Option) {
		if (!f_Option.m_wItem) {
			return;
		};
		//! TODO:
		//	- Maybe add a universal function in the option to mess with the text / icon
		TextWidget t_wText = TextWidget.Cast(f_Option.m_wItem.FindAnyWidget("m_Text"));
		ImageWidget t_wImg = ImageWidget.Cast(f_Option.m_wItem.FindAnyWidget("m_Icon"));
		if (!t_wText || !t_wImg) {
			return;
		};
		// t_wText.SetText(m_sName);
		string t_sText;
		if (gOS_m_bMarkers_ShowPlayers_IsFade) {
			t_sText = "Disable Fading Player Markers";
		} else {
			t_sText = "Enable Fading Player Markers";
		};
		t_wText.SetText(t_sText);
	};



	void gOS_RadUI_Markers_Group_Toggle (HCA_RadUI_Data f_Option) {
		gOS_m_bMarkers_ShowGroup = !gOS_m_bMarkers_ShowGroup;
		gOS_RadUI_Markers_Group_Toggle_OnDraw(f_Option);

	};
	bool gOS_m_bMarkers_ShowGroup = true;
	void gOS_RadUI_Markers_Group_Toggle_OnDraw (HCA_RadUI_Data f_Option) {
		if (!f_Option.m_wItem) {
			return;
		};
		//! TODO:
		//	- Maybe add a universal function in the option to mess with the text / icon
		TextWidget t_wText = TextWidget.Cast(f_Option.m_wItem.FindAnyWidget("m_Text"));
		ImageWidget t_wImg = ImageWidget.Cast(f_Option.m_wItem.FindAnyWidget("m_Icon"));
		if (!t_wText || !t_wImg) {
			return;
		};
		// t_wText.SetText(m_sName);
		string t_sText;
		if (gOS_m_bMarkers_ShowGroup) {
			t_sText = "Disable Group Markers";
		} else {
			t_sText = "Enable Group Markers";
		};
		t_wText.SetText(t_sText);
	};


	void gOS_RadUI_Markers_Group_Toggle_Fade (HCA_RadUI_Data f_Option) {
		gOS_m_bMarkers_ShowGroup_IsFade = !gOS_m_bMarkers_ShowGroup_IsFade;
		gOS_RadUI_Markers_Group_Toggle_Fade_OnDraw(f_Option);

	};
	bool gOS_m_bMarkers_ShowGroup_IsFade = true;
	void gOS_RadUI_Markers_Group_Toggle_Fade_OnDraw (HCA_RadUI_Data f_Option) {
		if (!f_Option.m_wItem) {
			return;
		};
		//! TODO:
		//	- Maybe add a universal function in the option to mess with the text / icon
		TextWidget t_wText = TextWidget.Cast(f_Option.m_wItem.FindAnyWidget("m_Text"));
		ImageWidget t_wImg = ImageWidget.Cast(f_Option.m_wItem.FindAnyWidget("m_Icon"));
		if (!t_wText || !t_wImg) {
			return;
		};
		// t_wText.SetText(m_sName);
		string t_sText;
		if (gOS_m_bMarkers_ShowGroup_IsFade) {
			t_sText = "Disable Fading Group Markers";
		} else {
			t_sText = "Enable Fading Group Markers";
		};
		t_wText.SetText(t_sText);
	};

	void gOS_RadUI_Exp_Action_Enable (HCA_RadUI_Data f_Option) {
		s_bExp_Action_Enable = !s_bExp_Action_Enable;
		gOS_RadUI_Exp_Action_Enable_OnDraw(f_Option);

	};
	// gOS_SCR_MiniMap_HUD.s_bExp_Action_Enable
	static bool s_bExp_Action_Enable = false;
	void gOS_RadUI_Exp_Action_Enable_OnDraw (HCA_RadUI_Data f_Option) {
		if (!f_Option.m_wItem) {
			return;
		};
		//! TODO:
		//	- Maybe add a universal function in the option to mess with the text / icon
		TextWidget t_wText = TextWidget.Cast(f_Option.m_wItem.FindAnyWidget("m_Text"));
		ImageWidget t_wImg = ImageWidget.Cast(f_Option.m_wItem.FindAnyWidget("m_Icon"));
		if (!t_wText || !t_wImg) {
			return;
		};
		// t_wText.SetText(m_sName);
		string t_sText;
		if (s_bExp_Action_Enable) {
			t_sText = "Disable interaction action";
		} else {
			t_sText = "Enable interaction action";
		};
		t_wText.SetText(t_sText);
	};















































































	void gOS_KeyDown_Options (float value, EActionTrigger trigger) {
		// if (!gOS_m_bIsSettingsShortcut) {
		// 	return;
		// };
		
		HCA_RadUI_HUD t_RadUI_HUD = HCA_RadUI_HUD.HCA_GetCurrent();
		if (!t_RadUI_HUD) {
			return;
		};

		// if the RadUI is already open, close
		if (t_RadUI_HUD.HCA_RadUI_m_bIsOpen) {
			gOS_RadUI_Exit();
			return;
		};



		HCA_RadUI_Man t_RadUI_Man = HCA_RadUI_Man.Get();
		if (!t_RadUI_Man) {
			return;
		};
		HCA_RadUI_Data t_Data = t_RadUI_Man.Get("vPad_Category");
		if (!t_Data) {
			return;
		};

		// open with the vpad category
		t_RadUI_HUD.HCA_RadUI_Open(t_Data);
	};
	void gOS_KeyDown_Settings (float value, EActionTrigger trigger) {
		if (!gOS_m_bIsSettingsShortcut) {
			return;
		};
		gOS_Settings_JumpTo();

		//---! TEST for open the options immediately
		//! TODO:
		// - when in settings, disable keybinds for this 

		// remove other tabs?
		// int t_iCount_Tabs = t_Comp_TabView.GetTabCount();
		// t_iCount_Tabs = t_iCount_Tabs - 1;
		// for (int t_iIdx = 0; t_iIdx < t_iCount_Tabs; t_iIdx++) {
		// 	t_Comp_TabView.RemoveTab(0);
		// };


		// t_Comp_TabView.ShowTabByIdentifier("SettingsGameplay");
	};
	void gOS_Settings_JumpTo () {

		HCA_RadUI_HUD t_RadUI_HUD = HCA_RadUI_HUD.HCA_GetCurrent();
		if (t_RadUI_HUD) {
			t_RadUI_HUD.HCA_RadUI_Close();
		};


		GetGame().OpenPauseMenu(false, true);

		SCR_SettingsSuperMenu t_MenuUI = SCR_SettingsSuperMenu.Cast(GetGame().GetMenuManager().FindMenuByPreset(ChimeraMenuPreset.SettingsSuperMenu));
        if (!t_MenuUI) {
            GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.SettingsSuperMenu);
            t_MenuUI = SCR_SettingsSuperMenu.Cast(GetGame().GetMenuManager().FindMenuByPreset(ChimeraMenuPreset.SettingsSuperMenu));
        };
        if (!t_MenuUI || !t_MenuUI.GetRootWidget()) {
            return;
        };
        // t_MenuUI.GetRootWidget().SetEnabled(false);
        // t_MenuUI.GetRootWidget().SetVisible(false);
		
		Widget t_wTabView = t_MenuUI.GetRootWidget().FindAnyWidget("TabView");
		// //--- EXP
        // #ifdef VPAD_EXP
		// Widget t_wTabView = t_MenuUI.GetRootWidget().FindAnyWidget("TabView");
		// #else
		// //--- STB
		// Widget t_wTabView = t_MenuUI.GetRootWidget().FindAnyWidget("TabViewRoot0");
		// #endif

		// Print(t_wTabView);
		if (!t_wTabView) {
			return;
		};
		SCR_TabViewComponent t_Comp_TabView = SCR_TabViewComponent.Cast(t_wTabView.FindHandler(SCR_TabViewComponent));
		// Print(t_Comp_TabView);
		if (!t_Comp_TabView) {
			return;
		};

		t_Comp_TabView.ShowTabByIdentifier("Settings_gOS");

		// focus the first option to help with UX on controller
		Widget t_wToFocus = t_wTabView.FindAnyWidget("gOS_GameplaySettings");
		if (t_wToFocus) {
			t_wToFocus = t_wToFocus.FindAnyWidget("wSetting_IsMinimapEnabled");
			// Print(t_wToFocus);
			if (t_wToFocus) {
				GetGame().GetWorkspace().SetFocusedWidget(t_wToFocus);
			};
		};


	};



	void gOS_KeyDown_OpenMenu (float value, EActionTrigger trigger) {
		if (value == 0) {
			return;
		};
	};

	// gOS_SCR_MiniMap_HUD.gOS_MiniMap_SetEnabled(true);
	static void gOS_MiniMap_SSetEnabled (bool f_bIsEnabled = true) {
		if (!gOS_GetCurrent()) {
			return;
		};

		// Print("gOS_SCR_MiniMap_HUD::gOS_MiniMap_SSetEnabled::" + f_bIsEnabled);
		
		GetGame().GetCallqueue().Remove(gOS_GetCurrent().gOS_MiniMap_SetEnabled);
		gOS_GetCurrent().gOS_MiniMap_SetEnabled(f_bIsEnabled);
		if (!f_bIsEnabled) {
			GetGame().GetCallqueue().Remove(gOS_GetCurrent().gOS_MiniMap_Update);
		};
	};

	static bool gOS_MiniMap_SGetEnabled () {
		if (!gOS_GetCurrent()) {
			return false;
		};
		return gOS_GetCurrent().gOS_MiniMap_GetEnabled();
	};

	bool gOS_MiniMap_GetEnabled () {
		return gOS_m_bIsEnabled;
	};
		
	private bool gOS_m_bIsEnabled = false;

	void gOS_MiniMap_TimeReset () {

		
		// true add updater
		gOS_m_fTimeLastMiniMap = GetGame().GetWorld().GetWorldTime() * 0.001;
	};
	void gOS_MiniMap_AddUpdater (float f_fTimeMS = 10) {

	};

	IEntity m_IEnt_Map_Box;
	gOS_SCR_MiniMapComponent m_Comp_MiniMap;
	bool m_bIsPaperMap = false;

    // call later
    void gOS_MiniMap_SetEnabled_Display_Enable () {
        if (!m_IEnt_Map_Box) {
            return;
        };

        // --- to replace materials:
        VObject mesh = m_IEnt_Map_Box.GetVObject();
        // Print("gOS_MiniMap_SetEnabled_Display_Enable::Mesh:" + mesh);

		string t_sMat = "{25BBE0B8CE47F38F}Assets/Data/Material_RT.emat";
		if (m_bIsPaperMap) {
			t_sMat = "{1A053271B1A6AA74}Assets/Data/Material_RT_Paper.emat";

			// m_IEnt_Map_Box.ClearFlags(EntityFlags.VISIBLE, true);
			// m_IEnt_Map_Box.SetFlags(EntityFlags.VISIBLE, true);
			// Print("SetFlags:" + m_IEnt_Map_Box);
			// m_IEnt_Map_Box.ClearFlags(EntityFlags.VISIBLE, true);


			SCR_GenericBoxEntity t_Box = SCR_GenericBoxEntity.Cast(m_IEnt_Map_Box);
			t_Box.Generate("0.22 0 0.22", "{1A053271B1A6AA74}Assets/Data/Material_RT_Paper.emat");

			return;
		};
        if (mesh) {
            string remap;
            string materials[256];
            int numMats = mesh.GetMaterials(materials);
            for (int i = 0; i < numMats; i++) {
                // Print(materials[i]);
                if (materials[i].Contains("Material")) {
        			// Print("gOS_MiniMap_SetEnabled_Display_Enable::remap");
					// Print("" + materials[i]);

                    // remap += string.Format("$remap '%1' '%2';", materials[i], "{25BBE0B8CE47F38F}Assets/Data/Material_RT.emat");
                    remap += string.Format("$remap '%1' '%2';", materials[i], t_sMat);
                };
            };
            m_IEnt_Map_Box.SetObject(mesh, remap);
        };
    };
    void gOS_MiniMap_SetEnabled_Display_Disable () {
		gOS_RadUI_Exit();
        if (!m_IEnt_Map_Box) {
            return;
        };
        VObject mesh = m_IEnt_Map_Box.GetVObject();
		string t_sMat = "{EFCE28F06F1226A1}Assets/Data/Material.emat";
		if (m_bIsPaperMap) {
			t_sMat = "{7F954376D2A494C4}Assets/Data/Material_Paper_Invis.emat";
			// m_IEnt_Map_Box.ClearFlags(EntityFlags.VISIBLE, true);
			// Print("ClearFlags:" + m_IEnt_Map_Box);
			// Print("ClearFlags");
			SCR_GenericBoxEntity t_Box = SCR_GenericBoxEntity.Cast(m_IEnt_Map_Box);
			if (t_Box) {
				// Print ("Box fouind:" + t_Box);
				// Print ("Box fouind:" + mesh);
			};
			t_Box.Generate("0.22 0 0.22", "{7F954376D2A494C4}Assets/Data/Material_Paper_Invis.emat");
			return;
		};
        if (mesh) {
            string remap;
            string materials[256];
            int numMats = mesh.GetMaterials(materials);
            for (int i = 0; i < numMats; i++) {
                if (materials[i].Contains("Material")) {
        			// Print("gOS_MiniMap_SetEnabled_Display_Disable::remap");
					// Print("" + materials[i]);
                    // remap += string.Format("$remap '%1' '%2';", materials[i], "{EFCE28F06F1226A1}Assets/Data/Material.emat");
                    remap += string.Format("$remap '%1' '%2';", materials[i], t_sMat);
                };
            };
            m_IEnt_Map_Box.SetObject(mesh, remap);
        };
    };


	void gOS_OnEnabled () {
		// Print("gOS_OnEnabled");

	};
	void gOS_OnDisabled () {
		// Print("gOS_OnDisabled");

	};


	void gOS_MiniMap_SetEnabled_Placed_Map_Swap () {


	};

	IEntity m_IEnt_Custom_Map;
	bool m_bCustom_Map_Enabled = false;
	void gOS_MiniMap_SetEnabled_Custom_Map () {

		if (!m_IEnt_Custom_Map) {
			Print("gOS_MiniMap_SetEnabled_Custom_Map::Fail no placed static IEnt");
			return;
		};

		gOS_MiniMap_SetEnabled_Display_Disable();

		

		IEntity t_IEnt_Map = m_IEnt_Custom_Map;

		// also assign the component
		gOS_SCR_MiniMapComponent t_Comp_MiniMap = gOS_SCR_MiniMapComponent.Cast(t_IEnt_Map.FindComponent(gOS_SCR_MiniMapComponent));
		if (t_Comp_MiniMap) {
			m_Comp_MiniMap = t_Comp_MiniMap;
			m_bIsPaperMap = m_Comp_MiniMap.m_bIsPaperMap;
		};


		RTTextureWidget t_wRTT = RTTextureWidget.Cast(m_wRoot.FindAnyWidget("RTTexture0"));
		IEntity t_IEnt_Box = t_IEnt_Map.GetChildren();
		while (t_IEnt_Box)  {
			if (t_IEnt_Box) {
				if (!m_bIsPaperMap) {
					break;
				};
				gOS_SCR_MiniMapComponent t_Comp_MiniMap_Box = gOS_SCR_MiniMapComponent.Cast(t_IEnt_Box.FindComponent(gOS_SCR_MiniMapComponent));

				if (t_Comp_MiniMap_Box && t_Comp_MiniMap_Box.m_bIsPaperMapDisplay) {
					break;
				};
			};
			t_IEnt_Box = t_IEnt_Box.GetSibling();
		};
		if (!t_IEnt_Box) {
			Print("gOS_MiniMap_SetEnabled_Custom_Map::Fail IEnt child found");
			return;
		};

		m_IEnt_Map_Box = t_IEnt_Box;


		// gOS_m_bIsPlaced_Map_Enabled = true;


		gOS_MiniMap_SetEnabled_Display_Enable();



		
		t_wRTT.SetGUIWidget(t_IEnt_Box, 1);
	};

	void gOS_MiniMap_SetEnabled_Placed_Map () {

		if (!gOS_SCR_MiniMapComponent.gOS_s_IEnt_Placed) {
			Print("gOS_RadUI_Place_Enable::Fail no placed static IEnt");
			gOS_m_bIsPlaced_Map_Enabled = false;
			gOS_MiniMap_SetEnabled(true);
			return;
		};

		gOS_MiniMap_SetEnabled_Display_Disable();

		

		IEntity t_IEnt_Map = gOS_SCR_MiniMapComponent.gOS_s_IEnt_Placed;

		// also assign the component
		gOS_SCR_MiniMapComponent t_Comp_MiniMap = gOS_SCR_MiniMapComponent.Cast(t_IEnt_Map.FindComponent(gOS_SCR_MiniMapComponent));
		if (t_Comp_MiniMap) {
			m_Comp_MiniMap = t_Comp_MiniMap;
			m_bIsPaperMap = m_Comp_MiniMap.m_bIsPaperMap;
		};


		RTTextureWidget t_wRTT = RTTextureWidget.Cast(m_wRoot.FindAnyWidget("RTTexture0"));
		IEntity t_IEnt_Box = t_IEnt_Map.GetChildren();
		while (t_IEnt_Box)  {
			if (t_IEnt_Box) {
				if (!m_bIsPaperMap) {
					break;
				};
				gOS_SCR_MiniMapComponent t_Comp_MiniMap_Box = gOS_SCR_MiniMapComponent.Cast(t_IEnt_Box.FindComponent(gOS_SCR_MiniMapComponent));

				if (t_Comp_MiniMap_Box && t_Comp_MiniMap_Box.m_bIsPaperMapDisplay) {
					break;
				};
			};
			t_IEnt_Box = t_IEnt_Box.GetSibling();
		};
		if (!t_IEnt_Box) {
			Print("gOS_RadUI_Place_Enable::Fail IEnt child found");
			return;
		};

		m_IEnt_Map_Box = t_IEnt_Box;


		gOS_m_bIsPlaced_Map_Enabled = true;


		gOS_MiniMap_SetEnabled_Display_Enable();



		
		t_wRTT.SetGUIWidget(t_IEnt_Box, 1);
	};


	// find the object which has a display for the mininmap
	//	- could be a child of the entity or sth
	void gOS_MiniMap_Obj_Find (IEntity f_IEnt) {

		gOS_SCR_MiniMapComponent t_Comp_MiniMap = gOS_SCR_MiniMapComponent.Cast(f_IEnt.FindComponent(gOS_SCR_MiniMapComponent));
		if (t_Comp_MiniMap) {
			// required for placing the minimap, the function is inside minimapComponent
			m_Comp_MiniMap = t_Comp_MiniMap;
			m_bIsPaperMap = m_Comp_MiniMap.m_bIsPaperMap;
		};

		// because of how minimap is setup as a child entity of the normal map:
		IEntity t_IEnt_Box = f_IEnt.GetChildren();
		while (t_IEnt_Box)  {
			if (t_IEnt_Box) {
				if (!m_bIsPaperMap) {
					break;
				};
				gOS_SCR_MiniMapComponent t_Comp_MiniMap_Box = gOS_SCR_MiniMapComponent.Cast(t_IEnt_Box.FindComponent(gOS_SCR_MiniMapComponent));

				if (t_Comp_MiniMap_Box && t_Comp_MiniMap_Box.m_bIsPaperMapDisplay) {
					break;
				};
			};
			t_IEnt_Box = t_IEnt_Box.GetSibling();
		};
		if (!t_IEnt_Box) {
			// GetGame().GetCallqueue().CallLater(gOS_MiniMap_Obj_Find, 1, false, true);	// call later because loading or something slow animation I dunno gadget system or sth
			return;
		};

		m_IEnt_Map_Box = t_IEnt_Box;
	};
	void gOS_MiniMap_Obj_Set (IEntity f_IEnt) {
		GetGame().GetCallqueue().Remove(gOS_MiniMap_GUI_Enable);
		// clear

		if (m_IEnt_Map_Box) {
			gOS_MiniMap_SetEnabled_Display_Disable();
			m_IEnt_Map_Box = NULL;
		};
		if (!f_IEnt) {
			return;
		};
		gOS_MiniMap_Obj_Find(f_IEnt);

		RTTextureWidget t_wRTT = RTTextureWidget.Cast(m_wRoot.FindAnyWidget("RTTexture0"));
		if (!t_wRTT) {
			GetGame().GetCallqueue().CallLater(gOS_MiniMap_Obj_Set, 1, false, f_IEnt);
			return;
		};

		m_IEnt_Map_Box = f_IEnt;
		gOS_MiniMap_SetEnabled_Display_Enable();
		// Print("gOS_SCR_MiniMap_HUD::box:" + m_IEnt_Map_Box);
		t_wRTT.SetGUIWidget(m_IEnt_Map_Box, 1);


		
		GetGame().GetCallqueue().CallLater(gOS_MiniMap_GUI_Enable, 1, false, true);
	};
	void gOS_MiniMap_Obj_Enable () {

	};
	void gOS_MiniMap_GUI_Enable () {
	
		Widget t_wMapFrame = m_wRoot.FindAnyWidget("VerticalLayout0");
		// Print(t_wMapFrame);
		if (!t_wMapFrame) {
			// Print("gOS_SCR_MiniMap_HUD::gOS_MiniMap_SetEnabled::fail no map frame");
			return;
		};
		if (!SCR_MapEntity.GetMapInstance()) {
			// Print("gOS_SCR_MiniMap_HUD::gOS_MiniMap_SetEnabled::fail no map instance");
			return;
		};
		SCR_MapEntity t_ME = SCR_MapEntity.GetMapInstance();
		MapConfiguration t_ME_Cfg;
		if (t_ME) {
			t_ME_Cfg = t_ME.GetMapConfig();
		};
		if ((t_ME && gOS_m_MapCfg != t_ME_Cfg) || !t_ME || gOS_m_MapCfg == null) {
            // Print("gOS_SCR_MiniMap_HUD::gOS_MiniMap_SetEnabled::Wrong Map:" + (gOS_m_MapCfg != t_ME_Cfg));
			// if (!t_Comp_Gadget.m_bFocused && t_ME) {
			if (t_ME) {
				MenuManager menuManager = GetGame().GetMenuManager();
				SCR_MapMenuUI t_MapMenu = SCR_MapMenuUI.Cast(menuManager.FindMenuByPreset(ChimeraMenuPreset.MapMenu));
				if (t_MapMenu) {
					// Print("gOS_SCR_MiniMap_HUD::gOS_MiniMap_SetEnabled::Map menu found");
					GetGame().GetCallqueue().CallLater(gOS_MiniMap_GUI_Enable, 1, false, true);
					return;
				};
				gOS_m_MapCfg = t_ME.SetupMapConfig(EMapEntityMode.MINIMAP, "{BBEA171227C9C09B}Configs/Map/GOS_MapSmall3.conf", t_wMapFrame);
				// Print("gOS_SCR_MiniMap_HUD::gOS_MiniMap_SetEnabled::Attempt at OPEN MAP:" + gOS_m_MapCfg);
				t_ME.OpenMap(gOS_m_MapCfg);
				// Print("gOS_SCR_MiniMap_HUD::gOS_MiniMap_SetEnabled::DONE at OPEN MAP:" + gOS_m_MapCfg);			
				Widget t_wMapWidget = m_wRoot.FindAnyWidget("MapWidget");
				if (t_wMapWidget) {
					t_ME.SetMapWidget(t_wMapWidget);
				};
				// gOS_MiniMap_SetEnabled(true);
				GetGame().GetCallqueue().CallLater(gOS_MiniMap_GUI_Enable, 1, false, true);
				// the MapConfig disables cam render, re-enable it
				PlayerController t_PC = GetGame().GetPlayerController();
				if (t_PC) {
					t_PC.SetCharacterCameraRenderActive(true);
				};
				return;
			};
			GetGame().GetCallqueue().CallLater(gOS_MiniMap_GUI_Enable, 1, false, true);
			return;
		};

		t_wMapFrame.SetVisible(true);
		t_wMapFrame.SetEnabled(true);


		// the MapConfig disables cam render, re-enable it
		PlayerController t_PC = GetGame().GetPlayerController();
		if (t_PC) {
			t_PC.SetCharacterCameraRenderActive(true);
		};

		
		GetGame().GetCallqueue().Remove(gOS_MiniMap_Update);
		GetGame().GetCallqueue().CallLater(gOS_MiniMap_Update, 1, false);
		gOS_m_bIsEnabled = true;

		gOS_Brigthness_Set(gOS_m_iBrightness);

	};

	void gOS_MiniMap_SetEnabled (bool f_bIsEnabled = true) {
		// Print("gOS_MiniMap_SetEnabled:" + f_bIsEnabled);

		// already good
		if (f_bIsEnabled && gOS_m_bIsEnabled) {
			// Print("gOS_SCR_MiniMap_HUD::gOS_MiniMap_SetEnabled::Want enable, ENABLED");
			//! TEST
			// Only do this for fake override
			// gOS_m_bIsEnabled = false;	//! TEST2 (old)
			// GetGame().GetCallqueue().Remove(gOS_MiniMap_Update);
			return;
		};
		if (!f_bIsEnabled && !gOS_m_bIsEnabled) {
			// Print("gOS_SCR_MiniMap_HUD::gOS_MiniMap_SetEnabled::Want disable, DISABLED");
			return;
		};



		//--- Disable the map
		if (!f_bIsEnabled) {
			gOS_OnDisabled();
			
			if (m_IEnt_Map_Box) {
				// m_IEnt_Map_Box.ClearFlags(EntityFlags.VISIBLE, false);

                
                // Print("gOS_MiniMap_SetEnabled::bye minimap");


			    GetGame().GetCallqueue().Remove(gOS_MiniMap_SetEnabled_Display_Enable);
                gOS_MiniMap_SetEnabled_Display_Disable();
			};
			
			// if (m_Comp_MiniMap) {
				// m_Comp_MiniMap.gOS_C2S_Place_Remove();
			// };
			m_IEnt_Map_Box = null;
			gOS_m_bIsEnabled = false;
			gOS_m_MapCfg = null;
			GetGame().GetCallqueue().Remove(gOS_MiniMap_Update);

			
			gOS_m_bIsPlacing = false;
			if (gOS_m_IEnt_Place_Test) {
				SCR_EntityHelper.DeleteEntityAndChildren(gOS_m_IEnt_Place_Test);
			};
		};
		// Widget t_wMapFrame = m_wRoot.FindAnyWidget("MapFrame");
		Widget t_wMapFrame = m_wRoot.FindAnyWidget("VerticalLayout0");
		// Print(t_wMapFrame);
		if (!t_wMapFrame) {
			// Print("gOS_SCR_MiniMap_HUD::gOS_MiniMap_SetEnabled::fail no map frame");
			return;
		};
		if (!SCR_MapEntity.GetMapInstance()) {
			// Print("gOS_SCR_MiniMap_HUD::gOS_MiniMap_SetEnabled::fail no map instance");
			return;
		};
		SCR_MapEntity t_ME = SCR_MapEntity.GetMapInstance();
		if (!f_bIsEnabled) {
			if (t_ME && t_ME.IsOpen()) {
				t_ME.CloseMap();
			    // Print("gOS_SCR_MiniMap_HUD::gOS_MiniMap_SetEnabled::close map 1");
			};
			// t_wMapFrame.SetVisible(false);
			// t_wMapFrame.SetEnabled(false);
			Widget t_wMapFrame2 = m_wRoot.FindAnyWidget("MapWidget");
			if (t_wMapFrame2) {
				// t_wMapFrame2.SetOpacity(0);
			};
			// Print("OPacity 0");
			gOS_m_bIsEnabled = false;
			GetGame().GetCallqueue().Remove(gOS_MiniMap_SetEnabled);
			

			return;
		} else {
			// Print(t_ME);
		};
		//------ ENABLE MAP:
        //--- TEST
        IEntity t_IEnt_Player = SCR_PlayerController.GetLocalControlledEntity();
        if (!t_IEnt_Player) {
            return;
        };
        SCR_GadgetManagerComponent t_Comp_GMan = SCR_GadgetManagerComponent.GetGadgetManager(t_IEnt_Player);
        if (!t_Comp_GMan) {
            return;
        };
        IEntity t_IEnt_Map = t_Comp_GMan.GetGadgetByType(EGadgetType.MAP);
        if (!t_IEnt_Map) {
            // Print("gOS_SCR_MiniMap_HUD::gOS_MiniMap_SetEnabled::Missing Map!");
            return;
        };
		SCR_MapGadgetComponent t_Comp_Gadget = SCR_MapGadgetComponent.Cast(t_IEnt_Map.FindComponent(SCR_MapGadgetComponent));
		if (!t_Comp_Gadget) {
            // Print("gOS_SCR_MiniMap_HUD::gOS_MiniMap_SetEnabled::Missing Map component (BUG?)!");
			return;
		};
		MapConfiguration t_ME_Cfg;
		if (t_ME) {
			t_ME_Cfg = t_ME.GetMapConfig();
		};
		if (t_Comp_Gadget.m_bFocused || (t_ME && gOS_m_MapCfg != t_ME_Cfg)  || !t_ME || gOS_m_MapCfg == null) {
            // Print("gOS_SCR_MiniMap_HUD::gOS_MiniMap_SetEnabled::Wrong Map:" + (gOS_m_MapCfg != t_ME_Cfg));
			if (!t_Comp_Gadget.m_bFocused && t_ME) {
				MenuManager menuManager = GetGame().GetMenuManager();
				SCR_MapMenuUI t_MapMenu = SCR_MapMenuUI.Cast(menuManager.FindMenuByPreset(ChimeraMenuPreset.MapMenu));
				if (t_MapMenu) {
					// Print("gOS_SCR_MiniMap_HUD::gOS_MiniMap_SetEnabled::Map menu found");
					GetGame().GetCallqueue().CallLater(gOS_MiniMap_SetEnabled, 1, false, true);
					return;
				};
				// gOS_m_MapCfg = t_ME.SetupMapConfig(EMapEntityMode.MINIMAP, "{4FE2508A2A14A8EF}Configs/Map/GOS_MapSmall2.conf", t_wMapFrame);
				// gOS_m_MapCfg = t_ME.SetupMapConfig(EMapEntityMode.MINIMAP, "{81A59D954FB86280}Configs/Map/GOS_MapSmall.conf", t_wMapFrame);
				gOS_m_MapCfg = t_ME.SetupMapConfig(EMapEntityMode.MINIMAP, "{BBEA171227C9C09B}Configs/Map/GOS_MapSmall3.conf", t_wMapFrame);
				// Print("gOS_SCR_MiniMap_HUD::gOS_MiniMap_SetEnabled::Attempt at OPEN MAP:" + gOS_m_MapCfg);
				t_ME.OpenMap(gOS_m_MapCfg);
				// Print("gOS_SCR_MiniMap_HUD::gOS_MiniMap_SetEnabled::DONE at OPEN MAP:" + gOS_m_MapCfg);			
				Widget t_wMapWidget = m_wRoot.FindAnyWidget("MapWidget");
				if (t_wMapWidget) {
					t_ME.SetMapWidget(t_wMapWidget);
				};
				// gOS_MiniMap_SetEnabled(true);
				GetGame().GetCallqueue().CallLater(gOS_MiniMap_SetEnabled, 1, false, true);
				// the MapConfig disables cam render, re-enable it
				PlayerController t_PC = GetGame().GetPlayerController();
				if (t_PC) {
					t_PC.SetCharacterCameraRenderActive(true);
				};
				return;
			};
			GetGame().GetCallqueue().CallLater(gOS_MiniMap_SetEnabled, 1, false, true);
			return;
		};

		//! TODO:
		//	- test paper map??????
		if (!m_IEnt_Map_Box) {
			if (gOS_m_bIsPlaced_Map_Enabled) {

				Print("gOS_MiniMap_SetEnabled::placed");
				gOS_MiniMap_SetEnabled_Placed_Map();
				return;
			};

			// also assign the component
			gOS_SCR_MiniMapComponent t_Comp_MiniMap = gOS_SCR_MiniMapComponent.Cast(t_IEnt_Map.FindComponent(gOS_SCR_MiniMapComponent));
			if (t_Comp_MiniMap) {
				m_Comp_MiniMap = t_Comp_MiniMap;
				m_bIsPaperMap = m_Comp_MiniMap.m_bIsPaperMap;
			};


			RTTextureWidget t_wRTT = RTTextureWidget.Cast(m_wRoot.FindAnyWidget("RTTexture0"));
			if (!t_wRTT) {
				GetGame().GetCallqueue().CallLater(gOS_MiniMap_SetEnabled, 1, false, true);
				return;
			};


			IEntity t_IEnt_Box = t_IEnt_Map.GetChildren();
			while (t_IEnt_Box)  {
				if (t_IEnt_Box) {
					if (!m_bIsPaperMap) {
						break;
					};
					gOS_SCR_MiniMapComponent t_Comp_MiniMap_Box = gOS_SCR_MiniMapComponent.Cast(t_IEnt_Box.FindComponent(gOS_SCR_MiniMapComponent));

					if (t_Comp_MiniMap_Box && t_Comp_MiniMap_Box.m_bIsPaperMapDisplay) {
						break;
					};
				};
				t_IEnt_Box = t_IEnt_Box.GetSibling();
			};
			if (!t_IEnt_Box) {
				GetGame().GetCallqueue().CallLater(gOS_MiniMap_SetEnabled, 1, false, true);	// call later because loading or something slow animation I dunno gadget system or sth
				return;
			};

			m_IEnt_Map_Box = t_IEnt_Box;
			gOS_MiniMap_SetEnabled_Display_Enable();
			// Print("gOS_SCR_MiniMap_HUD::box:" + t_IEnt_Box);
			t_wRTT.SetGUIWidget(t_IEnt_Box, 1);
			m_IEnt_Map_Box = t_IEnt_Box;

			// gOS_MiniMap_SetEnabled(true);
			GetGame().GetCallqueue().CallLater(gOS_MiniMap_SetEnabled, 1, false, true);
			return;
		};
		gOS_OnEnabled();

		// should be4 visible:
		t_wMapFrame.SetVisible(true);
		t_wMapFrame.SetEnabled(true);
		// the MapConfig disables cam render, re-enable it
		PlayerController t_PC = GetGame().GetPlayerController();
		if (t_PC) {
			t_PC.SetCharacterCameraRenderActive(true);
		};
		//https://youtu.be/K8L-WfBAxpg

		GetGame().GetCallqueue().Remove(gOS_MiniMap_Update);
		GetGame().GetCallqueue().CallLater(gOS_MiniMap_Update, 1, false);
		gOS_m_bIsEnabled = true;

		// set random position near the player as the center
		// trick player to not knowing where they are :D???

		// float t_fRandomX = Math.RandomFloat(-200, 200);
		// float t_fRandomY = Math.RandomFloat(-200, 200);
		float t_fRandomX = 0;
		float t_fRandomY = 0;

		vector t_fPosPlayer = SCR_PlayerController.GetLocalControlledEntity().GetOrigin();

		// was previously used to randomize starting positon without GPS
		gOS_m_vPosFake[0] = t_fPosPlayer[0] + t_fRandomX;
		gOS_m_vPosFake[2] = t_fPosPlayer[2] + t_fRandomY;
		// Print(gOS_m_vPosFake);

		// reapply brightness (not needed in EXP, but in STB for some reason required)
		gOS_Brigthness_Set(gOS_m_iBrightness);
	};

	// static vector gOS_s_vPosLast;

	private vector gOS_m_vPosLast;
	private vector gOS_m_vPosFake;
	private vector gOS_m_vPos_Center;

	ref MapConfiguration gOS_m_MapCfg;

	private float gOS_m_fTimeLastMiniMap = 0;

	private string gOS_m_sLocator_NameLast = "";

	private void gOS_Input_Enable () {
		gOS_m_bIsInputEnabled = true;
		GetGame().GetCallqueue().Remove(gOS_Input_Enable);
		gOS_m_bIsInputRequested = false;
	};
	bool gOS_m_bIsInputEnabled = false;
	bool gOS_m_bIsInputRequested = false;

	void gOS_MiniMap_Update () {
		// Print("gOS::gOS_MiniMap_Update");

		//--- complete failure condition:
		if (!SCR_PlayerController.GetLocalControlledEntity()) {
			// Print("gOS_SCR_MiniMap_HUD::gOS_MiniMap_Update::fail no local controller");
			gOS_MiniMap_SetEnabled(false);
			return;
		};
		if (!SCR_MapEntity.GetMapInstance()) {
			Print("gOS_SCR_MiniMap_HUD::gOS_MiniMap_Update::fail no map instance");
			gOS_MiniMap_SetEnabled(false);
			return;
		};
		// used to disable if some code below fails
		if (gOS_m_bIsEnabled) {
			GetGame().GetCallqueue().Remove(gOS_MiniMap_Update);
			GetGame().GetCallqueue().CallLater(gOS_MiniMap_Update, 1, false);
		} else {
			return;
		};
		bool t_bIsTimeNotSet = gOS_m_fTimeLastMiniMap == 0;
		// if (gOS_m_fTimeLastMiniMap == 0) {
			
		// }

		// call again if it should check again (it can re-enable it etc).
		float t_fTimeCurrent = GetGame().GetWorld().GetWorldTime() * 0.001;
		float t_fTimePassed = t_fTimeCurrent - gOS_m_fTimeLastMiniMap;
		gOS_m_fTimeLastMiniMap = t_fTimeCurrent;
		if (t_bIsTimeNotSet) {

			GetGame().GetCallqueue().Remove(gOS_MiniMap_Update);
			// GetGame().GetCallqueue().CallLater(gOS_MiniMap_Update, 3333, false);
			GetGame().GetCallqueue().CallLater(gOS_MiniMap_Update, 1, false);
			return;
		};
		// GetGame().GetCallqueue().CallLater(gOS_MiniMap_Update, 50, false);
        IEntity player = SCR_PlayerController.GetLocalControlledEntity();
        if (!player) {
            return;
        };
        SCR_GadgetManagerComponent gadgetManager = SCR_GadgetManagerComponent.GetGadgetManager(player);
        if (!gadgetManager) {
            return;
        };
        IEntity mapGadget = gadgetManager.GetGadgetByType(EGadgetType.MAP);
        if (!mapGadget) {
			return;
		};
		SCR_MapGadgetComponent t_Comp_Gadget = SCR_MapGadgetComponent.Cast(mapGadget.FindComponent(SCR_MapGadgetComponent));
		if (!t_Comp_Gadget) {
			return;
		};
		if (t_Comp_Gadget.GetMode() != EGadgetMode.IN_HAND) {
			// Print("gOS::gOS_MiniMap_Update::Disable gadgetmap act");

			// testing for vehicle map:
			if (!gOS_m_bIsPlaced_Map_Enabled) {
				gOS_MiniMap_SetEnabled(false);
				return;
			};

			if (!gOS_SCR_MiniMapComponent.gOS_s_IEnt_Placed) {
				gOS_MiniMap_SetEnabled(false);
				return;
			};

			SCR_ChimeraCharacter t_Char = SCR_ChimeraCharacter.Cast(player);
			// CompartmentAccessComponent t_Comp_CompartmentAccess = t_Char.GetCompartmentAccessComponent();
			Vehicle t_Vehicle = Vehicle.Cast(CompartmentAccessComponent.GetVehicleIn(t_Char));
			if (!t_Vehicle) {

				// disable the placed map because we want to focus on the main one if we got out
				gOS_m_bIsPlaced_Map_Enabled = false;
				gOS_MiniMap_SetEnabled(false);
				return;
			};



			// float f_Dist_Map_Placed = vector.Distance(
			// 	player.GetOrigin()
			// 	, gOS_SCR_MiniMapComponent.gOS_s_IEnt_Placed.GetOrigin()
			// );
			// if (f_Dist_Map_Placed > 5) {
			// 	gOS_MiniMap_SetEnabled(false);
			// 	return;
			// };
			// gOS_m_bIsPlaced_Map_Enabled
		};
		MenuManager menuManager = GetGame().GetMenuManager();
		SCR_MapMenuUI t_MapMenu = SCR_MapMenuUI.Cast(menuManager.FindMenuByPreset(ChimeraMenuPreset.MapMenu));
		// if the big map is already open, we can close this map
		if (t_MapMenu) {
			// Print("gOS_SCR_MiniMap_HUD::gOS_MiniMap_Update::fail there is a map already open");
			return;
		};
		if (m_MenuManager.IsAnyMenuOpen()) {
			// Print("gOS_SCR_MiniMap_HUD::gOS_MiniMap_Update::IsAnyMenuOpen YES");
			return;
		};

		// update to player position
		SCR_MapEntity t_ME = SCR_MapEntity.GetMapInstance();
		if (!t_ME || !t_ME.IsOpen()) {
			// Print("gOS_SCR_MiniMap_HUD::gOS_MiniMap_Update::NOT OPEN");
			return;
		};

			
		WorkspaceWidget t_wWorkspace = GetGame().GetWorkspace();
		bool t_bIsSomethingFocused = t_wWorkspace.GetFocusedWidget() != NULL;

		//--- focused widget detection:
		// * blocks minimap input when chat is open
		// * 55ms delay to allow input
		//		* this is because otherwise enter triggers when sloppy 
		if (t_bIsSomethingFocused) {
			gOS_m_bIsInputEnabled = false;
			GetGame().GetCallqueue().Remove(gOS_Input_Enable);
			gOS_m_bIsInputRequested = false;

		} else if (!gOS_m_bIsInputEnabled) {
			// Print("huh");
			if (!gOS_m_bIsInputRequested) {
				// Print("Request");
				GetGame().GetCallqueue().CallLater(gOS_Input_Enable, 155, false);
				gOS_m_bIsInputRequested = true;
			};
		};
		if (gOS_m_bIsInputEnabled) {
			EInputDeviceType t_eInputDeviceType = GetGame().GetInputManager().GetLastUsedInputDevice();

			bool t_bIsPlayerDriver = false;
			bool t_bIsGamePad = t_eInputDeviceType == EInputDeviceType.GAMEPAD;

			if (t_bIsGamePad) {

				SCR_ChimeraCharacter t_Char_Controlled = SCR_ChimeraCharacter.Cast(player);
				if (t_Char_Controlled) {
					CompartmentAccessComponent t_Comp_CompartmentAccess = t_Char_Controlled.GetCompartmentAccessComponent();
					if (
						t_Comp_CompartmentAccess
						&& t_Comp_CompartmentAccess.GetCompartment()
					) {
						// ECompartmentType t_eCmpType = SCR_CompartmentAccessComponent.GetCompartmentType(t_Comp_CompartmentAccess.GetCompartment());
						ECompartmentType t_eCmpType = t_Comp_CompartmentAccess.GetCompartment().GetType();
						t_bIsPlayerDriver = t_eCmpType == ECompartmentType.PILOT;
					};
				};
			};

			if (!gOS_m_bIsPlaced_Block_Keys || !gOS_m_bIsPlaced_Map_Enabled) {

				if (t_bIsPlayerDriver) {
					if (gOS_m_bEnableGPadVehPan && gOS_m_bEnableGPadVehZoom) {
						GetGame().GetInputManager().ActivateContext("gOS_Context_MiniMap", 777);
						// Print("gOS_Context_MiniMap");

					} else if (gOS_m_bEnableGPadVehPan && !gOS_m_bEnableGPadVehZoom) {
						GetGame().GetInputManager().ActivateContext("gOS_Context_MiniMap_NoZoom", 777);
						// Print("gOS_Context_MiniMap_NoZoom");

					} else if (!gOS_m_bEnableGPadVehPan && !gOS_m_bEnableGPadVehZoom) {
						GetGame().GetInputManager().ActivateContext("gOS_Context_MiniMap_NoPanNoZoom", 777);
						// Print("gOS_Context_MiniMap_NoPanNoZoom");

					} else if (!gOS_m_bEnableGPadVehPan && gOS_m_bEnableGPadVehZoom) {
						GetGame().GetInputManager().ActivateContext("gOS_Context_MiniMap_NoPan", 777);
						// Print("gOS_Context_MiniMap_NoPan");

					};
				} else {
					
					GetGame().GetInputManager().ActivateContext("gOS_Context_MiniMap", 777);
				};
			} else {

				GetGame().GetInputManager().ActivateContext("gOS_Context_MiniMap_Placed_Blocked", 777);
			};
			
			//! TODO:
			// - script invoker for auto-updating varibale using setting
			// - save into static variable on some class...
			BaseContainer t_Settings_gOS = GetGame().GetGameUserSettings().GetModule("gOS_SCR_Settings");
			bool t_bIsTest;
			if (t_Settings_gOS.Get("m_bIsTest1", t_bIsTest)) {
				// Print(t_bIsTest);
			};
		
			HCA_RadUI_HUD t_RadUI_HUD = HCA_RadUI_HUD.HCA_GetCurrent();
			bool t_bRadUI_DisablesBlocker = t_RadUI_HUD && t_RadUI_HUD.HCA_RadUI_m_bIsOpen;
			SCR_CharacterControllerComponent t_CharCtrl = SCR_CharacterControllerComponent.Cast(SCR_PlayerController.GetLocalControlledEntity().FindComponent(SCR_CharacterControllerComponent));
			if (t_CharCtrl && !t_bRadUI_DisablesBlocker) {
				

				if (!gOS_m_bIsPlaced_Block_Keys || !gOS_m_bIsPlaced_Map_Enabled) {
					// Print("NOT blocked");
					t_CharCtrl.gOS_ConflictBlock("gOS_MiniMap_PanX_Left", true, 0.3);
					t_CharCtrl.gOS_ConflictBlock("gOS_MiniMap_PanX_Right", true, 0.3);
					t_CharCtrl.gOS_ConflictBlock("gOS_MiniMap_PanY_Up", true, 0.3);
					t_CharCtrl.gOS_ConflictBlock("gOS_MiniMap_PanY_Down", true, 0.3);
					t_CharCtrl.gOS_ConflictBlock("gOS_MiniMap_Zoom_In", true, 0.3);
					t_CharCtrl.gOS_ConflictBlock("gOS_MiniMap_Zoom_Out", true, 0.3);
					t_CharCtrl.gOS_ConflictBlock("gOS_MiniMap_POI", true, 0.3);
				};
					
				t_CharCtrl.gOS_ConflictBlock("gOS_MiniMap_Settings", true, 0.3);

				// allow map button to be allowlisted
				if (gOS_m_bIsHoldForMap) {
					
					t_CharCtrl.gOS_ConflictBlock("gOS_Map_Hold", true, 0.3);
					// Print("Block hold map conflicts");
				} else {
					
					t_CharCtrl.gOS_ConflictBlock("GadgetMap", true, 0.3);
				};


				

				ChimeraCharacter t_Char = t_CharCtrl.GetCharacter();

				// IEntity vehicle = CompartmentAccessComponent.GetVehicleIn(char);
				// if (!vehicle)
				// 	return;
				if (gOS_m_bIsHoldForMap && t_bIsGamePad && gOS_m_bGPBlockGadgetAway && t_Char && t_Char.IsInVehicle()) {		
					Vehicle t_Vehicle = Vehicle.Cast(CompartmentAccessComponent.GetVehicleIn(t_Char));
					if (t_Vehicle) {

						if (!gOS_m_bIsPlaced_Block_Keys || !gOS_m_bIsPlaced_Map_Enabled) {
							t_CharCtrl.gOS_ConflictBlock("CarTurbo", true, 0.3);
							t_CharCtrl.gOS_ConflictBlock("CarThrust", true, 0.3);
						};
					};
				};
        	};
		};

		// };


		float o_fInput_Pan_Max = 50;
		float o_fInput_Zoom_Max = 10;
		float o_fPan_Max = 2000;
		float o_fZoom_Min = 1;
		float o_fZoom_Max = 30;

        // marker hack :^)
        // gOS_CreateMarker("", player.GetOrigin(), "up");

		// t_ME.OpenMap(gOS_m_MapCfg);
		vector t_vPos_Char = SCR_PlayerController.GetLocalControlledEntity().GetOrigin();
		vector t_vPos = gOS_m_vPos_Center;


		// make world center the position?
		vector t_vSize_World = t_ME.Size();
		float t_fWorldX_Size = t_vSize_World[0];
		float t_fWorldX = t_fWorldX_Size * 0.5;
		float t_fWorldY_Size = t_vSize_World[2];
		float t_fWorldY = t_fWorldY_Size * 0.5;

		o_fPan_Max = t_fWorldX_Size * 0.5;
		o_fInput_Pan_Max = (t_fWorldX_Size * 0.5) / 40;

		t_vPos[0] = t_fWorldX;
		t_vPos[2] = t_fWorldY;

		float t_fPosX = t_vPos[0];
		float t_fPosY = t_vPos[2];

		if (gOS_m_bControl_POI_Requested) {
			// t_vPos = t_vPos_Char;
		};
		// Print(t_vSize_World);
		// Print(gOS_m_fControl_PanX);

		if (gOS_m_vPosLast == vector.Zero) {
			
			gOS_m_fControl_PanX = (t_vPos_Char[0] - t_fPosX) * (o_fInput_Pan_Max / o_fPan_Max);
			gOS_m_fControl_PanY = (t_fPosY - t_vPos_Char[2]) * (o_fInput_Pan_Max / o_fPan_Max);
		};




		float screenW, screenH;
		float t_fScreenX,t_fScreenY;
		WorkspaceWidget workspace = GetGame().GetWorkspace();
		if (workspace) {
			workspace.GetScreenSize(screenW, screenH);
		} else {
			screenW = 1280;
		};
		// this value is different based on different screen sizes
		float t_fZoom = 0.00025 * screenW;
		float t_fScreenMod = 00025 * screenW;
		t_fZoom = 1;


		// limits:
		if (gOS_m_fControl_Zoom > o_fInput_Zoom_Max) {
			gOS_m_fControl_Zoom = o_fInput_Zoom_Max;
		};
		if (gOS_m_fControl_Zoom < -o_fInput_Zoom_Max) {
			gOS_m_fControl_Zoom = -o_fInput_Zoom_Max;
		};
		
		if (gOS_m_fControl_PanX > o_fInput_Pan_Max) {
			gOS_m_fControl_PanX = o_fInput_Pan_Max;
		};
		if (gOS_m_fControl_PanX < -o_fInput_Pan_Max) {
			gOS_m_fControl_PanX = -o_fInput_Pan_Max;
		};
		if (gOS_m_fControl_PanY > o_fInput_Pan_Max) {
			gOS_m_fControl_PanY = o_fInput_Pan_Max;
		};
		if (gOS_m_fControl_PanY < -o_fInput_Pan_Max) {
			gOS_m_fControl_PanY = -o_fInput_Pan_Max;
		};



        float t_fMod_Zoom = Math.InverseLerp(-o_fInput_Zoom_Max, o_fInput_Zoom_Max, gOS_m_fControl_Zoom);
        t_fMod_Zoom = Math.Clamp(t_fMod_Zoom, 0, 1);
        t_fMod_Zoom = Math.Lerp(o_fZoom_Min, o_fZoom_Max, t_fMod_Zoom);



		// Apply zoom?
		t_fZoom = t_fZoom * t_fMod_Zoom;
		
		gOS_m_fPanScale = 1 / t_fZoom;
        gOS_m_fPanScale = gOS_m_fPanScale * (t_fTimePassed * 25);

        // gOS_m_fZoomScale = t_fZoom * t_fTimePassed * 5;
        gOS_m_fZoomScale = t_fZoom * t_fTimePassed * 2.5;

		float t_fPOI_TimePassed = t_fTimeCurrent - gOS_m_fPOI_Time_Started;
		bool t_bIsMapMove = t_fPOI_TimePassed < 1;

		//--- override when POI is active
		//	- Focus on POI
		//	- show on screen taht POI in focus
		if (gOS_m_bControl_POI_Requested) {
            if (!t_bIsMapMove) {
            };


            
            // hack for converting the MINIMAP adjustments to the MapLocator
            vector t_vPosLoc = player.GetOrigin();
            float t_fPosX_Loc = t_vPosLoc[0];
            float t_fPosY_Loc = t_vPosLoc[2];

            float t_fPos_Speed = 1;
            t_fPos_Speed = t_fPos_Speed * o_fInput_Pan_Max*2;
            float t_fPosX_Change = t_fPos_Speed * t_fTimePassed;
            float t_fPosY_Change = t_fPosX_Change;

            float t_fZoom_Speed = 1 * t_fTimePassed;
            t_fZoom_Speed = t_fZoom_Speed * o_fInput_Zoom_Max*2;

            // calculate the DESIRED Control values
            //	- HOw much COORD difference is between Location and my map
            //	- 5000 - 4500 = 500
            //		- this would mean I need 500 m to the right
            t_fPosX_Loc = t_fPosX_Loc - t_fPosX;
            t_fPosY_Loc = t_fPosY - t_fPosY_Loc;
            // Print(t_fPosY_Loc);

            // resulting positons:
            float t_fControl_DesiredX = t_fPosX_Loc * (o_fInput_Pan_Max / o_fPan_Max);
            float t_fControl_DesiredY = t_fPosY_Loc * (o_fInput_Pan_Max / o_fPan_Max);



            if (gOS_m_fControl_PanX > t_fControl_DesiredX) {
                t_fPosX_Change = 0 - t_fPosX_Change;
            };
            gOS_m_fControl_PanX = gOS_m_fControl_PanX + t_fPosX_Change;
            if (t_fPosX_Change > 0) {
                if (gOS_m_fControl_PanX > t_fControl_DesiredX) {
                    // Print("X too big:" + gOS_m_fControl_PanX);
                    gOS_m_fControl_PanX = t_fControl_DesiredX;
                };
            } else {
                if (gOS_m_fControl_PanX < t_fControl_DesiredX) {
                    // Print("X too small:" + gOS_m_fControl_PanX);
                    gOS_m_fControl_PanX = t_fControl_DesiredX;
                };
            };

            if (gOS_m_fControl_PanY > t_fControl_DesiredY) {
                t_fPosY_Change = 0 - t_fPosY_Change;
            };
            gOS_m_fControl_PanY = gOS_m_fControl_PanY + t_fPosY_Change;
            if (t_fPosY_Change > 0) {
                if (gOS_m_fControl_PanY > t_fControl_DesiredY) {
                    // Print("X too big:" + gOS_m_fControl_PanX);
                    gOS_m_fControl_PanY = t_fControl_DesiredY;
                };
            } else {
                if (gOS_m_fControl_PanY < t_fControl_DesiredY) {
                    // Print("X too small:" + gOS_m_fControl_PanX);
                    gOS_m_fControl_PanY = t_fControl_DesiredY;
                };
            };
		};

        string t_sTextNew = "[ ]";
        if (gOS_m_bControl_POI_Requested) {
            t_sTextNew = "[X]";
        };
        if (gOS_m_sLocator_NameLast != t_sTextNew) {
            TextWidget t_wLocatorText = TextWidget.Cast(m_wRoot.FindAnyWidget("m_TextMap_POI"));
            if (t_wLocatorText) {
                t_wLocatorText.SetText(t_sTextNew);
                gOS_m_sLocator_NameLast = t_sTextNew;
            };
        };

		gOS_m_vPosLast[0] = t_fPosX;
		gOS_m_vPosLast[2] = t_fPosY;


        float t_fMod_X = Math.InverseLerp(-o_fInput_Pan_Max, o_fInput_Pan_Max, gOS_m_fControl_PanX);
        t_fMod_X = Math.Clamp(t_fMod_X, 0, 1);
        t_fMod_X = Math.Lerp(-o_fPan_Max, o_fPan_Max, t_fMod_X);
		// t_fMod_X = t_fMod_X * t_fZoom;

		// Print(t_fPosX);
		t_fPosX = t_fPosX + t_fMod_X;

        float t_fMod_Y = Math.InverseLerp(-o_fInput_Pan_Max, o_fInput_Pan_Max, gOS_m_fControl_PanY);
        t_fMod_Y = Math.Clamp(t_fMod_Y, 0, 1);
        t_fMod_Y = Math.Lerp(o_fPan_Max, -o_fPan_Max, t_fMod_Y);
		// t_fMod_Y = t_fMod_Y * t_fZoom;
		t_fPosY = t_fPosY + t_fMod_Y;

		t_fZoom = (t_fMod_Zoom * (t_fScreenMod * 0.00001));
		if (t_fZoom == 0) {
			t_fZoom = 1;
		};
		t_ME.SetZoom(t_fZoom, true);
		t_ME.WorldToScreen(t_fPosX, t_fPosY, t_fScreenX,t_fScreenY);
		t_fScreenX = workspace.DPIUnscale(t_fScreenX);
		t_fScreenY = workspace.DPIUnscale(t_fScreenY);

		t_ME.SetPan(t_fScreenX, t_fScreenY, true, true);

		// 
		// Print("gOS_SCR_MiniMap_HUD::gOS_MiniMap_Update::Update Done");
	};
	void gOS_MiniMap_Update2 () {
		return;

		Widget t_wMapFrame = m_wRoot.FindAnyWidget("MapWidget");
		if (!m_wRoot) {
			// Print("gOS_SCR_MiniMap_HUD::gOS_MiniMap_SetEnabled::fail no map frame");
			return;
		};

		float t_fFPS = System.GetFPS();
		if (t_fFPS < 10) {
			t_fFPS = 10;
		};
		float t_fOpacity = t_wMapFrame.GetOpacity();

		if (t_fOpacity >= 1 || !gOS_m_bIsEnabled) {
			return;
		};

		float t_fOpacity_Changed = 1 / t_fFPS;
		Print(t_fOpacity + t_fOpacity_Changed);

		
		GetGame().GetCallqueue().CallLater(gOS_MiniMap_Update2, 1555, false);
	};

	void gOS_OnKeyDown_OpenMenu () {
		// Print("I am open menu");

		// gOS_SCR_Mny_MenuUI.Open();
		// gOS_AARMenuUI.Open();


		// gOS_Mny_AddUpdate(15, gOS_Mny_ETransaction.DAMAGE);


		// gOS_Mny_BankingMenuUI.Open();
	};

	// this function is added to the event handler
	void gOS_ToggleKeyDown (float value, EActionTrigger trigger) {


		gOS_OnToggleKey();
	};

	// actually runs code
	protected void gOS_OnToggleKey () {

	};



	//--- Static setters
	// these are used to set the values of the HUD
	//	- If hud doesn't exist, value is saved
	//	- when hud loads, values are re-applied

	// map of widget status
	// gOS_SCR_MiniMap_HUD.gOS_s_mWidgetData
	static ref map<string, int> gOS_s_mWidgetData = new map<string, int>();

	static void gOS_ResetWidgetValues () {
		gOS_SCR_MiniMap_HUD t_CurrentHUD = gOS_GetCurrent();
		foreach (string t_sName, int t_iValue: gOS_s_mWidgetData) {
			if (t_iValue == 1) {
				if (t_CurrentHUD) {
					t_CurrentHUD.gOS_ApplyWidgetValue(t_sName, 0);
				};
			};
		};
		
		delete gOS_s_mWidgetData;
		gOS_s_mWidgetData = new map<string, int>;
	};

	// gOS_SCR_MiniMap_HUD.gOS_SetWidgetValue("valval", 15)
	static void gOS_SetWidgetValue (string f_sName, int f_iValue) {

		int t_iOldValue;
		if (gOS_s_mWidgetData.Find(f_sName, t_iOldValue)) {
			if (t_iOldValue == f_iValue) {
				return;
			};
			gOS_s_mWidgetData.Set(f_sName, f_iValue);
		} else {
			gOS_s_mWidgetData.Insert(f_sName, f_iValue);
		};

		// Print("gOS::gOS_SCR_MiniMap_HUD::gOS_SetWidgetValue::Name:" + f_sName + " Value:" + f_iValue);
				
		if (!gOS_SCR_MiniMap_HUD.gOS_GetCurrent()) {
			return;
		};

		gOS_SCR_MiniMap_HUD.gOS_GetCurrent().gOS_ApplyWidgetValue(f_sName, f_iValue);
	};

	void gOS_ApplyWidgetValue (string f_sName, int f_iValue) {

		if (!m_wRoot) {
			return;
		};
		Widget t_wMain = m_wRoot.FindAnyWidget(f_sName);
		if (!t_wMain) {
			// Print("gOS::gOS_SCR_MiniMap_HUD::gOS_ApplyWidgetValue::NO WIDGET! Name:" + f_sName + " Value:" + f_iValue);
			return;
		};


		// value interpetrations:
		// 0
		// Disabled widget
		//
		// > 1
		// Enabled widget
		//
		// 1000 - 2000
		// Enabled with MASK

		if (f_iValue == 0) {
			t_wMain.SetVisible(false);
			return;
		};
		t_wMain.SetVisible(true);

		if (f_iValue < 1000) {
			return;
		};

		ImageWidget t_wImage = ImageWidget.Cast(t_wMain.FindAnyWidget("Mask"));
		if (!t_wImage) {
			return;
		};

		// float t_fMaskProgress = ((f_iValue - 100) * 0.001) + 0.1;
		// float t_fMaskProgress = ((f_iValue - 100) * 0.0011);
		float t_fMaskProgress = ((f_iValue - 1000) * 0.001);
		t_wImage.SetMaskProgress(t_fMaskProgress);

	};


};



modded class SCR_MapSelectionModule {
	
	
	override void OnMapOpen (MapConfiguration config) {

		if (
			!m_CursorModule
			|| !m_MapEntity
			|| !m_DrawCanvas
			|| !m_CursorInfo
			|| !m_DrawCanvas
			|| !m_MapEntity
			
		) {
			return;
		};

		super.OnMapOpen(config);
	};
	
};
