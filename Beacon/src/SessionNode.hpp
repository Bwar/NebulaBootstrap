/*******************************************************************************
 * Project:  Beacon
 * @file     SessionNode.hpp
 * @brief 
 * @author   bwar
 * @date:    Sep 20, 2016
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef SRC_SESSIONNODE_HPP_
#define SRC_SESSIONNODE_HPP_

#include <string.h>
#include <string>
#include <set>
#include <map>
#include <util/json/CJsonObject.hpp>
#include <object/session/Session.hpp>

namespace beacon
{

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
struct tagNodeInfo
{
    char szNodeType[32];                    ///< node type
    char szHost[32];                        ///< ip address binded for inner communication
    char szGate[32];                        ///< ip address binded for outer communication
    uint8 ucStatus;                         ///< status: 0 offline,  1 online
    uint16 unWorkerNum;                     ///< worker process number
    uint16 unNodeId;                        ///< node id
    uint32 uiHostPort;                      ///<
    uint32 uiGatePort;                      ///<
    uint32 uiLoad;                          ///<
    uint32 uiConnection;                    ///< connection number
    uint32 uiRecvNum;                       ///<
    uint32 uiSentNum;                       ///<
    uint32 uiRecvByte;                      ///<
    uint32 uiSentByte;                      ///<
    uint32 uiClient;                        ///< client number, part of connection
    uint64 ullActiveTime;                   ///<

    tagNodeInfo();
    tagNodeInfo(const tagNodeInfo& stNode);
    tagNodeInfo& operator=(const tagNodeInfo& stNode);
};

class SessionNode: public neb::Session
{
public:
    SessionNode();
    virtual ~SessionNode();

    virtual neb::E_CMD_STATUS Timeout();

    virtual std::string ObjectName() const
    {
        return("beacon::SessionNode");
    }

public:
    void AddIpwhite(const std::string& strIpwhite);
    void AddSubscribe(const std::string& strNodeType, const std::string& strSubscribeNodeType);

    /**
     * @return node_id
     */
    uint16 AddNode(const neb::CJsonObject& oNodeInfo);
    void RemoveNode(const std::string& strNodeIdentify);

protected:
    void AddNodeBroadcast(const neb::CJsonObject& oNodeInfo);
    void RemoveNodeBroadcast(const neb::CJsonObject& oNodeInfo);

private:
    uint16 m_unLastNodeId;
    std::set<std::string> m_setIpwhite;
    std::map<std::string, std::set<std::string> > m_mapPublisher;               ///< map<node_type, set<subscribers_node_type> >
    std::set<uint16> m_setNodeId;
    std::map<std::string, uint16> m_mapIdentifyNodeId;                          ///< map<node_identify, node_id>
    std::map<std::string, std::string> m_mapIdentifyNodeType;                   ///< map<node_Identify，node_type>
    std::map<std::string, std::map<std::string, neb::CJsonObject> > m_mapNode;  ///< map<node_type, map<node_identify, neb::CJsonObject> >
};

}

#endif /* SRC_SESSIONNODE_HPP_ */
