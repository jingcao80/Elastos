
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/content/pm/CPackageInstallerSessionParams.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/net/Uri.h"

using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL_2(CPackageInstallerSessionParams, Object, IPackageInstallerSessionInfo, IParcelable)

CAR_OBJECT_IMPL(CPackageInstallerSessionParams)

CPackageInstallerSessionParams::CPackageInstallerSessionParams()
    : mMode(IPackageInstallerSessionParams::MODE_INVALID)
    , mInstallFlags(0)
    , mInstallLocation(IPackageInfo::INSTALL_LOCATION_INTERNAL_ONLY)
    , mSizeBytes(-1)
    , mAppIconLastModified(-1)
{
}

CPackageInstallerSessionParams::~CPackageInstallerSessionParams()
{
}

ECode CPackageInstallerSessionParams::constructor()
{
    return NOERROR;
}

ECode CPackageInstallerSessionParams::constructor(
    /* [in] */ Int32 mode)
{
    mMode = mode;
    return NOERROR;
}

ECode CPackageInstallerSessionParams::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mMode);
    dest->WriteInt32(mInstallFlags);
    dest->WriteInt32(mInstallLocation);
    dest->WriteInt64(mSizeBytes);
    dest->WriteString(mAppPackageName);
    dest->WriteInterfacePtr(mAppIcon);
    dest->WriteString(mAppLabel);
    dest->WriteInterfacePtr(mOriginatingUri);
    dest->WriteInterfacePtr(mReferrerUri);
    dest->WriteString(mAbiOverride);
    return NOERROR;
}

ECode CPackageInstallerSessionParams::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mMode);
    source->ReadInt32(&mInstallFlags);
    source->ReadInt32(&mInstallLocation);
    source->ReadInt64(&mSizeBytes);
    source->ReadString(&mAppPackageName);
    AutoPtr<IInterface> appIcon;
    source->ReadInterfacePtr((Handle32*)(IInterface**)&appIcon);
    mAppIcon = IBitmap::Probe(appIcon);
    source->ReadString(&mAppLabel);
    AutoPtr<IInterface> uri;
    source->ReadInterfacePtr((Handle32*)(IInterface**)&uri);
    mOriginatingUri = IUri::Probe(uri);
    uri = NULL;
    source->ReadInterfacePtr((Handle32*)(IInterface**)&uri);
    mReferrerUri = IUri::Probe(uri);
    source->ReadString(&mAbiOverride);
    return NOERROR;
}

ECode CPackageInstallerSessionParams::SetMode(
    /* [in] */ Int32 mode)
{
    mMode = mode;
    return NOERROR;
}

ECode CPackageInstallerSessionParams::SetInstallFlags(
    /* [in] */ Int32 flags)
{
    mInstallFlags = flags;
    return NOERROR;
}

ECode CPackageInstallerSessionParams::SetInstallLocation(
    /* [in] */ Int32 installLocation)
{
    mInstallLocation = installLocation;
    return NOERROR;
}

ECode CPackageInstallerSessionParams::SetSizeBytes(
    /* [in] */ Int64 bytes)
{
    mSizeBytes = bytes;
    return NOERROR;
}

ECode CPackageInstallerSessionParams::SetSize(
    /* [in] */ Int64 sizeBytes)
{
    mSizeBytes = sizeBytes;
    return NOERROR;
}

ECode CPackageInstallerSessionParams::SetAppPackageName(
    /* [in] */ const String& appPackageName)
{
    mAppPackageName = appPackageName;
    return NOERROR;
}

ECode CPackageInstallerSessionParams::SetAppIcon(
    /* [in] */ IBitmap* appIcon)
{
    mAppIcon = appIcon;
    return NOERROR;
}

ECode CPackageInstallerSessionParams::SetAppLabel(
    /* [in] */ ICharSequence* appLabel)
{
    mAppLabel = (appLabel != NULL) ? Object::ToString(appLabel) : String(NULL);
    return NOERROR;
}

ECode CPackageInstallerSessionParams::SetAppIconLastModified(
    /* [in] */ Int64 lastModified)
{
    mAppIconLastModified = lastModified;
    return NOERROR;
}

ECode CPackageInstallerSessionParams::SetOriginatingUri(
    /* [in] */ IUri* originatingUri)
{
    mOriginatingUri = originatingUri;
    return NOERROR;
}

ECode CPackageInstallerSessionParams::SetReferrerUri(
    /* [in] */ IUri* referrerUri)
{
    mReferrerUri = referrerUri;
    return NOERROR;
}

/** {@hide} */
ECode CPackageInstallerSessionParams::SetInstallFlagsInternal()
{
    mInstallFlags |= IPackageManager::INSTALL_INTERNAL;
    mInstallFlags &= ~IPackageManager::INSTALL_EXTERNAL;
    return NOERROR;
}

/** {@hide} */
ECode CPackageInstallerSessionParams::SetInstallFlagsExternal()
{
    mInstallFlags |= IPackageManager::INSTALL_EXTERNAL;
    mInstallFlags &= ~IPackageManager::INSTALL_INTERNAL;
    return NOERROR;
}

ECode CPackageInstallerSessionParams::SetAbiOverride(
    /* [in] */ const String& abiOverride)
{
    mAbiOverride = abiOverride;
    return NOERROR;
}

ECode CPackageInstallerSessionParams::GetMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode)
    *mode = mMode;
    return NOERROR;
}

ECode CPackageInstallerSessionParams::GetInstallFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags)
    *flags = mInstallFlags;
    return NOERROR;
}

ECode CPackageInstallerSessionParams::GetInstallLocation(
    /* [out] */ Int32* loc)
{
    VALIDATE_NOT_NULL(loc)
    *loc = mInstallLocation;
    return NOERROR;
}

ECode CPackageInstallerSessionParams::GetSizeBytes(
    /* [out] */ Int64* bytes)
{
    VALIDATE_NOT_NULL(bytes)
    *bytes = mSizeBytes;
    return NOERROR;
}

ECode CPackageInstallerSessionParams::GetAppPackageName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mAppPackageName;
    return NOERROR;
}

ECode CPackageInstallerSessionParams::GetAppIcon(
    /* [out] */ IBitmap** icon)
{
    VALIDATE_NOT_NULL(icon)
    *icon = mAppIcon;
    REFCOUNT_ADD(*icon)
    return NOERROR;
}

ECode CPackageInstallerSessionParams::GetAppLabel(
    /* [out] */ String* label)
{
    VALIDATE_NOT_NULL(label)
    *label = mAppLabel;
    return NOERROR;
}

ECode CPackageInstallerSessionParams::GetAppIconLastModified(
    /* [out] */ Int64* lastModified)
{
    VALIDATE_NOT_NULL(lastModified)
    *lastModified = mAppIconLastModified;
    return NOERROR;
}

ECode CPackageInstallerSessionParams::GetOriginatingUri(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    *uri = mOriginatingUri;
    REFCOUNT_ADD(*uri)
    return NOERROR;
}

ECode CPackageInstallerSessionParams::GetReferrerUri(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    *uri = mReferrerUri;
    REFCOUNT_ADD(*uri)
    return NOERROR;
}

ECode CPackageInstallerSessionParams::GetAbiOverride(
    /* [out] */ String* override)
{
    VALIDATE_NOT_NULL(override)
    *override = mAbiOverride;
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos