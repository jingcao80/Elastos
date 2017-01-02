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

#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Service.h"
#include "elastos/droid/settings/notification/CNotificationAccessSettings.h"
#include "../R.h"
#include "elastos/droid/Manifest.h"

using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Service::Notification::INotificationListenerService;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

const String CNotificationAccessSettings::TAG("CNotificationAccessSettings");

const AutoPtr<ManagedServiceSettings::Config> CNotificationAccessSettings::CONFIG = GetNotificationListenerConfig();

CAR_OBJECT_IMPL(CNotificationAccessSettings)

AutoPtr<ManagedServiceSettings::Config> CNotificationAccessSettings::GetNotificationListenerConfig()
{
    AutoPtr<Config> c = new Config();
    c->mTag = TAG;
    c->mSetting = ISettingsSecure::ENABLED_NOTIFICATION_LISTENERS;
    c->mIntentAction = INotificationListenerService::SERVICE_INTERFACE;
    c->mPermission = Manifest::permission::BIND_NOTIFICATION_LISTENER_SERVICE;
    c->mNoun = "notification listener";
    c->mWarningDialogTitle = R::string::notification_listener_security_warning_title;
    c->mWarningDialogSummary = R::string::notification_listener_security_warning_summary;
    c->mEmptyText = R::string::no_notification_listeners;
    return c;
}

AutoPtr<ManagedServiceSettings::Config> CNotificationAccessSettings::GetConfig()
{
    return CONFIG;
}

Int32 CNotificationAccessSettings::GetListenersCount(
    /* [in] */ IPackageManager* pm)
{
    return GetServicesCount(CONFIG, pm);
}

Int32 CNotificationAccessSettings::GetEnabledListenersCount(
    /* [in] */ IContext* context)
{
    return GetEnabledServicesCount(CONFIG, context);
}

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos