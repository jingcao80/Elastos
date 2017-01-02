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

#include "Elastos.Droid.Utility.h"
#include "elastos/droid/transition/TransitionValuesMaps.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/utility/CInt64SparseArray.h"
#include "elastos/droid/utility/CSparseArray.h"

using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::CInt64SparseArray;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// TransitionValuesMaps::
//===============================================================
CAR_INTERFACE_IMPL(TransitionValuesMaps, Object, ITransitionValuesMaps)

TransitionValuesMaps::TransitionValuesMaps()
{
    CArrayMap::New((IArrayMap**)&mViewValues);

    CSparseArray::New((ISparseArray**)&mIdValues);

    CInt64SparseArray::New((IInt64SparseArray**)&mItemIdValues);

    CArrayMap::New((IArrayMap**)&mNameValues);
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
