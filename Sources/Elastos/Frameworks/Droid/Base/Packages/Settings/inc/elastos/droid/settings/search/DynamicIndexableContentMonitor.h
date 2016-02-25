

package com.android.settings.search;

using Elastos::Droid::Accessibilityservice::IAccessibilityService;
using Elastos::Droid::Accessibilityservice::IAccessibilityServiceInfo;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Print::IPrintManager;
using Elastos::Droid::Printservice::IPrintService;
using Elastos::Droid::Printservice::IPrintServiceInfo;
using Elastos::Droid::Provider::IUserDictionary;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Inputmethod::IInputMethodInfo;
using Elastos::Droid::View::Inputmethod::IInputMethodManager;
using Elastos::Droid::Internal::Content::IPackageMonitor;
using Elastos::Droid::Settings::Accessibility::IAccessibilitySettings;
using Elastos::Droid::Settings::Inputmethod::IInputMethodAndLanguageSettings;
using Elastos::Droid::Settings::Print::IPrintSettingsFragment;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

public class DynamicIndexableContentMonitor extends PackageMonitor implements
        InputManager.InputDeviceListener {

    private static const Int64 DELAY_PROCESS_PACKAGE_CHANGE = 2000;

    private static const Int32 MSG_PACKAGE_AVAILABLE = 1;
    private static const Int32 MSG_PACKAGE_UNAVAILABLE = 2;

    private final List<String> mAccessibilityServices = new ArrayList<String>();
    private final List<String> mPrintServices = new ArrayList<String>();
    private final List<String> mImeServices = new ArrayList<String>();

    private final Handler mHandler = new Handler() {
        //@Override
        CARAPI HandleMessage(Message msg) {
            switch (msg.what) {
                case MSG_PACKAGE_AVAILABLE: {
                    String packageName = (String) msg.obj;
                    HandlePackageAvailable(packageName);
                } break;

                case MSG_PACKAGE_UNAVAILABLE: {
                    String packageName = (String) msg.obj;
                    HandlePackageUnavailable(packageName);
                } break;
            }
        }
    };

    private final ContentObserver mUserDictionaryContentObserver =
            new UserDictionaryContentObserver(mHandler);

    private Context mContext;
    private Boolean mHasFeaturePrinting;
    private Boolean mHasFeatureIme;

    private static Intent GetAccessibilityServiceIntent(String packageName) {
        final Intent intent = new Intent(AccessibilityService.SERVICE_INTERFACE);
        intent->SetPackage(packageName);
        return intent;
    }

    private static Intent GetPrintServiceIntent(String packageName) {
        final Intent intent = new Intent(PrintService.SERVICE_INTERFACE);
        intent->SetPackage(packageName);
        return intent;
    }

    private static Intent GetIMEServiceIntent(String packageName) {
        final Intent intent = new Intent("android.view.InputMethod");
        intent->SetPackage(packageName);
        return intent;
    }

    CARAPI Register(Context context) {
        mContext = context;

        mHasFeaturePrinting = mContext->GetPackageManager()->HasSystemFeature(
                PackageManager.FEATURE_PRINTING);
        mHasFeatureIme = mContext->GetPackageManager()->HasSystemFeature(
                PackageManager.FEATURE_INPUT_METHODS);

        // Cache accessibility service packages to know when they go away.
        AccessibilityManager accessibilityManager = (AccessibilityManager)
                mContext->GetSystemService(Context.ACCESSIBILITY_SERVICE);
        List<AccessibilityServiceInfo> accessibilityServices = accessibilityManager
                .GetInstalledAccessibilityServiceList();
        final Int32 accessibilityServiceCount = accessibilityServices->Size();
        for (Int32 i = 0; i < accessibilityServiceCount; i++) {
            AccessibilityServiceInfo accessibilityService = accessibilityServices->Get(i);
            ResolveInfo resolveInfo = accessibilityService->GetResolveInfo();
            if (resolveInfo == NULL || resolveInfo.serviceInfo == NULL) {
                continue;
            }
            mAccessibilityServices->Add(resolveInfo.serviceInfo.packageName);
        }

        if (mHasFeaturePrinting) {
            // Cache print service packages to know when they go away.
            PrintManager printManager = (PrintManager)
                    mContext->GetSystemService(Context.PRINT_SERVICE);
            List<PrintServiceInfo> printServices = printManager->GetInstalledPrintServices();
            final Int32 serviceCount = printServices->Size();
            for (Int32 i = 0; i < serviceCount; i++) {
                PrintServiceInfo printService = printServices->Get(i);
                ResolveInfo resolveInfo = printService->GetResolveInfo();
                if (resolveInfo == NULL || resolveInfo.serviceInfo == NULL) {
                    continue;
                }
                mPrintServices->Add(resolveInfo.serviceInfo.packageName);
            }
        }

        // Cache IME service packages to know when they go away.
        if (mHasFeatureIme) {
            InputMethodManager imeManager = (InputMethodManager)
                    mContext->GetSystemService(Context.INPUT_METHOD_SERVICE);
            List<InputMethodInfo> inputMethods = imeManager->GetInputMethodList();
            final Int32 inputMethodCount = inputMethods->Size();
            for (Int32 i = 0; i < inputMethodCount; i++) {
                InputMethodInfo inputMethod = inputMethods->Get(i);
                ServiceInfo serviceInfo = inputMethod->GetServiceInfo();
                if (serviceInfo == NULL) continue;
                mImeServices->Add(serviceInfo.packageName);
            }

            // Watch for related content URIs.
            mContext->GetContentResolver()->RegisterContentObserver(
                    UserDictionary.Words.CONTENT_URI, TRUE, mUserDictionaryContentObserver);
        }

        // Watch for input device changes.
        InputManager inputManager = (InputManager) context->GetSystemService(
                Context.INPUT_SERVICE);
        inputManager->RegisterInputDeviceListener(this, mHandler);

        // Start tracking packages.
        Register(context, Looper->GetMainLooper(), UserHandle.CURRENT, FALSE);
    }

    CARAPI Unregister() {
        super->Unregister();

        InputManager inputManager = (InputManager) mContext->GetSystemService(
                Context.INPUT_SERVICE);
        inputManager->UnregisterInputDeviceListener(this);

        if (mHasFeatureIme) {
            mContext->GetContentResolver()->UnregisterContentObserver(
                    mUserDictionaryContentObserver);
        }

        mAccessibilityServices->Clear();
        mPrintServices->Clear();
        mImeServices->Clear();
    }

    // Covers installed, appeared external storage with the package, upgraded.
    //@Override
    CARAPI OnPackageAppeared(String packageName, Int32 uid) {
        PostMessage(MSG_PACKAGE_AVAILABLE, packageName);
    }

    // Covers uninstalled, removed external storage with the package.
    //@Override
    CARAPI OnPackageDisappeared(String packageName, Int32 uid) {
        PostMessage(MSG_PACKAGE_UNAVAILABLE, packageName);
    }

    // Covers enabled, disabled.
    //@Override
    CARAPI OnPackageModified(String packageName) {
        super->OnPackageModified(packageName);
        final Int32 state = mContext->GetPackageManager()->GetApplicationEnabledSetting(
                packageName);
        if (state == PackageManager.COMPONENT_ENABLED_STATE_DEFAULT
                || state ==  PackageManager.COMPONENT_ENABLED_STATE_ENABLED) {
            PostMessage(MSG_PACKAGE_AVAILABLE, packageName);
        } else {
            PostMessage(MSG_PACKAGE_UNAVAILABLE, packageName);
        }
    }

    //@Override
    CARAPI OnInputDeviceAdded(Int32 deviceId) {
        Index->GetInstance(mContext).UpdateFromClassNameResource(
                InputMethodAndLanguageSettings.class->GetName(), FALSE, TRUE);
    }

    //@Override
    CARAPI OnInputDeviceRemoved(Int32 deviceId) {
        OnInputDeviceChanged(deviceId);
    }

    //@Override
    CARAPI OnInputDeviceChanged(Int32 deviceId) {
        Index->GetInstance(mContext).UpdateFromClassNameResource(
                InputMethodAndLanguageSettings.class->GetName(), TRUE, TRUE);
    }

    private void PostMessage(Int32 what, String packageName) {
        Message message = mHandler->ObtainMessage(what, packageName);
        mHandler->SendMessageDelayed(message, DELAY_PROCESS_PACKAGE_CHANGE);
    }

    private void HandlePackageAvailable(String packageName) {
        if (!mAccessibilityServices->Contains(packageName)) {
            final Intent intent = GetAccessibilityServiceIntent(packageName);
            if (!mContext->GetPackageManager()->QueryIntentServices(intent, 0).IsEmpty()) {
                mAccessibilityServices->Add(packageName);
                Index->GetInstance(mContext).UpdateFromClassNameResource(
                        AccessibilitySettings.class->GetName(), FALSE, TRUE);
            }
        }

        if (mHasFeaturePrinting) {
            if (!mPrintServices->Contains(packageName)) {
                final Intent intent = GetPrintServiceIntent(packageName);
                if (!mContext->GetPackageManager()->QueryIntentServices(intent, 0).IsEmpty()) {
                    mPrintServices->Add(packageName);
                    Index->GetInstance(mContext).UpdateFromClassNameResource(
                            PrintSettingsFragment.class->GetName(), FALSE, TRUE);
                }
            }
        }

        if (mHasFeatureIme) {
            if (!mImeServices->Contains(packageName)) {
                Intent intent = GetIMEServiceIntent(packageName);
                if (!mContext->GetPackageManager()->QueryIntentServices(intent, 0).IsEmpty()) {
                    mImeServices->Add(packageName);
                    Index->GetInstance(mContext).UpdateFromClassNameResource(
                            InputMethodAndLanguageSettings.class->GetName(), FALSE, TRUE);
                }
            }
        }
    }

    private void HandlePackageUnavailable(String packageName) {
        final Int32 accessibilityIndex = mAccessibilityServices->IndexOf(packageName);
        if (accessibilityIndex >= 0) {
            mAccessibilityServices->Remove(accessibilityIndex);
            Index->GetInstance(mContext).UpdateFromClassNameResource(
                    AccessibilitySettings.class->GetName(), TRUE, TRUE);
        }

        if (mHasFeaturePrinting) {
            final Int32 printIndex = mPrintServices->IndexOf(packageName);
            if (printIndex >= 0) {
                mPrintServices->Remove(printIndex);
                Index->GetInstance(mContext).UpdateFromClassNameResource(
                        PrintSettingsFragment.class->GetName(), TRUE, TRUE);
            }
        }

        if (mHasFeatureIme) {
            final Int32 imeIndex = mImeServices->IndexOf(packageName);
            if (imeIndex >= 0) {
                mImeServices->Remove(imeIndex);
                Index->GetInstance(mContext).UpdateFromClassNameResource(
                        InputMethodAndLanguageSettings.class->GetName(), TRUE, TRUE);
            }
        }
    }

    private final class UserDictionaryContentObserver extends ContentObserver {

        public UserDictionaryContentObserver(Handler handler) {
            Super(handler);
        }

        //@Override
        CARAPI OnChange(Boolean selfChange, Uri uri) {
            if (UserDictionary.Words.CONTENT_URI->Equals(uri)) {
                Index->GetInstance(mContext).UpdateFromClassNameResource(
                        InputMethodAndLanguageSettings.class->GetName(), TRUE, TRUE);
            }
        };
    }
}
