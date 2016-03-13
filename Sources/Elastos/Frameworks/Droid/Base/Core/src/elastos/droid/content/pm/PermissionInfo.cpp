
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/pm/PermissionInfo.h"
#include "elastos/droid/text/TextUtils.h"

using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL_2(PermissionInfo, PackageItemInfo, IPermissionInfo, IParcelable)

PermissionInfo::PermissionInfo()
    : mProtectionLevel(0)
    , mFlags(0)
    , mDescriptionRes(0)
{}

PermissionInfo::~PermissionInfo()
{}

ECode PermissionInfo::constructor()
{
    return NOERROR;
}

ECode PermissionInfo::constructor(
    /* [in] */ IPermissionInfo* orig)
{
    VALIDATE_NOT_NULL(orig)
    PackageItemInfo::constructor((IPackageItemInfo*)orig);
    orig->GetProtectionLevel(&mProtectionLevel);
    orig->GetGroup(&mGroup);
    orig->GetFlags(&mFlags);
    orig->GetDescriptionRes(&mDescriptionRes);
    orig->GetNonLocalizedDescription((ICharSequence**)&mNonLocalizedDescription);
    return NOERROR;
}

Int32 PermissionInfo::FixProtectionLevel(
    /* [in] */ Int32 level)
{
    if (level == PROTECTION_SIGNATURE_OR_SYSTEM) {
        level = PROTECTION_SIGNATURE | PROTECTION_FLAG_SYSTEM;
    }
    return level;
}

String PermissionInfo::ProtectionToString(
    /* [in] */ Int32 level)
{
    String protLevel("????");
    switch (level & PROTECTION_MASK_BASE) {
        case PROTECTION_DANGEROUS:
        {
            protLevel = String("dangerous");
            break;
        }
        case PROTECTION_NORMAL:
        {
            protLevel = String("normal");
            break;
        }
        case PROTECTION_SIGNATURE:
        {
            protLevel = String("signature");
            break;
        }
        case PROTECTION_SIGNATURE_OR_SYSTEM:
        {
            protLevel = String("signatureOrSystem");
            break;
        }
    }
    if ((level & IPermissionInfo::PROTECTION_FLAG_SYSTEM) != 0) {
        protLevel += String("|system");
    }
    if ((level & IPermissionInfo::PROTECTION_FLAG_DEVELOPMENT) != 0) {
        protLevel += String("|development");
    }
    if ((level& IPermissionInfo::PROTECTION_FLAG_APPOP) != 0) {
        protLevel += String("|appop");
    }

    return protLevel;
}

ECode PermissionInfo::LoadDescription(
    /* [in] */ IPackageManager* pm,
    /* [out] */ ICharSequence** description)
{
    VALIDATE_NOT_NULL(description);
    *description = NULL;

    if (mNonLocalizedDescription != NULL) {
        *description = mNonLocalizedDescription;
        REFCOUNT_ADD(*description);
        return NOERROR;
    }

    if (mDescriptionRes != 0) {
        pm->GetText(mPackageName, mDescriptionRes, NULL, description);
    }

    return NOERROR;
}

ECode PermissionInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    // return "PermissionInfo{"
    //         + Integer.toHexString(System.identityHashCode(this))
    //         + " " + name + "}";
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode PermissionInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    PackageItemInfo::ReadFromParcel(source);
    source->ReadInt32(&mProtectionLevel);
    source->ReadString(&mGroup);
    source->ReadInt32(&mFlags);
    source->ReadInt32(&mDescriptionRes);
    TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(source, (ICharSequence**)&mNonLocalizedDescription);
    return NOERROR;
}

ECode PermissionInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    PackageItemInfo::WriteToParcel(dest);
    dest->WriteInt32(mProtectionLevel);
    dest->WriteString(mGroup);
    dest->WriteInt32(mFlags);
    dest->WriteInt32(mDescriptionRes);
    TextUtils::WriteToParcel(mNonLocalizedDescription, dest);
    return NOERROR;
}

ECode PermissionInfo::LoadLabel(
    /* [in] */ IPackageManager* pm,
    /* [out] */ ICharSequence** label)
{
    return PackageItemInfo::LoadLabel(pm, label);
}

ECode PermissionInfo::LoadIcon(
    /* [in] */ IPackageManager* pm,
    /* [out] */ IDrawable** icon)
{
    return PackageItemInfo::LoadIcon(pm, icon);
}

ECode PermissionInfo::LoadLogo(
    /* [in] */ IPackageManager* pm,
    /* [out] */ IDrawable** logo)
{
    return PackageItemInfo::LoadLogo(pm, logo);
}

ECode PermissionInfo::LoadXmlMetaData(
    /* [in] */ IPackageManager* pm,
    /* [in] */ const String& name,
    /* [out] */ IXmlResourceParser** resource)
{
    VALIDATE_NOT_NULL(resource);
    return PackageItemInfo::LoadXmlMetaData(pm, name, resource);
}

ECode PermissionInfo::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    return PackageItemInfo::GetName(name);
}

ECode PermissionInfo::SetName(
    /* [in] */ const String& name)
{
    return PackageItemInfo::SetName(name);
}

ECode PermissionInfo::GetPackageName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    return PackageItemInfo::GetPackageName(name);
}

ECode PermissionInfo::SetPackageName(
    /* [in] */ const String& name)
{
    return PackageItemInfo::SetPackageName(name);
}

ECode PermissionInfo::GetLabelRes(
    /* [out] */ Int32* labelRes)
{
    VALIDATE_NOT_NULL(labelRes);
    return PackageItemInfo::GetLabelRes(labelRes);
}

ECode PermissionInfo::SetLabelRes(
    /* [in] */ Int32 labelRes)
{
    return PackageItemInfo::SetLabelRes(labelRes);
}

ECode PermissionInfo::GetNonLocalizedLabel(
    /* [out] */ ICharSequence** label)
{
    VALIDATE_NOT_NULL(label);
    return PackageItemInfo::GetNonLocalizedLabel(label);
}

ECode PermissionInfo::SetNonLocalizedLabel(
    /* [in] */ ICharSequence*  label)
{
    return PackageItemInfo::SetNonLocalizedLabel(label);
}

ECode PermissionInfo::GetIcon(
    /* [out] */ Int32* icon)
{
    VALIDATE_NOT_NULL(icon);
    return PackageItemInfo::GetIcon(icon);
}

ECode PermissionInfo::SetIcon(
    /* [in] */ Int32 icon)
{
    return PackageItemInfo::SetIcon(icon);
}

ECode PermissionInfo::GetLogo(
    /* [out] */ Int32* logo)
{
    VALIDATE_NOT_NULL(logo);
    return PackageItemInfo::GetLogo(logo);
}

ECode PermissionInfo::SetLogo(
    /* [in] */ Int32 logo)
{
    return PackageItemInfo::SetLogo(logo);
}

ECode PermissionInfo::GetMetaData(
    /* [out] */ IBundle** metaData)
{
    VALIDATE_NOT_NULL(metaData);
    return PackageItemInfo::GetMetaData(metaData);
}

ECode PermissionInfo::SetMetaData(
    /* [in] */ IBundle* metaData)
{
    return PackageItemInfo::SetMetaData(metaData);
}

ECode PermissionInfo::GetProtectionLevel(
    /* [out] */ Int32* level)
{
    VALIDATE_NOT_NULL(level);
    *level = mProtectionLevel;
    return NOERROR;
}

ECode PermissionInfo::SetProtectionLevel(
    /* [in] */ Int32 level)
{
    mProtectionLevel = level;
    return NOERROR;
}

ECode PermissionInfo::GetGroup(
    /* [out] */ String* group)
{
    VALIDATE_NOT_NULL(group);
    *group = mGroup;
    return NOERROR;
}

ECode PermissionInfo::SetGroup(
    /* [in] */ const String& group)
{
    mGroup = group;
    return NOERROR;
}

ECode PermissionInfo::GetFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags);
    *flags = mFlags;
    return NOERROR;
}

ECode PermissionInfo::SetFlags(
    /* [in] */ Int32 flags)
{
    mFlags = flags;
    return NOERROR;
}

ECode PermissionInfo::GetDescriptionRes(
    /* [out] */ Int32* desRes)
{
    VALIDATE_NOT_NULL(desRes);
    *desRes = mDescriptionRes;
    return NOERROR;
}

ECode PermissionInfo::SetDescriptionRes(
    /* [in] */ Int32 desRes)
{
    mDescriptionRes = desRes;
    return NOERROR;
}

ECode PermissionInfo::GetNonLocalizedDescription(
    /* [out] */ ICharSequence** des)
{
    VALIDATE_NOT_NULL(des);
    *des = mNonLocalizedDescription;
    REFCOUNT_ADD(*des);
    return NOERROR;
}

ECode PermissionInfo::SetNonLocalizedDescription(
    /* [in] */ ICharSequence* des)
{
    mNonLocalizedDescription = des;
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
