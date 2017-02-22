/*******************************************************************************
 * Project:  Logic
 * @file     CmdHello.hpp
 * @brief 
 * @author   bwar
 * @date:    Feb 21, 2017
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef SRC_CMDHELLO_CMDHELLO_HPP_
#define SRC_CMDHELLO_CMDHELLO_HPP_

#include <object/cmd/Cmd.hpp>

#ifdef __cplusplus
extern "C" {
#endif
    neb::Cmd* create();
#ifdef __cplusplus
}
#endif

namespace logic
{

class CmdHello: public neb::Cmd
{
public:
    CmdHello();
    virtual ~CmdHello();

    virtual bool Init();
    virtual bool AnyMessage(
                    const neb::tagChannelContext& stCtx,
                    const MsgHead& oMsgHead,
                    const MsgBody& oMsgBody);

    virtual std::string ObjectName() const
    {
        return("logic::CmdHello");
    }
};

} /* namespace logic */

#endif /* SRC_CMDHELLO_CMDHELLO_HPP_ */
