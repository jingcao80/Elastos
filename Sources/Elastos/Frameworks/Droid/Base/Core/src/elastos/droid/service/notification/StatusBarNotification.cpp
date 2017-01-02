//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/app/CNotification.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/service/notification/CStatusBarNotification.h"
#include "elastos/droid/service/notification/StatusBarNotification.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::App::CNotification;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Service::Notification::CStatusBarNotification;
using Elastos::Core::CSystem;
using Elastos::Core::EIID_ICloneable;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Notification {

CAR_INTERFACE_IMPL_3(StatusBarNotification, Object, IStatusBarNotification, IParcelable, ICloneable)

StatusBarNotification::StatusBarNotification()
    : mId(0)
    , mUid(0)
    , mInitialPid(0)
    , mPostTime(0L)
    , mScore(0)
{
}

ECode StatusBarNotification::constructor()
{
    return NOERROR;
}

ECode StatusBarNotification::constructor(
    /* [in] */ const String& pkg,
    /* [in] */ const String& opPkg,
    /* [in] */ Int32 id,
    /* [in] */ const String& tag,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 initialPid,
    /* [in] */ Int32 score,
    /* [in] */ INotification* notification,
    /* [in] */ IUserHandle* user)
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 milis;
    system->GetCurrentTimeMillis(&milis);
    return constructor(pkg, opPkg, id, tag, uid, initialPid, score, notification, user, milis);
}

ECode StatusBarNotification::constructor(
    /* [in] */ const String& pkg,
    /* [in] */ const String& opPkg,
    /* [in] */ Int32 id,
    /* [in] */ const String& tag,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 initialPid,
    /* [in] */ Int32 score,
    /* [in] */ INotification* notification,
    /* [in] */ IUserHandle* user,
    /* [in] */ Int64 postTime)
{
    if (pkg.IsNull()) return E_NULL_POINTER_EXCEPTION;
    if (notification == NULL) return E_NULL_POINTER_EXCEPTION;

    mPkg = pkg;
    mOpPkg = opPkg;
    mId = id;
    mTag = tag;
    mUid = uid;
    mInitialPid = initialPid;
    mScore = score;
    mNotification = notification;
    mUser = user;
    mPostTime = postTime;
    Key(&mKey);
    GroupKey(&mGroupKey);
    return NOERROR;
}

ECode StatusBarNotification::constructor(
    /* [in] */ IParcelable* in)
{
    return NOERROR;
}

ECode StatusBarNotification::Key(
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key)
    Int32 identifier;
    mUser->GetIdentifier(&identifier);
    StringBuilder sb(identifier);
    sb += "|";
    sb += mPkg;
    sb += "|";
    sb += mId;
    sb += "|";
    sb += mTag;
    sb += "|";
    sb += mUid;
    *key = sb.ToString();
    return NOERROR;
}

ECode StatusBarNotification::GroupKey(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<INotification> notification;
    GetNotification((INotification**)&notification);
    String group;
    notification->GetGroup(&group);
    String sortKey;
    notification->GetSortKey(&sortKey);
    if (group.IsNull() && sortKey.IsNull()) {
        // a group of one
        *result = mKey;
        return NOERROR;
    }
    Int32 identifier;
    mUser->GetIdentifier(&identifier);
    StringBuilder sb(identifier);
    sb += "|";
    sb += mPkg;
    sb += "|";
    if (group.IsNull()) {
        Int32 priority;
        notification->GetPriority(&priority);
        sb += "p:";
        sb += priority;
    }
    else {
        sb += "g:";
        sb += group;
    }
    *result = sb.ToString();
    return NOERROR;
}

ECode StatusBarNotification::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadString(&mPkg);
    in->ReadString(&mOpPkg);
    in->ReadInt32(&mId);
    Int32 value;
    in->ReadInt32(&value);
    if (value != 0) {
        in->ReadString(&mTag);
    }
    else {
        mTag = NULL;
    }
    in->ReadInt32(&mUid);
    in->ReadInt32(&mInitialPid);
    in->ReadInt32(&mScore);

    CNotification::New((INotification**)&mNotification);
    IParcelable::Probe(mNotification)->ReadFromParcel(in);
    UserHandle::ReadFromParcel(in, (IUserHandle**)&mUser);
    in->ReadInt64(&mPostTime);
    Key(&mKey);
    GroupKey(&mGroupKey);
    return NOERROR;
}

ECode StatusBarNotification::WriteToParcel(
    /* [in] */ IParcel* out)
{
    out->WriteString(mPkg);
    out->WriteString(mOpPkg);
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
    IParcelable::Probe(mNotification)->WriteToParcel(out);
    IParcelable::Probe(mUser)->WriteToParcel(out);

    out->WriteInt64(mPostTime);
    return NOERROR;
}

ECode StatusBarNotification::CloneLight(
    /* [out] */ IStatusBarNotification** notification)
{
    VALIDATE_NOT_NULL(notification)
    const AutoPtr<INotification> no;
    CNotification::New((INotification**)&no);
    mNotification->CloneInto(no, FALSE); // light copy
    CStatusBarNotification::New(mPkg, mOpPkg, mId, mTag, mUid, mInitialPid,
        mScore, no, mUser, mPostTime, notification);
    return NOERROR;
}

ECode StatusBarNotification::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    AutoPtr<IInterface> o;
    ICloneable::Probe(mNotification)->Clone((IInterface**)&o);
    AutoPtr<IStatusBarNotification> notification;
    CStatusBarNotification::New(mPkg, mOpPkg, mId, mTag, mUid, mInitialPid,
        mScore, INotification::Probe(o), mUser.Get(), mPostTime,
        (IStatusBarNotification**)&notification);
    *obj = IInterface::Probe(notification);
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode StatusBarNotification::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder sb("StatusBarNotification(pkg=");
    sb += mPkg;
    sb += " user=";
    sb += mUser;
    sb += " id=";
    sb += mId;
    sb += " tag=";
    sb += mTag;
    sb += " score=";
    sb += mScore;
    sb += " key=";
    sb += mKey;
    sb += ": ";
    sb += mNotification;
    *result = sb.ToString();
    return NOERROR;
}

ECode StatusBarNotification::IsOngoing(
    /* [out] */ Boolean* isongoing)
{
    VALIDATE_NOT_NULL(isongoing)
    Int32 flags;
    mNotification->GetFlags(&flags);
    *isongoing = (flags & INotification::FLAG_ONGOING_EVENT) != 0;
    return NOERROR;
}

ECode StatusBarNotification::IsClearable(
    /* [out] */ Boolean* isClearable)
{
    VALIDATE_NOT_NULL(isClearable)
    Int32 flags;
    mNotification->GetFlags(&flags);
    *isClearable = ((flags & INotification::FLAG_ONGOING_EVENT) == 0
        && ((flags & INotification::FLAG_NO_CLEAR) == 0));
    return NOERROR;
}

ECode StatusBarNotification::GetUserId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    mUser->GetIdentifier(id);
    return NOERROR;
}

ECode StatusBarNotification::GetPackageName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mPkg;
    return NOERROR;
}

ECode StatusBarNotification::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mId;
    return NOERROR;
}

ECode StatusBarNotification::GetTag(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = mTag;
    return NOERROR;
}

ECode StatusBarNotification::GetUid(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mUid;
    return NOERROR;
}

ECode StatusBarNotification::GetOpPkg(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = mOpPkg;
    return NOERROR;
}

ECode StatusBarNotification::GetInitialPid(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mInitialPid;
    return NOERROR;
}

ECode StatusBarNotification::GetNotification(
    /* [out] */ INotification** no)
{
    VALIDATE_NOT_NULL(no)
    *no = mNotification;
    REFCOUNT_ADD(*no)
    return NOERROR;
}

ECode StatusBarNotification::GetUser(
    /* [out] */ IUserHandle** us)
{
    VALIDATE_NOT_NULL(us)
    *us = mUser;
    REFCOUNT_ADD(*us)
    return NOERROR;
}

ECode StatusBarNotification::GetPostTime(
    /* [out] */ Int64* pt)
{
    VALIDATE_NOT_NULL(pt)
    *pt = mPostTime;
    return NOERROR;
}

ECode StatusBarNotification::GetScore(
    /* [out] */ Int32* isongoing)
{
    VALIDATE_NOT_NULL(isongoing)
    *isongoing = mScore;
    return NOERROR;
}

ECode StatusBarNotification::GetKey(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = mKey;
    return NOERROR;
}

ECode StatusBarNotification::GetGroupKey(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = mGroupKey;
    return NOERROR;
}

} // namespace Notification
} // namespace Service
} // namepsace Droid
} // namespace Elastos
