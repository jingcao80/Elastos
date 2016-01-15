#include "elastos/droid/location/Address.h"
#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/location/CAddress.h"
#include "elastos/droid/os/CBundle.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Location::CAddress;
using Elastos::Droid::Os::CBundle;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::IString;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CLocale;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Location {

CAR_INTERFACE_IMPL_2(Address, Object, IAddress, IParcelable)

Address::Address()
    : mMaxAddressLineIndex(-1)
    , mHasLatitude(FALSE)
    , mHasLongitude(FALSE)
{
}

Address::~Address()
{
}

ECode Address::constructor(
    /* [in] */ ILocale* locale)
{
    mLocale = locale;
    return NOERROR;
}

ECode Address::GetLocale(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale);
    *locale = mLocale;
    REFCOUNT_ADD(mLocale);
    return NOERROR;
}

ECode Address::GetMaxAddressLineIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = mMaxAddressLineIndex;
    return NOERROR;
}

ECode Address::GetAddressLine(
    /* [in] */ Int32 index,
    /* [out] */ String* line)
{
    VALIDATE_NOT_NULL(line);
    if (index < 0) {
        Logger::E("Address", "index = %d < 0", index);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mAddressLines != NULL) {
        AutoPtr<IInteger32> kobj;
        CInteger32::New(index, (IInteger32**)&kobj);
        AutoPtr<IInterface> obj;
        mAddressLines->Get(kobj.Get(), (IInterface**)&obj);
        ICharSequence::Probe(obj)->ToString(line);
    }
    else {
        *line = String(NULL);
    }
    return NOERROR;
}

ECode Address::SetAddressLine(
    /* [in] */ Int32 index,
    /* [in] */ const String& line)
{
    if (index < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mAddressLines == NULL) {
        CHashMap::New((IHashMap**)&mAddressLines);
    }
    AutoPtr<IInteger32> key;
    CInteger32::New(index, (IInteger32**)&key);
    AutoPtr<ICharSequence> value;
    CString::New(line, (ICharSequence**)&value);
    mAddressLines->Put(key.Get(), value.Get());

    if (line.IsNull()) {
        // We've eliminated a line, recompute the max index
        mMaxAddressLineIndex = -1;
        AutoPtr<ISet> ks;
        mAddressLines->GetKeySet((ISet**)&ks);
        AutoPtr<IIterator> iter;
        ks->GetIterator((IIterator**)&iter);
        Boolean hasNext;
        while (iter->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> next;
            iter->GetNext((IInterface**)&next);
            Int32 v;
            IInteger32::Probe(next)->GetValue(&v);
            mMaxAddressLineIndex = Elastos::Core::Math::Max(mMaxAddressLineIndex, v);
        }
    }
    else {
        mMaxAddressLineIndex = Elastos::Core::Math::Max(mMaxAddressLineIndex, index);
    }
    return NOERROR;
}

ECode Address::GetFeatureName(
    /* [out] */ String* featureName)
{
    VALIDATE_NOT_NULL(featureName);
    *featureName = mFeatureName;
    return NOERROR;
}

ECode Address::SetFeatureName(
    /* [in] */ const String& featureName)
{
    mFeatureName = featureName;
    return NOERROR;
}

ECode Address::GetAdminArea(
    /* [out] */ String* adminArea)
{
    VALIDATE_NOT_NULL(adminArea);
    *adminArea = mAdminArea;
    return NOERROR;
}

ECode Address::SetAdminArea(
    /* [in] */ const String& adminArea)
{
    mAdminArea = adminArea;
    return NOERROR;
}

ECode Address::GetSubAdminArea(
    /* [out] */ String* subAdminArea)
{
    VALIDATE_NOT_NULL(subAdminArea);
    *subAdminArea = mSubAdminArea;
    return NOERROR;
}

ECode Address::SetSubAdminArea(
    /* [in] */ const String& subAdminArea)
{
    mSubAdminArea = subAdminArea;
    return NOERROR;
}

ECode Address::GetLocality(
    /* [out] */ String* locality)
{
    VALIDATE_NOT_NULL(locality);
    *locality = mLocality;
    return NOERROR;
}

ECode Address::SetLocality(
    /* [in] */ const String& locality)
{
    mLocality = locality;
    return NOERROR;
}

ECode Address::GetSubLocality(
    /* [out] */ String* sublocality)
{
    VALIDATE_NOT_NULL(sublocality);
    *sublocality = mSubLocality;
    return NOERROR;
}

ECode Address::SetSubLocality(
    /* [in] */ const String& sublocality)
{
    mSubLocality = sublocality;
    return NOERROR;
}

ECode Address::GetThoroughfare(
    /* [out] */ String* thoroughfare)
{
    VALIDATE_NOT_NULL(thoroughfare);
    *thoroughfare = mThoroughfare;
    return NOERROR;
}

ECode Address::SetThoroughfare(
    /* [in] */ const String& thoroughfare)
{
    mThoroughfare = thoroughfare;
    return NOERROR;
}

ECode Address::GetSubThoroughfare(
    /* [out] */ String* subthoroughfare)
{
    VALIDATE_NOT_NULL(subthoroughfare);
    *subthoroughfare = mSubThoroughfare;
    return NOERROR;
}

ECode Address::SetSubThoroughfare(
    /* [in] */ const String& subthoroughfare)
{
    mSubThoroughfare = subthoroughfare;
    return NOERROR;
}

ECode Address::GetPremises(
    /* [out] */ String* premises)
{
    VALIDATE_NOT_NULL(premises);
    *premises = mPremises;
    return NOERROR;
}

ECode Address::SetPremises(
    /* [in] */ const String& premises)
{
    mPremises = premises;
    return NOERROR;
}

ECode Address::GetPostalCode(
    /* [out] */ String* postalCode)
{
    VALIDATE_NOT_NULL(postalCode);
    *postalCode = mPostalCode;
    return NOERROR;
}

ECode Address::SetPostalCode(
    /* [in] */ const String& postalCode)
{
    mPostalCode = postalCode;
    return NOERROR;
}

ECode Address::GetCountryCode(
    /* [out] */ String* countryCode)
{
    VALIDATE_NOT_NULL(countryCode);
    *countryCode = mCountryCode;
    return NOERROR;
}

ECode Address::SetCountryCode(
    /* [in] */ const String& countryCode)
{
    mCountryCode = countryCode;
    return NOERROR;
}

ECode Address::GetCountryName(
    /* [out] */ String* countryName)
{
    VALIDATE_NOT_NULL(countryName);
    *countryName = mCountryName;
    return NOERROR;
}

ECode Address::SetCountryName(
    /* [in] */ const String& countryName)
{
    mCountryName = countryName;
    return NOERROR;
}

ECode Address::HasLatitude(
    /* [out] */ Boolean* hasLatitude)
{
    VALIDATE_NOT_NULL(hasLatitude);
    *hasLatitude = mHasLatitude;
    return NOERROR;
}

ECode Address::GetLatitude(
    /* [out] */ Double* latitude)
{
    VALIDATE_NOT_NULL(latitude);
    if (mHasLatitude) {
        *latitude = mLatitude;
        return NOERROR;
    }
    else {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
}

ECode Address::SetLatitude(
    /* [in] */ Double latitude)
{
    mLatitude = latitude;
    mHasLatitude = TRUE;
    return NOERROR;
}

ECode Address::ClearLatitude()
{
    mHasLatitude = FALSE;
    return NOERROR;
}

ECode Address::HasLongitude(
    /* [out] */ Boolean* hasLongitude)
{
    VALIDATE_NOT_NULL(hasLongitude);
    *hasLongitude = mHasLongitude;
    return NOERROR;
}

ECode Address::GetLongitude(
    /* [out] */ Double* longitude)
{
    VALIDATE_NOT_NULL(longitude);

    if (mHasLongitude) {
        *longitude =mLongitude;
        return NOERROR;
    }
    else {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
}

ECode Address::SetLongitude(
    /* [in] */ Double longitude)
{
    mLongitude = longitude;
    mHasLongitude = TRUE;
    return NOERROR;
}

ECode Address::ClearLongitude()
{
    mHasLongitude = FALSE;
    return NOERROR;
}

ECode Address::GetPhone(
    /* [out] */ String* phone)
{
    VALIDATE_NOT_NULL(phone);
    *phone = mPhone;
    return NOERROR;
}

ECode Address::SetPhone(
    /* [in] */ const String& phone)
{
    mPhone = phone;
    return NOERROR;
}

ECode Address::GetUrl(
    /* [out] */ String* url)
{
    VALIDATE_NOT_NULL(url);
    *url = mUrl;
    return NOERROR;
}

ECode Address::SetUrl(
    /* [in] */ const String& Url)
{
    mUrl = Url;
    return NOERROR;
}

ECode Address::GetExtras(
    /* [out] */ IBundle** extras)
{
    VALIDATE_NOT_NULL(extras);
    *extras = mExtras.Get();
    REFCOUNT_ADD(*extras);
    return NOERROR;
}

ECode Address::SetExtras(
    /* [in] */ IBundle* extras)
{
    if (extras != NULL) {
        ASSERT_SUCCEEDED(CBundle::New(extras, (IBundle**)&mExtras));
    }
    else {
        mExtras = NULL;
    }
    return NOERROR;
}

ECode Address::ToString(
    /* [out] */ String* result)
{
    StringBuilder sb("Address[addressLines=[");
    for (Int32 i = 0; i <= mMaxAddressLineIndex; i++) {
        if (i > 0) {
            sb += ',';
        }
        sb += i;
        sb += ':';

        AutoPtr<IInteger32> k;
        CInteger32::New(i, (IInteger32**)&k);
        AutoPtr<IInterface> obj;
        mAddressLines->Get(k.Get(), (IInterface**)&obj);
        String line;
        ICharSequence::Probe(obj)->ToString(&line);

        if (line.IsNull()) {
            sb += "null";
        }
        else {
            sb += '\"';
            sb += line;
            sb += '\"';
        }
    }
    sb += ']';
    sb += ",feature=";
    sb += mFeatureName;
    sb += ",admin=";
    sb += mAdminArea;
    sb += ",sub-admin=";
    sb += mSubAdminArea;
    sb += ",locality=";
    sb += mLocality;
    sb += ",thoroughfare=";
    sb += mThoroughfare;
    sb += ",postalCode=";
    sb += mPostalCode;
    sb += ",countryCode=";
    sb += mCountryCode;
    sb += ",countryName=";
    sb += mCountryName;
    sb += ",hasLatitude=";
    sb += mHasLatitude;
    sb += ",latitude=";
    sb += mLatitude;
    sb += ",hasLongitude=";
    sb += mHasLongitude;
    sb += ",longitude=";
    sb += mLongitude;
    sb += ",phone=";
    sb += mPhone;
    sb += ",url=";
    sb += mUrl;
    sb += ",extras=";
    sb += mExtras;
    sb += ']';
    *result = sb.ToString();
    return NOERROR;
}

ECode Address::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    String language, country;
    source->ReadString(&language);
    source->ReadString(&country);
    AutoPtr<ILocale> locale;
    !country.IsNullOrEmpty() ?
        CLocale::New(language, country, (ILocale**)&locale) :
        CLocale::New(language, (ILocale**)&locale);
    Int32 N;
    source->ReadInt32(&N);
    if (N > 0) {
        CHashMap::New(N, (IHashMap**)&mAddressLines);
        for (Int32 i = 0; i < N; i++) {
            Int32 index;
            source->ReadInt32(&index);
            String line;
            source->ReadString(&line);
            AutoPtr<IInteger32> k;
            CInteger32::New(index, (IInteger32**)&k);
            AutoPtr<ICharSequence> v;
            CString::New(line, (ICharSequence**)&v);
            mAddressLines->Put(k.Get(), v.Get());
            mMaxAddressLineIndex = Elastos::Core::Math::Max(mMaxAddressLineIndex, index);
        }
    }
    else {
        mAddressLines = NULL;
        mMaxAddressLineIndex = -1;
    }
    source->ReadString(&mFeatureName);
    source->ReadString(&mAdminArea);
    source->ReadString(&mSubAdminArea);
    source->ReadString(&mLocality);
    source->ReadString(&mSubLocality);
    source->ReadString(&mThoroughfare);
    source->ReadString(&mSubThoroughfare);
    source->ReadString(&mPremises);
    source->ReadString(&mPostalCode);
    source->ReadString(&mCountryCode);
    source->ReadString(&mCountryName);
    Int32 value;
    source->ReadInt32(&value);
    mHasLatitude = value == 0 ? FALSE : TRUE;
    if (mHasLatitude) {
        source->ReadDouble(&mLatitude);
    }
    source->ReadInt32(&value);
    mHasLongitude = value == 0 ? FALSE : TRUE;
    if (mHasLongitude) {
        source->ReadDouble(&mLongitude);
    }
    source->ReadString(&mPhone);
    source->ReadString(&mUrl);

    AutoPtr<IInterface> info;
    source->ReadInterfacePtr((Handle32*)&info);
    mExtras = (info != NULL)? IBundle::Probe(info) : NULL;
    return NOERROR;
}

ECode Address::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    String language;
    mLocale->GetLanguage(&language);
    parcel->WriteString(language);
    String country;
    mLocale->GetCountry(&country);
    parcel->WriteString(country);
    if (mAddressLines == NULL) {
        parcel->WriteInt32(0);
    }
    else {
        AutoPtr<ISet> entries;
        mAddressLines->GetEntrySet((ISet**)&entries);
        Int32 entriesSize;
        entries->GetSize(&entriesSize);
        parcel->WriteInt32(entriesSize);

        AutoPtr<IIterator> iter;
        entries->GetIterator((IIterator**)&iter);
        Boolean hasNext;
        while(iter->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            iter->GetNext((IInterface**)&obj);
            AutoPtr<IMapEntry> e = IMapEntry::Probe(obj);
            AutoPtr<IInterface> ko, vo;
            e->GetKey((IInterface**)&ko);
            e->GetValue((IInterface**)&vo);
            Int32 key;
            IInteger32::Probe(ko)->GetValue(&key);
            String value;
            ICharSequence::Probe(vo)->ToString(&value);
            parcel->WriteInt32(key);
            parcel->WriteString(value);
        }
    }
    parcel->WriteString(mFeatureName);
    parcel->WriteString(mAdminArea);
    parcel->WriteString(mSubAdminArea);
    parcel->WriteString(mLocality);
    parcel->WriteString(mSubLocality);
    parcel->WriteString(mThoroughfare);
    parcel->WriteString(mSubThoroughfare);
    parcel->WriteString(mPremises);
    parcel->WriteString(mPostalCode);
    parcel->WriteString(mCountryCode);
    parcel->WriteString(mCountryName);
    parcel->WriteInt32(mHasLatitude ? 1 : 0);
    if (mHasLatitude) {
        parcel->WriteDouble(mLatitude);
    }
    parcel->WriteInt32(mHasLongitude ? 1 : 0);
    if (mHasLongitude){
        parcel->WriteDouble(mLongitude);
    }
    parcel->WriteString(mPhone);
    parcel->WriteString(mUrl);
    parcel->WriteInterfacePtr(mExtras);
    return NOERROR;
}

}//namespace Location
}//namespace Droid
}//namespace Elastos
