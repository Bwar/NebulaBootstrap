/*******************************************************************************
 * Project:  Beacon
 * @file     StepNodeBroadcast.hpp
 * @brief
 * @author   bwar
 * @date:    Dec 28, 2016
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef SRC_CMDREGISTER_STEPNODEBROADCAST_HPP_
#define SRC_CMDREGISTER_STEPNODEBROADCAST_HPP_

#include <actor/step/PbStep.hpp>

namespace beacon
{

class StepNodeBroadcast: public neb::PbStep, public neb::DynamicCreator<StepNodeBroadcast, std::string, int32, MsgBody>
{
public:
public:
    StepNodeBroadcast(const std::string& strNodeIdentity, int32 iCmd, const MsgBody& oMsgBody);
    virtual ~StepNodeBroadcast();

    virtual neb::E_CMD_STATUS Emit(
            int iErrno = 0,
            const std::string& strErrMsg = "",
            void* data = NULL);

    virtual neb::E_CMD_STATUS Callback(
            std::shared_ptr<neb::SocketChannel> pUpstreamChannel,
            const MsgHead& oInMsgHead,
            const MsgBody& oInMsgBody,
            void* data = NULL);

    virtual neb::E_CMD_STATUS Timeout();

private:
    std::string m_strTargetNodeIdentity;
    int32 m_iCmd;
    MsgBody m_oMsgBody;
};

} /* namespace beacon */

#endif /* SRC_CMDREGISTER_STEPNODEBROADCAST_HPP_ */
