/*******************************************************************************
 * Project:  Beacon
 * @file     CmdNodeDisconnect.hpp
 * @brief 
 * @author   bwar
 * @date:    Feb 14, 2017
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef SRC_CMDNODEDISCONNECT_CMDNODEDISCONNECT_HPP_
#define SRC_CMDNODEDISCONNECT_CMDNODEDISCONNECT_HPP_

#include <actor/cmd/Cmd.hpp>
#include <Error.hpp>
#include <SessionOnlineNodes.hpp>
#include <util/json/CJsonObject.hpp>

namespace beacon
{

class CmdNodeDisconnect: public neb::Cmd, public neb::DynamicCreator<CmdNodeDisconnect, int32>
{
public:
    CmdNodeDisconnect(int32 iCmd);
    virtual ~CmdNodeDisconnect();

    virtual bool Init();
    virtual bool AnyMessage(
                    std::shared_ptr<neb::SocketChannel> pUpstreamChannel,
                    const MsgHead& oMsgHead,
                    const MsgBody& oMsgBody);

    virtual std::string ObjectName() const
    {
        return("beacon::CmdNodeReport");
    }

private:
    std::shared_ptr<SessionOnlineNodes> m_pSessionOnlineNodes;
};

} /* namespace beacon */

#endif /* SRC_CMDNODEDISCONNECT_CMDNODEDISCONNECT_HPP_ */
