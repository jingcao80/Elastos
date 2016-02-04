
#ifndef __ELASTOS_DROID_SERVER_LOCATION_FUSEDLOCATIONHARDWARESECURE_H__
#define __ELASTOS_DROID_SERVER_LOCATION_FUSEDLOCATIONHARDWARESECURE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include <elastos/core/Object.h>
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Hardware::Location::IIFusedLocationHardware;
using Elastos::Droid::Hardware::Location::IIFusedLocationHardwareSink;
using Elastos::Droid::Location::IFusedBatchOptions;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * FusedLocationHardware decorator that adds permission checking.
 * @hide
 */
class FusedLocationHardwareSecure
    : public Object
    , public IIFusedLocationHardware
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    FusedLocationHardwareSecure();

    CARAPI constructor(
        /* [in] */ IIFusedLocationHardware* locationHardware,
        /* [in] */ IContext* context,
        /* [in] */ const String& permissionId);

    // @Override
    CARAPI RegisterSink(
        /* [in] */ IIFusedLocationHardwareSink* eventSink);

    // @Override
    CARAPI UnregisterSink(
        /* [in] */ IIFusedLocationHardwareSink* eventSink);

    // @Override
    CARAPI GetSupportedBatchSize(
        /* [out] */ Int32* size);

    // @Override
    CARAPI StartBatching(
        /* [in] */ Int32 id,
        /* [in] */ IFusedBatchOptions* batchOptions);

    // @Override
    CARAPI StopBatching(
        /* [in] */ Int32 id);

    // @Override
    CARAPI UpdateBatchingOptions(
        /* [in] */ Int32 id,
        /* [in] */ IFusedBatchOptions* batchoOptions);

    // @Override
    CARAPI RequestBatchOfLocations(
        /* [in] */ Int32 batchSizeRequested);

    // @Override
    CARAPI SupportsDiagnosticDataInjection(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI InjectDiagnosticData(
        /* [in] */ const String& data);

    // @Override
    CARAPI SupportsDeviceContextInjection(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI InjectDeviceContext(
        /* [in] */ Int32 deviceEnabledContext);


    CARAPI ToString(
        /* [out] */ String* str)
    {
        return Object::ToString(str);
    }

private:
    CARAPI_(void) CheckPermissions();

private:
    AutoPtr<IIFusedLocationHardware> mLocationHardware;
    AutoPtr<IContext> mContext;
    String mPermissionId;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_FUSEDLOCATIONHARDWARESECURE_H__

