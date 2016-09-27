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

struct tagNodeInfo
{
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
    uint32 uiSendNum;                       ///<
    uint32 uiRecvByte;                      ///<
    uint32 uiSendByte;                      ///<
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
    uint16 AddNode(const std::string& strNodeType, const std::string& strNodeIdentify, uint16 unWorkerNum);
    void DelNode(const std::string& strNodeType, const std::string& strNodeIdentify);

private:
    uint16 m_unLastNodeId;
    std::set<std::string> m_setIpwhite;
    std::map<std::string, std::set<std::string> > m_mapPublisher;               ///< map<node_type, set<subscribers> >
    std::set<uint16> m_setNodeId;
    std::map<std::string, std::map<std::string, tagNodeInfo> > m_mapNode;            ///< map<node_type, map<node_identify, worker_num> >
};

}

#endif /* SRC_SESSIONNODE_HPP_ */
