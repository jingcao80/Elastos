/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony;

using Elastos::Droid::Manifest::Ipermission;
using Elastos::Droid::App::IAppGlobals;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IDebug;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::Telephony::Sms::IIntents;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::ISmsManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Utility::ILog;

using Elastos::Droid::Internal::IR;
using Elastos::Droid::Internal::Content::IPackageMonitor;

using Elastos::Utility::IArrays;
using Elastos::Utility::ICollection;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;

/**
 * Class for managing the primary application that we will deliver SMS/MMS messages to
 *
 * {@hide}
 */
public class SmsApplication {
    static const String LOG_TAG = "SmsApplication";
    private static const String PHONE_PACKAGE_NAME = "com.android.phone";
    private static const String BLUETOOTH_PACKAGE_NAME = "com.android.bluetooth";
    private static const String MMS_SERVICE_PACKAGE_NAME = "com.android.mms.service";

    private static const String SCHEME_SMS = "sms";
    private static const String SCHEME_SMSTO = "smsto";
    private static const String SCHEME_MMS = "mms";
    private static const String SCHEME_MMSTO = "mmsto";
    private static const Boolean DEBUG_MULTIUSER = FALSE;

    private static SmsPackageMonitor sSmsPackageMonitor = NULL;

    public static class SmsApplicationData {
        /**
         * Name of this SMS app for display.
         */
        public String mApplicationName;

        /**
         * Package name for this SMS app.
         */
        public String mPackageName;

        /**
         * The class name of the SMS_DELIVER_ACTION receiver in this app.
         */
        public String mSmsReceiverClass;

        /**
         * The class name of the WAP_PUSH_DELIVER_ACTION receiver in this app.
         */
        public String mMmsReceiverClass;

        /**
         * The class name of the ACTION_RESPOND_VIA_MESSAGE intent in this app.
         */
        public String mRespondViaMessageClass;

        /**
         * The class name of the ACTION_SENDTO intent in this app.
         */
        public String mSendToClass;

        /**
         * The user-id for this application
         */
        public Int32 mUid;

        /**
         * Returns TRUE if this SmsApplicationData is Complete (all intents handled).
         * @return
         */
        public Boolean IsComplete() {
            Return (mSmsReceiverClass != NULL && mMmsReceiverClass != NULL
                    && mRespondViaMessageClass != NULL && mSendToClass != NULL);
        }

        public SmsApplicationData(String applicationName, String packageName, Int32 uid) {
            mApplicationName = applicationName;
            mPackageName = packageName;
            mUid = uid;
        }
    }

    /**
     * Returns the userId of the Context object, if called from a system app,
     * otherwise it returns the caller's userId
     * @param context The context object passed in by the caller.
     * @return
     */
    private static Int32 GetIncomingUserId(Context context) {
        Int32 contextUserId = context->GetUserId();
        final Int32 callingUid = Binder->GetCallingUid();
        If (DEBUG_MULTIUSER) {
            Logger::I(LOG_TAG, "getIncomingUserHandle caller=" + callingUid + ", myuid="
                    + android.os.Process->MyUid() + "\n\t" + Debug->GetCallers(4));
        }
        If (UserHandle->GetAppId(callingUid)
                < android.os.Process.FIRST_APPLICATION_UID) {
            return contextUserId;
        } else {
            return UserHandle->GetUserId(callingUid);
        }
    }

    /**
     * Returns the list of available SMS apps defined as apps that are registered for both the
     * SMS_RECEIVED_ACTION (SMS) and WAP_PUSH_RECEIVED_ACTION (MMS) Broadcasts (and their broadcast
     * receivers are enabled)
     *
     * Requirements to be an SMS application:
     * Implement SMS_DELIVER_ACTION broadcast receiver.
     * Require BROADCAST_SMS permission.
     *
     * Implement WAP_PUSH_DELIVER_ACTION broadcast receiver.
     * Require BROADCAST_WAP_PUSH permission.
     *
     * Implement RESPOND_VIA_MESSAGE intent.
     * Support smsto Uri scheme.
     * Require SEND_RESPOND_VIA_MESSAGE permission.
     *
     * Implement ACTION_SENDTO intent.
     * Support smsto Uri scheme.
     */
    public static Collection<SmsApplicationData> GetApplicationCollection(Context context) {
        Int32 userId = GetIncomingUserId(context);
        final Int64 token = Binder->ClearCallingIdentity();
        try {
            return GetApplicationCollectionInternal(context, userId);
        } finally {
            Binder->RestoreCallingIdentity(token);
        }
    }

    private static Collection<SmsApplicationData> GetApplicationCollectionInternal(
            Context context, Int32 userId) {
        PackageManager packageManager = context->GetPackageManager();

        // Get the list of apps registered for SMS
        Intent intent = new Intent(Intents.SMS_DELIVER_ACTION);
        List<ResolveInfo> smsReceivers = packageManager->QueryBroadcastReceivers(intent, 0,
                userId);

        HashMap<String, SmsApplicationData> receivers = new HashMap<String, SmsApplicationData>();

        // Add one entry to the map for every sms Receiver (ignoring duplicate sms receivers)
        For (ResolveInfo resolveInfo : smsReceivers) {
            final ActivityInfo activityInfo = resolveInfo.activityInfo;
            If (activityInfo == NULL) {
                continue;
            }
            If (!permission.BROADCAST_SMS->Equals(activityInfo.permission)) {
                continue;
            }
            final String packageName = activityInfo.packageName;
            If (!receivers->ContainsKey(packageName)) {
                final String applicationName = resolveInfo->LoadLabel(packageManager).ToString();
                final SmsApplicationData smsApplicationData = new SmsApplicationData(
                        applicationName, packageName, activityInfo.applicationInfo.uid);
                smsApplicationData.mSmsReceiverClass = activityInfo.name;
                receivers->Put(packageName, smsApplicationData);
            }
        }

        // Update any existing entries with mms receiver class
        intent = new Intent(Intents.WAP_PUSH_DELIVER_ACTION);
        intent->SetDataAndType(NULL, "application/vnd.wap.mms-message");
        List<ResolveInfo> mmsReceivers = packageManager->QueryBroadcastReceivers(intent, 0,
                userId);
        For (ResolveInfo resolveInfo : mmsReceivers) {
            final ActivityInfo activityInfo = resolveInfo.activityInfo;
            If (activityInfo == NULL) {
                continue;
            }
            If (!permission.BROADCAST_WAP_PUSH->Equals(activityInfo.permission)) {
                continue;
            }
            final String packageName = activityInfo.packageName;
            final SmsApplicationData smsApplicationData = receivers->Get(packageName);
            If (smsApplicationData != NULL) {
                smsApplicationData.mMmsReceiverClass = activityInfo.name;
            }
        }

        // Update any existing entries with respond via message intent class.
        intent = new Intent(TelephonyManager.ACTION_RESPOND_VIA_MESSAGE,
                Uri->FromParts(SCHEME_SMSTO, "", NULL));
        List<ResolveInfo> respondServices = packageManager->QueryIntentServicesAsUser(intent, 0,
                userId);
        For (ResolveInfo resolveInfo : respondServices) {
            final ServiceInfo serviceInfo = resolveInfo.serviceInfo;
            If (serviceInfo == NULL) {
                continue;
            }
            If (!permission.SEND_RESPOND_VIA_MESSAGE->Equals(serviceInfo.permission)) {
                continue;
            }
            final String packageName = serviceInfo.packageName;
            final SmsApplicationData smsApplicationData = receivers->Get(packageName);
            If (smsApplicationData != NULL) {
                smsApplicationData.mRespondViaMessageClass = serviceInfo.name;
            }
        }

        // Update any existing entries with supports send to.
        intent = new Intent(IIntent::ACTION_SENDTO,
                Uri->FromParts(SCHEME_SMSTO, "", NULL));
        List<ResolveInfo> sendToActivities = packageManager->QueryIntentActivitiesAsUser(intent, 0,
                userId);
        For (ResolveInfo resolveInfo : sendToActivities) {
            final ActivityInfo activityInfo = resolveInfo.activityInfo;
            If (activityInfo == NULL) {
                continue;
            }
            final String packageName = activityInfo.packageName;
            final SmsApplicationData smsApplicationData = receivers->Get(packageName);
            If (smsApplicationData != NULL) {
                smsApplicationData.mSendToClass = activityInfo.name;
            }
        }

        // Remove any entries for which we did not find all required intents.
        For (ResolveInfo resolveInfo : smsReceivers) {
            final ActivityInfo activityInfo = resolveInfo.activityInfo;
            If (activityInfo == NULL) {
                continue;
            }
            final String packageName = activityInfo.packageName;
            final SmsApplicationData smsApplicationData = receivers->Get(packageName);
            If (smsApplicationData != NULL) {
                If (!smsApplicationData->IsComplete()) {
                    receivers->Remove(packageName);
                }
            }
        }
        return receivers->Values();
    }

    /**
     * Checks to see if we have a valid installed SMS application for the specified package name
     * @return Data for the specified package name or NULL if there isn't one
     */
    private static SmsApplicationData GetApplicationForPackage(
            Collection<SmsApplicationData> applications, String packageName) {
        If (packageName == NULL) {
            return NULL;
        }
        // Is there an entry in the application list for the specified package?
        For (SmsApplicationData application : applications) {
            If (application.mPackageName->ContentEquals(packageName)) {
                return application;
            }
        }
        return NULL;
    }

    /**
     * Get the application we will use for delivering SMS/MMS messages.
     *
     * We return the preferred sms application with the following order of preference:
     * (1) User selected SMS App (if selected, and if still valid)
     * (2) Android Messaging (if installed)
     * (3) The currently configured highest priority broadcast receiver
     * (4) Null
     */
    private static SmsApplicationData GetApplication(Context context, Boolean updateIfNeeded,
            Int32 userId) {
        TelephonyManager tm = (TelephonyManager)
                context->GetSystemService(Context.TELEPHONY_SERVICE);
        If (!tm->IsSmsCapable()) {
            // No phone, no SMS
            return NULL;
        }

        Collection<SmsApplicationData> applications = GetApplicationCollectionInternal(context,
                userId);
        If (DEBUG_MULTIUSER) {
            Logger::I(LOG_TAG, "getApplication userId=" + userId);
        }
        // Determine which application receives the broadcast
        String defaultApplication = Settings.Secure->GetStringForUser(context->GetContentResolver(),
                Settings.Secure.SMS_DEFAULT_APPLICATION, userId);
        If (DEBUG_MULTIUSER) {
            Logger::I(LOG_TAG, "getApplication defaultApp=" + defaultApplication);
        }

        SmsApplicationData applicationData = NULL;
        If (defaultApplication != NULL) {
            applicationData = GetApplicationForPackage(applications, defaultApplication);
        }
        If (DEBUG_MULTIUSER) {
            Logger::I(LOG_TAG, "getApplication appData=" + applicationData);
        }
        // Picking a new SMS app requires AppOps and Settings.Secure permissions, so we only do
        // this if the caller asked us to.
        If (updateIfNeeded && applicationData == NULL) {
            // Try to find the default SMS package for this device
            Resources r = context->GetResources();
            String defaultPackage =
                    r->GetString(R::string::default_sms_application);
            applicationData = GetApplicationForPackage(applications, defaultPackage);

            If (applicationData == NULL) {
                // Are there any applications?
                If (applications->Size() != 0) {
                    applicationData = (SmsApplicationData)applications->ToArray()[0];
                }
            }

            // If we found a new default app, update the setting
            If (applicationData != NULL) {
                SetDefaultApplicationInternal(applicationData.mPackageName, context, userId);
            }
        }

        // If we found a package, make sure AppOps permissions are set up correctly
        If (applicationData != NULL) {
            AppOpsManager appOps = (AppOpsManager)context->GetSystemService(Context.APP_OPS_SERVICE);

            // We can only call checkOp if we are Privileged (updateIfNeeded) or if the app we
            // are checking is for our current uid. Doing this check from the unprivileged current
            // SMS app allows us to tell the current SMS app that it is not in a good state and
            // needs to ask to be the current SMS app again to work properly.
            If (updateIfNeeded || applicationData.mUid == android.os.Process->MyUid()) {
                // Verify that the SMS app has permissions
                Int32 mode = appOps->CheckOp(AppOpsManager.OP_WRITE_SMS, applicationData.mUid,
                        applicationData.mPackageName);
                If (mode != AppOpsManager.MODE_ALLOWED) {
                    Rlog->E(LOG_TAG, applicationData.mPackageName + " lost OP_WRITE_SMS: " +
                            (updateIfNeeded ? " (fixing)" : " (no permission to fix)"));
                    If (updateIfNeeded) {
                        appOps->SetMode(AppOpsManager.OP_WRITE_SMS, applicationData.mUid,
                                applicationData.mPackageName, AppOpsManager.MODE_ALLOWED);
                    } else {
                        // We can not return a package if permissions are not set up correctly
                        applicationData = NULL;
                    }
                }
            }

            // We can only verify the phone and BT app's permissions from a privileged caller
            If (updateIfNeeded) {
                // Ensure this component is still configured as the preferred activity. Usually the
                // current SMS app will already be the preferred activity - but checking whether or
                // not this is TRUE is just as expensive as reconfiguring the preferred activity so
                // we just reconfigure every time.
                PackageManager packageManager = context->GetPackageManager();
                ConfigurePreferredActivity(packageManager, new ComponentName(
                        applicationData.mPackageName, applicationData.mSendToClass),
                        userId);
                // Verify that the phone, BT app and MmsService have permissions
                try {
                    PackageInfo info = packageManager->GetPackageInfo(PHONE_PACKAGE_NAME, 0);
                    Int32 mode = appOps->CheckOp(AppOpsManager.OP_WRITE_SMS, info.applicationInfo.uid,
                            PHONE_PACKAGE_NAME);
                    If (mode != AppOpsManager.MODE_ALLOWED) {
                        Rlog->E(LOG_TAG, PHONE_PACKAGE_NAME + " lost OP_WRITE_SMS:  (fixing)");
                        appOps->SetMode(AppOpsManager.OP_WRITE_SMS, info.applicationInfo.uid,
                                PHONE_PACKAGE_NAME, AppOpsManager.MODE_ALLOWED);
                    }
                } Catch (NameNotFoundException e) {
                    // No phone app on this Device (unexpected, even for non-phone devices)
                    Rlog->E(LOG_TAG, "Phone package not found: " + PHONE_PACKAGE_NAME);
                    applicationData = NULL;
                }

                try {
                    PackageInfo info = packageManager->GetPackageInfo(BLUETOOTH_PACKAGE_NAME, 0);
                    Int32 mode = appOps->CheckOp(AppOpsManager.OP_WRITE_SMS, info.applicationInfo.uid,
                            BLUETOOTH_PACKAGE_NAME);
                    If (mode != AppOpsManager.MODE_ALLOWED) {
                        Rlog->E(LOG_TAG, BLUETOOTH_PACKAGE_NAME + " lost OP_WRITE_SMS:  (fixing)");
                        appOps->SetMode(AppOpsManager.OP_WRITE_SMS, info.applicationInfo.uid,
                                BLUETOOTH_PACKAGE_NAME, AppOpsManager.MODE_ALLOWED);
                    }
                } Catch (NameNotFoundException e) {
                    // No BT app on this device
                    Rlog->E(LOG_TAG, "Bluetooth package not found: " + BLUETOOTH_PACKAGE_NAME);
                }

                try {
                    PackageInfo info = packageManager->GetPackageInfo(MMS_SERVICE_PACKAGE_NAME, 0);
                    Int32 mode = appOps->CheckOp(AppOpsManager.OP_WRITE_SMS, info.applicationInfo.uid,
                            MMS_SERVICE_PACKAGE_NAME);
                    If (mode != AppOpsManager.MODE_ALLOWED) {
                        Rlog->E(LOG_TAG, MMS_SERVICE_PACKAGE_NAME + " lost OP_WRITE_SMS:  (fixing)");
                        appOps->SetMode(AppOpsManager.OP_WRITE_SMS, info.applicationInfo.uid,
                                MMS_SERVICE_PACKAGE_NAME, AppOpsManager.MODE_ALLOWED);
                    }
                } Catch (NameNotFoundException e) {
                    // No phone app on this Device (unexpected, even for non-phone devices)
                    Rlog->E(LOG_TAG, "MmsService package not found: " + MMS_SERVICE_PACKAGE_NAME);
                    applicationData = NULL;
                }

            }
        }
        If (DEBUG_MULTIUSER) {
            Logger::I(LOG_TAG, "getApplication returning appData=" + applicationData);
        }
        return applicationData;
    }

    /**
     * Sets the specified package as the default SMS/MMS application. The caller of this method
     * needs to have permission to set AppOps and write to secure settings.
     */
    public static void SetDefaultApplication(String packageName, Context context) {
        TelephonyManager tm = (TelephonyManager)context->GetSystemService(Context.TELEPHONY_SERVICE);
        If (!tm->IsSmsCapable()) {
            // No phone, no SMS
            return;
        }

        final Int32 userId = GetIncomingUserId(context);
        final Int64 token = Binder->ClearCallingIdentity();
        try {
            SetDefaultApplicationInternal(packageName, context, userId);
        } finally {
            Binder->RestoreCallingIdentity(token);
        }
    }

    private static void SetDefaultApplicationInternal(String packageName, Context context,
            Int32 userId) {
        // Get old package name
        String oldPackageName = Settings.Secure->GetStringForUser(context->GetContentResolver(),
                Settings.Secure.SMS_DEFAULT_APPLICATION, userId);

        If (packageName != NULL && oldPackageName != NULL && packageName->Equals(oldPackageName)) {
            // No change
            return;
        }

        // We only make the change if the new package is valid
        PackageManager packageManager = context->GetPackageManager();
        Collection<SmsApplicationData> applications = GetApplicationCollection(context);
        SmsApplicationData applicationData = GetApplicationForPackage(applications, packageName);
        If (applicationData != NULL) {
            // Ignore OP_WRITE_SMS for the previously configured default SMS app.
            AppOpsManager appOps = (AppOpsManager)context->GetSystemService(Context.APP_OPS_SERVICE);
            If (oldPackageName != NULL) {
                try {
                    PackageInfo info = packageManager->GetPackageInfo(oldPackageName,
                            PackageManager.GET_UNINSTALLED_PACKAGES);
                    appOps->SetMode(AppOpsManager.OP_WRITE_SMS, info.applicationInfo.uid,
                            oldPackageName, AppOpsManager.MODE_IGNORED);
                } Catch (NameNotFoundException e) {
                    Rlog->W(LOG_TAG, "Old SMS package not found: " + oldPackageName);
                }
            }

            // Update the secure setting.
            Settings.Secure->PutStringForUser(context->GetContentResolver(),
                    Settings.Secure.SMS_DEFAULT_APPLICATION, applicationData.mPackageName,
                    userId);

            // Configure this as the preferred activity for SENDTO sms/mms intents
            ConfigurePreferredActivity(packageManager, new ComponentName(
                    applicationData.mPackageName, applicationData.mSendToClass), userId);

            // Allow OP_WRITE_SMS for the newly configured default SMS app.
            appOps->SetMode(AppOpsManager.OP_WRITE_SMS, applicationData.mUid,
                    applicationData.mPackageName, AppOpsManager.MODE_ALLOWED);

            // Phone needs to always have this permission to write to the sms database
            try {
                PackageInfo info = packageManager->GetPackageInfo(PHONE_PACKAGE_NAME, 0);
                appOps->SetMode(AppOpsManager.OP_WRITE_SMS, info.applicationInfo.uid,
                        PHONE_PACKAGE_NAME, AppOpsManager.MODE_ALLOWED);
            } Catch (NameNotFoundException e) {
                // No phone app on this Device (unexpected, even for non-phone devices)
                Rlog->E(LOG_TAG, "Phone package not found: " + PHONE_PACKAGE_NAME);
            }

            // BT needs to always have this permission to write to the sms database
            try {
                PackageInfo info = packageManager->GetPackageInfo(BLUETOOTH_PACKAGE_NAME, 0);
                appOps->SetMode(AppOpsManager.OP_WRITE_SMS, info.applicationInfo.uid,
                        BLUETOOTH_PACKAGE_NAME, AppOpsManager.MODE_ALLOWED);
            } Catch (NameNotFoundException e) {
                // No BT app on this device
                Rlog->E(LOG_TAG, "Bluetooth package not found: " + BLUETOOTH_PACKAGE_NAME);
            }

            // MmsService needs to always have this permission to write to the sms database
            try {
                PackageInfo info = packageManager->GetPackageInfo(MMS_SERVICE_PACKAGE_NAME, 0);
                appOps->SetMode(AppOpsManager.OP_WRITE_SMS, info.applicationInfo.uid,
                        MMS_SERVICE_PACKAGE_NAME, AppOpsManager.MODE_ALLOWED);
            } Catch (NameNotFoundException e) {
                // No phone app on this Device (unexpected, even for non-phone devices)
                Rlog->E(LOG_TAG, "MmsService package not found: " + MMS_SERVICE_PACKAGE_NAME);
            }
        }
    }

    /**
     * Tracks package changes and ensures that the default SMS app is always configured to be the
     * preferred activity for SENDTO sms/mms intents.
     */
    private static const class SmsPackageMonitor extends PackageMonitor {
        final Context mContext;

        public SmsPackageMonitor(Context context) {
            Super();
            mContext = context;
        }

        //@Override
        CARAPI OnPackageDisappeared(String packageName, Int32 reason) {
            OnPackageChanged(packageName);
        }

        //@Override
        CARAPI OnPackageAppeared(String packageName, Int32 reason) {
            OnPackageChanged(packageName);
        }

        //@Override
        CARAPI OnPackageModified(String packageName) {
            OnPackageChanged(packageName);
        }

        private void OnPackageChanged(String packageName) {
            PackageManager packageManager = mContext->GetPackageManager();
            Context userContext = mContext;
            final Int32 userId = GetSendingUserId();
            If (userId != UserHandle.USER_OWNER) {
                try {
                    userContext = mContext->CreatePackageContextAsUser(mContext->GetPackageName(), 0,
                            new UserHandle(userId));
                } Catch (NameNotFoundException nnfe) {
                    If (DEBUG_MULTIUSER) {
                        Logger::W(LOG_TAG, "Unable to create package context for user " + userId);
                    }
                }
            }
            // Ensure this component is still configured as the preferred activity
            ComponentName componentName = GetDefaultSendToApplication(userContext, TRUE);
            If (componentName != NULL) {
                ConfigurePreferredActivity(packageManager, componentName, userId);
            }
        }
    }

    public static void InitSmsPackageMonitor(Context context) {
        sSmsPackageMonitor = new SmsPackageMonitor(context);
        sSmsPackageMonitor->Register(context, context->GetMainLooper(), UserHandle.ALL, FALSE);
    }

    private static void ConfigurePreferredActivity(PackageManager packageManager,
            ComponentName componentName, Int32 userId) {
        // Add the four activity preferences we want to direct to this app.
        ReplacePreferredActivity(packageManager, componentName, userId, SCHEME_SMS);
        ReplacePreferredActivity(packageManager, componentName, userId, SCHEME_SMSTO);
        ReplacePreferredActivity(packageManager, componentName, userId, SCHEME_MMS);
        ReplacePreferredActivity(packageManager, componentName, userId, SCHEME_MMSTO);
    }

    /**
     * Updates the ACTION_SENDTO activity to the specified component for the specified scheme.
     */
    private static void ReplacePreferredActivity(PackageManager packageManager,
            ComponentName componentName, Int32 userId, String scheme) {
        // Build the set of existing activities that handle this scheme
        Intent intent = new Intent(IIntent::ACTION_SENDTO, Uri->FromParts(scheme, "", NULL));
        List<ResolveInfo> resolveInfoList = packageManager->QueryIntentActivitiesAsUser(
                intent, PackageManager.MATCH_DEFAULT_ONLY | PackageManager.GET_RESOLVED_FILTER,
                userId);

        // Build the set of ComponentNames for these activities
        final Int32 n = resolveInfoList->Size();
        ComponentName[] set = new ComponentName[n];
        For (Int32 i = 0; i < n; i++) {
            ResolveInfo info = resolveInfoList->Get(i);
            set[i] = new ComponentName(info.activityInfo.packageName, info.activityInfo.name);
        }

        // Update the preferred SENDTO activity for the specified scheme
        IntentFilter intentFilter = new IntentFilter();
        intentFilter->AddAction(IIntent::ACTION_SENDTO);
        intentFilter->AddCategory(Intent.CATEGORY_DEFAULT);
        intentFilter->AddDataScheme(scheme);
        packageManager->ReplacePreferredActivityAsUser(intentFilter,
                IntentFilter.MATCH_CATEGORY_SCHEME + IntentFilter.MATCH_ADJUSTMENT_NORMAL,
                set, componentName, userId);
    }

    /**
     * Returns SmsApplicationData for this package if this package is capable of being set as the
     * default SMS application.
     */
    public static SmsApplicationData GetSmsApplicationData(String packageName, Context context) {
        Collection<SmsApplicationData> applications = GetApplicationCollection(context);
        return GetApplicationForPackage(applications, packageName);
    }

    /**
     * Gets the default SMS application
     * @param context context from the calling app
     * @param updateIfNeeded update the default app if there is no valid default app configured.
     * @return component name of the app and class to deliver SMS messages to
     */
    public static ComponentName GetDefaultSmsApplication(Context context, Boolean updateIfNeeded) {
        Int32 userId = GetIncomingUserId(context);
        final Int64 token = Binder->ClearCallingIdentity();
        try {
            ComponentName component = NULL;
            SmsApplicationData smsApplicationData = GetApplication(context, updateIfNeeded,
                    userId);
            If (smsApplicationData != NULL) {
                component = new ComponentName(smsApplicationData.mPackageName,
                        smsApplicationData.mSmsReceiverClass);
            }
            return component;
        } finally {
            Binder->RestoreCallingIdentity(token);
        }
    }

    /**
     * Gets the default MMS application
     * @param context context from the calling app
     * @param updateIfNeeded update the default app if there is no valid default app configured.
     * @return component name of the app and class to deliver MMS messages to
     */
    public static ComponentName GetDefaultMmsApplication(Context context, Boolean updateIfNeeded) {
        Int32 userId = GetIncomingUserId(context);
        final Int64 token = Binder->ClearCallingIdentity();
        try {
            ComponentName component = NULL;
            SmsApplicationData smsApplicationData = GetApplication(context, updateIfNeeded,
                    userId);
            If (smsApplicationData != NULL) {
                component = new ComponentName(smsApplicationData.mPackageName,
                        smsApplicationData.mMmsReceiverClass);
            }
            return component;
        } finally {
            Binder->RestoreCallingIdentity(token);
        }
    }

    /**
     * Gets the default Respond Via Message application
     * @param context context from the calling app
     * @param updateIfNeeded update the default app if there is no valid default app configured.
     * @return component name of the app and class to direct Respond Via Message intent to
     */
    public static ComponentName GetDefaultRespondViaMessageApplication(Context context,
            Boolean updateIfNeeded) {
        Int32 userId = GetIncomingUserId(context);
        final Int64 token = Binder->ClearCallingIdentity();
        try {
            ComponentName component = NULL;
            SmsApplicationData smsApplicationData = GetApplication(context, updateIfNeeded,
                    userId);
            If (smsApplicationData != NULL) {
                component = new ComponentName(smsApplicationData.mPackageName,
                        smsApplicationData.mRespondViaMessageClass);
            }
            return component;
        } finally {
            Binder->RestoreCallingIdentity(token);
        }
    }

    /**
     * Gets the default Send To (smsto) application.
     * <p>
     * Caller must pass in the correct user context if calling from a singleton service.
     * @param context context from the calling app
     * @param updateIfNeeded update the default app if there is no valid default app configured.
     * @return component name of the app and class to direct SEND_TO (smsto) intent to
     */
    public static ComponentName GetDefaultSendToApplication(Context context,
            Boolean updateIfNeeded) {
        Int32 userId = GetIncomingUserId(context);
        final Int64 token = Binder->ClearCallingIdentity();
        try {
            ComponentName component = NULL;
            SmsApplicationData smsApplicationData = GetApplication(context, updateIfNeeded,
                    userId);
            If (smsApplicationData != NULL) {
                component = new ComponentName(smsApplicationData.mPackageName,
                        smsApplicationData.mSendToClass);
            }
            return component;
        } finally {
            Binder->RestoreCallingIdentity(token);
        }
    }

    /**
     * Returns whether need to write the SMS message to SMS database for this package.
     * <p>
     * Caller must pass in the correct user context if calling from a singleton service.
     */
    public static Boolean ShouldWriteMessageForPackage(String packageName, Context context) {
        If (packageName == NULL) return TRUE;

        If (SmsManager->GetDefault()->GetAutoPersisting()) {
            return TRUE;
        }

        String defaultSmsPackage = NULL;
        ComponentName component = GetDefaultSmsApplication(context, FALSE);
        If (component != NULL) {
            defaultSmsPackage = component->GetPackageName();
        }

        List<String> ignorePackages = Arrays->AsList(
                context->GetResources()->GetStringArray(R.array.config_ignored_sms_packages));

        If (ignorePackages->Contains(packageName)) {
            return FALSE;
        }

        If ((defaultSmsPackage == NULL || !defaultSmsPackage->Equals(packageName)) &&
                !packageName->Equals(BLUETOOTH_PACKAGE_NAME)) {
            // To write the message for someone other than the default SMS and BT app
            return TRUE;
        }

        return FALSE;
    }
}
