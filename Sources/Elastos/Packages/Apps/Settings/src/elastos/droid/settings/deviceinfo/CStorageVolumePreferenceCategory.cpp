
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/settings/deviceinfo/CStorageVolumePreferenceCategory.h"
#include "elastos/droid/settings/deviceinfo/CStorageItemPreference.h"
#include "elastos/droid/settings/deviceinfo/CUsageBarPreference.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/text/format/Formatter.h"
#include "../R.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::CActivityThreadHelper;
using Elastos::Droid::App::IActivityThreadHelper;
using Elastos::Droid::App::IDownloadManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Hardware::Usb::IUsbManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::Storage::IStorageManagerHelper;
using Elastos::Droid::Os::Storage::CStorageManagerHelper;
using Elastos::Droid::Preference::CPreference;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Provider::IMediaStoreImagesMedia;
using Elastos::Droid::Provider::CMediaStoreImagesMedia;
using Elastos::Droid::Text::Format::Formatter;
using Elastos::Core::IInteger64;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IInteger64;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Deviceinfo {

//===============================================================================
//                  CStorageVolumePreferenceCategory::PreferenceHeader
//===============================================================================

CStorageVolumePreferenceCategory::PreferenceHeader::PreferenceHeader()
{}

ECode CStorageVolumePreferenceCategory::PreferenceHeader::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 titleRes)
{
    Preference::constructor(context, NULL, Elastos::Droid::R::attr::preferenceCategoryStyle);
    SetTitle(titleRes);
    return NOERROR;
}

ECode CStorageVolumePreferenceCategory::PreferenceHeader::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICharSequence* title)
{
    Preference::constructor(context, NULL, Elastos::Droid::R::attr::preferenceCategoryStyle);
    SetTitle(title);
    return NOERROR;
}

ECode CStorageVolumePreferenceCategory::PreferenceHeader::IsEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

//===============================================================================
//                  CStorageVolumePreferenceCategory::InitUpdateHandler
//===============================================================================

CStorageVolumePreferenceCategory::InitUpdateHandler::InitUpdateHandler(
    /* [in] */ CStorageVolumePreferenceCategory* host)
    : mHost(host)
{}

ECode CStorageVolumePreferenceCategory::InitUpdateHandler::constructor()
{
    return Handler::constructor();
}

ECode CStorageVolumePreferenceCategory::InitUpdateHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_UI_UPDATE_APPROXIMATE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IArrayOf* objArrayOf = IArrayOf::Probe(obj);

            Int32 length = 0;
            objArrayOf->GetLength(&length);

            AutoPtr< ArrayOf<Int64> > size = ArrayOf<Int64>::Alloc(length);

            for (Int32 i = 0; i < length; i++) {
                AutoPtr<IInterface> tmp;
                objArrayOf->Get(i, (IInterface**)&tmp);
                Int64 value;
                IInteger64::Probe(tmp)->GetValue(&value);
                (*size)[i] = value;
            }

            mHost->UpdateApproximate((*size)[0], (*size)[1]);
            break;
        }
        case MSG_UI_UPDATE_DETAILS: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IMeasurementDetails* details = IMeasurementDetails::Probe(obj);
            mHost->UpdateDetails(details);
            break;
        }
    }
    return NOERROR;
}

//===============================================================================
//                  CStorageVolumePreferenceCategory::InitMeasurementReceiver
//===============================================================================

CAR_INTERFACE_IMPL(CStorageVolumePreferenceCategory::InitMeasurementReceiver, Object, IStorageMeasurementReceiver)

CStorageVolumePreferenceCategory::InitMeasurementReceiver::InitMeasurementReceiver(
    /* [in] */ CStorageVolumePreferenceCategory* host)
    : mHost(host)
{}

ECode CStorageVolumePreferenceCategory::InitMeasurementReceiver::UpdateApproximate(
    /* [in] */ IStorageMeasurement* meas,
    /* [in] */ Int64 totalSize,
    /* [in] */ Int64 availSize)
{
    AutoPtr< ArrayOf<Int64> > args = ArrayOf<Int64>::Alloc(2);
    (*args)[0] = totalSize;
    (*args)[1] = availSize;

    AutoPtr<IArrayOf> array;
    CArrayOf::New(EIID_IInteger64, args->GetLength(), (IArrayOf**)&array);
    for (Int32 i = 0; i < args->GetLength(); ++i) {
        array->Set(i, CoreUtils::Convert((*args)[i]));
    }

    AutoPtr<IMessage> message;
    mHost->mUpdateHandler->ObtainMessage(MSG_UI_UPDATE_APPROXIMATE, array, (IMessage**)&message);
    return message->SendToTarget();
}

ECode CStorageVolumePreferenceCategory::InitMeasurementReceiver::UpdateDetails(
    /* [in] */ IStorageMeasurement* meas,
    /* [in] */ IMeasurementDetails* details)
{
    AutoPtr<IMessage> message;
    mHost->mUpdateHandler->ObtainMessage(MSG_UI_UPDATE_DETAILS, details, (IMessage**)&message);
    return message->SendToTarget();
}

//===============================================================================
//                  CStorageVolumePreferenceCategory
//===============================================================================

const String CStorageVolumePreferenceCategory::KEY_CACHE("cache");
const Int32 CStorageVolumePreferenceCategory::ORDER_USAGE_BAR = -2;
const Int32 CStorageVolumePreferenceCategory::ORDER_STORAGE_LOW = -1;

const Int32 CStorageVolumePreferenceCategory::MSG_UI_UPDATE_APPROXIMATE;
const Int32 CStorageVolumePreferenceCategory::MSG_UI_UPDATE_DETAILS;

CAR_OBJECT_IMPL(CStorageVolumePreferenceCategory)

AutoPtr<IPreferenceCategory> CStorageVolumePreferenceCategory::BuildForInternal(
    /* [in] */ IContext* context)
{
    AutoPtr<IPreferenceCategory> pc;
    CStorageVolumePreferenceCategory::New(context, NULL, (IPreferenceCategory**)&pc);
    return pc;
}

AutoPtr<IPreferenceCategory> CStorageVolumePreferenceCategory::BuildForPhysical(
    /* [in] */ IContext* context,
    /* [in] */ IStorageVolume* volume)
{
    AutoPtr<IPreferenceCategory> pc;
    CStorageVolumePreferenceCategory::New(context, volume, (IPreferenceCategory**)&pc);
    return pc;
}

CStorageVolumePreferenceCategory::CStorageVolumePreferenceCategory()
    : mUsbConnected(FALSE)
    , mTotalSize(0)
{}

ECode CStorageVolumePreferenceCategory::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IStorageVolume* volume)
{
    PreferenceCategory::constructor(context);

    CArrayList::New((IList**)&mItemUsers);
    mUpdateHandler = new InitUpdateHandler(this);
    mUpdateHandler->constructor();

    mReceiver = new InitMeasurementReceiver(this);

    mVolume = volume;
    mMeasure = StorageMeasurement::GetInstance(context, volume);

    context->GetResources((IResources**)&mResources);
    AutoPtr<IStorageManagerHelper> helper;
    CStorageManagerHelper::AcquireSingleton((IStorageManagerHelper**)&helper);
    helper->From(context, (IStorageManager**)&mStorageManager);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    mUserManager = IUserManager::Probe(obj);

    String str;
    if (volume != NULL) {
        volume->GetDescription(context, &str);
    }
    else {
        AutoPtr<ICharSequence> cs;
        context->GetText(R::string::internal_storage, (ICharSequence**)&cs);
        cs->ToString(&str);
    }
    return SetTitle(CoreUtils::Convert(str));
}

AutoPtr<IPreference> CStorageVolumePreferenceCategory::BuildItem(
    /* [in] */ Int32 titleRes,
    /* [in] */ Int32 colorRes)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IPreference> pref;
    CStorageItemPreference::New(context, titleRes, colorRes, (IPreference**)&pref);
    return pref;
}

ECode CStorageVolumePreferenceCategory::Init()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);

    RemoveAll();

    AutoPtr<IUserInfo> currentUser;
    ECode ec = ActivityManagerNative::GetDefault()->GetCurrentUser((IUserInfo**)&currentUser);
    if (FAILED(ec)) {//RemoteException
        Logger::E("CStorageVolumePreferenceCategory", "Failed to get current user");
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<IList> otherUsers = GetUsersExcluding(currentUser); //List<UserInfo>
    Int32 size;
    const Boolean showUsers = mVolume == NULL && (otherUsers->GetSize(&size), size > 0);

    CUsageBarPreference::New(context, (IPreference**)&mUsageBarPreference);
    mUsageBarPreference->SetOrder(ORDER_USAGE_BAR);
    Boolean res;
    AddPreference(mUsageBarPreference, &res);

    mItemTotal = BuildItem(R::string::memory_size, 0);
    mItemAvailable = BuildItem(R::string::memory_available, R::color::memory_avail);
    AddPreference(mItemTotal, &res);
    AddPreference(mItemAvailable, &res);

    mItemApps = BuildItem(R::string::memory_apps_usage, R::color::memory_apps_usage);
    mItemDcim = BuildItem(R::string::memory_dcim_usage, R::color::memory_dcim);
    mItemMusic = BuildItem(R::string::memory_music_usage, R::color::memory_music);
    mItemDownloads = BuildItem(R::string::memory_downloads_usage, R::color::memory_downloads);
    mItemCache = BuildItem(R::string::memory_media_cache_usage, R::color::memory_cache);
    mItemMisc = BuildItem(R::string::memory_media_misc_usage, R::color::memory_misc);

    mItemCache->SetKey(KEY_CACHE);

    const Boolean showDetails = mVolume == NULL || (mVolume->IsPrimary(&res), res);
    if (showDetails) {
        if (showUsers) {
            String name;
            currentUser->GetName(&name);
            AutoPtr<PreferenceHeader> pref = new PreferenceHeader();
            pref->constructor(context, CoreUtils::Convert(name));
            AddPreference((IPreference*)pref, &res);
        }

        AddPreference(mItemApps, &res);
        AddPreference(mItemDcim, &res);
        AddPreference(mItemMusic, &res);
        AddPreference(mItemDownloads, &res);
        AddPreference(mItemCache, &res);
        AddPreference(mItemMisc, &res);

        if (showUsers) {
            AutoPtr<PreferenceHeader> pref = new PreferenceHeader();
            pref->constructor(context, R::string::storage_other_users);
            AddPreference((IPreference*)pref, &res);

            Int32 count = 0;
            otherUsers->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                otherUsers->Get(i, (IInterface**)&obj);
                IUserInfo* info = IUserInfo::Probe(obj);

                const Int32 colorRes = count++ % 2 == 0 ? R::color::memory_user_light
                        : R::color::memory_user_dark;

                String name;
                info->GetName(&name);
                Int32 id;
                info->GetId(&id);
                AutoPtr<IPreference> userPref;
                CStorageItemPreference::New(context, CoreUtils::Convert(name), colorRes, id, (IPreference**)&userPref);
                mItemUsers->Add(userPref);
                AddPreference(userPref, &res);
            }
        }
    }

    const Boolean isRemovable = mVolume != NULL ? (mVolume->IsRemovable(&res), res) : FALSE;
    // Always create the preference since many code rely on it existing
    CPreference::New(context, (IPreference**)&mMountTogglePreference);
    if (isRemovable) {
        mMountTogglePreference->SetTitle(R::string::sd_eject);
        mMountTogglePreference->SetSummary(R::string::sd_eject_summary);
        AddPreference(mMountTogglePreference, &res);
    }

    const Boolean allowFormat = mVolume != NULL;
    if (allowFormat) {
        CPreference::New(context, (IPreference**)&mFormatPreference);
        mFormatPreference->SetTitle(R::string::sd_format);
        mFormatPreference->SetSummary(R::string::sd_format_summary);
        AddPreference(mFormatPreference, &res);
    }

    AutoPtr<IActivityThreadHelper> helper;
    CActivityThreadHelper::AcquireSingleton((IActivityThreadHelper**)&helper);
    AutoPtr<IIPackageManager> pm;
    helper->GetPackageManager((IIPackageManager**)&pm);
    // try {
    if (pm->IsStorageLow(&res), res) {
        CPreference::New(context, (IPreference**)&mStorageLow);
        mStorageLow->SetOrder(ORDER_STORAGE_LOW);
        mStorageLow->SetTitle(R::string::storage_low_title);
        mStorageLow->SetSummary(R::string::storage_low_summary);
        AddPreference(mStorageLow, &res);
    }
    else if (mStorageLow != NULL) {
        RemovePreference(mStorageLow, &res);
        mStorageLow = NULL;
    }
    // } catch (RemoteException e) {
    // }
    return NOERROR;
}

AutoPtr<IStorageVolume> CStorageVolumePreferenceCategory::GetStorageVolume()
{
    return mVolume;
}

void CStorageVolumePreferenceCategory::UpdatePreferencesFromState()
{
    // Only update for physical volumes
    if (mVolume == NULL) return;

    mMountTogglePreference->SetEnabled(TRUE);

    String path;
    mVolume->GetPath(&path);
    String state;
    mStorageManager->GetVolumeState(path, &state);

    if (IEnvironment::MEDIA_MOUNTED_READ_ONLY.Equals(state)) {
        mItemAvailable->SetTitle(R::string::memory_available_read_only);
    }
    else {
        mItemAvailable->SetTitle(R::string::memory_available);
    }

    Boolean res;
    String str;
    if (IEnvironment::MEDIA_MOUNTED.Equals(state)
            || IEnvironment::MEDIA_MOUNTED_READ_ONLY.Equals(state)) {
        mMountTogglePreference->SetEnabled(TRUE);
        mResources->GetString(R::string::sd_eject, &str);
        mMountTogglePreference->SetTitle(CoreUtils::Convert(str));
        mResources->GetString(R::string::sd_eject_summary, &str);
        mMountTogglePreference->SetSummary(CoreUtils::Convert(str));
        AddPreference(mUsageBarPreference, &res);
        AddPreference(mItemTotal, &res);
        AddPreference(mItemAvailable, &res);
    }
    else {
        if (IEnvironment::MEDIA_UNMOUNTED.Equals(state) || IEnvironment::MEDIA_NOFS.Equals(state)
                || IEnvironment::MEDIA_UNMOUNTABLE.Equals(state)) {
            mMountTogglePreference->SetEnabled(TRUE);
            mResources->GetString(R::string::sd_mount, &str);
            mMountTogglePreference->SetTitle(CoreUtils::Convert(str));
            mResources->GetString(R::string::sd_mount_summary, &str);
            mMountTogglePreference->SetSummary(CoreUtils::Convert(str));
        }
        else {
            mMountTogglePreference->SetEnabled(FALSE);
            mResources->GetString(R::string::sd_mount, &str);
            mMountTogglePreference->SetTitle(CoreUtils::Convert(str));
            mResources->GetString(R::string::sd_insert_summary, &str);
            mMountTogglePreference->SetSummary(CoreUtils::Convert(str));
        }

        RemovePreference(mUsageBarPreference, &res);
        RemovePreference(mItemTotal, &res);
        RemovePreference(mItemAvailable, &res);
    }

    if (mUsbConnected && (IUsbManager::USB_FUNCTION_MTP.Equals(mUsbFunction) ||
            IUsbManager::USB_FUNCTION_PTP.Equals(mUsbFunction))) {
        mMountTogglePreference->SetEnabled(FALSE);
        if (IEnvironment::MEDIA_MOUNTED.Equals(state)
                || IEnvironment::MEDIA_MOUNTED_READ_ONLY.Equals(state)) {
            mResources->GetString(R::string::mtp_ptp_mode_summary, &str);
            mMountTogglePreference->SetSummary(CoreUtils::Convert(str));
        }

        if (mFormatPreference != NULL) {
            mFormatPreference->SetEnabled(FALSE);
            mResources->GetString(R::string::mtp_ptp_mode_summary, &str);
            mFormatPreference->SetSummary(CoreUtils::Convert(str));
        }
    }
    else if (mFormatPreference != NULL) {
        mMountTogglePreference->IsEnabled(&res);
        mFormatPreference->SetEnabled(res);
        mResources->GetString(R::string::sd_format_summary, &str);
        mFormatPreference->SetSummary(CoreUtils::Convert(str));
    }
}

ECode CStorageVolumePreferenceCategory::UpdateApproximate(
    /* [in] */ Int64 totalSize,
    /* [in] */ Int64 availSize)
{
    mItemTotal->SetSummary(CoreUtils::Convert(FormatSize(totalSize)));
    mItemAvailable->SetSummary(CoreUtils::Convert(FormatSize(availSize)));

    mTotalSize = totalSize;

    const Int64 usedSize = totalSize - availSize;
    CUsageBarPreference* usageBarPreference = (CUsageBarPreference*)mUsageBarPreference.Get();
    usageBarPreference->Clear();
    usageBarPreference->AddEntry(0, usedSize / (Float) totalSize, IColor::GRAY);
    usageBarPreference->Commit();

    UpdatePreferencesFromState();
    return NOERROR;
}

Int64 CStorageVolumePreferenceCategory::TotalValues(
    /* [in] */ IHashMap* map,// HashMap<String, Long> ,
    /* [in] */ ArrayOf<String>* keys)
{
    Int64 total = 0;
    Boolean res;
    for (Int32 i = 0; i < keys->GetLength(); i++) {
        String key = (*keys)[i];

        if (map->ContainsKey(CoreUtils::Convert(key), &res), res) {
            AutoPtr<IInterface> obj;
            map->Get(CoreUtils::Convert(key), (IInterface**)&obj);
            Int64 value;
            IInteger64::Probe(obj)->GetValue(&value);
            total += value;
        }
    }
    return total;
}

ECode CStorageVolumePreferenceCategory::UpdateDetails(
    /* [in] */ IMeasurementDetails* _details)
{
    Boolean res;
    const Boolean showDetails = mVolume == NULL || (mVolume->IsPrimary(&res), res);
    if (!showDetails) return NOERROR;

    AutoPtr<StorageMeasurement::MeasurementDetails> details = (StorageMeasurement::MeasurementDetails*)_details;

    // Count caches as available space, since system manages them
    mItemTotal->SetSummary(CoreUtils::Convert(FormatSize(details->mTotalSize)));
    mItemAvailable->SetSummary(CoreUtils::Convert(FormatSize(details->mAvailSize)));

    CUsageBarPreference* usageBarPreference = (CUsageBarPreference*)mUsageBarPreference.Get();
    usageBarPreference->Clear();

    UpdatePreference(mItemApps, details->mAppsSize);

    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(3);
    (*args)[0] = IEnvironment::DIRECTORY_DCIM;
    (*args)[1] = IEnvironment::DIRECTORY_MOVIES;
    (*args)[2] = IEnvironment::DIRECTORY_PICTURES;
    const Int64 dcimSize = TotalValues(details->mMediaSize, args);
    UpdatePreference(mItemDcim, dcimSize);

    AutoPtr< ArrayOf<String> > args1 = ArrayOf<String>::Alloc(5);
    (*args1)[0] = IEnvironment::DIRECTORY_MUSIC;
    (*args1)[1] = IEnvironment::DIRECTORY_ALARMS;
    (*args1)[2] = IEnvironment::DIRECTORY_NOTIFICATIONS;
    (*args1)[3] = IEnvironment::DIRECTORY_RINGTONES;
    (*args1)[4] = IEnvironment::DIRECTORY_PODCASTS;

    const Int64 musicSize = TotalValues(details->mMediaSize, args1);
    UpdatePreference(mItemMusic, musicSize);

    AutoPtr< ArrayOf<String> > args2 = ArrayOf<String>::Alloc(1);
    (*args2)[0] = IEnvironment::DIRECTORY_DOWNLOADS;

    const Int64 downloadsSize = TotalValues(details->mMediaSize, args2);
    UpdatePreference(mItemDownloads, downloadsSize);

    UpdatePreference(mItemCache, details->mCacheSize);
    UpdatePreference(mItemMisc, details->mMiscSize);

    Int32 size;
    mItemUsers->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mItemUsers->Get(i, (IInterface**)&obj);
        IPreference* userPref = IPreference::Probe(obj);

        Int64 userSize;
        details->mUsersSize->Get(((CStorageItemPreference*)userPref)->mUserHandle, &userSize);
        UpdatePreference(userPref, userSize);
    }

    return usageBarPreference->Commit();
}

void CStorageVolumePreferenceCategory::UpdatePreference(
    /* [in] */ IPreference* pref, //StorageItemPreference
    /* [in] */ Int64 size)
{
    if (size > 0) {
        pref->SetSummary(CoreUtils::Convert(FormatSize(size)));
        Int32 order;
        pref->GetOrder(&order);
        ((CUsageBarPreference*)mUsageBarPreference.Get())->AddEntry(order,
                size / (Float) mTotalSize, ((CStorageItemPreference*)pref)->mColor);
    }
    else {
        Boolean res;
        RemovePreference(pref, &res);
    }
}

void CStorageVolumePreferenceCategory::Measure()
{
    mMeasure->Invalidate();
    mMeasure->Measure();
}

ECode CStorageVolumePreferenceCategory::OnResume()
{
    mMeasure->SetReceiver(mReceiver);
    Measure();
    return NOERROR;
}

ECode CStorageVolumePreferenceCategory::OnStorageStateChanged()
{
    Init();
    Measure();
    return NOERROR;
}

ECode CStorageVolumePreferenceCategory::OnUsbStateChanged(
    /* [in] */ Boolean isUsbConnected,
    /* [in] */ const String& usbFunction)
{
    mUsbConnected = isUsbConnected;
    mUsbFunction = usbFunction;
    Measure();
    return NOERROR;
}

ECode CStorageVolumePreferenceCategory::OnMediaScannerFinished()
{
    Measure();
    return NOERROR;
}

ECode CStorageVolumePreferenceCategory::OnCacheCleared()
{
    Measure();
    return NOERROR;
}

ECode CStorageVolumePreferenceCategory::OnPause()
{
    return mMeasure->CleanUp();
}

String CStorageVolumePreferenceCategory::FormatSize(
    /* [in] */ Int64 size)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    return Formatter::FormatFileSize(context, size);
}

Boolean CStorageVolumePreferenceCategory::MountToggleClicked(
    /* [in] */ IPreference* preference)
{
    return preference == mMountTogglePreference.Get();
}

AutoPtr<IIntent> CStorageVolumePreferenceCategory::IntentForClick(
    /* [in] */ IPreference* pref)
{
    AutoPtr<IIntent> intent;

    // TODO The current "delete" story is not fully handled by the respective applications.
    // When it is done, make sure the intent types below are correct.
    // If that cannot be done, remove these intents.
    String key;
    pref->GetKey(&key);
    if (pref == mFormatPreference.Get()) {
        CIntent::New(IIntent::ACTION_VIEW, (IIntent**)&intent);
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        intent->SetClass(context, Elastos::Droid::Settings::ECLSID_CMediaFormat);
        intent->PutExtra(IStorageVolume::EXTRA_STORAGE_VOLUME, IParcelable::Probe(mVolume));
    }
    else if (pref == mItemApps.Get()) {
        CIntent::New(IIntent::ACTION_MANAGE_PACKAGE_STORAGE, (IIntent**)&intent);
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        intent->SetClass(context, ECLSID_CSettingsManageApplicationsActivity);
    }
    else if (pref == mItemDownloads.Get()) {
        CIntent::New(IDownloadManager::ACTION_VIEW_DOWNLOADS, (IIntent**)&intent);
        intent->PutBooleanExtra(IDownloadManager::INTENT_EXTRAS_SORT_BY_SIZE, TRUE);
    }
    else if (pref == mItemMusic.Get()) {
        CIntent::New(IIntent::ACTION_GET_CONTENT, (IIntent**)&intent);
        intent->SetType(String("audio/mp3"));
    }
    else if (pref == mItemDcim.Get()) {
        CIntent::New(IIntent::ACTION_VIEW, (IIntent**)&intent);
        intent->PutBooleanExtra(IIntent::EXTRA_LOCAL_ONLY, TRUE);
        // TODO Create a Videos category, MediaStore.Video.Media.EXTERNAL_CONTENT_URI
        AutoPtr<IMediaStoreImagesMedia> media;
        CMediaStoreImagesMedia::AcquireSingleton((IMediaStoreImagesMedia**)&media);
        AutoPtr<IUri> uri;
        media->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
        intent->SetData(uri);
    }
    else if (pref == mItemMisc.Get()) {
        AutoPtr<IContext> tmp;
        GetContext((IContext**)&tmp);
        AutoPtr<IContext> context;
        tmp->GetApplicationContext((IContext**)&context);
        CIntent::New(context, ECLSID_CMiscFilesHandler, (IIntent**)&intent);
        intent->PutExtra(IStorageVolume::EXTRA_STORAGE_VOLUME, IParcelable::Probe(mVolume));
    }

    return intent;
}

AutoPtr<IList> CStorageVolumePreferenceCategory::GetUsersExcluding(
    /* [in] */ IUserInfo* excluding)
{
    AutoPtr<IList> users; //List<UserInfo>
    mUserManager->GetUsers((IList**)&users);
    AutoPtr<IIterator> i;
    users->GetIterator((IIterator**)&i);
    Boolean hasNext;
    while (i->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        i->GetNext((IInterface**)&obj);
        Int32 id, otherId;
        IUserInfo::Probe(obj)->GetId(&id);
        excluding->GetId(&otherId);
        if (id == otherId) {
            i->Remove();
        }
    }
    return users;
}

} // namespace Deviceinfo
} // namespace Settings
} // namespace Droid
} // namespace Elastos