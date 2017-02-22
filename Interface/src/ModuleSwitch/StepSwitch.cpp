/*******************************************************************************
 * Project:  InterfaceServer
 * @file     StepSwitch.cpp
 * @brief 
 * @author   lbh
 * @date:    2016年7月6日
 * @note
 * Modify history:
 ******************************************************************************/
#include <util/UnixTime.hpp>
#include <util/http/http_parser.h>
#include "StepSwitch.hpp"

namespace inter
{

StepSwitch::StepSwitch(const neb::tagChannelContext& stCtx,
                const HttpMsg& oInHttpMsg, const neb::CJsonObject* pModuleConf, uint32 uiImid)
    : m_stCtx(stCtx), m_oInHttpMsg(oInHttpMsg), m_oModuleConf(pModuleConf)
{
}

StepSwitch::~StepSwitch()
{
}

neb::E_CMD_STATUS StepSwitch::Emit(int iErrno, const std::string& strErrMsg, void* data)
{
    LOG4_DEBUG("%s()", __FUNCTION__);
    if (neb::ERR_OK != iErrno)
    {
        LOG4_ERROR("error %d: %s!", iErrno, strErrMsg.c_str());
        Response(iErrno, strErrMsg, strErrMsg);
        return(neb::CMD_STATUS_FAULT);
    }

    return(EmitSwitch());
}

neb::E_CMD_STATUS StepSwitch::Callback(
        const neb::tagChannelContext& stCtx,
        const MsgHead& oInMsgHead,
        const MsgBody& oInMsgBody,
        void* data)
{
    LOG4_TRACE("%s()", __FUNCTION__);
    return(CallbackSwitch(stCtx, oInMsgHead, oInMsgBody));
}

neb::E_CMD_STATUS StepSwitch::Timeout()
{
    LOG4_DEBUG("%s()", __FUNCTION__);
    return(neb::CMD_STATUS_FAULT);
}

neb::E_CMD_STATUS StepSwitch::EmitSwitch()
{
    LOG4_DEBUG("%s", __FUNCTION__);
    uint32 uiCmd = 0;
    m_oModuleConf.Get("cmd", uiCmd);
    m_oSwitchMsgBody.set_data(m_oInHttpMsg.body());

    if (m_oSwitchMsgBody.has_req_target())
    {
        if (m_oSwitchMsgBody.req_target().route_id() > 0)
        {
            SendToWithMod("LOGIC", m_oSwitchMsgBody.req_target().route_id(), uiCmd, GetSequence(), m_oSwitchMsgBody);
        }
        else if (m_oSwitchMsgBody.req_target().route().size() > 0)
        {
            // TODO add a hash function
            SendToWithMod("LOGIC", m_oSwitchMsgBody.req_target().route().size(), uiCmd, GetSequence(), m_oSwitchMsgBody);
        }
        else
        {
            SendToNext("LOGIC", uiCmd, GetSequence(), m_oSwitchMsgBody);
        }
    }
    else
    {
        SendToNext("LOGIC", uiCmd, GetSequence(), m_oSwitchMsgBody);
    }
    return (neb::CMD_STATUS_RUNNING);
}

neb::E_CMD_STATUS StepSwitch::CallbackSwitch(
                const neb::tagChannelContext& stCtx, const MsgHead& oInMsgHead, const MsgBody& oInMsgBody)
{
    LOG4_DEBUG("%s()", __FUNCTION__);
    HttpMsg oHttpMsg;
    oHttpMsg.set_type(HTTP_RESPONSE);
    oHttpMsg.set_status_code(200);
    oHttpMsg.set_http_major(m_oInHttpMsg.http_major());
    oHttpMsg.set_http_minor(m_oInHttpMsg.http_minor());
    oHttpMsg.set_body(oInMsgBody.data());
    SendTo(m_stCtx, oHttpMsg);
    return (neb::CMD_STATUS_COMPLETED);
}

void StepSwitch::Response(int iErrno, const std::string& strErrMsg, const std::string& strErrClientShow)
{
    LOG4_TRACE("%s()", __FUNCTION__);
    HttpMsg oHttpMsg;
    neb::CJsonObject oResponseData;
    oHttpMsg.set_type(HTTP_RESPONSE);
    oHttpMsg.set_status_code(200);
    oHttpMsg.set_http_major(m_oInHttpMsg.http_major());
    oHttpMsg.set_http_minor(m_oInHttpMsg.http_minor());
    oResponseData.Add("code", iErrno);
    oResponseData.Add("msg", strErrClientShow);
    if (iErrno == 0)
    {
        oResponseData.Add("data", neb::CJsonObject("[]"));
    }
    oHttpMsg.set_body(oResponseData.ToFormattedString());
    SendTo(m_stCtx, oHttpMsg);
}

} /* namespace im */
