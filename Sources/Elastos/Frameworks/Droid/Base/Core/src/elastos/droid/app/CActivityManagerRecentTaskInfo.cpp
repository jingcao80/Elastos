
#include "elastos/droid/app/CActivityManagerRecentTaskInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CComponentName;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CActivityManagerRecentTaskInfo, Object, IActivityManagerRecentTaskInfo, IParcelable)

CAR_OBJECT_IMPL(CActivityManagerRecentTaskInfo)

CActivityManagerRecentTaskInfo::CActivityManagerRecentTaskInfo()
    : mId(0)
    , mPersistentId(0)
    , mStackId(0)
    , mUserId(0)
    , mFirstActiveTime(0)
    , mLastActiveTime(0)
    , mAffiliatedTaskId(0)
    , mAffiliatedTaskColor(0)
{
}

CActivityManagerRecentTaskInfo::~CActivityManagerRecentTaskInfo()
{
}

ECode CActivityManagerRecentTaskInfo::constructor()
{
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mId);
    dest->WriteInt32(mPersistentId);

    if (mBaseIntent != NULL) {
        dest->WriteInt32(1);
        dest->WriteInterfacePtr(mBaseIntent);
    }
    else {
        dest->WriteInt32(0);
    }

    dest->WriteInterfacePtr(mOrigActivity);
    TextUtils::WriteToParcel(mDescription, dest);

    if (mTaskDescription != NULL) {
        dest->WriteInt32(1);
        dest->WriteInterfacePtr(mTaskDescription);
    }
    else {
        dest->WriteInt32(0);
    }

    dest->WriteInt32(mStackId);
    dest->WriteInt32(mUserId);
    dest->WriteInt64(mFirstActiveTime);
    dest->WriteInt64(mLastActiveTime);
    dest->WriteInt32(mAffiliatedTaskId);
    dest->WriteInt32(mAffiliatedTaskColor);
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mId);
    source->ReadInt32(&mPersistentId);

    AutoPtr<IInterface> obj;
    Int32 ival;
    source->ReadInt32(&ival);
    if (ival > 0) {
        source->ReadInterfacePtr((Handle32*)&obj);
        mBaseIntent = IIntent::Probe(obj);
    }
    else {
        mBaseIntent = NULL;
    }

    obj = NULL;
    source->ReadInterfacePtr((Handle32*)&obj);
    mOrigActivity = IComponentName::Probe(obj);

    TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(
        source, (ICharSequence**)&mDescription);

    source->ReadInt32(&ival);
    if (ival > 0) {
        obj = NULL;
        source->ReadInterfacePtr((Handle32*)&obj);
        mTaskDescription = IActivityManagerTaskDescription::Probe(obj);
    }
    else {
        mTaskDescription = NULL;
    }

    source->ReadInt32(&mStackId);
    source->ReadInt32(&mUserId);
    source->ReadInt64(&mFirstActiveTime);
    source->ReadInt64(&mLastActiveTime);
    source->ReadInt32(&mAffiliatedTaskId);
    source->ReadInt32(&mAffiliatedTaskColor);
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mId;
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::SetId(
    /* [in] */ Int32 id)
{
    mId = id;
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::GetPersistentId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mPersistentId;
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::SetPersistentId(
    /* [in] */ Int32 id)
{
    mPersistentId = id;
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::GetBaseIntent(
    /* [out] */ IIntent** baseIntent)
{
    VALIDATE_NOT_NULL(baseIntent);
    *baseIntent = mBaseIntent;
    REFCOUNT_ADD(*baseIntent);
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::SetBaseIntent(
    /* [in] */ IIntent* baseIntent)
{
    mBaseIntent = baseIntent;
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::GetOrigActivity(
    /* [out] */ IComponentName** origActivity)
{
    VALIDATE_NOT_NULL(origActivity);
    *origActivity = mOrigActivity;
    REFCOUNT_ADD(*origActivity);
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::SetOrigActivity(
    /* [in] */ IComponentName* origActivity)
{
    mOrigActivity = origActivity;
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::GetDescription(
    /* [out] */ ICharSequence** description)
{
    VALIDATE_NOT_NULL(description);
    *description = mDescription;
    REFCOUNT_ADD(*description);
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::SetDescription(
    /* [in] */ ICharSequence* description)
{
    mDescription = description;
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::GetStackId(
    /* [out] */ Int32* stackId)
{
    VALIDATE_NOT_NULL(stackId);
    *stackId = mStackId;
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::SetStackId(
    /* [in] */ Int32 stackId)
{
    mStackId = stackId;
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::GetUserId(
    /* [out] */ Int32* userId)
{
    VALIDATE_NOT_NULL(userId);
    *userId = mUserId;
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::SetUserId(
    /* [in] */ Int32 userId)
{
    mUserId = userId;
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::GetFirstActiveTime(
    /* [out] */ Int64* firstActiveTime)
{
    VALIDATE_NOT_NULL(firstActiveTime);
    *firstActiveTime = mFirstActiveTime;
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::SetFirstActiveTime(
    /* [in] */ Int64 firstActiveTime)
{
    mFirstActiveTime = firstActiveTime;
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::GetLastActiveTime(
    /* [out] */ Int64* lastActiveTime)
{
    VALIDATE_NOT_NULL(lastActiveTime);
    *lastActiveTime = mLastActiveTime;
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::SetLastActiveTime(
    /* [in] */ Int64 lastActiveTime)
{
    mLastActiveTime = lastActiveTime;
    return NOERROR;
}


ECode CActivityManagerRecentTaskInfo::SetTaskDescription(
    /* [in] */ IActivityManagerTaskDescription* description)
{
    mTaskDescription = description;
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::GetTaskDescription(
    /* [out] */ IActivityManagerTaskDescription** description)
{
    VALIDATE_NOT_NULL(description)
    *description = mTaskDescription;
    REFCOUNT_ADD(*description);
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::GetAffiliatedTaskId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mAffiliatedTaskId;
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::SetAffiliatedTaskId(
    /* [in] */ Int32 id)
{
    mAffiliatedTaskId = id;
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::GetAffiliatedTaskColor(
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color)
    *color = mAffiliatedTaskColor;
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::SetAffiliatedTaskColor(
    /* [in] */ Int32 color)
{
    mAffiliatedTaskColor = color;
    return NOERROR;
}

ECode CActivityManagerRecentTaskInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    StringBuilder sb("CActivityManagerRecentTaskInfo{0x");
    sb += StringUtils::ToHexString((Int32)this);
    if (mOrigActivity != NULL) {
        sb += ", ";
        sb += TO_CSTR(mOrigActivity);
    }
    else if (mTaskDescription != NULL) {
        sb += ", ";
        sb += TO_CSTR(mTaskDescription);
    }
    else if (mDescription != NULL) {
        sb += ", ";
        sb += TO_CSTR(mDescription);
    }
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
