// ATAK Mission Display Integration Example
// Demonstrates how to integrate ATAK_MissionSyncComponent with ATAK device UI
// This file shows the pattern but doesn't modify existing components

//------------------------------------------------------------------------------------------------
// Example: Adding Mission Display to ATAK_DeviceDisplay
//------------------------------------------------------------------------------------------------

/*
To integrate mission sync with the ATAK device display, add the following to your entity:

1. Add components to entity prefab (DefaultPlayerController.et):
   - ATAK_NetworkRetryComponent
   - ATAK_MissionSyncComponent

2. In ATAK_DeviceDisplay or ATAK_DeviceMenu, add mission display widgets:
*/

class ATAK_MissionDisplayExample
{
	// Widget references for Missions tab
	protected TextWidget m_MissionTitleText;
	protected TextWidget m_MissionBriefingText;
	protected TextWidget m_MissionObjectivesText;
	protected TextWidget m_MissionStatusText;
	
	// Component reference
	protected ATAK_MissionSyncComponent m_MissionSyncComponent;
	
	//------------------------------------------------------------------------------------------------
	// Example: Initialize mission display (call from OnMenuOpen or DisplayStartDraw)
	void InitializeMissionDisplay(Widget rootWidget, IEntity owner)
	{
		// Find mission display widgets
		m_MissionTitleText = TextWidget.Cast(rootWidget.FindAnyWidget("MissionTitle"));
		m_MissionBriefingText = TextWidget.Cast(rootWidget.FindAnyWidget("MissionBriefing"));
		m_MissionObjectivesText = TextWidget.Cast(rootWidget.FindAnyWidget("MissionObjectives"));
		m_MissionStatusText = TextWidget.Cast(rootWidget.FindAnyWidget("MissionStatus"));
		
		// Get mission sync component
		m_MissionSyncComponent = ATAK_MissionSyncComponent.Cast(
			owner.FindComponent(ATAK_MissionSyncComponent)
		);
		
		if (!m_MissionSyncComponent)
		{
			Print("[ATAK_MissionDisplay] WARNING: No mission sync component found", LogLevel.WARNING);
			return;
		}
		
		// Update display with current mission
		UpdateMissionDisplay();
		
		Print("[ATAK_MissionDisplay] Mission display initialized");
	}
	
	//------------------------------------------------------------------------------------------------
	// Example: Update mission display (call from OnMenuUpdate or DisplayUpdate)
	void UpdateMissionDisplay()
	{
		if (!m_MissionSyncComponent)
			return;
		
		// Get current mission data
		ATAK_MissionData mission = m_MissionSyncComponent.GetCurrentMission();
		
		if (!mission)
		{
			// No mission active
			if (m_MissionTitleText)
				m_MissionTitleText.SetText("NO ACTIVE MISSION");
			
			if (m_MissionBriefingText)
				m_MissionBriefingText.SetText("Awaiting mission briefing from command...");
			
			if (m_MissionObjectivesText)
				m_MissionObjectivesText.SetText("");
			
			return;
		}
		
		// Update widgets with mission data
		if (m_MissionTitleText)
			m_MissionTitleText.SetText(mission.m_sMissionTitle);
		
		if (m_MissionBriefingText)
			m_MissionBriefingText.SetText(mission.m_sMissionBriefing);
		
		if (m_MissionObjectivesText)
		{
			string objectivesText = string.Format("OBJECTIVES:\n%1", mission.m_sObjectives);
			m_MissionObjectivesText.SetText(objectivesText);
		}
		
		// Show sync status
		if (m_MissionStatusText)
		{
			int pendingUpdates = m_MissionSyncComponent.GetPendingUpdateCount();
			
			if (pendingUpdates > 0)
			{
				m_MissionStatusText.SetText(string.Format("⟳ Syncing (%1 pending)", pendingUpdates));
				m_MissionStatusText.SetColor(Color.FromInt(Color.YELLOW));
			}
			else
			{
				m_MissionStatusText.SetText("✓ Synced");
				m_MissionStatusText.SetColor(Color.FromInt(Color.GREEN));
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	// Example: Server-side command to set mission (for testing)
	// Call this from a command or admin interface
	void ServerSetMissionExample()
	{
		// Only works on server
		if (!Replication.IsServer())
		{
			Print("[ATAK_MissionDisplay] ERROR: Can only set missions on server!", LogLevel.ERROR);
			return;
		}
		
		if (!m_MissionSyncComponent)
		{
			Print("[ATAK_MissionDisplay] ERROR: No mission sync component!", LogLevel.ERROR);
			return;
		}
		
		// Set example mission
		string missionId = string.Format("mission_%1", System.GetTickCount());
		string title = "OPERATION ARCTIC STORM";
		string briefing = "Enemy forces have established a forward operating base in the northern sector. " +
		                  "Your team is tasked with reconnaissance and disruption of enemy operations.";
		string objectives = "1. Locate enemy FOB\n" +
		                   "2. Identify defensive positions\n" +
		                   "3. Eliminate high-value targets if opportunity presents\n" +
		                   "4. Extract via designated LZ";
		
		m_MissionSyncComponent.SetMissionBriefing(missionId, title, briefing, objectives);
		
		Print("[ATAK_MissionDisplay] Mission briefing set and broadcasting to all clients");
	}
}

//------------------------------------------------------------------------------------------------
// Example: Layout widgets for Missions tab (add to ATAK_LayoutMain.layout or separate layout)
//------------------------------------------------------------------------------------------------

/*
Add these widgets to your layout file for the Missions tab:

TextWidget MissionTitle {
 position 20 40
 size 440 30
 font "RobotoCondensed"
 text "MISSION TITLE"
 "text size" 24
 "text halign" center
 "text color" 0.2 1.0 0.2 1.0
}

TextWidget MissionBriefing {
 position 20 80
 size 440 200
 font "RobotoCondensed"
 text "Mission briefing will appear here..."
 "text size" 14
 "text valign" top
 "wrap text" 1
 "text color" 0.9 0.9 0.9 1.0
}

TextWidget MissionObjectives {
 position 20 290
 size 440 150
 font "RobotoCondensed"
 text ""
 "text size" 14
 "text valign" top
 "wrap text" 1
 "text color" 0.9 0.9 0.9 1.0
}

TextWidget MissionStatus {
 position 20 450
 size 440 20
 font "RobotoCondensed"
 text "✓ Synced"
 "text size" 12
 "text halign" center
 "text color" 0.2 1.0 0.2 1.0
}
*/

//------------------------------------------------------------------------------------------------
// Example: Console command for testing (add to ATAK_DiagMenuCommands.c)
//------------------------------------------------------------------------------------------------

/*
Add this method to ATAK_DiagMenuCommands to test mission sync from console:

[ScriptInvokerMethod(ScriptInvokerOrder.DEFAULT, true)]
void TestMissionSync()
{
	// Get player controller
	SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
	if (!playerController)
	{
		Print("ERROR: No player controller found!", LogLevel.ERROR);
		return;
	}
	
	// Get mission sync component
	ATAK_MissionSyncComponent missionSync = ATAK_MissionSyncComponent.Cast(
		playerController.FindComponent(ATAK_MissionSyncComponent)
	);
	
	if (!missionSync)
	{
		Print("ERROR: No mission sync component found!", LogLevel.ERROR);
		return;
	}
	
	// Set test mission (server only)
	if (Replication.IsServer())
	{
		string missionId = string.Format("test_mission_%1", System.GetTickCount());
		missionSync.SetMissionBriefing(
			missionId,
			"TEST MISSION: NETWORK RETRY",
			"This is a test mission to verify network retry functionality. The mission sync component will attempt to broadcast this briefing to all connected clients with automatic retry on failure.",
			"1. Verify mission appears on all clients\n2. Check retry logs for failed attempts\n3. Confirm exponential backoff timing"
		);
		
		Print("✓ Test mission set and broadcasting with retry");
	}
	else
	{
		Print("ERROR: Must be run on server!", LogLevel.ERROR);
	}
}
*/

//------------------------------------------------------------------------------------------------
// Integration Checklist
//------------------------------------------------------------------------------------------------

/*
To fully integrate mission sync with retry into ATAK Systems:

1. ✓ Add ATAK_NetworkRetryComponent to player controller prefab
2. ✓ Add ATAK_MissionSyncComponent to player controller prefab
3. □ Add mission display widgets to ATAK_LayoutMain.layout (Missions tab)
4. □ Integrate mission display code into ATAK_DeviceDisplay or ATAK_DeviceMenu
5. □ Add console commands for testing mission sync
6. □ Test in multiplayer with simulated network failures
7. □ Document mission sync in user guide

Components are ready to use - integration with UI is the next step!
*/
