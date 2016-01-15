
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/content/pm/CPackageInstallerSessionInfo.h"
#include "elastos/droid/content/CIntent.h"

using Elastos::Droid::Content::CIntent;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL_2(CPackageInstallerSessionInfo, Object, IPackageInstallerSessionInfo, IParcelable)

CAR_OBJECT_IMPL(CPackageInstallerSessionInfo)

CPackageInstallerSessionInfo::CPackageInstallerSessionInfo()
    : mSessionId(0)
    , mProgress(0)
    , mSealed(FALSE)
    , mActive(FALSE)
    , mMode(0)
    , mSizeBytes(0)
{}

CPackageInstallerSessionInfo::~CPackageInstallerSessionInfo()
{}

ECode CPackageInstallerSessionInfo::constructor()
{
    return NOERROR;
}

ECode CPackageInstallerSessionInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mSessionId);
    dest->WriteString(mInstallerPackageName);
    dest->WriteString(mResolvedBaseCodePath);
    dest->WriteFloat(mProgress);
    dest->WriteInt32(mSealed ? 1 : 0);
    dest->WriteInt32(mActive ? 1 : 0);

    dest->WriteInt32(mMode);
    dest->WriteInt64(mSizeBytes);
    dest->WriteString(mAppPackageName);
    dest->WriteInterfacePtr(mAppIcon);
    dest->WriteString(Object::ToString(mAppLabel));
    return NOERROR;
}

ECode CPackageInstallerSessionInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mSessionId);
    source->ReadString(&mInstallerPackageName);
    source->ReadString(&mResolvedBaseCodePath);
    source->ReadFloat(&mProgress);
    Int32 ival;
    source->ReadInt32(&ival);
    mSealed = ival != 0;
    source->ReadInt32(&ival);
    mActive = ival != 0;

    source->ReadInt32(&mMode);
    source->ReadInt64(&mSizeBytes);
    source->ReadString(&mAppPackageName);
    source->ReadInterfacePtr((Handle32*)&mAppIcon);
    String str;
    source->ReadString(&str);
    CString::New(str, (ICharSequence**)&mAppLabel);
    return NOERROR;
}

ECode CPackageInstallerSessionInfo::GetSessionId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mSessionId;
    return NOERROR;
}

ECode CPackageInstallerSessionInfo::GetInstallerPackageName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mInstallerPackageName;
    return NOERROR;
}

ECode CPackageInstallerSessionInfo::GetProgress(
    /* [out] */ Float* progress)
{
    VALIDATE_NOT_NULL(progress)
    *progress = mProgress;
    return NOERROR;
}

ECode CPackageInstallerSessionInfo::IsActive(
    /* [out] */ Boolean* active)
{
    VALIDATE_NOT_NULL(active)
    *active = mActive;
    return NOERROR;
}

ECode CPackageInstallerSessionInfo::IsOpen(
    /* [out] */ Boolean* open)
{
    VALIDATE_NOT_NULL(open)
    *open = mActive;
    return NOERROR;
}

ECode CPackageInstallerSessionInfo::GetAppPackageName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mAppPackageName;
    return NOERROR;
}

ECode CPackageInstallerSessionInfo::GetAppIcon(
    /* [out] */ IBitmap** icon)
{
    VALIDATE_NOT_NULL(icon)
    *icon = mAppIcon;
    REFCOUNT_ADD(*icon)
    return NOERROR;
}

ECode CPackageInstallerSessionInfo::GetAppLabel(
    /* [out] */ ICharSequence** label)
{
    VALIDATE_NOT_NULL(label)
    *label = mAppLabel;
    REFCOUNT_ADD(*label)
    return NOERROR;
}

ECode CPackageInstallerSessionInfo::CreateDetailsIntent(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)

    CIntent::New(IPackageInstaller::ACTION_SESSION_DETAILS, intent);
    (*intent)->PutExtra(IPackageInstaller::EXTRA_SESSION_ID, mSessionId);
    (*intent)->SetPackage(mInstallerPackageName);
    (*intent)->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    return NOERROR;
}

ECode CPackageInstallerSessionInfo::GetDetailsIntent(
    /* [out] */ IIntent** intent)
{
    return CreateDetailsIntent(intent);
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos