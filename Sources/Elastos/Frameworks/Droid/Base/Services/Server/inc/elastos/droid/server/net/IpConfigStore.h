
#ifndef __ELASTOS_DROID_SERVER_NET_IPCONFIGSTORE_H__
#define __ELASTOS_DROID_SERVER_NET_IPCONFIGSTORE_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include "elastos/droid/server/net/DelayedDiskWrite.h"

using Elastos::Droid::Utility::ISparseArray;
using Elastos::IO::IDataOutputStream;
using Elastos::Droid::Net::IIpConfiguration;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

class IpConfigStore
    : public Object
{
    friend class InnerSub_Writer;

private:
    class InnerSub_Writer
        : public Object
        , public IDelayedDiskWriteWriter
    {
    public:
        CAR_INTERFACE_DECL();

        InnerSub_Writer(
            /* [in] */ IpConfigStore* host,
            /* [in] */ ISparseArray* networks);

        CARAPI OnWriteCalled(
            /* [in] */ IDataOutputStream* out);

    private:
        IpConfigStore* mHost;
        AutoPtr<ISparseArray> mNetworks;
    };

public:
    CARAPI constructor();

    CARAPI WriteIpAndProxyConfigurations(
        /* [in] */ const String& filePath,
        /* [in] */ ISparseArray* networks);

    CARAPI ReadIpAndProxyConfigurations(
        /* [in] */ const String& filePath,
        /* [out] */ ISparseArray** result);

    CARAPI Loge(
        /* [in] */ const char* fmt, ...);

    CARAPI Log(
        /* [in] */ const char* fmt, ...);

private:
    CARAPI WriteConfig(
        /* [in] */ IDataOutputStream* out,
        /* [in] */ Int32 configKey,
        /* [in] */ IIpConfiguration* config,
        /* [out] */ Boolean* result);

public:
    /* const */ AutoPtr<DelayedDiskWrite> mWriter;

    /* IP and proxy configuration keys */
    static const String ID_KEY;

    static const String IP_ASSIGNMENT_KEY;

    static const String LINK_ADDRESS_KEY;

    static const String GATEWAY_KEY;

    static const String DNS_KEY;

    static const String PROXY_SETTINGS_KEY;

    static const String PROXY_HOST_KEY;

    static const String PROXY_PORT_KEY;

    static const String PROXY_PAC_FILE;

    static const String EXCLUSION_LIST_KEY;

    static const String EOS;

    static const Int32 IPCONFIG_FILE_VERSION;

private:
    static const String TAG;

    static const Boolean DBG;
};

} // namespace Net
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_NET_IPCONFIGSTORE_H__
