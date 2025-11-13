// ATAK Network Retry Component
// Handles automatic retry logic for failed network requests (RPC calls)
// Implements exponential backoff strategy for multiplayer reliability

//------------------------------------------------------------------------------------------------
//! Request data structure to track retry state
class ATAK_NetworkRequest
{
	string m_sRequestId;           // Unique request identifier
	int m_iAttemptCount;           // Number of attempts made
	float m_fNextRetryTime;        // When to retry next (in seconds)
	float m_fInitialTime;          // When request was first created
	bool m_bCompleted;             // Whether request succeeded
	bool m_bFailed;                // Whether request permanently failed
	
	void ATAK_NetworkRequest(string requestId)
	{
		m_sRequestId = requestId;
		m_iAttemptCount = 0;
		m_fNextRetryTime = 0;
		m_fInitialTime = System.GetTickCount() / 1000.0;
		m_bCompleted = false;
		m_bFailed = false;
	}
}

//------------------------------------------------------------------------------------------------
//! Component class for network retry functionality
class ATAK_NetworkRetryComponentClass : ScriptGameComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Network retry component - manages automatic retry of failed RPC calls
class ATAK_NetworkRetryComponent : ScriptGameComponent
{
	// Configuration
	protected const int MAX_RETRY_ATTEMPTS = 3;           // Maximum number of retry attempts
	protected const float INITIAL_RETRY_DELAY = 1.0;      // Initial retry delay in seconds
	protected const float BACKOFF_MULTIPLIER = 2.0;       // Exponential backoff multiplier
	protected const float MAX_REQUEST_TIMEOUT = 30.0;     // Max time before request is abandoned
	
	// State tracking
	protected ref map<string, ref ATAK_NetworkRequest> m_mPendingRequests;
	protected bool m_bProcessingEnabled = true;
	protected float m_fLastProcessTime = 0;
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		// Initialize request map
		m_mPendingRequests = new map<string, ref ATAK_NetworkRequest>();
		
		// Start retry processing loop (check every 0.5 seconds)
		GetGame().GetCallqueue().CallLater(ProcessRetryQueue, 500, true);
		
		Print("[ATAK_NetworkRetry] Component initialized - retry enabled");
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnDelete(IEntity owner)
	{
		// Stop processing
		m_bProcessingEnabled = false;
		GetGame().GetCallqueue().Remove(ProcessRetryQueue);
		
		// Clear pending requests
		if (m_mPendingRequests)
			m_mPendingRequests.Clear();
		
		super.OnDelete(owner);
		Print("[ATAK_NetworkRetry] Component cleaned up");
	}
	
	//------------------------------------------------------------------------------------------------
	//! Register a new network request for retry handling
	//! @param requestId Unique identifier for this request
	//! @return Request object for tracking
	ATAK_NetworkRequest RegisterRequest(string requestId)
	{
		if (!requestId || requestId == "")
		{
			Print("[ATAK_NetworkRetry] ERROR: Invalid request ID!", LogLevel.ERROR);
			return null;
		}
		
		// Check if request already exists
		ATAK_NetworkRequest existingRequest = m_mPendingRequests.Get(requestId);
		if (existingRequest)
		{
			PrintFormat("[ATAK_NetworkRetry] WARNING: Request '%1' already registered", requestId);
			return existingRequest;
		}
		
		// Create new request
		ATAK_NetworkRequest request = new ATAK_NetworkRequest(requestId);
		m_mPendingRequests.Set(requestId, request);
		
		PrintFormat("[ATAK_NetworkRetry] Registered request: %1", requestId);
		return request;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Mark a request as completed successfully
	//! @param requestId Request identifier to complete
	void CompleteRequest(string requestId)
	{
		ATAK_NetworkRequest request = m_mPendingRequests.Get(requestId);
		if (!request)
		{
			PrintFormat("[ATAK_NetworkRetry] WARNING: Cannot complete unknown request '%1'", requestId);
			return;
		}
		
		request.m_bCompleted = true;
		PrintFormat("[ATAK_NetworkRetry] ✓ Request '%1' completed after %2 attempt(s)", 
			requestId, request.m_iAttemptCount);
		
		// Remove from pending queue
		m_mPendingRequests.Remove(requestId);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Mark a request as failed (retry or abandon based on attempt count)
	//! @param requestId Request identifier that failed
	//! @return true if request will be retried, false if abandoned
	bool FailRequest(string requestId)
	{
		ATAK_NetworkRequest request = m_mPendingRequests.Get(requestId);
		if (!request)
		{
			PrintFormat("[ATAK_NetworkRetry] WARNING: Cannot fail unknown request '%1'", requestId);
			return false;
		}
		
		// Increment attempt count
		request.m_iAttemptCount = request.m_iAttemptCount + 1;
		
		// Check if we should retry
		if (request.m_iAttemptCount >= MAX_RETRY_ATTEMPTS)
		{
			// Max retries exceeded - abandon request
			request.m_bFailed = true;
			PrintFormat("[ATAK_NetworkRetry] ✗ Request '%1' permanently failed after %2 attempts", 
				requestId, request.m_iAttemptCount);
			
			// Remove from pending queue
			m_mPendingRequests.Remove(requestId);
			return false;
		}
		
		// Calculate exponential backoff delay
		float delay = CalculateRetryDelay(request.m_iAttemptCount);
		float currentTime = System.GetTickCount() / 1000.0;
		request.m_fNextRetryTime = currentTime + delay;
		
		PrintFormat("[ATAK_NetworkRetry] ⟳ Request '%1' failed - retry #%2 in %.1fs", 
			requestId, request.m_iAttemptCount, delay);
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Check if a request should be retried now
	//! @param requestId Request identifier to check
	//! @return true if request should be retried
	bool ShouldRetry(string requestId)
	{
		ATAK_NetworkRequest request = m_mPendingRequests.Get(requestId);
		if (!request)
			return false;
		
		// Don't retry completed or permanently failed requests
		if (request.m_bCompleted || request.m_bFailed)
			return false;
		
		// Check if timeout exceeded
		float currentTime = System.GetTickCount() / 1000.0;
		float elapsed = currentTime - request.m_fInitialTime;
		if (elapsed > MAX_REQUEST_TIMEOUT)
		{
			PrintFormat("[ATAK_NetworkRetry] ✗ Request '%1' timed out after %.1fs", 
				requestId, elapsed);
			request.m_bFailed = true;
			m_mPendingRequests.Remove(requestId);
			return false;
		}
		
		// Check if retry time has arrived
		if (currentTime >= request.m_fNextRetryTime)
			return true;
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Get list of requests ready for retry
	//! @param outRequests Array to fill with request IDs ready for retry
	void GetRetryReadyRequests(out array<string> outRequests)
	{
		if (!outRequests)
			outRequests = new array<string>();
		else
			outRequests.Clear();
		
		float currentTime = System.GetTickCount() / 1000.0;
		
		// Iterate through pending requests
		for (int i = 0; i < m_mPendingRequests.Count(); i++)
		{
			string requestId = m_mPendingRequests.GetKey(i);
			ATAK_NetworkRequest request = m_mPendingRequests.GetElement(i);
			
			if (!request)
				continue;
			
			// Skip completed or failed requests
			if (request.m_bCompleted || request.m_bFailed)
				continue;
			
			// Check timeout
			float elapsed = currentTime - request.m_fInitialTime;
			if (elapsed > MAX_REQUEST_TIMEOUT)
			{
				PrintFormat("[ATAK_NetworkRetry] ✗ Request '%1' timed out after %.1fs", 
					requestId, elapsed);
				request.m_bFailed = true;
				continue;
			}
			
			// Check if ready for retry
			if (currentTime >= request.m_fNextRetryTime)
				outRequests.Insert(requestId);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Calculate retry delay with exponential backoff
	//! @param attemptCount Number of attempts already made
	//! @return Delay in seconds before next retry
	protected float CalculateRetryDelay(int attemptCount)
	{
		// Exponential backoff: delay = INITIAL_RETRY_DELAY * (BACKOFF_MULTIPLIER ^ (attemptCount - 1))
		float delay = INITIAL_RETRY_DELAY;
		
		// Calculate backoff manually (no Math.Pow in Enforce Script)
		int multiplier = 1;
		for (int i = 1; i < attemptCount; i++)
		{
			multiplier = multiplier * 2; // BACKOFF_MULTIPLIER = 2.0
		}
		
		delay = delay * multiplier;
		
		// Cap maximum delay at 10 seconds
		if (delay > 10.0)
			delay = 10.0;
		
		return delay;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Process retry queue (called periodically)
	protected void ProcessRetryQueue()
	{
		if (!m_bProcessingEnabled)
			return;
		
		// Get current time
		float currentTime = System.GetTickCount() / 1000.0;
		
		// Throttle logging (only log every 10 seconds)
		if (currentTime - m_fLastProcessTime > 10.0)
		{
			PrintFormat("[ATAK_NetworkRetry] Processing queue - %1 pending request(s)", 
				m_mPendingRequests.Count());
			m_fLastProcessTime = currentTime;
		}
		
		// Clean up completed/failed requests
		array<string> requestsToRemove = new array<string>();
		
		for (int i = 0; i < m_mPendingRequests.Count(); i++)
		{
			string requestId = m_mPendingRequests.GetKey(i);
			ATAK_NetworkRequest request = m_mPendingRequests.GetElement(i);
			
			if (!request)
				continue;
			
			// Mark completed/failed for removal
			if (request.m_bCompleted || request.m_bFailed)
			{
				requestsToRemove.Insert(requestId);
				continue;
			}
			
			// Check timeout
			float elapsed = currentTime - request.m_fInitialTime;
			if (elapsed > MAX_REQUEST_TIMEOUT)
			{
				PrintFormat("[ATAK_NetworkRetry] ✗ Request '%1' timed out after %.1fs", 
					requestId, elapsed);
				request.m_bFailed = true;
				requestsToRemove.Insert(requestId);
			}
		}
		
		// Remove completed/failed/timed out requests
		for (int j = 0; j < requestsToRemove.Count(); j++)
		{
			m_mPendingRequests.Remove(requestsToRemove.Get(j));
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Get count of pending requests
	//! @return Number of pending requests
	int GetPendingRequestCount()
	{
		if (!m_mPendingRequests)
			return 0;
		
		return m_mPendingRequests.Count();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Clear all pending requests (useful for cleanup or reset)
	void ClearAllRequests()
	{
		if (m_mPendingRequests)
			m_mPendingRequests.Clear();
		
		Print("[ATAK_NetworkRetry] All pending requests cleared");
	}
}
