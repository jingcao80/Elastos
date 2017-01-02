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

#include "elastos/droid/launcher2/HideFromAccessibilityHelper.h"
#include "Elastos.Droid.Service.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Launcher2::ECLSID_CAppsCustomizeTabHost;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewGroupOnHierarchyChangeListener;
using Elastos::Droid::View::EIID_IViewGroupOnHierarchyChangeListener;
using Elastos::Core::IInteger32;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(HideFromAccessibilityHelper, Object, IViewGroupOnHierarchyChangeListener);

HideFromAccessibilityHelper::HideFromAccessibilityHelper()
    : mHide(FALSE)
    , mOnlyAllApps(FALSE)
{
    CHashMap::New((IHashMap**)&mPreviousValues);
}

ECode HideFromAccessibilityHelper::SetImportantForAccessibilityToNo(
    /* [in] */ IView* v,
    /* [in] */ Boolean onlyAllApps)
{
    mOnlyAllApps = onlyAllApps;
    SetImportantForAccessibilityToNoHelper(v);
    mHide = TRUE;
    return NOERROR;
}

void HideFromAccessibilityHelper::SetImportantForAccessibilityToNoHelper(
    /* [in] */ IView* v)
{
    Int32 mode;
    v->GetImportantForAccessibility(&mode);
    AutoPtr<IInteger32> obj = CoreUtils::Convert(mode);
    mPreviousValues->Put(TO_IINTERFACE(v), TO_IINTERFACE(obj));
    v->SetImportantForAccessibility(IView::IMPORTANT_FOR_ACCESSIBILITY_NO);

    // Call method on children recursively
    if (IViewGroup::Probe(v) != NULL) {
        AutoPtr<IViewGroup> vg = IViewGroup::Probe(v);
        vg->SetOnHierarchyChangeListener(this);
        Int32 count;
        vg->GetChildCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> child;
            vg->GetChildAt(i, (IView**)&child);

            if (IncludeView(child)) {
                SetImportantForAccessibilityToNoHelper(child);
            }
        }
    }
}

ECode HideFromAccessibilityHelper::RestoreImportantForAccessibility(
    /* [in] */ IView* v)
{
    if (mHide) {
        RestoreImportantForAccessibilityHelper(v);
    }
    mHide = FALSE;
    return NOERROR;
}

void HideFromAccessibilityHelper::RestoreImportantForAccessibilityHelper(
    /* [in] */ IView* v)
{
    AutoPtr<IInterface> obj;
    mPreviousValues->Get(TO_IINTERFACE(v), (IInterface**)&obj);
    AutoPtr<IInteger32> intObj = IInteger32::Probe(obj);
    Int32 value;
    intObj->GetValue(&value);
    v->SetImportantForAccessibility(value);
    mPreviousValues->Remove(TO_IINTERFACE(v));

    // Call method on children recursively
    AutoPtr<IViewGroup> vg = IViewGroup::Probe(v);
    if (vg != NULL) {
        // We assume if a class implements OnHierarchyChangeListener, it listens
        // to changes to any of its children (happens to be the case in Launcher)
        IViewGroupOnHierarchyChangeListenerHolder* holder = IViewGroupOnHierarchyChangeListenerHolder::Probe(vg);
        if (holder != NULL) {
            AutoPtr<IViewGroupOnHierarchyChangeListener> listener;
            holder->GetViewGroupHierarchyChangeListener((IViewGroupOnHierarchyChangeListener**)&listener);
            vg->SetOnHierarchyChangeListener(listener);
        }
        else {
            IViewGroupOnHierarchyChangeListener* listener = IViewGroupOnHierarchyChangeListener::Probe(vg);
            if (listener != NULL) {
                Logger::E("HideFromAccessibilityHelper",
                    "Error: %s should implements IViewGroupOnHierarchyChangeListenerHolder"
                    " instead of IViewGroupOnHierarchyChangeListener to avoid circular reference.", TO_CSTR(vg));
                assert(0 && "view should implements IViewGroupOnHierarchyChangeListenerHolder"
                    " instead of IViewGroupOnHierarchyChangeListener to avoid circular reference.");
            }

            vg->SetOnHierarchyChangeListener(NULL);
        }
        Int32 count;
        vg->GetChildCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> child;
            vg->GetChildAt(i, (IView**)&child);
            if (IncludeView(child)) {
                RestoreImportantForAccessibilityHelper(child);
            }
        }
    }
}

ECode HideFromAccessibilityHelper::OnChildViewAdded(
    /* [in] */ IView* parent,
    /* [in] */ IView* child)
{
    if (mHide && IncludeView(child)) {
        SetImportantForAccessibilityToNoHelper(child);
    }
    return NOERROR;
}

ECode HideFromAccessibilityHelper::OnChildViewRemoved(
    /* [in] */ IView* parent,
    /* [in] */ IView* child)
{
    if (mHide && IncludeView(child)) {
        RestoreImportantForAccessibilityHelper(child);
    }
    return NOERROR;
}

Boolean HideFromAccessibilityHelper::IncludeView(
    /* [in] */ IView* v)
{
    return !HasAncestorOfType(v, ECLSID_CCling) &&
                (!mOnlyAllApps || HasAncestorOfType(v, ECLSID_CAppsCustomizeTabHost));
}

Boolean HideFromAccessibilityHelper::HasAncestorOfType(
    /* [in] */ IView* v,
    /* [in] */ ClassID c)
{
    if (v == NULL) {
        return FALSE;
    }
    else {
        ClassID clsId;
        IObject::Probe(v)->GetClassID(&clsId);
        if (c == clsId) {
            return TRUE;
        }

        AutoPtr<IViewParent> parentParent;
        v->GetParent((IViewParent**)&parentParent);
        if (IViewGroup::Probe(parentParent) == NULL) {
            return FALSE;
        }

        AutoPtr<IViewGroup> vg = IViewGroup::Probe(parentParent);
        return HasAncestorOfType(IView::Probe(vg), c);
    }
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos