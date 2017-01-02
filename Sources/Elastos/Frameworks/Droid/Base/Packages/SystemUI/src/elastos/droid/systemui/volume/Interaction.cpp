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

#include "elastos/droid/systemui/volume/Interaction.h"

using Elastos::Droid::View::EIID_IViewOnGenericMotionListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Volume {

//-------------------------------------------------------------------------
// Interaction::MyOnTouchListener
//--------------------------------------------------------------------------

CAR_INTERFACE_IMPL(Interaction::MyOnTouchListener, Object, IViewOnTouchListener)

Interaction::MyOnTouchListener::MyOnTouchListener(
    /* [in] */ IInteractionCallback* callback)
    : mCallback(callback)
{}

ECode Interaction::MyOnTouchListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mCallback) mCallback->OnInteraction();
    *result = FALSE;
    return NOERROR;
}

//-------------------------------------------------------------------------
// Interaction::MyOnGenericMotionListener
//--------------------------------------------------------------------------

CAR_INTERFACE_IMPL(Interaction::MyOnGenericMotionListener, Object, IViewOnGenericMotionListener)

Interaction::MyOnGenericMotionListener::MyOnGenericMotionListener(
    /* [in] */ IInteractionCallback* callback)
    : mCallback(callback)
{}

ECode Interaction::MyOnGenericMotionListener::OnGenericMotion(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mCallback) mCallback->OnInteraction();
    *result = FALSE;
    return NOERROR;
}

//-------------------------------------------------------------------------
// Interaction
//--------------------------------------------------------------------------

ECode Interaction::Register(
    /* [in] */ IView* v,
    /* [in] */ IInteractionCallback* callback)
{
    AutoPtr<MyOnTouchListener> otl = new MyOnTouchListener(callback);
    AutoPtr<MyOnGenericMotionListener> ogml = new MyOnGenericMotionListener(callback);
    v->SetOnTouchListener(otl);
    v->SetOnGenericMotionListener(ogml);
    return NOERROR;
}

} // namespace Volume
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
