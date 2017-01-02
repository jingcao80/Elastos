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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/CPointerCoordsHelper.h"
#include "elastos/droid/view/MotionEvent.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(CPointerCoordsHelper, Singleton, IPointerCoordsHelper);

CAR_SINGLETON_IMPL(CPointerCoordsHelper);

ECode CPointerCoordsHelper::CreateArray(
    /* [in] */ Int32 size,
    /* [out, callee] */ ArrayOf<IPointerCoords*>** array)
{
    VALIDATE_NOT_NULL(array);
    AutoPtr< ArrayOf<IPointerCoords*> > _array = MotionEvent::PointerCoords::CreateArray(size);
    *array = _array;
    REFCOUNT_ADD(*array);
    return NOERROR;
}

} // View
} // Droid
} // Elastos
