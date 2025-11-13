# ATAK Network Retry System

## Overview

The ATAK Network Retry System provides automatic retry logic for failed network requests (RPC calls) in multiplayer scenarios. It implements exponential backoff to handle transient network failures gracefully without overwhelming the network.

## Components

### 1. ATAK_NetworkRetryComponent

Core component that manages retry logic for network requests.

**Features:**
- Automatic retry with exponential backoff
- Configurable retry attempts (default: 3)
- Request timeout handling (default: 30 seconds)
- State tracking for pending/completed/failed requests
- Periodic cleanup of old requests

**Configuration:**
```cpp
MAX_RETRY_ATTEMPTS = 3          // Maximum retry attempts
INITIAL_RETRY_DELAY = 1.0       // Initial delay (1 second)
BACKOFF_MULTIPLIER = 2.0        // Exponential backoff (2x each retry)
MAX_REQUEST_TIMEOUT = 30.0      // Request timeout (30 seconds)
```

**Retry Schedule:**
- 1st attempt: Immediate
- 2nd attempt: After 1 second
- 3rd attempt: After 2 seconds
- 4th attempt: After 4 seconds
- After 3 failures: Request permanently fails

### 2. ATAK_MissionSyncComponent

Example component demonstrating retry usage for mission data synchronization.

**Features:**
- Mission briefing synchronization
- Automatic retry of failed broadcasts
- Client/server architecture support
- Integration with ATAK device UI

### 3. ATAK_NetworkRetryTest

Testing component for validating retry behavior.

## Usage

### Basic Usage

1. **Add component to entity:**

```cpp
// In your entity prefab or component initialization
ATAK_NetworkRetryComponent retryComponent = ATAK_NetworkRetryComponent.Cast(
    owner.FindComponent(ATAK_NetworkRetryComponent)
);
```

2. **Register a network request:**

```cpp
// Register request before sending RPC
string requestId = "mission_update_12345";
ATAK_NetworkRequest request = retryComponent.RegisterRequest(requestId);
```

3. **Send RPC and handle result:**

```cpp
// Send your RPC call
bool success = SendMyRPC(data);

if (success)
{
    // Mark request as completed
    retryComponent.CompleteRequest(requestId);
}
else
{
    // Mark request as failed - will retry automatically
    bool willRetry = retryComponent.FailRequest(requestId);
    
    if (!willRetry)
    {
        // Maximum retries exceeded - handle permanent failure
        Print("Request permanently failed - giving up");
    }
}
```

4. **Check for retry-ready requests:**

```cpp
// Periodically check for requests ready to retry
array<string> retryReady = new array<string>();
retryComponent.GetRetryReadyRequests(retryReady);

for (int i = 0; i < retryReady.Count(); i++)
{
    string requestId = retryReady.Get(i);
    // Resend the RPC for this request
    ResendMyRPC(requestId);
}
```

### Advanced Usage: Mission Synchronization

Example from `ATAK_MissionSyncComponent`:

```cpp
// Server-side: Set mission briefing
void SetMissionBriefing(string missionId, string title, string briefing, string objectives)
{
    // Register request
    string requestId = string.Format("mission_update_%1_%2", missionId, Math.RandomInt(0, 9999));
    ATAK_NetworkRequest request = m_RetryComponent.RegisterRequest(requestId);
    
    // Send RPC
    bool success = Rpc(RPC_UpdateMissionBriefing, missionId, title, briefing, objectives);
    
    if (success)
    {
        m_RetryComponent.CompleteRequest(requestId);
    }
    else
    {
        m_RetryComponent.FailRequest(requestId);
    }
}

// Periodic retry check (in component update loop)
void CheckRetryQueue()
{
    array<string> retryReady = new array<string>();
    m_RetryComponent.GetRetryReadyRequests(retryReady);
    
    for (int i = 0; i < retryReady.Count(); i++)
    {
        // Resend failed mission updates
        ResendMissionUpdate(retryReady.Get(i));
    }
}
```

## Integration with Entity Prefabs

To use the retry system, add `ATAK_NetworkRetryComponent` to your entity prefab:

1. Open entity prefab in Workbench (e.g., `DefaultPlayerController.et`)
2. Add component: `ATAK_NetworkRetryComponent`
3. Configure if needed (or use defaults)

Example configuration in `.et` file:
```
SCR_BaseGameMode {
  components {
    ATAK_NetworkRetryComponent {
      // Uses default configuration
    }
    ATAK_MissionSyncComponent {
      // Will automatically find and use retry component
    }
  }
}
```

## Best Practices

### 1. Use Unique Request IDs
Always generate unique request IDs to avoid conflicts:
```cpp
string requestId = string.Format("mission_update_%1_%2", 
    missionId, System.GetTickCount());
```

### 2. Clean Up Completed Requests
The component automatically removes completed/failed requests, but you can also manually clear:
```cpp
retryComponent.ClearAllRequests();
```

### 3. Handle Permanent Failures
Always check if retry will happen and handle permanent failures:
```cpp
bool willRetry = retryComponent.FailRequest(requestId);
if (!willRetry)
{
    // Log error, notify user, or use fallback
    Print("Request permanently failed after 3 attempts");
}
```

### 4. Monitor Pending Requests
Track pending request count for diagnostics:
```cpp
int pendingCount = retryComponent.GetPendingRequestCount();
if (pendingCount > 10)
{
    Print("WARNING: High number of pending requests - network issues?");
}
```

## Testing

Run the test component to verify retry behavior:

1. Add `ATAK_NetworkRetryTest` to an entity
2. The component will run automated tests after 2 seconds
3. Check console output for test results

Expected output:
```
[ATAK_RetryTest] ========== RUNNING RETRY TESTS ==========
[ATAK_RetryTest] --- Test 1: Successful Request ---
[ATAK_RetryTest] ✓ Request registered successfully
[ATAK_RetryTest] ✓ Request completed - should be removed from queue
[ATAK_RetryTest] --- Test 2: Failed Request with Retry ---
[ATAK_RetryTest] ⟳ Request 'test_fail_1' failed - retry #1 in 1.0s
...
```

## Performance Considerations

- **Processing Interval:** Retry queue is processed every 0.5 seconds
- **Cleanup:** Completed/failed requests are cleaned up every 10 seconds
- **Memory:** Each request uses ~100 bytes (minimal overhead)
- **Network:** Exponential backoff prevents network flooding

## Troubleshooting

### Requests not retrying?

1. Check if retry component is initialized:
   ```cpp
   if (!m_RetryComponent)
       Print("ERROR: Retry component not found!");
   ```

2. Verify request was registered:
   ```cpp
   ATAK_NetworkRequest request = m_RetryComponent.RegisterRequest(requestId);
   if (!request)
       Print("ERROR: Failed to register request!");
   ```

3. Check retry timing:
   ```cpp
   bool shouldRetry = m_RetryComponent.ShouldRetry(requestId);
   Print(string.Format("Should retry: %1", shouldRetry));
   ```

### Too many retries?

Adjust configuration constants in `ATAK_NetworkRetryComponent.c`:
```cpp
protected const int MAX_RETRY_ATTEMPTS = 3;        // Reduce to 2
protected const float MAX_REQUEST_TIMEOUT = 30.0;  // Reduce to 15.0
```

### Request timing out?

Increase timeout if legitimate requests are timing out:
```cpp
protected const float MAX_REQUEST_TIMEOUT = 60.0;  // Increase to 60 seconds
```

## Future Enhancements

Potential improvements for future versions:

1. **Priority Queues:** High-priority requests retry first
2. **Adaptive Backoff:** Adjust delays based on network conditions
3. **Batch Retries:** Group multiple retries into single RPC
4. **Success Rate Metrics:** Track and log retry success rates
5. **Custom Callbacks:** Per-request success/failure callbacks

## See Also

- **Reference Mod:** `.github/docs/reference-mods/Tactical-Data-Link-main/` - RPC patterns
- **Copilot Instructions:** `.github/copilot-instructions.md` - Network sync patterns
- **API Documentation:** `.github/docs/api/` - Replication and RPC APIs

## License

Part of ATAK Systems mod for Arma Reforger.
