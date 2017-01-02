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

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/provider/CSettingsSystem.h"
#include "elastos/droid/provider/Settings.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CSettingsSystem)

CAR_INTERFACE_IMPL(CSettingsSystem, Singleton, ISettingsSystem)

ECode CSettingsSystem::GetUriFor(
    /* [in] */ IUri* u,
    /* [in] */ const String& name,
    /* [out] */ IUri** value)
{
    VALIDATE_NOT_NULL(value)
    return Settings::NameValueTable::GetUriFor(u, name, value);
}

ECode CSettingsSystem::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    *uri = Settings::System::CONTENT_URI;
    REFCOUNT_ADD(*uri)
    return NOERROR;
}

ECode CSettingsSystem::GetSETTINGS_TO_BACKUP(
    /* [out, callee] */ ArrayOf<String>** array)
{
    VALIDATE_NOT_NULL(array)
    *array = Settings::System::SETTINGS_TO_BACKUP;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

ECode CSettingsSystem::GetCLONE_TO_MANAGED_PROFILE(
    /* [out, callee] */ ArrayOf<String>** array)
{
    VALIDATE_NOT_NULL(array)
    *array = Settings::System::CLONE_TO_MANAGED_PROFILE;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

ECode CSettingsSystem::GetDEFAULT_RINGTONE_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    *uri = Settings::System::DEFAULT_RINGTONE_URI;
    REFCOUNT_ADD(*uri)
    return NOERROR;
}

ECode CSettingsSystem::GetDEFAULT_RINGTONE_URI_2(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    *uri = Settings::System::DEFAULT_RINGTONE_URI_2;
    REFCOUNT_ADD(*uri)
    return NOERROR;
}

ECode CSettingsSystem::GetDEFAULT_RINGTONE_URI_3(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    *uri = Settings::System::DEFAULT_RINGTONE_URI_3;
    REFCOUNT_ADD(*uri)
    return NOERROR;
}

ECode CSettingsSystem::GetDEFAULT_NOTIFICATION_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    *uri = Settings::System::DEFAULT_NOTIFICATION_URI;
    REFCOUNT_ADD(*uri)
    return NOERROR;
}

ECode CSettingsSystem::GetDEFAULT_ALARM_ALERT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    *uri = Settings::System::DEFAULT_ALARM_ALERT_URI;
    REFCOUNT_ADD(*uri)
    return NOERROR;
}

ECode CSettingsSystem::GetVOLUME_SETTINGS(
    /* [out, callee] */ ArrayOf<String>** settings)
{
    VALIDATE_NOT_NULL(settings)
    *settings = Settings::System::VOLUME_SETTINGS;
    REFCOUNT_ADD(*settings)
    return NOERROR;
}

ECode CSettingsSystem::PutListAsDelimitedString(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [in] */ const String& delimiter,
    /* [in] */ IList* list)
{
    return Settings::System::PutListAsDelimitedString(resolver, name, delimiter, list);
}

ECode CSettingsSystem::GetDelimitedStringAsList(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [in] */ const String& delimiter,
    /* [out] */ IList** list)
{
    return Settings::System::GetDelimitedStringAsList(resolver, name, delimiter, list);
}

ECode CSettingsSystem::GetMovedKeys(
    /* [in] */ IHashSet* outKeySet)
{
    return Settings::System::GetMovedKeys(outKeySet);
}

ECode CSettingsSystem::GetNonLegacyMovedKeys(
    /* [in] */ IHashSet* outKeySet)
{
    return Settings::System::GetNonLegacyMovedKeys(outKeySet);
}

ECode CSettingsSystem::GetString(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    return Settings::System::GetString(resolver, name, value);
}

ECode CSettingsSystem::GetStringForUser(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [in] */ Int32 userHandle,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    return Settings::System::GetStringForUser(resolver, name, userHandle, value);
}

ECode CSettingsSystem::PutString(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [in] */ const String& value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return Settings::System::PutString(resolver, name, value, result);
}

ECode CSettingsSystem::PutStringForUser(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [in] */ const String& value,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    return Settings::System::PutStringForUser(resolver, name, value, userHandle, res);
}

ECode CSettingsSystem::GetUriFor(
    /* [in] */ const String& name,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    return Settings::System::GetUriFor(name, uri);
}

ECode CSettingsSystem::GetInt32(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int32 def,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    return Settings::System::GetInt32(cr, name, def, value);
}

ECode CSettingsSystem::GetInt32ForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int32 def,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    return Settings::System::GetInt32ForUser(cr, name, def, userHandle, value);
}

ECode CSettingsSystem::GetInt32(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    return Settings::System::GetInt32(cr, name, value);
}

ECode CSettingsSystem::GetInt32ForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    return Settings::System::GetInt32ForUser(cr, name, userHandle, value);
}

ECode CSettingsSystem::PutInt32(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int32 value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return Settings::System::PutInt32(cr, name, value, result);
}

/** @hide */
ECode CSettingsSystem::PutInt32ForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int32 value,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    return Settings::System::PutInt32ForUser(cr, name, value, userHandle, res);
}

ECode CSettingsSystem::GetInt64(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int64 def,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    return Settings::System::GetInt64(cr, name, def, value);
}

ECode CSettingsSystem::GetInt64ForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int64 def,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    return Settings::System::GetInt64ForUser(cr, name, def, userHandle, value);
}

ECode CSettingsSystem::GetInt64(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    return Settings::System::GetInt64(cr, name, value);
}

ECode CSettingsSystem::GetInt64ForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    return Settings::System::GetInt64ForUser(cr, name, userHandle, value);
}

ECode CSettingsSystem::PutInt64(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int64 value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return Settings::System::PutInt64(cr, name, value, result);
}

ECode CSettingsSystem::PutInt64ForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int64 value,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    return Settings::System::PutInt64ForUser(cr, name, value, userHandle, res);
}

ECode CSettingsSystem::GetFloat(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Float def,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value)
    return Settings::System::GetFloat(cr, name, def, value);
}

ECode CSettingsSystem::GetFloatForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Float def,
    /* [in] */ Int32 userHandle,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value)
    return Settings::System::GetFloatForUser(cr, name, def, userHandle, value);
}

ECode CSettingsSystem::GetFloat(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value)
    return Settings::System::GetFloat(cr, name, value);
}

ECode CSettingsSystem::GetFloatForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int32 userHandle,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value)
    return Settings::System::GetFloatForUser(cr, name, userHandle, value);
}

ECode CSettingsSystem::PutFloat(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Float value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return Settings::System::PutFloat(cr, name, value, result);
}

ECode CSettingsSystem::PutFloatForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Float value,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return Settings::System::PutFloatForUser(cr, name, value, userHandle, result);
}

ECode CSettingsSystem::GetConfiguration(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IConfiguration* outConfig)
{
    Settings::System::GetConfiguration(cr, outConfig);
    return NOERROR;
}

ECode CSettingsSystem::GetConfigurationForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IConfiguration* outConfig,
    /* [in] */ Int32 userHandle)
{
    Settings::System::GetConfigurationForUser(cr, outConfig, userHandle);
    return NOERROR;
}

ECode CSettingsSystem::ClearConfiguration(
    /* [in] */ IConfiguration* inoutConfig)
{
    Settings::System::ClearConfiguration(inoutConfig);
    return NOERROR;
}

ECode CSettingsSystem::PutConfiguration(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IConfiguration* config,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return Settings::System::PutConfiguration(cr, config, result);
}

ECode CSettingsSystem::PutConfigurationForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IConfiguration* config,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return Settings::System::PutConfigurationForUser(cr, config, userHandle, result);
}

ECode CSettingsSystem::HasInterestingConfigurationChanges(
    /* [in] */ Int32 changes,
    /* [out] */ Boolean* result)
 {
    VALIDATE_NOT_NULL(result)
    *result = Settings::System::HasInterestingConfigurationChanges(changes);
    return NOERROR;
 }

ECode CSettingsSystem::GetShowGTalkServiceStatus(
    /* [in] */ IContentResolver* cr,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return Settings::System::GetShowGTalkServiceStatus(cr, result);
}

ECode CSettingsSystem::GetShowGTalkServiceStatusForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return Settings::System::GetShowGTalkServiceStatusForUser(cr, userHandle, result);
}

ECode CSettingsSystem::SetShowGTalkServiceStatus(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Boolean flag)
{
    return Settings::System::SetShowGTalkServiceStatus(cr, flag);
}

ECode CSettingsSystem::SetShowGTalkServiceStatusForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Boolean flag,
    /* [in] */ Int32 userHandle)
{
    return Settings::System::SetShowGTalkServiceStatusForUser(cr, flag, userHandle);
}

} //namespace Provider
} //namespace Droid
} //namespace Elastos
