
#include "usb/CUsbService.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/Manifest.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Content::CIntentFilter;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Usb {

const String CUsbService::TAG("UsbService");

CUsbService::UserChangedReceiver::UserChangedReceiver(
    /* [in] */ CUsbService* host)
    : mHost(host)
{}

ECode CUsbService::UserChangedReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Int32 userId;
    intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, -1, &userId);

    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_USER_SWITCHED)) {
        mHost->SetCurrentUser(userId);
    }
    else if (action.Equals(IIntent::ACTION_USER_STOPPED)) {
        AutoLock lock(mHost->_m_syncLock);
        HashMap<Int32, AutoPtr<UsbSettingsManager> >::Iterator it = mHost->mSettingsByUser.Find(userId);
        if (it != mHost->mSettingsByUser.End()) {
            mHost->mSettingsByUser.Erase(it);
        }
    }

    return NOERROR;
}


CUsbService::CUsbService()
{
    mUserReceiver = (IBroadcastReceiver*)new UserChangedReceiver(this);
}

CUsbService::~CUsbService()
{
    mSettingsByUser.Clear();
}

AutoPtr<UsbSettingsManager> CUsbService::GetSettingsForUser(
    /* [in] */ Int32 userId)
{
    AutoLock lock(this);

    AutoPtr<UsbSettingsManager> settings = mSettingsByUser[userId];
    if (settings == NULL) {
        AutoPtr<IUserHandle> userHandle;
        CUserHandle::New(userId, (IUserHandle**)&userHandle);
        settings = new UsbSettingsManager(mContext, userHandle);
        mSettingsByUser[userId] = settings;
    }
    return settings;
}

ECode CUsbService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;

    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    Boolean hasFeature = FALSE;
    pm->HasSystemFeature(IPackageManager::FEATURE_USB_HOST, &hasFeature);
    if (hasFeature) {
        mHostManager = new UsbHostManager(context);
    }

    AutoPtr<IFile> file;
    CFile::New(String("/sys/class/android_usb"), (IFile**)&file);
    Boolean isExist = FALSE;
    if (file->Exists(&isExist), isExist) {
        mDeviceManager = new UsbDeviceManager(context);
        mDeviceManager->Init(context);
    }

    SetCurrentUser(IUserHandle::USER_OWNER);

    AutoPtr<IIntentFilter> userFilter;
    CIntentFilter::New((IIntentFilter**)&userFilter);
    userFilter->AddAction(String(IIntent::ACTION_USER_SWITCHED));
    userFilter->AddAction(String(IIntent::ACTION_USER_STOPPED));
    AutoPtr<IIntent> stickyIntent;
    mContext->RegisterReceiver(mUserReceiver, userFilter, String(NULL), NULL, (IIntent**)&stickyIntent);

    return NOERROR;
}

void CUsbService::SetCurrentUser(
    /* [in] */ Int32 userId)
{
    AutoPtr<UsbSettingsManager> userSettings = GetSettingsForUser(userId);

    if (mHostManager != NULL) {
        mHostManager->SetCurrentSettings(userSettings);
    }
    if (mDeviceManager != NULL) {
        mDeviceManager->SetCurrentSettings(userSettings);
    }
}

void CUsbService::SystemReady()
{
    if (mDeviceManager != NULL) {
        mDeviceManager->SystemReady();
    }
    if (mHostManager != NULL) {
        mHostManager->SystemReady();
    }
}

ECode CUsbService::GetDeviceList(
    /* [out] */ IBundle** devices)
{
    VALIDATE_NOT_NULL(devices);
    if (mHostManager != NULL) {
        AutoPtr<IBundle> temp = mHostManager->GetDeviceList();
        *devices = temp;
        REFCOUNT_ADD(*devices);
    }
    else {
        CBundle::New(devices);
    }
    return NOERROR;
}

ECode CUsbService::OpenDevice(
    /* [in] */ const String& deviceName,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    VALIDATE_NOT_NULL(descriptor);
    if (mHostManager != NULL) {
        return mHostManager->OpenDevice(deviceName, descriptor);
    }
    *descriptor = NULL;
    return NOERROR;
}

ECode CUsbService::GetCurrentAccessory(
    /* [out] */ IUsbAccessory** accessory)
{
    VALIDATE_NOT_NULL(accessory);
    if (mDeviceManager != NULL) {
        AutoPtr<IUsbAccessory> temp = mDeviceManager->GetCurrentAccessory();
        *accessory = temp;
    }
    else {
        *accessory = NULL;
    }
    REFCOUNT_ADD(*accessory);
    return NOERROR;
}

ECode CUsbService::OpenAccessory(
    /* [in] */ IUsbAccessory* accessory,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    VALIDATE_NOT_NULL(descriptor);
    if (mDeviceManager != NULL) {
        return mDeviceManager->OpenAccessory(accessory, descriptor);
    }
    *descriptor = NULL;
    return NOERROR;
}

ECode CUsbService::SetDevicePackage(
    /* [in] */ IUsbDevice* device,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::MANAGE_USB, String(NULL)));
    GetSettingsForUser(userId)->SetDevicePackage(device, packageName);
    return NOERROR;
}

ECode CUsbService::SetAccessoryPackage(
    /* [in] */ IUsbAccessory* accessory,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::MANAGE_USB, String(NULL)));
    GetSettingsForUser(userId)->SetAccessoryPackage(accessory, packageName);
    return NOERROR;
}

ECode CUsbService::HasDevicePermission(
    /* [in] */ IUsbDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 userId = UserHandle::GetCallingUserId();
    *result = GetSettingsForUser(userId)->HasPermission(device);
    return NOERROR;
}

ECode CUsbService::HasAccessoryPermission(
    /* [in] */ IUsbAccessory* accessory,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 userId = UserHandle::GetCallingUserId();
    *result = GetSettingsForUser(userId)->HasPermission(accessory);
    return NOERROR;
}

ECode CUsbService::RequestDevicePermission(
    /* [in] */ IUsbDevice* device,
    /* [in] */ const String& packageName,
    /* [in] */ IPendingIntent* pi)
{
    Int32 userId = UserHandle::GetCallingUserId();
    GetSettingsForUser(userId)->RequestPermission(device, packageName, pi);
    return NOERROR;
}

ECode CUsbService::RequestAccessoryPermission(
    /* [in] */ IUsbAccessory* accessory,
    /* [in] */ const String& packageName,
    /* [in] */ IPendingIntent* pi)
{
    Int32 userId = UserHandle::GetCallingUserId();
    GetSettingsForUser(userId)->RequestPermission(accessory, packageName, pi);
    return NOERROR;
}

ECode CUsbService::GrantDevicePermission(
    /* [in] */ IUsbDevice* device,
    /* [in] */ Int32 uid)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::MANAGE_USB, String(NULL)));
    Int32 userId = UserHandle::GetCallingUserId();
    GetSettingsForUser(userId)->GrantDevicePermission(device, uid);
    return NOERROR;
}

ECode CUsbService::GrantAccessoryPermission(
    /* [in] */ IUsbAccessory* accessory,
    /* [in] */ Int32 uid)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::MANAGE_USB, String(NULL)));
    Int32 userId = UserHandle::GetCallingUserId();
    GetSettingsForUser(userId)->GrantAccessoryPermission(accessory, uid);
    return NOERROR;
}

ECode CUsbService::HasDefaults(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::MANAGE_USB, String(NULL)));
    *result = GetSettingsForUser(userId)->HasDefaults(packageName);
    return NOERROR;
}

ECode CUsbService::ClearDefaults(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::MANAGE_USB, String(NULL)));
    GetSettingsForUser(userId)->ClearDefaults(packageName);
    return NOERROR;
}

ECode CUsbService::SetCurrentFunction(
    /* [in] */ const String& function,
    /* [in] */ Boolean makeDefault)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::MANAGE_USB, String(NULL)));
    if (mDeviceManager == NULL) {
        // throw new IllegalStateException("USB device mode not supported");
        Slogger::E(TAG, "USB device mode not supported");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mDeviceManager->SetCurrentFunctions(function, makeDefault);
    return NOERROR;
}

ECode CUsbService::SetMassStorageBackingFile(
    /* [in] */ const String& path)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::MANAGE_USB, String(NULL)));
    if (mDeviceManager == NULL) {
        // throw new IllegalStateException("USB device mode not supported");
        Slogger::E(TAG, "USB device mode not supported");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mDeviceManager->SetMassStorageBackingFile(path);
    return NOERROR;
}

ECode CUsbService::AllowUsbDebugging(
    /* [in] */ Boolean alwaysAllow,
    /* [in] */ const String& publicKey)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::MANAGE_USB, String(NULL)));
    mDeviceManager->AllowUsbDebugging(alwaysAllow, publicKey);
    return NOERROR;
}

ECode CUsbService::DenyUsbDebugging()
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::MANAGE_USB, String(NULL)));
    mDeviceManager->DenyUsbDebugging();
    return NOERROR;
}

} // namespace Usb
} // namespace Server
} // namepsace Droid
} // namespace Elastos
