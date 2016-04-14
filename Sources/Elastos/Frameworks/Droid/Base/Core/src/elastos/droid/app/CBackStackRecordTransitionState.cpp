
#include "elastos/droid/app/CBackStackRecordTransitionState.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CBackStackRecordTransitionState, Object, IBackStackRecordTransitionState)

CAR_OBJECT_IMPL(CBackStackRecordTransitionState)

CBackStackRecordTransitionState::CBackStackRecordTransitionState()
{}

ECode CBackStackRecordTransitionState::GetNameOverrides(
    /* [out] */ IArrayMap** map)
{
    VALIDATE_NOT_NULL(map)
    *map = mNameOverrides;
    REFCOUNT_ADD(*map)
    return NOERROR;
}

ECode CBackStackRecordTransitionState::SetNameOverrides(
    /* [in] */ IArrayMap* map)
{
    mNameOverrides = map;
    return NOERROR;
}

ECode CBackStackRecordTransitionState::GetEnteringEpicenterView(
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEnteringEpicenterView;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CBackStackRecordTransitionState::SetEnteringEpicenterView(
    /* [in] */ IView* result)
{
    mEnteringEpicenterView = result;
    return NOERROR;
}

ECode CBackStackRecordTransitionState::GetNonExistentView(
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mNonExistentView;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CBackStackRecordTransitionState::SetNonExistentView(
    /* [in] */ IView* result)
{
    mNonExistentView = result;
    return NOERROR;
}


}// namespace App
}// namespace Droid
}// namespace Elastos
