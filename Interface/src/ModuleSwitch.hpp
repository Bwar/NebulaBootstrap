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

#include <string>
#include <map>
#include <Error.hpp>
#include <util/json/CJsonObject.hpp>
#include <actor/cmd/Module.hpp>
#include "StepSwitch.hpp"

namespace inter
{

class ModuleSwitch: public neb::Module, public neb::DynamicCreator<ModuleSwitch, std::string>
{
public:
    ModuleSwitch(const std::string& strModulePath);
    virtual ~ModuleSwitch();

    bool Init();

    virtual bool AnyMessage(
                    std::shared_ptr<neb::SocketChannel> pUpstreamChannel,
                    const HttpMsg& oHttpMsg);
protected:
    void Response(std::shared_ptr<neb::SocketChannel> pUpstreamChannel,
                    const HttpMsg& oInHttpMsg,
                    int iErrno, const std::string& strErrMsg);

private:
    std::map<std::string, neb::CJsonObject*> m_mapModuleConf;

public:
    std::shared_ptr<StepSwitch> pStepSwitch;
};

} /* namespace inter */

#endif /* SRC_MODULESWITCH_MODULESWITCH_HPP_ */
