
#ifndef __ELASTOS_DROID_SERVER_LOCATION_LOCATIONPROVIDERPROXY_H__
#define __ELASTOS_DROID_SERVER_LOCATION_LOCATIONPROVIDERPROXY_H__

#ifdef DROID_CORE
#include "Elastos.Droid.Core_server.h"
#elif defined(DROID_SERVER)
#include "Elastos.Droid.Core.h"
#endif
#include "elastos/droid/ext/frameworkext.h"
#include "location/LocationProviderInterface.h"
#include "ServiceWatcher.h"
#include <elastos/utility/etl/List.h>

using Elastos::Core::IRunnable;
using Elastos::Utility::Etl::List;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IFileDescriptor;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Location::ILocationProvider;
using Elastos::Droid::Location::IILocationProvider;
using Elastos::Droid::Location::IProviderProperties;
using Elastos::Droid::Location::IProviderRequest;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Os::CWorkSource;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * Proxy for ILocationProvider implementations.
 */
class LocationProviderProxy
            : public ElRefBase
            , public ILocationProviderInterface
{
private:
    static const String TAG;// = "LocationProviderProxy";

    class NewServiceWorkRunnable
            : public IRunnable
            , public ElRefBase
    {
    public:

        CAR_INTERFACE_DECL()

        NewServiceWorkRunnable(
            /* [in] */ LocationProviderProxy* host);

        CARAPI Run();
    private:

        LocationProviderProxy* mHost;
    };

public:

    static CARAPI_(AutoPtr<LocationProviderProxy>) CreateAndBind(
        /* [in] */ IContext* context,
        /* [in] */ const String& name,
        /* [in] */ const String& action,
        /* [in] */ List<String>* initialPackageNames,
        /* [in] */ IHandler* handler,
        /* [in] */ Int32 userId);

    CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *object,
            /* [out] */ InterfaceID *IID);

    CARAPI_(String) GetConnectedPackageName();

    //@Override
    CARAPI GetName(
        /* [out] */ String* name);

    //@Override
    CARAPI GetProperties(
        /* [out] */ IProviderProperties** properties);

    //@Override
    CARAPI Enable();

    //@Override
    CARAPI Disable();

    //@Override
    CARAPI IsEnabled(
        /* [out] */ Boolean* enable);

    //@Override
    CARAPI SetRequest(
        /* [in] */ IProviderRequest* request,
        /* [in] */ IWorkSource* source);

    //@Override
    CARAPI SwitchUser(
        /* [in] */ Int32 userId);

    //@Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    //@Override
    CARAPI GetStatus(
        /* [in] */ IBundle* extras,
        /* [out] */ Int32* status);

    //@Override
    CARAPI GetStatusUpdateTime(
        /* [out] */ Int64* time);

    //@Override
    CARAPI SendExtraCommand(
        /* [in] */ const String& command,
        /* [in] */ IBundle* extras,
        /* [out] */ Boolean* result);

private:

    LocationProviderProxy(
        /* [in] */ IContext* context,
        /* [in] */ const String& name,
        /* [in] */ const String& action,
        /* [in] */ List<String>* initialPackageNames,
        /* [in] */ IHandler* handler,
        /* [in] */ Int32 userId);

    CARAPI_(Boolean) Bind();

    CARAPI_(AutoPtr<IILocationProvider>) GetService();

private:

    static const Boolean D;// = ILocationManagerService::D;

    AutoPtr<IContext> mContext;
    String mName;
    AutoPtr<ServiceWatcher> mServiceWatcher;

    Object mLock;

    // cached values set by the location manager, synchronized on mLock
    AutoPtr<IProviderProperties> mProperties;
    Boolean mEnabled;// = false;
    AutoPtr<IProviderRequest> mRequest;
    AutoPtr<IWorkSource> mWorksource;// = new WorkSource();

    /**
     * Work to apply current state to a newly connected provider.
     * Remember we can switch the service that implements a providers
     * at run-time, so need to apply current state.
     */
    AutoPtr<IRunnable> mNewServiceWork;
 };

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_LOCATIONPROVIDERPROXY_H__
