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

#include "elastos/droid/packageinstaller/CCaffeinatedScrollView.h"
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.Droid.View.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::IView;
using Elastos::Droid::Utility::IDisplayMetrics;

namespace Elastos {
namespace Droid {
namespace PackageInstaller {

CAR_INTERFACE_IMPL(CCaffeinatedScrollView, ScrollView, ICaffeinatedScrollView)

CAR_OBJECT_IMPL(CCaffeinatedScrollView)

CCaffeinatedScrollView::CCaffeinatedScrollView()
    : mBottomSlop(0)
{
}

ECode CCaffeinatedScrollView::constructor(
    /* [in] */ IContext* context)
{
    return ScrollView::constructor(context);
}

ECode CCaffeinatedScrollView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ScrollView::constructor(context, attrs);
}

Boolean CCaffeinatedScrollView::AwakenScrollBars()
{
    return ScrollView::AwakenScrollBars();
}

ECode CCaffeinatedScrollView::SetFullScrollAction(
    /* [in] */ IRunnable* action)
{
    VALIDATE_NOT_NULL(action);

    mFullScrollAction = action;
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> displayMetrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&displayMetrics);
    Float density;
    displayMetrics->GetDensity(&density);
    mBottomSlop = (Int32)(4 * density);
    return NOERROR;
}

void CCaffeinatedScrollView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    ScrollView::OnDraw(canvas);
    CheckFullScrollAction();
}

ECode CCaffeinatedScrollView::OnScrollChanged(
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 oldl,
    /* [in] */ Int32 oldt)
{
    ScrollView::OnScrollChanged(l, t, oldl, oldt);
    CheckFullScrollAction();
    return NOERROR;
}

void CCaffeinatedScrollView::CheckFullScrollAction()
{
    if (mFullScrollAction != NULL) {
        AutoPtr<IView> viewTmp;
        GetChildAt(0, (IView**)&viewTmp);
        Int32 daBottom;
        viewTmp->GetBottom(&daBottom);

        Int32 scrollY = 0;
        GetScrollY(&scrollY);
        Int32 height = 0;
        GetHeight(&height);
        Int32 paddingBottom = 0;
        GetPaddingBottom(&paddingBottom);
        Int32 screenBottom = scrollY + height - paddingBottom;
        if ((daBottom - screenBottom) < mBottomSlop) {
            mFullScrollAction->Run();
            mFullScrollAction = NULL;
        }
    }
}

} // namespace PackageInstaller
} // namespace Droid
} // namespace Elastos
