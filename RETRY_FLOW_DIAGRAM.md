# Network Retry System Flow Diagram

## Request Lifecycle

```
┌─────────────────────────────────────────────────────────────────┐
│                    NETWORK RETRY SYSTEM                          │
└─────────────────────────────────────────────────────────────────┘

PHASE 1: REGISTRATION
─────────────────────
    Application Code
         │
         ├──► RegisterRequest("mission_update_123")
         │
         └──► ATAK_NetworkRequest created
              - attemptCount = 0
              - nextRetryTime = 0
              - completed = false
              - failed = false


PHASE 2: INITIAL SEND
─────────────────────
    Application Code
         │
         ├──► Rpc(RPC_UpdateMission, data)
         │
         ├──► SUCCESS?
         │    │
         │    ├─YES─► CompleteRequest("mission_update_123")
         │    │        - Mark as completed
         │    │        - Remove from queue
         │    │        └──► ✓ DONE
         │    │
         │    └─NO──► FailRequest("mission_update_123")
         │             - Increment attemptCount to 1
         │             - Calculate delay: 1.0s
         │             - Set nextRetryTime = now + 1.0s
         │             └──► Continue to PHASE 3


PHASE 3: RETRY PROCESSING (every 0.5s)
──────────────────────────────────────
    ProcessRetryQueue()
         │
         ├──► Check all pending requests
         │
         ├──► Is nextRetryTime <= now?
         │    │
         │    ├─NO──► Wait for next cycle
         │    │
         │    └─YES─► GetRetryReadyRequests()
         │             │
         │             └──► Returns ["mission_update_123"]
         │
         └──► Application Code (CheckRetryQueue)
              │
              ├──► Rpc(RPC_UpdateMission, data)  // RETRY #1
              │
              ├──► SUCCESS?
              │    │
              │    ├─YES─► CompleteRequest()
              │    │        └──► ✓ DONE
              │    │
              │    └─NO──► FailRequest()
              │             - Increment attemptCount to 2
              │             - Calculate delay: 2.0s
              │             - Set nextRetryTime = now + 2.0s
              │             └──► Continue retry loop


EXPONENTIAL BACKOFF CALCULATION
────────────────────────────────
Attempt 1: delay = 1.0s * (2^0) = 1.0s
Attempt 2: delay = 1.0s * (2^1) = 2.0s
Attempt 3: delay = 1.0s * (2^2) = 4.0s
Attempt 4+: delay capped at 10.0s


FAILURE SCENARIOS
────────────────
┌────────────────────────────────────────┐
│ Scenario 1: Max Retries Exceeded      │
├────────────────────────────────────────┤
│ After 3 failures:                      │
│  - attemptCount >= MAX_RETRY_ATTEMPTS  │
│  - Mark as failed = true               │
│  - Remove from queue                   │
│  - Return willRetry = false            │
│  └──► ✗ PERMANENT FAILURE              │
└────────────────────────────────────────┘

┌────────────────────────────────────────┐
│ Scenario 2: Timeout                    │
├────────────────────────────────────────┤
│ After 30 seconds:                      │
│  - elapsed > MAX_REQUEST_TIMEOUT       │
│  - Mark as failed = true               │
│  - Remove from queue                   │
│  └──► ✗ TIMEOUT FAILURE                │
└────────────────────────────────────────┘


TIMING DIAGRAM
──────────────
Time     Event                              Request State
─────────────────────────────────────────────────────────
0.0s     Initial send FAILED                attemptCount=1, nextRetry=1.0s
0.5s     ProcessRetryQueue (no action)      waiting...
1.0s     ProcessRetryQueue → RETRY #1       Attempt 1
1.0s     Retry #1 FAILED                    attemptCount=2, nextRetry=3.0s
1.5s     ProcessRetryQueue (no action)      waiting...
2.0s     ProcessRetryQueue (no action)      waiting...
2.5s     ProcessRetryQueue (no action)      waiting...
3.0s     ProcessRetryQueue → RETRY #2       Attempt 2
3.0s     Retry #2 FAILED                    attemptCount=3, nextRetry=7.0s
3.5s     ProcessRetryQueue (no action)      waiting...
...
7.0s     ProcessRetryQueue → RETRY #3       Attempt 3 (FINAL)
7.0s     Retry #3 FAILED                    attemptCount=4, failed=true
         └──► ✗ PERMANENT FAILURE


MISSION SYNC INTEGRATION
────────────────────────
Server-Side:
┌─────────────────────────────────────────┐
│ SetMissionBriefing()                    │
├─────────────────────────────────────────┤
│ 1. Create mission data                  │
│ 2. Register request with retry system   │
│ 3. Broadcast RPC to all clients         │
│ 4. Mark success/failure                 │
│ 5. Retry component handles failures     │
└─────────────────────────────────────────┘
         │
         ├──► Rpc(RPC_UpdateMissionBriefing)
         │    - Sent to all connected clients
         │    - Reliable channel
         │    - Broadcast receiver
         │
         └──► Clients receive mission update
              - Update local mission data
              - Refresh UI


COMPONENT DEPENDENCIES
─────────────────────
┌────────────────────────────────────┐
│  Entity (Player Controller)        │
├────────────────────────────────────┤
│  ┌──────────────────────────────┐  │
│  │ ATAK_NetworkRetryComponent   │  │
│  │ - Core retry logic           │  │
│  │ - Request queue management   │  │
│  └────────────▲─────────────────┘  │
│               │                     │
│  ┌────────────┴─────────────────┐  │
│  │ ATAK_MissionSyncComponent    │  │
│  │ - Uses retry component       │  │
│  │ - Mission data sync          │  │
│  └──────────────────────────────┘  │
│                                     │
│  ┌──────────────────────────────┐  │
│  │ ATAK_NetworkRetryTest        │  │
│  │ - Tests retry component      │  │
│  │ - Validation suite           │  │
│  └──────────────────────────────┘  │
└────────────────────────────────────┘
```

## Key Takeaways

1. **Automatic**: Once registered, retries happen automatically
2. **Exponential Backoff**: Prevents network flooding (1s → 2s → 4s)
3. **Time-bound**: 30-second timeout prevents infinite waiting
4. **Attempt-limited**: Maximum 3 retries before permanent failure
5. **State-tracked**: Clear states (pending/completed/failed)
6. **Clean lifecycle**: Automatic cleanup of old requests
7. **Low overhead**: 500ms processing intervals
8. **Configurable**: Easy to adjust retry parameters
