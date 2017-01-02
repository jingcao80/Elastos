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

#ifndef __ELASTOS_DROID_TRANSITION_TRANSITIONVALUESMAPS_H__
#define __ELASTOS_DROID_TRANSITION_TRANSITIONVALUESMAPS_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Transition.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::IInt64SparseArray;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::ISparseArray;

namespace Elastos {
namespace Droid {
namespace Transition {

class TransitionValuesMaps
    : public Object
    , public ITransitionValuesMaps
{
public:
    CAR_INTERFACE_DECL()

    TransitionValuesMaps();

public:
    AutoPtr<IArrayMap> mViewValues;
    AutoPtr<ISparseArray> mIdValues;
    AutoPtr<IInt64SparseArray> mItemIdValues;
    AutoPtr<IArrayMap> mNameValues;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_TRANSITIONVALUESMAPS_H__
