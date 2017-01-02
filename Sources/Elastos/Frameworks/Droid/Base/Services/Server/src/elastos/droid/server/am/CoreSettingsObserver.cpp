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

#include "elastos/droid/server/am/CoreSettingsObserver.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include <elastos/droid/provider/Settings.h>
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Core::ICloneable;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

const String CoreSettingsObserver::TAG("CoreSettingsObserver");

// mapping form property name to its type
HashMap<String, String> CoreSettingsObserver::sSecureSettingToTypeMap;
HashMap<String, String> CoreSettingsObserver::sSystemSettingToTypeMap;
HashMap<String, String> CoreSettingsObserver::sGlobalSettingToTypeMap;

Boolean CoreSettingsObserver::Init()
{
    sSecureSettingToTypeMap[ISettingsSecure::LONG_PRESS_TIMEOUT] = String("Int32");
    // add other secure settings here...

    sSystemSettingToTypeMap[ISettingsSystem::TIME_12_24] = String("String");
    // add other system settings here...

    sGlobalSettingToTypeMap[ISettingsGlobal::DEBUG_VIEW_ATTRIBUTES] = String("Int32");
    // add other global settings here...
    return TRUE;
}

Boolean sInit = CoreSettingsObserver::Init();

CoreSettingsObserver::CoreSettingsObserver()
{
}

ECode CoreSettingsObserver::constructor(
    /* [in] */ CActivityManagerService* activityManagerService)
{
    ContentObserver::constructor(activityManagerService->mHandler);
    mActivityManagerService = activityManagerService;

    CBundle::New((IBundle**)&mCoreSettings);
    assert(mCoreSettings != NULL);
    BeginObserveCoreSettings();
    SendCoreSettings();
    return NOERROR;
}

AutoPtr<IBundle> CoreSettingsObserver::GetCoreSettingsLocked()
{
    AutoPtr<IInterface> coreSettings;
    ICloneable::Probe(mCoreSettings)->Clone((IInterface**)&coreSettings);
    return IBundle::Probe(coreSettings);
}

// @Override
ECode CoreSettingsObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    AutoLock lock(mActivityManagerService);
    SendCoreSettings();
    return NOERROR;
}

void CoreSettingsObserver::SendCoreSettings()
{
    PopulateSettings(mCoreSettings, &sSecureSettingToTypeMap);
    PopulateSettings(mCoreSettings, &sSystemSettingToTypeMap);
    PopulateSettings(mCoreSettings, &sGlobalSettingToTypeMap);
    mActivityManagerService->OnCoreSettingsChange(mCoreSettings);
}

void CoreSettingsObserver::BeginObserveCoreSettings()
{
    AutoPtr<IContentResolver> resolver;
    mActivityManagerService->mContext->GetContentResolver((IContentResolver**)&resolver);
    HashMap<String, String>::Iterator it;
    for (it = sSecureSettingToTypeMap.Begin(); it != sSecureSettingToTypeMap.End(); ++it) {
        AutoPtr<IUri> uri;
        Settings::Secure::GetUriFor(it->mFirst, (IUri**)&uri);
        resolver->RegisterContentObserver(uri, FALSE, this);
    }

    for (it = sSystemSettingToTypeMap.Begin(); it != sSystemSettingToTypeMap.End(); ++it) {
        AutoPtr<IUri> uri;
        Settings::System::GetUriFor(it->mFirst, (IUri**)&uri);
        resolver->RegisterContentObserver(uri, FALSE, this);
    }

    for (it = sGlobalSettingToTypeMap.Begin(); it != sGlobalSettingToTypeMap.End(); ++it) {
        AutoPtr<IUri> uri;
        Settings::Global::GetUriFor(it->mFirst, (IUri**)&uri);
        resolver->RegisterContentObserver(uri, FALSE, this);
    }
}

void CoreSettingsObserver::PopulateSettings(
    /* [in] */ IBundle* snapshot,
    /* [in] */ HashMap<String, String>* map)
{
    AutoPtr<IContext> context = mActivityManagerService->mContext;
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    HashMap<String, String>::Iterator it;
    for (it = map->Begin(); it != map->End(); ++it) {
        String setting = it->mFirst;
        String type = it->mSecond;
        // try {
        ECode ec = NOERROR;
        do {
            if (type.Equals("String")) {
                String value;
                if (map == &sSecureSettingToTypeMap) {
                    ec = Settings::Secure::GetString(resolver, setting, &value);
                }
                else if (map == &sSystemSettingToTypeMap) {
                    ec = Settings::System::GetString(resolver, setting, &value);
                }
                else {
                    ec = Settings::Global::GetString(resolver, setting, &value);
                }
                if (FAILED(ec))
                    break;
                snapshot->PutString(setting, value);
            }
            else if (type.Equals("Int32")) {
                Int32 value;
                if (map == &sSecureSettingToTypeMap) {
                    ec = Settings::Secure::GetInt32(resolver, setting, &value);
                }
                else if (map == &sSystemSettingToTypeMap) {
                    ec = Settings::System::GetInt32(resolver, setting, &value);
                }
                else {
                    ec = Settings::Global::GetInt32(resolver, setting, &value);
                }
                if (FAILED(ec))
                    break;
                snapshot->PutInt32(setting, value);
            }
            else if (type.Equals("Float")) {
                Float value;
                if (map == &sSecureSettingToTypeMap) {
                    ec = Settings::Secure::GetFloat(resolver, setting, &value);
                }
                else if (map == &sSystemSettingToTypeMap) {
                    ec = Settings::System::GetFloat(resolver, setting, &value);
                }
                else {
                    ec = Settings::Global::GetFloat(resolver, setting, &value);
                }
                if (FAILED(ec))
                    break;
                snapshot->PutFloat(setting, value);
            }
            else if (type.Equals("Int64")) {
                Int64 value;
                if (map == &sSecureSettingToTypeMap) {
                    ec = Settings::Secure::GetInt64(resolver, setting, &value);
                }
                else if (map == &sSystemSettingToTypeMap) {
                    ec = Settings::System::GetInt64(resolver, setting, &value);
                }
                else {
                    ec = Settings::Global::GetInt64(resolver, setting, &value);
                }
                if (FAILED(ec))
                    break;
                snapshot->PutInt64(setting, value);
            }
        } while (0);

        if (FAILED(ec)) {
            Slogger::W(TAG, "Cannot find setting \"%s\" with type \"%s\"", setting.string(), type.string());
        }
    }
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
