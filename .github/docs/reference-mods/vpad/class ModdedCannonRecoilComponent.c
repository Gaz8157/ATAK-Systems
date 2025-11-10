class ModdedCannonRecoilComponentClass : ScriptComponentClass {}

[ComponentEditorProps(category: "Modded/Vehicle Effects", description: "Applies a temporary, fast rotation to a bone (e.g., root2) when the cannon fires to simulate visual recoil or vibration.")]
class ModdedCannonRecoilComponent : ScriptComponent
{
    // --- Configuration Fields ---
    
    [Attribute("root2", UIWidgets.EditBox, desc: "Name of the bone to rotate (e.g., root2).")]
    protected string m_BoneName = "root2";

    [Attribute("2000.0", UIWidgets.EditBox, desc: "Rotations Per Minute (RPM) for the spin effect.")]
    protected float m_TargetRPM = 2000.0;

    [Attribute("0.1", UIWidgets.EditBox, desc: "Duration the bone spins after the weapon fires (in seconds).")]
    protected float m_SpinDuration = 0.1;

    [Attribute("false", desc: "Enable automatic simulation for testing")]
    protected bool m_EnableAutoTest = false;

    [Attribute("5.0", UIWidgets.EditBox, desc: "Auto test interval (seconds)")]
    protected float m_AutoTestInterval = 5.0;

    // --- Runtime Variables ---
    protected bool m_IsSpinning = false;
    protected float m_SpinTimer = 0.0;
    protected float m_LastAutoTest = 0.0;
    
    // --- Constants for Calculation ---
    protected const float DEGREES_PER_ROTATION = 360.0;
    protected const float SECONDS_PER_MINUTE = 60.0;

    // --- Component Lifecycle ---
    override void OnPostInit(IEntity owner)
    {
        super.OnPostInit(owner);
        SetEventMask(owner, EntityEvent.FRAME);
        Print("[ModdedCannonRecoil] Component initialized on bone: " + m_BoneName);
    }

    override void EOnFrame(IEntity owner, float timeSlice)
    {
        // Handle auto testing if enabled
        if (m_EnableAutoTest)
        {
            m_LastAutoTest += timeSlice;
            if (m_LastAutoTest >= m_AutoTestInterval)
            {
                OnWeaponFired();
                m_LastAutoTest = 0.0;
            }
        }

        // Stop if the effect is not active
        if (!m_IsSpinning)
            return;

        // Decrement the timer. If time is up, stop spinning.
        m_SpinTimer -= timeSlice;
        if (m_SpinTimer <= 0.0)
        {
            m_IsSpinning = false;
            m_SpinTimer = 0.0;
            Print("[ModdedCannonRecoil] Spin effect on " + m_BoneName + " finished.");
            return;
        }

        // --- Fetch Skeleton and Bone ---
        Entity entity = GetOwner();
        if (!entity)
            return;

        Skeleton skeleton = entity.GetSkeleton();
        if (!skeleton)
            return;

        Bone bone = skeleton.FindBone(m_BoneName);
        if (!bone)
        {
            Print("[ModdedCannonRecoil] Warning: Bone '" + m_BoneName + "' not found on entity.");
            return;
        }

        // --- Continuous Rotation Logic ---
        
        // Calculate the angular velocity in Degrees Per Second (DPS)
        float degreesPerSecond = (m_TargetRPM * DEGREES_PER_ROTATION) / SECONDS_PER_MINUTE; 
        
        // Calculate the angle change for this specific frame
        float angleChange = degreesPerSecond * timeSlice; 

        // Create the rotation quaternion for the Z-axis (Roll)
        // We only want rotation around the Z-axis (Roll), so Pitch (X) and Yaw (Y) are 0.
        Math.Quaternion rotationToAdd = Math.Quaternion.FromEuler(0.0, 0.0, angleChange);

        // Apply the rotation locally
        Math.Quaternion currentRotation = bone.GetLocalRotation();
        Math.Quaternion newRotation = currentRotation * rotationToAdd; 
        
        bone.SetLocalRotation(newRotation);
    }

    // --- Public API ---
    
    /**
     * @brief Method called by the vehicle weapon's firing system or external scripts
     */
    void OnWeaponFired()
    {
        // Start the spin effect
        m_IsSpinning = true;
        m_SpinTimer = m_SpinDuration;
        Print("[ModdedCannonRecoil] Cannon Fired: Starting " + m_TargetRPM.ToString() + " RPM spin on " + m_BoneName + ".");
    }

    /**
     * @brief Manually trigger the recoil effect
     */
    void TriggerRecoil()
    {
        OnWeaponFired();
    }

    /**
     * @brief Stop the current recoil effect
     */
    void StopRecoil()
    {
        m_IsSpinning = false;
        m_SpinTimer = 0.0;
        Print("[ModdedCannonRecoil] Recoil effect manually stopped.");
    }

    /**
     * @brief Check if the component is currently spinning
     */
    bool IsSpinning()
    {
        return m_IsSpinning;
    }

    /**
     * @brief Get the current spin progress (0.0 to 1.0)
     */
    float GetSpinProgress()
    {
        if (!m_IsSpinning) return 0.0;
        return 1.0 - (m_SpinTimer / m_SpinDuration);
    }

    /**
     * @brief Set the target bone name at runtime
     */
    void SetBoneName(string boneName)
    {
        if (boneName && !boneName.IsEmpty())
        {
            m_BoneName = boneName;
            Print("[ModdedCannonRecoil] Bone name changed to: " + m_BoneName);
        }
    }

    /**
     * @brief Set the RPM at runtime
     */
    void SetTargetRPM(float rpm)
    {
        if (rpm > 0)
        {
            m_TargetRPM = rpm;
            Print("[ModdedCannonRecoil] Target RPM changed to: " + m_TargetRPM.ToString());
        }
    }

    /**
     * @brief Set the spin duration at runtime
     */
    void SetSpinDuration(float duration)
    {
        if (duration > 0)
        {
            m_SpinDuration = duration;
            Print("[ModdedCannonRecoil] Spin duration changed to: " + m_SpinDuration.ToString() + " seconds");
        }
    }
}