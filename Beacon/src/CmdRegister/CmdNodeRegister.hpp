/*******************************************************************************
 * Project:  Beacon
 * @file     CmdRegister.hpp
 * @brief 
 * @author   bwar
 * @date:    Sep 19, 2016
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef SRC_CMDREGISTER_CMDNODEREGISTER_HPP_
#define SRC_CMDREGISTER_CMDNODEREGISTER_HPP_

#include <Error.hpp>
#include <util/json/CJsonObject.hpp>
#include <object/cmd/Cmd.hpp>
#include "SessionNode.hpp"

namespace beacon
{

class CmdNodeRegister: public neb::Cmd
{
public:
    CmdNodeRegister();
    virtual ~CmdNodeRegister();

    virtual bool Init();
    virtual bool AnyMessage(
                    const neb::tagChannelContext& stCtx,
                    const MsgHead& oMsgHead,
                    const MsgBody& oMsgBody);

    virtual std::string ObjectName() const
    {
        return("beacon::CmdNodeRegister");
    }

protected:
    bool InitFromDb(const neb::CJsonObject& oDbConf);
    bool InitFromLocal(const neb::CJsonObject& oLocalConf);

private:
    SessionNode* m_pSessionNode;
};

} /* namespace beacon */

#endif /* SRC_CMDREGISTER_CMDNODEREGISTER_HPP_ */
