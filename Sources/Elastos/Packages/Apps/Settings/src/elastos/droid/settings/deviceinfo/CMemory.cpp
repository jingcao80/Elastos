
#include "elastos/droid/settings/deviceinfo/CMemory.h"
#include "elastos/droid/settings/deviceinfo/CMemoryClearCacheObserver.h"
#include "elastos/droid/settings/deviceinfo/CMemoryConfirmClearCacheFragment.h"
#include "elastos/droid/settings/deviceinfo/CStorageVolumePreferenceCategory.h"
#include "elastos/droid/settings/search/SearchIndexableRaw.h"
#include "elastos/droid/settings/CSettingsActivity.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/os/ServiceManager.h"
#include "../R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Settings::CSettingsActivity;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::Search::SearchIndexableRaw;
using Elastos::Droid::Settings::Search::ISearchIndexableRaw;
using Elastos::Droid::Settings::Search::EIID_IIndexable;

using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::EIID_IIPackageDataObserver;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::Storage::IStorageVolume;
using Elastos::Droid::Os::Storage::IStorageManagerHelper;
using Elastos::Droid::Os::Storage::CStorageManagerHelper;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Deviceinfo {

//===============================================================================
//                  CMemory::MyBaseSearchIndexProvider
//===============================================================================

CMemory::MyBaseSearchIndexProvider::MyBaseSearchIndexProvider()
{}

CMemory::MyBaseSearchIndexProvider::~MyBaseSearchIndexProvider()
{}

ECode CMemory::MyBaseSearchIndexProvider::GetRawDataToIndex(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled,
    /* [out] */ IList** list) //List<SearchIndexableRaw>
{
    VALIDATE_NOT_NULL(list)
    *list = NULL;

    AutoPtr<IList> result;
    CArrayList::New((IList**)&result);

    AutoPtr<SearchIndexableRaw> data = new SearchIndexableRaw();
    data->constructor(context);
    context->GetString(R::string::storage_settings, &data->mTitle);
    context->GetString(R::string::storage_settings, &data->mScreenTitle);
    result->Add((ISearchIndexableRaw*)data);

    data = NULL;
    data = new SearchIndexableRaw();
    data->constructor(context);
    context->GetString(R::string::internal_storage, &data->mTitle);
    context->GetString(R::string::storage_settings, &data->mScreenTitle);
    result->Add((ISearchIndexableRaw*)data);

    data = NULL;
    data = new SearchIndexableRaw();
    data->constructor(context);
    AutoPtr<IStorageManagerHelper> helper;
    CStorageManagerHelper::AcquireSingleton((IStorageManagerHelper**)&helper);
    AutoPtr<IStorageManager> sm;
    helper->From(context, (IStorageManager**)&sm);
    AutoPtr< ArrayOf<IStorageVolume*> > storageVolumes;
    sm->GetVolumeList((ArrayOf<IStorageVolume*>**)&storageVolumes);
    Boolean isEmulated;
    for (Int32 i = 0; i < storageVolumes->GetLength(); i++) {
        AutoPtr<IStorageVolume> volume = (*storageVolumes)[i];
        if (volume->IsEmulated(&isEmulated), !isEmulated) {
            volume->GetDescription(context, &data->mTitle);
            context->GetString(R::string::storage_settings, &data->mScreenTitle);
            result->Add((ISearchIndexableRaw*)data);
        }
    }

    data = NULL;
    data = new SearchIndexableRaw();
    data->constructor(context);
    context->GetString(R::string::memory_size, &data->mTitle);
    context->GetString(R::string::storage_settings, &data->mScreenTitle);
    result->Add((ISearchIndexableRaw*)data);

    data = NULL;
    data = new SearchIndexableRaw();
    data->constructor(context);
    context->GetString(R::string::memory_available, &data->mTitle);
    context->GetString(R::string::storage_settings, &data->mScreenTitle);
    result->Add((ISearchIndexableRaw*)data);

    data = NULL;
    data = new SearchIndexableRaw();
    data->constructor(context);
    context->GetString(R::string::memory_apps_usage, &data->mTitle);
    context->GetString(R::string::storage_settings, &data->mScreenTitle);
    result->Add((ISearchIndexableRaw*)data);

    data = NULL;
    data = new SearchIndexableRaw();
    data->constructor(context);
    context->GetString(R::string::memory_dcim_usage, &data->mTitle);
    context->GetString(R::string::storage_settings, &data->mScreenTitle);
    result->Add((ISearchIndexableRaw*)data);

    data = NULL;
    data = new SearchIndexableRaw();
    data->constructor(context);
    context->GetString(R::string::memory_music_usage, &data->mTitle);
    context->GetString(R::string::storage_settings, &data->mScreenTitle);
    result->Add((ISearchIndexableRaw*)data);

    data = NULL;
    data = new SearchIndexableRaw();
    data->constructor(context);
    context->GetString(R::string::memory_downloads_usage, &data->mTitle);
    context->GetString(R::string::storage_settings, &data->mScreenTitle);
    result->Add((ISearchIndexableRaw*)data);

    data = NULL;
    data = new SearchIndexableRaw();
    data->constructor(context);
    context->GetString(R::string::memory_media_cache_usage, &data->mTitle);
    context->GetString(R::string::storage_settings, &data->mScreenTitle);
    result->Add((ISearchIndexableRaw*)data);

    data = NULL;
    data = new SearchIndexableRaw();
    data->constructor(context);
    context->GetString(R::string::memory_media_misc_usage, &data->mTitle);
    context->GetString(R::string::storage_settings, &data->mScreenTitle);
    result->Add((ISearchIndexableRaw*)data);

    *list = result;
    REFCOUNT_ADD(*list)
    return NOERROR;
}

//===============================================================================
//                  CMemory::ConfirmClearCacheFragment
//===============================================================================

CMemory::ConfirmClearCacheFragment::ConfirmClearCacheFragment()
{}

ECode CMemory::ConfirmClearCacheFragment::constructor()
{
    return DialogFragment::constructor();
}

void CMemory::ConfirmClearCacheFragment::Show(
    /* [in] */ CMemory* parent)
{
    Boolean res;
    if (parent->IsAdded(&res), !res) return;
    AutoPtr<IDialogFragment> dialog;
    CMemoryConfirmClearCacheFragment::New((IDialogFragment**)&dialog);
    IFragment::Probe(dialog)->SetTargetFragment(parent, 0);
    AutoPtr<IFragmentManager> fm;
    parent->GetFragmentManager((IFragmentManager**)&fm);
    dialog->Show(fm, TAG_CONFIRM_CLEAR_CACHE);
}

ECode CMemory::ConfirmClearCacheFragment::OnCreateDialog(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IDialog** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext* context = IContext::Probe(activity);

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(context, (IAlertDialogBuilder**)&builder);
    builder->SetTitle(R::string::memory_clear_cache_title);
    String str;
    GetString(R::string::memory_clear_cache_message, &str);
    builder->SetMessage(CoreUtils::Convert(str));

    AutoPtr<ConfirmClearCacheFragmentDialogInterfaceOnClickListener> listener = new ConfirmClearCacheFragmentDialogInterfaceOnClickListener(this);
    builder->SetPositiveButton(Elastos::Droid::R::string::ok, listener);
    builder->SetNegativeButton(Elastos::Droid::R::string::cancel, NULL);

    AutoPtr<IAlertDialog> dialog;
    builder->Create((IAlertDialog**)&dialog);
    *result = IDialog::Probe(dialog);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//===============================================================================
//                  CMemory::ConfirmClearCacheFragmentDialogInterfaceOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CMemory::ConfirmClearCacheFragmentDialogInterfaceOnClickListener, Object, IDialogInterfaceOnClickListener)

CMemory::ConfirmClearCacheFragmentDialogInterfaceOnClickListener::ConfirmClearCacheFragmentDialogInterfaceOnClickListener(
    /* [in] */ ConfirmClearCacheFragment* host)
    : mHost(host)
{}

ECode CMemory::ConfirmClearCacheFragmentDialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    AutoPtr<IFragment> fragment;
    mHost->GetTargetFragment((IFragment**)&fragment);

    CMemory* target = (CMemory*) ISettingsPreferenceFragment::Probe(fragment);

    AutoPtr<IActivity> activity;
    mHost->GetActivity((IActivity**)&activity);

    AutoPtr<IPackageManager> pm;
    IContext::Probe(activity)->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IList> infos;//List<PackageInfo>
    pm->GetInstalledPackages(0, (IList**)&infos);
    Int32 size;
    infos->GetSize(&size);
    AutoPtr<IIPackageDataObserver> observer;
    CMemoryClearCacheObserver::New((ISettingsPreferenceFragment*)target, size, (IIPackageDataObserver**)&observer);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        infos->Get(i, (IInterface**)&obj);
        IPackageInfo* info = IPackageInfo::Probe(obj);
        String packageName;
        info->GetPackageName(&packageName);
        pm->DeleteApplicationCacheFiles(packageName, observer);
    }
    return NOERROR;
}

//===============================================================================
//                  CMemory::MyStorageEventListener
//===============================================================================

CMemory::MyStorageEventListener::MyStorageEventListener(
    /* [in] */ CMemory* host)
    : mHost(host)
{}

ECode CMemory::MyStorageEventListener::OnStorageStateChanged(
    /* [in] */ const String& path,
    /* [in] */ const String& oldState,
    /* [in] */ const String& newState)
{
    Logger::I(TAG, "Received storage state changed notification that changed state from  to ",
            path.string(), oldState.string(), newState.string());

    Int32 size;
    mHost->mCategories->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mHost->mCategories->Get(i, (IInterface**)&obj);
        CStorageVolumePreferenceCategory* category = (CStorageVolumePreferenceCategory*)IPreferenceCategory::Probe(obj);

        AutoPtr<IStorageVolume> volume = category->GetStorageVolume();
        String str;
        if (volume != NULL && path.Equals((volume->GetPath(&str), str))) {
            category->OnStorageStateChanged();
            break;
        }
    }
    return NOERROR;
}

//===============================================================================
//                  CMemory::MyBroadcastReceiver
//===============================================================================

CMemory::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ CMemory* host)
    : mHost(host)
{}

ECode CMemory::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IUsbManager::ACTION_USB_STATE)) {
        Boolean isUsbConnected;
        intent->GetBooleanExtra(IUsbManager::USB_CONNECTED, FALSE, &isUsbConnected);
        String usbFunction;
        mHost->mUsbManager->GetDefaultFunction(&usbFunction);
        Int32 size;
        mHost->mCategories->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mHost->mCategories->Get(i, (IInterface**)&obj);
            CStorageVolumePreferenceCategory* category = (CStorageVolumePreferenceCategory*)IPreferenceCategory::Probe(obj);
            category->OnUsbStateChanged(isUsbConnected, usbFunction);
        }
    }
    else if (action.Equals(IIntent::ACTION_MEDIA_SCANNER_FINISHED)) {
        Int32 size;
        mHost->mCategories->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mHost->mCategories->Get(i, (IInterface**)&obj);
            CStorageVolumePreferenceCategory* category = (CStorageVolumePreferenceCategory*)IPreferenceCategory::Probe(obj);
            category->OnMediaScannerFinished();
        }
    }
    return NOERROR;
}

//===============================================================================
//                  CMemory::ClearCacheObserver
//===============================================================================

CAR_INTERFACE_IMPL_2(CMemory::ClearCacheObserver, Object, IIPackageDataObserver, IBinder)

CMemory::ClearCacheObserver::ClearCacheObserver()
{}

ECode CMemory::ClearCacheObserver::constructor(
    /* [in] */ ISettingsPreferenceFragment* target,
    /* [in] */ Int32 remaining)
{
    mTarget = (CMemory*)target;
    mRemaining = remaining;
    return NOERROR;
}

CMemory::ClearCacheObserver::~ClearCacheObserver()
{}

ECode CMemory::ClearCacheObserver::OnRemoveCompleted(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean succeeded)
{
    {
        AutoLock syncLock(this);
        if (--mRemaining == 0) {
            mTarget->OnCacheCleared();
        }
    }
    return NOERROR;
}

//===============================================================================
//                  CMemory::OnCreateDialogOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CMemory::OnCreateDialogOnClickListener, Object, IDialogInterfaceOnClickListener)

CMemory::OnCreateDialogOnClickListener::OnCreateDialogOnClickListener(
    /* [in] */ CMemory* host)
    : mHost(host)
{}

ECode CMemory::OnCreateDialogOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    mHost->DoUnmount();
    return NOERROR;
}

//===============================================================================
//                  CMemory
//===============================================================================

const String CMemory::TAG("MemorySettings");

const String CMemory::TAG_CONFIRM_CLEAR_CACHE("confirmClearCache");

const Int32 CMemory::DLG_CONFIRM_UNMOUNT = 1;
const Int32 CMemory::DLG_ERROR_UNMOUNT = 2;

AutoPtr<IPreference> CMemory::sLastClickedMountToggle;
String CMemory::sClickedMountPoint;

AutoPtr<IIndexableSearchIndexProvider> CMemory::SEARCH_INDEX_DATA_PROVIDER;

AutoPtr<IIndexableSearchIndexProvider> CMemory::GetSEARCH_INDEX_DATA_PROVIDER()
{
    if (SEARCH_INDEX_DATA_PROVIDER == NULL) {
        SEARCH_INDEX_DATA_PROVIDER = new CMemory::MyBaseSearchIndexProvider();
    }

    return SEARCH_INDEX_DATA_PROVIDER;
}

CAR_INTERFACE_IMPL(CMemory, SettingsPreferenceFragment, IIndexable)

CAR_OBJECT_IMPL(CMemory)

CMemory::CMemory()
{}

CMemory::~CMemory()
{}

ECode CMemory::constructor()
{
    CArrayList::New((IArrayList**)&mCategories);

    mStorageListener = new MyStorageEventListener(this);
    mMediaScannerReceiver = new MyBroadcastReceiver(this);
    return SettingsPreferenceFragment::constructor();
}

ECode CMemory::OnCreate(
    /* [in] */ IBundle* icicle)
{
    SettingsPreferenceFragment::OnCreate(icicle);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext* context = IContext::Probe(activity);

    AutoPtr<IInterface> obj = GetSystemService(IContext::USB_SERVICE);
    mUsbManager = IUsbManager::Probe(obj);

    AutoPtr<IStorageManagerHelper> helper;
    CStorageManagerHelper::AcquireSingleton((IStorageManagerHelper**)&helper);
    helper->From(context, (IStorageManager**)&mStorageManager);
    mStorageManager->RegisterListener(mStorageListener);

    AddPreferencesFromResource(R::xml::device_info_memory);
    AddCategory(CStorageVolumePreferenceCategory::BuildForInternal(context));

    AutoPtr< ArrayOf<IStorageVolume*> > storageVolumes;
    mStorageManager->GetVolumeList((ArrayOf<IStorageVolume*>**)&storageVolumes);
    Boolean res;
    for (Int32 i = 0; i < storageVolumes->GetLength(); i++) {
        AutoPtr<IStorageVolume> volume = (*storageVolumes)[i];

        if (volume->IsEmulated(&res), !res) {
            AddCategory(CStorageVolumePreferenceCategory::BuildForPhysical(context, volume));
        }
    }

    return SetHasOptionsMenu(TRUE);
}

void CMemory::AddCategory(
    /* [in] */ IPreferenceCategory* category)
{
    mCategories->Add(category);
    AutoPtr<IPreferenceScreen> screen;
    GetPreferenceScreen((IPreferenceScreen**)&screen);
    Boolean res;
    IPreferenceGroup::Probe(screen)->AddPreference(IPreference::Probe(category), &res);
    ((CStorageVolumePreferenceCategory*)category)->Init();
}

Boolean CMemory::IsMassStorageEnabled()
{
    // Mass storage is enabled if primary volume supports it
    AutoPtr< ArrayOf<IStorageVolume*> > volumes;
    mStorageManager->GetVolumeList((ArrayOf<IStorageVolume*>**)&volumes);
    AutoPtr<IStorageManagerHelper> helper;
    CStorageManagerHelper::AcquireSingleton((IStorageManagerHelper**)&helper);
    AutoPtr<IStorageVolume> primary;
    helper->GetPrimaryVolume(volumes, (IStorageVolume**)&primary);
    Boolean res;
    return primary != NULL && (primary->AllowMassStorage(&res), res);
}

ECode CMemory::OnResume()
{
    SettingsPreferenceFragment::OnResume();
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New(IIntent::ACTION_MEDIA_SCANNER_STARTED, (IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IIntent::ACTION_MEDIA_SCANNER_FINISHED);
    intentFilter->AddDataScheme(String("file"));
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext* context = IContext::Probe(activity);
    AutoPtr<IIntent> tmpIntent;
    context->RegisterReceiver(mMediaScannerReceiver, intentFilter, (IIntent**)&tmpIntent);

    intentFilter = NULL;
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IUsbManager::ACTION_USB_STATE);
    tmpIntent = NULL;
    context->RegisterReceiver(mMediaScannerReceiver, intentFilter, (IIntent**)&tmpIntent);

    Int32 size;
    mCategories->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCategories->Get(i, (IInterface**)&obj);
        CStorageVolumePreferenceCategory* category = (CStorageVolumePreferenceCategory*)IPreferenceCategory::Probe(obj);
        category->OnResume();
    }
    return NOERROR;
}

ECode CMemory::OnPause()
{
    SettingsPreferenceFragment::OnPause();
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext::Probe(activity)->UnregisterReceiver(mMediaScannerReceiver);

    Int32 size;
    mCategories->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCategories->Get(i, (IInterface**)&obj);
        CStorageVolumePreferenceCategory* category = (CStorageVolumePreferenceCategory*)IPreferenceCategory::Probe(obj);
        category->OnPause();
    }
    return NOERROR;
}

ECode CMemory::OnDestroy()
{
    if (mStorageManager != NULL && mStorageListener != NULL) {
        mStorageManager->UnregisterListener(mStorageListener);
    }
    SettingsPreferenceFragment::OnDestroy();
    return NOERROR;
}

ECode CMemory::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [in] */ IMenuInflater* inflater)
{
    inflater->Inflate(R::menu::storage, menu);
    return NOERROR;
}

ECode CMemory::OnPrepareOptionsMenu(
    /* [in] */ IMenu* menu)
{
    AutoPtr<IMenuItem> usb;
    menu->FindItem(R::id::storage_usb, (IMenuItem**)&usb);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IInterface> obj;
    IContext::Probe(activity)->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    IUserManager* um = IUserManager::Probe(obj);
    Boolean res;
    Boolean usbItemVisible = IsMassStorageEnabled()
            && (um->HasUserRestriction(IUserManager::DISALLOW_USB_FILE_TRANSFER, &res), !res);
    usb->SetVisible(usbItemVisible);
    return NOERROR;
}

ECode CMemory::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int32 id;
    item->GetItemId(&id);
    switch (id) {
        case R::id::storage_usb: {
            AutoPtr<IActivity> activity;
            GetActivity((IActivity**)&activity);
            if (ISettingsActivity::Probe(activity) != NULL) {
                ((CSettingsActivity*)ISettingsActivity::Probe(activity))->StartPreferencePanel(
                        String("Elastos.Droid.Settings.Deviceinfo.CUsbSettings"),
                        NULL, R::string::storage_title_usb, NULL, this, 0);
            }
            else {
                StartFragment(this, String("Elastos.Droid.Settings.Deviceinfo.CUsbSettings"),
                        R::string::storage_title_usb, -1, NULL);
            }
            *result = TRUE;
            return NOERROR;
        }
    }
    return SettingsPreferenceFragment::OnOptionsItemSelected(item, result);
}

AutoPtr<IIMountService> CMemory::GetMountService()
{
    {
        AutoLock syncLock(this);
        if (mMountService == NULL) {
            AutoPtr<IInterface> service = ServiceManager::GetService(String("mount"));
            if (service != NULL) {
                mMountService = IIMountService::Probe(service);
            }
            else {
                Logger::E(TAG, "Can't get mount service");
            }
        }
        return mMountService;
    }
}

ECode CMemory::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* preferenceScreen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    String key;
    preference->GetKey(&key);
    if (CStorageVolumePreferenceCategory::KEY_CACHE.Equals(key)) {
        ConfirmClearCacheFragment::Show(this);
        *result = TRUE;
        return NOERROR;
    }

    Int32 size;
    mCategories->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCategories->Get(i, (IInterface**)&obj);
        CStorageVolumePreferenceCategory* category = (CStorageVolumePreferenceCategory*)IPreferenceCategory::Probe(obj);

        AutoPtr<IIntent> intent = category->IntentForClick(preference);
        if (intent != NULL) {
            // Don't go across app boundary if monkey is running
            if (!Utils::IsMonkeyRunning()) {
                ECode ec = StartActivity(intent);
                if (FAILED(ec)) { //ActivityNotFoundException
                    Logger::W(TAG, "No activity found for intent %s", TO_CSTR(intent));
                }
            }
            *result = TRUE;
            return NOERROR;
        }

        AutoPtr<IStorageVolume> volume = category->GetStorageVolume();
        if (volume != NULL && category->MountToggleClicked(preference)) {
            sLastClickedMountToggle = preference;
            volume->GetPath(&sClickedMountPoint);
            String path, state;
            volume->GetPath(&path);
            mStorageManager->GetVolumeState(path, &state);
            if (IEnvironment::MEDIA_MOUNTED.Equals(state) ||
                    IEnvironment::MEDIA_MOUNTED_READ_ONLY.Equals(state)) {
                Unmount();
            }
            else {
                Mount();
            }
            *result = TRUE;
            return TRUE;
        }
    }

    return NOERROR;
}

ECode CMemory::OnCreateDialog(
    /* [in] */ Int32 id,
    /* [out] */ IDialog** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    switch (id) {
        case DLG_CONFIRM_UNMOUNT: {
            AutoPtr<IAlertDialogBuilder> builder;
            CAlertDialogBuilder::New(IContext::Probe(activity), (IAlertDialogBuilder**)&builder);
            builder->SetTitle(R::string::dlg_confirm_unmount_title);
            AutoPtr<OnCreateDialogOnClickListener> listener = new OnCreateDialogOnClickListener(this);
            builder->SetPositiveButton(R::string::dlg_ok, listener);
            builder->SetNegativeButton(R::string::cancel, NULL);
            builder->SetMessage(R::string::dlg_confirm_unmount_text);
            AutoPtr<IAlertDialog> dialog;
            builder->Create((IAlertDialog**)&dialog);
            *result = IDialog::Probe(dialog);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        case DLG_ERROR_UNMOUNT: {
            AutoPtr<IAlertDialogBuilder> builder;
            CAlertDialogBuilder::New(IContext::Probe(activity), (IAlertDialogBuilder**)&builder);
            builder->SetTitle(R::string::dlg_error_unmount_title);
            builder->SetNeutralButton(R::string::dlg_ok, NULL);
            builder->SetMessage(R::string::dlg_error_unmount_text);
            AutoPtr<IAlertDialog> dialog;
            builder->Create((IAlertDialog**)&dialog);
            *result = IDialog::Probe(dialog);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }
    return NOERROR;
}

void CMemory::DoUnmount()
{
    // Present a toast here
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IToastHelper> helper;
    CToastHelper::AcquireSingleton((IToastHelper**)&helper);
    AutoPtr<IToast> toast;
    helper->MakeText(IContext::Probe(activity), R::string::unmount_inform_text, IToast::LENGTH_SHORT, (IToast**)&toast);
    toast->Show();
    AutoPtr<IIMountService> mountService = GetMountService();
    sLastClickedMountToggle->SetEnabled(FALSE);
    String str;
    GetString(R::string::sd_ejecting_title, &str);
    sLastClickedMountToggle->SetTitle(CoreUtils::Convert(str));
    GetString(R::string::sd_ejecting_summary, &str);
    sLastClickedMountToggle->SetSummary(CoreUtils::Convert(str));
    ECode ec = mountService->UnmountVolume(sClickedMountPoint, TRUE, FALSE);
    if (FAILED(ec)) { //RemoteException
        // Informative dialog to user that unmount failed.
        ShowDialogInner(DLG_ERROR_UNMOUNT);
    }
}

void CMemory::ShowDialogInner(
    /* [in] */ Int32 id)
{
    RemoveDialog(id);
    ShowDialog(id);
}

Boolean CMemory::HasAppsAccessingStorage()
{
    AutoPtr<IIMountService> mountService = GetMountService();
    AutoPtr< ArrayOf<Int32> > stUsers;
    mountService->GetStorageUsers(sClickedMountPoint, (ArrayOf<Int32>**)&stUsers);
    if (stUsers != NULL && stUsers->GetLength() > 0) {
        return TRUE;
    }
    // TODO FIXME Parameterize with mountPoint and uncomment.
    // On HC-MR2, no apps can be installed on sd and the emulated internal storage is not
    // removable: application cannot interfere with unmount
    /*
    ActivityManager am = (ActivityManager)GetSystemService(IContext::ACTIVITY_SERVICE);
    List<ApplicationInfo> list = am->GetRunningExternalApplications();
    if (list != NULL && list->Size() > 0) {
        return TRUE;
    }
    */
    // Better safe than sorry. Assume the storage is used to ask for confirmation.
    return TRUE;
}

void CMemory::Unmount()
{
    // Check if external media is in use.
    // try {
       if (HasAppsAccessingStorage()) {
           // Present dialog to user
           ShowDialogInner(DLG_CONFIRM_UNMOUNT);
       }
       else {
           DoUnmount();
       }
    // } catch (RemoteException e) {
    //     // Very unlikely. But present an error dialog anyway
    //     Logger::E(TAG, "Is MountService running?");
    //     ShowDialogInner(DLG_ERROR_UNMOUNT);
    // }
}

void CMemory::Mount()
{
    AutoPtr<IIMountService> mountService = GetMountService();
    // try {
    if (mountService != NULL) {
        Int32 data;
        mountService->MountVolume(sClickedMountPoint, &data);
    }
    else {
        Logger::E(TAG, "Mount service is NULL, can't mount");
    }
    // } catch (RemoteException ex) {
    //     // Not much can be done
    // }
}

void CMemory::OnCacheCleared()
{
    Int32 size;
    mCategories->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCategories->Get(i, (IInterface**)&obj);
        CStorageVolumePreferenceCategory* category = (CStorageVolumePreferenceCategory*)IPreferenceCategory::Probe(obj);
        category->OnCacheCleared();
    }
}

} // namespace Deviceinfo
} // namespace Settings
} // namespace Droid
} // namespace Elastos