
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCSWITCHASYNCCHANNEL_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCSWITCHASYNCCHANNEL_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/internal/utility/AsyncChannel.h>

using Elastos::Droid::Internal::Utility::AsyncChannel;
using Elastos::Droid::Os::IMessage;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

class DcSwitchAsyncChannel
    : public AsyncChannel
    , public IDcSwitchAsyncChannel
{
public:
    CAR_INTERFACE_DECL()

    DcSwitchAsyncChannel();

    static CARAPI CmdToString(
        /* [in] */ Int32 cmd,
        /* [out] */ String* result);

    CARAPI constructor(
        /* [in] */ IDcSwitchState* dcSwitchState,
        /* [in] */ Int32 id);

    CARAPI ReqConnect(
        /* [in] */ const String& type);

    CARAPI RspConnect(
        /* [in] */ IMessage* response,
        /* [out] */ Int32* result);

    CARAPI ConnectSync(
        /* [in] */ const String& type,
        /* [out] */ Int32* result);

    CARAPI ReqDisconnect(
        /* [in] */ const String& type);

    CARAPI RspDisconnect(
        /* [in] */ IMessage* response,
        /* [out] */ Int32* result);

    CARAPI DisconnectSync(
        /* [in] */ const String& type,
        /* [out] */ Int32* result);

    CARAPI ReqIsIdle();

    CARAPI RspIsIdle(
        /* [in] */ IMessage* response,
        /* [out] */ Boolean* result);

    CARAPI IsIdleSync(
        /* [out] */ Boolean* result);

    CARAPI ReqIsIdleOrDeacting();

    CARAPI RspIsIdleOrDeacting(
        /* [in] */ IMessage* response,
        /* [out] */ Boolean* result);

    CARAPI IsIdleOrDeactingSync(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

private:
    CARAPI Log(
        /* [in] */ const String& s);

    static CARAPI_(AutoPtr<ArrayOf<String> >) InitCmdToString();

public:
    static const Int32 REQ_CONNECT;

    static const Int32 RSP_CONNECT;

    static const Int32 REQ_DISCONNECT;

    static const Int32 RSP_DISCONNECT;

    static const Int32 REQ_IS_IDLE_STATE;

    static const Int32 RSP_IS_IDLE_STATE;

    static const Int32 REQ_IS_IDLE_OR_DEACTING_STATE;

    static const Int32 RSP_IS_IDLE_OR_DEACTING_STATE;

private:
    static const Boolean DBG;

    static const Boolean VDBG;

    static const String LOG__TAG;

    Int32 mTagId;

    AutoPtr<IDcSwitchState> mDcSwitchState;

    // ***** Event codes for driving the state machine
    static const Int32 BASE;

    static const Int32 CMD_TO_STRING_COUNT;

    static AutoPtr<ArrayOf<String> > sCmdToString;
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCSWITCHASYNCCHANNEL_H__
