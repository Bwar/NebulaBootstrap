/*******************************************************************************
 * Project:  Beacon
 * @file     CmdRegister.cpp
 * @brief 
 * @author   bwar
 * @date:    Sep 19, 2016
 * @note
 * Modify history:
 ******************************************************************************/

#include "CmdNodeRegister.hpp"

namespace beacon
{

CmdNodeRegister::CmdNodeRegister(int32 iCmd)
    : neb::Cmd(iCmd), m_pSessionOnlineNodes(nullptr)
{
}

CmdNodeRegister::~CmdNodeRegister()
{
}

bool CmdNodeRegister::Init()
{
    m_pSessionOnlineNodes = std::dynamic_pointer_cast<SessionOnlineNodes>(MakeSharedSession("beacon::SessionOnlineNodes"));

    neb::CJsonObject oBeaconConf = GetCustomConf();
    if (std::string("db_config") == oBeaconConf("config_choice"))
    {
        return(InitFromDb(oBeaconConf["db_config"]));
    }
    else if (std::string("local_config") == oBeaconConf("config_choice"))
    {
        return(InitFromLocal(oBeaconConf["local_config"]));
    }
    else
    {
        LOG4_ERROR("invalid config!");
        return(false);
    }
}

bool CmdNodeRegister::InitFromDb(const neb::CJsonObject& oDbConf)
{
    return(true);
}

bool CmdNodeRegister::InitFromLocal(const neb::CJsonObject& oLocalConf)
{
    neb::CJsonObject oBeacon = oLocalConf;
    for (int i = 0; i < oBeacon["ipwhite"].GetArraySize(); ++i)
    {
        m_pSessionOnlineNodes->AddIpwhite(oBeacon["ipwhite"](i));
    }
    for (int i = 0; i < oBeacon["node_type"].GetArraySize(); ++i)
    {
        for (int j = 0; j < oBeacon["node_type"][i]["subscribe"].GetArraySize(); ++j)
        {
            m_pSessionOnlineNodes->AddSubscribe(oBeacon["node_type"][i]("node_type"), oBeacon["node_type"][i]["subscribe"](j));
        }
    }
    return(true);
}

/**
 * @brief report node status
 * @note node info：
 * {
 *     "node_type":"ACCESS",
 *     "node_ip":"192.168.11.12",
 *     "node_port":9988,
 *     "gate_ip":"120.234.2.106",
 *     "gate_port":10001,
 *     "node_id":0,
 *     "worker_num":10,
 *     "active_time":16879561651.06,
 *     "node":{
 *         "load":1885792, "connect":495873, "recv_num":98755266, "recv_byte":98856648832, "sent_num":154846322, "sent_byte":648469320222,"client":495870
 *     },
 *     "worker":
 *     [
 *          {"load":655666, "connect":495873, "recv_num":98755266, "recv_byte":98856648832, "sent_num":154846322, "sent_byte":648469320222,"client":195870}},
 *          {"load":655235, "connect":485872, "recv_num":98755266, "recv_byte":98856648832, "sent_num":154846322, "sent_byte":648469320222,"client":195870}},
 *          {"load":585696, "connect":415379, "recv_num":98755266, "recv_byte":98856648832, "sent_num":154846322, "sent_byte":648469320222,"client":195870}}
 *     ]
 * }
 */

bool CmdNodeRegister::AnyMessage(
                std::shared_ptr<neb::SocketChannel> pUpstreamChannel,
                const MsgHead& oMsgHead, const MsgBody& oMsgBody)
{
    MsgHead oOutMsgHead;
    MsgBody oOutMsgBody;
    neb::CJsonObject oNodeInfo;
    SendTo(pUpstreamChannel, oMsgHead.cmd() + 1, oMsgHead.seq(), oOutMsgBody);
    if (oNodeInfo.Parse(oMsgBody.data()))
    {
        uint16 unNodeId = m_pSessionOnlineNodes->AddNode(oNodeInfo);
        if (0 == unNodeId)
        {
            oOutMsgBody.mutable_rsp_result()->set_code(neb::ERR_NODE_NUM);
            oOutMsgBody.mutable_rsp_result()->set_msg("there is no valid node_id in the system!");
            return(false);
        }
        else
        {
            neb::CJsonObject oNodeId;
            oNodeId.Add("node_id", unNodeId);
            oOutMsgBody.set_data(oNodeId.ToString());
            oOutMsgBody.mutable_rsp_result()->set_code(neb::ERR_OK);
            oOutMsgBody.mutable_rsp_result()->set_msg("OK");
            return(true);
        }
    }
    else
    {
        LOG4_ERROR("failed to parse node info json from MsgBody.data()!");
        oOutMsgBody.mutable_rsp_result()->set_code(neb::ERR_BODY_JSON);
        oOutMsgBody.mutable_rsp_result()->set_msg("failed to parse node info json from MsgBody.data()!");
        return(false);
    }
}

} /* namespace beacon */
