#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_GATEWAYINFO_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_GATEWAYINFO_H__

#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Telecomm.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

/**
 * When calls are made, they may contain gateway information for services which route phone calls
 * through their own service/numbers. The data consists of a number to call and the package name of
 * the service. This data is used in two ways:
 * <ol>
 * <li> Call the appropriate routing number
 * <li> Display information about how the call is being routed to the user
 * </ol>
 * @hide
 */
class GatewayInfo
    : public Object
    , public IGatewayInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /** @hide */
    CARAPI constructor(
        /* [in] */ const String& packageName,
        /* [in] */ IUri* gatewayUri,
        /* [in] */ IUri* originalAddress);

    /**
     * Package name of the gateway provider service. used to place the call with.
     */
    CARAPI GetGatewayProviderPackageName(
        /* [out] */ String* result);

    /**
     * Gateway provider address to use when actually placing the call.
     */
    CARAPI GetGatewayAddress(
        /* [out] */ IUri** result);

    /**
     * The actual call address that the user is trying to connect to via the gateway.
     */
    CARAPI GetOriginalAddress(
        /* [out] */ IUri** result);

    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
     * {@inheritDoc}
     */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* destination);

private:
    String mGatewayProviderPackageName;
    AutoPtr<IUri> mGatewayAddress;
    AutoPtr<IUri> mOriginalAddress;
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_GATEWAYINFO_H__