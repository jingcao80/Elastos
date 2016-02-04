
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/net/DhcpResults.h"
#include "elastos/droid/net/NetworkUtils.h"
#include "elastos/droid/net/CLinkAddress.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringBuffer.h>
#include <elastos/utility/Objects.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Text::TextUtils;

using Elastos::Core::StringBuffer;
using Elastos::Net::IInet4Address;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(DhcpResults, StaticIpConfiguration, IDhcpResults)

const String DhcpResults::TAG("DhcpResults");

ECode DhcpResults::GetServerAddress(
    /* [out] */ IInetAddress** result)
{
    VALIDATE_NOT_NULL(*result)
    *result = mServerAddress;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DhcpResults::SetServerAddress(
    /* [in] */ IInetAddress* serverAddress)
{
    VALIDATE_NOT_NULL(serverAddress)

    mServerAddress = serverAddress;
    return NOERROR;
}

ECode DhcpResults::GetVendorInfo(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mVendorInfo;
    return NOERROR;
}

ECode DhcpResults::GetLeaseDuration(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mLeaseDuration;
    return NOERROR;
}

ECode DhcpResults::constructor()
{
    return StaticIpConfiguration::constructor();
}

ECode DhcpResults::constructor(
    /* [in] */ IStaticIpConfiguration* source)
{
    return StaticIpConfiguration::constructor(source);
}

ECode DhcpResults::constructor(
    /* [in] */ IDhcpResults* source)
{
    StaticIpConfiguration::constructor(IStaticIpConfiguration::Probe(source));
    if (source != NULL) {
        // All these are immutable, so no need to make copies.
        AutoPtr<DhcpResults> dr = (DhcpResults*)source;
        mServerAddress = dr->mServerAddress;
        mVendorInfo = dr->mVendorInfo;
        mLeaseDuration = dr->mLeaseDuration;
    }
    return NOERROR;
}

ECode DhcpResults::UpdateFromDhcpRequest(
    /* [in] */ IDhcpResults* orig)
{
    if (orig == NULL) return NOERROR;
    AutoPtr<DhcpResults> dr = (DhcpResults*) orig;
    if (mGateway == NULL) mGateway = dr->mGateway;
    Int32 size;
    mDnsServers->GetSize(&size);
    if (size == 0) {
        Boolean b;
        mDnsServers->AddAll(ICollection::Probe(dr->mDnsServers), &b);
    }
    return NOERROR;
}

ECode DhcpResults::HasMeteredHint(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (!mVendorInfo.IsNull()) {
        *result = mVendorInfo.Contains("ANDROID_METERED");
        return NOERROR;
    }
    return NOERROR;
}

ECode DhcpResults::Clear()
{
    StaticIpConfiguration::Clear();
    mVendorInfo = NULL;
    mLeaseDuration = 0;
    return NOERROR;
}

ECode DhcpResults::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    String s;
    StaticIpConfiguration::ToString(&s);
    StringBuffer str(s);

    str.Append(" DHCP server ");
    IObject::Probe(mServerAddress)->ToString(&s);
    str.Append(s);
    str.Append(" Vendor info ");
    str.Append(mVendorInfo);
    str.Append(s);
    str.Append(" lease ");
    str.Append(mLeaseDuration);
    str.Append(s);
    str.Append(" seconds");

    str.ToString(result);
    return NOERROR;
}

ECode DhcpResults::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (this->Probe(EIID_IInterface) == IInterface::Probe(obj)) {
        *result = TRUE;
        return NOERROR;
    }

    AutoPtr<IDhcpResults> dr = IDhcpResults::Probe(obj);

    if (dr == NULL) {
        return NOERROR;
    }

    AutoPtr<DhcpResults> target = (DhcpResults*) dr.Get();
    Boolean b;
    StaticIpConfiguration::Equals(IInterface::Probe(dr), &b);
    *result =  b &&
            Objects::Equals(this->mServerAddress, target->mServerAddress) &&
            this->mVendorInfo.Equals(target->mVendorInfo) &&
            this->mLeaseDuration == target->mLeaseDuration;
    return NOERROR;
}

ECode DhcpResults::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    StaticIpConfiguration::WriteToParcel(dest);
    dest->WriteInt32(mLeaseDuration);
    dest->WriteInterfacePtr(mServerAddress.Get());
    dest->WriteString(mVendorInfo);
    return NOERROR;
}

ECode DhcpResults::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    StaticIpConfiguration::ReadFromParcel(in);
    in->ReadInt32(&mLeaseDuration);
    AutoPtr<IInterface> obj;
    in->ReadInterfacePtr((Handle32*)&obj);
    mServerAddress = IInetAddress::Probe(obj);
    in->ReadString(&mVendorInfo);
    return NOERROR;
}

ECode DhcpResults::SetIpAddress(
    /* [in] */ const String& addrString,
    /* [in] */ Int32 prefixLength,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IInetAddress> addr;
    AutoPtr<ILinkAddress> ipAddress;
    ECode ec;
    FAIL_GOTO((ec = NetworkUtils::NumericToInetAddress(addrString, (IInetAddress**)&addr)), FAIL_CATCH);
    FAIL_GOTO((ec = CLinkAddress::New(addr, prefixLength, (ILinkAddress**)&ipAddress)), FAIL_CATCH);
    return NOERROR;
FAIL_CATCH:
    if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION || ec == (ECode)E_CLASS_CAST_EXCEPTION) {
        Logger::E(TAG, "setIpAddress failed with addrString %s/%d", addrString.string(), prefixLength);
        *result = TRUE;
        return NOERROR;
    }
    return ec;
}

ECode DhcpResults::SetGateway(
    /* [in] */ const String& addrString,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(*result)
    *result = FALSE;

    ECode ec = NetworkUtils::NumericToInetAddress(addrString, (IInetAddress**)&mGateway);
    if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        Logger::E(TAG, "setGateway failed with addrString %s", addrString.string());
        *result = TRUE;
        return NOERROR;
    }
    return NOERROR;
}

ECode DhcpResults::AddDns(
    /* [in] */ const String& addrString,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (TextUtils::IsEmpty(addrString) == FALSE) {
        AutoPtr<IInetAddress> inetAddress;
        ECode ec = NetworkUtils::NumericToInetAddress(addrString, (IInetAddress**)&inetAddress);
        if (FAILED(ec)) {
            if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
                Logger::E(TAG, "addDns failed with addrString %s", addrString.string());
                *result = TRUE;
                return NOERROR;
            }
            return ec;
        }
        mDnsServers->Add(inetAddress);
    }
    *result = FALSE;
    return NOERROR;
}

ECode DhcpResults::SetServerAddress(
    /* [in] */ const String& addrString,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    mServerAddress = NULL;
    ECode ec = NetworkUtils::NumericToInetAddress(addrString, (IInetAddress**)&mServerAddress);
    if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        Logger::E(TAG, "setServerAddress failed with addrString %s", addrString.string());
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode DhcpResults::SetLeaseDuration(
    /* [in] */ Int32 duration)
{
    mLeaseDuration = duration;
    return NOERROR;
}

ECode DhcpResults::SetVendorInfo(
    /* [in] */ const String& info)
{
    mVendorInfo = info;
    return NOERROR;
}

ECode DhcpResults::SetDomains(
    /* [in] */ const String& domains)
{
    this->mDomains = domains;
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
