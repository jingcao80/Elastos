
#include "elastos/droid/app/CActivityManagerRecentTaskInfo.h"
// #include "elastos/droid/app/CActivityManagerTaskDescription.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/content/CIntent.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntent;

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
    dest->WriteInterfacePtr(mOrigActivity);
    dest->WriteInterfacePtr(mBaseIntent);
    dest->WriteInterfacePtr(mDescription);
    // TODO
//    TextUtils.writeToParcel(description, dest,
//            Parcelable.PARCELABLE_WRITE_RETURN_VALUE);

    if (mTaskDescription != NULL) {
        dest->WriteInt32(1);
        IParcelable::Probe(mTaskDescription)->WriteToParcel(dest);
    } else {
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
    AutoPtr<IInterface> obj;
    source->ReadInt32(&mId);
    source->ReadInt32(&mPersistentId);
    source->ReadInterfacePtr((Handle32*)&obj);
    mOrigActivity = IComponentName::Probe(obj);
    obj = NULL;
    source->ReadInterfacePtr((Handle32*)&obj);
    mBaseIntent = IIntent::Probe(obj);
    obj = NULL;
    source->ReadInterfacePtr((Handle32*)&obj);
    mDescription = ICharSequence::Probe(obj);
    // TODO
//    description = TextUtils.CHAR_SEQUENCE_CREATOR.createFromParcel(source);
    Int32 ival;
    source->ReadInt32(&ival);
    if (ival > 0) {
        // CActivityManagerTaskDescription((IActivityManagerTaskDescription**)&mTaskDescription);
        IParcelable::Probe(mTaskDescription)->ReadFromParcel(source);
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

} // namespace App
} // namespace Droid
} // namespace Elastos
