// ATAK Network Retry Test Component
// Demonstrates usage of ATAK_NetworkRetryComponent with example scenarios

//------------------------------------------------------------------------------------------------
class ATAK_NetworkRetryTestClass : ScriptGameComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Test component for network retry functionality
class ATAK_NetworkRetryTest : ScriptGameComponent
{
	protected ATAK_NetworkRetryComponent m_RetryComponent;
	protected bool m_bTestsRun = false;
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		// Get retry component
		m_RetryComponent = ATAK_NetworkRetryComponent.Cast(owner.FindComponent(ATAK_NetworkRetryComponent));
		if (!m_RetryComponent)
		{
			Print("[ATAK_RetryTest] ERROR: No retry component found!", LogLevel.ERROR);
			return;
		}
		
		// Run tests after a delay (allow system to initialize)
		GetGame().GetCallqueue().CallLater(RunTests, 2000, false);
		
		Print("[ATAK_RetryTest] Test component initialized - tests will run in 2 seconds");
	}
	
	//------------------------------------------------------------------------------------------------
	//! Run test scenarios
	protected void RunTests()
	{
		if (m_bTestsRun)
			return;
		
		m_bTestsRun = true;
		
		Print("[ATAK_RetryTest] ========== RUNNING RETRY TESTS ==========");
		
		// Test 1: Successful request (no retry needed)
		TestSuccessfulRequest();
		
		// Test 2: Failed request with retry (after 1.5 seconds)
		GetGame().GetCallqueue().CallLater(TestFailedRequest, 1500, false);
		
		// Test 3: Multiple concurrent requests (after 3 seconds)
		GetGame().GetCallqueue().CallLater(TestMultipleRequests, 3000, false);
		
		// Test 4: Check retry timing (after 6 seconds)
		GetGame().GetCallqueue().CallLater(TestRetryTiming, 6000, false);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Test 1: Successful request
	protected void TestSuccessfulRequest()
	{
		Print("[ATAK_RetryTest] --- Test 1: Successful Request ---");
		
		// Register request
		ATAK_NetworkRequest request = m_RetryComponent.RegisterRequest("test_success_1");
		if (!request)
		{
			Print("[ATAK_RetryTest] ✗ Failed to register request", LogLevel.ERROR);
			return;
		}
		
		Print("[ATAK_RetryTest] ✓ Request registered successfully");
		
		// Simulate successful completion
		m_RetryComponent.CompleteRequest("test_success_1");
		
		Print("[ATAK_RetryTest] ✓ Request completed - should be removed from queue");
		PrintFormat("[ATAK_RetryTest] Pending requests: %1", m_RetryComponent.GetPendingRequestCount());
	}
	
	//------------------------------------------------------------------------------------------------
	//! Test 2: Failed request with retry
	protected void TestFailedRequest()
	{
		Print("[ATAK_RetryTest] --- Test 2: Failed Request with Retry ---");
		
		// Register request
		ATAK_NetworkRequest request = m_RetryComponent.RegisterRequest("test_fail_1");
		if (!request)
		{
			Print("[ATAK_RetryTest] ✗ Failed to register request", LogLevel.ERROR);
			return;
		}
		
		Print("[ATAK_RetryTest] ✓ Request registered");
		
		// Simulate first failure
		bool willRetry = m_RetryComponent.FailRequest("test_fail_1");
		PrintFormat("[ATAK_RetryTest] First failure - will retry: %1", willRetry);
		
		// Check if ready for retry after delay
		GetGame().GetCallqueue().CallLater(CheckRetryStatus, 2000, false, "test_fail_1");
	}
	
	//------------------------------------------------------------------------------------------------
	//! Test 3: Multiple concurrent requests
	protected void TestMultipleRequests()
	{
		Print("[ATAK_RetryTest] --- Test 3: Multiple Concurrent Requests ---");
		
		// Register multiple requests
		for (int i = 0; i < 5; i++)
		{
			string requestId = string.Format("test_multi_%1", i);
			ATAK_NetworkRequest request = m_RetryComponent.RegisterRequest(requestId);
			
			if (request)
				PrintFormat("[ATAK_RetryTest] ✓ Registered request: %1", requestId);
		}
		
		PrintFormat("[ATAK_RetryTest] Total pending requests: %1", m_RetryComponent.GetPendingRequestCount());
		
		// Complete some requests
		m_RetryComponent.CompleteRequest("test_multi_0");
		m_RetryComponent.CompleteRequest("test_multi_2");
		
		// Fail others
		m_RetryComponent.FailRequest("test_multi_1");
		m_RetryComponent.FailRequest("test_multi_3");
		
		PrintFormat("[ATAK_RetryTest] After completions/failures - pending: %1", 
			m_RetryComponent.GetPendingRequestCount());
	}
	
	//------------------------------------------------------------------------------------------------
	//! Test 4: Check retry timing
	protected void TestRetryTiming()
	{
		Print("[ATAK_RetryTest] --- Test 4: Retry Timing Check ---");
		
		// Get retry-ready requests
		array<string> retryReady = new array<string>();
		m_RetryComponent.GetRetryReadyRequests(retryReady);
		
		PrintFormat("[ATAK_RetryTest] Requests ready for retry: %1", retryReady.Count());
		
		for (int i = 0; i < retryReady.Count(); i++)
		{
			PrintFormat("[ATAK_RetryTest]   - %1", retryReady.Get(i));
		}
		
		Print("[ATAK_RetryTest] ========== TESTS COMPLETE ==========");
	}
	
	//------------------------------------------------------------------------------------------------
	//! Check if request is ready for retry
	//! @param requestId Request to check
	protected void CheckRetryStatus(string requestId)
	{
		bool shouldRetry = m_RetryComponent.ShouldRetry(requestId);
		PrintFormat("[ATAK_RetryTest] Request '%1' should retry: %2", requestId, shouldRetry);
		
		if (shouldRetry)
		{
			Print("[ATAK_RetryTest] ⟳ Retrying request...");
			
			// Simulate second failure
			bool willRetry = m_RetryComponent.FailRequest(requestId);
			PrintFormat("[ATAK_RetryTest] Second failure - will retry: %1", willRetry);
		}
	}
}
