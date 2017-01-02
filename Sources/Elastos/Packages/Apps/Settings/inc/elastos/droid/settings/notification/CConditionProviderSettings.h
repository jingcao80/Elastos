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

#ifndef __ELASTOS_DROID_SETTINGS_NOTIFICATION_CCONDITIONPROVIDERSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_NOTIFICATION_CCONDITIONPROVIDERSETTINGS_H__

#include "_Elastos_Droid_Settings_Notification_CConditionProviderSettings.h"
#include "elastos/droid/settings/notification/ManagedServiceSettings.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

CarClass(CConditionProviderSettings)
    , public ManagedServiceSettings
{
public:
    CAR_OBJECT_DECL()

    TO_STRING_IMPL("CConditionProviderSettings")

    static CARAPI_(Int32) GetProviderCount(
        /* [in] */ IPackageManager* pm);

    static CARAPI_(Int32) GetEnabledProviderCount(
        /* [in] */ IContext* context);

protected:
    //@Override
    CARAPI_(AutoPtr<ManagedServiceSettings::Config>) GetConfig();

private:
    static CARAPI_(AutoPtr<ManagedServiceSettings::Config>) GetConditionProviderConfig();

private:
    static const String TAG;
    static const  AutoPtr<ManagedServiceSettings::Config> CONFIG;
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_NOTIFICATION_CCONDITIONPROVIDERSETTINGS_H__
