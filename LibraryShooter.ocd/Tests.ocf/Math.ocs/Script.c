protected func InitializePlayer(int plr)
{
	// Set zoom to full map size.
	SetPlayerZoomByViewRange(plr, 300, nil, PLRZOOM_Direct);
	
	// No FoW to see everything happening.
	SetFoW(false, plr);
		
	// Start!
	LaunchTest(1);
	return;
}

/*-- Test Control --*/

static const FX_TEST_CONTROL = "IntTestControl";

// Aborts the current test and launches the specified test instead.
global func LaunchTest(int nr)
{
	// Get the control effect.
	var effect = GetEffect(FX_TEST_CONTROL, nil);
	if (!effect)
	{
		// Create a new control effect and launch the test.
		effect = AddEffect(FX_TEST_CONTROL, nil, 100, 2);
		effect.test_number = nr;
		effect.launched = false;
		effect.player = GetPlayerByIndex(0, C4PT_User);
		effect.user = GetHiRank(effect.player);
		effect.user->SetPosition(LandscapeWidth() / 2, effect.user->GetY());		
		return;
	}
	// Finish the currently running test.
	Call(Format("~Test%d_OnFinished", effect.test_number));
	// Start the requested test by just setting the test number and setting 
	// effect.launched to false, effect will handle the rest.
	effect.test_number = nr;
	effect.launched = false;
	return;
}

// Calling this function skips the current test, does not work if last test has been ran already.
global func SkipTest()
{
	Test().tests_skipped++;
	NextTest();
}

global func FailTest()
{
	Test().tests_failed++;
	NextTest();
}

global func NextTest()
{
	// Get the control effect.
	var effect = Test();
	if (!effect)
		return;
	// Finish the previous test.
	Call(Format("~Test%d_OnFinished", effect.test_number));
	// Start the next test by just increasing the test number and setting 
	// effect.launched to false, effect will handle the rest.
	effect.test_number++;
	effect.launched = false;
	return;
}

global func Test()
{
	// Get the control effect.
	var effect = GetEffect(FX_TEST_CONTROL, nil);
	return effect;
}


/*-- Test Effect --*/

global func FxIntTestControlStart(object target, proplist effect, int temporary)
{
	if (temporary)
		return FX_OK;
	// Set default interval.
	effect.Interval = 2;
	return FX_OK;
}

global func FxIntTestControlTimer(object target, proplist effect)
{
	// Launch new test if needed.
	if (!effect.launched)
	{
		// Log test start.
		Log("=====================================");
		Log("Test %d started:", effect.test_number);
		// Start the test if available, otherwise finish test sequence.
		if (!Call(Format("~Test%d_OnStart", effect.test_number)))
		{
			Log("Test %d not available, the previous test was the last test.", effect.test_number);
			Log("=====================================");
			
			if (effect.tests_skipped <= 0 && effect.tests_failed <= 0)
			{
				Log("All tests have been successfully completed!");
			}
			else
			{
				Log("%d tests were skipped.", effect.tests_skipped);
				Log("%d tests failed.", effect.tests_failed);
			}
			return FX_Execute_Kill;
		}
		effect.launched = true;
	}		
	// Check whether the current test has been finished.
	if (Call(Format("Test%d_Completed", effect.test_number)))
	{
		effect.launched = false;
		// Call the test on finished function.
		Call(Format("~Test%d_OnFinished", effect.test_number));
		// Log result and increase test number.
		Log("Test %d successfully completed.", effect.test_number);
		effect.test_number++;
	}
	return FX_OK;
}

global func pass(string message)
{
	Log(Format("[Passed] %s", message));
}

global func fail(string message)
{
	Log(Format("[Failed] %s", message));
}

global func doTest(string message, actual, expected)
{
	var passed = actual == expected;
	
	var log_message = Format(message, actual, expected);
	
	if (passed)
		pass(log_message);
	else
		fail(log_message);
		
	return passed;
}

global func askUser(string message)
{
	return false;
}

/*-- The actual tests --*/

// --------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------

global func Test1_OnStart()
{
	Log("Test for math function: GetExponent");
	return true;
}

global func Test1_Completed()
{
	var bases = [2, 3, 5, 7, 10];

	for (var base in bases)
	{
		for (var exponent = 0; exponent <= 5; ++exponent)
		{
			var value = base ** exponent;
			Log("Testing %d ^ %d = %d", base, exponent, value);
			doTest("Exponent is %d, should be %d", GetExponent(value, base), exponent);
		}
	}

	return true;
}

global func Test1_OnFinished(){}
