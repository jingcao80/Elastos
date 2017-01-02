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

#include "elastos/droid/app/CActivityManagerRunningTaskInfo.h"
#include "elastos/droid/content/CComponentNameHelper.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Graphics::CBitmap;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CActivityManagerRunningTaskInfo, Object, IActivityManagerRunningTaskInfo, IParcelable)

CAR_OBJECT_IMPL(CActivityManagerRunningTaskInfo)

CActivityManagerRunningTaskInfo::CActivityManagerRunningTaskInfo()
    : mId(0)
    , mNumActivities(0)
    , mNumRunning(0)
    , mLastActiveTime(0)
{
}

ECode CActivityManagerRunningTaskInfo::constructor()
{
    return NOERROR;
}

ECode CActivityManagerRunningTaskInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);
    FAIL_RETURN(dest->WriteInt32(mId));

    AutoPtr<IComponentNameHelper> helper;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
    helper->WriteToParcel(mBaseActivity, dest);
    helper->WriteToParcel(mTopActivity, dest);

    if (mThumbnail != NULL) {
        dest->WriteInt32(1);
        IParcelable* parcel = IParcelable::Probe(mThumbnail);
        parcel->WriteToParcel(dest);
    } else {
        dest->WriteInt32(0);
    }

    TextUtils::WriteToParcel(mDescription, dest);

    dest->WriteInt32(mNumActivities);
    dest->WriteInt32(mNumRunning);

    return NOERROR;
}

ECode CActivityManagerRunningTaskInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    source->ReadInt32(&mId);

    AutoPtr<IComponentNameHelper> helper;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
    helper->ReadFromParcel(source, (IComponentName**)&mBaseActivity);
    helper->ReadFromParcel(source, (IComponentName**)&mTopActivity);

    Int32 value;
    source->ReadInt32(&value);
    if (value != 0) {
        AutoPtr<IBitmap> bitmap;
        CBitmap::New((IBitmap**)&bitmap);
        IParcelable* parcel = IParcelable::Probe(bitmap);
        parcel->ReadFromParcel(source);
        mThumbnail = bitmap;
    }
    else {
        mThumbnail = NULL;
    }

    TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(
        source, (ICharSequence**)&mDescription);

    source->ReadInt32(&mNumActivities);
    source->ReadInt32(&mNumRunning);

    return NOERROR;
}

/**
 * A unique identifier for this task.
 */
ECode CActivityManagerRunningTaskInfo::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mId;
    return NOERROR;
}

ECode CActivityManagerRunningTaskInfo::SetId(
    /* [in] */ Int32 id)
{
    mId = id;
    return NOERROR;
}

ECode CActivityManagerRunningTaskInfo::GetLastActiveTime(
    /* [out] */ Int64* lastActiveTime)
{
    VALIDATE_NOT_NULL(lastActiveTime);
    *lastActiveTime = mLastActiveTime;
    return NOERROR;
}

ECode CActivityManagerRunningTaskInfo::SetLastActiveTime(
    /* [in] */ Int64 lastActiveTime)
{
    mLastActiveTime = lastActiveTime;
    return NOERROR;
}

/**
 * The component launched as the first activity in the task.  This can
 * be considered the "application" of this task.
 */
ECode CActivityManagerRunningTaskInfo::GetBaseActivity(
    /* [out] */ IComponentName** baseActivity)
{
    VALIDATE_NOT_NULL(baseActivity);
    *baseActivity = mBaseActivity;
    REFCOUNT_ADD(*baseActivity)
    return NOERROR;
}

ECode CActivityManagerRunningTaskInfo::SetBaseActivity(
    /* [in] */ IComponentName* baseActivity)
{
    mBaseActivity = baseActivity;
    return NOERROR;
}

/**
 * The activity component at the top of the history stack of the task.
 * This is what the user is currently doing.
 */
ECode CActivityManagerRunningTaskInfo::GetTopActivity(
    /* [out] */ IComponentName** topActivity)
{
    VALIDATE_NOT_NULL(topActivity);
    *topActivity = mTopActivity;
    REFCOUNT_ADD(*topActivity)
    return NOERROR;
}

ECode CActivityManagerRunningTaskInfo::SetTopActivity(
    /* [in] */ IComponentName* topActivity)
{
    mTopActivity = topActivity;
    return NOERROR;
}

/**
 * Thumbnail representation of the task's current state.  Currently
 * always null.
 */
ECode CActivityManagerRunningTaskInfo::GetThumbnail(
    /* [out] */ IBitmap** thumbnail)
{
    VALIDATE_NOT_NULL(thumbnail);
    *thumbnail = mThumbnail;
    REFCOUNT_ADD(*thumbnail)
    return NOERROR;
}

ECode CActivityManagerRunningTaskInfo::SetThumbnail(
    /* [in] */ IBitmap* thumbnail)
{
    mThumbnail = thumbnail;
    return NOERROR;
}

/**
 * Description of the task's current state.
 */
ECode CActivityManagerRunningTaskInfo::GetDescription(
    /* [out] */ ICharSequence** description)
{
    VALIDATE_NOT_NULL(description);
    *description = mDescription;
    REFCOUNT_ADD(*description)
    return NOERROR;
}

ECode CActivityManagerRunningTaskInfo::SetDescription(
    /* [in] */ ICharSequence* description)
{
    mDescription = description;
    return NOERROR;
}

/**
 * Number of activities in this task.
 */
ECode CActivityManagerRunningTaskInfo::GetNumActivities(
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);
    *num = mNumActivities;
    return NOERROR;
}

ECode CActivityManagerRunningTaskInfo::SetNumActivities(
    /* [in] */ Int32 num)
{
    mNumActivities = num;
    return NOERROR;
}

/**
 * Number of activities that are currently running (not stopped
 * and persisted) in this task.
 */
ECode CActivityManagerRunningTaskInfo::GetNumRunning(
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);
    *num = mNumRunning;
    return NOERROR;
}

ECode CActivityManagerRunningTaskInfo::SetNumRunning(
    /* [in] */ Int32 num)
{
    mNumRunning = num;
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos

