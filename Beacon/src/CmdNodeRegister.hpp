/*******************************************************************************
 * Project:  Beacon
 * @file     CmdRegister.hpp
 * @brief    节点注册
 * @author   bwar
 * @date:    Sep 19, 2016
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef SRC_CMDNODEREGISTER_HPP_
#define SRC_CMDNODEREGISTER_HPP_

#include <actor/cmd/Cmd.hpp>
#include <Error.hpp>
#include <util/json/CJsonObject.hpp>
#include "SessionNode.hpp"

namespace beacon
{

class CmdNodeRegister: public neb::Cmd, public neb::DynamicCreator<CmdNodeRegister, int32>
{
public:
    CmdNodeRegister(int32 iCmd);
    virtual ~CmdNodeRegister();

    virtual bool Init();
    virtual bool AnyMessage(
                    const neb::tagChannelContext& stCtx,
                    const MsgHead& oMsgHead,
                    const MsgBody& oMsgBody);
protected:
    bool InitFromDb(const neb::CJsonObject& oDbConf);
    bool InitFromLocal(const neb::CJsonObject& oLocalConf);

private:
    SessionNode* m_pSessionNode;
};

} /* namespace beacon */

#endif /* SRC_CMDNODEREGISTER_CMDNODEREGISTER_HPP_ */