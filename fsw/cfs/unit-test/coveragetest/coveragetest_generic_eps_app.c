/*
** File: coveragetest_generic_eps_app.c
**
** Purpose:
**   Coverage Unit Test cases for the GENERIC_EPS application
*/

#include "generic_eps_app_coveragetest_common.h"
#include "ut_generic_eps_app.h"

/*
 * UT helper to watch for a specific EVS_SendEvent
 */
typedef struct {
    uint16      ExpectedEvent;   // Numeric ID of the event
    uint32      MatchCount;   // How many times the expected event occurred
    const char *ExpectedFormat;
} UT_CheckEvent_t;


// Hook that intercepts code calls rather than sending them to the flight software system
static int32 UT_CheckEvent_Hook(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                const UT_StubContext_t *Context, va_list va)
{
    UT_CheckEvent_t *State = UserObj;
    uint16 EventId;
    if (Context->ArgCount > 0 &&   // Make sure the stub was called at least once and grab the event ID and ensure it matches
        (EventId = UT_Hook_GetArgValueByName(Context, "EventID", uint16)) == State->ExpectedEvent)
    {

        if (!State->ExpectedFormat ||
            strcmp(UT_Hook_GetArgValueByName(Context, "Spec", const char *),
        State->ExpectedFormat) == 0)
        {
            ++State->MatchCount;   // If the event ID and expected format match, count the occurrence
        }
    }
    return 0;
}


static void UT_CheckEvent_Setup(UT_CheckEvent_t *Evt, uint16 ExpectedEvent, const char *ExpectedFormat)
{
    memset(Evt, 0, sizeof(*Evt));  // Set match count back to 0
    Evt->ExpectedEvent  = ExpectedEvent;
    Evt->ExpectedFormat = ExpectedFormat;
    UT_SetVaHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_CheckEvent_Hook, Evt);  // install the hook everytime an event occurs
}

/*--------------------------------------------------------------------
 * Unit Tests for EPS App for TDAC Intern Project
 *-------------------------------------------------------------------*/

/*--------------------------------------------------------------------
 * Test EPS App Main - Test the EPS applications initialization process
 *-------------------------------------------------------------------*/

// Test the nominal EPS app startup (clean startup)
void Test_GENERIC_EPS_AppMain_Nominal(void) {

    // Nominal execution should call ExitApp after done
    GENERIC_EPS_AppMain();
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_ExitApp)) == 1, "CFE_ES_ExitApp() called");
}

// Test that the EPS app properly detects a failed init and sets its run status to APP_ERROR
// (unsuccessful startup)
void Test_GENERIC_EPS_AppMain_InitFailure(void){

    // Stub the EVS register to return an error code
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, CFE_EVS_INVALID_PARAMETER);

    // Run the application
    GENERIC_EPS_AppMain();

    // EPS app should have a run status of APP_ERROR"RunStatus==APP_ERROR on init failure");
    UtAssert_True(
            GENERIC_EPS_AppData.RunStatus == CFE_ES_RunStatus_APP_ERROR,
            "GENERIC_EPS_AppData.RunStatus == CFE_ES_RunStatus_APP_ERROR"
    );
}


// Verify that when the software bus pipe read fails,
// the EPS app does a clean shutdown and doesn't hang or crash
void Test_GENERIC_EPS_AppMain_PipeReadError(void){

    UT_CheckEvent_t EventTest;

    // Allow the EPS app to enter its run-loop once
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);

    // Force the first SB receive to return a pipe-read error
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 1, CFE_SB_PIPE_RD_ERR);

    // Setup to look for the PIPE_ERROR
    UT_CheckEvent_Setup(&EventTest, GENERIC_EPS_PIPE_ERR_EID, "GENERIC_EPS: SB Pipe Read Error = %d");

    // Run the app
    GENERIC_EPS_AppMain();

    // Assert we saw 1 pipe error event
    UtAssert_True(EventTest.MatchCount == 1,
                  "Expected one PIPE_ERR event on receive failure");

    // Assert a clean exit was still done after the error
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_ExitApp)) == 1,
                  "CFE_ES_ExitApp() should be called after pipe-read error");
}

/*--------------------------------------------------------------------
 * Test Command Dispatch and Packet Processing - Test the EPS applications initialization process
 *-------------------------------------------------------------------*/

// Test the nominal processing of a packet
void Test_GENERIC_EPS_ProcessCommandPacket_Valid(void)
{
    // Construct the msg id for a ground command packet
    CFE_SB_MsgId_t MsgId = CFE_SB_ValueToMsgId(GENERIC_EPS_CMD_MID);

    // Prepare a NOOP packet (simplest command)
    CFE_MSG_FcnCode_t Fcn = GENERIC_EPS_NOOP_CC;
    size_t Sz = sizeof(GENERIC_EPS_NoArgs_cmd_t);

    UT_CheckEvent_t evt;

    // Stub out the message so GetMsgId/ GetFcnCode / GetSize return our fake NOOP
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &Fcn, sizeof(Fcn), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Sz, sizeof(Sz), false);

    // Tell the harness to look for any error events
    UT_CheckEvent_Setup(&evt, GENERIC_EPS_PROCESS_CMD_ERR_EID, NULL);

    // Attempt to process the NOOP packet
    GENERIC_EPS_ProcessCommandPacket();

    // Assert that no error event was generated
    UtAssert_True(evt.MatchCount == 0, "No error event on valid command packet");
}

// Test a packet received with an invalid message ID: should give a process_cmd_error
void Test_GENERIC_EPS_ProcessCommandPacket_InvalidMID(void)
{
    UT_CheckEvent_t evt;

    // Stub GetMsgId to return a fake MID
    CFE_SB_MsgId_t badId = CFE_SB_ValueToMsgId(0xFFFF); UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &badId, sizeof(badId), false);

    // Stub a correct function code and size (isolate MID as the only discrepancy)
    CFE_MSG_FcnCode_t dummyF = GENERIC_EPS_NOOP_CC;
    size_t dummySz = sizeof(GENERIC_EPS_NoArgs_cmd_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &dummyF, sizeof(dummyF), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &dummySz, sizeof(dummySz), false);

    // Watch for the PROCESS_CMD_ERR event
    UT_CheckEvent_Setup(&evt, GENERIC_EPS_PROCESS_CMD_ERR_EID, NULL);

    // Dispatch the command
    GENERIC_EPS_ProcessCommandPacket();

    // Verify exactly one error event
    UtAssert_True(evt.MatchCount == 1, "PROCESS_CMD_ERR fired on invalid MID");
}

// Test an invalid function code: should give a cmd_err
void Test_GENERIC_EPS_ProcessGroundCommand_InvalidCode(void)
{
    UT_CheckEvent_t evt;

    // Prepare a valid mid
    CFE_SB_MsgId_t    cmdId = CFE_SB_ValueToMsgId(GENERIC_EPS_CMD_MID);

    // Give it an invalid function code
    CFE_MSG_FcnCode_t badF  = 0xFF;  // undefined CC

    // Give it a correct packet length
    size_t            sz    = sizeof(GENERIC_EPS_NoArgs_cmd_t);

    // Stub the CFE retrievals with these paramters
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &cmdId, sizeof(cmdId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &badF,sizeof(badF), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &sz, sizeof(sz), false);

    UT_CheckEvent_Setup(&evt, GENERIC_EPS_CMD_ERR_EID, NULL);

    GENERIC_EPS_ProcessGroundCommand();

    // Make sure cmd_err was sent
    UtAssert_True(evt.MatchCount == 1, "CMD_ERR fired on invalid function code");
}

// Test an invalid packet length
void Test_GENERIC_EPS_ProcessGroundCommand_BadLength(void)
{
    // Prepare a valid MID
    CFE_SB_MsgId_t cmdId = CFE_SB_ValueToMsgId(GENERIC_EPS_CMD_MID);

    // Prepare a valid function code
    CFE_MSG_FcnCode_t swCC = GENERIC_EPS_SWITCH_CC;

    // Prepare an incorrect packet length
    size_t badSz = sizeof(GENERIC_EPS_NoArgs_cmd_t);

    // Stub the parameters
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &cmdId, sizeof(cmdId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &swCC, sizeof(swCC), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &badSz, sizeof(badSz), false);

    // Set up an event hook to watch specifically for the LEN_ERR event
    UT_CheckEvent_t evt;
    UT_CheckEvent_Setup(&evt, GENERIC_EPS_LEN_ERR_EID, NULL);

    // Invoke the ground‐command processor; VerifyCmdLength should fail
    GENERIC_EPS_ProcessGroundCommand();

    // Check that exactly one LEN_ERR event was sent
    UtAssert_True(evt.MatchCount == 1, "GENERIC_EPS_LEN_ERR_EID should fire on bad packet length");
}

// Off-nominal scenario where a packet is sent with a valid MID, FID, and length, but they are mismatched
void Test_GENERIC_EPS_TelemetryRequest_MismatchedCode(void)
{
    // Set a valid MID
    CFE_SB_MsgId_t reqId = CFE_SB_ValueToMsgId(GENERIC_EPS_REQ_HK_MID);

    // Set a valid FID
    CFE_MSG_FcnCode_t fcode = GENERIC_EPS_NOOP_CC;

    // Set a valid packet length
    size_t sz = sizeof(GENERIC_EPS_NoArgs_cmd_t);

    // Stub all three calls
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId),&reqId, sizeof(reqId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode),&fcode, sizeof(fcode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &sz, sizeof(sz), false);

    // Look for a telemetry request error
    UT_CheckEvent_t evt;
    UT_CheckEvent_Setup(&evt,GENERIC_EPS_DEVICE_TLM_ERR_EID,NULL);

    GENERIC_EPS_ProcessCommandPacket();

    // Verify that exactly one DEVICE_TLM_ERR event was sent
    UtAssert_True(evt.MatchCount == 1, "GENERIC_EPS_DEVICE_TLM_ERR_EID should fire on mismatched HK request code");
}

// TEST 1: This tests that a command to turn on switch 7 goes through (even if it was a mistake)
void Test_GENERIC_EPS_ProcessGroundCommand_Switch7On(void)
{
    UT_CheckEvent_t evt;
    CFE_SB_MsgId_t    cmdMid = CFE_SB_ValueToMsgId(GENERIC_EPS_CMD_MID);
    CFE_MSG_FcnCode_t switchCc = GENERIC_EPS_SWITCH_CC;
    size_t            expectedLen = sizeof(GENERIC_EPS_Switch_cmd_t);
    GENERIC_EPS_Switch_cmd_t pkt;

    //Build a “Switch 7 → ON (0xAA)” packet in local buffer
    pkt.CmdHeader.MsgId  = GENERIC_EPS_CMD_MID;
    pkt.CmdHeader.FcnCode = GENERIC_EPS_SWITCH_CC;
    pkt.SwitchNumber      = 7;
    pkt.State             = 0xAA;

    // Stubs
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId),
                     &cmdMid, sizeof(cmdMid), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode),
                     &switchCc, sizeof(switchCc), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize),
                     &expectedLen, sizeof(expectedLen), false);

    // Look that the switch command was recieved
    UT_CheckEvent_Setup(&evt, GENERIC_EPS_CMD_SWITCH_INF_EID,
                        "GENERIC_EPS: Switch command received");

    GENERIC_EPS_ProcessGroundCommand();

    // Verify one switch event occured
    UtAssert_True(evt.MatchCount == 1,
                  "CFE_EVS_SendEvent(EPS_CMD_SWITCH_INF_EID) called once");

    /* Now—since we never actually touched the device, we can only verify
       that the HK‐telemetry counter was incremented and the switch‐count */
    UtAssert_True(GENERIC_EPS_AppData.HkTelemetryPkt.DeviceHK.Switch[7].Status == 0xAA,
                  "DeviceHK.Switch[7].Status == 0xAA");
}


/*--------------------------------------------------------------------
 * UT registration
 *-------------------------------------------------------------------*/
void Generic_eps_UT_Setup(void)   { UT_ResetState(0); }
void Generic_eps_UT_TearDown(void){}

void UtTest_Setup(void)
{
    ADD_TEST(Test_GENERIC_EPS_AppMain_Nominal);
    ADD_TEST(Test_GENERIC_EPS_AppMain_InitFailure);
    ADD_TEST(Test_GENERIC_EPS_AppMain_PipeReadError);
    ADD_TEST(Test_GENERIC_EPS_ProcessCommandPacket_Valid);
    ADD_TEST(Test_GENERIC_EPS_ProcessCommandPacket_InvalidMID);
    ADD_TEST(Test_GENERIC_EPS_ProcessGroundCommand_InvalidCode);
    ADD_TEST(Test_GENERIC_EPS_ProcessGroundCommand_BadLength);
    ADD_TEST(Test_GENERIC_EPS_TelemetryRequest_MismatchedCode);
}
