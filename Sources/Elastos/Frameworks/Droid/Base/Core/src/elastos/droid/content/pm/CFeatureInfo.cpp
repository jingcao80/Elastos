
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/pm/CFeatureInfo.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL_2(CFeatureInfo, Object, IFeatureInfo, IParcelable)

CAR_OBJECT_IMPL(CFeatureInfo)

CFeatureInfo::CFeatureInfo()
    : mReqGlEsVersion(0)
    , mFlags(0)
{}

CFeatureInfo::~CFeatureInfo()
{}

ECode CFeatureInfo::constructor()
{
    return NOERROR;
}

ECode CFeatureInfo::constructor(
    /* [in] */ IFeatureInfo* orig)
{
    assert(orig != NULL);
    orig->GetName(&mName);
    orig->GetReqGlEsVersion(&mReqGlEsVersion);
    orig->GetFlags(&mFlags);
    return NOERROR;
}

ECode CFeatureInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(source != NULL);
    source->ReadString(&mName);
    source->ReadInt32(&mReqGlEsVersion);
    source->ReadInt32(&mFlags);
    return NOERROR;
}

ECode CFeatureInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(dest != NULL);
    dest->WriteString(mName);
    dest->WriteInt32(mReqGlEsVersion);
    dest->WriteInt32(mFlags);
    return NOERROR;
}

ECode CFeatureInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    // if (name != null) {
    //     return "FeatureInfo{"
    //             + Integer.toHexString(System.identityHashCode(this))
    //             + " " + name + " fl=0x" + Integer.toHexString(flags) + "}";
    // } else {
    //     return "FeatureInfo{"
    //             + Integer.toHexString(System.identityHashCode(this))
    //             + " glEsVers=" + getGlEsVersion()
    //             + " fl=0x" + Integer.toHexString(flags) + "}";
    // }
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CFeatureInfo::GetGlEsVersion(
    /* [out] */ String* ver)
{
    VALIDATE_NOT_NULL(ver);

    Int32 major = ((mReqGlEsVersion & 0xffff0000) >> 16);
    Int32 minor = mReqGlEsVersion & 0x0000ffff;
    *ver = StringUtils::ToString(major) + String(".") + StringUtils::ToString(minor);
    return NOERROR;
}

ECode CFeatureInfo::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode CFeatureInfo::SetName(
    /* [in] */ const String& name)
{
    mName = name;
    return NOERROR;
}

ECode CFeatureInfo::GetReqGlEsVersion(
    /* [out] */ Int32* reqGlEsVersion)
{
    VALIDATE_NOT_NULL(reqGlEsVersion);
    *reqGlEsVersion = mReqGlEsVersion;
    return NOERROR;
}

ECode CFeatureInfo::SetReqGlEsVersion(
    /* [in] */ Int32 reqGlEsVersion)
{
    mReqGlEsVersion = reqGlEsVersion;
    return NOERROR;
}

ECode CFeatureInfo::GetFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags);
    *flags = mFlags;
    return NOERROR;
}

ECode CFeatureInfo::SetFlags(
    /* [in] */ Int32 flags)
{
    mFlags = flags;
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
