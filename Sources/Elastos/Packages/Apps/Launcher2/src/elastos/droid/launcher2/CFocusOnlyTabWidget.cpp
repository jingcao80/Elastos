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

#include "elastos/droid/launcher2/CFocusOnlyTabWidget.h"
#include "Elastos.Droid.Service.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(CFocusOnlyTabWidget, TabWidget, IFocusOnlyTabWidget);

CAR_OBJECT_IMPL(CFocusOnlyTabWidget);

ECode CFocusOnlyTabWidget::GetSelectedTab(
    /* [out] */ IView** tab)
{
    VALIDATE_NOT_NULL(tab);

    Int32 count;
    GetTabCount(&count);
    for (Int32 i = 0; i < count; ++i) {
        AutoPtr<IView> v;
        GetChildTabViewAt(i, (IView**)&v);
        Boolean res;
        v->IsSelected(&res);
        if (res) {
            *tab = v;
            REFCOUNT_ADD(*tab);
            return NOERROR;
        }
    }
    *tab = NULL;
    return NOERROR;
}

ECode CFocusOnlyTabWidget::GetChildTabIndex(
    /* [in] */ IView* v,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);

    Int32 tabCount;
    GetTabCount(&tabCount);
    for (Int32 i = 0; i < tabCount; ++i) {
        AutoPtr<IView> tmp;
        GetChildTabViewAt(i, (IView**)&tmp);
        if (TO_IINTERFACE(tmp) == TO_IINTERFACE(v)) {
            *index = i;
            return NOERROR;
        }
    }
    *index = -1;
    return NOERROR;
}

ECode CFocusOnlyTabWidget::SetCurrentTabToFocusedTab()
{
    AutoPtr<IView> tab;
    Int32 index = -1;
    Int32 count;
    GetTabCount(&count);
    for (Int32 i = 0; i < count; ++i) {
        AutoPtr<IView> v;
        GetChildTabViewAt(i, (IView**)&v);
        Boolean res;
        v->HasFocus(&res);
        if (res) {
            tab = v;
            index = i;
            break;
        }
    }
    if (index > -1) {
        TabWidget::SetCurrentTab(index);
        TabWidget::OnFocusChange(tab, TRUE);
    }
    return NOERROR;
}

ECode CFocusOnlyTabWidget::SuperOnFocusChange(
    /* [in] */ IView* v,
    /* [in] */ Boolean hasFocus)
{
    return TabWidget::OnFocusChange(v, hasFocus);
}

ECode CFocusOnlyTabWidget::OnFocusChange(
    /* [in] */ IView* v,
    /* [in] */ Boolean hasFocus)
{
    if (TO_IINTERFACE(v) == TO_IINTERFACE(this) && hasFocus) {
        Int32 count;
        GetTabCount(&count);
        if (count > 0) {
            AutoPtr<IView> view;
            GetSelectedTab((IView**)&view);
            Boolean res;
            view->RequestFocus(&res);
            return NOERROR;
        }
    }
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos