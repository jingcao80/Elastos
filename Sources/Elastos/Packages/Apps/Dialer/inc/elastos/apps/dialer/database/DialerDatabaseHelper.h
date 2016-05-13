#ifndef __ELASTOS_APPS_DIALER_DATABASE_DIALERDATABASEHELPER_H__
#define __ELASTOS_APPS_DIALER_DATABASE_DIALERDATABASEHELPER_H__

#include "SQLiteOpenHelper.h"

using Elastos::Droid::Database::Sqlite::SQLiteOpenHelper;

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Database {

class DialerDatabaseHelper
    : public SQLiteOpenHelper
    , public IDialerDatabaseHelper
{
public:
    class Tables
    {
    public:
        /** Saves the necessary smart dial information of all contacts. */
        static const String SMARTDIAL_TABLE; // = "smartdial_table";
        /** Saves all possible prefixes to refer to a contacts.*/
        static const String PREFIX_TABLE; // = "prefix_table";
        /** Database properties for internal use */
        static const String PROPERTIES; // = "properties";
    };

    class SmartDialDbColumns
    {
    public:
        static const String _ID; // = "id";
        static const String DATA_ID; // = "data_id";
        static const String NUMBER; // = "phone_number";
        static const String CONTACT_ID; // = "contact_id";
        static const String LOOKUP_KEY; // = "lookup_key";
        static const String DISPLAY_NAME_PRIMARY; // = "display_name";
        static const String PHOTO_ID; // = "photo_id";
        static const String LAST_TIME_USED; // = "last_time_used";
        static const String TIMES_USED; // = "times_used";
        static const String STARRED; // = "starred";
        static const String IS_SUPER_PRIMARY; // = "is_super_primary";
        static const String IN_VISIBLE_GROUP; // = "in_visible_group";
        static const String IS_PRIMARY; // = "is_primary";
        static const String LAST_SMARTDIAL_UPDATE_TIME; // = "last_smartdial_update_time";
    };

    class PrefixColumns
    {
    public:
        static const String PREFIX; // = "prefix";
        static const String CONTACT_ID; // = "contact_id";
    };

    class PropertiesColumns
    {
    public:
        static const AutoPtr<IUri> URI;
        static const String PROJECTION[];

        static const Int32 PHONE_ID; // = 0;
        static const Int32 PHONE_TYPE; // = 1;
        static const Int32 PHONE_LABEL; // = 2;
        static const Int32 PHONE_NUMBER; // = 3;
        static const Int32 PHONE_CONTACT_ID; // = 4;
        static const Int32 PHONE_LOOKUP_KEY; // = 5;
        static const Int32 PHONE_DISPLAY_NAME; // = 6;
        static const Int32 PHONE_PHOTO_ID; // = 7;
        static const Int32 PHONE_LAST_TIME_USED; // = 8;
        static const Int32 PHONE_TIMES_USED; // = 9;
        static const Int32 PHONE_STARRED; // = 10;
        static const Int32 PHONE_IS_SUPER_PRIMARY; // = 11;
        static const Int32 PHONE_IN_VISIBLE_GROUP; // = 12;
        static const Int32 PHONE_IS_PRIMARY; // = 13;

        /** Selects only rows that have been updated after a certain time stamp.*/
        static const String SELECT_UPDATED_CLAUSE; // =
                // Phone.CONTACT_LAST_UPDATED_TIMESTAMP + " > ?";

        /** Ignores contacts that have an unreasonably long lookup key. These are likely to be
         * the result of multiple (> 50) merged raw contacts, and are likely to cause
         * OutOfMemoryExceptions within SQLite, or cause memory allocation problems later on
         * when iterating through the cursor set (see b/13133579)
         */
        static const String SELECT_IGNORE_LOOKUP_KEY_TOO_LONG_CLAUSE; // =
                // "length(" + Phone.LOOKUP_KEY + ") < 1000";

        static const String SELECTION; // = SELECT_UPDATED_CLAUSE + " AND " +
                // SELECT_IGNORE_LOOKUP_KEY_TOO_LONG_CLAUSE;
    };

    /** Query options for querying the deleted contact database.*/
    class DeleteContactQuery
    {
    public:
        static const AutoPtr<IUri> URI; // = ContactsContract.DeletedContacts.CONTENT_URI;

        static const String PROJECTION[];

        static const Int32 DELETED_CONTACT_ID; // = 0;
        static const Int32 DELECTED_TIMESTAMP; // = 1;
    };

    /**
     * Simple data format for a contact, containing only information needed for showing up in
     * smart dial interface.
     */
    class ContactNumber
        : public Object
    {
    public:
        ContactNumber(
            /* [in] */ Int64 id,
            /* [in] */ Int64 dataID,
            /* [in] */ const String& displayName,
            /* [in] */ const String& phoneNumber,
            /* [in] */ const String& lookupKey,
            /* [in] */ Int64 photoId);

        // @Override
        CARAPI HashCode(
            /* [out] */ Int32* hashCode);

        // @Override
        CARAPI Equals(
            /* [in] */ IInterface* other,
            /* [out] */ Boolean* result);
    public:
        Int64 mId;
        Int64 mDataId;
        String mDisplayName;
        String mPhoneNumber;
        String mLookupKey;
        Int64 mPhotoId;
    };

private:
    /**
     * Gets the sorting order for the smartdial table. This computes a SQL "ORDER BY" argument by
     * composing contact status and recent contact details together.
     */
    class SmartDialSortingOrder
    {
        /** Current contacts - those contacted within the last 3 days (in milliseconds) */
        static const Int64 LAST_TIME_USED_CURRENT_MS; // = 3L * 24 * 60 * 60 * 1000;
        /** Recent contacts - those contacted within the last 30 days (in milliseconds) */
        static const Int64 LAST_TIME_USED_RECENT_MS; // = 30L * 24 * 60 * 60 * 1000;

        /** Time since last contact. */
        static const String TIME_SINCE_LAST_USED_MS; // = "( ?1 - " +
                // Tables.SMARTDIAL_TABLE + "." + SmartDialDbColumns.LAST_TIME_USED + ")";

        /** Contacts that have been used in the past 3 days rank higher than contacts that have
         * been used in the past 30 days, which rank higher than contacts that have not been used
         * in recent 30 days.
         */
        static const String SORT_BY_DATA_USAGE;

        /** This sort order is similar to that used by the ContactsProvider when returning a list
         * of frequently called contacts.
         */
        static const String SORT_ORDER;
    };

    /**
     * Data format for finding duplicated contacts.
     */
    class ContactMatch
        : public Object
    {
    public:
        ContactMatch(
            /* [in] */ String lookupKey,
            /* [in] */ Int64 id);

        // @Override
        CARAPI HashCode(
            /* [out] */ Int32* hashCode);

        // @Override
        CARAPI Equals(
            /* [in] */ IInterface* other,
            /* [out] */ Boolean* result);
    private:
        String mLookupKey;
        Int64 mId;
    };

    class SmartDialUpdateAsyncTask
        : public AsyncTask
    {
    public:
        SmartDialUpdateAsyncTask(
            /* [in] */ DialerDatabaseHelper* host);

    protected:
        // @Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

        // @Override
        CARAPI OnCancelled();

        // @Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* result);
    private:
        DialerDatabaseHelper* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * Access function to get the singleton instance of DialerDatabaseHelper.
     */
    static AutoPtr<IDialerDatabaseHelper> GetInstance(
        /* [in] */ IContext* context);

    /**
     * Returns a new instance for unit tests. The database will be created in memory.
     */
    // @VisibleForTesting
    static AutoPtr<IDialerDatabaseHelper> GetNewInstanceForTest(
        /* [in] */ IContext* context);

    /**
     * Creates tables in the database when database is created for the first time.
     *
     * @param db The database.
     */
    // @Override
    CARAPI OnCreate(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) DropTables(
        /* [in] */ ISQLiteDatabase* db);

    // @Override
    CARAPI OnUpgrade(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ Int32 oldVersion,
        /* [in] */ Int32 newVersion);

    /**
     * Stores a key-value pair in the {@link Tables#PROPERTIES} table.
     */
    CARAPI SetProperty(
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    CARAPI SetProperty(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    /**
     * Returns the value from the {@link Tables#PROPERTIES} table.
     */
    CARAPI GetProperty(
        /* [in] */ const String& key,
        /* [in] */ const String& defaultValue,
        /* [out] */ String* property);

    CARAPI GetProperty(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& key,
        /* [in] */ const String& defaultValue,
        /* [out] */ String* property);

    CARAPI GetPropertyAsInt(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& key,
        /* [in] */ Int32 defaultValue,
        /* [out] */ Int32* property);

    /**
     * Starts the database upgrade process in the background.
     */
    CARAPI StartSmartDialUpdateThread();

    /**
     * Removes all entries in the smartdial contact database.
     */
    // @VisibleForTesting
    CARAPI_(void) RemoveAllContacts(
        /* [in] */ ISQLiteDatabase* db);

    /**
     * Counts number of rows of the prefix table.
     */
    // @VisibleForTesting
    CARAPI_(Int32) CountPrefixTableRows(
        /* [in] */ ISQLiteDatabase* db);

    /**
     * Inserts prefixes of contact names to the prefix table.
     *
     * @param db Database pointer to the smartdial database.
     * @param nameCursor Cursor pointing to the list of distinct updated contacts.
     */
    // @VisibleForTesting
    CARAPI_(void) InsertNamePrefixes(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ ICursor* nameCursor);

    /**
     * Updates the smart dial and prefix database.
     * This method queries the Delta API to get changed contacts since last update, and updates the
     * records in smartdial database and prefix database accordingly.
     * It also queries the deleted contact database to remove newly deleted contacts since last
     * update.
     */
    CARAPI UpdateSmartDialDatabase();

    /**
     * Returns a list of candidate contacts where the query is a prefix of the dialpad index of
     * the contact's name or phone number.
     *
     * @param query The prefix of a contact's dialpad index.
     * @return A list of top candidate contacts that will be suggested to user to match their input.
     */
    CARAPI GetLooseMatches(
        /* [in] */ const String& query,
        /* [in] */ ISmartDialNameMatcher* nameMatcher,
        /* [out] */ IArrayList** result);

protected:
    DialerDatabaseHelper(
        /* [in] */ IContext* context,
        /* [in] */ const String& databaseName);

    DialerDatabaseHelper(
        /* [in] */ IContext* context,
        /* [in] */ const String& databaseName,
        /* [in] */ Int32 dbVersion);

    /**
     * Inserts updated contacts as rows to the smartdial table.
     *
     * @param db Database pointer to the smartdial database.
     * @param updatedContactCursor Cursor pointing to the list of recently updated contacts.
     * @param currentMillis Current time to be recorded in the smartdial table as update timestamp.
     */
    // @VisibleForTesting
    CARAPI_(void) InsertUpdatedContactsAndNumberPrefix(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ ICursor* updatedContactCursor,
        /* [in] */ IInteger64* currentMillis);

private:
    CARAPI_(void) SetupTables(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) ResetSmartDialLastUpdatedTime();

    /**
     * Removes rows in the smartdial database that matches the contacts that have been deleted
     * by other apps since last update.
     *
     * @param db Database pointer to the dialer database.
     * @param last_update_time Time stamp of last update on the smartdial database
     */
    CARAPI_(void) RemoveDeletedContacts(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& last_update_time);

    /**
     * Removes potentially corrupted entries in the database. These contacts may be added before
     * the previous instance of the dialer was destroyed for some reason. For data integrity, we
     * delete all of them.

     * @param db Database pointer to the dialer database.
     * @param last_update_time Time stamp of last successful update of the dialer database.
     */
    CARAPI_(void) RemovePotentiallyCorruptedContacts(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& last_update_time);

    /**
     * Removes rows in the smartdial database that matches updated contacts.
     *
     * @param db Database pointer to the smartdial database
     * @param updatedContactCursor Cursor pointing to the list of recently updated contacts.
     */
    CARAPI_(void) RemoveUpdatedContacts(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ ICursor* updatedContactCursor);

private:
    static const String TAG; // = "DialerDatabaseHelper";
    static const Boolean DEBUG; // = false;

    static AutoPtr<IDialerDatabaseHelper> sSingleton;

    static Object mLock;
    static const AutoPtr<IAtomicBoolean> sInUpdate; // = new AtomicBoolean(false);
    AutoPtr<IContext> mContext;

    /**
     * Saves the last update time of smart dial databases to shared preferences.
     */
    static const String DATABASE_LAST_CREATED_SHARED_PREF; // = "com.android.dialer";
    static const String LAST_UPDATED_MILLIS; // = "last_updated_millis";
    static const String DATABASE_VERSION_PROPERTY; // = "database_version";

    static const Int32 MAX_ENTRIES; // = 20;
};

} // Database
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_DATABASE_DIALERDATABASEHELPER_H__
