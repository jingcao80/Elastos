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

#ifndef __ELASTOS_DROID_NET_CDNSPINGER_H__
#define __ELASTOS_DROID_NET_CDNSPINGER_H__

#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_Net_CDnsPinger.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::SystemClock;

using Elastos::Core::IInteger32;
using Elastos::Net::IDatagramSocket;
using Elastos::Net::IInetAddress;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IList;
using Elastos::Utility::IRandom;

namespace Elastos {
namespace Droid {
namespace Net {

CarClass(CDnsPinger)
    , public Handler
    , public IDnsPinger
{
private:
    class ActivePing
        : public Object
    {
    public:
        ActivePing()
            : mInternalId(0)
            , mPacketId(0)
            , mTimeout(0)
            , mResult(0)
            , mStart(SystemClock::GetElapsedRealtime())
        {}

    public:
        AutoPtr<IDatagramSocket> mSocket;
        Int32 mInternalId;
        Int16 mPacketId;
        Int32 mTimeout;
        AutoPtr<IInteger32> mResult;
        Int64 mStart;
    };

    /* Message argument for ACTION_PING_DNS */
    class DnsArg
        : public Object
    {
    public:
        DnsArg(
          /* [in] */ IInetAddress* dns,
          /* [in] */ Int32 seq);

    public:
        AutoPtr<IInetAddress> mDns;
        Int32 mSeq;
    };

public:
    TO_STRING_IMPL("CDnsPinger")

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CDnsPinger();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ const String& tagstr,
        /* [in] */ ILooper* looper,
        /* [in] */ IHandler* target,
        /* [in] */ Int32 connectionType);

    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    void HandleActionPingDNS(
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ DnsArg* dnsArg);

    void HandleActionListenForResponse(
        /* [in] */ Int32 arg1);

    void HandleActionCancelAllPings();

    /**
     * Returns a list of DNS addresses, coming from either the link properties of the
     * specified connection or the default system DNS if the link properties has no dnses.
     * @return a non-empty non-null list
     */
    CARAPI GetDnsList(
        /* [out] */ IList** dnslist);

    /**
     * Send a ping.  The response will come via a {@link #DNS_PING_RESULT} to the handler
     * specified at creation.
     * @param dns address of dns server to ping
     * @param timeout timeout for ping
     * @return an ID field, which will also be included in the {@link #DNS_PING_RESULT} message.
     */
    CARAPI PingDnsAsync(
        /* [in] */ IInetAddress* dns,
        /* [in] */ Int32 timeout,
        /* [in] */ Int32 delay,
        /* [out] */ Int32* result);

    CARAPI CancelPings();

private:
    CARAPI SendResponse(
        /* [in] */ Int32 internalId,
        /* [in] */ Int32 externalId,
        /* [in] */ Int32 responseVal);

    CARAPI GetCurrentLinkProperties(
        /* [out] */ ILinkProperties** result);

    CARAPI GetDefaultDns(
        /* [out] */ IInetAddress** result);

    CARAPI Log(
        /* [in] */ const char* fmt, ...);

    CARAPI Loge(
        /* [in] */ const char* fmt, ...);

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) InitDnsQuery();

private:
    static const Boolean DBG;

    static const Int32 RECEIVE_POLL_INTERVAL_MS;
    static const Int32 DNS_PORT;

    /** Short socket timeout so we don't block one any 'receive' call */
    static const Int32 SOCKET_TIMEOUT_MS;

    /** Used to generate IDs */
    static const AutoPtr<IRandom> sRandom;
    static const AutoPtr<IAtomicInteger32> sCounter;

    AutoPtr<IConnectivityManager> mConnectivityManager;
    AutoPtr<IContext> mContext;
    Int32 mConnectionType;
    AutoPtr<IHandler> mTarget;
    AutoPtr<IList> mDefaultDns;
    String TAG;

    //Invalidates old dns requests upon a cancel
    AutoPtr<IAtomicInteger32> mCurrentToken;

    static const Int32 BASE;

    /**
     * Send a new ping via a socket.  arg1 is ID, arg2 is timeout, obj is InetAddress to ping
     */
    static const Int32 ACTION_PING_DNS;
    static const Int32 ACTION_LISTEN_FOR_RESPONSE;
    static const Int32 ACTION_CANCEL_ALL_PINGS;

    AutoPtr<IList> mActivePings;
    Int32 mEventCounter;

    static /* const */ AutoPtr<ArrayOf<Byte> > mDnsQuery;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_NET_CDNSPINGER_H__
