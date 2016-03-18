#ifndef __ELASTOS_DROID_PROVIDER_CSETTINGSSYSTEM_H__
#define __ELASTOS_DROID_PROVIDER_CSETTINGSSYSTEM_H__

#include "_Elastos_Droid_Provider_CSettingsSystem.h"
#include "elastos/droid/provider/Settings.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CSettingsSystem)
    , public Singleton
    , public ISettingsSystem
    , public ISettingsNameValueTable
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetUriFor(
        /* [in] */ IUri* u,
        /* [in] */ const String& name,
        /* [out] */ IUri** value);

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    CARAPI GetSETTINGS_TO_BACKUP(
        /* [out] */ ArrayOf<String>** array);

    CARAPI GetDEFAULT_RINGTONE_URI(
        /* [out] */ IUri** uri);

    CARAPI GetDEFAULT_NOTIFICATION_URI(
        /* [out] */ IUri** uri);

    CARAPI GetDEFAULT_ALARM_ALERT_URI(
        /* [out] */ IUri** uri);

    CARAPI GetVOLUME_SETTINGS(
        /* [out, callee] */ ArrayOf<String>** settings);

    /** @hide */
    CARAPI GetMovedKeys(
        /* [in] */ IHashSet* outKeySet);

    /** @hide */
    CARAPI GetNonLegacyMovedKeys(
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
        /* [out] */ Boolean* res);

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
     * Convenience function for retrieving a single system settings value
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

    CARAPI GetInt32ForUser(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& name,
        /* [in] */ Int32 def,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* value);

    /**
     * Convenience function for retrieving a single system settings value
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
        /* [out] */ Boolean* res);

    /**
     * Convenience function for retrieving a single system settings value
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

    CARAPI GetInt64ForUser(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& name,
        /* [in] */ Int64 def,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int64* value);

    /**
     * Convenience function for retrieving a single system settings value
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
     * Convenience function for updating a single settings value as a long
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

    CARAPI PutInt64ForUser(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& name,
        /* [in] */ Int64 value,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* res);

    /**
     * Convenience function for retrieving a single system settings value
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
     * Convenience function for retrieving a single system settings value
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
     * Convenience function to read all of the current
     * configuration-related settings into a
     * {@link Configuration} object.
     *
     * @param cr The ContentResolver to access.
     * @param outConfig Where to place the configuration settings.
     */
    CARAPI GetConfiguration(
        /* [in] */ IContentResolver* cr,
        /* [in] */ IConfiguration* outConfig);

    CARAPI GetConfigurationForUser(
        /* [in] */ IContentResolver* cr,
        /* [in] */ IConfiguration* outConfig,
        /* [in] */ Int32 userHandle);

    /**
     * @hide Erase the fields in the Configuration that should be applied
     * by the settings.
     */
    CARAPI ClearConfiguration(
        /* [in] */ IConfiguration* inoutConfig);

    /**
     * Convenience function to write a batch of configuration-related
     * settings from a {@link Configuration} object.
     *
     * @param cr The ContentResolver to access.
     * @param config The settings to write.
     * @return true if the values were set, false on database errors
     */
    CARAPI PutConfiguration(
        /* [in] */ IContentResolver* cr,
        /* [in] */ IConfiguration* config,
        /* [out] */ Boolean* result);

    CARAPI PutConfigurationForUser(
        /* [in] */ IContentResolver* cr,
        /* [in] */ IConfiguration* config,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI HasInterestingConfigurationChanges(
        /* [in] */ Int32 changes,
        /* [out] */ Boolean* result);

    CARAPI GetShowGTalkServiceStatus(
        /* [in] */ IContentResolver* cr,
        /* [out] */ Boolean* result);

    /**
     * @hide
     * @deprecated - Do not use
     */
    CARAPI GetShowGTalkServiceStatusForUser(
        /* [in] */ IContentResolver* cr,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

    CARAPI SetShowGTalkServiceStatus(
        /* [in] */ IContentResolver* cr,
        /* [in] */ Boolean flag);

    /**
     * @hide
     * @deprecated - Do not use
     */
    //@Deprecated
    CARAPI SetShowGTalkServiceStatusForUser(
        /* [in] */ IContentResolver* cr,
        /* [in] */ Boolean flag,
        /* [in] */ Int32 userHandle);
};

} //namespace Provider
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_PROVIDER_CSETTINGSSYSTEM_H__
