
#ifndef __ELASTOS_DROID_NET_NETWORKMISC_H__
#define __ELASTOS_DROID_NET_NETWORKMISC_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * A grab-bag of information (metadata, policies, properties, etc) about a {@link Network}.
 *
 * @hide
 */
class NetworkMisc
    : public Object
    , public IParcelable
    , public INetworkMisc
{
public:
    CAR_INTERFACE_DECL()

    NetworkMisc();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ INetworkMisc* nm);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * If the {@link Network} is a VPN, whether apps are allowed to bypass the VPN. This is set by
     * a {@link VpnService} and used by {@link ConnectivityService} when creating a VPN.
     */
    CARAPI GetAllowBypass(
        /* [out] */ Boolean* result);

    /**
     * If the {@link Network} is a VPN, whether apps are allowed to bypass the VPN. This is set by
     * a {@link VpnService} and used by {@link ConnectivityService} when creating a VPN.
     */
    CARAPI SetAllowBypass(
        /* [in] */ Boolean allowBypass);

    /**
     * Set if the network was manually/explicitly connected to by the user either from settings
     * or a 3rd party app.  For example, turning on cell data is not explicit but tapping on a wifi
     * ap in the wifi settings to trigger a connection is explicit.  A 3rd party app asking to
     * connect to a particular access point is also explicit, though this may change in the future
     * as we want apps to use the multinetwork apis.
     */
    CARAPI GetExplicitlySelected(
        /* [out] */ Boolean* result);

    /**
     * Set if the network was manually/explicitly connected to by the user either from settings
     * or a 3rd party app.  For example, turning on cell data is not explicit but tapping on a wifi
     * ap in the wifi settings to trigger a connection is explicit.  A 3rd party app asking to
     * connect to a particular access point is also explicit, though this may change in the future
     * as we want apps to use the multinetwork apis.
     */
    CARAPI SetExplicitlySelected(
        /* [in] */ Boolean explicitlySelected);

private:
    /**
     * If the {@link Network} is a VPN, whether apps are allowed to bypass the VPN. This is set by
     * a {@link VpnService} and used by {@link ConnectivityService} when creating a VPN.
     */
    Boolean mAllowBypass;

    /**
     * Set if the network was manually/explicitly connected to by the user either from settings
     * or a 3rd party app.  For example, turning on cell data is not explicit but tapping on a wifi
     * ap in the wifi settings to trigger a connection is explicit.  A 3rd party app asking to
     * connect to a particular access point is also explicit, though this may change in the future
     * as we want apps to use the multinetwork apis.
     */
    Boolean mExplicitlySelected;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_NETWORKMISC_H__
