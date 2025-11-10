sealed class GunBuilderUI_MultifunctionSlotUIComponent : SCR_ListBoxElementComponent {
	static string m_sIconImageset = "{FDD5423E69D007F8}UI/Textures/Icons/icons_wrapperUI-128.imageset";
	static string m_sIcon_Remove = "cancel";
	static string m_sIcon_EmptySlot = "careerCircleOutline";
	static string m_sIcon_Invalid = "cancelCircle";
	static string m_sIcon_Add = "addCircle";
	static string m_sIcon_Message = "comments";
	static string m_sIcon_SubArsenal = "repairCircle";
	static string m_sIcon_Back = "move";
	
	// widget names
	static string m_sWidgetName_BlockedPanel = "BlockedPanel";
	static string m_sWidgetName_ItemPreview = "SlotPreview";
	static string m_sWidgetName_ItemCount = "ItemCount";
	static string m_sWidgetName_HasStorage = "ImageHasStorage";
	static string m_sWidgetName_SlotText = "SlotText";
	static string m_sWidgetName_SlotImage = "SlotImage";
	// item hightlighting
	static string m_sWidgetName_ItemHighlightContainer = "ItemHighlightContainer";
	static string m_sWidgetName_ItemHighlightImageExtra = "ItemHighlightImageExtra";
	static string m_sWidgetName_ItemHighlightImage = "ItemHighlightImage";
	static string m_sWidgetName_ItemHighlightGlow = "ItemHighlightGlow";
	
	static const string SUPPLY_OVERLAY_WIDGET_NAME = "SupplyOverlay";
	static const string SUPPLY_OVERLAY_TEXT_WIDGET_NAME = "SupplyText";
	
	static const string RANK_OVERLAY_WIDGET_NAME = "RankOverlay";
	static const string RANK_OVERLAY_TEXT_WIDGET_NAME = "RankText";

	int m_iParentChildId;
	GunBuilderUI_SlotsUIComponent m_parentListbox;
	// Widget m_wRoot;
	
	// --------- static
	static void CreatePreviewForSlot(GunBuilderUI_MultifunctionSlotUIComponent comp, Bacon_GunBuilderUI_SlotInfo slotInfo) {
		// hack
		if (slotInfo.slotName.StartsWith("_")) {
			string slotName = slotInfo.slotName.Substring(1, slotInfo.slotName.Length()-1);
			// slotInfo.slotName = slotInfo.slotName.Substring(1, slotInfo.slotName.Length()-1);
			comp.SetTextWidget(string.Format("%1\n%2", slotName, slotInfo.itemName));
		} else {
			comp.SetTextWidget(string.Format("%1\n%2", slotInfo.slotName, slotInfo.itemName));
		}

		if (slotInfo.slot && slotInfo.slot.GetAttachedEntity())
			comp.SetPreviewFromEntity(slotInfo.slot.GetAttachedEntity());
		else
			comp.SetPreviewFromEntity(null);
		
		SetSupplyAndRankInformation(comp, slotInfo.prefab);

		if (slotInfo.hasStorage)
			comp.SetWidgetVisible(m_sWidgetName_HasStorage);
		
		if (!slotInfo.slotEnabled)
			comp.SetWidgetVisible(m_sWidgetName_BlockedPanel);
	}
	static void CreatePreviewForChoice(GunBuilderUI_MultifunctionSlotUIComponent comp, Bacon_GunBuilderUI_SlotChoice slotChoice) {
		if (CreateImageItem(comp, slotChoice))
			return;
		
		// hack
		if (slotChoice.slotName.StartsWith("_")) {
			string slotName = slotChoice.slotName.Substring(1, slotChoice.slotName.Length()-1);
			// slotInfo.slotName = slotInfo.slotName.Substring(1, slotInfo.slotName.Length()-1);
			comp.SetTextWidget(string.Format("%1\n%2", slotName, slotChoice.itemName));
		} else {
			comp.SetTextWidget(string.Format("%1\n%2", slotChoice.slotName, slotChoice.itemName));
		}
		
		// hack
		// if (slotChoice.slotName.StartsWith("_"))
		//	slotChoice.slotName = slotChoice.slotName.Substring(1, slotChoice.slotName.Length()-1);

		// comp.SetTextWidget(string.Format("%1\n%2", slotChoice.slotName, slotChoice.itemName));
		comp.SetPreviewFromResourceName(slotChoice.prefab);
		
		SetSupplyAndRankInformation(comp, slotChoice.prefab);
		
		if (!slotChoice.slotEnabled)
			comp.SetWidgetVisible(m_sWidgetName_BlockedPanel);
	}
	static void CreatePreviewForLoadout(GunBuilderUI_MultifunctionSlotUIComponent comp, Bacon_GunBuilderUI_SlotLoadout slotLoadout) {
//		if (CreateImageItem(comp, slotChoice))
//			return;

		comp.SetTextWidget(slotLoadout.metadataClothes, "ClothesText");
		
		if (slotLoadout.metadataWeapons.IsEmpty()) {
			comp.SetWidgetVisible("WeaponsText", false);
			comp.SetWidgetVisible("WeaponsImage", false);
			comp.SetImageWidget(m_sIcon_EmptySlot, widgetName: "ClothesImage");
			return;
		}
		
		SetSupplyCostInformation(comp, slotLoadout.supplyCost);
		
		// if (slotLoadout.requiredRank != SCR_ECharacterRank.INVALID)
		if (slotLoadout.requiredRank != SCR_ECharacterRank.INVALID && slotLoadout.requiredRank != SCR_ECharacterRank.RENEGADE)
			SetRequiredRankInformation(comp, slotLoadout.requiredRank);
		
		comp.SetTextWidget(slotLoadout.metadataWeapons, "WeaponsText");
		
		// {E76BD3521B9F3224}UI/BaconLoadoutEditor/GunBuilder_LoadoutItem.layout
		// comp.SetPreviewFromResourceName(slotChoice.prefab);
	}
		
	// supply icon handler
	static void SetSupplyCostInformation(GunBuilderUI_MultifunctionSlotUIComponent comp, float cost) {
		comp.SetTextWidget(cost.ToString(), SUPPLY_OVERLAY_TEXT_WIDGET_NAME);
		comp.SetWidgetVisible(SUPPLY_OVERLAY_WIDGET_NAME, true);
	}
	static void SetRequiredRankInformation(GunBuilderUI_MultifunctionSlotUIComponent comp, SCR_ECharacterRank rank) {
		comp.SetTextWidget(typename.EnumToString(SCR_ECharacterRank, rank), RANK_OVERLAY_TEXT_WIDGET_NAME);
		comp.SetWidgetVisible(RANK_OVERLAY_WIDGET_NAME, true);
	}
	static void SetSupplyAndRankInformation(GunBuilderUI_MultifunctionSlotUIComponent comp, ResourceName prefab) {
		Bacon_GunBuilderUI_Cache cache = Bacon_GunBuilderUI_Cache.GetInstance();
		if (!cache)
			return;
		
		float cost;
		SCR_ECharacterRank rank;
		
		cache.GetArsenalItemCostAndRank(prefab, cost, rank);

		if (cost > 0 && cache.AreSuppliesEnabled())
			SetSupplyCostInformation(comp, Math.Round(cost));
		
		// if (rank != SCR_ECharacterRank.INVALID && SCR_GameModeCampaign.GetInstance())
		if (rank != SCR_ECharacterRank.INVALID && rank != SCR_ECharacterRank.RENEGADE)
			SetRequiredRankInformation(comp, rank);
	}
	static void SetSupplyCostInformation(GunBuilderUI_MultifunctionSlotUIComponent comp, ResourceName prefab) {
		Bacon_GunBuilderUI_Cache cache = Bacon_GunBuilderUI_Cache.GetInstance();
		if (!cache)
			return;
		
		float cost = Math.Round(cache.GetArsenalItemCost(prefab));
		
		if (cost < 0.1)
			return;

		SetSupplyCostInformation(comp, cost);
	}
	static bool CreateImageItem(GunBuilderUI_MultifunctionSlotUIComponent comp, Bacon_GunBuilderUI_SlotChoice slotChoice) {
		switch (slotChoice.slotType) {
			case Bacon_GunBuilderUI_SlotType.REMOVE: {
				comp.SetTextWidget("Remove");
				comp.SetImageWidget(m_sIcon_Remove);
				return true;
			}
			case Bacon_GunBuilderUI_SlotType.ADD: {
				comp.SetTextWidget("Add");
				comp.SetImageWidget(m_sIcon_Add);
				return true;
			}
			case Bacon_GunBuilderUI_SlotType.MESSAGE: {
				comp.SetTextWidget(slotChoice.slotName);
				comp.SetImageWidget(m_sIcon_Message);
				return true;
			}
			case Bacon_GunBuilderUI_SlotType.ARSENAL_ITEM_BACK: {
				comp.SetTextWidget("Back");
				comp.SetImageWidget(m_sIcon_Back);
				return true;
			}
		}
		
		return false;
	}
	static void CreatePreviewForEditorOption(GunBuilderUI_MultifunctionSlotUIComponent comp, GunBuilderUI_EditorOptionData optionData) {
		comp.SetTextWidget(string.Format("%1\n%2", optionData.editorOptionLabel, optionData.optionLabel));
		comp.SetImageWidget(optionData.iconName, optionData.iconImageSet, color: optionData.iconColor);
	}
	static void CreatePreviewForInventoryItem(GunBuilderUI_MultifunctionSlotUIComponent comp, Bacon_GunBuilderUI_SlotInfoStorageItem slotInfo) {
		comp.SetTextWidget(string.Format("%1\n%2", slotInfo.slotName, slotInfo.itemName));
		comp.SetPreviewFromEntity(slotInfo.slot.GetAttachedEntity());

		SetSupplyAndRankInformation(comp, slotInfo.prefab);
		
		if (slotInfo.numItems > 1)
			comp.SetTextWidget(string.Format("%1x", slotInfo.numItems), m_sWidgetName_ItemCount);
	}
/*
	static void CreatePreviewForIdentity(GunBuilderUI_MultifunctionSlotUIComponent comp, Bacon_GunBuilderUI_SlotChoiceIdentity slotInfo) {
		comp.SetTextWidget(string.Format("%1\n%2", slotInfo.slotName, slotInfo.itemName));

		comp.CreateBullshitPreview(slotInfo.prefabBody);
	}
*/
	static void HighlightArsenalItem(GunBuilderUI_MultifunctionSlotUIComponent comp, string iconName = "weapons") {
		comp.SetImageWidget(iconName, m_sIconImageset, m_sWidgetName_ItemHighlightImage, false);
		comp.SetWidgetVisible(m_sWidgetName_ItemHighlightImage, true);
		comp.SetWidgetVisible(m_sWidgetName_ItemHighlightContainer, true);
		comp.SetWidgetVisible(m_sWidgetName_ItemHighlightGlow, true);
	}
	static void HighlightArsenalItemArsenal(GunBuilderUI_MultifunctionSlotUIComponent comp, string iconName = "weapons") {
		comp.SetImageWidget(m_sIcon_SubArsenal, m_sIconImageset, m_sWidgetName_ItemHighlightImageExtra, false);
		comp.SetWidgetVisible(m_sWidgetName_ItemHighlightImageExtra, true);
		comp.SetWidgetVisible(m_sWidgetName_ItemHighlightContainer, true);
	}
	// ---------
	
	override void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
	}
	
	int GetZOrder() {
		return m_wRoot.GetZOrder();
	}
	
	override void SetData(Managed data) {
		super.SetData(data);

		Bacon_GunBuilderUI_SlotItem slotInfo = Bacon_GunBuilderUI_SlotItem.Cast(data);
/*
		if (Bacon_GunBuilderUI_SlotChoiceIdentity.Cast(data)) {
			CreatePreviewForIdentity(this, Bacon_GunBuilderUI_SlotChoiceIdentity.Cast(data));
			return;
		}
*/
		
		if (Bacon_GunBuilderUI_SlotInfoStorageItem.Cast(data)) {
			CreatePreviewForInventoryItem(this, Bacon_GunBuilderUI_SlotInfoStorageItem.Cast(data));
			return;
		}
		
		if (GunBuilderUI_EditorOptionData.Cast(data)) {
			CreatePreviewForEditorOption(this, GunBuilderUI_EditorOptionData.Cast(data));
			return;
		}
		
		if (Bacon_GunBuilderUI_SlotInfo.Cast(data)) {
			CreatePreviewForSlot(this, Bacon_GunBuilderUI_SlotInfo.Cast(data));
			return;
		}
		
		if (Bacon_GunBuilderUI_SlotChoice.Cast(data)) {
			CreatePreviewForChoice(this, Bacon_GunBuilderUI_SlotChoice.Cast(data));
			return;
		}
		
		if (Bacon_GunBuilderUI_SlotLoadout.Cast(data)) {
			CreatePreviewForLoadout(this, Bacon_GunBuilderUI_SlotLoadout.Cast(data));
			return;
		}
	}
	
	void SetListBoxId(int id, GunBuilderUI_SlotsUIComponent parent) {
		m_iParentChildId = id;
		m_parentListbox = parent;
		
		m_bToggledOnlyThroughApi = true;
	}
	
	override bool OnFocus(Widget w, int x, int y) {
		bool state = super.OnFocus(w, x, y);
		
		if (!m_parentListbox)
			return state;

		m_parentListbox.FocusChanged(m_iParentChildId);
		
		return state;
	}
	
	void SetImageWidget(string image, string imageset = m_sIconImageset, string widgetName = m_sWidgetName_SlotImage, bool hidePreview = true, Color color = Color.White) {
		ImageWidget iw = ImageWidget.Cast(m_wRoot.FindAnyWidget(widgetName));
		if (!iw)
			return;
		
		iw.LoadImageFromSet(0, imageset, image);
		iw.SetColor(color);
		iw.SetVisible(true);
		
		if (hidePreview)
			SetWidgetVisible(m_sWidgetName_ItemPreview, false);
	}
	
	void SetTextWidget(string text, string widgetName = m_sWidgetName_SlotText)
	{		
		TextWidget tw = TextWidget.Cast(m_wRoot.FindAnyWidget(widgetName));
		if (!tw)
			return;
		
		tw.SetText(text);
		tw.SetVisible(true);
	}

	void SetWidgetVisible(string widgetName, bool visible = true) {
		Widget widget = m_wRoot.FindAnyWidget(widgetName);
		if (!widget)
			return;

		widget.SetVisible(visible);
	}
		
	void SetPreviewFromEntity(IEntity entity) {
		if (!entity) {
			SetImageWidget(m_sIcon_EmptySlot);
			return;
		}

		ItemPreviewManagerEntity previewManager = Bacon_GunBuilderUI_Helpers.GetPreviewManager();
		if (!previewManager)
			return;
		
		ItemPreviewWidget previewWidget = ItemPreviewWidget.Cast(m_wRoot.FindAnyWidget(m_sWidgetName_ItemPreview));
		if (!previewWidget)
			return;
		
		// clear first
		// previewManager.SetPreviewItem(previewWidget, null, null, true);
		SetWidgetVisible(m_sWidgetName_SlotImage, false);
		previewManager.SetPreviewItem(previewWidget, entity);
		previewWidget.SetVisible(true);
	}
	
/*
	static IEntity previewEntity;
	static BaseWorld world = null;
	static ref SharedItemRef garbage = null;	
	
	void CreateBullshitPreview(ResourceName prefab) {
		ItemPreviewWidget rtWidget = ItemPreviewWidget.Cast(m_wRoot.FindAnyWidget(m_sWidgetName_ItemPreview));

		SetWidgetVisible(m_sWidgetName_SlotImage, false);		
		if (!garbage) {
			garbage = BaseWorld.CreateWorld("Preview", "BullshitPreview");
			world = garbage.GetRef();
		}
		
		SCR_EntityHelper.DeleteEntityAndChildren(previewEntity);
		// SCR_EntityHelper.DeleteEntityAndChildren(bgEntity);
				// m_RTEntity.SetOrigin(m_vPrefabPos);
				// m_RTEntity.SetFixedLOD(0);
		
		// rtWidget.SetBlendMode(RenderTargetWidgetBlendMode.DISABLED);
		
		PrintFormat("Creating preview for prefab %1 in %2", prefab, rtWidget);
		Resource loaded = Resource.Load("{3E413771E1834D2F}Prefabs/Weapons/Rifles/M16/Rifle_M16A2.et");
		
		EntitySpawnParams params = new EntitySpawnParams;
		params.TransformMode = ETransformMode.WORLD;
		
		// bgEntity = GetGame().SpawnEntityPrefabLocal(Resource.Load("{E91350F79536248E}Prefabs/BaconLoadoutEditor/PreviewWorld/GunBuilderPreviewWorld.et"), world);
		
		previewEntity = GetGame().SpawnEntityPrefabLocal(loaded, world, params);
		
		// previewEntity.SetOrigin(vector.Zero);
		// previewEntity.SetAngles(vector.Up * 90);
		previewEntity.SetFixedLOD(0);

		// world.SetCamera(0, "0 0 -0.5", "0 0 0");
		world.SetCameraType(0, CameraType.PERSPECTIVE);
		world.SetCameraNearPlane(0, 0.01);
		world.SetCameraFarPlane(0, 500);
		world.SetCameraVerticalFOV(0, 90);
		
		vector mat[4];
		mat[3] = vector.Forward * -1;
		SCR_Math3D.LookAt(vector.Forward * -1, mat[3], vector.Up, mat);
		
		world.SetCameraEx(0, mat);
		
		rtWidget.SetWorld(world, 0);
		rtWidget.SetVisible(true);

		PrintFormat("%1", previewEntity);
	}
*/
	
	void SetPreviewFromResourceName(ResourceName res) {		
		ItemPreviewManagerEntity previewManager = Bacon_GunBuilderUI_Helpers.GetPreviewManager();
		if (!previewManager)
			return;
		
		ItemPreviewWidget previewWidget = ItemPreviewWidget.Cast(m_wRoot.FindAnyWidget(m_sWidgetName_ItemPreview));
		if (!previewWidget)
			return;

		previewManager.SetPreviewItemFromPrefab(previewWidget, res);
		previewWidget.SetVisible(true);
	}
	// overrides
	override void SetImage(ResourceName imageOrImageset, string iconName)
	{
		return;
	}
	
	override void SetText(string text)
	{
		return;
	}
	
	// interactions
	override bool OnClick(Widget w, int x, int y, int button)
	{
		#ifdef DEBUG_MODULAR_BUTTON
		_print("OnClick");
		#endif
		
		// Auto focus is focusable
		if (m_wRoot.IsFocusable())
		{
			auto workspace = GetGame().GetWorkspace();
			Widget currentFocus = workspace.GetFocusedWidget();
			if (currentFocus != m_wRoot)
				workspace.SetFocusedWidget(m_wRoot);
		}
		
		bool eventReturnValue = m_eEventReturnValue & EModularButtonEventHandler.CLICK;
		
		if (m_bIgnoreStandardInputs)
			return eventReturnValue;
		
		if (m_bCanBeToggled && !m_bToggledOnlyThroughApi)
			Internal_SetToggled(!m_bToggled);
		
		InvokeEffectsEvent(EModularButtonEventFlag.EVENT_CLICKED);
		
		EModularButtonState state = GetCurrentState();
		InvokeEffectsEvent(state);
		
		m_OnClicked.Invoke(this, button);
			
		return eventReturnValue;
	}
	
//	override bool OnFocus(Widget w, int x, int y)
//	{
//		if (!super.OnFocus(w,x,y))
//			return false;
//		
//		
//	}
}