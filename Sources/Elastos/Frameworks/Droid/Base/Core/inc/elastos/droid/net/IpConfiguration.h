
#ifndef __ELASTOS_DROID_NET_IPCONFIGURATION_H__
#define __ELASTOS_DROID_NET_IPCONFIGURATION_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * A class representing a configured network.
 * @hide
 */
class IpConfiguration
    : public Object
    , public IIpConfiguration
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

public:
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IpConfigurationIpAssignment ipAssignment,
        /* [in] */ IpConfigurationProxySettings proxySettings,
        /* [in] */ IStaticIpConfiguration* staticIpConfiguration,
        /* [in] */ IProxyInfo* httpProxy);

    CARAPI constructor(
        /* [in] */ IIpConfiguration* source);

    CARAPI GetIpAssignment(
        /* [out] */ IpConfigurationIpAssignment* result);

    CARAPI SetIpAssignment(
        /* [in] */ IpConfigurationIpAssignment ipAssignment);

    CARAPI GetStaticIpConfiguration(
        /* [out] */ IStaticIpConfiguration** result);

    CARAPI SetStaticIpConfiguration(
        /* [in] */ IStaticIpConfiguration* staticIpConfiguration);

    CARAPI GetProxySettings(
        /* [out] */ IpConfigurationProxySettings* result);

    CARAPI SetProxySettings(
        /* [in] */ IpConfigurationProxySettings proxySettings);

    CARAPI GetHttpProxy(
        /* [out] */ IProxyInfo** result);

    CARAPI SetHttpProxy(
        /* [in] */ IProxyInfo* httpProxy);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    /** Implement the Parcelable interface */
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    CARAPI Init(
        /* [in] */ IpConfigurationIpAssignment ipAssignment,
        /* [in] */ IpConfigurationProxySettings proxySettings,
        /* [in] */ IStaticIpConfiguration* staticIpConfiguration,
        /* [in] */ IProxyInfo* httpProxy);

private:
    static const String TAG;

    IpConfigurationIpAssignment mIpAssignment;

    AutoPtr<IStaticIpConfiguration> mStaticIpConfiguration;

    IpConfigurationProxySettings mProxySettings;

    AutoPtr<IProxyInfo> mHttpProxy;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_IPCONFIGURATION_H__
