// ATAK Device Widget - Widget handler for ATAK UI (optional, for backward compatibility)
class ATAK_DeviceWidget : ScriptedWidgetComponent
{
    protected Widget m_wRootWidget;
    protected ATAK_UIComponent m_ATAKUIComponent;
    
    // Widget references
    protected Widget m_wBtnToggle;
    protected Widget m_wMap;
    protected bool m_bMiniMode = false;
    protected float m_fZoom = 1.0;
    protected vector m_vPan = vector.Zero;
    protected bool m_bDragging = false;
    
    // Device data
    protected string m_sCurrentGrid = "31U 12345 67890";
    protected float m_fPlayerHeading = 271.0;
    
    //------------------------------------------------------------------------------------------------
    override protected void HandlerAttached(Widget w)
    {
        m_wRootWidget = w;
        Print("[ATAK_DeviceWidget] Widget handler attached");

        // Find widgets
        m_wBtnToggle = w.FindAnyWidget("BtnToggle");
        m_wMap = w.FindAnyWidget("MapDisplay");

        // Connect button events if needed
        ConnectButtonEvents();
    }
    
    //------------------------------------------------------------------------------------------------
    void SetATAKUIComponent(ATAK_UIComponent component)
    {
        m_ATAKUIComponent = component;
        Print("[ATAK_DeviceWidget] ATAK UI Component reference set");
    }
    
    //------------------------------------------------------------------------------------------------
    void ConnectButtonEvents()
    {
        // Connect to your existing ButtonActionComponent or create button handlers
        ConnectButtonAction("BtnZoomIn");
        ConnectButtonAction("BtnZoomOut");
        ConnectButtonAction("BtnCenter");
        ConnectButtonAction("BtnMenu");
        ConnectButtonAction("BtnCloseMenu");
        
        Print("[ATAK_Device] Button events connected");
    }
    
    //------------------------------------------------------------------------------------------------
    void ConnectButtonAction(string buttonName)
    {
        Widget button = m_wRootWidget.FindAnyWidget(buttonName);
        if (!button)
        {
            Print(string.Format("[ATAK_Device] Button %1 not found", buttonName));
            return;
        }
            
        // Use ButtonActionComponent if available
        ScriptInvoker onAction = ButtonActionComponent.GetOnAction(button);
        if (onAction)
        {
            // Connect to appropriate handler based on button name
            if (buttonName == "BtnZoomIn")
                onAction.Insert(this.OnZoomInButton);
            else if (buttonName == "BtnZoomOut")
                onAction.Insert(this.OnZoomOutButton);
            else if (buttonName == "BtnCenter")
                onAction.Insert(this.OnCenterViewButton);
            else if (buttonName == "BtnMenu")
                onAction.Insert(this.OnToggleMenuButton);
            else if (buttonName == "BtnCloseMenu")
                onAction.Insert(this.OnCloseMenuButton);
                
            Print(string.Format("[ATAK_Device] ButtonActionComponent connected for %1", buttonName));
        }
    }
    
    //------------------------------------------------------------------------------------------------
    void OnZoomInButton()
    {
        Print("[ATAK_DeviceWidget] Zoom In button clicked");
        if (m_ATAKUIComponent)
            m_ATAKUIComponent.ZoomIn();
    }
    
    void OnZoomOutButton()
    {
        Print("[ATAK_DeviceWidget] Zoom Out button clicked");
        if (m_ATAKUIComponent)
            m_ATAKUIComponent.ZoomOut();
    }
    
    void OnCenterViewButton()
    {
        Print("[ATAK_DeviceWidget] Center View button clicked");
        if (m_ATAKUIComponent)
            m_ATAKUIComponent.CenterView();
    }
    
    void OnToggleMenuButton()
    {
        Print("[ATAK_DeviceWidget] Toggle Menu button clicked");
        if (m_ATAKUIComponent)
            m_ATAKUIComponent.ToggleNavigationMenu();
    }
    
    void OnCloseMenuButton()
    {
        Print("[ATAK_DeviceWidget] Close Menu button clicked");
        if (m_ATAKUIComponent)
            m_ATAKUIComponent.CloseNavigationMenu();
    }
    
    //------------------------------------------------------------------------------------------------
    void UpdateUI()
    {
        // Update grid coordinates directly (no nested widget)
        if (m_wGridDisplay)
        {
            TextWidget gridText = TextWidget.Cast(m_wGridDisplay);
            if (gridText)
            {
                gridText.SetText(m_sCurrentGrid);
                Print(string.Format("[ATAK_Device] Updated grid display: %1", m_sCurrentGrid));
            }
        }
        
        // Update compass directly (no nested widget)
        if (m_wCompassDisplay)
        {
            TextWidget compassText = TextWidget.Cast(m_wCompassDisplay);
            if (compassText)
            {
                string headingText = string.Format("W %1°", m_fPlayerHeading);
                compassText.SetText(headingText);
                Print(string.Format("[ATAK_Device] Updated compass display: %1", headingText));
            }
        }
    }
    
    protected Widget m_wGridDisplay;
    protected Widget m_wCompassDisplay;

    bool OnMouseMove(Widget w, int x, int y, int dx, int dy)
    {
        if (!m_bDragging || !m_wMap)
            return false;

        m_vPan[0] = m_vPan[0] + dx;
        m_vPan[1] = m_vPan[1] + dy;
        ApplyTransform();
        return true;
    }

    bool OnKeyDown(Widget w, int x, int y, int key)
    {
        if (key == 33) // F key
        {
            m_bMiniMode = !m_bMiniMode;
            ApplyViewMode();
            return true;
        }

        if (key == 1 && m_ATAKUIComponent) // Escape key
        {
            m_ATAKUIComponent.HideATAK();
            return true;
        }

        return false;
    }

    override bool OnClick(Widget w, int x, int y, int button)
    {
        if (w != m_wBtnToggle)
            return false;

        m_bMiniMode = !m_bMiniMode;
        ApplyViewMode();
        return true;
    }

    //------------------------------------------------------------------------------------------------
    // Helper methods for map transformation and view modes
    protected void ApplyTransform()
    {
        if (!m_wMap)
            return;

        // For now, just handle zoom and pan differently
        // The actual transform manipulation would depend on the widget type
        Print(string.Format("[ATAK_Device] Transform applied - Zoom: %1, Pan: %2", m_fZoom, m_vPan.ToString()));
        m_wMap.Update();
    }

    protected void ApplyViewMode()
    {
        if (!m_wMap)
            return;

        // Handle view mode changes
        string mode = "full";
        if (m_bMiniMode)
            mode = "mini";
            
        Print(string.Format("[ATAK_Device] View mode changed to: %1", mode));
        m_wMap.Update();
    }
}
