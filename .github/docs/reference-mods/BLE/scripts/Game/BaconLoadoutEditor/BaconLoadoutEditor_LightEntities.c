sealed class BaconLoadoutEditor_LightEntitiesClass: GenericEntityClass {}
sealed class BaconLoadoutEditor_LightEntities: GenericEntity {
	void BaconLoadoutEditor_LightEntities(IEntitySource src, IEntity parent) {
		GunBuilderUI_PreviewUIComponent.m_LightEntities = this;
	}
}