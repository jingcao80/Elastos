
#ifndef __ELASTOS_DROID_PROVIDER_TELEPHONY_MMSSMSDATABASEHELPER_H__
#define __ELASTOS_DROID_PROVIDER_TELEPHONY_MMSSMSDATABASEHELPER_H__

#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/sqlite/SQLiteOpenHelper.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::Database::Sqlite::SQLiteOpenHelper;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Telephony {

class MmsSmsDatabaseHelper: public SQLiteOpenHelper
{
public:
    class LowStorageMonitor: public BroadcastReceiver
    {
    public:
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);
    };

public:
    /**
     * Return a singleton helper for the combined MMS and SMS
     * database.
     */
    /* package */
    static CARAPI_(AutoPtr<MmsSmsDatabaseHelper>) GetInstance(
        /* [in] */ IContext* context);

    static CARAPI_(void) UpdateThread(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ Int64 thread_id);

    static CARAPI_(void) UpdateAllThreads(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* whereArgs);

    static CARAPI_(Int32) DeleteOneSms(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ Int32 message_id);

    // @Override
    CARAPI OnCreate(
        /* [in] */ ISQLiteDatabase* db);

    // @Override
    CARAPI OnUpgrade(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ Int32 oldVersion,
        /* [in] */ Int32 currentVersion);

    // @Override
    CARAPI GetWritableDatabase(
        /* [out] */ ISQLiteDatabase** result);

private:
    MmsSmsDatabaseHelper(
        /* [in] */ IContext* context);

    /**
     * Look through all the recipientIds referenced by the threads and then delete any
     * unreferenced rows from the canonical_addresses table.
     */
    static CARAPI RemoveUnferencedCanonicalAddresses(
        /* [in] */ ISQLiteDatabase* db);

    // When upgrading the database we need to populate the words
    // table with the rows out of sms and part.
    CARAPI_(void) PopulateWordsTable(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) CreateWordsTables(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) CreateIndices(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) CreateThreadIdIndex(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) CreateMmsTables(
        /* [in] */ ISQLiteDatabase* db);

    // Unlike the other trigger-creating functions, this function can be called multiple times
    // without harm.
    CARAPI CreateMmsTriggers(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) CreateSmsTables(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) CreateCommonTables(
        /* [in] */ ISQLiteDatabase* db);

    // TODO Check the query plans for these triggers.
    CARAPI_(void) CreateCommonTriggers(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) DropAll(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) UpgradeDatabaseToVersion41(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) UpgradeDatabaseToVersion42(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) UpgradeDatabaseToVersion43(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) UpgradeDatabaseToVersion44(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) UpgradeDatabaseToVersion45(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) UpgradeDatabaseToVersion46(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) UpgradeDatabaseToVersion47(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) UpgradeDatabaseToVersion48(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) UpgradeDatabaseToVersion51(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) UpgradeDatabaseToVersion53(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) UpgradeDatabaseToVersion54(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) UpgradeDatabaseToVersion55(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) UpgradeDatabaseToVersion56(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) UpgradeDatabaseToVersion57(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) UpgradeDatabaseToVersion58(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) UpgradeDatabaseToVersion59(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) UpgradeDatabaseToVersion60(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) UpgradeDatabaseToVersion61(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) UpgradeDatabaseToVersion62(
        /* [in] */ ISQLiteDatabase* db);

    // Determine whether a particular table has AUTOINCREMENT in its schema.
    CARAPI_(Boolean) HasAutoIncrement(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& tableName);

    // upgradeThreadsTableToAutoIncrement() is called to add the AUTOINCREMENT keyword to
    // the threads table. This could fail if the user has a lot of conversations and not enough
    // storage to make a copy of the threads table. That's ok. This upgrade is optional. It'll
    // be called again next time the device is rebooted.
    CARAPI UpgradeThreadsTableToAutoIncrement(
        /* [in] */ ISQLiteDatabase* db);

    // upgradeAddressTableToAutoIncrement() is called to add the AUTOINCREMENT keyword to
    // the canonical_addresses table. This could fail if the user has a lot of people they've
    // messaged with and not enough storage to make a copy of the canonical_addresses table.
    // That's ok. This upgrade is optional. It'll be called again next time the device is rebooted.
    CARAPI UpgradeAddressTableToAutoIncrement(
        /* [in] */ ISQLiteDatabase* db);

    // upgradePartTableToAutoIncrement() is called to add the AUTOINCREMENT keyword to
    // the part table. This could fail if the user has a lot of sound/video/picture attachments
    // and not enough storage to make a copy of the part table.
    // That's ok. This upgrade is optional. It'll be called again next time the device is rebooted.
    CARAPI UpgradePartTableToAutoIncrement(
        /* [in] */ ISQLiteDatabase* db);

    // upgradePduTableToAutoIncrement() is called to add the AUTOINCREMENT keyword to
    // the pdu table. This could fail if the user has a lot of mms messages
    // and not enough storage to make a copy of the pdu table.
    // That's ok. This upgrade is optional. It'll be called again next time the device is rebooted.
    CARAPI UpgradePduTableToAutoIncrement(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(void) UpdateThreadsAttachmentColumn(
        /* [in] */ ISQLiteDatabase* db);

    // Determine whether this database has CM11 columns...
    CARAPI_(Boolean) IsCM11DB(
        /* [in] */ ISQLiteDatabase* db);

private:
    static const String TAG;

    static const String SMS_UPDATE_THREAD_READ_BODY;

    static const String UPDATE_THREAD_COUNT_ON_NEW;

    static const String UPDATE_THREAD_COUNT_ON_OLD;

    static const String SMS_UPDATE_THREAD_DATE_SNIPPET_COUNT_ON_UPDATE;

    static const String PDU_UPDATE_THREAD_CONSTRAINTS;

    // When looking in the pdu table for unread messages, only count messages that
    // are displayed to the user. The constants are defined in PduHeaders and could be used
    // here, but the string "(m_type=132 OR m_type=130 OR m_type=128)" is used throughout this
    // file and so it is used here to be consistent.
    //     m_type=128   = MESSAGE_TYPE_SEND_REQ
    //     m_type=130   = MESSAGE_TYPE_NOTIFICATION_IND
    //     m_type=132   = MESSAGE_TYPE_RETRIEVE_CONF
    static const String PDU_UPDATE_THREAD_READ_BODY;

    static const String PDU_UPDATE_THREAD_DATE_SNIPPET_COUNT_ON_UPDATE;

    static const String UPDATE_THREAD_SNIPPET_SNIPPET_CS_ON_DELETE;


    // When a part is inserted, if it is not text/plain or application/smil
    // (which both can exist with text-only MMSes), then there is an attachment.
    // Set has_attachment=1 in the threads table for the thread in question.
    static const String PART_UPDATE_THREADS_ON_INSERT_TRIGGER;

    // When the 'mid' column in the part table is updated, we need to run the trigger to update
    // the threads table's has_attachment column, if the part is an attachment.
    static const String PART_UPDATE_THREADS_ON_UPDATE_TRIGGER;


    // When a part is deleted (with the same non-text/SMIL constraint as when
    // we set has_attachment), update the threads table for all threads.
    // Unfortunately we cannot update only the thread that the part was
    // attached to, as it is possible that the part has been orphaned and
    // the message it was attached to is already gone.
    static const String PART_UPDATE_THREADS_ON_DELETE_TRIGGER;

    // When the 'thread_id' column in the pdu table is updated, we need to run the trigger to update
    // the threads table's has_attachment column, if the message has an attachment in 'part' table
    static const String PDU_UPDATE_THREADS_ON_UPDATE_TRIGGER;

    static AutoPtr<MmsSmsDatabaseHelper> sInstance;
    static Boolean sTriedAutoIncrement;
    static Boolean sFakeLowStorageTest;     // for testing only

    static const String DATABASE_NAME;
    static const Int32 DATABASE_VERSION;
    AutoPtr<IContext> mContext;
    AutoPtr<IBroadcastReceiver> mLowStorageMonitor;
    static Object THIS;
};

} // namespace Telephony
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_TELEPHONY_MMSSMSDATABASEHELPER_H__
