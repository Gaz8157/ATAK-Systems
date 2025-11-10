sealed class GunBuilderUI_RankAndSupplyInfoComponent: ScriptedWidgetComponent {
	static const float UPDATE_INTERVAL_SECONDS = 2;
	
	private float m_fCooldown = 1;
	
	private IEntity m_ArsenalEntity;
	private IEntity m_CharacterEntity;
	private SCR_ResourceComponent m_ArsenalResourceComponent;
	
	// private SCR_CharacterRankComponent m_RankComponent;
	
	private Widget m_wRoot;
	
	private Widget m_wSupplyOverlay;
	private Widget m_wRankOverlay;
	
	private TextWidget m_wSupplyText;
	private TextWidget m_wRankText;
	
	override void HandlerAttached(Widget w) {
		m_wRoot = w;
		
		m_wSupplyOverlay = w.FindAnyWidget(GunBuilderUI_MultifunctionSlotUIComponent.SUPPLY_OVERLAY_WIDGET_NAME);
		m_wRankOverlay = w.FindAnyWidget(GunBuilderUI_MultifunctionSlotUIComponent.RANK_OVERLAY_WIDGET_NAME);
		m_wSupplyText = TextWidget.Cast(w.FindAnyWidget(GunBuilderUI_MultifunctionSlotUIComponent.SUPPLY_OVERLAY_TEXT_WIDGET_NAME));
		m_wRankText = TextWidget.Cast(w.FindAnyWidget(GunBuilderUI_MultifunctionSlotUIComponent.RANK_OVERLAY_TEXT_WIDGET_NAME));
		
		
	}
	
	void Init(IEntity arsenal, IEntity playerCharacter) {
		m_ArsenalEntity = arsenal;
		m_CharacterEntity = playerCharacter;
		
		m_ArsenalResourceComponent	= SCR_ResourceComponent.FindResourceComponent(m_ArsenalEntity);
		if (!m_ArsenalResourceComponent)
			return;
	}
	
	private void RefreshSupply() {
		if (!SCR_ResourceSystemHelper.IsGlobalResourceTypeEnabled()) {
			return m_wSupplyOverlay.SetVisible(false);
		}
		
		float supply;
		if (!SCR_ResourceSystemHelper.GetAvailableResources(m_ArsenalResourceComponent, supply)) {
			return m_wSupplyOverlay.SetVisible(false);
		}
		
		m_wSupplyText.SetText(SCR_ResourceSystemHelper.SuppliesToString(supply));
		m_wSupplyOverlay.SetVisible(true);
	}
	
	private void RefreshRank() {
		if (!SCR_GameModeCampaign.GetInstance() || !m_CharacterEntity) {
			return m_wRankOverlay.SetVisible(false);
		}
		
		SCR_ECharacterRank rank = SCR_CharacterRankComponent.GetCharacterRank(m_CharacterEntity);
		if (rank == SCR_ECharacterRank.INVALID) {
			return m_wRankOverlay.SetVisible(false);
		}
		
		m_wRankText.SetText(typename.EnumToString(SCR_ECharacterRank, rank));
		m_wRankOverlay.SetVisible(true);
	}
	
	void Update(float tDelta) {
		m_fCooldown -= tDelta;
		if (m_fCooldown > 0)
			return;

		m_fCooldown = UPDATE_INTERVAL_SECONDS;
		
		RefreshRank();
		RefreshSupply();
	}
}