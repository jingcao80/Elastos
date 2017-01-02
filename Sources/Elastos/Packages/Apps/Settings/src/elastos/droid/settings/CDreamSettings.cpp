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

#include "elastos/droid/settings/CDreamSettings.h"
#include "elastos/droid/settings/CSettingsActivity.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/settings/widget/CSwitchBar.h"
#include "../R.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Settings::Widget::CSwitchBar;
using Elastos::Droid::Settings::Widget::EIID_ISwitchBarOnSwitchChangeListener;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::View::EIID_IOnMenuItemClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IRadioButton;
using Elastos::Droid::Widget::ITextView;
using Elastos::Utility::IList;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {

//===============================================================================
//                  CDreamSettings::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL(CDreamSettings::InnerListener, Object, ISwitchBarOnSwitchChangeListener)

CDreamSettings::InnerListener::InnerListener(
    /* [in] */ CDreamSettings* host)
    : mHost(host)
{}

ECode CDreamSettings::InnerListener::OnSwitchChanged(
    /* [in] */ ISwitch* switchView,
    /* [in] */ Boolean isChecked)
{
    return mHost->OnSwitchChanged(switchView, isChecked);
}

//===============================================================================
//                  CDreamSettings::MyRunnable
//===============================================================================

CDreamSettings::MyRunnable::MyRunnable(
    /* [in] */ CDreamSettings* host,
    /* [in] */ Int32 id)
    : mHost(host)
    , mId(id)
{}

ECode CDreamSettings::MyRunnable::Run()
{
    switch (mId) {
        case 0:
            mHost->mBackend->StartDreaming();
            break;
        case 1:
            mHost->ShowDialog(DIALOG_WHEN_TO_DREAM);
            break;
    }
    return NOERROR;
}

//===============================================================================
//                  CDreamSettings::OnMenuItemClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CDreamSettings::OnMenuItemClickListener, Object, IOnMenuItemClickListener)

CDreamSettings::OnMenuItemClickListener::OnMenuItemClickListener(
    /* [in] */ IRunnable* onClick)
    : mOnClick(onClick)
{}

ECode CDreamSettings::OnMenuItemClickListener::OnMenuItemClick(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    mOnClick->Run();
    *result = TRUE;
    return NOERROR;
}

//===============================================================================
//                  CDreamSettings::DialogInterfaceOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CDreamSettings::DialogInterfaceOnClickListener, Object, IDialogInterfaceOnClickListener)

CDreamSettings::DialogInterfaceOnClickListener::DialogInterfaceOnClickListener(
    /* [in] */ CDreamSettings* host)
    : mHost(host)
{}

ECode CDreamSettings::DialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 item)
{
    mHost->mBackend->SetActivatedOnDock(item == 0 || item == 2);
    mHost->mBackend->SetActivatedOnSleep(item == 1 || item == 2);
    dialog->Dismiss();
    return NOERROR;
}

//===============================================================================
//                  CDreamSettings::DreamInfoAdapter
//===============================================================================

CDreamSettings::DreamInfoAdapter::DreamInfoAdapter(
    /* [in] */ CDreamSettings* host,
    /* [in] */ IContext* context)
    : mHost(host)
{
    ArrayAdapter::constructor(context, 0);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    mInflater = ILayoutInflater::Probe(obj);
}

ECode CDreamSettings::DreamInfoAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IInterface> obj;
    GetItem(position, (IInterface**)&obj);
    DreamBackend::DreamInfo* dreamInfo = (DreamBackend::DreamInfo*)IObject::Probe(obj);

    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, dreamInfo->mCaption);

    Logd(String("GetView(%s)"), args);
    AutoPtr<IView> row;
    if (convertView != NULL) {
        row = convertView;
    }
    else {
        row = CreateDreamInfoRow(parent);
    }
    row->SetTag((IObject*)dreamInfo);

    // bind icon
    AutoPtr<IView> tmp;
    row->FindViewById(Elastos::Droid::R::id::icon, (IView**)&tmp);
    IImageView::Probe(tmp)->SetImageDrawable(dreamInfo->mIcon);

    // bind caption
    tmp = NULL;
    row->FindViewById(Elastos::Droid::R::id::title, (IView**)&tmp);
    ITextView::Probe(tmp)->SetText(dreamInfo->mCaption);

    // bind radio button
    AutoPtr<IView> radioButtonTmp;
    row->FindViewById(Elastos::Droid::R::id::button1, (IView**)&radioButtonTmp);
    AutoPtr<IRadioButton> radioButton = IRadioButton::Probe(radioButtonTmp);
    ICheckable::Probe(radioButton)->SetChecked(dreamInfo->mIsActive);
    AutoPtr<MyViewOnTouchListener> touchListener = new MyViewOnTouchListener(this, row);
    radioButtonTmp->SetOnTouchListener(touchListener);

    // bind settings button + divider
    Boolean showSettings = dreamInfo->mSettingsComponentName != NULL;
    AutoPtr<IView> settingsDivider;
    row->FindViewById(R::id::divider, (IView**)&settingsDivider);
    settingsDivider->SetVisibility(showSettings ? IView::VISIBLE : IView::INVISIBLE);

    AutoPtr<IView> settingsButtonTmp;
    row->FindViewById(Elastos::Droid::R::id::button2, (IView**)&settingsButtonTmp);
    AutoPtr<IImageView> settingsButton = IImageView::Probe(settingsButtonTmp);
    settingsButtonTmp->SetVisibility(showSettings ? IView::VISIBLE : IView::INVISIBLE);
    settingsButton->SetAlpha(dreamInfo->mIsActive ? 1.0f : Utils::DISABLED_ALPHA);
    settingsButtonTmp->SetEnabled(dreamInfo->mIsActive);
    settingsButtonTmp->SetFocusable(dreamInfo->mIsActive);
    AutoPtr<IViewOnClickListener> listener = new DreamInfoAdapterOnClickListener(mHost, this, row, 0);
    settingsButtonTmp->SetOnClickListener(listener);

    *result = row;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

AutoPtr<IView> CDreamSettings::DreamInfoAdapter::CreateDreamInfoRow(
    /* [in] */ IViewGroup* parent)
{
    AutoPtr<IView> row;
    mInflater->Inflate(R::layout::dream_info_row, parent, FALSE, (IView**)&row);
    AutoPtr<IView> header;
    row->FindViewById(Elastos::Droid::R::id::widget_frame, (IView**)&header);
    AutoPtr<IViewOnClickListener> listener = new DreamInfoAdapterOnClickListener(mHost, this, row, 1);
    header->SetOnClickListener(listener);
    return row;
}

AutoPtr<DreamBackend::DreamInfo> CDreamSettings::DreamInfoAdapter::GetCurrentSelection()
{
    Int32 count;
    GetCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        GetItem(i, (IInterface**)&obj);
        AutoPtr<DreamBackend::DreamInfo> dreamInfo = (DreamBackend::DreamInfo*) IObject::Probe(obj);
        if (dreamInfo->mIsActive)
            return dreamInfo;
    }
    return NULL;
}

void CDreamSettings::DreamInfoAdapter::Activate(
    /* [in] */ DreamBackend::DreamInfo* dreamInfo)
{
    if (Object::Equals((IObject*)dreamInfo, (IObject*)GetCurrentSelection())) {
        return;
    }
    Int32 count;
    GetCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        GetItem(i, (IInterface**)&obj);
        ((DreamBackend::DreamInfo*) IObject::Probe(obj))->mIsActive = FALSE;
    }
    dreamInfo->mIsActive = TRUE;
    mHost->mBackend->SetActiveDream(dreamInfo->mComponentName);
    NotifyDataSetChanged();
}

//===============================================================================
//                  CDreamSettings::MyViewOnTouchListener
//===============================================================================

CAR_INTERFACE_IMPL(CDreamSettings::MyViewOnTouchListener, Object, IViewOnTouchListener)

CDreamSettings::MyViewOnTouchListener::MyViewOnTouchListener(
    /* [in] */ DreamInfoAdapter* host,
    /* [in] */ IView* row)
    : mHost(host)
    , mRow(row)
{}

ECode CDreamSettings::MyViewOnTouchListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean res;
    mRow->OnTouchEvent(event, &res);
    *result = FALSE;
    return NOERROR;
}

//===============================================================================
//                  CDreamSettings::DreamInfoAdapterOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CDreamSettings::DreamInfoAdapterOnClickListener, Object, IViewOnClickListener)

CDreamSettings::DreamInfoAdapterOnClickListener::DreamInfoAdapterOnClickListener(
    /* [in] */ CDreamSettings* host,
    /* [in] */ DreamInfoAdapter* owner,
    /* [in] */ IView* row,
    /* [in] */ Int32 id)
    : mHost(host)
    , mOwner(owner)
    , mRow(row)
    , mId(id)
{}

ECode CDreamSettings::DreamInfoAdapterOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    switch (mId) {
        case 0: {
            AutoPtr<IInterface> obj;
            mRow->GetTag((IInterface**)&obj);
            mHost->mBackend->LaunchSettings((DreamBackend::DreamInfo*)IObject::Probe(obj));
            return NOERROR;
        }
        case 1: {
            v->SetPressed(TRUE);
            AutoPtr<IInterface> obj;
            mRow->GetTag((IInterface**)&obj);
            mOwner->Activate((DreamBackend::DreamInfo*)IObject::Probe(obj));
            return NOERROR;
        }

    }
    return NOERROR;
}

//===============================================================================
//                  CDreamSettings::PackageReceiver
//===============================================================================

CDreamSettings::PackageReceiver::PackageReceiver(
    /* [in] */ CDreamSettings* host)
    : mHost(host)
{}

ECode CDreamSettings::PackageReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Logd(String("PackageReceiver.onReceive"), NULL);
    mHost->RefreshFromBackend();
    return NOERROR;
}

//===============================================================================
//                  CDreamSettings
//===============================================================================

const Boolean CDreamSettings::DEBUG;

const String CDreamSettings::TAG("CDreamSettings");
const Int32 CDreamSettings::DIALOG_WHEN_TO_DREAM = 1;
const String CDreamSettings::PACKAGE_SCHEME("package");

CAR_OBJECT_IMPL(CDreamSettings)

CDreamSettings::CDreamSettings()
    : mContext(NULL)
    , mRefreshing(FALSE)
{}

CDreamSettings::~CDreamSettings()
{}

ECode CDreamSettings::constructor()
{
    mPackageReceiver = new PackageReceiver(this);
    return SettingsPreferenceFragment::constructor();
}

Int32 CDreamSettings::GetHelpResource()
{
    return R::string::help_url_dreams;
}

ECode CDreamSettings::OnAttach(
    /* [in] */ IActivity* activity)
{
    AutoPtr<IClassInfo> klass;
    CObject::ReflectClassInfo(activity, (IClassInfo**)&klass);
    if (klass == NULL) {
        Logger::E("CDreamSettings", "can not find activity's class infomations");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String className;
    klass->GetName(&className);

    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, CoreUtils::Convert(className));

    Logd(String("OnAttach(%s)"), args);
    SettingsPreferenceFragment::OnAttach(activity);
    mContext = IContext::Probe(activity);
    return NOERROR;
}

ECode CDreamSettings::OnCreate(
    /* [in] */ IBundle* icicle)
{
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, icicle);
    Logd(String("OnCreate(%s)"), args);
    SettingsPreferenceFragment::OnCreate(icicle);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    mBackend = new DreamBackend(IContext::Probe(activity));

    SetHasOptionsMenu(TRUE);
    return NOERROR;
}

ECode CDreamSettings::OnSwitchChanged(
    /* [in] */ ISwitch* switchView,
    /* [in] */ Boolean isChecked)
{
    if (!mRefreshing) {
        mBackend->SetEnabled(isChecked);
        RefreshFromBackend();
    }
    return NOERROR;
}

ECode CDreamSettings::OnStart()
{
    Logd(String("OnStart()"), NULL);
    return SettingsPreferenceFragment::OnStart();
}

ECode CDreamSettings::OnDestroyView()
{
    Logd(String("OnDestroyView()"), NULL);
    SettingsPreferenceFragment::OnDestroyView();

    CSwitchBar* switchBar = (CSwitchBar*)mSwitchBar.Get();
    AutoPtr<InnerListener> listener = new InnerListener(this);
    switchBar->RemoveOnSwitchChangeListener(listener);
    switchBar->Hide();
    return NOERROR;
}

ECode CDreamSettings::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, savedInstanceState);
    Logd(String("OnActivityCreated(%s)"), args);
    SettingsPreferenceFragment::OnActivityCreated(savedInstanceState);

    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    listView->SetItemsCanFocus(TRUE);

    AutoPtr<IView> view;
    GetView((IView**)&view);
    AutoPtr<IView> emptyViewTmp;
    view->FindViewById(Elastos::Droid::R::id::empty, (IView**)&emptyViewTmp);
    ITextView* emptyView = ITextView::Probe(emptyViewTmp);
    emptyView->SetText(R::string::screensaver_settings_disabled_prompt);
    IAdapterView* _listView = IAdapterView::Probe(listView);
    _listView->SetEmptyView(emptyViewTmp);

    mAdapter = new DreamInfoAdapter(this, mContext);
    _listView->SetAdapter(mAdapter);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    CSettingsActivity* sa = (CSettingsActivity*)ISettingsActivity::Probe(activity);
    sa->GetSwitchBar((ISwitchBar**)&mSwitchBar);
    CSwitchBar* switchBar = (CSwitchBar*)mSwitchBar.Get();
    AutoPtr<InnerListener> listener = new InnerListener(this);
    switchBar->AddOnSwitchChangeListener(listener);
    switchBar->Show();
    return NOERROR;
}

ECode CDreamSettings::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [in] */ IMenuInflater* inflater)
{
    Logd(String("OnCreateOptionsMenu()"), NULL);

    Boolean isEnabled = mBackend->IsEnabled();

    // create "start" action
    AutoPtr<MyRunnable> runnable0 = new MyRunnable(this, 0);
    AutoPtr<IMenuItem> start = CreateMenuItem(menu, R::string::screensaver_settings_dream_start,
            IMenuItem::SHOW_AS_ACTION_NEVER,
            isEnabled, runnable0);

    // create "when to dream" overflow menu item
    AutoPtr<MyRunnable> runnable1 = new MyRunnable(this, 1);
    AutoPtr<IMenuItem> whenToDream = CreateMenuItem(menu,
            R::string::screensaver_settings_when_to_dream,
            IMenuItem::SHOW_AS_ACTION_NEVER,
            isEnabled, runnable1);

    // create "help" overflow menu item (make sure it appears last)
    SettingsPreferenceFragment::OnCreateOptionsMenu(menu, inflater);

    mMenuItemsWhenEnabled = ArrayOf<IMenuItem*>::Alloc(2);
    mMenuItemsWhenEnabled->Set(0, start);
    mMenuItemsWhenEnabled->Set(1, whenToDream);

    return NOERROR;
}

AutoPtr<IMenuItem> CDreamSettings::CreateMenuItem(
    /* [in] */ IMenu* menu,
    /* [in] */ Int32 titleRes,
    /* [in] */ Int32 actionEnum,
    /* [in] */ Boolean isEnabled,
    /* [in] */ IRunnable* onClick)
{
    AutoPtr<IMenuItem> item;
    menu->Add(titleRes, (IMenuItem**)&item);
    item->SetShowAsAction(actionEnum);
    item->SetEnabled(isEnabled);
    AutoPtr<OnMenuItemClickListener> listener = new OnMenuItemClickListener(onClick);
    item->SetOnMenuItemClickListener(listener);
    return item;
}

ECode CDreamSettings::OnCreateDialog(
    /* [in] */ Int32 dialogId,
    /* [out] */ IDialog** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, CoreUtils::Convert(dialogId));
    Logd(String("OnCreateDialog(%s)"), args);

    if (dialogId == DIALOG_WHEN_TO_DREAM) {
        AutoPtr<IDialog> dialog = CreateWhenToDreamDialog();
        *result = dialog;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    return SettingsPreferenceFragment::OnCreateDialog(dialogId, result);
}

AutoPtr<IDialog> CDreamSettings::CreateWhenToDreamDialog()
{
    AutoPtr< ArrayOf<ICharSequence*> > items = ArrayOf<ICharSequence*>::Alloc(3);
    String str;
    mContext->GetString(R::string::screensaver_settings_summary_dock, &str);
    items->Set(0, CoreUtils::Convert(str));
    mContext->GetString(R::string::screensaver_settings_summary_sleep, &str);
    items->Set(1, CoreUtils::Convert(str));
    mContext->GetString(R::string::screensaver_settings_summary_either_short, &str);
    items->Set(2, CoreUtils::Convert(str));

    Int32 initialSelection = mBackend->IsActivatedOnDock() && mBackend->IsActivatedOnSleep() ? 2
            : mBackend->IsActivatedOnDock() ? 0
            : mBackend->IsActivatedOnSleep() ? 1
            : -1;

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(mContext, (IAlertDialogBuilder**)&builder);
    builder->SetTitle(R::string::screensaver_settings_when_to_dream);
    AutoPtr<IDialogInterfaceOnClickListener> listener = new DialogInterfaceOnClickListener(this);
    builder->SetSingleChoiceItems(items, initialSelection, listener);
    AutoPtr<IAlertDialog> dialog;
    builder->Create((IAlertDialog**)&dialog);
    return IDialog::Probe(dialog);
}

ECode CDreamSettings::OnPause()
{
    Logd(String("OnPause()"), NULL);
    SettingsPreferenceFragment::OnPause();

    mContext->UnregisterReceiver(mPackageReceiver);
    return NOERROR;
}

ECode CDreamSettings::OnResume()
{
    Logd(String("OnResume()"), NULL);
    SettingsPreferenceFragment::OnResume();
    RefreshFromBackend();

    // listen for package changes
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_PACKAGE_ADDED);
    filter->AddAction(IIntent::ACTION_PACKAGE_CHANGED);
    filter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
    filter->AddAction(IIntent::ACTION_PACKAGE_REPLACED);
    filter->AddDataScheme(PACKAGE_SCHEME);
    AutoPtr<IIntent> result;
    mContext->RegisterReceiver(mPackageReceiver, filter, (IIntent**)&result);
    return NOERROR;
}

Int32 CDreamSettings::GetSummaryResource(
    /* [in] */ IContext* context)
{
    AutoPtr<DreamBackend> backend = new DreamBackend(context);
    Boolean isEnabled = backend->IsEnabled();
    Boolean activatedOnSleep = backend->IsActivatedOnSleep();
    Boolean activatedOnDock = backend->IsActivatedOnDock();
    Boolean activatedOnEither = activatedOnSleep && activatedOnDock;
    return !isEnabled ? R::string::screensaver_settings_summary_off
            : activatedOnEither ? R::string::screensaver_settings_summary_either_long
            : activatedOnSleep ? R::string::screensaver_settings_summary_sleep
            : activatedOnDock ? R::string::screensaver_settings_summary_dock
            : 0;
}

AutoPtr<ICharSequence> CDreamSettings::GetSummaryTextWithDreamName(
    /* [in] */ IContext* context)
{
    AutoPtr<DreamBackend> backend = new DreamBackend(context);
    Boolean isEnabled = backend->IsEnabled();
    if (!isEnabled) {
        String str;
        context->GetString(R::string::screensaver_settings_summary_off, &str);
        return CoreUtils::Convert(str);
    }
    else {
        return backend->GetActiveDreamName();
    }
}

void CDreamSettings::RefreshFromBackend()
{
    Logd(String("RefreshFromBackend()"), NULL);
    mRefreshing = TRUE;
    Boolean dreamsEnabled = mBackend->IsEnabled();
    CSwitchBar* switchBar = (CSwitchBar*)mSwitchBar.Get();
    Boolean res;
    switchBar->IsChecked(&res);
    if (res != dreamsEnabled)
        switchBar->SetChecked(dreamsEnabled);

    mAdapter->Clear();
    if (dreamsEnabled) {
        AutoPtr<IList> dreamInfos = mBackend->GetDreamInfos();
        mAdapter->AddAll(ICollection::Probe(dreamInfos));
    }
    if (mMenuItemsWhenEnabled != NULL) {
        for (Int32 i = 0; i < mMenuItemsWhenEnabled->GetLength(); i++) {
            AutoPtr<IMenuItem> menuItem = (*mMenuItemsWhenEnabled)[i];
            menuItem->SetEnabled(dreamsEnabled);
        }
    }
    mRefreshing = FALSE;
}

void CDreamSettings::Logd(
    /* [in] */ const String& msg,
    /* [in] */ ArrayOf<IInterface*>* args)
{
    if (DEBUG) {
        Logger::D(TAG, args == NULL || args->GetLength() == 0 ? msg : StringUtils::Format(msg, args));
    }
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos