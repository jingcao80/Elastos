
#ifndef __ELASTOS_DROID_NET_STATICIPCONFIGURATION_H__
#define __ELASTOS_DROID_NET_STATICIPCONFIGURATION_H__

#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Net::IInetAddress;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Class that describes static IP configuration.
 *
 * This class is different from LinkProperties because it represents
 * configuration intent. The general contract is that if we can represent
 * a configuration here, then we should be able to configure it on a network.
 * The intent is that it closely match the UI we have for configuring networks.
 *
 * In contrast, LinkProperties represents current state. It is much more
 * expressive. For example, it supports multiple IP addresses, multiple routes,
 * stacked interfaces, and so on. Because LinkProperties is so expressive,
 * using it to represent configuration intent as well as current state causes
 * problems. For example, we could unknowingly save a configuration that we are
 * not in fact capable of applying, or we could save a configuration that the
 * UI cannot display, which has the potential for malicious code to hide
 * hostile or unexpected configuration from the user: see, for example,
 * http://b/12663469 and http://b/16893413 .
 *
 * @hide
 */
class StaticIpConfiguration
    : public Object
    , public IParcelable
    , public IStaticIpConfiguration
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IStaticIpConfiguration* source);

    CARAPI Clear();

    /**
     * Returns the network routes specified by this object. Will typically include a
     * directly-connected route for the IP address's local subnet and a default route.
     */
    CARAPI GetRoutes(
        /* [in] */ const String& iface,
        /* [out] */ IList** result);

    /**
     * Returns a LinkProperties object expressing the data in this object. Note that the information
     * contained in the LinkProperties will not be a complete picture of the link's configuration,
     * because any configuration information that is obtained dynamically by the network (e.g.,
     * IPv6 configuration) will not be included.
     */
    CARAPI ToLinkProperties(
        /* [in] */ const String& iface,
        /* [out] */ ILinkProperties** result);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    /** Implement the Parcelable interface */
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetIpAddress(
        /* [out] */ ILinkAddress** result);

    CARAPI SetIpAddress(
        /* [in] */ ILinkAddress* ipAddress);

    CARAPI GetGateway(
        /* [out] */ IInetAddress** result);

    CARAPI SetGateway(
        /* [in] */ IInetAddress* gateway);

    CARAPI GetDnsServers(
        /* [out] */ IArrayList** result);

    CARAPI GetDomains(
        /* [out] */ String* result);

    CARAPI SetDomains(
        /* [in] */ const String& domains);

protected:
    AutoPtr<ILinkAddress> mIpAddress;

    AutoPtr<IInetAddress> mGateway;

    AutoPtr<IArrayList> mDnsServers;

    String mDomains;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_STATICIPCONFIGURATION_H__
