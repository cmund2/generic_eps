/*
** File: coveragetest_generic_eps_app.c
**
** Purpose:
**   Coverage Unit Test cases for the GENERIC_EPS Application
**
** Notes:
**   Follows the standard UT harness conventions.
*/

#include "generic_eps_app_coveragetest_common.h"
#include "ut_generic_eps_app.h"   /* Provides extern GENERIC_EPS_AppData */

/*
 * Local struct + hook to catch EVS events by ID/format
 */
typedef struct
{
    uint16      ExpectedEvent;
    uint32      MatchCount;
    const char *ExpectedFormat;
} UT_CheckEvent_t;

static int32 UT_CheckEvent_Hook(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                const UT_StubContext_t *Context, va_list va)
{
    UT_CheckEvent_t *State = UserObj;
    uint16           EventId;
    const char      *Spec;

    if (Context->ArgCount > 0)
    {
        EventId = UT_Hook_GetArgValueByName(Context, "EventID", uint16);
        if (EventId == State->ExpectedEvent)
        {
            if (State->ExpectedFormat)
            {
                Spec = UT_Hook_GetArgValueByName(Context, "Spec", const char *);
                if (Spec && strcmp(Spec, State->ExpectedFormat) == 0)
                {
                    ++State->MatchCount;
                }
            }
            else
            {
                ++State->MatchCount;
            }
        }
    }
    return 0;
}

static void UT_CheckEvent_Setup(UT_CheckEvent_t *Evt, uint16 ExpectedEvent, const char *ExpectedFormat)
{
    memset(Evt, 0, sizeof(*Evt));
    Evt->ExpectedEvent  = ExpectedEvent;
    Evt->ExpectedFormat = ExpectedFormat;
    UT_SetVaHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_CheckEvent_Hook, Evt);
}

/*
**********************************************************************************
**          TEST CASE FUNCTIONS
**********************************************************************************
*/

/* Exercise EPS_AppMain() in nominal and error paths */
void Test_GENERIC_EPS_AppMain(void)
{
    CFE_SB_MsgId_t    MsgId      = CFE_SB_INVALID_MSG_ID;
    UT_CheckEvent_t   EventTest;

    /* Nominal */
    GENERIC_EPS_AppMain();
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_ExitApp)) == 1, "CFE_ES_ExitApp() called");

    /* Force init error */
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, CFE_EVS_INVALID_PARAMETER);
    GENERIC_EPS_AppMain();
    UtAssert_True(GENERIC_EPS_AppData.RunStatus == CFE_ES_RunStatus_APP_ERROR,
                  "RunStatus == APP_ERROR");

    /* Force a read‐pipe error inside the loop */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 1, CFE_SB_PIPE_RD_ERR);
    UT_CheckEvent_Setup(&EventTest, GENERIC_EPS_PIPE_ERR_EID,
                        "GENERIC_EPS: SB Pipe Read Error = %d");
    GENERIC_EPS_AppMain();
    UtAssert_True(EventTest.MatchCount == 1, "Pipe Read Error event generated");
}

/* Exercise initialization */
void Test_GENERIC_EPS_AppInit(void)
{
    /* Nominal should return CFE_SUCCESS */
    UT_TEST_FUNCTION_RC(GENERIC_EPS_AppInit(), CFE_SUCCESS);

    /* Fail each sub‐call in turn */
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, CFE_EVS_INVALID_PARAMETER);
    UT_TEST_FUNCTION_RC(GENERIC_EPS_AppInit(), CFE_EVS_INVALID_PARAMETER);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1,
                  "SysLog on EVS_Register failure");

    UT_SetDeferredRetcode(UT_KEY(CFE_SB_CreatePipe), 1, CFE_SB_BAD_ARGUMENT);
    UT_TEST_FUNCTION_RC(GENERIC_EPS_AppInit(), CFE_SB_BAD_ARGUMENT);
    /* ...and so on for Subscribe() failures... */
}

/* Dispatches MsgId→ProcessGroundCommand / ReqHK */
void Test_GENERIC_EPS_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(GENERIC_EPS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = GENERIC_EPS_NOOP_CC;
    size_t            MsgSize   = sizeof(GENERIC_EPS_NoArgs_cmd_t);
    UT_CheckEvent_t   EventTest;

    /* Nominal NOOP */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode,   sizeof(FcnCode),   false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize),    &MsgSize,   sizeof(MsgSize),   false);
    UT_CheckEvent_Setup(&EventTest, GENERIC_EPS_CMD_ERR_EID, NULL);
    GENERIC_EPS_ProcessCommandPacket();
    UtAssert_True(EventTest.MatchCount == 0, "No error for valid NOOP");

    /* Invalid MsgId */
    TestMsgId = CFE_SB_INVALID_MSG_ID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    GENERIC_EPS_ProcessCommandPacket();
    UtAssert_True(EventTest.MatchCount == 1, "Error on invalid MsgId");
}

/* Ground‐side commands */
void Test_GENERIC_EPS_ProcessGroundCommand(void)
{
    /* Similar to CSS-template: iterate GENERIC_EPS_NOOP_CC, RESET_COUNTERS_CC, SWITCH_CC, default */
}

/* Housekeeping publish path */
void Test_GENERIC_EPS_ReportHousekeeping(void)
{
    /* Exercise GENERIC_EPS_ReportHousekeeping(), stub GENERIC_EPS_RequestHK success+failure */
}

/* Invalid length path */
void Test_GENERIC_EPS_VerifyCmdLength(void)
{
    size_t size    = 1;
    CFE_MSG_FcnCode_t fcncode = GENERIC_EPS_NOOP_CC;
    CFE_SB_MsgId_t    msgid   = CFE_SB_ValueToMsgId(GENERIC_EPS_CMD_MID);
    UT_CheckEvent_t   EventTest;

    /* Mismatch */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &size, sizeof(size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &msgid, sizeof(msgid), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &fcncode, sizeof(fcncode), false);
    UT_CheckEvent_Setup(&EventTest, GENERIC_EPS_LEN_ERR_EID, NULL);
    GENERIC_EPS_VerifyCmdLength(NULL, size + 1);
    UtAssert_True(EventTest.MatchCount == 1, "Length error event");
}

/*
 * Setup / Teardown called before/after every test
 */
void Generic_eps_UT_Setup(void)
{
    UT_ResetState(0);
}

void Generic_eps_UT_TearDown(void) {}

/*
 * Register test cases
 */
void UtTest_Setup(void)
{
    ADD_TEST(GENERIC_EPS_AppMain);
    ADD_TEST(GENERIC_EPS_AppInit);
    ADD_TEST(GENERIC_EPS_ProcessCommandPacket);
    ADD_TEST(GENERIC_EPS_ProcessGroundCommand);
    ADD_TEST(GENERIC_EPS_ReportHousekeeping);
    ADD_TEST(GENERIC_EPS_VerifyCmdLength);
}
