/*******************************************************************************
 * Project:  Beacon
 * @file     CmdNodeDisconnect.cpp
 * @brief 
 * @author   bwar
 * @date:    Feb 14, 2017
 * @note
 * Modify history:
 ******************************************************************************/
#include "CmdNodeDisconnect.hpp"

namespace beacon
{

CmdNodeDisconnect::CmdNodeDisconnect(int32 iCmd)
    : neb::Cmd(iCmd), m_pSessionNode(nullptr)
{
}

CmdNodeDisconnect::~CmdNodeDisconnect()
{
}

bool CmdNodeDisconnect::Init()
{
    return(true);
}

bool CmdNodeDisconnect::AnyMessage(const neb::tagChannelContext& stCtx,
                const MsgHead& oMsgHead,
                const MsgBody& oMsgBody)
{
    neb::CJsonObject oNodeInfo;
    tagNodeInfo stNodeInfo;
    if (NULL == m_pSessionNode)
    {
        m_pSessionNode = std::dynamic_pointer_cast<SessionNode>(GetSession(1, "beacon::SessionNode"));
        if (NULL == m_pSessionNode)
        {
            LOG4_ERROR("no session node found!");
        }
    }
    if (oNodeInfo.Parse(oMsgBody.data()))
    {
        LOG4_DEBUG("%s disconnect, remove from node list.", oMsgBody.data().c_str());
        m_pSessionNode->RemoveNode(oMsgBody.data());
    }
    else
    {
        LOG4_ERROR("failed to parse node info json from MsgBody.data()!");
    }
    return(true);
}

} /* namespace beacon */
