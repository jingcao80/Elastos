#ifndef __ELASTOS_DROID_SERVER_ETHERNET_CETHERNETSERVICE_H__
#define __ELASTOS_DROID_SERVER_ETHERNET_CETHERNETSERVICE_H__

#include "_Elastos_Droid_Server_Ethernet_CEthernetServiceImpl.h"
#include "elastos/droid/server/ethernet/EthernetConfigStore.h"
#include "elastos/droid/server/ethernet/EthernetNetworkFactory.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::IIEthernetManager;
using Elastos::Droid::Net::IIpConfiguration;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IINetworkManagementService;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Ethernet {

/**
 * EthernetServiceImpl handles remote Ethernet operation requests by implementing
 * the IEthernetManager interface.
 *
 * @hide
 */
CarClass(CEthernetServiceImpl)
    , public Object
    , public IIEthernetManager
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI Start();

    /**
     * Get Ethernet configuration
     * @return the Ethernet Configuration, contained in {@link IpConfiguration}.
     */
    CARAPI GetConfiguration(
        /* [out] */ IIpConfiguration** config);

    /**
     * Set Ethernet configuration
     */
    CARAPI SetConfiguration(
        /* [in] */ IIpConfiguration* config);

    CARAPI ToString(
        /* [out] */ String* str);

protected:
    // @Override
    CARAPI_(void) Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* writer,
        /* [in] */ ArrayOf<String>* args);

private:
    CARAPI EnforceAccessPermission();

    CARAPI EnforceChangePermission();

    CARAPI EnforceConnectivityInternalPermission();

private:
    static const String TAG;

    AutoPtr<IContext> mContext;
    AutoPtr<EthernetConfigStore> mEthernetConfigStore;
    AutoPtr<IINetworkManagementService> mNMService;
    AutoPtr<IAtomicBoolean> mStarted;
    AutoPtr<IIpConfiguration> mIpConfiguration;
    AutoPtr<IConnectivityManager> mCM;

    AutoPtr<IHandler> mHandler;
    AutoPtr<INetworkInfo> mNetworkInfo;
    AutoPtr<EthernetNetworkFactory> mTracker;
};

} // namespace Ethernet
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_ETHERNET_CETHERNETSERVICE_H__

