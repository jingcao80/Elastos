
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/location/GeocoderParams.h"

using Elastos::Utility::CLocale;

namespace Elastos {
namespace Droid {
namespace Location {

CAR_INTERFACE_IMPL_2(GeocoderParams, Object, IGeocoderParams, IParcelable)

GeocoderParams::GeocoderParams()
{}

ECode GeocoderParams::constructor()
{
    return NOERROR;
}

ECode GeocoderParams::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ILocale* locale)
{
    mLocale = locale;
    context->GetPackageName(&mPackageName);
    return NOERROR;
}

ECode GeocoderParams::GetLocale(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale);
    *locale = mLocale;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}

ECode GeocoderParams::GetClientPackage(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName);
    *packageName = mPackageName;
    return NOERROR;
}

ECode GeocoderParams::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    String language;
    String country;
    String variant;
    parcel->ReadString(&language);
    parcel->ReadString(&country);
    parcel->ReadString(&variant);
    CLocale::New(language, country, variant, (ILocale**)&mLocale);
    parcel->ReadString(&mPackageName);
    return NOERROR;
}

ECode GeocoderParams::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    VALIDATE_NOT_NULL(parcel);
    String language, country, variant;
    mLocale->GetLanguage(&language);
    mLocale->GetCountry(&country);
    mLocale->GetVariant(&variant);
    parcel->WriteString(language);
    parcel->WriteString(country);
    parcel->WriteString(variant);
    parcel->WriteString(mPackageName);
    return NOERROR;
}

}//namespace Location
}//namespace Droid
}//namespace Elastos
