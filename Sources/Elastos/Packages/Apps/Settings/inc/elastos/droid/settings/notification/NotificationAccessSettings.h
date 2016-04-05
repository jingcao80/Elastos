/*
 * Copyright (C) 2010 The Android Open Source Project
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

package com.android.settings.notification;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Service::Notification::INotificationListenerService;

using Elastos::Droid::Settings::IR;

public class NotificationAccessSettings extends ManagedServiceSettings {
    private static const String TAG = NotificationAccessSettings.class->GetSimpleName();
    private static const Config CONFIG = GetNotificationListenerConfig();

    private static Config GetNotificationListenerConfig() {
        final Config c = new Config();
        c.tag = TAG;
        c.setting = Settings.Secure.ENABLED_NOTIFICATION_LISTENERS;
        c.intentAction = NotificationListenerService.SERVICE_INTERFACE;
        c.permission = Manifest::permission::BIND_NOTIFICATION_LISTENER_SERVICE;
        c.noun = "notification listener";
        c.warningDialogTitle = R::string::notification_listener_security_warning_title;
        c.warningDialogSummary = R::string::notification_listener_security_warning_summary;
        c.emptyText = R::string::no_notification_listeners;
        return c;
    }

    //@Override
    protected Config GetConfig() {
        return CONFIG;
    }

    public static Int32 GetListenersCount(PackageManager pm) {
        return GetServicesCount(CONFIG, pm);
    }

    public static Int32 GetEnabledListenersCount(Context context) {
        return GetEnabledServicesCount(CONFIG, context);
    }
}
