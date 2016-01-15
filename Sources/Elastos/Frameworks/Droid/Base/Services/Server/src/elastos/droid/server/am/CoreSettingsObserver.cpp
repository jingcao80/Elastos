
#include "elastos/droid/server/am/CoreSettingsObserver.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Core::ICloneable;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

const String CoreSettingsObserver::TAG("CoreSettingsObserver"); // = CoreSettingsObserver.class.getSimpleName();

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

CoreSettingsObserver::CoreSettingsObserver(
    /* [in] */ CActivityManagerService* activityManagerService)
    : mActivityManagerService(activityManagerService)
{
    ContentObserver::constructor(activityManagerService->mHandler);
    CBundle::New((IBundle**)&mCoreSettings);
    BeginObserveCoreSettings();
    SendCoreSettings();
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
    assert(0);
    // mActivityManagerService->OnCoreSettingsChange(mCoreSettings);
}

void CoreSettingsObserver::BeginObserveCoreSettings()
{
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    AutoPtr<IContentResolver> resolver;
    mActivityManagerService->mContext->GetContentResolver((IContentResolver**)&resolver);
    HashMap<String, String>::Iterator it;
    for (it = sSecureSettingToTypeMap.Begin(); it != sSecureSettingToTypeMap.End(); ++it) {
        String setting = it->mFirst;
        AutoPtr<IUri> uri;
        settingsSecure->GetUriFor(setting, (IUri**)&uri);
        resolver->RegisterContentObserver(uri, FALSE, this);
    }

    for (it = sSystemSettingToTypeMap.Begin(); it != sSystemSettingToTypeMap.End(); ++it) {
        String setting = it->mFirst;
        AutoPtr<IUri> uri;
        settingsSecure->GetUriFor(setting, (IUri**)&uri);
        resolver->RegisterContentObserver(uri, FALSE, this);
    }

    for (it = sGlobalSettingToTypeMap.Begin(); it != sGlobalSettingToTypeMap.End(); ++it) {
        String setting = it->mFirst;
        AutoPtr<IUri> uri;
        settingsSecure->GetUriFor(setting, (IUri**)&uri);
        resolver->RegisterContentObserver(uri, FALSE, this);
    }
}

void CoreSettingsObserver::PopulateSettings(
    /* [in] */ IBundle* snapshot,
    /* [in] */ HashMap<String, String>* map)
{
    AutoPtr<IContext> context = mActivityManagerService->mContext;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    AutoPtr<ISettingsSystem> settingsSystem;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSystem);
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);

    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    HashMap<String, String>::Iterator it;
    for (it = map->Begin(); it != map->End(); ++it) {
        String setting = it->mFirst;
        String type = it->mSecond;
        // try {
        ECode ec;
        do {
            if (type.Equals("String")) {
                String value;
                if (map == &sSecureSettingToTypeMap) {
                    ec = settingsSecure->GetString(resolver, setting, &value);
                }
                else if (map == &sSystemSettingToTypeMap) {
                    ec = settingsSystem->GetString(resolver, setting, &value);
                }
                else {
                    ec = settingsGlobal->GetString(resolver, setting, &value);
                }
                if (FAILED(ec))
                    break;
                snapshot->PutString(setting, value);
            }
            else if (type.Equals("Int32")) {
                Int32 value;
                if (map == &sSecureSettingToTypeMap) {
                    ec = settingsSecure->GetInt32(resolver, setting, &value);
                }
                else if (map == &sSystemSettingToTypeMap) {
                    ec = settingsSystem->GetInt32(resolver, setting, &value);
                }
                else {
                    ec = settingsGlobal->GetInt32(resolver, setting, &value);
                }
                if (FAILED(ec))
                    break;
                snapshot->PutInt32(setting, value);
            }
            else if (type.Equals("Float")) {
                Float value;
                if (map == &sSecureSettingToTypeMap) {
                    ec = settingsSecure->GetFloat(resolver, setting, &value);
                }
                else if (map == &sSystemSettingToTypeMap) {
                    ec = settingsSystem->GetFloat(resolver, setting, &value);
                }
                else {
                    ec = settingsGlobal->GetFloat(resolver, setting, &value);
                }
                if (FAILED(ec))
                    break;
                snapshot->PutFloat(setting, value);
            }
            else if (type.Equals("Int64")) {
                Int64 value;
                if (map == &sSecureSettingToTypeMap) {
                    ec = settingsSecure->GetInt64(resolver, setting, &value);
                }
                else if (map == &sSystemSettingToTypeMap) {
                    ec = settingsSystem->GetInt64(resolver, setting, &value);
                }
                else {
                    ec = settingsGlobal->GetInt64(resolver, setting, &value);
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
