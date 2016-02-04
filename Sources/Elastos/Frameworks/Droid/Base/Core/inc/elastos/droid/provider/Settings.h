#ifndef __ELASTOS_DROID_PROVIDER_SETTINGS_H__
#define __ELASTOS_DROID_PROVIDER_SETTINGS_H__

#include <elastos/droid/ext/frameworkext.h>
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIContentProvider;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Internal::Widget::IILockSettings;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Privacy::IPrivacySettingsManager;
using Elastos::Core::ICharSequence;
using Elastos::Core::Object;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IHashSet;

namespace Elastos {
namespace Droid {
namespace Provider {

class ECO_PUBLIC Settings
{
private:
    // Thread-safe.
    class ECO_LOCAL NameValueCache : public Object
    {
    public:
        NameValueCache(
            /* [in] */ const String& versionSystemProperty,
            /* [in] */ IUri* uri,
            /* [in] */ const String& getCommand,
            /* [in] */ const String& setCommand);

        CARAPI PutStringForUser(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [in] */ const String& value,
            /* [in] */ Int32 userHandle,
            /* [out] */ Boolean* result);

        CARAPI GetStringForUser(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [in] */ Int32 userHandle,
            /* [out] */ String* value);

    private:
        CARAPI LazyGetProvider(
            /* [in] */ IContentResolver* cr,
            /* [out] */ IIContentProvider** provider);

    private:
        const String mVersionSystemProperty;
        const AutoPtr<IUri> mUri;
        static const AutoPtr< ArrayOf<String> > SELECT_VALUE;
        static const String NAME_EQ_PLACEHOLDER;
        // Must synchronize on 'this' to access mValues and mValuesVersion.
        AutoPtr<IHashMap> mValues;

        Int64 mValuesVersion;

        AutoPtr<IIContentProvider> mContentProvider;

        // The method we'll call (or null, to not use) on the provider
        // for the fast path of retrieving settings.
        const String mCallGetCommand;
        const String mCallSetCommand;
        static Object sLock;
    };

public:
    class NameValueTable
    {
    public:
        static CARAPI GetUriFor(
            /* [in] */ IUri* uri,
            /* [in] */ const String& name,
            /* [out] */ IUri** value);

    protected:
        static CARAPI_(Boolean) PutString(
            /* [in] */ IContentResolver* resolver,
            /* [in] */ IUri* uri,
            /* [in] */ const String& name,
            /* [in] */ const String& value);
    };

    class System : public NameValueTable
    {
    public:
        /** @hide */
        static CARAPI GetMovedKeys(
            /* [in] */ IHashSet* outKeySet);

        /** @hide */
        static CARAPI GetNonLegacyMovedKeys(
            /* [in] */ IHashSet* outKeySet);

        static const AutoPtr< ArrayOf<String> > SETTINGS_TO_BACKUP;

        /**
         * Look up a name in the database.
         * @param resolver to access the database with
         * @param name to look up in the table
         * @return the corresponding value, or null if not present
         */
        static CARAPI GetString(
            /* [in] */ IContentResolver* resolver,
            /* [in] */ const String& name,
            /* [out] */ String* value);

        /** @hide */
        static CARAPI GetStringForUser(
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
        static CARAPI PutString(
            /* [in] */ IContentResolver* resolver,
            /* [in] */ const String& name,
            /* [in] */ const String& value,
            /* [out] */ Boolean* result);

        static CARAPI PutStringForUser(
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
        static CARAPI GetUriFor(
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
        static CARAPI GetInt32(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [in] */ Int32 def,
            /* [out] */ Int32* value);

        static CARAPI GetInt32ForUser(
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
        static CARAPI GetInt32(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [out] */ Int32* value);

        /** @hide */
        static CARAPI GetInt32ForUser(
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
        static CARAPI PutInt32(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [in] */ Int32 value,
            /* [out] */ Boolean* result);

        /** @hide */
        static CARAPI PutInt32ForUser(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [in] */ Int32 value,
            /* [in] */ Int32 userHandle,
            /* [out] */ Boolean* result);

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
        static CARAPI GetInt64(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [in] */ Int64 def,
            /* [out] */ Int64* value);

        static CARAPI GetInt64ForUser(
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
        static CARAPI GetInt64(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [out] */ Int64* value);

        static CARAPI GetInt64ForUser(
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
        static CARAPI PutInt64(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [in] */ Int64 value,
            /* [out] */ Boolean* result);

        static CARAPI PutInt64ForUser(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [in] */ Int64 value,
            /* [in] */ Int32 userHandle,
            /* [out] */ Boolean* result);

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
        static CARAPI GetFloat(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [in] */ Float def,
            /* [out] */ Float* value);

        static CARAPI GetFloatForUser(
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
        static CARAPI GetFloat(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [out] */ Float* value);

        static CARAPI GetFloatForUser(
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
        static CARAPI PutFloat(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [in] */ Float value,
            /* [out] */ Boolean* result);

        static CARAPI PutFloatForUser(
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
        static CARAPI GetConfiguration(
            /* [in] */ IContentResolver* cr,
            /* [in] */ IConfiguration* outConfig);

        static CARAPI GetConfigurationForUser(
            /* [in] */ IContentResolver* cr,
            /* [in] */ IConfiguration* outConfig,
            /* [in] */ Int32 userHandle);

        static CARAPI_(void) ClearConfiguration(
            /* [in] */ IConfiguration* inoutConfig);

        /**
         * Convenience function to write a batch of configuration-related
         * settings from a {@link Configuration} object.
         *
         * @param cr The ContentResolver to access.
         * @param config The settings to write.
         * @return true if the values were set, false on database errors
         */
        static CARAPI PutConfiguration(
            /* [in] */ IContentResolver* cr,
            /* [in] */ IConfiguration* config,
            /* [out] */ Boolean* result);

        /** @hide */
        static CARAPI PutConfigurationForUser(
            /* [in] */ IContentResolver* cr,
            /* [in] */ IConfiguration* config,
            /* [in] */ Int32 userHandle,
            /* [out] */ Boolean* result);

        /** @hide */
        static CARAPI_(Boolean) HasInterestingConfigurationChanges(
            /* [in] */ Int32 changes);

        static CARAPI GetShowGTalkServiceStatus(
            /* [in] */ IContentResolver* cr,
            /* [out] */ Boolean* result);

        static CARAPI GetShowGTalkServiceStatusForUser(
            /* [in] */ IContentResolver* cr,
            /* [in] */ Int32 userHandle,
            /* [out] */ Boolean* result);

        static CARAPI SetShowGTalkServiceStatus(
            /* [in] */ IContentResolver* cr,
            /* [in] */ Boolean flag);

        /**
         * @hide
         * @deprecated - Do not use
         */
        //@Deprecated
        static CARAPI SetShowGTalkServiceStatusForUser(
            /* [in] */ IContentResolver* cr,
            /* [in] */ Boolean flag,
            /* [in] */ Int32 userHandle);

    private:
        ECO_LOCAL static CARAPI_(Boolean) InitMOVEDTOSECURE();

        ECO_LOCAL static CARAPI_(Boolean) InitMOVEDTOGLOBALANDSECURE();

    public:
        /**
         * The content:// style URL for this table
         */
        static const AutoPtr<IUri> CONTENT_URI;

        /**
         * A {@link Uri} that will point to the current default ringtone at any
         * given time.
         * <p>
         * If the current default ringtone is in the DRM provider and the caller
         * does not have permission, the exception will be a
         * FileNotFoundException.
         */
        static const AutoPtr<IUri> DEFAULT_RINGTONE_URI;

        /**
         * A {@link Uri} that will point to the current default notification
         * sound at any given time.
         *
         * @see #DEFAULT_RINGTONE_URI
         */
        static const AutoPtr<IUri> DEFAULT_NOTIFICATION_URI;

        /**
         * A {@link Uri} that will point to the current default alarm alert at
         * any given time.
         *
         * @see #DEFAULT_ALARM_ALERT_URI
         */
        static const AutoPtr<IUri> DEFAULT_ALARM_ALERT_URI;

        /**
         * The mapping of stream type (integer) to its setting.
         */
        static const AutoPtr< ArrayOf<String> > VOLUME_SETTINGS;

    private:
        ECO_LOCAL static const AutoPtr<NameValueCache> sNameValueCache;
        ECO_LOCAL static const AutoPtr<IHashSet> MOVED_TO_SECURE;
        ECO_LOCAL static const AutoPtr<IHashSet> MOVED_TO_GLOBAL;
        ECO_LOCAL static const AutoPtr<IHashSet> MOVED_TO_SECURE_THEN_GLOBAL;
    };

    class Secure : public NameValueTable
    {
    public:
        /** @hide */
        static CARAPI GetMovedKeys(
            /* [in] */ IHashSet* outKeySet);

        /**
         * Look up a name in the database.
         * @param resolver to access the database with
         * @param name to look up in the table
         * @return the corresponding value, or null if not present
         */
        static CARAPI GetString(
            /* [in] */ IContentResolver* resolver,
            /* [in] */ const String& name,
            /* [out] */ String* value);

        static CARAPI GetStringForUser(
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
        static CARAPI PutString(
            /* [in] */ IContentResolver* resolver,
            /* [in] */ const String& name,
            /* [in] */ const String& value,
            /* [out] */ Boolean* result);

        /** @hide */
        static CARAPI PutStringForUser(
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
        static CARAPI GetUriFor(
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
        static CARAPI GetInt32(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [in] */ Int32 def,
            /* [out] */ Int32* value);

        static CARAPI GetInt32ForUser(
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
        static CARAPI GetInt32(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [out] */ Int32* value);

        static CARAPI GetInt32ForUser(
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
        static CARAPI PutInt32(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [in] */ Int32 value,
            /* [out] */ Boolean* result);

        static CARAPI PutInt32ForUser(
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
        static CARAPI GetInt64(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [in] */ Int64 def,
            /* [out] */ Int64* value);

        static CARAPI GetInt64ForUser(
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
        static CARAPI GetInt64(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [out] */ Int64* value);

        static CARAPI GetInt64ForUser(
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
        static CARAPI PutInt64(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [in] */ Int64 value,
            /* [out] */ Boolean* result);

        static CARAPI PutInt64ForUser(
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
        static CARAPI GetFloat(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [in] */ Float def,
            /* [out] */ Float* value);

        static CARAPI GetFloatForUser(
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
        static CARAPI GetFloat(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [out] */ Float* value);

        static CARAPI GetFloatForUser(
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
        static CARAPI PutFloat(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [in] */ Float value,
            /* [out] */ Boolean* result);

        static CARAPI PutFloatForUser(
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
        static CARAPI IsLocationProviderEnabled(
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
        static CARAPI IsLocationProviderEnabledForUser(
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
        static CARAPI SetLocationProviderEnabled(
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
        // @Deprecated
        static CARAPI SetLocationProviderEnabledForUser(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& provider,
            /* [in] */ Boolean enabled,
            /* [in] */ Int32 userId,
            /* [out] */ Boolean* result);

    private:
        ECO_LOCAL static CARAPI_(Boolean) InitHashSet();

        ECO_LOCAL static Boolean SetLocationModeForUser(
            /* [in] */ IContentResolver* cr,
            /* [in] */ Int32 mode,
            /* [in] */ Int32 userId);

        ECO_LOCAL static Int32 GetLocationModeForUser(
            /* [in] */ IContentResolver* cr,
            /* [in] */ Int32 userId);

    public:
        static const AutoPtr<IUri> CONTENT_URI;

        /**
         * This are the settings to be backed up.
         *
         * NOTE: Settings are backed up and restored in the order they appear
         *       in this array. If you have one setting depending on another,
         *       make sure that they are ordered appropriately.
         *
         * @hide
         */
        static const AutoPtr< ArrayOf<String> > SETTINGS_TO_BACKUP;

        static const AutoPtr<ArrayOf<String> > CLONE_TO_MANAGED_PROFILE;

    private:
        // Populated lazily, guarded by class object:
        ECO_LOCAL static const AutoPtr<NameValueCache> sNameValueCache;
        //assert(0 && "TODO");
        ECO_LOCAL static AutoPtr<IILockSettings> sLockSettings;

        ECO_LOCAL static Boolean sIsSystemProcess;
        ECO_LOCAL static const AutoPtr<IHashSet> MOVED_TO_LOCK_SETTINGS;
        ECO_LOCAL static const AutoPtr<IHashSet> MOVED_TO_GLOBAL;
        ECO_LOCAL static Object sSecureLock;
    };

    class Global : public NameValueTable
    {
    public:
        static CARAPI ZenModeToString(
            /* [in] */ Int32 mode,
            /* [out] */ String* key);

        static CARAPI GetBluetoothMapPriorityKey(
            /* [in] */ const String& address,
            /* [out] */ String* result);

        /**
         * Get the key that retrieves a bluetooth headset's priority.
         * @hide
         */
        static CARAPI_(String) GetBluetoothHeadsetPriorityKey(
            /* [in] */ const String& address);

        /**
         * Get the key that retrieves a bluetooth a2dp sink's priority.
         * @hide
         */
        static CARAPI_(String) GetBluetoothA2dpSinkPriorityKey(
            /* [in] */ const String& address);

        /**
         * Get the key that retrieves a bluetooth a2dp sink's priority.
         * @hide
         */
        static CARAPI_(String) GetBluetoothInputDevicePriorityKey(
            /* [in] */ const String& address);

        /**
         * Look up a name in the database.
         * @param resolver to access the database with
         * @param name to look up in the table
         * @return the corresponding value, or null if not present
         */
        static CARAPI GetString(
            /* [in] */ IContentResolver* resolver,
            /* [in] */ const String& name,
            /* [out] */ String* value);

        /** @hide */
        static CARAPI GetStringForUser(
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
        static CARAPI PutString(
            /* [in] */ IContentResolver* resolver,
            /* [in] */ const String& name,
            /* [in] */ const String& value,
            /* [out] */ Boolean* result);

        /** @hide */
        static CARAPI PutStringForUser(
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
        static CARAPI GetUriFor(
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
        static CARAPI GetInt32(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [in] */ Int32 def,
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
        static CARAPI GetInt32(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
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
        static CARAPI PutInt32(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [in] */ Int32 value,
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
        static CARAPI GetInt64(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [in] */ Int64 def,
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
        static CARAPI GetInt64(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
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
        static CARAPI PutInt64(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [in] */ Int64 value,
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
        static CARAPI GetFloat(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [in] */ Float def,
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
        static CARAPI GetFloat(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
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
        static CARAPI PutFloat(
            /* [in] */ IContentResolver* cr,
            /* [in] */ const String& name,
            /* [in] */ Float value,
            /* [out] */ Boolean* result);

    public:
        static const AutoPtr<IUri> CONTENT_URI;

        /**
         * Settings to backup. This is here so that it's in the same place as the settings
         * keys and easy to update.
         *
         * These keys may be mentioned in the SETTINGS_TO_BACKUP arrays in System
         * and Secure as well.  This is because those tables drive both backup and
         * restore, and restore needs to properly whitelist keys that used to live
         * in those namespaces.  The keys will only actually be backed up / restored
         * if they are also mentioned in this table (Global.SETTINGS_TO_BACKUP).
         *
         * NOTE: Settings are backed up and restored in the order they appear
         *       in this array. If you have one setting depending on another,
         *       make sure that they are ordered appropriately.
         *
         * @hide
         */
        static const AutoPtr<ArrayOf<String> > SETTINGS_TO_BACKUP;
        static const AutoPtr<ArrayOf<String> > MULTI_SIM_USER_PREFERRED_SUBS;

    private:
        // Populated lazily, guarded by class object:
        ECO_LOCAL static const AutoPtr<NameValueCache> sNameValueCache;
        ECO_LOCAL static const AutoPtr<IHashSet> MOVED_TO_SECURE;
    };

    class Bookmarks
    {
    public:
       /**
        * Convenience function to retrieve the bookmarked Intent for a
        * particular shortcut key.
        *
        * @param cr The ContentResolver to query.
        * @param shortcut The shortcut key.
        *
        * @return Intent The bookmarked URL, or null if there is no bookmark
        *         matching the given shortcut.
        */
        static CARAPI GetIntentForShortcut(
            /* [in] */ IContentResolver* cr,
            /* [in] */ Char32 shortcuts,
            /* [out] */ IIntent** intent);

       /**
        * Add a new bookmark to the system.
        *
        * @param cr The ContentResolver to query.
        * @param intent The desired target of the bookmark.
        * @param title Bookmark title that is shown to the user; null if none
        *            or it should be resolved to the intent's title.
        * @param folder Folder in which to place the bookmark; null if none.
        * @param shortcut Shortcut that will invoke the bookmark; 0 if none. If
        *            this is non-zero and there is an existing bookmark entry
        *            with this same shortcut, then that existing shortcut is
        *            cleared (the bookmark is not removed).
        * @return The unique content URL for the new bookmark entry.
        */
        static CARAPI Add(
            /* [in] */ IContentResolver* cr,
            /* [in] */ IIntent* intent,
            /* [in] */ const String& title,
            /* [in] */ const String& folder,
            /* [in] */ Char32 shortcut,
            /* [in] */ Int32 ordering,
            /* [out] */ IUri** uri);

       /**
        * Return the folder name as it should be displayed to the user.  This
        * takes care of localizing special folders.
        *
        * @param r Resources object for current locale; only need access to
        *          system resources.
        * @param folder The value found in the {@link #FOLDER} column.
        *
        * @return CharSequence The label for this folder that should be shown
        *         to the user.
        */
        static CARAPI_(AutoPtr<ICharSequence>) GetLabelForFolder(
            /* [in] */ IResources* r,
            /* [in] */ const String& folder);

       /**
        * Return the title as it should be displayed to the user. This takes
        * care of localizing bookmarks that point to activities.
        *
        * @param context A context.
        * @param cursor A cursor pointing to the row whose title should be
        *        returned. The cursor must contain at least the {@link #TITLE}
        *        and {@link #INTENT} columns.
        * @return A title that is localized and can be displayed to the user,
        *         or the empty string if one could not be found.
        */
        static CARAPI GetTitle(
            /* [in] */ IContext* context,
            /* [in] */ ICursor* cursor,
            /* [out] */ ICharSequence** title);

        public:
            static AutoPtr<IUri> CONTENT_URI;

    private:
        ECO_LOCAL static const String TAG;
        ECO_LOCAL static const AutoPtr< ArrayOf<String> > sIntentProjection;
        ECO_LOCAL static const AutoPtr< ArrayOf<String> > sShortcutProjection;
        ECO_LOCAL static const String sShortcutSelection;
    };

public:
    /**
     * Returns the device ID that we should use when connecting to the mobile gtalk server.
     * This is a string like "android-0x1242", where the hex string is the Android ID obtained
     * from the GoogleLoginService.
     *
     * @param androidId The Android ID for this device.
     * @return The device ID that should be used when connecting to the mobile gtalk server.
     * @hide
     */
    static CARAPI_(String) GetGTalkDeviceId(
        /* [in] */ Int64 androidId);

private:
    ECO_LOCAL static const String JID_RESOURCE_PREFIX;
    ECO_LOCAL static const String TAG;
    ECO_LOCAL static const Boolean LOCAL_LOGV;
    ECO_LOCAL static Object sLocationSettingsLock;
};

} //namespace Provider
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_PROVIDER_SETTINGS_H__
