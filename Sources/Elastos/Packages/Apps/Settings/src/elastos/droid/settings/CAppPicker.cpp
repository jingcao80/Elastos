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

#include "elastos/droid/settings/CAppPicker.h"
#include "elastos/droid/settings/applications/AppViewHolder.h"
#include "elastos/droid/os/Build.h"
#include "../R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Settings::Applications::AppViewHolder;

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::IProcess;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IComparator;
using Elastos::Text::ICollatorHelper;
using Elastos::Text::CCollatorHelper;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Settings {

AutoPtr<IComparator> CAppPicker::sDisplayNameComparator = new MyComparator();

//===============================================================================
//                  CAppPicker::AppListAdapter
//===============================================================================

CAppPicker::AppListAdapter::AppListAdapter(
    /* [in] */ CAppPicker* host)
    : mHost(host)
{}

ECode CAppPicker::AppListAdapter::constructor(
    /* [in] */ IContext* context)
{
    CArrayList::New((IList**)&mPackageInfoList);

    ArrayAdapter::constructor(context, 0);

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    mInflater = ILayoutInflater::Probe(obj);
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IList> pkgs; // List<ApplicationInfo>
    pm->GetInstalledApplications(0, (IList**)&pkgs);
    Int32 size;
    pkgs->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> aiObj;
        pkgs->Get(i, (IInterface**)&aiObj);
        IApplicationInfo* ai = IApplicationInfo::Probe(aiObj);
        Int32 uid;
        ai->GetUid(&uid);
        if (uid == IProcess::SYSTEM_UID) {
            continue;
        }
        // On a user build, we only allow debugging of apps that
        // are marked as debuggable.  Otherwise (for platform development)
        // we allow all apps.
        Int32 flags;
        ai->GetFlags(&flags);
        if ((flags & IApplicationInfo::FLAG_DEBUGGABLE) == 0
                && String("user").Equals(Build::TYPE)) {
            continue;
        }
        AutoPtr<MyApplicationInfo> info = new MyApplicationInfo();
        info->mInfo = ai;
        AutoPtr<IPackageManager> packageManager;
        mHost->GetPackageManager((IPackageManager**)&packageManager);
        IPackageItemInfo::Probe(info->mInfo)->LoadLabel(packageManager, (ICharSequence**)&(info->mLabel));
        mPackageInfoList->Add((IObject*)info);
    }

    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);
    coll->Sort(mPackageInfoList, sDisplayNameComparator);
    AutoPtr<MyApplicationInfo> info = new MyApplicationInfo();
    context->GetText(R::string::no_application, (ICharSequence**)&(info->mLabel));
    mPackageInfoList->Add(0, (IObject*)info);
    AddAll(ICollection::Probe(mPackageInfoList));
    return NOERROR;
}

ECode CAppPicker::AppListAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* _convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IView> convertView = _convertView;

    // A ViewHolder keeps references to children views to avoid unnecessary calls
    // to FindViewById() on each row.
    AutoPtr<AppViewHolder> holder = AppViewHolder::CreateOrRecycle(mInflater, convertView);
    convertView = holder->mRootView;
    AutoPtr<IInterface> obj;
    GetItem(position, (IInterface**)&obj);
    MyApplicationInfo* info = (MyApplicationInfo*) IObject::Probe(obj);
    holder->mAppName->SetText(info->mLabel);
    if (info->mInfo != NULL) {
        AutoPtr<IPackageManager> pm;
        mHost->GetPackageManager((IPackageManager**)&pm);
        IPackageItemInfo* pii = IPackageItemInfo::Probe(info->mInfo);
        AutoPtr<IDrawable> drawable;
        pii->LoadIcon(pm, (IDrawable**)&drawable);
        holder->mAppIcon->SetImageDrawable(drawable);
        String packageName;
        pii->GetPackageName(&packageName);
        holder->mAppSize->SetText(CoreUtils::Convert(packageName));
    }
    else {
        holder->mAppIcon->SetImageDrawable(NULL);
        holder->mAppSize->SetText(CoreUtils::Convert(""));
    }
    IView::Probe(holder->mDisabled)->SetVisibility(IView::GONE);
    IView::Probe(holder->mCheckBox)->SetVisibility(IView::GONE);

    *result = convertView;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//===============================================================================
//                  CAppPicker::MyComparator
//===============================================================================

CAR_INTERFACE_IMPL(CAppPicker::MyComparator, Object, IComparator)

CAppPicker::MyComparator::MyComparator()
{
    AutoPtr<ICollatorHelper> helper;
    CCollatorHelper::AcquireSingleton((ICollatorHelper**)&helper);
    helper->GetInstance((ICollator**)&mCollator);
}

ECode CAppPicker::MyComparator::Compare(
    /* [in] */ IInterface* left,
    /* [in] */ IInterface* right,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    MyApplicationInfo* a = (MyApplicationInfo*)IObject::Probe(left);
    MyApplicationInfo* b = (MyApplicationInfo*)IObject::Probe(right);

    return mCollator->Compare(a->mLabel, b->mLabel, result);
}

//===============================================================================
//                  CAppPicker
//===============================================================================

CAR_OBJECT_IMPL(CAppPicker)

CAppPicker::CAppPicker()
{}

CAppPicker::~CAppPicker()
{}

ECode CAppPicker::constructor()
{
    return ListActivity::constructor();
}

ECode CAppPicker::OnCreate(
    /* [in] */ IBundle* icicle)
{
    ListActivity::OnCreate(icicle);

    mAdapter = new AppListAdapter(this);
    mAdapter->constructor(this);
    Int32 count;
    mAdapter->GetCount(&count);
    if (count <= 0) {
        Finish();
    }
    else {
        SetListAdapter(mAdapter);
    }
    return NOERROR;
}

ECode CAppPicker::OnResume()
{
    return ListActivity::OnResume();
}

ECode CAppPicker::OnStop()
{
    return ListActivity::OnStop();
}

ECode CAppPicker::OnListItemClick(
    /* [in] */ IListView* l,
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<IInterface> obj;
    mAdapter->GetItem(position, (IInterface**)&obj);
    MyApplicationInfo* app = (MyApplicationInfo*) IObject::Probe(obj);
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    if (app->mInfo != NULL) {
        String packageName;
        IPackageItemInfo::Probe(app->mInfo)->GetPackageName(&packageName);
        intent->SetAction(packageName);
    }
    SetResult(RESULT_OK, intent);
    Finish();
    return NOERROR;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos