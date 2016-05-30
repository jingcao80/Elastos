
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/utility/CSparseArray.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Utility::CSparseArray;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Os {

static AutoPtr<IUserHandle> InitUserHandle(Int32 h)
{
#ifdef DROID_CORE
    AutoPtr<CUserHandle> handle;
    CUserHandle::NewByFriend(h, (CUserHandle**)&handle);
    return handle.Get();
#elif defined(DROID_SERVER)
    AutoPtr<IUserHandle> handle;
    CUserHandle::New(h, (IUserHandle**)&handle);
    return handle;
#endif
}

const AutoPtr<IUserHandle> UserHandle::ALL = InitUserHandle(IUserHandle::USER_ALL);
const AutoPtr<IUserHandle> UserHandle::CURRENT = InitUserHandle(IUserHandle::USER_CURRENT);
const AutoPtr<IUserHandle> UserHandle::CURRENT_OR_SELF = InitUserHandle(IUserHandle::USER_CURRENT_OR_SELF);
const AutoPtr<IUserHandle> UserHandle::OWNER = InitUserHandle(IUserHandle::USER_OWNER);

static AutoPtr<ISparseArray> InitmUserHandles()
{
    AutoPtr<ISparseArray> array;
    CSparseArray::New((ISparseArray**)&array);
    return array;
}

AutoPtr<ISparseArray> UserHandle::mUserHandles = InitmUserHandles();

CAR_INTERFACE_IMPL_2(UserHandle, Object, IUserHandle, IParcelable)

UserHandle::UserHandle()
    : mHandle(0)
{
}

UserHandle::UserHandle(
    /* [in] */ Int32 h)
    : mHandle(h)
{
}

ECode UserHandle::constructor()
{
    return NOERROR;
}

ECode UserHandle::constructor(
    /* [in] */ Int32 h)
{
    mHandle = h;
    return NOERROR;
}

ECode UserHandle::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);
    return source->ReadInt32(&mHandle);
}

ECode UserHandle::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);
    return dest->WriteInt32(mHandle);
}

ECode UserHandle::WriteToParcel(
    /* [in] */ IUserHandle* h,
    /* [in] */ IParcel* out)
{
    if (h != NULL) {
        IParcelable* parcelable = IParcelable::Probe(h);
        if (parcelable) {
            return parcelable->WriteToParcel(out);
        }
        return NOERROR;
    }
    else {
        VALIDATE_NOT_NULL(out);
        return out->WriteInt32(IUserHandle::USER_NULL);
    }
}

ECode UserHandle::ReadFromParcel(
    /* [in] */ IParcel* in,
    /* [out] */ IUserHandle** h)
{
    VALIDATE_NOT_NULL(h);
    *h = NULL;
    VALIDATE_NOT_NULL(in);

    Int32 handle;
    FAIL_RETURN(in->ReadInt32(&handle));
    return CUserHandle::New(handle, h);
}

ECode UserHandle::GetIdentifier(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mHandle;
    return NOERROR;
}

ECode UserHandle::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb("UserHandle{");
    sb += mHandle;
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

ECode UserHandle::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* equals)
{
    return Equals(IUserHandle::Probe(other), equals);
}

ECode UserHandle::Equals(
    /* [in] */ IUserHandle* other,
    /* [out] */ Boolean* equals)
{
    VALIDATE_NOT_NULL(equals);
    *equals = FALSE;

    if (other != NULL) {
        Int32 oid;
        other->GetIdentifier(&oid);
        *equals = oid == mHandle;
    }
    return NOERROR;
}

ECode UserHandle::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = mHandle;
    return NOERROR;
}

Boolean UserHandle::IsSameUser(
    /* [in] */ Int32 uid1,
    /* [in] */ Int32 uid2)
{
    return GetUserId(uid1) == GetUserId(uid2);
}

Boolean UserHandle::IsSameApp(
    /* [in] */ Int32 uid1,
    /* [in] */ Int32 uid2)
{
    return GetAppId(uid1) == GetAppId(uid2);
}

Boolean UserHandle::IsIsolated(
    /* [in] */ Int32 uid)
{
    if (uid > 0) {
        Int32 appId = GetAppId(uid);
        return appId >= IProcess::FIRST_ISOLATED_UID && appId <= IProcess::LAST_ISOLATED_UID;
    } else {
        return FALSE;
    }
}

Boolean UserHandle::IsApp(
    /* [in] */ Int32 uid)
{
    if (uid > 0) {
        Int32 appId = GetAppId(uid);
        return appId >= IProcess::FIRST_APPLICATION_UID && appId <= IProcess::LAST_APPLICATION_UID;
    } else {
        return FALSE;
    }
}

Int32 UserHandle::GetUserId(
    /* [in] */ Int32 uid)
{
    if (IUserHandle::MU_ENABLED) {
        return uid / IUserHandle::PER_USER_RANGE;
    } else {
        return 0;
    }
}

Int32 UserHandle::GetCallingUserId()
{
    return GetUserId(Binder::GetCallingUid());
}

AutoPtr<IUserHandle> UserHandle::GetCallingUserHandle()
{
    Int32 userId = GetUserId(Binder::GetCallingUid());
    AutoPtr<IInterface> obj;
    mUserHandles->Get(userId, (IInterface**)&obj);
    AutoPtr<IUserHandle> userHandle = IUserHandle::Probe(obj);
    // Intentionally not synchronized to save time
    if (userHandle == NULL) {
        CUserHandle::New(userId, (IUserHandle**)&userHandle);
        mUserHandles->Put(userId, userHandle);
    }
    return userHandle;
}

Int32 UserHandle::GetUid(
    /* [in] */ Int32 userId,
    /* [in] */ Int32 appId)
{
    if (IUserHandle::MU_ENABLED) {
        return userId * IUserHandle::PER_USER_RANGE + (appId % IUserHandle::PER_USER_RANGE);
    } else {
        return appId;
    }
}

Int32 UserHandle::GetAppId(
    /* [in] */ Int32 uid)
{
    return uid % IUserHandle::PER_USER_RANGE;
}

Int32 UserHandle::GetSharedAppGid(
    /* [in] */ Int32 id)
{
    return IProcess::FIRST_SHARED_APPLICATION_GID + (id % IUserHandle::PER_USER_RANGE)
            - IProcess::FIRST_APPLICATION_UID;
}

ECode UserHandle::FormatUid(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ Int32 uid)
{
    if (uid < IProcess::FIRST_APPLICATION_UID) {
        sb->Append(uid);
    }
    else {
        sb->AppendChar('u');
        sb->Append(GetUserId(uid));
        Int32 appId = GetAppId(uid);
        if (appId >= IProcess::FIRST_ISOLATED_UID && appId <= IProcess::LAST_ISOLATED_UID) {
            sb->AppendChar('i');
            sb->Append(appId - IProcess::FIRST_ISOLATED_UID);
        }
        else if (appId >= IProcess::FIRST_APPLICATION_UID) {
            sb->AppendChar('a');
            sb->Append(appId - IProcess::FIRST_APPLICATION_UID);
        }
        else {
            sb->AppendChar('s');
            sb->Append(appId);
        }
    }
    return NOERROR;
}

ECode UserHandle::FormatUid(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int32 uid)
{
    if (uid < IProcess::FIRST_APPLICATION_UID) {
        pw->Print(uid);
    }
    else {
        pw->PrintChar('u');
        pw->Print(GetUserId(uid));
        Int32 appId = GetAppId(uid);
        if (appId >= IProcess::FIRST_ISOLATED_UID && appId <= IProcess::LAST_ISOLATED_UID) {
            pw->PrintChar('i');
            pw->Print(appId - IProcess::FIRST_ISOLATED_UID);
        }
        else if (appId >= IProcess::FIRST_APPLICATION_UID) {
            pw->PrintChar('a');
            pw->Print(appId - IProcess::FIRST_APPLICATION_UID);
        }
        else {
            pw->PrintChar('s');
            pw->Print(appId);
        }
    }
    return NOERROR;
}

Int32 UserHandle::GetMyUserId()
{
    Int32 uid = Process::MyUid();
    return GetUserId(uid);
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
