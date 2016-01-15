
#include "elastos/droid/net/IpConfiguration.h"
#include "elastos/droid/net/CStaticIpConfiguration.h"
#include "elastos/droid/net/CProxyInfo.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Objects.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL_2(IpConfiguration, Object, IIpConfiguration, IParcelable)

const String IpConfiguration::TAG("IpConfiguration");

ECode IpConfiguration::Init(
    /* [in] */ IpConfigurationIpAssignment ipAssignment,
    /* [in] */ IpConfigurationProxySettings proxySettings,
    /* [in] */ IStaticIpConfiguration* staticIpConfiguration,
    /* [in] */ IProxyInfo* httpProxy)
{
    mIpAssignment = ipAssignment;
    mProxySettings = proxySettings;
    if (staticIpConfiguration == NULL) {
        mStaticIpConfiguration = NULL;
    }
    else {
        CStaticIpConfiguration::New(staticIpConfiguration, (IStaticIpConfiguration**)&mStaticIpConfiguration);
    }
    if (httpProxy == NULL) {
        mHttpProxy = NULL;
    }
    else {
        CProxyInfo::New(httpProxy, (IProxyInfo**)&mHttpProxy);
    }
    return NOERROR;
}

ECode IpConfiguration::constructor()
{
    return Init(UNASSIGNED_IpAssignment, UNASSIGNED_ProxySettings, NULL, NULL);
}

ECode IpConfiguration::constructor(
    /* [in] */ IpConfigurationIpAssignment ipAssignment,
    /* [in] */ IpConfigurationProxySettings proxySettings,
    /* [in] */ IStaticIpConfiguration* staticIpConfiguration,
    /* [in] */ IProxyInfo* httpProxy)
{
    return Init(ipAssignment, proxySettings, staticIpConfiguration, httpProxy);
}

ECode IpConfiguration::constructor(
    /* [in] */ IIpConfiguration* source)
{
    constructor();
    if (source != NULL) {
        IpConfigurationIpAssignment ipAssignment;
        source->GetIpAssignment(&ipAssignment);
        AutoPtr<IStaticIpConfiguration> staticIpConfiguration;
        source->GetStaticIpConfiguration((IStaticIpConfiguration**)&staticIpConfiguration);
        IpConfigurationProxySettings proxySettings;
        source->GetProxySettings(&proxySettings);
        AutoPtr<IProxyInfo> httpProxy;
        source->GetHttpProxy((IProxyInfo**)&httpProxy);
        return Init(ipAssignment, proxySettings, staticIpConfiguration, httpProxy);
    }
    return NOERROR;
}

ECode IpConfiguration::GetIpAssignment(
    /* [out] */ IpConfigurationIpAssignment* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mIpAssignment;
    return NOERROR;
}

ECode IpConfiguration::SetIpAssignment(
    /* [in] */ IpConfigurationIpAssignment ipAssignment)
{
    mIpAssignment = ipAssignment;
    return NOERROR;
}

ECode IpConfiguration::GetStaticIpConfiguration(
    /* [out] */ IStaticIpConfiguration** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mStaticIpConfiguration;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode IpConfiguration::SetStaticIpConfiguration(
    /* [in] */ IStaticIpConfiguration* staticIpConfiguration)
{
    VALIDATE_NOT_NULL(staticIpConfiguration)

    mStaticIpConfiguration = staticIpConfiguration;
    return NOERROR;
}

ECode IpConfiguration::GetProxySettings(
    /* [out] */ IpConfigurationProxySettings* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mProxySettings;
    return NOERROR;
}

ECode IpConfiguration::SetProxySettings(
    /* [in] */ IpConfigurationProxySettings proxySettings)
{
    mProxySettings = proxySettings;
    return NOERROR;
}

ECode IpConfiguration::GetHttpProxy(
    /* [out] */ IProxyInfo** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mHttpProxy;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode IpConfiguration::SetHttpProxy(
    /* [in] */ IProxyInfo* httpProxy)
{
    VALIDATE_NOT_NULL(httpProxy)

    mHttpProxy = httpProxy;
    return NOERROR;
}

ECode IpConfiguration::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    StringBuilder sbuf;
    String s;
    sbuf.Append("IP assignment: ");
    sbuf.Append(mIpAssignment);
    sbuf.Append("\n");
    if (mStaticIpConfiguration != NULL) {
        IObject::Probe(mStaticIpConfiguration)->ToString(&s);
        sbuf.Append(String("Static configuration: ") + s);
        sbuf.Append("\n");
    }
    sbuf.Append("Proxy settings: ");
    sbuf.Append(mProxySettings);
    sbuf.Append("\n");
    if (mHttpProxy != NULL) {
        IObject::Probe(mHttpProxy)->ToString(&s);
        sbuf.Append(String("HTTP proxy: ") + s);
        sbuf.Append("\n");
    }
    sbuf.ToString(result);
    return NOERROR;
}

ECode IpConfiguration::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(this) == IInterface::Probe(o)) {
        *result = TRUE;
        return NOERROR;
    }
    if (IIpConfiguration::Probe(o) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IIpConfiguration> iother = IIpConfiguration::Probe(o);
    AutoPtr<IpConfiguration> other = (IpConfiguration*) iother.Get();
    *result = mIpAssignment == other->mIpAssignment &&
            mProxySettings == other->mProxySettings &&
            Objects::Equals(mStaticIpConfiguration, other->mStaticIpConfiguration) &&
            Objects::Equals(mHttpProxy, other->mHttpProxy);
    return NOERROR;
}

ECode IpConfiguration::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = 13;
    Int32 hashCode;
    if (mStaticIpConfiguration != NULL) {
        IObject::Probe(mStaticIpConfiguration)->GetHashCode(&hashCode);
    }
    else {
        hashCode = 0;
    }
    *result += hashCode;
    *result += 17 * mIpAssignment;
    *result += 47 * mProxySettings;
    IObject::Probe(mHttpProxy)->GetHashCode(&hashCode);
    *result += 83 * hashCode;
    return NOERROR;
}

ECode IpConfiguration::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    AutoPtr<IInterface> obj;
    parcel->ReadInt32(&mIpAssignment);
    parcel->ReadInterfacePtr((Handle32*)&obj);
    mStaticIpConfiguration = IStaticIpConfiguration::Probe(obj);
    parcel->ReadInt32(&mProxySettings);
    obj = NULL;
    parcel->ReadInterfacePtr((Handle32*)&obj);
    mHttpProxy = IProxyInfo::Probe(obj);
    return NOERROR;
}

ECode IpConfiguration::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mIpAssignment);
    dest->WriteInterfacePtr(mStaticIpConfiguration);
    dest->WriteInt32(mProxySettings);
    dest->WriteInterfacePtr(mHttpProxy);
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
