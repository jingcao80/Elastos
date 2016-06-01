#include "elastos/droid/systemui/recent/TaskDescription.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Os::IUserHandle;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

CAR_INTERFACE_IMPL(TaskDescription, Object, ITaskDescription)

TaskDescription::TaskDescription(
    /* [in] */ Int32 _taskId,
    /* [in] */ Int32 _persistentTaskId,
    /* [in] */ IResolveInfo* _resolveInfo,
    /* [in] */ IIntent* _intent,
    /* [in] */ const String& _packageName,
    /* [in] */ ICharSequence* _description,
    /* [in] */ Int32 _userId)
    : mResolveInfo(_resolveInfo)
    , mTaskId(_taskId)
    , mPersistentTaskId(_persistentTaskId)
    , mIntent(_intent)
    , mPackageName(_packageName)
    , mDescription(_description)
    , mUserId(_userId)
{
}

TaskDescription::TaskDescription()
    : mResolveInfo(NULL)
    , mTaskId(-1)
    , mPersistentTaskId(-1)
    , mIntent(NULL)
    , mPackageName(NULL)
    , mDescription(NULL)
    , mUserId(IUserHandle::USER_NULL)
{
}

ECode TaskDescription::SetLoaded(
    /* [in] */ Boolean loaded)
{
    mLoaded = loaded;
    return NOERROR;
}

ECode TaskDescription::IsLoaded(
    /* [out] */ Boolean* isLoaded)
{
    VALIDATE_NOT_NULL(isLoaded)
    *isLoaded = mLoaded;
    return NOERROR;
}

ECode TaskDescription::IsNull(
    /* [out] */ Boolean* isNull)
{
    VALIDATE_NOT_NULL(isNull)
    *isNull = mResolveInfo == NULL;
    return NOERROR;
}

// mark all these as locked?
ECode TaskDescription::GetLabel(
    /* [out] */ ICharSequence** isLoaded)
{
    VALIDATE_NOT_NULL(isLoaded)
    *isLoaded = mLabel;
    REFCOUNT_ADD(*isLoaded)
    return NOERROR;
}

ECode TaskDescription::SetLabel(
    /* [in] */ ICharSequence* label)
{
    mLabel = label;
    return NOERROR;
}

ECode TaskDescription::GetIcon(
    /* [out] */ IDrawable** icon)
{
    VALIDATE_NOT_NULL(icon)
    *icon = mIcon;
    REFCOUNT_ADD(*icon)
    return NOERROR;
}

ECode TaskDescription::SetIcon(
    /* [in] */ IDrawable* icon)
{
    mIcon = icon;
    return NOERROR;
}

ECode TaskDescription::SetThumbnail(
    /* [in] */ IDrawable* thumbnail)
{
    mThumbnail = thumbnail;
    return NOERROR;
}

ECode TaskDescription::GetThumbnail(
    /* [out] */ IDrawable** thumbnail)
{
    VALIDATE_NOT_NULL(thumbnail)
    *thumbnail = mThumbnail;
    REFCOUNT_ADD(*thumbnail)
    return NOERROR;
}

ECode TaskDescription::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb(128);
    sb += "TaskDescription{0x";
    sb += StringUtils::ToHexString((Int32)this);
    sb += ", package:";
    sb += mPackageName;
    sb += ", label:";
    sb += TO_CSTR(mLabel);
    sb += ", icon:";
    sb += TO_CSTR(mIcon);
    *str = sb.ToString();
    return NOERROR;
}

} // namespace Recent
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos