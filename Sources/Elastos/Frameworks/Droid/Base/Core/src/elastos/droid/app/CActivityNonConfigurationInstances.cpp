
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/app/CActivityNonConfigurationInstances.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CActivityNonConfigurationInstances, Object, IActivityNonConfigurationInstances)

CAR_OBJECT_IMPL(CActivityNonConfigurationInstances)

ECode CActivityNonConfigurationInstances::constructor()
{
    return NOERROR;
}

ECode CActivityNonConfigurationInstances::constructor(
    /* [in] */ IInterface* activity,
    /* [in] */ IHashMap* children,
    /* [in] */ IArrayList* fragments,
    /* [in] */ IArrayMap* loaders,
    /* [in] */ IVoiceInteractor* vi)
{
    mActivity = activity;
    mChildren = children;
    mFragments = fragments;
    mVoiceInteractor = vi;
    return NOERROR;
}

ECode CActivityNonConfigurationInstances::SetActivity(
    /* [in] */ IInterface* activity)
{
    mActivity = activity;
    return NOERROR;
}

ECode CActivityNonConfigurationInstances::GetActivity(
    /* [out] */ IInterface** activity)
{
    VALIDATE_NOT_NULL(activity);
    *activity = mActivity;
    REFCOUNT_ADD(*activity);
    return NOERROR;
}

ECode CActivityNonConfigurationInstances::SetChildren(
    /* [in] */ IHashMap* children)
{
    mChildren = children;
    return NOERROR;
}

ECode CActivityNonConfigurationInstances::GetChildren(
    /* [out] */ IHashMap** children)
{
    VALIDATE_NOT_NULL(children);
    *children = mChildren;
    REFCOUNT_ADD(*children);
    return NOERROR;
}

ECode CActivityNonConfigurationInstances::SetFragments(
    /* [in] */ IArrayList* fragments)
{
    mFragments = fragments;
    return NOERROR;
}

ECode CActivityNonConfigurationInstances::GetFragments(
    /* [out] */ IArrayList** fragments)
{
    VALIDATE_NOT_NULL(fragments);
    *fragments = mFragments;
    REFCOUNT_ADD(*fragments);
    return NOERROR;
}

ECode CActivityNonConfigurationInstances::SetLoaders(
    /* [in] */ IArrayMap* loaders)
{
    mLoaders = loaders;
    return NOERROR;
}

ECode CActivityNonConfigurationInstances::GetLoaders(
    /* [out] */ IArrayMap** loaders)
{
    VALIDATE_NOT_NULL(loaders);
    *loaders = mLoaders;
    REFCOUNT_ADD(*loaders);
    return NOERROR;
}

ECode CActivityNonConfigurationInstances::SetVoiceInteractor(
    /* [in] */ IVoiceInteractor* vi)
{
    mVoiceInteractor = vi;
    return NOERROR;
}

ECode CActivityNonConfigurationInstances::GetVoiceInteractor(
    /* [out] */ IVoiceInteractor** vi)
{
    VALIDATE_NOT_NULL(vi)
    *vi = mVoiceInteractor;
    REFCOUNT_ADD(*vi)
    return NOERROR;
}


} // namespace App
} // namespace Droid
} // namespace Elastos

