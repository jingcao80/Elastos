#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/os/UserHandle.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CUserHandleHelper, Singleton, IUserHandleHelper)

CAR_SINGLETON_IMPL(CUserHandleHelper)

ECode CUserHandleHelper::IsSameUser(
    /* [in] */ Int32 uid1,
    /* [in] */ Int32 uid2,
    /* [out] */ Boolean* isSameUser)
{
    VALIDATE_NOT_NULL(isSameUser);
    *isSameUser = UserHandle::IsSameUser(uid1, uid2);
    return NOERROR;
}

ECode CUserHandleHelper::IsSameApp(
    /* [in] */ Int32 uid1,
    /* [in] */ Int32 uid2,
    /* [out] */ Boolean* isSameApp)
{
    VALIDATE_NOT_NULL(isSameApp);
    *isSameApp = UserHandle::IsSameApp(uid1, uid2);
    return NOERROR;
}

ECode CUserHandleHelper::IsIsolated(
    /* [in] */ Int32 uid,
    /* [out] */ Boolean* isIsolated)
{
    VALIDATE_NOT_NULL(isIsolated);
    *isIsolated = UserHandle::IsIsolated(uid);
    return NOERROR;
}

ECode CUserHandleHelper::IsApp(
    /* [in] */ Int32 uid,
    /* [out] */ Boolean* isApp)
{
    VALIDATE_NOT_NULL(isApp);
    *isApp = UserHandle::IsApp(uid);
    return NOERROR;
}

ECode CUserHandleHelper::GetUserId(
    /* [in] */ Int32 uid,
    /* [out] */ Int32* userId)
{
    VALIDATE_NOT_NULL(userId);
    *userId = UserHandle::GetUserId(uid);
    return NOERROR;
}

ECode CUserHandleHelper::GetCallingUserId(
    /* [out] */ Int32* userId)
{
    VALIDATE_NOT_NULL(userId);
    *userId = UserHandle::GetCallingUserId();
    return NOERROR;
}

ECode CUserHandleHelper::GetUid(
    /* [in] */ Int32 userId,
    /* [in] */ Int32 appId,
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid);
    *uid = UserHandle::GetUid(userId, appId);
    return NOERROR;
}

ECode CUserHandleHelper::GetAppId(
    /* [in] */ Int32 uid,
    /* [out] */ Int32* appId)
{
    VALIDATE_NOT_NULL(appId);
    *appId = UserHandle::GetAppId(uid);
    return NOERROR;
}

ECode CUserHandleHelper::GetSharedAppGid(
    /* [in] */ Int32 id,
    /* [out] */ Int32* appGid)
{
    VALIDATE_NOT_NULL(appGid);
    *appGid = UserHandle::GetSharedAppGid(id);
    return NOERROR;
}

ECode CUserHandleHelper::FormatUid(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ Int32 uid)
{
    return UserHandle::FormatUid(sb, uid);
}

ECode CUserHandleHelper::FormatUid(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int32 uid)
{
    return UserHandle::FormatUid(pw, uid);
}

ECode CUserHandleHelper::GetMyUserId(
    /* [out] */ Int32* myUserId)
{
    VALIDATE_NOT_NULL(myUserId);
    *myUserId = UserHandle::GetMyUserId();
    return NOERROR;
}

ECode CUserHandleHelper::ReadFromParcel(
    /* [in] */ IParcel* in,
    /* [out] */ IUserHandle** h)
{
    return UserHandle::ReadFromParcel(in, h);
}

ECode CUserHandleHelper::WriteToParcel(
    /* [in] */ IUserHandle* h,
    /* [in] */ IParcel* out)
{
    return UserHandle::WriteToParcel(h, out);
}

ECode CUserHandleHelper::GetALL(
    /* [out] */ IUserHandle** h)
{
    VALIDATE_NOT_NULL(h);
    *h = UserHandle::ALL;
    REFCOUNT_ADD(*h);
    return NOERROR;
}

ECode CUserHandleHelper::GetCURRENT(
    /* [out] */ IUserHandle** h)
{
    VALIDATE_NOT_NULL(h);
    *h = UserHandle::CURRENT;
    REFCOUNT_ADD(*h);
    return NOERROR;
}

ECode CUserHandleHelper::GetCURRENT_OR_SELF(
    /* [out] */ IUserHandle** h)
{
    VALIDATE_NOT_NULL(h);
    *h = UserHandle::CURRENT_OR_SELF;
    REFCOUNT_ADD(*h);
    return NOERROR;
}

ECode CUserHandleHelper::GetOWNER(
    /* [out] */ IUserHandle** h)
{
    VALIDATE_NOT_NULL(h);
    *h = UserHandle::OWNER;
    REFCOUNT_ADD(*h);
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
