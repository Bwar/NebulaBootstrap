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
#include <SessionNodesHolder.hpp>
#include <util/json/CJsonObject.hpp>

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
                    std::shared_ptr<neb::SocketChannel> pUpstreamChannel,
                    const MsgHead& oMsgHead,
                    const MsgBody& oMsgBody);

    virtual std::string ObjectName() const
    {
        return("beacon::CmdNodeReport");
    }

private:
    std::shared_ptr<SessionNodesHolder> m_pSessionNodesHolder;
};

} /* namespace beacon */

#endif /* SRC_CMDNODEREPORT_CMDNODEREPORT_HPP_ */
