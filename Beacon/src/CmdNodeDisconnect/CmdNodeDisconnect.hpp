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

#include <Error.hpp>
#include <util/json/CJsonObject.hpp>
#include <object/cmd/Cmd.hpp>
#include "SessionNode.hpp"

#ifdef __cplusplus
extern "C" {
#endif
    neb::Cmd* create();
#ifdef __cplusplus
}
#endif

namespace beacon
{

class CmdNodeDisconnect: public neb::Cmd
{
public:
    CmdNodeDisconnect();
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
    SessionNode* m_pSessionNode;
};

} /* namespace beacon */

#endif /* SRC_CMDNODEDISCONNECT_CMDNODEDISCONNECT_HPP_ */
