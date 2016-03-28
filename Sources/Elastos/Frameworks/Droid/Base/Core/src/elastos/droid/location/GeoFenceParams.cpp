
#include "elastos/droid/location/GeoFenceParams.h"
#include "elastos/droid/app/CPendingIntent.h"
#include "elastos/droid/os/Binder.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::App::CPendingIntent;
using Elastos::Droid::Os::Binder;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Location {

CAR_INTERFACE_IMPL_2(GeoFenceParams, Object, IGeoFenceParams, IParcelable)

ECode GeoFenceParams::constructor()
{
    return NOERROR;
}

ECode GeoFenceParams::constructor(
    /* [in] */ Double lat,
    /* [in] */ Double lon,
    /* [in] */ Float r,
    /* [in] */ Int64 expire,
    /* [in] */ IPendingIntent* intent,
    /* [in] */ const String& packageName)
{
    return constructor(Binder::GetCallingUid(), lat, lon, r, expire, intent, packageName);
}

ECode GeoFenceParams::constructor(
    /* [in] */ Int32 uid,
    /* [in] */ Double lat,
    /* [in] */ Double lon,
    /* [in] */ Float r,
    /* [in] */ Int64 expire,
    /* [in] */ IPendingIntent* intent,
    /* [in] */ const String& packageName)
{
    mUid = uid;
    mLatitude = lat;
    mLongitude = lon;
    mRadius = r;
    mExpiration = expire;
    mIntent = intent;
    mPackageName = packageName;
    return NOERROR;
}

ECode GeoFenceParams::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadInt32(&mUid);
    in->ReadDouble(&mLatitude);
    in->ReadDouble(&mLongitude);
    in->ReadFloat(&mRadius);
    in->ReadInt64(&mExpiration);
    if (mIntent == NULL)
        CPendingIntent::New((IPendingIntent**)&mIntent);
    IParcelable::Probe(mIntent)->ReadFromParcel(in);
    in->ReadString(&mPackageName);
    return NOERROR;
}

ECode GeoFenceParams::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mUid);
    dest->WriteDouble(mLatitude);
    dest->WriteDouble(mLongitude);
    dest->WriteFloat(mRadius);
    dest->WriteInt64(mExpiration);
    IParcelable::Probe(mIntent)->WriteToParcel(dest);
    dest->WriteString(mPackageName);
    return NOERROR;
}

ECode GeoFenceParams::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb;
    sb.Append("GeoFenceParams:\n\tmUid - ");
    sb.Append(mUid);
    sb.Append("\n\tmLatitide - ");
    sb.Append(mLatitude);
    sb.Append("\n\tmLongitude - ");
    sb.Append(mLongitude);
    sb.Append("\n\tmRadius - ");
    sb.Append(mRadius);
    sb.Append("\n\tmExpiration - ");
    sb.Append(mExpiration);
    sb.Append("\n\tmIntent - ");
    sb.Append(mIntent);
    return sb.ToString(str);
}

ECode GeoFenceParams::GetExpiration(
    /* [out] */ Int64* expiration)
{
    VALIDATE_NOT_NULL(expiration)
    *expiration = mExpiration;
    return NOERROR;
}

ECode GeoFenceParams::GetIntent(
    /* [out] */ IPendingIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    *intent = mIntent;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

ECode GeoFenceParams::GetCallerUid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid)
    *uid = mUid;
    return NOERROR;
}

ECode GeoFenceParams::GetLatitude(
    /* [out] */ Double* latitude)
{
    VALIDATE_NOT_NULL(latitude)
    *latitude = mLatitude;
    return NOERROR;
}

ECode GeoFenceParams::GetLongitude(
    /* [out] */ Double* longitude)
{
    VALIDATE_NOT_NULL(longitude)
    *longitude = mLongitude;
    return NOERROR;
}

ECode GeoFenceParams::GetRadius(
    /* [out] */ Float* radius)
{
    VALIDATE_NOT_NULL(radius)
    *radius = mRadius;
    return NOERROR;
}

ECode GeoFenceParams::GetPackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName)
    *packageName = mPackageName;
    return NOERROR;
}

ECode GeoFenceParams::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    pw->Print(prefix);
    pw->Println((IGeoFenceParams*)this);
    pw->Print(prefix);
    pw->Print(String("mLatitude="));
    pw->Print(mLatitude);
    pw->Print(String(" mLongitude="));
    pw->Println(mLongitude);
    pw->Print(prefix);
    pw->Print(String("mRadius="));
    pw->Print(mRadius);
    pw->Print(String(" mExpiration="));
    pw->Println(mExpiration);
    return NOERROR;
}

}//namespace Location
}//namespace Droid
}//namespace Elastos

