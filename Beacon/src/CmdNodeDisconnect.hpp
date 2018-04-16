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
#include <util/json/CJsonObject.hpp>
#include "SessionNode.hpp"

namespace beacon
{

class CmdNodeDisconnect: public neb::Cmd, public neb::DynamicCreator<CmdNodeDisconnect, int32>
{
public:
    CmdNodeDisconnect(int32 iCmd);
    virtual ~CmdNodeDisconnect();

    virtual bool Init();
    virtual bool AnyMessage(
                    const neb::tagChannelContext& stCtx,
                    const MsgHead& oMsgHead,
                    const MsgBody& oMsgBody);

    virtual std::string ObjectName() const
    {
        return("beacon::CmdNodeReport");
    }

private:
    std::shared_ptr<SessionNode> m_pSessionNode;
};

} /* namespace beacon */

#endif /* SRC_CMDNODEDISCONNECT_CMDNODEDISCONNECT_HPP_ */
