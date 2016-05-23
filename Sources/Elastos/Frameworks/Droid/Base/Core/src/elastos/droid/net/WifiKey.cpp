
#include "elastos/droid/net/WifiKey.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include <elastos/utility/Objects.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/regex/Pattern.h>

using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Objects;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::Pattern;

namespace Elastos {
namespace Droid {
namespace Net {

const AutoPtr<IPattern> WifiKey::SSID_PATTERN = InitPattern(String("(\".*\")|(0x[\\p{XDigit}]+)"));
const AutoPtr<IPattern> WifiKey::BSSID_PATTERN = InitPattern(String("([\\p{XDigit}]{2}:){5}[\\p{XDigit}]{2}"));

CAR_INTERFACE_IMPL_2(WifiKey, Object, IParcelable, IWifiKey)

WifiKey::WifiKey()
{}

AutoPtr<IPattern> WifiKey::InitPattern(
    /* [in] */ const String& pattern)
{
    AutoPtr<IPattern> rev;
    Pattern::Compile(pattern, (IPattern**)&rev);
    return rev;
}

ECode WifiKey::constructor()
{
    return NOERROR;
}

ECode WifiKey::constructor(
    /* [in] */ const String& ssid,
    /* [in] */ const String& bssid)
{
    AutoPtr<IMatcher> matcher;
    SSID_PATTERN->Matcher(ssid, (IMatcher**)&matcher);
    if (!Ptr(matcher)->Func(matcher->Matches)) {
        Logger::E("WifiKey", "Invalid ssid: %s", mSsid.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    matcher = NULL;
    BSSID_PATTERN->Matcher(bssid, (IMatcher**)&matcher);
    if (!Ptr(matcher)->Func(matcher->Matches)) {
        Logger::E("WifiKey", "Invalid bssid: %s", mBssid.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mSsid = ssid;
    mBssid = bssid;
    return NOERROR;
}

ECode WifiKey::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(this) == IInterface::Probe(o)) FUNC_RETURN(TRUE)
    if (o == NULL) FUNC_RETURN(FALSE)
    ClassID this_cid, o_cid;
    IObject::Probe(TO_IINTERFACE(this))->GetClassID(&this_cid);
    IObject::Probe(o)->GetClassID(&o_cid);
    if (this_cid != o_cid) FUNC_RETURN(FALSE)
    AutoPtr<WifiKey> wifiKey = (WifiKey*) IWifiKey::Probe(o);
    *result = mSsid.Equals(wifiKey->mSsid) && mBssid.Equals(wifiKey->mBssid);
    return NOERROR;
}

ECode WifiKey::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(0);
    (*array)[0] = mSsid.GetHashCode();
    (*array)[1] = mBssid.GetHashCode();
    *result = Arrays::GetHashCode(array);
    return NOERROR;
}

ECode WifiKey::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = String("WifiKey[SSID=") + mSsid + ",BSSID=" + mBssid + "]";
    return NOERROR;
}

ECode WifiKey::ReadFromParcel(
        /* [in] */ IParcel* parcel)
{
    parcel->ReadString(&mSsid);
    parcel->ReadString(&mBssid);
    return NOERROR;
}

ECode WifiKey::WriteToParcel(
        /* [in] */ IParcel* dest)
{
    dest->WriteString(mSsid);
    dest->WriteString(mBssid);
    return NOERROR;
}

ECode WifiKey::GetSsid(
        /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mSsid;
    return NOERROR;
}

ECode WifiKey::GetBssid(
        /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mBssid;
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
