/*******************************************************************************
 * Project:  Beacon
 * @file     CmdNodeReport.hpp
 * @brief 
 * @author   bwar
 * @date:    Feb 14, 2017
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef SRC_CMDNODEREPORT_CMDNODEREPORT_HPP_
#define SRC_CMDNODEREPORT_CMDNODEREPORT_HPP_

#include <actor/cmd/Cmd.hpp>
#include <Error.hpp>
#include <util/json/CJsonObject.hpp>
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

class CmdNodeReport: public neb::Cmd, public neb::DynamicCreator<CmdNodeReport, int32>
{
public:
    CmdNodeReport(int32 iCmd);
    virtual ~CmdNodeReport();

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

#endif /* SRC_CMDNODEREPORT_CMDNODEREPORT_HPP_ */
