/*******************************************************************************
 * Project:  Beacon
 * @file     CmdNodeReport.cpp
 * @brief 
 * @author   bwar
 * @date:    Feb 14, 2017
 * @note
 * Modify history:
 ******************************************************************************/
#include "CmdNodeReport.hpp"

namespace beacon
{

CmdNodeReport::CmdNodeReport(int32 iCmd)
    : neb::Cmd(iCmd), m_pSessionOnlineNodes(nullptr)
{
}

CmdNodeReport::~CmdNodeReport()
{
}

bool CmdNodeReport::Init()
{
    return(true);
}

bool CmdNodeReport::AnyMessage(std::shared_ptr<neb::SocketChannel> pUpstreamChannel,
                const MsgHead& oMsgHead,
                const MsgBody& oMsgBody)
{
    MsgHead oOutMsgHead;
    MsgBody oOutMsgBody;
    neb::CJsonObject oNodeInfo;
    if (nullptr == m_pSessionOnlineNodes)
    {
        m_pSessionOnlineNodes = std::dynamic_pointer_cast<SessionOnlineNodes>(GetSession(1, "beacon::SessionOnlineNodes"));
        if (nullptr == m_pSessionOnlineNodes)
        {
            LOG4_ERROR("no session node found!");
        }
    }
    if (oNodeInfo.Parse(oMsgBody.data()))
    {
        uint16 unNodeId = m_pSessionOnlineNodes->AddNode(oNodeInfo);
        if (0 == unNodeId)
        {
            oOutMsgBody.mutable_rsp_result()->set_code(neb::ERR_NODE_NUM);
            oOutMsgBody.mutable_rsp_result()->set_msg("there is no valid node_id in the system!");
        }
        else
        {
            neb::CJsonObject oNodeId;
            oNodeId.Add("node_id", unNodeId);
            oOutMsgBody.set_data(oNodeId.ToString());
            oOutMsgBody.mutable_rsp_result()->set_code(neb::ERR_OK);
            oOutMsgBody.mutable_rsp_result()->set_msg("OK");
        }
    }
    else
    {
        LOG4_ERROR("failed to parse node info json from MsgBody.data()!");
        oOutMsgBody.mutable_rsp_result()->set_code(neb::ERR_BODY_JSON);
        oOutMsgBody.mutable_rsp_result()->set_msg("failed to parse node info json from MsgBody.data()!");
    }
    return(SendTo(pUpstreamChannel, oMsgHead.cmd() + 1, oMsgHead.seq(), oOutMsgBody));
}

} /* namespace beacon */
