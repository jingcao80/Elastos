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

#include "elastos/droid/view/CDragEventHelper.h"
#include "elastos/droid/view/CDragEvent.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(CDragEventHelper, Singleton, IDragEventHelper)
CAR_SINGLETON_IMPL(CDragEventHelper)

ECode CDragEventHelper::Obtain(
    /* [out] */ IDragEvent** event)
{
    VALIDATE_NOT_NULL(event)

    AutoPtr<IDragEvent> tmp = CDragEvent::Obtain();
    *event = tmp;
    REFCOUNT_ADD(*event)

    return NOERROR;
}

ECode CDragEventHelper::Obtain(
    /* [in] */ Int32 action,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IInterface* localState,
    /* [in] */ IClipDescription* description,
    /* [in] */ IClipData* data,
    /* [in] */ Boolean result,
    /* [out] */ IDragEvent** event)
{
    VALIDATE_NOT_NULL(event)

    AutoPtr<IDragEvent> tmp = CDragEvent::Obtain(
        action, x, y, localState, description,
        data, result);
    *event = tmp;
    REFCOUNT_ADD(*event)

    return NOERROR;
}

ECode CDragEventHelper::Obtain(
    /* [in] */ IDragEvent* source,
    /* [out] */ IDragEvent** event)
{
    VALIDATE_NOT_NULL(event)

    AutoPtr<IDragEvent> tmp = CDragEvent::Obtain(source);
    *event = tmp;
    REFCOUNT_ADD(*event)

    return NOERROR;
}

} //view
} //Droid
} //Elastos

