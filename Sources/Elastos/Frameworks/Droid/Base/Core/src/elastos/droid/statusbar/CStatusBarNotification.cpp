#include "elastos/droid/statusbar/CStatusBarNotification.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace StatusBar {

CStatusBarNotification::CStatusBarNotification()
    : mId(0)
    , mUid(0)
    , mInitialPid(0)
    , mScore(0)
{
}

ECode CStatusBarNotification::constructor(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 id,
    /* [in] */ const String& tag,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 initialPid,
    /* [in] */ Int32 score,
    /* [in] */ INotification* notification)
{
    AutoPtr<IUserHandle> IUserHandle_OWNER;
    CUserHandle::New(0, (IUserHandle**)&IUserHandle_OWNER);
    return constructor(pkg, id, tag, uid, initialPid, score, notification, IUserHandle_OWNER);
}

ECode CStatusBarNotification::constructor(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 id,
    /* [in] */ const String& tag,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 initialPid,
    /* [in] */ Int32 score,
    /* [in] */ INotification* notification,
    /* [in] */ IUserHandle* user)
{
    if (pkg.IsNull())
        return E_NULL_POINTER_EXCEPTION;
    if (notification == NULL)
        return E_NULL_POINTER_EXCEPTION;

    mPkg = pkg;
    mId = id;
    mTag = tag;
    mUid = uid;
    mInitialPid = initialPid;
    mScore = score;
    mNotification = notification;
    mUser = user;
    mNotification->SetUser(user);

    return NOERROR;
}

ECode CStatusBarNotification::constructor()
{
    return NOERROR;
}

ECode CStatusBarNotification::WriteToParcel(
    /* [in] */ IParcel* out)
{
    out->WriteString(mPkg);
    out->WriteInt32(mId);

    if (!mTag.IsNull()) {
        out->WriteInt32(1);
        out->WriteString(mTag);
    }
    else {
        out->WriteInt32(0);
    }

    out->WriteInt32(mUid);
    out->WriteInt32(mInitialPid);
    out->WriteInt32(mScore);

    out->WriteInterfacePtr((IInterface*)mNotification);
    out->WriteInterfacePtr((IInterface*)mUser);

    return NOERROR;
}

ECode CStatusBarNotification::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    Int32 value;

    in->ReadString(&mPkg);
    in->ReadInt32(&mId);

    in->ReadInt32(&value);
    if (value != 0) {
        in->ReadString(&mTag);
    }
    else {
        mTag = String(NULL);
    }

    in->ReadInt32(&mUid);
    in->ReadInt32(&mInitialPid);
    in->ReadInt32(&mScore);

    mNotification = NULL;
    in->ReadInterfacePtr((Handle32*)(INotification**)&mNotification);

    mUser = NULL;
    in->ReadInterfacePtr((Handle32*)(INotification**)&mUser);

    if (mNotification != NULL && mUser != NULL) {
        mNotification->SetUser(mUser);
    }

    return NOERROR;
}

ECode CStatusBarNotification::IsOngoing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 flags = 0;
    mNotification->GetFlags(&flags);

    *result = (flags & INotification::FLAG_ONGOING_EVENT) != 0;
    return NOERROR;
}

ECode CStatusBarNotification::IsClearable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 flags = 0;
    mNotification->GetFlags(&flags);
    *result = ((flags & INotification::FLAG_ONGOING_EVENT) == 0 ) && ((flags & INotification::FLAG_NO_CLEAR) == 0);
    return NOERROR;
}

/** Returns a userHandle for the instance of the app that posted this notification. */
ECode CStatusBarNotification::GetUserId(
    /* [out] */ Int32* userId)
{
    VALIDATE_NOT_NULL(userId);
    mUser->GetIdentifier(userId);
    return NOERROR;
}

ECode CStatusBarNotification::GetPkg(
    /* [out] */ String* pkg)
{
    VALIDATE_NOT_NULL(pkg)
    *pkg = mPkg;
    return NOERROR;
}

ECode CStatusBarNotification::GetTag(
    /* [out] */ String* tag)
{
    VALIDATE_NOT_NULL(tag)
    *tag = mTag;
    return NOERROR;
}

ECode CStatusBarNotification::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mId;
    return NOERROR;
}

ECode CStatusBarNotification::GetNotification(
    /* [out] */ INotification** n)
{
    VALIDATE_NOT_NULL(n)
    *n = mNotification;
    REFCOUNT_ADD(*n);
    return NOERROR;
}

ECode CStatusBarNotification::GetUid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid)
    *uid = mUid;
    return NOERROR;
}

ECode CStatusBarNotification::GetInitialPid(
    /* [out] */ Int32* initialPid)
{
    VALIDATE_NOT_NULL(initialPid)
    *initialPid = mInitialPid;
    return NOERROR;
}

ECode CStatusBarNotification::GetScore(
    /* [out] */ Int32* score)
{
    VALIDATE_NOT_NULL(score)
    *score = mScore;
    return NOERROR;
}

ECode CStatusBarNotification::GetUser(
    /* [out] */ IUserHandle** user)
{
    VALIDATE_NOT_NULL(user)
    *user = mUser;
    REFCOUNT_ADD(*user);
    return NOERROR;
}

ECode CStatusBarNotification::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    StringBuilder sb("StatusBarNotification{pkg=");
    sb += mPkg.IsNull() ? "NULL" : mPkg;
    sb += ", id=";
    sb += mId;
    sb += ", tag=";
    sb += mTag.IsNull() ? "NULL" : mTag;
    sb += ", score=";
    sb += mScore;

    if (mNotification != NULL) {
        String nstr;
        mNotification->ToString(&nstr);
        sb += ", notification=";
        sb += nstr.string();
    }
    else {
        sb += ", notification=NULL";
    }

    if (mUser != NULL) {
        String nstr;
        mUser->ToString(&nstr);
        sb += ", user=";
        sb += nstr.string();
    }
    else {
        sb += ", user=NULL";
    }

    sb += "}";

    *result = sb.ToString();
    return NOERROR;
}

}//namespace StatusBar
}//namespace Droid
}//namespace Elastos
