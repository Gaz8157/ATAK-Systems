modded class SCR_ResourcePlayerControllerInventoryComponent {
	bool Bacon_GunBuilder_TryPerformResourceConsumption(notnull SCR_ResourceActor actor, float resourceValue, bool ignoreOnEmptyBehavior = false) {
		return TryPerformResourceConsumption(actor, resourceValue, ignoreOnEmptyBehavior);
	}
	bool Bacon_GunBuilder_TryPerformRefund(notnull SCR_ResourceActor actor, float resourceValue) {
		return TryPerformResourceGeneration(actor, resourceValue);
	}
}