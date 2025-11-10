modded class SCR_ArsenalManagerComponent {
	// Components to inspect inside saved loadouts for supply/rank computations
	static const ref array<string> ARSENALLOADOUT_COMPONENTS_TO_CHECK = {
		"SCR_EditableCharacterComponent",
		"SCR_InventoryStorageManagerComponent"
	};
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		if (Replication.IsServer())
			GetGame().GetCallqueue().CallLater(GunBuilderUI_CacheAllArsenalItems, 33, false);
	}
	
	void GunBuilderUI_CacheAllArsenalItems() {
		PrintFormat("SCR_ArsenalManagerComponent.GunBuilderUI_CacheAllArsenalItems | Initializing serverside cache...", level: LogLevel.NORMAL);
		
		SCR_EntityCatalogManagerComponent catalogManager = SCR_EntityCatalogManagerComponent.GetInstance();
		if (!catalogManager) {
			PrintFormat("SCR_ArsenalManagerComponent.GunBuilderUI_CacheAllArsenalItems | No Entity Catalog Manager!", level: LogLevel.ERROR);
			return;
		}
		
		FactionManager factionManager = GetGame().GetFactionManager();
		if (!factionManager) {
			PrintFormat("SCR_ArsenalManagerComponent.GunBuilderUI_CacheAllArsenalItems | No Faction Manager!", level: LogLevel.ERROR);
			return;
		}
		
		array<Faction> factions = {};
		factionManager.GetFactionsList(factions);
		
		array<SCR_ArsenalItem> temp = {};
		int total = 0;
		
		foreach (Faction faction : factions) {
			SCR_Faction scrFaction = SCR_Faction.Cast(faction);
			if (!scrFaction){
				PrintFormat("SCR_ArsenalManagerComponent.GunBuilderUI_CacheAllArsenalItems | Faction %1 is not of SCR_Faction type", faction, level: LogLevel.WARNING);
				continue;
			}

			// temp = catalogManager.GetArsenalItems(m_eSupportedArsenalItemTypes, m_eSupportedArsenalItemModes, SCR_ArsenalManagerComponent.GetArsenalGameModeType_Static(), scrFaction, requiresDisplayType);
			catalogManager.GetFactionArsenalItems(temp, scrFaction);
			
			foreach (SCR_ArsenalItem arsenalItem : temp) {
				total += 1;
				Bacon_GunBuilderUI_Helpers.FillSupplyAndRankCache(arsenalItem);
			}
			
			temp.Clear();
		}
		
		PrintFormat("SCR_ArsenalManagerComponent.GunBuilderUI_CacheAllArsenalItems | Cached %1 items", total, level: LogLevel.NORMAL);
	}
	
	bool GunBuilderUI_CanSaveLoadout(int playerId, GameEntity characterEntity, FactionAffiliationComponent playerFactionAffiliation, SCR_ArsenalComponent arsenalComponent, bool sendNotificationOnFailed) {
		return CanSaveLoadout(playerId, characterEntity, playerFactionAffiliation, arsenalComponent, sendNotificationOnFailed);
	}
	
	bool GunBuilderUI_GetLoadoutRespawnCost(string loadout, SCR_Faction faction, out float cost) {
		if (!faction)
			return false;
		
		SCR_ArsenalPlayerLoadout playerLoadout = new SCR_ArsenalPlayerLoadout;
		playerLoadout.loadout = loadout;
		playerLoadout.suppliesCost = 0.0;
		
		SCR_JsonLoadContext context = new SCR_JsonLoadContext(false);
		if (!context.ImportFromString(playerLoadout.loadout))
			return false;
		
		GunBuilderUI_ComputeEntity(context, faction, playerLoadout, SCR_EArsenalSupplyCostType.RESPAWN_COST);

		cost = playerLoadout.suppliesCost;
		return true;
	}
	
	protected void GunBuilderUI_ComputeEntity(notnull SCR_JsonLoadContext context, notnull SCR_Faction faction, notnull SCR_ArsenalPlayerLoadout playerLoadout, SCR_EArsenalSupplyCostType arsenalSupplyType)
	{
		if (!context.StartObject("Native"))
			return;
		
		if (!context.StartObject("components"))
			return;
		
		foreach (string componentName: ARSENALLOADOUT_COMPONENTS_TO_CHECK)
		{
			ComputeStorage(context, faction, playerLoadout, componentName, arsenalSupplyType);
		}
		
		if (!context.EndObject())
			return;
		
		if (!context.EndObject())
			return;
	}

// 1.6 compatibility: if base class no longer exposes ComputeStorage, provide a no-op stub
protected void ComputeStorage(notnull SCR_JsonLoadContext context, notnull SCR_Faction faction, notnull SCR_ArsenalPlayerLoadout playerLoadout, string componentName, SCR_EArsenalSupplyCostType arsenalSupplyType)
{
    // Keep parser alignment; specific cost computation is handled by the base in older versions
    if (context.StartObject(componentName))
    {
        context.EndObject();
    }
}
}