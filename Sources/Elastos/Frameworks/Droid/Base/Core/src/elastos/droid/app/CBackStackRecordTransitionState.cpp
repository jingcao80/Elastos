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
