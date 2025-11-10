[ComponentEditorProps(category: "GameScripted/GameMode/Components", description: "vPad component, required for settings and syncronization")]
class gOS_SCR_GameModeComp_MainClass : SCR_BaseGameModeComponentClass {

}

class gOS_SCR_GameModeComp_Main : SCR_BaseGameModeComponent {


    // data sync
    ref map<string, bool> gOS_m_mVarNameToBool = new map<string, bool>;
    ref map<string, int> gOS_m_mVarNameToUpdateID = new map<string, int>;
    ref map<string, int> gOS_Srv_m_mVarNameToUpdateID = new map<string, int>;

	void gOS_Data_Bool_Set (
        string f_sVarName
        , bool f_bValue
    ) {
        gOS_m_mVarNameToBool.Set(f_sVarName, f_bValue);

        // broadcast:
		// int t_iSrvTick = System.GetTickCount();
		int t_iUpdateID;
        if (!gOS_Srv_m_mVarNameToUpdateID.Find(f_sVarName, t_iUpdateID)) {
            t_iUpdateID = 0;
        };
        t_iUpdateID = t_iUpdateID + 1;

        gOS_Srv_m_mVarNameToUpdateID.Set(f_sVarName, t_iUpdateID);

        // ! TODO:
        //  - test if server sets the value correctly (because of onRecieve)

        Rpc(
            gOS_S2B_Data_Bool_Send
            , f_sVarName
            , f_bValue
            , t_iUpdateID
        );

        
        // save the data 10 seconds after anything changes
        GetGame().GetCallqueue().Remove(gOS_Data_Save);
        GetGame().GetCallqueue().CallLater(gOS_Data_Save, 10000, false);
    };

    // bool t_bIsEnabled = gOS_SCR_GameModeComp_Main.gOS_S_Bool_Get("bMyVar", true);
    static bool gOS_S_Bool_Get (
        string f_sVarName
        , bool f_bValue_Default = false
    ) {
        gOS_SCR_GameModeComp_Main t_GMComp_Main = gOS_SCR_GameModeComp_Main.gOS_GetComp();
        if (!t_GMComp_Main) {
            return f_bValue_Default;
        };
        return t_GMComp_Main.gOS_Data_Bool_Get(f_sVarName, f_bValue_Default);
    };
	bool gOS_Data_Bool_Get (
        string f_sVarName
        , bool f_bValue_Default = false
    ) {
        bool t_bValue;
        if (gOS_m_mVarNameToBool.Find(f_sVarName, t_bValue)) {

            return t_bValue;
        };
        return f_bValue_Default;
    };
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void gOS_S2B_Data_Bool_Send (
        string f_sVarName
        , bool f_bValue
        , int f_iUpdateID
    ) {
        // Print("gOS_S2B_Data_Bool_Send");

        int t_iUpdateID_Last;
        if (gOS_m_mVarNameToUpdateID.Find(f_sVarName, t_iUpdateID_Last)) {

            // last update was newer
            if (t_iUpdateID_Last > f_iUpdateID) {
                return;
            };
        };

        
        gOS_m_mVarNameToBool.Set(f_sVarName, f_bValue);
        gOS_Data_Bool_OnRecieve(f_sVarName, f_bValue);
    };

    void gOS_Data_Bool_OnRecieve (
        string f_sVarName
        , bool f_bValue
    ) {
        // Print("gOS_Data_Bool_OnRecieve::start::" + f_sVarName + " = " + f_bValue);

        bool t_bHasClientOpenedSettingsEver = false;
        BaseContainer t_Settings_gOS = GetGame().GetGameUserSettings().GetModule("gOS_SCR_Settings");
        if (t_Settings_gOS) {
            t_Settings_gOS.Get("m_bHasClientOpenedSettingsEver", t_bHasClientOpenedSettingsEver);
        };
        // Print("gOS_Data_Bool_OnRecieve::OpenedSettingsEver:" + t_bHasClientOpenedSettingsEver);
        
        if (f_sVarName == "bSetting_Default_Disable") {
            if (t_bHasClientOpenedSettingsEver) {
                // Print("gOS_Data_Bool_OnRecieve::bSetting_Default_Disable, settings were opened");
                return;
            };
            t_Settings_gOS.Set("m_bIsMinimapEnabled", f_bValue);
            GetGame().UserSettingsChanged();
            GetGame().SaveUserSettings();

            // Print("gOS_Data_Bool_OnRecieve::bSetting_Default_Disable:" + f_bValue);
            return;
        };
        if (f_sVarName == "bSetting_Default_TapForMap") {
            if (t_bHasClientOpenedSettingsEver) {
                // Print("gOS_Data_Bool_OnRecieve::bSetting_Default_TapForMap, settings were opened");
                return;
            };
            t_Settings_gOS.Set("m_bIsTapForMap", f_bValue);
            GetGame().UserSettingsChanged();
            GetGame().SaveUserSettings();

            // Print("gOS_Data_Bool_OnRecieve::bSetting_Default_TapForMap:" + f_bValue);
            return;
        };
        if (f_sVarName.Contains("bSetting_IsPaperMap_Forced_")) {

            // could loop over all the factions and variable names here
            // Could just make ALL players automatically test their own faction
        };
    };



	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void gOS_C2S_Data_Request (
        int f_iGameID_Player
    ) {
        // Print("gOS_SCR_GameModeComp_Main::gOS_C2S_Data_Request::Client:" + f_iGameID_Player);


        
        // Rpc(gOS_C2S_Data_Request);
    };


    void gOS_Data_OnConnect (
        // int f_iGameID_Player
        gOS_SCR_PlayerController_Comp_Main f_PCComp_Main
    ) {
        // Print("gOS_SCR_GameModeComp_Main::gOS_Data_OnConnect:" + f_PCComp_Main);

        // if (Replication.IsServer()) {
        //     return;
        // };
        if (!f_PCComp_Main) {
            return;
        };

        int t_iUpdateID;
        foreach (string t_sVarName, bool t_bValue : gOS_m_mVarNameToBool) {
            if (!gOS_Srv_m_mVarNameToUpdateID.Find(t_sVarName, t_iUpdateID)) {
                t_iUpdateID = 0;
            };
            f_PCComp_Main.Rpc(
                f_PCComp_Main.gOS_S2C_Data_Bool_Send
                , t_sVarName
                , t_bValue
                , t_iUpdateID
            );
        };



        // int t_iGameID_Player = SCR_PlayerController.GetLocalPlayerId();
        // Print("gOS_SCR_GameModeComp_Main::gOS_Data_OnConnect::Start Client:" + t_iGameID_Player);
        // Rpc(gOS_C2S_Data_Request, t_iGameID_Player);
        
        // array<int> players = new array<int>;
        // GetGame().GetPlayerManager().GetPlayers(players);

    };










	[RplProp(onRplName: "gOS_RplProp_IsEnabled_OnChanged")]
	bool gOS_m_bIsEnabled = false;
	[RplProp(onRplName: "gOS_RplProp_IsPaperMap_OnChanged")]
	bool gOS_m_bIsPaperMap = false;









	void gOS_RplProp_IsPaperMap_OnChanged () {
		// Print("gOS_SCR_GameModeComp_Main::gOS_RplProp_IsPaperMap_OnChanged:" + gOS_m_bIsPaperMap);
	};
	void gOS_IsPaperMap_Set (bool f_bIsPaperMap) {
		// Print("gOS_SCR_GameModeComp_Main::gOS_IsPaperMap_Set");
		gOS_m_bIsPaperMap = f_bIsPaperMap;
		Replication.BumpMe();

        // save the data 10 seconds after anything changes
        GetGame().GetCallqueue().Remove(gOS_Data_Save);
        GetGame().GetCallqueue().CallLater(gOS_Data_Save, 10000, false);
	};
	bool gOS_IsPaperMap_Get () {
		// Print("gOS_SCR_GameModeComp_Main::gOS_IsPaperMap_Get");
		return gOS_m_bIsPaperMap;
	};
	string gOS_SettingName_GetFactionVar (SCR_Faction f_Faction, string f_sSetting) {
        if (!f_Faction) {
            Print("gOS_IsPaperMap_Forced_GetFactionVar:: fail");
            return "fail";
        };

		string t_sFactionKey = f_Faction.GetFactionKey();
		string t_sVarName_Data = ("bSetting_" + f_sSetting + "_" + t_sFactionKey);
        return t_sVarName_Data;
    };
	string gOS_IsPaperMap_Forced_GetFactionVar (SCR_Faction f_Faction) {
        if (!f_Faction) {
            Print("gOS_IsPaperMap_Forced_GetFactionVar:: fail");
            return "fail";
        };

		string t_sFactionKey = f_Faction.GetFactionKey();
		string t_sVarName_Data = ("bSetting_IsPaperMap_Forced_" + t_sFactionKey);
        return t_sVarName_Data;
    };


	bool gOS_SettingName_GetFactionBool (
        SCR_Faction f_Faction
        , string f_sSetting
        , bool f_bIsEnabled_Default = false
    ) {
        if (!f_Faction) {
            Print("gOS_SettingName_GetFactionBool::fail:" + f_sSetting);
            return f_bIsEnabled_Default;
        };

		string t_sFactionKey = f_Faction.GetFactionKey();
		string t_sVarName_Data = ("bSetting_" + f_sSetting + "_" + t_sFactionKey);
        return gOS_Data_Bool_Get(t_sVarName_Data, f_bIsEnabled_Default);
    };













	void gOS_RplProp_IsEnabled_OnChanged () {
		// Print("gOS_SCR_GameModeComp_Main::gOS_RplProp_IsEnabled_OnChanged:" + gOS_m_bIsEnabled);
	};
	void gOS_IsEnabled_Set (bool f_bIsEnabled) {
		// Print("gOS_SCR_GameModeComp_Main::gOS_IsEnabled_Set");
		gOS_m_bIsEnabled = f_bIsEnabled;
		Replication.BumpMe();


        // if (Replication.IsServer()) {
		    // Print("gOS_SCR_GameModeComp_Main::gOS_IsEnabled_Set::Servussy");
        // };

        //! TODO:
        //  - If a variable changes often, such as brightness of screen, DO NOT save

        // save the data 10 seconds after anything changes
        GetGame().GetCallqueue().Remove(gOS_Data_Save);
        GetGame().GetCallqueue().CallLater(gOS_Data_Save, 10000, false);
        
	};
	bool gOS_IsEnabled_Get () {
		// Print("gOS_SCR_GameModeComp_Main::gOS_IsEnabled_Get");
		return gOS_m_bIsEnabled;
	};

    // gOS_SCR_GameModeComp_Main t_GMComp_Main = gOS_SCR_GameModeComp_Main.gOS_GetComp();
    static gOS_SCR_GameModeComp_Main gOS_GetComp () {


        BaseGameMode t_GameMode = GetGame().GetGameMode();
		if (!t_GameMode) {
		    // Print("gOS_SCR_GameModeComp_Main::gOS_GetComp::NO GameMode");
			return null;
        };		
		gOS_SCR_GameModeComp_Main t_GMComp_Main = gOS_SCR_GameModeComp_Main.Cast(t_GameMode.FindComponent(gOS_SCR_GameModeComp_Main));
        if (!t_GMComp_Main) {
		    // Print("gOS_SCR_GameModeComp_Main::gOS_GetComp::NO GMComp main");
            return null;
        };

        return t_GMComp_Main;
    };



    // load data etc.
	override void OnPostInit (IEntity owner) {
		super.OnPostInit(owner);


        gOS_OnPostInit(owner);
	};
    void gOS_OnPostInit (IEntity f_IEnt_Owner) {
        // Print("gOS_SCR_GameModeComp_Main::gOS_OnPostInit::START");

        // data connection code for clientel
        // gOS_Data_OnConnect();

        gOS_m_Data = new gOS_Data_Settings;

        // try loading
        if (gOS_m_Data.gOS_Data_LoadFromFile()) {
            gOS_Data_ApplyToGame();
            return;
        };

        // loading failed (didn't exist)
        // * create load based on current values
        gOS_Data_Save();

    };
    ref gOS_Data_Settings gOS_m_Data;

    void gOS_Data_Save () {
        // Print("gOS_SCR_GameModeComp_Main::gOS_Data_Save::START");
        if (!gOS_m_Data) {
            gOS_m_Data = new gOS_Data_Settings;
        };

        if (!Replication.IsServer()) {
		    // Print("gOS_SCR_GameModeComp_Main::gOS_Data_Save::NOT SERVER");
        };


        gOS_Data_LoadFromGame();
        gOS_m_Data.gOS_Data_SaveToFile();
    };
    void gOS_Data_LoadFromGame () {
        // Print("gOS_SCR_GameModeComp_Main::gOS_Data_LoadFromGame::START");
        if (!gOS_m_Data) {
            gOS_m_Data = new gOS_Data_Settings;
        };



		gOS_m_Data.gOS_m_bIsEnabled = gOS_m_bIsEnabled;
        // Print("gOS_Data_LoadFromGame::gOS_m_bIsEnabled:" + gOS_m_bIsEnabled);

        gOS_m_Data.gOS_m_aBool_VarNames.Clear();
        gOS_m_Data.gOS_m_aBool_Values.Clear();
        int t_iIdx = 0;
        foreach (string t_sVarName, bool t_bValue : gOS_m_mVarNameToBool) {
            gOS_m_Data.gOS_m_aBool_VarNames.Insert(t_sVarName);
            gOS_m_Data.gOS_m_aBool_Values.Insert(t_bValue);
        };
    };
    void gOS_Data_ApplyToGame () {
        // Print("gOS_SCR_GameModeComp_Main::gOS_Data_ApplyToGame::START");
        if (!gOS_m_Data) {
            gOS_m_Data = new gOS_Data_Settings;
        };

        if (!Replication.IsServer()) {
		    // Print("gOS_SCR_GameModeComp_Main::gOS_Data_ApplyToGame::NOT SERVER");
        };


	    gOS_IsEnabled_Set(gOS_m_Data.gOS_m_bIsEnabled);


        gOS_m_mVarNameToBool.Clear();
        int t_iIdx = 0;
        int t_iCount = gOS_m_Data.gOS_m_aBool_VarNames.Count();

        string t_sVarName;
        bool t_bValue;
        for (; t_iIdx < t_iCount; t_iIdx++) {
            t_sVarName = gOS_m_Data.gOS_m_aBool_VarNames.Get(t_iIdx);
            t_bValue = gOS_m_Data.gOS_m_aBool_Values.Get(t_iIdx);
            gOS_m_mVarNameToBool.Insert(t_sVarName, t_bValue);
        };

    };
};



class gOS_Data_Settings : JsonApiStruct {

    bool gOS_m_bIsEnabled = false;
    ref array<string> gOS_m_aBool_VarNames = new array<string>;
    ref array<bool> gOS_m_aBool_Values = new array<bool>;


    void gOS_Data_Settings () {



		RegV("gOS_m_bIsEnabled");


		RegV("gOS_m_aBool_VarNames");
		RegV("gOS_m_aBool_Values");


    };

    //SCR_JsonSaveContext 


    //*** code
    static string gOS_s_sPath = "$profile:vPad/Settings.json";

    // loads the current values from the file
    //  * if nothign saved, create save based on game values
    // void gOS_Data_LoadFromFile () {
    bool gOS_Data_LoadFromFile () {
        // Print("gOS_Data_Settings::gOS_Data_LoadFromFile");


        gOS_Data_Settings.HCA_MakeFolders();
        if (FileIO.FileExists(gOS_s_sPath)) {

            SCR_JsonLoadContext t_jsCtx_Load = new SCR_JsonLoadContext(false);
            t_jsCtx_Load.LoadFromFile(gOS_s_sPath);
            t_jsCtx_Load.ReadValue("gOS_m_aBool_VarNames", gOS_m_aBool_VarNames);
            t_jsCtx_Load.ReadValue("gOS_m_aBool_Values", gOS_m_aBool_Values);

            // LoadFromFile(gOS_s_sPath);
            return true;
        };
        return false;
    };

    void gOS_Data_SaveToFile () {
        // Print("gOS_Data_Settings::gOS_Data_SaveToFile");


        gOS_Data_Settings.HCA_MakeFolders();

		SCR_JsonSaveContext t_jsCtx_Save = new SCR_JsonSaveContext();
        t_jsCtx_Save.WriteValue("gOS_m_aBool_VarNames", gOS_m_aBool_VarNames);
        t_jsCtx_Save.WriteValue("gOS_m_aBool_Values", gOS_m_aBool_Values);
        t_jsCtx_Save.SaveToFile(gOS_s_sPath);

        // PackToFile(gOS_s_sPath);
    };

    // gOS_Data_Settings.HCA_MakeFolders()
    static void HCA_MakeFolders () {
        FileIO.MakeDirectory("$profile:vPad");
    };
};



[EntityEditorProps(description: "Server-Client connection for gOS(vPad)")]
class gOS_SCR_PlayerController_Comp_MainClass : ScriptComponentClass {};
class gOS_SCR_PlayerController_Comp_Main : ScriptComponent {
    

    
	protected void OnConnectedServer (int playerID) {
        // Print("gOS_SCR_PlayerController_Comp_Main::OnConnectedServer:" + playerID);
		// Rpc(Bool, f_iGameID_Player);

        gOS_SCR_GameModeComp_Main t_GMComp_Main = gOS_SCR_GameModeComp_Main.gOS_GetComp();
        if (!t_GMComp_Main) {
            return;
        };
        t_GMComp_Main.gOS_Data_OnConnect(this);
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void gOS_S2C_Data_Bool_Send (
        
        string f_sVarName
        , bool f_bValue
        , int f_iUpdateID
    ) {
        
        gOS_SCR_GameModeComp_Main t_GMComp_Main = gOS_SCR_GameModeComp_Main.gOS_GetComp();
        if (!t_GMComp_Main) {
            return;
        };

        t_GMComp_Main.gOS_S2B_Data_Bool_Send(
            f_sVarName
            , f_bValue
            , f_iUpdateID
        );

	};

    override void OnPostInit (IEntity owner) {
        gOS_OnPostInit(owner);
	};
    void gOS_OnPostInit (IEntity f_IEnt_Owner) {
		SetEventMask(f_IEnt_Owner, EntityEvent.INIT);
    };
	override void EOnInit (IEntity owner) {
        super.EOnInit(owner);

		SCR_BaseGameMode t_GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (!t_GameMode) {
            return;
        };
        t_GameMode.GetOnPlayerConnected().Insert(OnConnectedServer);

    };
	void ~gOS_SCR_PlayerController_Comp_Main () {
        gOS_OnDeconstructor();
    };
    void gOS_OnDeconstructor () {

		SCR_BaseGameMode t_GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (!t_GameMode) {
            return;
        };
        t_GameMode.GetOnPlayerConnected().Remove(OnConnectedServer);
    };
}
