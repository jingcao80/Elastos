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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/CDragShadowBuilder.h"

#include <elastos/utility/logging/Slogger.h>

namespace Elastos {
namespace Droid {
namespace View {

CAR_OBJECT_IMPL(CDragShadowBuilder)

CAR_INTERFACE_IMPL(CDragShadowBuilder, Object, IDragShadowBuilder)

CDragShadowBuilder::CDragShadowBuilder()
{}

ECode CDragShadowBuilder::constructor(
    /* [in] */ IView* view)
{
    AutoPtr<IWeakReferenceSource> obj = IWeakReferenceSource::Probe(view);
    obj->GetWeakReference((IWeakReference**)&mView);
    return NOERROR;
}

ECode CDragShadowBuilder::constructor()
{
    mView = NULL;
    return NOERROR;
}

ECode CDragShadowBuilder::GetView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)

    if (mView == NULL) {
        *view = NULL;
        return NOERROR;
    }
    return mView->Resolve(EIID_IView, (IInterface**)view);

}

ECode CDragShadowBuilder::OnProvideShadowMetrics(
    /* [in] */ IPoint* shadowSize,
    /* [in] */ IPoint* shadowTouchPoint)
{
    AutoPtr<IView> view;
    mView->Resolve(EIID_IView, (IInterface**)&view);
    if (view != NULL) {
        Int32 w = 0, h = 0;
        view->GetWidth(&w);
        view->GetHeight(&h);
        shadowSize->Set(w, h);
        Int32 x, y;
        shadowSize->GetX(&x);
        shadowSize->GetY(&y);
        shadowTouchPoint->Set(x / 2, y / 2);
    }
    else {
        SLOGGERE("View::DragShadowBuilder", "Asked for drag thumb metrics but no view");
    }

    return NOERROR;
}

ECode CDragShadowBuilder::OnDrawShadow(
    /* [in] */ ICanvas* canvas)
{
    AutoPtr<IView> view;
    mView->Resolve(EIID_IView, (IInterface**)&view);
    if (view != NULL) {
        return view->Draw(canvas);
    }
    else {
        SLOGGERE("View::DragShadowBuilder", "Asked to draw drag shadow but no view")
    }

    return NOERROR;
}

}   //namespace View
}   //namespace Droid
}   //namespace Elastos
