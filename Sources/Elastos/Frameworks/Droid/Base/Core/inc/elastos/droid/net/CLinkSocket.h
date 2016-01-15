
#ifndef __ELASTOS_DROID_NET_CLINKSOCKET_H__
#define __ELASTOS_DROID_NET_CLINKSOCKET_H__

#include "_Elastos_Droid_Net_CLinkSocket.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Net::IInetAddress;
using Elastos::Net::ISocketAddress;
using Elastos::Net::ISocketChannel;

namespace Elastos {
namespace Droid {
namespace Net {

CarClass(CLinkSocket)
{
public:
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ILinkSocketNotifier* notifier);

    CARAPI constructor(
        /* [in] */ ILinkSocketNotifier* notifier,
        /* [in] */ IProxy* proxy);

    CARAPI GetLinkProperties(
        /* [out] */ ILinkProperties** result);

    CARAPI SetNeededCapabilities(
        /* [in] */ ILinkCapabilities* needs,
        /* [out] */ Boolean* result);

    CARAPI GetNeededCapabilities(
        /* [out] */ ILinkCapabilities** result);

    CARAPI GetCapabilities(
        /* [out] */ ILinkCapabilities** result);

    CARAPI GetCapabilities(
        /* [in] */ IObjectContainer* capabilities,
        /* [out] */ ILinkCapabilities** result);

    CARAPI SetTrackedCapabilities(
        /* [in] */ IObjectContainer* capabilities);

    CARAPI GetTrackedCapabilities(
        /* [out] */ IObjectContainer** result);

    CARAPI Connect(
        /* [in] */ ISocketAddress* remoteAddr);

    CARAPI Connect(
        /* [in] */ ISocketAddress* remoteAddr,
        /* [in] */ Int32 timeout);

    CARAPI Connect(
        /* [in] */ const String& dstName,
        /* [in] */ Int32 dstPort,
        /* [in] */ Int32 timeout);

    CARAPI Connect(
        /* [in] */ const String&  dstName,
        /* [in] */ Int32 dstPort);

    CARAPI Connect(
        /* [in] */ Int32 timeout);

    CARAPI Connect();

    CARAPI Close();

    CARAPI RequestNewLink(
        /* [in] */ ILinkRequestReason* linkRequestReason);

    CARAPI Bind(
        /* [in] */ ISocketAddress* localAddr);

    CARAPI IsBound(
        /* [out] */ Boolean* isBound);

    CARAPI IsConnected(
        /* [out] */ Boolean* isConnected);

    CARAPI IsClosed(
        /* [out] */ Boolean* isClosed);

    CARAPI ShutdownInput();

    CARAPI ShutdownOutput();

    CARAPI SetTrafficClass(
        /* [in] */ Int32 value);

    CARAPI GetTrafficClass(
        /* [out] */ Int32* value);

    CARAPI SetOOBInline(
        /* [in] */ Boolean oobinline);

    CARAPI GetOOBInline(
        /* [out] */ Boolean* oobinline);

    CARAPI GetInetAddress(
        /* [out] */ IInetAddress** address);

    CARAPI SetKeepAlive(
        /* [in] */ Boolean keepAlive);


    CARAPI IsInputShutdown(
        /* [out] */ Boolean* isInputShutdown);

    CARAPI IsOutputShutdown(
        /* [out] */ Boolean* isOutputShutdown);

    CARAPI SetReuseAddress(
        /* [in] */ Boolean reuse);

    CARAPI GetReuseAddress(
        /* [out] */ Boolean* reuse);

    CARAPI GetPort(
        /* [out] */ Int32* port);

    CARAPI GetSoLinger(
        /* [out] */ Int32* value);

    CARAPI SetSoLinger(
        /* [in] */ Boolean on,
        /* [in] */ Int32 timeout);

    CARAPI SetSoTimeout(
        /* [in] */ Int32 timeout);

    CARAPI SetTcpNoDelay(
        /* [in] */ Boolean on);

    CARAPI SetSendBufferSize(
        /* [in] */ Int32 size);

    CARAPI SetReceiveBufferSize(
        /* [in] */ Int32 size);

    CARAPI GetReceiveBufferSize(
        /* [out] */ Int32* size);

    CARAPI GetSendBufferSize(
        /* [out] */ Int32* size);

    CARAPI GetSoTimeout(
        /* [out] */ Int32* timeout);

    CARAPI GetTcpNoDelay(
        /* [out] */ Boolean* noDelay);

    CARAPI GetLocalAddress(
        /* [out] */ IInetAddress** address);

    CARAPI GetLocalPort(
        /* [out] */ Int32* localPort);

    CARAPI GetKeepAlive(
        /* [out] */ Boolean* keepAlive);

    CARAPI SendUrgentData(
        /* [in] */ Int32 value);

    CARAPI GetInputStream(
        /* [out] */ IInputStream** is);

    CARAPI GetOutputStream(
        /* [out] */ IOutputStream** os);

    CARAPI GetLocalSocketAddress(
        /* [out] */ ISocketAddress** address);

    CARAPI GetRemoteSocketAddress(
        /* [out] */ ISocketAddress** address);

    CARAPI GetChannel(
        /* [out] */ ISocketChannel** channel);

    CARAPI GetFileDescriptor(
        /* [out] */ IFileDescriptor** fd);

    CARAPI SetPerformancePreferences(
        /* [in] */ Int32 connectionTime,
        /* [in] */ Int32 latency,
        /* [in] */ Int32 bandwidth);

private:
   Object mLock;
   static const String TAG;
   static const Boolean DBG;
};

} // namespace Net
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CLINKSOCKET_H__

