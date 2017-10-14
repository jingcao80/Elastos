//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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

    using AsyncChannel::ConnectSync;

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
        /* [in] */ const char *fmt, ...);

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
