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

#include "Elastos.Droid.View.h"
#include "elastos/droid/settings/notification/CDropDownPreference.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemSelectedListener;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::CSpinner;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

//===============================================================================
//                  CDropDownPreference::MyOnItemSelectedListener
//===============================================================================

CAR_INTERFACE_IMPL(CDropDownPreference::MyOnItemSelectedListener, Object, IAdapterViewOnItemSelectedListener)

CDropDownPreference::MyOnItemSelectedListener::MyOnItemSelectedListener(
    /* [in] */ CDropDownPreference* host)
    : mHost(host)
{}

ECode CDropDownPreference::MyOnItemSelectedListener::OnItemSelected(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return mHost->SetSelectedItem(position);
}

ECode CDropDownPreference::MyOnItemSelectedListener::OnNothingSelected(
    /* [in] */ IAdapterView* parent)
{
    // noop
    return NOERROR;
}

//===============================================================================
//                  CDropDownPreference::MyOnPreferenceClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CDropDownPreference::MyOnPreferenceClickListener, Object, IPreferenceOnPreferenceClickListener)

CDropDownPreference::MyOnPreferenceClickListener::MyOnPreferenceClickListener(
    /* [in] */ CDropDownPreference* host)
    : mHost(host)
{}

ECode CDropDownPreference::MyOnPreferenceClickListener::OnPreferenceClick(
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    Boolean res;
    IView::Probe(mHost->mSpinner)->PerformClick(&res);
    *result = TRUE;
    return NOERROR;
}

//===============================================================================
//                  CDropDownPreference
//===============================================================================

CAR_INTERFACE_IMPL(CDropDownPreference, Preference, IDropDownPreference)

CAR_OBJECT_IMPL(CDropDownPreference)

CDropDownPreference::CDropDownPreference()
{
    CArrayList::New((IArrayList**)&mValues);
}

ECode CDropDownPreference::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CDropDownPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    Preference::constructor(context, attrs);
    mContext = context;
    CArrayAdapter::New(mContext,
            Elastos::Droid::R::layout::simple_spinner_dropdown_item,
            (IArrayAdapter**)&mAdapter);

    CSpinner::New(mContext, (ISpinner**)&mSpinner);

    IView::Probe(mSpinner)->SetVisibility(IView::INVISIBLE);
    IAdapterView* spinner = IAdapterView::Probe(mSpinner);
    spinner->SetAdapter(IAdapter::Probe(mAdapter));
    AutoPtr<IAdapterViewOnItemSelectedListener> listener = new MyOnItemSelectedListener(this);
    spinner->SetOnItemSelectedListener(listener);

    SetPersistent(FALSE);
    AutoPtr<MyOnPreferenceClickListener> listener1 = new MyOnPreferenceClickListener(this);
    SetOnPreferenceClickListener(listener1);
    return NOERROR;
}

ECode CDropDownPreference::SetDropDownWidth(
    /* [in] */ Int32 dimenResId)
{
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    Int32 size;
    resources->GetDimensionPixelSize(dimenResId, &size);
    mSpinner->SetDropDownWidth(size);
    return NOERROR;
}

ECode CDropDownPreference::SetCallback(
    /* [in] */ IDropDownPreferenceCallback* callback)
{
    mCallback = callback;
    return NOERROR;
}

ECode CDropDownPreference::SetSelectedItem(
    /* [in] */ Int32 position)
{
    AutoPtr<IInterface> value;
    mValues->Get(position, (IInterface**)&value);
    Boolean res;
    if (mCallback != NULL && (mCallback->OnItemSelected(position, value, &res), !res)) {
        return NOERROR;
    }
    IAdapterView::Probe(mSpinner)->SetSelection(position);
    AutoPtr<IInterface> obj;
    IAdapter::Probe(mAdapter)->GetItem(position, (IInterface**)&obj);
    SetSummary(ICharSequence::Probe(obj));
    const Boolean disableDependents = value == NULL;
    return NotifyDependencyChange(disableDependents);
}

ECode CDropDownPreference::SetSelectedValue(
    /* [in] */ IInterface* value)
{
    Int32 i;
    mValues->IndexOf(value, &i);
    if (i > -1) {
        SetSelectedItem(i);
    }
    return NOERROR;
}

ECode CDropDownPreference::AddItem(
    /* [in] */ Int32 captionResid,
    /* [in] */ IInterface* value)
{
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    String str;
    resources->GetString(captionResid, &str);
    AddItem(str, value);
    return NOERROR;
}

ECode CDropDownPreference::AddItem(
    /* [in] */ const String& caption,
    /* [in] */ IInterface* value)
{
    mAdapter->Add(CoreUtils::Convert(caption));
    mValues->Add(value);
    return NOERROR;
}

ECode CDropDownPreference::ClearItems()
{
    mAdapter->Clear();
    mValues->Clear();
    return NOERROR;
}

ECode CDropDownPreference::OnBindView(
    /* [in] */ IView* view)
{
    Preference::OnBindView(view);
    AutoPtr<IViewParent> parent;
    IView* spinner = IView::Probe(mSpinner);
    spinner->GetParent((IViewParent**)&parent);
    Boolean res;
    IObject::Probe(view)->Equals(parent, &res);
    if (res) return NOERROR;
    if (parent != NULL) {
        IViewGroup::Probe(parent)->RemoveView(spinner);
    }
    IViewGroup* vg = IViewGroup::Probe(view);
    vg->AddView(spinner, 0);

    AutoPtr<IViewGroupLayoutParams> lp;
    spinner->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    lp->SetWidth(0);
    return spinner->SetLayoutParams(lp);
}

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos