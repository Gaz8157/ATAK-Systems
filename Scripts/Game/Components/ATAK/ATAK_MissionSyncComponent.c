// ATAK Mission Sync Component
// Handles synchronization of mission briefings and tactical data across multiplayer
// Uses ATAK_NetworkRetryComponent for automatic retry of failed RPC calls

//------------------------------------------------------------------------------------------------
//! Mission data structure
class ATAK_MissionData
{
	string m_sMissionId;           // Unique mission identifier
	string m_sMissionTitle;        // Mission title
	string m_sMissionBriefing;     // Mission briefing text
	string m_sObjectives;          // Mission objectives
	float m_fTimestamp;            // When mission was created/updated
	
	void ATAK_MissionData(string missionId, string title, string briefing, string objectives)
	{
		m_sMissionId = missionId;
		m_sMissionTitle = title;
		m_sMissionBriefing = briefing;
		m_sObjectives = objectives;
		m_fTimestamp = System.GetTickCount() / 1000.0;
	}
}

//------------------------------------------------------------------------------------------------
//! Component class
class ATAK_MissionSyncComponentClass : ScriptGameComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Mission sync component with retry logic
class ATAK_MissionSyncComponent : ScriptGameComponent
{
	protected ATAK_NetworkRetryComponent m_RetryComponent;
	protected ref ATAK_MissionData m_CurrentMission;
	protected ref array<string> m_aPendingMissionUpdates;
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		// Get retry component
		m_RetryComponent = ATAK_NetworkRetryComponent.Cast(owner.FindComponent(ATAK_NetworkRetryComponent));
		if (!m_RetryComponent)
		{
			Print("[ATAK_MissionSync] WARNING: No ATAK_NetworkRetryComponent found - retry disabled", LogLevel.WARNING);
		}
		
		// Initialize pending updates array
		m_aPendingMissionUpdates = new array<string>();
		
		// Start retry check loop (check every second)
		GetGame().GetCallqueue().CallLater(CheckRetryQueue, 1000, true);
		
		Print("[ATAK_MissionSync] Component initialized");
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnDelete(IEntity owner)
	{
		// Stop retry checking
		GetGame().GetCallqueue().Remove(CheckRetryQueue);
		
		super.OnDelete(owner);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Set mission briefing (server only)
	//! @param missionId Unique mission identifier
	//! @param title Mission title
	//! @param briefing Mission briefing text
	//! @param objectives Mission objectives
	void SetMissionBriefing(string missionId, string title, string briefing, string objectives)
	{
		// Verify this is server
		if (!Replication.IsServer())
		{
			Print("[ATAK_MissionSync] ERROR: SetMissionBriefing can only be called on server!", LogLevel.ERROR);
			return;
		}
		
		// Create mission data
		m_CurrentMission = new ATAK_MissionData(missionId, title, briefing, objectives);
		
		// Broadcast to all clients with retry
		BroadcastMissionUpdate(missionId, title, briefing, objectives);
		
		PrintFormat("[ATAK_MissionSync] Server: Mission '%1' set and broadcasting", title);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Broadcast mission update with retry logic
	//! @param missionId Mission identifier
	//! @param title Mission title
	//! @param briefing Mission briefing
	//! @param objectives Mission objectives
	protected void BroadcastMissionUpdate(string missionId, string title, string briefing, string objectives)
	{
		// Register request with retry component
		string requestId = string.Format("mission_update_%1_%2", missionId, Math.RandomInt(0, 9999));
		
		if (m_RetryComponent)
		{
			ATAK_NetworkRequest request = m_RetryComponent.RegisterRequest(requestId);
			if (request)
			{
				// Track this request for retry
				m_aPendingMissionUpdates.Insert(requestId);
			}
		}
		
		// Send RPC to all clients
		// Note: In a real implementation, this would be an actual RPC call
		// For now, we'll simulate the call and retry logic
		SendMissionUpdateRPC(requestId, missionId, title, briefing, objectives);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Simulate sending mission update RPC
	//! @param requestId Request identifier for tracking
	//! @param missionId Mission identifier
	//! @param title Mission title
	//! @param briefing Mission briefing
	//! @param objectives Mission objectives
	protected void SendMissionUpdateRPC(string requestId, string missionId, string title, string briefing, string objectives)
	{
		// In a real implementation, this would call:
		// Rpc(RPC_UpdateMissionBriefing, missionId, title, briefing, objectives);
		
		// For demonstration, we'll simulate success/failure
		PrintFormat("[ATAK_MissionSync] Sending RPC for mission '%1' (requestId: %2)", title, requestId);
		
		// Simulate random failure (20% chance) for testing retry logic
		float randomValue = Math.RandomFloat01();
		if (randomValue < 0.2)
		{
			// Simulate failure
			PrintFormat("[ATAK_MissionSync] ✗ RPC failed for request '%1' - will retry", requestId);
			
			if (m_RetryComponent)
			{
				bool willRetry = m_RetryComponent.FailRequest(requestId);
				if (!willRetry)
				{
					// Request permanently failed - remove from pending
					int idx = m_aPendingMissionUpdates.Find(requestId);
					if (idx != -1)
						m_aPendingMissionUpdates.Remove(idx);
				}
			}
		}
		else
		{
			// Simulate success
			PrintFormat("[ATAK_MissionSync] ✓ RPC succeeded for request '%1'", requestId);
			
			if (m_RetryComponent)
			{
				m_RetryComponent.CompleteRequest(requestId);
				
				// Remove from pending
				int idx = m_aPendingMissionUpdates.Find(requestId);
				if (idx != -1)
					m_aPendingMissionUpdates.Remove(idx);
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Check retry queue and resend failed requests
	protected void CheckRetryQueue()
	{
		if (!m_RetryComponent || m_aPendingMissionUpdates.Count() == 0)
			return;
		
		// Get requests ready for retry
		array<string> retryReady = new array<string>();
		m_RetryComponent.GetRetryReadyRequests(retryReady);
		
		if (retryReady.Count() == 0)
			return;
		
		PrintFormat("[ATAK_MissionSync] Processing %1 request(s) ready for retry", retryReady.Count());
		
		// Retry each request
		for (int i = 0; i < retryReady.Count(); i++)
		{
			string requestId = retryReady.Get(i);
			
			// Check if this is one of our mission update requests
			if (m_aPendingMissionUpdates.Find(requestId) == -1)
				continue;
			
			// Retry the mission update
			if (m_CurrentMission)
			{
				PrintFormat("[ATAK_MissionSync] ⟳ Retrying mission update for request '%1'", requestId);
				SendMissionUpdateRPC(requestId, m_CurrentMission.m_sMissionId, 
					m_CurrentMission.m_sMissionTitle, m_CurrentMission.m_sMissionBriefing, 
					m_CurrentMission.m_sObjectives);
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! RPC handler for receiving mission updates (client-side)
	//! Note: This would be marked with [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)] in real implementation
	void RPC_UpdateMissionBriefing(string missionId, string title, string briefing, string objectives)
	{
		// Update local mission data
		m_CurrentMission = new ATAK_MissionData(missionId, title, briefing, objectives);
		
		PrintFormat("[ATAK_MissionSync] Client: Received mission update - '%1'", title);
		
		// Trigger UI update if needed
		OnMissionUpdated();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Callback when mission is updated
	protected void OnMissionUpdated()
	{
		// This would trigger UI updates in the ATAK device menu
		// For example, updating the Missions tab with new briefing
		Print("[ATAK_MissionSync] Mission data updated - UI should refresh");
	}
	
	//------------------------------------------------------------------------------------------------
	//! Get current mission data
	//! @return Current mission data or null if none
	ATAK_MissionData GetCurrentMission()
	{
		return m_CurrentMission;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Get count of pending mission updates
	//! @return Number of pending updates
	int GetPendingUpdateCount()
	{
		return m_aPendingMissionUpdates.Count();
	}
}
