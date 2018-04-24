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
#include <SessionOnlineNodes.hpp>
#include <util/json/CJsonObject.hpp>

namespace beacon
{

class CmdNodeRegister: public neb::Cmd, public neb::DynamicCreator<CmdNodeRegister, int32>
{
public:
    CmdNodeRegister(int32 iCmd);
    virtual ~CmdNodeRegister();

    virtual bool Init();
    virtual bool AnyMessage(
                    std::shared_ptr<neb::SocketChannel> pUpstreamChannel,
                    const MsgHead& oMsgHead,
                    const MsgBody& oMsgBody);
protected:
    bool InitFromDb(const neb::CJsonObject& oDbConf);
    bool InitFromLocal(const neb::CJsonObject& oLocalConf);

private:
    std::shared_ptr<SessionOnlineNodes> m_pSessionOnlineNodes;
};

} /* namespace beacon */

#endif /* SRC_CMDNODEREGISTER_CMDNODEREGISTER_HPP_ */
