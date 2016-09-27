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

bool CmdNodeRegister::AnyMessage(
                const neb::tagChannelContext& stCtx,
                const MsgHead& oMsgHead, const MsgBody& oMsgBody)
{
    MsgHead oOutMsgHead;
    MsgBody oOutMsgBody;
    neb::CJsonObject oNodeInfo;
    tagNodeInfo stNodeInfo;
    oOutMsgHead.set_cmd(oMsgHead.cmd() + 1);
    oOutMsgHead.set_seq(oMsgHead.seq());
    if (oNodeInfo.Parse(oMsgBody.data()))
    {
    }
    else
    {
        LOG4_ERROR("failed to parse node info json from MsgBody.data()!");
        oOutMsgBody.mutable_rsp_result()->set_code(neb::ERR_BODY_JSON);
        oOutMsgBody.mutable_rsp_result()->set_msg("failed to parse node info json from MsgBody.data()!");
    }
    return(true);
}

} /* namespace beacon */
