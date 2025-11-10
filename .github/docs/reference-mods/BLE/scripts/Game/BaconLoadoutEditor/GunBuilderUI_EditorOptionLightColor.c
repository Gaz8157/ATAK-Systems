sealed class GunBuilderUI_EditorOptionLightColorComponentClass: GunBuilderUI_EditorOptionComponentClass {}
class GunBuilderUI_EditorOptionLightColorComponent: GunBuilderUI_EditorOptionComponent {
	override void FillOptions() {
		m_aOptionChoices.Set("1", new GunBuilderUI_EditorOptionData(m_sOptionName, icon: "careerCircleSelected", label: "Neutral", value: "1", color: new Color(1.000000, 1.000000, 1.000000, 1.000000)));
		m_aOptionChoices.Set("2", new GunBuilderUI_EditorOptionData(m_sOptionName, icon: "careerCircleSelected", label: "Light Blue", value: "2", color: new Color(0.656992, 0.892989, 1.000000, 1.000000)));
		m_aOptionChoices.Set("3", new GunBuilderUI_EditorOptionData(m_sOptionName, icon: "careerCircleSelected", label: "Orange", value: "3", color: new Color(0.923110, 0.539498, 0.270329, 1.000000)));
		m_aOptionChoices.Set("4", new GunBuilderUI_EditorOptionData(m_sOptionName, icon: "careerCircleSelected", label: "Light Purple", value: "4", color: new Color(0.923964, 0.656992, 1.000000, 1.000000)));
	}
	
	override void ApplyOption(GunBuilderUI_EditorOptionData data) {
		LightEntity light = LightEntity.Cast(m_owner);
		if (!light) {
			Print("GunBuilderUI_EditorOptionLightColorComponent | No light entity!", LogLevel.ERROR);
			return;
		}
		
		light.SetColor(data.iconColor, 8.0);
	}
}

// 