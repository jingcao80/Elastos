
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/content/pm/PackageItemInfo.h"
#include "elastos/droid/content/pm/PackageManager.h"

using Elastos::Core::CString;
using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL(PackageItemInfo, Object, IPackageItemInfo)

PackageItemInfo::PackageItemInfo()
    : mLabelRes(0)
    , mIcon(0)
    , mThemedIcon(0)
    , mBanner(0)
    , mLogo(0)
    , mShowUserIcon(0)
{}

PackageItemInfo::~PackageItemInfo()
{}

ECode PackageItemInfo::constructor()
{
    return NOERROR;
}

ECode PackageItemInfo::constructor(
    /* [in] */ IPackageItemInfo* origObj)
{
    VALIDATE_NOT_NULL(origObj)

    PackageItemInfo* orig = (PackageItemInfo*)origObj;
    mName = orig->mName;
    if (!mName.IsNull()) mName = mName.Trim();
    mPackageName = orig->mPackageName;
    mLabelRes = orig->mLabelRes;
    mNonLocalizedLabel = orig->mNonLocalizedLabel;
    if (mNonLocalizedLabel != NULL) {
        String str = Object::ToString(mNonLocalizedLabel).Trim();
        mNonLocalizedLabel = NULL;
        CString::New(str, (ICharSequence**)&mNonLocalizedLabel);
    }
    mIcon = orig->mIcon;
    mBanner = orig->mBanner;
    mLogo = orig->mLogo;
    mMetaData = orig->mMetaData;
    mShowUserIcon = orig->mShowUserIcon;
    mThemedIcon = orig->mThemedIcon;

    return NOERROR;
}

ECode PackageItemInfo::LoadLabel(
    /* [in] */ IPackageManager* pm,
    /* [out] */ ICharSequence** label)
{
    VALIDATE_NOT_NULL(label)
    *label = NULL;

    if (mNonLocalizedLabel != NULL) {
        *label = mNonLocalizedLabel;
        REFCOUNT_ADD(*label);
        return NOERROR;
    }
    if (mLabelRes != 0) {
        AutoPtr<ICharSequence> lb;
        pm->GetText(mPackageName, mLabelRes, GetApplicationInfo(), (ICharSequence**)&lb);
        if (lb != NULL) {
            String str;
            lb->ToString(&str);
            return CString::New(str.Trim(), label);
        }
    }
    if (!mName.IsNull()) {
        return CString::New(mName, label);
    }
    return CString::New(mPackageName, label);
}

ECode PackageItemInfo::LoadIcon(
    /* [in] */ IPackageManager* pm,
    /* [out] */ IDrawable** icon)
{
    return ((PackageManager*)pm)->LoadItemIcon(this, GetApplicationInfo(), icon);
}

ECode PackageItemInfo::LoadBanner(
    /* [in] */ IPackageManager* pm,
    /* [out] */ IDrawable** icon)
{
    VALIDATE_NOT_NULL(icon)
    if (mBanner != 0) {
        AutoPtr<IDrawable> dr;
        pm->GetDrawable(mPackageName, mBanner, GetApplicationInfo(), (IDrawable**)&dr);
        if (dr != NULL) {
            *icon = dr;
            REFCOUNT_ADD(*icon)
            return NOERROR;
        }
    }
    return LoadDefaultBanner(pm, icon);
}

ECode PackageItemInfo::LoadDefaultIcon(
    /* [in] */ IPackageManager* pm,
    /* [out] */ IDrawable** icon)
{
    return pm->GetDefaultActivityIcon(icon);
}

ECode PackageItemInfo::LoadDefaultBanner(
    /* [in] */ IPackageManager* pm,
    /* [out] */ IDrawable** icon)
{
    VALIDATE_NOT_NULL(icon)
    *icon = NULL;
    return NOERROR;
}

ECode PackageItemInfo::LoadLogo(
    /* [in] */ IPackageManager* pm,
    /* [out] */ IDrawable** logo)
{
    VALIDATE_NOT_NULL(logo)

    if (mLogo != 0) {
       pm->GetDrawable(mPackageName, mLogo, GetApplicationInfo(), logo);
       if (*logo != NULL) {
           return NOERROR;
       }
    }
    return LoadDefaultLogo(pm, logo);
}

ECode PackageItemInfo::LoadDefaultLogo(
    /* [in] */ IPackageManager* pm,
    /* [out] */ IDrawable** icon)
{
    VALIDATE_NOT_NULL(icon)
    *icon = NULL;
    return NOERROR;
}

ECode PackageItemInfo::LoadXmlMetaData(
    /* [in] */ IPackageManager* pm,
    /* [in] */ const String& name,
    /* [out] */ IXmlResourceParser** resource)
{
    VALIDATE_NOT_NULL(resource)

    if (mMetaData != NULL) {
        Int32 resid = 0;
        mMetaData->GetInt32(name, &resid);
        if (resid != 0) {
            return pm->GetXml(mPackageName, resid, GetApplicationInfo(), resource);
        }
    }
    *resource = NULL;
    return NOERROR;
}

ECode PackageItemInfo::DumpFront(
    /* [in] */ IPrinter* pw,
    /* [in] */ const String& prefix)
{
    // if (name != null) {
    //     pw.println(prefix + "name=" + name);
    // }
    // pw.println(prefix + "packageName=" + packageName);
    // if (labelRes != 0 || nonLocalizedLabel != null || icon != 0 || banner != 0) {
    //     pw.println(prefix + "labelRes=0x" + Integer.toHexString(labelRes)
    //             + " nonLocalizedLabel=" + nonLocalizedLabel
    //             + " icon=0x" + Integer.toHexString(icon)
    //             + " banner=0x" + Integer.toHexString(banner));
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode PackageItemInfo::DumpBack(
    /* [in] */ IPrinter* pw,
    /* [in] */ const String& prefix)
{
    // no back here
    return NOERROR;
}

ECode PackageItemInfo::WriteToParcel(
        /* [in] */ IParcel* dest)
{
    assert(dest != NULL);

    dest->WriteString(mName);
    dest->WriteString(mPackageName);
    dest->WriteInt32(mLabelRes);
    TextUtils::WriteToParcel(mNonLocalizedLabel, dest);
    dest->WriteInt32(mIcon);
    dest->WriteInt32(mLogo);
    dest->WriteInterfacePtr(mMetaData);
    dest->WriteInt32(mBanner);
    dest->WriteInt32(mShowUserIcon);
    dest->WriteInt32(mThemedIcon);
    return NOERROR;
}

ECode PackageItemInfo::ReadFromParcel(
    /* [in] */ IParcel* src)
{
    assert(src != NULL);

    src->ReadString(&mName);
    src->ReadString(&mPackageName);
    src->ReadInt32(&mLabelRes);
    TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(src, (ICharSequence**)&mNonLocalizedLabel);
    src->ReadInt32(&mIcon);
    src->ReadInt32(&mLogo);
    AutoPtr<IInterface> obj;
    src->ReadInterfacePtr((Handle32*)&obj);
    mMetaData = obj != NULL ? IBundle::Probe(obj) : NULL;
    src->ReadInt32(&mBanner);
    src->ReadInt32(&mShowUserIcon);
    src->ReadInt32(&mThemedIcon);
    return NOERROR;
}

AutoPtr<IApplicationInfo> PackageItemInfo::GetApplicationInfo()
{
    return NULL;
}

ECode PackageItemInfo::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mName;
    return NOERROR;
}

ECode PackageItemInfo::SetName(
    /* [in] */ const String& name)
{
    mName = name;
    return NOERROR;
}

ECode PackageItemInfo::GetPackageName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mPackageName;
    return NOERROR;
}

ECode PackageItemInfo::SetPackageName(
    /* [in] */ const String& name)
{
    mPackageName = name;
    return NOERROR;
}

ECode PackageItemInfo::GetLabelRes(
    /* [out] */ Int32* labelRes)
{
    VALIDATE_NOT_NULL(labelRes)
    *labelRes = mLabelRes;
    return NOERROR;
}

ECode PackageItemInfo::SetLabelRes(
    /* [in] */ Int32 labelRes)
{
    mLabelRes = labelRes;
    return NOERROR;
}

ECode PackageItemInfo::GetNonLocalizedLabel(
    /* [out] */ ICharSequence** label)
{
    VALIDATE_NOT_NULL(label)
    *label = mNonLocalizedLabel;
    REFCOUNT_ADD(*label);
    return NOERROR;
}

ECode PackageItemInfo::SetNonLocalizedLabel(
    /* [in] */ ICharSequence* label)
{
    mNonLocalizedLabel = label;
    return NOERROR;
}

ECode PackageItemInfo::GetIcon(
    /* [out] */ Int32* icon)
{
    VALIDATE_NOT_NULL(icon)
    *icon = mIcon;
    return NOERROR;
}

ECode PackageItemInfo::SetIcon(
    /* [in] */ Int32 icon)
{
    mIcon = icon;
    return NOERROR;
}

ECode PackageItemInfo::GetThemedIcon(
    /* [out] */ Int32* themeIcon)
{
    VALIDATE_NOT_NULL(themeIcon)
    *themeIcon = mThemedIcon;
    return NOERROR;
}

ECode PackageItemInfo::SetThemedIcon(
    /* [in] */ Int32 themeIcon)
{
    mThemedIcon = themeIcon;
    return NOERROR;
}

ECode PackageItemInfo::GetBanner(
    /* [out] */ Int32* banner)
{
    VALIDATE_NOT_NULL(banner)
    *banner = mBanner;
    return NOERROR;
}

ECode PackageItemInfo::SetBanner(
    /* [in] */ Int32 banner)
{
    mBanner = banner;
    return NOERROR;
}

ECode PackageItemInfo::GetLogo(
    /* [out] */ Int32* logo)
{
    VALIDATE_NOT_NULL(logo)
    *logo = mLogo;
    return NOERROR;
}

ECode PackageItemInfo::SetLogo(
    /* [in] */ Int32 logo)
{
    mLogo = logo;
    return NOERROR;
}

ECode PackageItemInfo::GetMetaData(
    /* [out] */ IBundle** metaData)
{
    VALIDATE_NOT_NULL(metaData)
    *metaData = mMetaData;
    REFCOUNT_ADD(*metaData);
    return NOERROR;
}

ECode PackageItemInfo::SetMetaData(
    /* [in] */ IBundle* metaData)
{
    mMetaData = metaData;
    return NOERROR;
}

ECode PackageItemInfo::GetShowUserIcon(
    /* [out] */ Int32* icon)
{
    VALIDATE_NOT_NULL(icon)
    *icon = mShowUserIcon;
    return NOERROR;
}

ECode PackageItemInfo::SetShowUserIcon(
    /* [in] */ Int32 icon)
{
    mShowUserIcon = icon;
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
