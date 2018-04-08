/*******************************************************************************
 * Project:  Beacon
 * @file     SessionNode.cpp
 * @brief 
 * @author   bwar
 * @date:    Sep 20, 2016
 * @note
 * Modify history:
 ******************************************************************************/
#include "Definition.hpp"
#include "SessionNode.hpp"

namespace beacon
{

SessionNode::SessionNode()
    : neb::Session(1, neb::gc_dNoTimeout, "beacon::SessionNode"),
      m_unLastNodeId(0)
{
}

SessionNode::~SessionNode()
{
}

neb::E_CMD_STATUS SessionNode::Timeout()
{
    return(neb::CMD_STATUS_RUNNING);
}

void SessionNode::AddIpwhite(const std::string& strIpwhite)
{
    m_setIpwhite.insert(strIpwhite);
}

void SessionNode::AddSubscribe(const std::string& strNodeType, const std::string& strSubscribeNodeType)
{
    auto pub_iter = m_mapPublisher.find(strSubscribeNodeType);
    if (pub_iter == m_mapPublisher.end())
    {
        std::unordered_set<std::string> setSubscriber;
        setSubscriber.insert(strNodeType);
        m_mapPublisher.insert(std::make_pair(strSubscribeNodeType, setSubscriber));
    }
    else
    {
        pub_iter->second.insert(strNodeType);
    }
}

uint16 SessionNode::AddNode(const neb::CJsonObject& oNodeInfo)
{
    LOG4_TRACE("%s", __FUNCTION__);
    uint32 uiNodeId = 0;
    std::unordered_set<uint16>::iterator node_id_iter;
    std::string strNodeIdentify = oNodeInfo("node_ip") + std::string(":") + oNodeInfo("node_port");
    neb::CJsonObject oNodeInfoWithNodeId = oNodeInfo;
    oNodeInfo.Get("node_id", uiNodeId);

    // find node_id or create node_id
    auto identify_node_iter = m_mapIdentifyNodeId.find(strNodeIdentify);
    if (identify_node_iter == m_mapIdentifyNodeId.end())
    {
        while (0 == uiNodeId)
        {
            uiNodeId = ++m_unLastNodeId;
            node_id_iter = m_setNodeId.find(uiNodeId);
            if (node_id_iter != m_setNodeId.end())
            {
                uiNodeId = 0;
            }
            if (m_setNodeId.size() >= 65535)
            {
                LOG4_ERROR("there is no valid node_id in the system!");
                return(0);
            }
        }
        m_mapIdentifyNodeId.insert(std::make_pair(strNodeIdentify, uiNodeId));
    }
    else
    {
        uiNodeId = identify_node_iter->second;
    }
    oNodeInfoWithNodeId.Replace("node_id", uiNodeId);


    auto node_type_iter = m_mapNode.find(oNodeInfoWithNodeId("node_type"));
    if (node_type_iter == m_mapNode.end())
    {
        std::unordered_map<std::string, neb::CJsonObject> mapNodeInfo;
        mapNodeInfo.insert(std::make_pair(strNodeIdentify, oNodeInfoWithNodeId));
        m_mapNode.insert(std::make_pair(oNodeInfoWithNodeId("node_type"), mapNodeInfo));
        m_mapIdentifyNodeType.insert(std::make_pair(strNodeIdentify, oNodeInfoWithNodeId("node_type")));
        m_setNodeId.insert(uiNodeId);
        AddNodeBroadcast(oNodeInfoWithNodeId);
        return(uiNodeId);
    }
    else
    {
        auto node_iter = node_type_iter->second.find(strNodeIdentify);
        if (node_iter == node_type_iter->second.end())
        {
            node_type_iter->second.insert(std::make_pair(strNodeIdentify, oNodeInfoWithNodeId));
            m_setNodeId.insert(uiNodeId);
            m_mapIdentifyNodeType.insert(std::make_pair(strNodeIdentify, oNodeInfoWithNodeId("node_type")));
            AddNodeBroadcast(oNodeInfoWithNodeId);
            return(uiNodeId);
        }
        else
        {
            node_iter->second = oNodeInfoWithNodeId;
            m_setNodeId.insert(uiNodeId);
            return(uiNodeId);
        }
    }
}

void SessionNode::RemoveNode(const std::string& strNodeIdentify)
{
    LOG4_TRACE("%s", __FUNCTION__);
    auto identity_node_iter = m_mapIdentifyNodeType.find(strNodeIdentify);
    if (identity_node_iter != m_mapIdentifyNodeType.end())
    {
        auto node_type_iter = m_mapNode.find(identity_node_iter->second);
        if (node_type_iter != m_mapNode.end())
        {
            auto node_iter = node_type_iter->second.find(strNodeIdentify);
            if (node_iter != node_type_iter->second.end())
            {
                uint32 uiNodeId = 0;
                node_iter->second.Get("node_id", uiNodeId);
                RemoveNodeBroadcast(node_iter->second);
                m_setNodeId.erase(uiNodeId);
                m_mapIdentifyNodeType.erase(strNodeIdentify);
                node_type_iter->second.erase(node_iter);
            }
        }
    }
}

void SessionNode::AddNodeBroadcast(const neb::CJsonObject& oNodeInfo)
{
    LOG4_TRACE("%s", __FUNCTION__);
    std::unordered_map<std::string, std::unordered_map<std::string, neb::CJsonObject> >::iterator node_list_iter;
    std::unordered_map<std::string, neb::CJsonObject>::iterator node_iter;
    std::unordered_set<std::string>::iterator node_type_iter;
    neb::CJsonObject oSubcribeNodeInfo;
    neb::CJsonObject oAddNodes;
    neb::CJsonObject oAddedNodeInfo = oNodeInfo;
    oAddedNodeInfo.Delete("node");
    oAddedNodeInfo.Delete("worker");
    oAddNodes.Add("add_nodes", neb::CJsonObject("[]"));
    oAddNodes["add_nodes"].Add(oAddedNodeInfo);
    oSubcribeNodeInfo.Add("add_nodes", neb::CJsonObject("[]"));
    for (auto sub_iter = m_mapPublisher.begin();
                    sub_iter != m_mapPublisher.end(); ++sub_iter)
    {
        for (node_type_iter = sub_iter->second.begin(); node_type_iter != sub_iter->second.end(); ++node_type_iter)
        {
            /* send this node info to subscriber */
            if (sub_iter->first == oNodeInfo("node_type"))
            {
                node_list_iter = m_mapNode.find(*node_type_iter);
                if (node_list_iter != m_mapNode.end())
                {
                    LOG4_TRACE("m_mapNode[%s].size() = %u", node_type_iter->c_str(), node_list_iter->second.size());
                    try
                    {
                        for (node_iter = node_list_iter->second.begin(); node_iter != node_list_iter->second.end(); ++node_iter)
                        {
                            if (node_iter->second("node_id") == oNodeInfo("node_id"))
                            {
                                continue;
                            }

                            MsgBody oMsgBody;
                            oMsgBody.set_data(oAddNodes.ToString());
                            neb::Step* pStep = NewStep("neb::StepNodeBroadcast", node_iter->first, neb::CMD_REQ_NODE_REG_NOTICE, oMsgBody);
                            pStep->Emit();
                        }
                    }
                    catch(std::bad_alloc& e)
                    {
                        LOG4_ERROR("new StepNodeBroadcast error: %s", e.what());
                    }
                }
            }

            /* make subscribe node info */
            if ((*node_type_iter) == oNodeInfo("node_type"))
            {
                node_list_iter = m_mapNode.find(sub_iter->first);
                if (node_list_iter != m_mapNode.end())
                {
                    for (node_iter = node_list_iter->second.begin(); node_iter != node_list_iter->second.end(); ++node_iter)
                    {
                        neb::CJsonObject oExistNodeInfo = node_iter->second;
                        oExistNodeInfo.Delete("node");
                        oExistNodeInfo.Delete("worker");
                        oSubcribeNodeInfo["add_nodes"].Add(oExistNodeInfo);
                    }
                }
            }

        }
    }

    /* send subscribe node info to this node */
    if (oSubcribeNodeInfo["add_nodes"].GetArraySize() > 0)
    {
        try
        {
            MsgBody oMsgBody;
            char szThisNodeIdentity[32] = {0};
            oMsgBody.set_data(oSubcribeNodeInfo.ToString());
            snprintf(szThisNodeIdentity, sizeof(szThisNodeIdentity),
                            "%s:%s", oNodeInfo("node_ip").c_str(), oNodeInfo("node_port").c_str());
            neb::Step* pStep = NewStep("neb::StepNodeBroadcast", std::string(szThisNodeIdentity), neb::CMD_REQ_NODE_REG_NOTICE, oMsgBody);
            pStep->Emit();
        }
        catch(std::bad_alloc& e)
        {
            LOG4_ERROR("new StepNodeBroadcast error: %s", e.what());
        }
    }
}

void SessionNode::RemoveNodeBroadcast(const neb::CJsonObject& oNodeInfo)
{
    LOG4_TRACE("%s", __FUNCTION__);
    std::unordered_map<std::string, std::unordered_map<std::string, neb::CJsonObject> >::iterator node_list_iter;
    std::unordered_map<std::string, neb::CJsonObject>::iterator node_iter;
    std::unordered_set<std::string>::iterator node_type_iter;
    neb::CJsonObject oDelNodes;
    neb::CJsonObject oDeletedNodeInfo = oNodeInfo;
    oDeletedNodeInfo.Delete("node");
    oDeletedNodeInfo.Delete("worker");
    oDelNodes.Add("del_nodes", neb::CJsonObject("[]"));
    oDelNodes["del_nodes"].Add(oDeletedNodeInfo);
    for (auto sub_iter = m_mapPublisher.begin();
                    sub_iter != m_mapPublisher.end(); ++sub_iter)
    {
        for (node_type_iter = sub_iter->second.begin(); node_type_iter != sub_iter->second.end(); ++node_type_iter)
        {
            /* send this node info to subscriber */
            if (sub_iter->first == oNodeInfo("node_type"))
            {
                node_list_iter = m_mapNode.find(*node_type_iter);
                if (node_list_iter != m_mapNode.end())
                {
                    for (node_iter = node_list_iter->second.begin(); node_iter != node_list_iter->second.end(); ++node_iter)
                    {
                        MsgBody oMsgBody;
                        oMsgBody.set_data(oDelNodes.ToString());
                        neb::Step* pStep = NewStep("neb::StepNodeBroadcast", node_iter->first, neb::CMD_REQ_NODE_REG_NOTICE, oMsgBody);
                        pStep->Emit();
                    }
                }
            }
        }
    }
}

tagNodeInfo::tagNodeInfo()
    : ucStatus(0), unWorkerNum(0), unNodeId(0), uiHostPort(0), uiGatePort(0),
      uiLoad(0), uiConnection(0), uiRecvNum(0), uiSentNum(0), uiRecvByte(0), uiSentByte(0),
      uiClient(0), ullActiveTime(0)
{
    memset(szNodeType, 0, sizeof(szNodeType));
    memset(szHost, 0, sizeof(szHost));
    memset(szGate, 0, sizeof(szGate));
}

tagNodeInfo::tagNodeInfo(const tagNodeInfo& stNode)
    : ucStatus(stNode.ucStatus), unWorkerNum(stNode.unWorkerNum),
      unNodeId(stNode.unNodeId), uiHostPort(stNode.uiHostPort), uiGatePort(stNode.uiGatePort),
      uiLoad(stNode.uiLoad), uiConnection(stNode.uiConnection),
      uiRecvNum(stNode.uiRecvNum), uiSentNum(stNode.uiSentNum), uiRecvByte(stNode.uiRecvByte), uiSentByte(stNode.uiSentByte),
      uiClient(stNode.uiClient), ullActiveTime(stNode.ullActiveTime)
{
    snprintf(szNodeType, sizeof(szNodeType), stNode.szNodeType);
    snprintf(szHost, sizeof(szHost), stNode.szHost);
    snprintf(szGate, sizeof(szGate), stNode.szGate);
}

tagNodeInfo& tagNodeInfo::operator=(const tagNodeInfo& stNode)
{
    snprintf(szNodeType, sizeof(szNodeType), stNode.szNodeType);
    snprintf(szHost, sizeof(szHost), stNode.szHost);
    snprintf(szGate, sizeof(szGate), stNode.szGate);
    ucStatus = stNode.ucStatus;
    unWorkerNum = stNode.unWorkerNum;
    unNodeId = stNode.unNodeId;
    uiHostPort = stNode.uiHostPort;
    uiGatePort = stNode.uiGatePort;
    uiLoad = stNode.uiLoad;
    uiConnection = stNode.uiConnection;
    uiRecvNum = stNode.uiRecvNum;
    uiSentNum = stNode.uiSentNum;
    uiRecvByte = stNode.uiRecvByte;
    uiSentByte = stNode.uiSentByte;
    uiClient = stNode.uiClient;
    ullActiveTime = stNode.ullActiveTime;
    return(*this);
}

}
