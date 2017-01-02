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

#ifndef __ELASTOS_DROID_PROVIDER_CSETTINGSSECURE_H__
#define __ELASTOS_DROID_PROVIDER_CSETTINGSSECURE_H__

#include "_Elastos_Droid_Provider_CSettingsSecure.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Net::IUri;
using Elastos::Core::Singleton;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CSettingsSecure)
    , public Singleton
    , public ISettingsSecure
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetUriFor(
        /* [in] */ IUri* u,
        /* [in] */ const String& name,
        /* [out] */ IUri** value);

    CARAPI PutListAsDelimitedString(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ const String& name,
        /* [in] */ const String& delimiter,
        /* [in] */ IList* list);

    CARAPI GetDelimitedStringAsList(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ const String& name,
        /* [in] */ const String& delimiter,
        /* [out] */ IList** list);

    /** @hide */
    CARAPI GetMovedKeys(
        /* [in] */ IHashSet* outKeySet);

    /**
     * Look up a name in the database.
     * @param resolver to access the database with
     * @param name to look up in the table
     * @return the corresponding value, or null if not present
     */
    CARAPI GetString(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ const String& name,
        /* [out] */ String* value);

    /** @hide */
    CARAPI GetStringForUser(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ const String& name,
        /* [in] */ Int32 userHandle,
        /* [out] */ String* value);

    /**
     * Store a name/value pair into the database.
     * @param resolver to access the database with
     * @param name to store
     * @param value to associate with the name
     * @return true if the value was set, false on database errors
     */
    CARAPI PutString(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ const String& name,
        /* [in] */ const String& value,
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI PutStringForUser(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ const String& name,
        /* [in] */ const String& value,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

    /**
     * Construct the content URI for a particular name/value pair,
     * useful for monitoring changes with a ContentObserver.
     * @param name to look up in the table
     * @return the corresponding content URI, or null if not present
     */
    CARAPI GetUriFor(
        /* [in] */ const String& name,
        /* [out] */ IUri** uri);

    /**
     * Convenience function for retrieving a single secure settings value
     * as an integer.  Note that internally setting values are always
     * stored as strings; this function converts the string to an integer
     * for you.  The default value will be returned if the setting is
     * not defined or not an integer.
     *
     * @param cr The ContentResolver to access.
     * @param name The name of the setting to retrieve.
     * @param def Value to return if the setting is not defined.
     *
     * @return The setting's current value, or 'def' if it is not defined
     * or not a valid integer.
     */
    CARAPI GetInt32(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& name,
        /* [in] */ Int32 def,
        /* [out] */ Int32* value);

    /** @hide */
    CARAPI GetInt32ForUser(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& name,
        /* [in] */ Int32 def,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* value);

    /**
     * Convenience function for retrieving a single secure settings value
     * as an integer.  Note that internally setting values are always
     * stored as strings; this function converts the string to an integer
     * for you.
     * <p>
     * This version does not take a default value.  If the setting has not
     * been set, or the string value is not a number,
     * it throws {@link SettingNotFoundException}.
     *
     * @param cr The ContentResolver to access.
     * @param name The name of the setting to retrieve.
     *
     * @throws SettingNotFoundException Thrown if a setting by the given
     * name can't be found or the setting value is not an integer.
     *
     * @return The setting's current value.
     */
    CARAPI GetInt32(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& name,
        /* [out] */ Int32* value);

    /** @hide */
    CARAPI GetInt32ForUser(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& name,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* value);

    /**
     * Convenience function for updating a single settings value as an
     * integer. This will either create a new entry in the table if the
     * given name does not exist, or modify the value of the existing row
     * with that name.  Note that internally setting values are always
     * stored as strings, so this function converts the given value to a
     * string before storing it.
     *
     * @param cr The ContentResolver to access.
     * @param name The name of the setting to modify.
     * @param value The new value for the setting.
     * @return true if the value was set, false on database errors
     */
    CARAPI PutInt32(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& name,
        /* [in] */ Int32 value,
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI PutInt32ForUser(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& name,
        /* [in] */ Int32 value,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

    /**
     * Convenience function for retrieving a single secure settings value
     * as a {@code long}.  Note that internally setting values are always
     * stored as strings; this function converts the string to a {@code long}
     * for you.  The default value will be returned if the setting is
     * not defined or not a {@code long}.
     *
     * @param cr The ContentResolver to access.
     * @param name The name of the setting to retrieve.
     * @param def Value to return if the setting is not defined.
     *
     * @return The setting's current value, or 'def' if it is not defined
     * or not a valid {@code long}.
     */
    CARAPI GetInt64(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& name,
        /* [in] */ Int64 def,
        /* [out] */ Int64* value);

    /** @hide */
    CARAPI GetInt64ForUser(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& name,
        /* [in] */ Int64 def,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int64* value);

    /**
     * Convenience function for retrieving a single secure settings value
     * as a {@code long}.  Note that internally setting values are always
     * stored as strings; this function converts the string to a {@code long}
     * for you.
     * <p>
     * This version does not take a default value.  If the setting has not
     * been set, or the string value is not a number,
     * it throws {@link SettingNotFoundException}.
     *
     * @param cr The ContentResolver to access.
     * @param name The name of the setting to retrieve.
     *
     * @return The setting's current value.
     * @throws SettingNotFoundException Thrown if a setting by the given
     * name can't be found or the setting value is not an integer.
     */
    CARAPI GetInt64(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& name,
        /* [out] */ Int64* value);

    /** @hide */
    CARAPI GetInt64ForUser(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& name,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int64* value);

    /**
     * Convenience function for updating a secure settings value as a long
     * integer. This will either create a new entry in the table if the
     * given name does not exist, or modify the value of the existing row
     * with that name.  Note that internally setting values are always
     * stored as strings, so this function converts the given value to a
     * string before storing it.
     *
     * @param cr The ContentResolver to access.
     * @param name The name of the setting to modify.
     * @param value The new value for the setting.
     * @return true if the value was set, false on database errors
     */
    CARAPI PutInt64(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& name,
        /* [in] */ Int64 value,
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI PutInt64ForUser(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& name,
        /* [in] */ Int64 value,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

    /**
     * Convenience function for retrieving a single secure settings value
     * as a floating point number.  Note that internally setting values are
     * always stored as strings; this function converts the string to an
     * float for you. The default value will be returned if the setting
     * is not defined or not a valid float.
     *
     * @param cr The ContentResolver to access.
     * @param name The name of the setting to retrieve.
     * @param def Value to return if the setting is not defined.
     *
     * @return The setting's current value, or 'def' if it is not defined
     * or not a valid float.
     */
    CARAPI GetFloat(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& name,
        /* [in] */ Float def,
        /* [out] */ Float* value);

    /** @hide */
    CARAPI GetFloatForUser(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& name,
        /* [in] */ Float def,
        /* [in] */ Int32 userHandle,
        /* [out] */ Float* value);

    /**
     * Convenience function for retrieving a single secure settings value
     * as a float.  Note that internally setting values are always
     * stored as strings; this function converts the string to a float
     * for you.
     * <p>
     * This version does not take a default value.  If the setting has not
     * been set, or the string value is not a number,
     * it throws {@link SettingNotFoundException}.
     *
     * @param cr The ContentResolver to access.
     * @param name The name of the setting to retrieve.
     *
     * @throws SettingNotFoundException Thrown if a setting by the given
     * name can't be found or the setting value is not a float.
     *
     * @return The setting's current value.
     */
    CARAPI GetFloat(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& name,
        /* [out] */ Float* value);

    /** @hide */
    CARAPI GetFloatForUser(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& name,
        /* [in] */ Int32 userHandle,
        /* [out] */ Float* value);

    /**
     * Convenience function for updating a single settings value as a
     * floating point number. This will either create a new entry in the
     * table if the given name does not exist, or modify the value of the
     * existing row with that name.  Note that internally setting values
     * are always stored as strings, so this function converts the given
     * value to a string before storing it.
     *
     * @param cr The ContentResolver to access.
     * @param name The name of the setting to modify.
     * @param value The new value for the setting.
     * @return true if the value was set, false on database errors
     */
    CARAPI PutFloat(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& name,
        /* [in] */ Float value,
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI PutFloatForUser(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& name,
        /* [in] */ Float value,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

    /**
     * Helper method for determining if a location provider is enabled.
     * @param cr the content resolver to use
     * @param provider the location provider to query
     * @return true if the provider is enabled
     */
    CARAPI IsLocationProviderEnabled(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& provider,
        /* [out] */ Boolean* result);

    /**
     * Helper method for determining if a location provider is enabled.
     * @param cr the content resolver to use
     * @param provider the location provider to query
     * @param userId the userId to query
     * @return true if the provider is enabled
     * @hide
     */
    CARAPI IsLocationProviderEnabledForUser(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& provider,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    /**
     * Thread-safe method for enabling or disabling a single location provider.
     * @param cr the content resolver to use
     * @param provider the location provider to enable or disable
     * @param enabled true if the provider should be enabled
     */
    CARAPI SetLocationProviderEnabled(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& provider,
        /* [in] */ Boolean enabled);

    /**
     * Thread-safe method for enabling or disabling a single location provider.
     * @param cr the content resolver to use
     * @param provider the location provider to enable or disable
     * @param enabled true if the provider should be enabled
     * @param userId the userId for which to enable/disable providers
     * @hide
     */
    CARAPI SetLocationProviderEnabledForUser(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& provider,
        /* [in] */ Boolean enabled,
        /* [in] */ Int32 userId);

    CARAPI GetNAVIGATION_RING_TARGETS(
        /* [out, callee] */ ArrayOf<String>** targets);

    CARAPI GetCLONE_TO_MANAGED_PROFILE(
        /* [out, callee] */ ArrayOf<String>** array);

    CARAPI GetSETTINGS_TO_BACKUP(
        /* [out, callee] */ ArrayOf<String>** backups);

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    CARAPI SetLocationProviderEnabledForUser(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& provider,
        /* [in] */ Boolean enabled,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    CARAPI SetLocationModeForUser(
        /* [in] */ IContentResolver* cr,
        /* [in] */ Int32 mode,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    CARAPI GetLocationModeForUser(
        /* [in] */ IContentResolver* cr,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* mode);
};

} //namespace Provider
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_PROVIDER_CSETTINGSSECURE_H__
