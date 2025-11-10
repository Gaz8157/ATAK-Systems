[ComponentEditorProps(category: "ATAK", description: "Advanced Tactical Awareness Kit - UI Component")]
class ATAK_UIComponentClass : ScriptComponentClass
{
}

class ATAK_UIComponent : ScriptComponent
{
    [Attribute("", UIWidgets.ResourceNamePicker, "ATAK Main Layout", "layout")]
    ResourceName m_ATakLayoutMain;
    
    [Attribute("", UIWidgets.ResourceNamePicker, "ATAK Systems Layout", "layout")]
    ResourceName m_ATakLayoutSystems;
    
    protected WorkspaceWidget m_WS;
    protected Widget          m_Root;
    protected Widget          m_SystemsRoot;
    protected ref ATAK_DeviceWidget m_DeviceWidget;
    protected ref ATAK_TacticalOverlayWidgetHandler m_WidgetHandler;
    protected FrameWidget     m_NavigationMenu;
    protected bool m_bATAKVisible = false;
    protected bool m_bSystemsVisible = false;

    override void OnPostInit(IEntity owner)
    {
        super.OnPostInit(owner);
        m_WS = GetGame().GetWorkspace();
        Print("[ATAK_UI] UI Component initialized");
    }
    
    //------------------------------------------------------------------------------------------------
    // Lazy-create the layout when first needed
    protected void EnsureLayout()
    {
        Print("[ATAK_UI] EnsureLayout called");
        if (m_Root)
        {
            Print("[ATAK_UI] Layout already exists");
            return;
        }

        if (!m_WS) 
        {
            m_WS = GetGame().GetWorkspace();
            if (!m_WS) 
            { 
                Print("[ATAK_UI] No Workspace!", LogLevel.ERROR); 
                return; 
            }
        }

        if (!m_ATakLayoutMain || m_ATakLayoutMain == "{0000000000000000}") 
        {
            Print("[ATAK_UI] Missing ATAK Main Layout ResourceName", LogLevel.ERROR);
            return;
        }

        PrintFormat("[ATAK_UI] Creating layout from: %1", m_ATakLayoutMain);
        m_Root = m_WS.CreateWidgets(m_ATakLayoutMain);
        if (!m_Root) 
        { 
            Print("[ATAK_UI] CreateWidgets failed", LogLevel.ERROR); 
            return; 
        }

        Print("[ATAK_UI] Layout created successfully");
        
        // Verify device background exists and check its properties
        ImageWidget deviceBg = ImageWidget.Cast(m_Root.FindAnyWidget("DeviceBackground"));
        if (deviceBg)
        {
            Print("[ATAK_UI] DeviceBackground widget found in layout");
            
            // Check widget flags
            int flags = deviceBg.GetFlags();
            PrintFormat("[ATAK_UI] DeviceBackground flags: %1 (VISIBLE=%2)", flags, (flags & WidgetFlags.VISIBLE) != 0);
            
            // Force visibility
            deviceBg.SetFlags(deviceBg.GetFlags() | WidgetFlags.VISIBLE);
            
            Print("[ATAK_UI] DeviceBackground widget is visible and ready");
        }
        else
        {
            Print("[ATAK_UI] Warning: DeviceBackground widget not found during layout creation!", LogLevel.WARNING);
        }

        // Load systems layout if specified
        EnsureSystemsLayout();

        // Create and initialize the tactical overlay widget handler programmatically
        // Get the rootFrame widget (parent of OverlayRoot)
        Widget rootFrame = m_Root.GetParent();
        if (rootFrame)
        {
            m_WidgetHandler = new ATAK_TacticalOverlayWidgetHandler();
            if (m_WidgetHandler)
            {
                // Manually initialize the handler with the rootFrame
                if (m_WidgetHandler.Initialize(rootFrame))
                {
                    m_WidgetHandler.SetUIComponent(this);
                    Print("[ATAK_UI] ATAK_TacticalOverlayWidgetHandler created and initialized successfully");
                }
                else
                {
                    Print("[ATAK_UI] Warning: Failed to initialize ATAK_TacticalOverlayWidgetHandler", LogLevel.WARNING);
                    m_WidgetHandler = null;
                }
            }
        }
        else
        {
            Print("[ATAK_UI] Warning: rootFrame not found", LogLevel.WARNING);
        }
        
        // Keep backward compatibility with old device widget
        m_DeviceWidget = ATAK_DeviceWidget.Cast(m_Root.FindHandler(ATAK_DeviceWidget));
        if (m_DeviceWidget)
        {
            m_DeviceWidget.SetATAKUIComponent(this);
            Print("[ATAK_UI] Found ATAK_DeviceWidget component attached to layout");
        }
        
        // Find and initially hide the NavigationMenu
        m_NavigationMenu = FrameWidget.Cast(m_Root.FindAnyWidget("NavigationMenu"));
        if (m_NavigationMenu)
        {
            m_NavigationMenu.SetFlags(m_NavigationMenu.GetFlags() & ~WidgetFlags.VISIBLE);
            Print("[ATAK_UI] NavigationMenu found and set to hidden");
        }
        else
        {
            // Try to find it in systems layout
            if (m_SystemsRoot)
            {
                m_NavigationMenu = FrameWidget.Cast(m_SystemsRoot.FindAnyWidget("NavigationMenu"));
                if (m_NavigationMenu)
                {
                    m_NavigationMenu.SetFlags(m_NavigationMenu.GetFlags() & ~WidgetFlags.VISIBLE);
                    Print("[ATAK_UI] NavigationMenu found in systems layout and set to hidden");
                }
            }
            if (!m_NavigationMenu)
            {
                Print("[ATAK_UI] Warning: NavigationMenu widget not found!", LogLevel.WARNING);
            }
        }
        
        // Check for animation widgets to silence warnings
        ImageWidget fadeImage = ImageWidget.Cast(m_Root.FindAnyWidget("FadeImage"));
        ImageWidget loadingVisual = ImageWidget.Cast(m_Root.FindAnyWidget("LoadingVisual"));
        ImageWidget initLoadingImage = ImageWidget.Cast(m_Root.FindAnyWidget("InitLoadingImage"));
        
        if (fadeImage) Print("[ATAK_UI] FadeImage found - animation warnings should stop");
        if (loadingVisual) Print("[ATAK_UI] LoadingVisual found - animation warnings should stop");
        if (initLoadingImage) Print("[ATAK_UI] InitLoadingImage found - animation warnings should stop");
    }
    
    //------------------------------------------------------------------------------------------------
    // Load systems layout and attach to main layout
    protected void EnsureSystemsLayout()
    {
        if (m_SystemsRoot || !m_Root) return;
        
        if (!m_ATakLayoutSystems || m_ATakLayoutSystems == "{0000000000000000}")
        {
            Print("[ATAK_UI] No Systems Layout specified - skipping");
            return;
        }
        
        PrintFormat("[ATAK_UI] Creating systems layout from: %1", m_ATakLayoutSystems);
        m_SystemsRoot = m_WS.CreateWidgets(m_ATakLayoutSystems);
        if (!m_SystemsRoot)
        {
            Print("[ATAK_UI] Systems layout creation failed", LogLevel.ERROR);
            return;
        }
        
        // Hide systems layout initially
        if (m_SystemsRoot)
        {
            m_SystemsRoot.SetFlags(m_SystemsRoot.GetFlags() & ~WidgetFlags.VISIBLE);
            m_bSystemsVisible = false;
            Print("[ATAK_UI] Systems layout loaded and hidden");
        }
    }

    void ShowATAK()
    {
        Print("[ATAK_UI] ShowATAK called");
        EnsureLayout();
        if (!m_Root) 
        {
            Print("[ATAK_UI] Cannot show ATAK - layout creation failed", LogLevel.ERROR);
            return;
        }

        m_Root.SetFlags(m_Root.GetFlags() | WidgetFlags.VISIBLE);
        
        // Ensure device background is visible
        ImageWidget deviceBg = ImageWidget.Cast(m_Root.FindAnyWidget("DeviceBackground"));
        if (deviceBg)
        {
            deviceBg.SetFlags(deviceBg.GetFlags() | WidgetFlags.VISIBLE);
            Print("[ATAK_UI] DeviceBackground found and set to visible");
        }
        else
        {
            Print("[ATAK_UI] Warning: DeviceBackground widget not found!", LogLevel.WARNING);
        }
        
        m_bATAKVisible = true;
        Print("[ATAK_UI] ATAK UI shown");
    }

    void HideATAK()
    {
        if (!m_Root) 
        {
            Print("[ATAK_UI] Cannot hide ATAK - no layout exists");
            return;
        }
        
        m_Root.SetFlags(m_Root.GetFlags() & ~WidgetFlags.VISIBLE);
        m_bATAKVisible = false;
        Print("[ATAK_UI] ATAK UI hidden");
    }
    
    //------------------------------------------------------------------------------------------------
    // Toggle systems layout visibility
    void ToggleSystemsLayout()
    {
        if (!m_SystemsRoot && m_Root)
        {
            EnsureSystemsLayout();
        }
        
        if (!m_SystemsRoot)
        {
            Print("[ATAK_UI] Systems layout not available");
            return;
        }
        
        m_bSystemsVisible = !m_bSystemsVisible;
        if (m_bSystemsVisible)
        {
            m_SystemsRoot.SetFlags(m_SystemsRoot.GetFlags() | WidgetFlags.VISIBLE);
            Print("[ATAK_UI] Systems layout shown");
        }
        else
        {
            m_SystemsRoot.SetFlags(m_SystemsRoot.GetFlags() & ~WidgetFlags.VISIBLE);
            Print("[ATAK_UI] Systems layout hidden");
        }
    }
    
    void ShowSystemsLayout()
    {
        if (!m_SystemsRoot && m_Root)
        {
            EnsureSystemsLayout();
        }
        
        if (m_SystemsRoot)
        {
            m_SystemsRoot.SetFlags(m_SystemsRoot.GetFlags() | WidgetFlags.VISIBLE);
            m_bSystemsVisible = true;
        }
    }
    
    void HideSystemsLayout()
    {
        if (m_SystemsRoot)
        {
            m_SystemsRoot.SetFlags(m_SystemsRoot.GetFlags() & ~WidgetFlags.VISIBLE);
            m_bSystemsVisible = false;
        }
    }
    
    void ToggleNavigationMenu()
    {
        if (!m_Root) return;
        
        if (!m_NavigationMenu)
        {
            m_NavigationMenu = FrameWidget.Cast(m_Root.FindAnyWidget("NavigationMenu"));
            if (!m_NavigationMenu)
            {
                Print("[ATAK_UI] Warning: NavigationMenu widget not found!", LogLevel.WARNING);
                return;
            }
        }
        
        bool isVisible = (m_NavigationMenu.GetFlags() & WidgetFlags.VISIBLE) != 0;
        if (isVisible)
        {
            m_NavigationMenu.SetFlags(m_NavigationMenu.GetFlags() & ~WidgetFlags.VISIBLE);
            Print("[ATAK_UI] Navigation menu hidden");
        }
        else
        {
            m_NavigationMenu.SetFlags(m_NavigationMenu.GetFlags() | WidgetFlags.VISIBLE);
            Print("[ATAK_UI] Navigation menu shown");
        }
    }

    bool IsVisible() 
    { 
        return m_Root != null && (m_Root.GetFlags() & WidgetFlags.VISIBLE) != 0; 
    }
    
    // Methods called by ATAK_DeviceWidget
    void ZoomIn()
    {
        Print("[ATAK_UI] ZoomIn called");
        // Implementation for zoom in
    }
    
    void ZoomOut()
    {
        Print("[ATAK_UI] ZoomOut called");
        // Implementation for zoom out
    }
    
    void CenterView()
    {
        Print("[ATAK_UI] CenterView called");
        // Implementation for center view
    }
    
    
    void CloseNavigationMenu()
    {
        Print("[ATAK_UI] CloseNavigationMenu called");
        // Implementation for close navigation menu
    }
}
