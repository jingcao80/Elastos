
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/pm/CUserInfo.h"
#include "elastos/droid/os/CUserHandle.h"

using Elastos::Droid::Os::CUserHandle;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL_2(CUserInfo, Object, IUserInfo, IParcelable)

CAR_OBJECT_IMPL(CUserInfo)

CUserInfo::CUserInfo()
    : mId(0)
    , mSerialNumber(0)
    , mFlags(0)
    , mCreationTime(0)
    , mLastLoggedInTime(0)
    , mPartial(FALSE)
    , mGuestToRemove(FALSE)
{
    mProfileGroupId = NO_PROFILE_GROUP_ID;
}

CUserInfo::~CUserInfo()
{}

ECode CUserInfo::constructor()
{
    return NOERROR;
}

ECode CUserInfo::constructor(
    /* [in] */ IUserInfo* other)
{
    VALIDATE_NOT_NULL(other)
    CUserInfo* orig = (CUserInfo*)other;

    mName = orig->mName;
    mIconPath = orig->mIconPath;
    mId = orig->mId;
    mFlags = orig->mFlags;
    mSerialNumber = orig->mSerialNumber;
    mCreationTime = orig->mCreationTime;
    mLastLoggedInTime = orig->mLastLoggedInTime;
    mPartial = orig->mPartial;
    mProfileGroupId = orig->mProfileGroupId;
    mGuestToRemove = orig->mGuestToRemove;

    return NOERROR;
}

ECode CUserInfo::constructor(
    /* [in] */ Int32 id,
    /* [in] */ const String& name,
    /* [in] */ Int32 flags)
{
    return constructor(id, name, String(NULL), flags);
}

ECode CUserInfo::constructor(
    /* [in] */ Int32 id,
    /* [in] */ const String& name,
    /* [in] */ const String& iconPath,
    /* [in] */ Int32 flags)
{
    mId = id;
    mName = name;
    mFlags = flags;
    mIconPath = iconPath;
    return NOERROR;
}

ECode CUserInfo::IsPrimary(
    /* [out] */ Boolean* isPrimary)
{
    VALIDATE_NOT_NULL(isPrimary);
    *isPrimary = (mFlags & FLAG_PRIMARY) == FLAG_PRIMARY;
    return NOERROR;
}

ECode CUserInfo::IsAdmin(
    /* [out] */ Boolean* isAdmin)
{
    VALIDATE_NOT_NULL(isAdmin);
    *isAdmin = (mFlags & FLAG_ADMIN) == FLAG_ADMIN;
    return NOERROR;
}

ECode CUserInfo::IsGuest(
    /* [out] */ Boolean* isGuest)
{
    VALIDATE_NOT_NULL(isGuest);
    *isGuest = (mFlags & FLAG_GUEST) == FLAG_GUEST;
    return NOERROR;
}

ECode CUserInfo::IsRestricted(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mFlags & FLAG_RESTRICTED) == FLAG_RESTRICTED;
    return NOERROR;
}

ECode CUserInfo::IsManagedProfile(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mFlags & FLAG_MANAGED_PROFILE) == FLAG_MANAGED_PROFILE;
    return NOERROR;
}

ECode CUserInfo::IsEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mFlags & FLAG_DISABLED) != FLAG_DISABLED;
    return NOERROR;
}

ECode CUserInfo::SupportsSwitchTo(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // TODO remove fw.show_hidden_users when we have finished developing managed profiles.
    // return !isManagedProfile() || SystemProperties.getBoolean("fw.show_hidden_users", false);
    return NOERROR;
}


ECode CUserInfo::GetUserHandle(
    /* [out] */ IUserHandle** userHandle)
{
    VALIDATE_NOT_NULL(userHandle);
    return CUserHandle::New(mId, userHandle);
}

ECode CUserInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    // return "UserInfo{" + id + ":" + name + ":" + Integer.toHexString(flags) + "}";
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CUserInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(source != NULL);
    source->ReadInt32(&mId);
    source->ReadInt32(&mSerialNumber);
    source->ReadString(&mName);
    source->ReadInt32(&mFlags);
    source->ReadInt64(&mCreationTime);
    source->ReadInt64(&mLastLoggedInTime);
    source->ReadString(&mIconPath);
    source->ReadBoolean(&mPartial);
    source->ReadInt32(&mProfileGroupId);
    Int32 ival;
    source->ReadInt32(&ival);
    mGuestToRemove = ival != 0;
    return NOERROR;
}

ECode CUserInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(dest != NULL);
    dest->WriteInt32(mId);
    dest->WriteInt32(mSerialNumber);
    dest->WriteString(mName);
    dest->WriteInt32(mFlags);
    dest->WriteInt64(mCreationTime);
    dest->WriteInt64(mLastLoggedInTime);
    dest->WriteString(mIconPath);
    dest->WriteBoolean(mPartial);
    dest->WriteInt32(mProfileGroupId);
    dest->WriteInt32(mGuestToRemove ? 1 : 0);
    return NOERROR;
}

ECode CUserInfo::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mId;
    return NOERROR;
}

ECode CUserInfo::SetId(
    /* [in] */ Int32 id)
{
    mId = id;
    return NOERROR;
}

ECode CUserInfo::GetSerialNumber(
    /* [out] */ Int32* serialNumber)
{
    VALIDATE_NOT_NULL(serialNumber);
    *serialNumber = mSerialNumber;
    return NOERROR;
}

ECode CUserInfo::SetSerialNumber(
    /* [in] */ Int32 serialNumber)
{
    mSerialNumber = serialNumber;
    return NOERROR;
}

ECode CUserInfo::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode CUserInfo::SetName(
    /* [in] */ const String& name)
{
    mName = name;
    return NOERROR;
}

ECode CUserInfo::GetIconPath(
    /* [out] */ String* iconPath)
{
    VALIDATE_NOT_NULL(iconPath);
    *iconPath = mIconPath;
    return NOERROR;
}

ECode CUserInfo::SetIconPath(
    /* [in] */ const String& iconPath)
{
    mIconPath = iconPath;
    return NOERROR;
}

ECode CUserInfo::GetFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags);
    *flags = mFlags;
    return NOERROR;
}

ECode CUserInfo::SetFlags(
    /* [in] */ Int32 flags)
{
    mFlags = flags;
    return NOERROR;
}

ECode CUserInfo::GetCreationTime(
    /* [out] */ Int64* creationTime)
{
    VALIDATE_NOT_NULL(creationTime);
    *creationTime = mCreationTime;
    return NOERROR;
}

ECode CUserInfo::SetCreationTime(
    /* [in] */ Int64 creationTime)
{
    mCreationTime = creationTime;
    return NOERROR;
}

ECode CUserInfo::GetLastLoggedInTime(
    /* [out] */ Int64* lastLoggedInTime)
{
    VALIDATE_NOT_NULL(lastLoggedInTime);
    *lastLoggedInTime = mLastLoggedInTime;
    return NOERROR;
}

ECode CUserInfo::SetLastLoggedInTime(
    /* [in] */ Int64 lastLoggedInTime)
{
    mLastLoggedInTime = lastLoggedInTime;
    return NOERROR;
}

ECode CUserInfo::GetPartial(
    /* [out] */ Boolean* partial)
{
    VALIDATE_NOT_NULL(partial);
    *partial = mPartial;
    return NOERROR;
}

ECode CUserInfo::SetPartial(
    /* [in] */ Boolean partial)
{
    mPartial = partial;
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
