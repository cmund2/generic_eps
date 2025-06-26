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
    uint16      ExpectedEvent;
    uint32      MatchCount;
    const char *ExpectedFormat;
} UT_CheckEvent_t;

static int32 UT_CheckEvent_Hook(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                const UT_StubContext_t *Context, va_list va)
{
    UT_CheckEvent_t *State = UserObj;
    uint16           EventId;
    if (Context->ArgCount > 0 &&
        (EventId = UT_Hook_GetArgValueByName(Context, "EventID", uint16)) == State->ExpectedEvent)
    {
        if (!State->ExpectedFormat ||
            strcmp(UT_Hook_GetArgValueByName(Context, "Spec", const char *),
        State->ExpectedFormat) == 0)
        {
            ++State->MatchCount;
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

/*--------------------------------------------------------------------
 * Test GENERIC_EPS_AppMain()
 *-------------------------------------------------------------------*/
void Test_GENERIC_EPS_AppMain(void)
{
    /* 1) Nominal execution should call ExitApp */
    GENERIC_EPS_AppMain();
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_ExitApp)) == 1,
                  "CFE_ES_ExitApp() called");

    /* 2) Force init-failure by stubbing EVS_Register */
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, CFE_EVS_INVALID_PARAMETER);
    GENERIC_EPS_AppMain();
    UtAssert_True(GENERIC_EPS_AppData.RunStatus == CFE_ES_RunStatus_APP_ERROR,
                  "RunStatus==APP_ERROR on init failure");

    /* 3) Exercise loop + receive error */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId),
                     &(CFE_SB_MsgId_t){GENERIC_EPS_CMD_MID},
                     sizeof(CFE_SB_MsgId_t), false);
    GENERIC_EPS_AppMain();
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_SB_ReceiveBuffer)) == 1,
                  "Called CFE_SB_ReceiveBuffer");

    /* Now fail the pipe read */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 1, CFE_SB_PIPE_RD_ERR);
    {
        UT_CheckEvent_t EventTest;
        UT_CheckEvent_Setup(&EventTest, GENERIC_EPS_PIPE_ERR_EID,
                            "GENERIC_EPS: SB Pipe Read Error = %d");
        GENERIC_EPS_AppMain();
        UtAssert_True(EventTest.MatchCount == 1,
                      "PIPE_ERR event generated");
    }
}

/*--------------------------------------------------------------------
 * Test GENERIC_EPS_AppInit()
 *-------------------------------------------------------------------*/
void Test_GENERIC_EPS_AppInit(void)
{
    /* Nominal success */
    UT_TEST_FUNCTION_RC(GENERIC_EPS_AppInit(), CFE_SUCCESS);

    /* EVS_Register failure */
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, CFE_EVS_INVALID_PARAMETER);
    UT_TEST_FUNCTION_RC(GENERIC_EPS_AppInit(), CFE_EVS_INVALID_PARAMETER);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) >= 1,
                  "Logged syslog on EVS_Register fail");

    /* Pipe creation failure */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_CreatePipe), 1, CFE_SB_BAD_ARGUMENT);
    UT_TEST_FUNCTION_RC(GENERIC_EPS_AppInit(), CFE_SB_BAD_ARGUMENT);

    /* Subscription failure */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 1, CFE_SB_BAD_ARGUMENT);
    UT_TEST_FUNCTION_RC(GENERIC_EPS_AppInit(), CFE_SB_BAD_ARGUMENT);
}

/*--------------------------------------------------------------------
 * Test packet processing & command dispatch
 *-------------------------------------------------------------------*/
void Test_GENERIC_EPS_ProcessCommandPacket(void)
{
    union {
        CFE_SB_Buffer_t          SBBuf;
        GENERIC_EPS_NoArgs_cmd_t Noop;
    } TestMsg;
    CFE_SB_MsgId_t    MsgId = CFE_SB_ValueToMsgId(GENERIC_EPS_CMD_MID);
    CFE_MSG_FcnCode_t Code  = GENERIC_EPS_NOOP_CC;
    size_t            Sz    = sizeof(TestMsg.Noop);
    UT_CheckEvent_t   EventTest;

    memset(&TestMsg,0,sizeof(TestMsg));
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &Code, sizeof(Code),   false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize),    &Sz,   sizeof(Sz),     false);

    /* Valid command => no error event */
    UT_CheckEvent_Setup(&EventTest, GENERIC_EPS_PROCESS_CMD_ERR_EID, NULL);
    GENERIC_EPS_ProcessCommandPacket();
    UtAssert_True(EventTest.MatchCount == 0,
                  "No PROCESS_CMD_ERR for valid packet");

    /* Invalid MID => error event */
    MsgId = CFE_SB_INVALID_MSG_ID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    GENERIC_EPS_ProcessCommandPacket();
    UtAssert_True(EventTest.MatchCount == 1,
                  "PROCESS_CMD_ERR for invalid packet");
}

void Test_GENERIC_EPS_ProcessGroundCommand(void)
{
    CFE_SB_MsgId_t    MsgId = CFE_SB_ValueToMsgId(GENERIC_EPS_CMD_MID);
    CFE_MSG_FcnCode_t Code;
    size_t            Sz;
    UT_CheckEvent_t   EventTest;

    /* NOOP dispatch */
    Code = GENERIC_EPS_NOOP_CC; Sz = sizeof(GENERIC_EPS_NoArgs_cmd_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId),     &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode),   &Code,  sizeof(Code),  false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize),      &Sz,    sizeof(Sz),    false);
    UT_CheckEvent_Setup(&EventTest, GENERIC_EPS_CMD_NOOP_INF_EID, NULL);
    GENERIC_EPS_ProcessGroundCommand();
    UtAssert_True(EventTest.MatchCount == 1, "NOOP event");

    /* SWITCH length‐mismatch */
    Code = GENERIC_EPS_SWITCH_CC; Sz = sizeof(GENERIC_EPS_NoArgs_cmd_t);
    UT_SetDeferredRetcode(UT_KEY(CFE_MSG_GetSize), 1, Sz);
    UT_CheckEvent_Setup(&EventTest, GENERIC_EPS_LEN_ERR_EID, NULL);
    GENERIC_EPS_ProcessGroundCommand();
    UtAssert_True(EventTest.MatchCount > 0, "LEN_ERR on bad SWITCH size");

    /* Bad code */
    Code = 0xFF;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &Code, sizeof(Code), false);
    UT_CheckEvent_Setup(&EventTest, GENERIC_EPS_CMD_ERR_EID, NULL);
    GENERIC_EPS_ProcessGroundCommand();
    UtAssert_True(EventTest.MatchCount > 0, "CMD_ERR on invalid code");
}

/*--------------------------------------------------------------------
 * Test GENERIC_EPS_ReportHousekeeping()
 *-------------------------------------------------------------------*/
void Test_GENERIC_EPS_ReportHousekeeping(void)
{
    CFE_MSG_Message_t *MsgSend, *MsgTS;
    CFE_SB_MsgId_t     ReqId = CFE_SB_ValueToMsgId(GENERIC_EPS_REQ_HK_MID);
    UT_CheckEvent_t    EventTest;

    /* Normal path */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId),     &ReqId, sizeof(ReqId), false);
    UT_SetDataBuffer(UT_KEY(CFE_SB_TransmitMsg),   &MsgSend, sizeof(MsgSend), false);
    UT_SetDataBuffer(UT_KEY(CFE_SB_TimeStampMsg),  &MsgTS,   sizeof(MsgTS),   false);
    GENERIC_EPS_ReportHousekeeping();
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_SB_TransmitMsg))==1,
                  "TransmitMsg called");
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_SB_TimeStampMsg))==1,
                  "TimeStampMsg called");

    /* Inject device‐read failure */
    UT_SetDeferredRetcode(UT_KEY(GENERIC_EPS_RequestHK), 1, OS_ERROR);
    UT_CheckEvent_Setup(&EventTest, GENERIC_EPS_REQ_HK_ERR_EID,
                        "GENERIC_EPS: Request device HK reported error %d");
    GENERIC_EPS_ReportHousekeeping();
    UtAssert_True(EventTest.MatchCount > 0, "REQ_HK_ERR event");
}

/*--------------------------------------------------------------------
 * Test GENERIC_EPS_VerifyCmdLength()
 *-------------------------------------------------------------------*/
void Test_GENERIC_EPS_VerifyCmdLength(void)
{
    UT_CheckEvent_t EventTest;
    size_t size = 1;
    CFE_SB_MsgId_t mid  = CFE_SB_ValueToMsgId(GENERIC_EPS_CMD_MID);
    CFE_MSG_FcnCode_t code = GENERIC_EPS_NOOP_CC;

    /* Correct size => no event */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &size, sizeof(size), false);
    UT_CheckEvent_Setup(&EventTest, GENERIC_EPS_LEN_ERR_EID, NULL);
    GENERIC_EPS_VerifyCmdLength(NULL, size);
    UtAssert_True(EventTest.MatchCount == 0, "No LEN_ERR on correct size");

    /* Mismatch => LEN_ERR */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId),   &mid,  sizeof(mid),  false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &code, sizeof(code), false);
    UT_CheckEvent_Setup(&EventTest, GENERIC_EPS_LEN_ERR_EID, NULL);
    GENERIC_EPS_VerifyCmdLength(NULL, size+1);
    UtAssert_True(EventTest.MatchCount > 0, "LEN_ERR on incorrect size");
}

/*--------------------------------------------------------------------
 * UT registration
 *-------------------------------------------------------------------*/
void Generic_eps_UT_Setup(void)   { UT_ResetState(0); }
void Generic_eps_UT_TearDown(void){}

void UtTest_Setup(void)
{
    ADD_TEST(GENERIC_EPS_AppMain);
    ADD_TEST(GENERIC_EPS_AppInit);
    ADD_TEST(GENERIC_EPS_ProcessCommandPacket);
    ADD_TEST(GENERIC_EPS_ProcessGroundCommand);
    ADD_TEST(GENERIC_EPS_ReportHousekeeping);
    ADD_TEST(GENERIC_EPS_VerifyCmdLength);
}
