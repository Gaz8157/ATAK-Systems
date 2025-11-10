//---------------------------------------------------------------------------
// ATAK Map Component - Renders topography and satellite imagery
//---------------------------------------------------------------------------
[ComponentEditorProps(category: "ATAK", description: "ATAK Map Rendering Component")]
class ATAK_MapComponentClass : ScriptComponentClass
{
}

class ATAK_MapComponent : ScriptComponent
{
	protected World m_World;
	protected RenderTargetWidget m_MapWidget;
	protected IEntity m_PlayerEntity;
	
	// Map rendering state
	protected bool m_bInitialized = false;
	protected vector m_MapCenter;
	protected float m_fMapZoom = 1000.0;
	
	//---------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		m_World = GetGame().GetWorld();
		
		// Start periodic map updates every 0.2 seconds (5 times per second)
		GetGame().GetCallqueue().CallLater(UpdateMapDisplay, 200, true);
		
		m_bInitialized = true;
	}
	
	//---------------------------------------------------------------------------
	// Set the map widget to render into
	void SetMapWidget(RenderTargetWidget widget)
	{
		m_MapWidget = widget;
	}
	
	//---------------------------------------------------------------------------
	// Update map display with current world data
	protected void UpdateMapDisplay()
	{
		if (!m_World || !m_MapWidget) return;
		
		// Get player position for map center
		IEntity player = SCR_PlayerController.GetLocalControlledEntity();
		if (player)
		{
			m_MapCenter = player.GetOrigin();
			m_PlayerEntity = player;
		}
		
		// TODO: Render map using World data
		// This would require getting terrain data, texture data, etc.
		// For now, the RenderTargetWidget shows as a colored background
	}
	
	//---------------------------------------------------------------------------
	// Get current map center (for other components)
	vector GetMapCenter()
	{
		return m_MapCenter;
	}
	
	//---------------------------------------------------------------------------
	// Get current zoom level
	float GetMapZoom()
	{
		return m_fMapZoom;
	}
}

