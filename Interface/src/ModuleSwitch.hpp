/*******************************************************************************
 * Project:  InterfaceServer
 * @file     ModuleSwitch.hpp
 * @brief    Http请求转换器
 * @author   lbh
 * @date:    2016年7月6日
 * @note     将http请求转换成内部协议到Logic服务器进行处理
 * Modify history:
 ******************************************************************************/
#ifndef SRC_MODULESWITCH_MODULESWITCH_HPP_
#define SRC_MODULESWITCH_MODULESWITCH_HPP_

#include <actor/cmd/Module.hpp>
#include <string>
#include <map>
#include <Error.hpp>
#include <util/json/CJsonObject.hpp>
#include "StepSwitch.hpp"

namespace inter
{

class ModuleSwitch: public neb::Module
{
public:
    ModuleSwitch(const std::string& strModulePath);
    virtual ~ModuleSwitch();

    bool Init();

    virtual bool AnyMessage(
                    const neb::tagChannelContext& stCtx,
                    const HttpMsg& oHttpMsg);

    virtual std::string ObjectName() const
    {
        return("inter::ModuleSwitch");
    }

protected:
    void Response(const neb::tagChannelContext& stMsgShell,
                    const HttpMsg& oInHttpMsg,
                    int iErrno, const std::string& strErrMsg);

private:
    std::map<std::string, neb::CJsonObject*> m_mapModuleConf;

public:
    StepSwitch* pStepSwitch;
};

} /* namespace inter */

#endif /* SRC_MODULESWITCH_MODULESWITCH_HPP_ */
