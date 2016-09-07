
#include "Elastos.Droid.AccessibilityService.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/settings/search/DynamicIndexableContentMonitor.h"
#include "elastos/droid/settings/search/Index.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/UserHandle.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::AccessibilityService::IAccessibilityService;
using Elastos::Droid::AccessibilityService::IAccessibilityServiceInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Hardware::Input::EIID_IInputDeviceListener;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Provider::IUserDictionaryWords;
using Elastos::Droid::Provider::CUserDictionaryWords;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::InputMethod::IInputMethodInfo;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Search {

const Int64 DynamicIndexableContentMonitor::DELAY_PROCESS_PACKAGE_CHANGE = 2000;

const Int32 DynamicIndexableContentMonitor::MSG_PACKAGE_AVAILABLE;
const Int32 DynamicIndexableContentMonitor::MSG_PACKAGE_UNAVAILABLE;

//===============================================================================
//                  DynamicIndexableContentMonitor::MyHandler
//===============================================================================

DynamicIndexableContentMonitor::MyHandler::MyHandler(
    /* [in] */ DynamicIndexableContentMonitor* host)
    : mHost(host)
{}

DynamicIndexableContentMonitor::MyHandler::~MyHandler()
{}

ECode DynamicIndexableContentMonitor::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_PACKAGE_AVAILABLE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            String packageName;
            ICharSequence::Probe(obj)->ToString(&packageName);
            mHost->HandlePackageAvailable(packageName);
        } break;

        case MSG_PACKAGE_UNAVAILABLE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            String packageName;
            ICharSequence::Probe(obj)->ToString(&packageName);
            mHost->HandlePackageUnavailable(packageName);
        } break;
    }
    return NOERROR;
}

//===============================================================================
//                DynamicIndexableContentMonitor::UserDictionaryContentObserver
//===============================================================================

ECode DynamicIndexableContentMonitor::UserDictionaryContentObserver::constructor(
    /* [in] */ IHandler* handler,
    /* [in] */ DynamicIndexableContentMonitor* host)
{
    mHost = host;
    return ContentObserver::constructor(handler);
}

DynamicIndexableContentMonitor::UserDictionaryContentObserver::~UserDictionaryContentObserver()
{}

ECode DynamicIndexableContentMonitor::UserDictionaryContentObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    AutoPtr<IUserDictionaryWords> words;
    CUserDictionaryWords::AcquireSingleton((IUserDictionaryWords**)&words);
    AutoPtr<IUri> conUri;
    words->GetCONTENT_URI((IUri**)&conUri);
    Boolean res;
    if (IObject::Probe(conUri)->Equals(uri, &res), res) {
        Index::GetInstance(mHost->mContext)->UpdateFromClassNameResource(
                String("Elastos.Droid.Settings.Inputmethod.CInputMethodAndLanguageSettings"),
                TRUE, TRUE);
    }
    return NOERROR;
};

//===============================================================================
//                  DynamicIndexableContentMonitor
//===============================================================================

CAR_INTERFACE_IMPL(DynamicIndexableContentMonitor, PackageMonitor, IInputDeviceListener);

DynamicIndexableContentMonitor::DynamicIndexableContentMonitor()
    : mHasFeaturePrinting(FALSE)
    , mHasFeatureIme(FALSE)
{
    CArrayList::New((IList**)&mAccessibilityServices);
    CArrayList::New((IList**)&mPrintServices);
    CArrayList::New((IList**)&mImeServices);

    mHandler = new MyHandler(this);
    mHandler->constructor();

    mUserDictionaryContentObserver = new UserDictionaryContentObserver();
    mUserDictionaryContentObserver->constructor(mHandler, this);
}

DynamicIndexableContentMonitor::~DynamicIndexableContentMonitor()
{}

AutoPtr<IIntent> DynamicIndexableContentMonitor::GetAccessibilityServiceIntent(
    /* [in] */ const String& packageName)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IAccessibilityService::SERVICE_INTERFACE, (IIntent**)&intent);
    intent->SetPackage(packageName);
    return intent;
}

AutoPtr<IIntent> DynamicIndexableContentMonitor::GetPrintServiceIntent(
    /* [in] */ const String& packageName)
{
    AutoPtr<IIntent> intent;
    CIntent::New(String("android.printservice.PrintService")/*PrintService.SERVICE_INTERFACE*/,
            (IIntent**)&intent);
    intent->SetPackage(packageName);
    return intent;
}

AutoPtr<IIntent> DynamicIndexableContentMonitor::GetIMEServiceIntent(
    /* [in] */ const String& packageName)
{
    AutoPtr<IIntent> intent;
    CIntent::New(String("android.view.InputMethod"), (IIntent**)&intent);
    intent->SetPackage(packageName);
    return intent;
}

ECode DynamicIndexableContentMonitor::Register(
    /* [in] */ IContext* context)
{
    mContext = context;

    AutoPtr<IPackageManager> manager;
    mContext->GetPackageManager((IPackageManager**)&manager);
    manager->HasSystemFeature(
            IPackageManager::FEATURE_PRINTING, &mHasFeaturePrinting);
    manager->HasSystemFeature(
            IPackageManager::FEATURE_INPUT_METHODS, &mHasFeatureIme);

    // Cache accessibility service packages to know when they go away.
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::ACCESSIBILITY_SERVICE, (IInterface**)&obj);
    IAccessibilityManager* accessibilityManager = IAccessibilityManager::Probe(obj);
    AutoPtr<IList> accessibilityServices;
    accessibilityManager->GetInstalledAccessibilityServiceList((IList**)&accessibilityServices);
    Int32 accessibilityServiceCount;
    accessibilityServices->GetSize(&accessibilityServiceCount);
    for (Int32 i = 0; i < accessibilityServiceCount; i++) {
        AutoPtr<IInterface> object;
        accessibilityServices->Get(i, (IInterface**)&object);
        IAccessibilityServiceInfo* accessibilityService = IAccessibilityServiceInfo::Probe(object);
        AutoPtr<IResolveInfo> resolveInfo;
        accessibilityService->GetResolveInfo((IResolveInfo**)&resolveInfo);
        AutoPtr<IServiceInfo> serviceInfo;
        if (resolveInfo == NULL) {
            continue;
        }
        else {
            resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
            if (serviceInfo == NULL) {
                continue;
            }
        }
        String packageName;
        IPackageItemInfo::Probe(serviceInfo)->GetPackageName(&packageName);
        mAccessibilityServices->Add(CoreUtils::Convert(packageName));
    }

    if (mHasFeaturePrinting) {
        // TODO:
        Slogger::I("DynamicIndexableContentMonitor::Register", "PrintServices is TODO");
        // // Cache print service packages to know when they go away.
        // PrintManager printManager = (PrintManager)
        //         mContext->GetSystemService(Context.PRINT_SERVICE);
        // List<PrintServiceInfo> printServices = printManager->GetInstalledPrintServices();
        // final Int32 serviceCount = printServices->Size();
        // for (Int32 i = 0; i < serviceCount; i++) {
        //     PrintServiceInfo printService = printServices->Get(i);
        //     ResolveInfo resolveInfo = printService->GetResolveInfo();
        //     if (resolveInfo == NULL || resolveInfo.serviceInfo == NULL) {
        //         continue;
        //     }
        //     mPrintServices->Add(resolveInfo.serviceInfo.packageName);
        // }
    }

    // Cache IME service packages to know when they go away.
    if (mHasFeatureIme) {
        AutoPtr<IInterface> obj;
        mContext->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
        IInputMethodManager* imeManager = IInputMethodManager::Probe(obj);
        AutoPtr<IList> inputMethods;
        imeManager->GetInputMethodList((IList**)&inputMethods);
        Int32 inputMethodCount;
        inputMethods->GetSize(&inputMethodCount);
        for (Int32 i = 0; i < inputMethodCount; i++) {
            AutoPtr<IInterface> object;
            inputMethods->Get(i, (IInterface**)&object);
            IInputMethodInfo* inputMethod = IInputMethodInfo::Probe(object);
            AutoPtr<IServiceInfo> serviceInfo;
            inputMethod->GetServiceInfo((IServiceInfo**)&serviceInfo);
            if (serviceInfo == NULL) continue;
            String packageName;
            IPackageItemInfo::Probe(serviceInfo)->GetPackageName(&packageName);
            mImeServices->Add(CoreUtils::Convert(packageName));
        }

        // Watch for related content URIs.
        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);

        AutoPtr<IUserDictionaryWords> words;
        CUserDictionaryWords::AcquireSingleton((IUserDictionaryWords**)&words);
        AutoPtr<IUri> conUri;
        words->GetCONTENT_URI((IUri**)&conUri);

        resolver->RegisterContentObserver(
                conUri, TRUE, mUserDictionaryContentObserver);
    }

    // Watch for input device changes.
    obj = NULL;
    context->GetSystemService(IContext::INPUT_SERVICE, (IInterface**)&obj);
    IInputManager* inputManager = IInputManager::Probe(obj);
    inputManager->RegisterInputDeviceListener(this, mHandler);

    // Start tracking packages.
    PackageMonitor::Register(context, Looper::GetMainLooper(), UserHandle::CURRENT, FALSE);
    return NOERROR;
}

ECode DynamicIndexableContentMonitor::Unregister()
{
    PackageMonitor::Unregister();

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::INPUT_SERVICE, (IInterface**)&obj);
    IInputManager* inputManager = IInputManager::Probe(obj);

    inputManager->UnregisterInputDeviceListener(this);

    if (mHasFeatureIme) {
        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        resolver->UnregisterContentObserver(mUserDictionaryContentObserver);
    }

    mAccessibilityServices->Clear();
    mPrintServices->Clear();
    mImeServices->Clear();
    return NOERROR;
}

ECode DynamicIndexableContentMonitor::OnPackageAppeared(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    PostMessage(MSG_PACKAGE_AVAILABLE, packageName);
    return NOERROR;
}

ECode DynamicIndexableContentMonitor::OnPackageDisappeared(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    PostMessage(MSG_PACKAGE_UNAVAILABLE, packageName);
    return NOERROR;
}

ECode DynamicIndexableContentMonitor::OnPackageModified(
    /* [in] */ const String& packageName)
{
    PackageMonitor::OnPackageModified(packageName);
    AutoPtr<IPackageManager> manager;
    mContext->GetPackageManager((IPackageManager**)&manager);
    Int32 state;
    manager->GetApplicationEnabledSetting(packageName, &state);

    if (state == IPackageManager::COMPONENT_ENABLED_STATE_DEFAULT
            || state ==  IPackageManager::COMPONENT_ENABLED_STATE_ENABLED) {
        PostMessage(MSG_PACKAGE_AVAILABLE, packageName);
    }
    else {
        PostMessage(MSG_PACKAGE_UNAVAILABLE, packageName);
    }
    return NOERROR;
}

ECode DynamicIndexableContentMonitor::OnInputDeviceAdded(
    /* [in] */ Int32 deviceId)
{
    Index::GetInstance(mContext)->UpdateFromClassNameResource(
            String("Elastos.Droid.Settings.Inputmethod.CInputMethodAndLanguageSettings"), FALSE, TRUE);
    return NOERROR;
}

ECode DynamicIndexableContentMonitor::OnInputDeviceRemoved(
    /* [in] */ Int32 deviceId)
{
    return OnInputDeviceChanged(deviceId);
}

ECode DynamicIndexableContentMonitor::OnInputDeviceChanged(
    /* [in] */ Int32 deviceId)
{
    Index::GetInstance(mContext)->UpdateFromClassNameResource(
            String("Elastos.Droid.Settings.Inputmethod.CInputMethodAndLanguageSettings"), TRUE, TRUE);
    return NOERROR;
}

void DynamicIndexableContentMonitor::PostMessage(
    /* [in] */ Int32 what,
    /* [in] */ const String& packageName)
{
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(what, CoreUtils::Convert(packageName), (IMessage**)&message);
    Boolean res;
    mHandler->SendMessageDelayed(message, DELAY_PROCESS_PACKAGE_CHANGE, &res);
}

void DynamicIndexableContentMonitor::HandlePackageAvailable(
    /* [in] */ const String& packageName)
{
    Boolean res;
    if (mAccessibilityServices->Contains(CoreUtils::Convert(packageName), &res), !res) {
        AutoPtr<IIntent> intent = GetAccessibilityServiceIntent(packageName);
        AutoPtr<IPackageManager> manager;
        mContext->GetPackageManager((IPackageManager**)&manager);
        AutoPtr<IList> list;
        manager->QueryIntentServices(intent, 0, (IList**)&list);
        if (list->IsEmpty(&res), !res) {
            mAccessibilityServices->Add(CoreUtils::Convert(packageName));
            Index::GetInstance(mContext)->UpdateFromClassNameResource(
                    String("Elastos.Droid.Settings.Accessibility.AccessibilitySettings"), FALSE, TRUE);
        }
    }

    if (mHasFeaturePrinting) {
        // TODO:
        Slogger::I("DynamicIndexableContentMonitor::HandlePackageAvailable", "PrintServices is TODO");
        // if (!mPrintServices->Contains(packageName)) {
        //     final Intent intent = GetPrintServiceIntent(packageName);
        //     if (!mContext->GetPackageManager()->QueryIntentServices(intent, 0).IsEmpty()) {
        //         mPrintServices->Add(packageName);
        //         Index::GetInstance(mContext)->UpdateFromClassNameResource(
        //                 PrintSettingsFragment.class->GetName(), FALSE, TRUE);
        //     }
        // }
    }

    if (mHasFeatureIme) {
        if (mImeServices->Contains(CoreUtils::Convert(packageName), &res), !res) {
            AutoPtr<IIntent> intent = GetIMEServiceIntent(packageName);
            AutoPtr<IPackageManager> manager;
            mContext->GetPackageManager((IPackageManager**)&manager);
            AutoPtr<IList> list;
            manager->QueryIntentServices(intent, 0, (IList**)&list);
            if (list->IsEmpty(&res), !res) {
                mImeServices->Add(CoreUtils::Convert(packageName));
                Index::GetInstance(mContext)->UpdateFromClassNameResource(
                        String("Elastos.Droid.Settings.Inputmethod.CInputMethodAndLanguageSettings"), FALSE, TRUE);
            }
        }
    }
}

void DynamicIndexableContentMonitor::HandlePackageUnavailable(
    /* [in] */ const String& packageName)
{
    Int32 accessibilityIndex;
    mAccessibilityServices->IndexOf(CoreUtils::Convert(packageName), &accessibilityIndex);
    if (accessibilityIndex >= 0) {
        mAccessibilityServices->Remove(accessibilityIndex);
        Index::GetInstance(mContext)->UpdateFromClassNameResource(
                String("Elastos.Droid.Settings.Accessibility.AccessibilitySettings"), TRUE, TRUE);
    }

    if (mHasFeaturePrinting) {
        // TODO:
        Slogger::I("DynamicIndexableContentMonitor::HandlePackageUnavailable", "PrintServices is TODO");
        // final Int32 printIndex = mPrintServices->IndexOf(packageName);
        // if (printIndex >= 0) {
        //     mPrintServices->Remove(printIndex);
        //     Index::GetInstance(mContext)->UpdateFromClassNameResource(
        //             PrintSettingsFragment.class->GetName(), TRUE, TRUE);
        // }
    }

    if (mHasFeatureIme) {
        Int32 imeIndex;
        mImeServices->IndexOf(CoreUtils::Convert(packageName), &imeIndex);
        if (imeIndex >= 0) {
            mImeServices->Remove(imeIndex);
            Index::GetInstance(mContext)->UpdateFromClassNameResource(
                    String("Elastos.Droid.Settings.Inputmethod.CInputMethodAndLanguageSettings"), TRUE, TRUE);
        }
    }
}

} // namespace Search
} // namespace Settings
} // namespace Droid
} // namespace Elastos
