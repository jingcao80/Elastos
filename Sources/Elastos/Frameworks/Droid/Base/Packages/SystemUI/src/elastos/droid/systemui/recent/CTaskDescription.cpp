#include "elastos/droid/systemui/recent/CTaskDescription.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

CTaskDescription::CTaskDescription()
    : mTaskId(0)
    , mPersistentTaskId(0)
    , mLoaded(FALSE)
{}

ECode CTaskDescription::constructor()
{
    mResolveInfo = NULL;
    mIntent = NULL;
    mTaskId = -1;
    mPersistentTaskId = -1;

    mDescription = NULL;
    mPackageName = NULL;
    return NOERROR;
}

ECode CTaskDescription::constructor(
    /* [in] */ Int32 taskId,
    /* [in] */ Int32 persistentTaskId,
    /* [in] */ IResolveInfo* resolveInfo,
    /* [in] */ IIntent* intent,
    /* [in] */ const String& packageName,
    /* [in] */ ICharSequence* description)
{
    mResolveInfo = resolveInfo;
    mIntent = intent;
    mTaskId = taskId;
    mPersistentTaskId = persistentTaskId;

    mDescription = description;
    mPackageName = packageName;
    return NOERROR;
}

ECode CTaskDescription::SetLoaded(
    /* [in] */ Boolean loaded)
{
    mLoaded = loaded;
    return NOERROR;
}

ECode CTaskDescription::IsLoaded(
    /* [out] */ Boolean* loaded)
{
    VALIDATE_NOT_NULL(loaded);
    *loaded = mLoaded;
    return NOERROR;
}

ECode CTaskDescription::IsNull(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mResolveInfo == NULL;
    return NOERROR;
}

ECode CTaskDescription::GetLabel(
    /* [out] */ ICharSequence** label)
{
    VALIDATE_NOT_NULL(label);
    *label = mLabel;
    REFCOUNT_ADD(*label);
    return NOERROR;
}

ECode CTaskDescription::SetLabel(
    /* [in] */ ICharSequence* label)
{
    mLabel = label;
    return NOERROR;
}

ECode CTaskDescription::GetIcon(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    *drawable = mIcon;
    REFCOUNT_ADD(*drawable);
    return NOERROR;
}

ECode CTaskDescription::SetIcon(
    /* [in] */ IDrawable* drawable)
{
    mIcon = drawable;
    return NOERROR;
}

ECode CTaskDescription::SetThumbnail(
    /* [in] */ IBitmap* bitmap)
{
    mThumbnail = bitmap;
    return NOERROR;
}

ECode CTaskDescription::GetThumbnail(
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    *bitmap = mThumbnail;
    REFCOUNT_ADD(*bitmap);
    return NOERROR;
}

ECode CTaskDescription::SetResolveInfo(
    /* [in] */ IResolveInfo* resolveInfo)
{
    mResolveInfo = resolveInfo;
    return NOERROR;
}

ECode CTaskDescription::GetResolveInfo(
    /* [out] */ IResolveInfo** resolveInfo)
{
    VALIDATE_NOT_NULL(resolveInfo);
    *resolveInfo = mResolveInfo;
    REFCOUNT_ADD(*resolveInfo);
    return NOERROR;
}

ECode CTaskDescription::SetTaskId(
    /* [in] */ Int32 taskId)
{
    mTaskId = taskId;
    return NOERROR;
}

ECode CTaskDescription::GetTaskId(
    /* [out] */ Int32* taskId)
{
    VALIDATE_NOT_NULL(taskId);
    *taskId = mTaskId;
    return NOERROR;
}

ECode CTaskDescription::SetPersistentTaskId(
    /* [in] */ Int32 persistentTaskId)
{
    mPersistentTaskId = persistentTaskId;
    return NOERROR;
}

ECode CTaskDescription::GetPersistentTaskId(
    /* [out] */ Int32* persistentTaskId)
{
    VALIDATE_NOT_NULL(persistentTaskId);
    *persistentTaskId = mPersistentTaskId;
    return NOERROR;
}

ECode CTaskDescription::SetIntent(
    /* [in] */ IIntent* intent)
{
    mIntent = intent;
    return NOERROR;
}

ECode CTaskDescription::GetIntent(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent);
    *intent = mIntent;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode CTaskDescription::SetPackageName(
    /* [in] */ const String& packageName)
{
    mPackageName = packageName;
    return NOERROR;
}

ECode CTaskDescription::GetPackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName);
    *packageName = mPackageName;
    return NOERROR;
}

ECode CTaskDescription::SetDescription(
    /* [in] */ ICharSequence* description)
{
    mDescription = description;
    return NOERROR;
}

ECode CTaskDescription::GetDescription(
    /* [out] */ ICharSequence** description)
{
    VALIDATE_NOT_NULL(description);
    *description = mDescription;
    REFCOUNT_ADD(*description);
    return NOERROR;
}

ECode CTaskDescription::Lock()
{
    _m_syncLock.Lock();
    return NOERROR;
}

ECode CTaskDescription::Unlock()
{
    _m_syncLock.Unlock();
    return NOERROR;
}

}// namespace Recent
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
