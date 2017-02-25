/*******************************************************************************
 * Project:  Beacon
 * @file     CmdRegister.cpp
 * @brief 
 * @author   bwar
 * @date:    Sep 19, 2016
 * @note
 * Modify history:
 ******************************************************************************/

#include <CmdNodeRegister/CmdNodeRegister.hpp>

#ifdef __cplusplus
extern "C"
{
#endif
neb::Cmd* create()
{
    neb::Cmd* pCmd = new beacon::CmdNodeRegister();
    return(pCmd);
}
#ifdef __cplusplus
}
#endif

namespace beacon
{

CmdNodeRegister::CmdNodeRegister()
    : m_pSessionNode(NULL)
{
}

CmdNodeRegister::~CmdNodeRegister()
{
}

bool CmdNodeRegister::Init()
{
    try
    {
        m_pSessionNode = new SessionNode();
    }
    catch(std::bad_alloc& e)
    {
        LOG4_ERROR("new SessionNode error: %s", e.what());
        return(false);
    }
    Register(m_pSessionNode);

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
        m_pSessionNode->AddIpwhite(oBeacon["ipwhite"](i));
    }
    for (int i = 0; i < oBeacon["node_type"].GetArraySize(); ++i)
    {
        for (int j = 0; j < oBeacon["node_type"][i]["subscribe"].GetArraySize(); ++j)
        {
            m_pSessionNode->AddSubscribe(oBeacon["node_type"][i]("node_type"), oBeacon["node_type"][i]["subscribe"](j));
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
                const neb::tagChannelContext& stCtx,
                const MsgHead& oMsgHead, const MsgBody& oMsgBody)
{
    MsgHead oOutMsgHead;
    MsgBody oOutMsgBody;
    neb::CJsonObject oNodeInfo;
    tagNodeInfo stNodeInfo;
    SendTo(stCtx, oMsgHead.cmd() + 1, oMsgHead.seq(), oOutMsgBody);
    if (oNodeInfo.Parse(oMsgBody.data()))
    {
        /*
        char szNodeIdentify[32] = {0};
        snprintf(stNodeInfo.szNodeType, sizeof(stNodeInfo.szNodeType), oNodeInfo("node_type").c_str());
        snprintf(stNodeInfo.szHost, sizeof(stNodeInfo.szHost), oNodeInfo("node_ip").c_str());
        snprintf(stNodeInfo.szGate, sizeof(stNodeInfo.szGate), oNodeInfo("gate_ip").c_str());
        stNodeInfo.ucStatus = 1;
        stNodeInfo.unWorkerNum = strtoul(oNodeInfo("worker_num").c_str(), NULL, 10);
        stNodeInfo.unNodeId = 0;
        stNodeInfo.uiHostPort = strtoul(oNodeInfo("node_port").c_str(), NULL, 10);
        stNodeInfo.uiGatePort = strtoul(oNodeInfo("gate_port").c_str(), NULL, 10);
        stNodeInfo.uiLoad = strtoul(oNodeInfo["node"]("load").c_str(), NULL, 10);
        stNodeInfo.uiConnection = strtoul(oNodeInfo["node"]("connect").c_str(), NULL, 10);
        stNodeInfo.uiRecvNum = strtoul(oNodeInfo["node"]("recv_num").c_str(), NULL, 10);
        stNodeInfo.uiSentNum = strtoul(oNodeInfo["node"]("sent_num").c_str(), NULL, 10);
        stNodeInfo.uiRecvByte = strtoul(oNodeInfo["node"]("recv_byte").c_str(), NULL, 10);
        stNodeInfo.uiSentByte = strtoul(oNodeInfo["node"]("sent_byte").c_str(), NULL, 10);
        stNodeInfo.uiClient = strtoul(oNodeInfo["node"]("client").c_str(), NULL, 10);
        stNodeInfo.ullActiveTime = GetNowTime();
        snprintf(szNodeIdentify, sizeof(szNodeIdentify), "%s:%u", stNodeInfo.szHost, stNodeInfo.uiHostPort);
        */
        uint16 unNodeId = m_pSessionNode->AddNode(oNodeInfo);
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
