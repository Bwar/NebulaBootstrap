/*******************************************************************************
 * Project:  Beacon
 * @file     SessionNode.cpp
 * @brief 
 * @author   bwar
 * @date:    Sep 20, 2016
 * @note
 * Modify history:
 ******************************************************************************/
#include "SessionNode.hpp"

namespace beacon
{

SessionNode::SessionNode()
    : neb::Session(1, 0.0, "beacon::SessionNode"),
      m_unLastNodeId(0)
{
}

SessionNode::~SessionNode()
{
}

void SessionNode::AddIpwhite(const std::string& strIpwhite)
{
    m_setIpwhite.insert(strIpwhite);
}

void SessionNode::AddSubscribe(const std::string& strNodeType, const std::string& strSubscribeNodeType)
{
    std::map<std::string, std::set<std::string> >::iterator pub_iter = m_mapPublisher.find(strSubscribeNodeType);
    if (pub_iter == m_mapPublisher.end())
    {
        std::set<std::string> setSubscriber;
        setSubscriber.insert(strNodeType);
        m_mapPublisher.insert(std::make_pair(strSubscribeNodeType, setSubscriber));
    }
    else
    {
        pub_iter->second.insert(strNodeType);
    }
}

uint16 SessionNode::AddNode(const std::string& strNodeType, const std::string& strNodeIdentify, uint16 unWorkerNum)
{
    std::set<uint16>::iterator node_id_iter;
    std::map<std::string, std::map<std::string, tagNodeInfo> >::iterator node_type_iter = m_mapNode.find(strNodeType);
    if (node_type_iter == m_mapNode.end())
    {
        std::map<std::string, tagNodeInfo> mapNodeWorker;
        tagNodeInfo stNodeInfo;
        stNodeInfo.unWorkerNum = unWorkerNum;
        do
        {
            stNodeInfo.unNodeId = ++m_unLastNodeId;
            node_id_iter = m_setNodeId.find(stNodeInfo.unNodeId);
            if (m_setNodeId.size() >= 65535)
            {
                LOG4_ERROR("the node number was too large! for the system");
                return(0);
            }
        }
        while (node_id_iter != m_setNodeId.end() || 0 == stNodeInfo.unNodeId);

        mapNodeWorker.insert(std::make_pair(strNodeIdentify, stNodeInfo));
        m_mapNode.insert(std::make_pair(strNodeType, mapNodeWorker));
        m_setNodeId.insert(stNodeInfo.unNodeId);
        return(stNodeInfo.unNodeId);
    }
    else
    {
        std::map<std::string, tagNodeInfo>::iterator node_iter = node_type_iter->second.find(strNodeIdentify);
        if (node_iter == node_type_iter->second.end())
        {
            tagNodeInfo stNodeInfo;
            stNodeInfo.unWorkerNum = unWorkerNum;
            do
            {
                stNodeInfo.unNodeId = ++m_unLastNodeId;
                node_id_iter = m_setNodeId.find(stNodeInfo.unNodeId);
                if (m_setNodeId.size() >= 65535)
                {
                    LOG4_ERROR("the node number was too large! for the system");
                    return(0);
                }
            }
            while (node_id_iter != m_setNodeId.end() || 0 == stNodeInfo.unNodeId);

            node_type_iter->second.insert(std::make_pair(strNodeIdentify, stNodeInfo));
            m_setNodeId.insert(stNodeInfo.unNodeId);
            return(stNodeInfo.unNodeId);
        }
        else
        {
            node_iter->second.unWorkerNum = unWorkerNum;
            m_setNodeId.insert(node_iter->second.unNodeId);
            return(node_iter->second.unNodeId);
        }
    }
}

void SessionNode::DelNode(const std::string& strNodeType, const std::string& strNodeIdentify)
{
    std::map<std::string, std::map<std::string, tagNodeInfo> >::iterator node_type_iter = m_mapNode.find(strNodeType);
    if (node_type_iter != m_mapNode.end())
    {
        std::map<std::string, tagNodeInfo>::iterator node_iter = node_type_iter->second.find(strNodeIdentify);
        if (node_iter != node_type_iter->second.end())
        {
            m_setNodeId.erase(node_iter->second.unNodeId);
            node_type_iter->second.erase(node_iter);
        }
    }
}

tagNodeInfo::tagNodeInfo()
    : ucStatus(0), unWorkerNum(0), unNodeId(0), uiHostPort(0), uiGatePort(0),
      uiLoad(0), uiConnection(0), uiRecvNum(0), uiSendNum(0), uiRecvByte(0), uiSendByte(0),
      uiClient(0), ullActiveTime(0)
{
    memset(szHost, 0, sizeof(szHost));
    memset(szGate, 0, sizeof(szGate));
}

tagNodeInfo::tagNodeInfo(const tagNodeInfo& stNode)
    : ucStatus(stNode.ucStatus), unWorkerNum(stNode.unWorkerNum),
      unNodeId(stNode.unNodeId), uiHostPort(stNode.uiHostPort), uiGatePort(stNode.uiGatePort),
      uiLoad(stNode.uiLoad), uiConnection(stNode.uiConnection),
      uiRecvNum(stNode.uiRecvNum), uiSendNum(stNode.uiSendNum), uiRecvByte(stNode.uiRecvByte), uiSendByte(stNode.uiSendByte),
      uiClient(stNode.uiClient), ullActiveTime(stNode.ullActiveTime)
{
    snprintf(szHost, sizeof(szHost), stNode.szHost);
    snprintf(szGate, sizeof(szGate), stNode.szGate);
}

tagNodeInfo& tagNodeInfo::operator=(const tagNodeInfo& stNode)
{
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
    uiSendNum = stNode.uiSendNum;
    uiRecvByte = stNode.uiRecvByte;
    uiSendByte = stNode.uiSendByte;
    uiClient = stNode.uiClient;
    ullActiveTime = stNode.ullActiveTime;
    return(*this);
}

}
