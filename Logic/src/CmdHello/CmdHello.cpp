/*******************************************************************************
 * Project:  Logic
 * @file     CmdHello.cpp
 * @brief 
 * @author   bwar
 * @date:    Feb 21, 2017
 * @note
 * Modify history:
 ******************************************************************************/
#include "CmdHello.hpp"

#ifdef __cplusplus
extern "C"
{
#endif
neb::Cmd* create()
{
    neb::Cmd* pCmd = new logic::CmdHello();
    return(pCmd);
}
#ifdef __cplusplus
}
#endif

namespace logic
{

CmdHello::CmdHello()
{
}

CmdHello::~CmdHello()
{
}

bool CmdHello::Init()
{
    return(true);
}

bool CmdHello::AnyMessage(
                const neb::tagChannelContext& stCtx,
                const MsgHead& oMsgHead, const MsgBody& oMsgBody)
{
    MsgBody oOutMsgBody;
    oOutMsgBody.set_data("Nebula: hello!");
    SendTo(stCtx, oMsgHead.cmd() + 1, oMsgHead.seq(), oOutMsgBody);
    return(true);
}

} /* namespace logic */
