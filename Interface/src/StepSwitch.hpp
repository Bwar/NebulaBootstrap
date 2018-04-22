/*******************************************************************************
 * Project:  InterfaceServer
 * @file     StepSwitch.hpp
 * @brief    将http请求转换成内部协议到Logic服务器进行处理
 * @author   lbh
 * @date:    2016年7月6日
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef SRC_MODULESWITCH_STEPSWITCH_HPP_
#define SRC_MODULESWITCH_STEPSWITCH_HPP_

#include <actor/step/PbStep.hpp>
#include "util/json/CJsonObject.hpp"

namespace inter
{

class StepSwitch: public neb::PbStep, public neb::DynamicCreator<StepSwitch, std::shared_ptr<neb::SocketChannel>, HttpMsg, neb::CJsonObject*>
{
public:
    StepSwitch(std::shared_ptr<neb::SocketChannel> pUpstreamChannel, const HttpMsg& oInHttpMsg, const neb::CJsonObject* pModuleConf);
    virtual ~StepSwitch();

    virtual neb::E_CMD_STATUS Emit(int iErrno, const std::string& strErrMsg = "",  void* data = NULL);

    virtual neb::E_CMD_STATUS Callback(
                    std::shared_ptr<neb::SocketChannel> pUpstreamChannel,
                    const MsgHead& oInMsgHead,
                    const MsgBody& oInMsgBody,
                    void* data = NULL);

    virtual neb::E_CMD_STATUS Timeout();

    virtual std::string ObjectName() const
    {
        return("inter::StepSwitch");
    }

protected:
    virtual neb::E_CMD_STATUS EmitSwitch();
    virtual neb::E_CMD_STATUS CallbackSwitch(std::shared_ptr<neb::SocketChannel> pUpstreamChannel,
                        const MsgHead& oInMsgHead, const MsgBody& oInMsgBody);

    void Response(int iErrno, const std::string& strErrMsg, const std::string& strErrClientShow);

private:
    std::shared_ptr<neb::SocketChannel> m_pRequestUpstreamChannel;
    HttpMsg m_oInHttpMsg;
    neb::CJsonObject m_oModuleConf;
    MsgBody m_oSwitchMsgBody;
};

} /* namespace im */

#endif /* SRC_MODULESWITCH_STEPSWITCH_HPP_ */
