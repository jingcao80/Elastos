
#include "elastos/droid/location/Country.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Utility::ILocale;
using Elastos::Core::StringBuilder;
using Elastos::Droid::Os::SystemClock;

namespace Elastos {
namespace Droid {
namespace Location {

CAR_INTERFACE_IMPL_2(Country, Object, ICountry, IParcelable)

Country::Country()
    : mSource(0)
    , mHashCode(0)
    , mTimestamp(0LL)
{
}

ECode Country::constructor()
{
    return NOERROR;
}

ECode Country::constructor(
    /* [in] */ const String& countryIso,
    /* [in] */ Int32 source)
{
    if (countryIso.IsNull() || source < ICountry::COUNTRY_SOURCE_NETWORK
            || source > ICountry::COUNTRY_SOURCE_LOCALE) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mCountryIso = countryIso.ToUpperCase(/*ILocale::US*/);

    mSource = source;
    mTimestamp = SystemClock::GetElapsedRealtime();
    return NOERROR;
}

ECode Country::constructor(
    /* [in] */ const String& countryIso,
    /* [in] */ Int32 source,
    /* [in] */ Int64 timestamp)
{
    if (countryIso.IsNull() || source < ICountry::COUNTRY_SOURCE_NETWORK
            || source > ICountry::COUNTRY_SOURCE_LOCALE) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mCountryIso = countryIso.ToUpperCase(/*ILocale::US*/);

    mSource = source;
    mTimestamp = timestamp;
    return NOERROR;
}

ECode Country::constructor(
    /* [in] */ ICountry* country)
{
    VALIDATE_NOT_NULL(country);
    Country* _country = (Country*)country;

    mCountryIso = _country->mCountryIso;
    mSource = _country->mSource;
    mTimestamp = _country->mTimestamp;
    return NOERROR;
}

ECode Country::GetCountryIso(
    /* [out] */ String* strOut)
{
    VALIDATE_NOT_NULL(strOut);
    *strOut = mCountryIso;

    return NOERROR;
}

ECode Country::GetSource(
    /* [out] */ Int32* source)
{
    VALIDATE_NOT_NULL(source);
    *source = mSource;
    return NOERROR;
}

ECode Country::GetTimestamp(
    /* [out] */ Int64* timestamp)
{
    VALIDATE_NOT_NULL(timestamp);
    *timestamp = mTimestamp;
    return NOERROR;
}

ECode Country::ReadFromParcel(
        /* [in] */ IParcel* parcel)
{
    parcel->ReadString(&mCountryIso);
    parcel->ReadInt32(&mSource);
    parcel->ReadInt64(&mTimestamp);

    return NOERROR;
}

ECode Country::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    VALIDATE_NOT_NULL(parcel);
    parcel->WriteString(mCountryIso);
    parcel->WriteInt32(mSource);
    parcel->WriteInt64(mTimestamp);
    return NOERROR;
}

ECode Country::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (IInterface::Probe(object) == TO_IINTERFACE(this)) {
        *result = TRUE;
        return NOERROR;
    }
    AutoPtr<ICountry> c = ICountry::Probe(object);
    if (c != NULL) {
        // No need to check the equivalence of the timestamp
        String str;
        c->GetCountryIso(&str);
        Int32 source;
        c->GetSource(&source);
        *result = mCountryIso.Equals(str) && mSource == source;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode Country::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    Int32 hash = mHashCode;
    if (hash == 0) {
        hash = 17;
        hash = hash * 13 + mCountryIso.GetHashCode();
        hash = hash * 13 + mSource;
        mHashCode = hash;
    }
    *hashCode = mHashCode;
    return NOERROR;
}

ECode Country::EqualsIgnoreSource(
    /* [in] */ ICountry* country,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(country);
    VALIDATE_NOT_NULL(result);
    String str;
    country->GetCountryIso(&str);
    *result = country != NULL && mCountryIso.Equals(str);
    return NOERROR;
}

ECode Country::ToString(
    /* [out] */ String* strOut)
{
    VALIDATE_NOT_NULL(strOut);
    StringBuilder sb("Country {ISO=");
    sb += mCountryIso;
    sb += ", source=";
    sb += mSource;
    sb += ", time=";
    sb += mTimestamp;
    sb += "}";
    *strOut = sb.ToString();
    return NOERROR;
}

} // namespace Location
} // namepsace Droid
} // namespace Elastos
