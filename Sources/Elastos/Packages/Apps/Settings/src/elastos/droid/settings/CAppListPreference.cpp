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
#include "elastos/droid/settings/CAppListPreference.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::Widget::ICheckedTextView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Settings {

//===============================================================================
//                  CAppListPreference::AppArrayAdapter
//===============================================================================

CAppListPreference::AppArrayAdapter::AppArrayAdapter()
    : mSelectedIndex(0)
{}

ECode CAppListPreference::AppArrayAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 textViewResourceId,
    /* [in] */ ArrayOf<ICharSequence*>* objects,
    /* [in] */ ArrayOf<IDrawable*>* imageDrawables,
    /* [in] */ Int32 selectedIndex)
{
    Int32 length = objects->GetLength();
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        args->Set(i, (*objects)[i]);
    }
    ArrayAdapter::constructor(context, textViewResourceId, args);
    mSelectedIndex = selectedIndex;
    mImageDrawables = imageDrawables;
    return NOERROR;
}

ECode CAppListPreference::AppArrayAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<ILayoutInflater> inflater;
    IActivity::Probe(context)->GetLayoutInflater((ILayoutInflater**)&inflater);
    AutoPtr<IView> view;
    inflater->Inflate(R::layout::app_preference_item, parent, FALSE, (IView**)&view);
    AutoPtr<IView> tmp;
    view->FindViewById(R::id::app_label, (IView**)&tmp);
    ICheckedTextView* checkedTextView = ICheckedTextView::Probe(tmp);
    AutoPtr<IInterface> obj;
    GetItem(position, (IInterface**)&obj);
    ITextView::Probe(checkedTextView)->SetText(ICharSequence::Probe(obj));
    if (position == mSelectedIndex) {
        ICheckable::Probe(checkedTextView)->SetChecked(TRUE);
    }
    tmp = NULL;
    view->FindViewById(R::id::app_image, (IView**)&tmp);
    IImageView* imageView = IImageView::Probe(tmp);
    imageView->SetImageDrawable((*mImageDrawables)[position]);
    *result = view;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//===============================================================================
//                  CAppListPreference
//===============================================================================

CAR_OBJECT_IMPL(CAppListPreference)

CAppListPreference::CAppListPreference()
{}

CAppListPreference::~CAppListPreference()
{}

ECode CAppListPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ListPreference::constructor(context, attrs);
}

ECode CAppListPreference::SetPackageNames(
    /* [in] */ ArrayOf<String>* packageNames,
    /* [in] */ const String& defaultPackageName)
{
    // Look up all package names in PackageManager. Skip ones we can't find.
    Int32 foundPackages = 0;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    Int32 length = packageNames->GetLength();
    AutoPtr< ArrayOf<IApplicationInfo*> > appInfos = ArrayOf<IApplicationInfo*>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        // try {
        AutoPtr<IApplicationInfo> info;
        ECode ec = pm->GetApplicationInfo((*packageNames)[i], 0, (IApplicationInfo**)&info);
        if (SUCCEEDED(ec)) {
            appInfos->Set(i, info);
            foundPackages++;
        }
        // } catch (NameNotFoundException e) {
        //     // Leave appInfos[i] uninitialized; it will be skipped in the list.
        // }
    }

    // Show the label and icon for each application package.
    AutoPtr< ArrayOf<ICharSequence*> > applicationNames = ArrayOf<ICharSequence*>::Alloc(foundPackages);
    mEntryDrawables = ArrayOf<IDrawable*>::Alloc(foundPackages);
    Int32 index = 0;
    Int32 selectedIndex = -1;
    for (Int32 i = 0; i < appInfos->GetLength(); i++) {
        AutoPtr<IApplicationInfo> appInfo = (*appInfos)[i];

        if (appInfo != NULL) {
            IPackageItemInfo* _appInfo = IPackageItemInfo::Probe(appInfo);
            AutoPtr<ICharSequence> cs;
            _appInfo->LoadLabel(pm, (ICharSequence**)&cs);
            applicationNames->Set(index, cs);
            AutoPtr<IDrawable> drawable;
            _appInfo->LoadIcon(pm, (IDrawable**)&drawable);
            mEntryDrawables->Set(index, drawable);
            String packageName;
            if (!defaultPackageName.IsNull() &&
                    (_appInfo->GetPackageName(&packageName), packageName).Equals(defaultPackageName)) {
                selectedIndex = index;
            }
            index++;
        }
    }
    SetEntries(applicationNames);
    AutoPtr< ArrayOf<ICharSequence*> > entriesValues = ArrayOf<ICharSequence*>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        entriesValues->Set(i, CoreUtils::Convert((*packageNames)[i]));
    }
    SetEntryValues(entriesValues);
    if (selectedIndex != -1) {
        SetValueIndex(selectedIndex);
    }
    return NOERROR;
}

ECode CAppListPreference::OnPrepareDialogBuilder(
    /* [in] */ IAlertDialogBuilder* builder)
{
    String value;
    GetValue(&value);
    Int32 selectedIndex;
    FindIndexOfValue(value, &selectedIndex);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr< ArrayOf<ICharSequence*> > entries;
    GetEntries((ArrayOf<ICharSequence*>**)&entries);
    AutoPtr<AppArrayAdapter> adapter = new AppArrayAdapter();
    adapter->constructor(context, R::layout::app_preference_item, entries, mEntryDrawables, selectedIndex);
    builder->SetAdapter((IListAdapter*)adapter, this);
    return ListPreference::OnPrepareDialogBuilder(builder);
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos