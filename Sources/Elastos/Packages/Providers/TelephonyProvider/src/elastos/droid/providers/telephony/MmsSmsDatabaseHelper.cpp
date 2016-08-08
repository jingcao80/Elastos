
#include "elastos/droid/providers/telephony/MmsSmsDatabaseHelper.h"
#include "elastos/droid/providers/telephony/CLowStorageMonitor.h"
#include "elastos/droid/providers/telephony/CSmsProvider.h"
#include "_Elastos.Droid.Providers.Telephony.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Google.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Google::Mms::Pdu::CEncodedStringValue;
using Elastos::Droid::Google::Mms::Pdu::IEncodedStringValue;
using Elastos::Droid::Google::Mms::Pdu::IPduHeaders;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::ITelephonyBaseMmsColumns;
using Elastos::Droid::Provider::ITelephonyMmsAddr;
using Elastos::Droid::Provider::ITelephonyMmsPart;
using Elastos::Droid::Provider::ITelephonyMmsRate;
using Elastos::Droid::Provider::ITelephonyMmsSms;
using Elastos::Droid::Provider::ITelephonyMmsSmsPendingMessages;
using Elastos::Droid::Provider::ITelephonyMmsSmsWordsTable;
using Elastos::Droid::Provider::ITelephonyTextBasedSmsColumns;
using Elastos::Droid::Provider::ITelephonyThreadsColumns;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFile;
using Elastos::IO::ICloseable;
using Elastos::IO::IInputStream;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Telephony {

const String MmsSmsDatabaseHelper::TAG("MmsSmsDatabaseHelper");
const String MmsSmsDatabaseHelper::SMS_UPDATE_THREAD_READ_BODY =
                    String("  UPDATE threads SET read = ") +
                    "    CASE (SELECT COUNT(*)" +
                    "          FROM sms" +
                    "          WHERE " + ITelephonyTextBasedSmsColumns::READ + " = 0" +
                    "            AND " + ITelephonyTextBasedSmsColumns::THREAD_ID + " = threads._id)" +
                    "      WHEN 0 THEN 1" +
                    "      ELSE 0" +
                    "    END" +
                    "  WHERE threads._id = new." + ITelephonyTextBasedSmsColumns::THREAD_ID + "; ";

const String MmsSmsDatabaseHelper::UPDATE_THREAD_COUNT_ON_NEW =
                    String("  UPDATE threads SET message_count = ") +
                    "     (SELECT COUNT(sms._id) FROM sms LEFT JOIN threads " +
                    "      ON threads._id = " + ITelephonyTextBasedSmsColumns::THREAD_ID +
                    "      WHERE " + ITelephonyTextBasedSmsColumns::THREAD_ID + " = new.thread_id" +
                    "        AND sms." + ITelephonyTextBasedSmsColumns::TYPE + " != 3) + " +
                    "     (SELECT COUNT(pdu._id) FROM pdu LEFT JOIN threads " +
                    "      ON threads._id = " + ITelephonyBaseMmsColumns::THREAD_ID +
                    "      WHERE " + ITelephonyBaseMmsColumns::THREAD_ID + " = new.thread_id" +
                    "        AND (m_type=132 OR m_type=130 OR m_type=128)" +
                    "        AND " + ITelephonyBaseMmsColumns::MESSAGE_BOX + " != 3) " +
                    "  WHERE threads._id = new.thread_id; ";

const String MmsSmsDatabaseHelper::UPDATE_THREAD_COUNT_ON_OLD =
                    String("  UPDATE threads SET message_count = ") +
                    "     (SELECT COUNT(sms._id) FROM sms LEFT JOIN threads " +
                    "      ON threads._id = " + ITelephonyTextBasedSmsColumns::THREAD_ID +
                    "      WHERE " + ITelephonyTextBasedSmsColumns::THREAD_ID + " = old.thread_id" +
                    "        AND sms." + ITelephonyTextBasedSmsColumns::TYPE + " != 3) + " +
                    "     (SELECT COUNT(pdu._id) FROM pdu LEFT JOIN threads " +
                    "      ON threads._id = " + ITelephonyBaseMmsColumns::THREAD_ID +
                    "      WHERE " + ITelephonyBaseMmsColumns::THREAD_ID + " = old.thread_id" +
                    "        AND (m_type=132 OR m_type=130 OR m_type=128)" +
                    "        AND " + ITelephonyBaseMmsColumns::MESSAGE_BOX + " != 3) " +
                    "  WHERE threads._id = old.thread_id; ";

const String MmsSmsDatabaseHelper::SMS_UPDATE_THREAD_DATE_SNIPPET_COUNT_ON_UPDATE =
                    String("BEGIN") +
                    "  UPDATE threads SET" +
                    "    date = (strftime('%s','now') * 1000), " +
                    "    snippet = new." + ITelephonyTextBasedSmsColumns::BODY + ", " +
                    "    snippet_cs = 0" +
                    "  WHERE threads._id = new." + ITelephonyTextBasedSmsColumns::THREAD_ID + "; " +
                    UPDATE_THREAD_COUNT_ON_NEW +
                    SMS_UPDATE_THREAD_READ_BODY +
                    "END;";

const String MmsSmsDatabaseHelper::PDU_UPDATE_THREAD_CONSTRAINTS =
                    String("  WHEN new.") + ITelephonyBaseMmsColumns::MESSAGE_TYPE + "=" +
                    StringUtils::ToString(IPduHeaders::MESSAGE_TYPE_RETRIEVE_CONF) +
                    "    OR new." + ITelephonyBaseMmsColumns::MESSAGE_TYPE + "=" +
                    StringUtils::ToString(IPduHeaders::MESSAGE_TYPE_NOTIFICATION_IND) +
                    "    OR new." + ITelephonyBaseMmsColumns::MESSAGE_TYPE + "=" +
                    StringUtils::ToString(IPduHeaders::MESSAGE_TYPE_SEND_REQ) + " ";

// When looking in the pdu table for unread messages, only count messages that
// are displayed to the user. The constants are defined in PduHeaders and could be used
// here, but the string "(m_type=132 OR m_type=130 OR m_type=128)" is used throughout this
// file and so it is used here to be consistent.
//     m_type=128   = MESSAGE_TYPE_SEND_REQ
//     m_type=130   = MESSAGE_TYPE_NOTIFICATION_IND
//     m_type=132   = MESSAGE_TYPE_RETRIEVE_CONF
const String MmsSmsDatabaseHelper::PDU_UPDATE_THREAD_READ_BODY =
                    String("  UPDATE threads SET read = ") +
                    "    CASE (SELECT COUNT(*)" +
                    "          FROM " + IMmsProvider::TABLE_PDU +
                    "          WHERE " + ITelephonyBaseMmsColumns::READ + " = 0" +
                    "            AND " + ITelephonyBaseMmsColumns::THREAD_ID + " = threads._id " +
                    "            AND (m_type=132 OR m_type=130 OR m_type=128)) " +
                    "      WHEN 0 THEN 1" +
                    "      ELSE 0" +
                    "    END" +
                    "  WHERE threads._id = new." + ITelephonyBaseMmsColumns::THREAD_ID + "; ";

const String MmsSmsDatabaseHelper::PDU_UPDATE_THREAD_DATE_SNIPPET_COUNT_ON_UPDATE =
                    String("BEGIN") +
                    "  UPDATE threads SET" +
                    "    date = (strftime('%s','now') * 1000), " +
                    "    snippet = new." + ITelephonyBaseMmsColumns::SUBJECT + ", " +
                    "    snippet_cs = new." + ITelephonyBaseMmsColumns::SUBJECT_CHARSET +
                    "  WHERE threads._id = new." + ITelephonyBaseMmsColumns::THREAD_ID + "; " +
                    UPDATE_THREAD_COUNT_ON_NEW +
                    PDU_UPDATE_THREAD_READ_BODY +
                    "END;";

const String MmsSmsDatabaseHelper::UPDATE_THREAD_SNIPPET_SNIPPET_CS_ON_DELETE =
                    String("  UPDATE threads SET snippet = ") +
                    "   (SELECT snippet FROM" +
                    "     (SELECT date * 1000 AS date, sub AS snippet, thread_id FROM pdu" +
                    "      UNION SELECT date, body AS snippet, thread_id FROM sms)" +
                    "    WHERE thread_id = OLD.thread_id ORDER BY date DESC LIMIT 1) " +
                    "  WHERE threads._id = OLD.thread_id; " +
                    "  UPDATE threads SET snippet_cs = " +
                    "   (SELECT snippet_cs FROM" +
                    "     (SELECT date * 1000 AS date, sub_cs AS snippet_cs, thread_id FROM pdu" +
                    "      UNION SELECT date, 0 AS snippet_cs, thread_id FROM sms)" +
                    "    WHERE thread_id = OLD.thread_id ORDER BY date DESC LIMIT 1) " +
                    "  WHERE threads._id = OLD.thread_id; ";


// When a part is inserted, if it is not text/plain or application/smil
// (which both can exist with text-only MMSes), then there is an attachment.
// Set has_attachment=1 in the threads table for the thread in question.
const String MmsSmsDatabaseHelper::PART_UPDATE_THREADS_ON_INSERT_TRIGGER =
                    String("CREATE TRIGGER update_threads_on_insert_part ") +
                    " AFTER INSERT ON part " +
                    " WHEN new.ct != 'text/plain' AND new.ct != 'application/smil' " +
                    " BEGIN " +
                    "  UPDATE threads SET has_attachment=1 WHERE _id IN " +
                    "   (SELECT pdu.thread_id FROM part JOIN pdu ON pdu._id=part.mid " +
                    "     WHERE part._id=new._id LIMIT 1); " +
                    " END";

// When the 'mid' column in the part table is updated, we need to run the trigger to update
// the threads table's has_attachment column, if the part is an attachment.
const String MmsSmsDatabaseHelper::PART_UPDATE_THREADS_ON_UPDATE_TRIGGER =
                    String("CREATE TRIGGER update_threads_on_update_part ") +
                    " AFTER UPDATE of " + ITelephonyMmsPart::MSG_ID + " ON part " +
                    " WHEN new.ct != 'text/plain' AND new.ct != 'application/smil' " +
                    " BEGIN " +
                    "  UPDATE threads SET has_attachment=1 WHERE _id IN " +
                    "   (SELECT pdu.thread_id FROM part JOIN pdu ON pdu._id=part.mid " +
                    "     WHERE part._id=new._id LIMIT 1); " +
                    " END";


// When a part is deleted (with the same non-text/SMIL constraint as when
// we set has_attachment), update the threads table for all threads.
// Unfortunately we cannot update only the thread that the part was
// attached to, as it is possible that the part has been orphaned and
// the message it was attached to is already gone.
const String MmsSmsDatabaseHelper::PART_UPDATE_THREADS_ON_DELETE_TRIGGER =
                    String("CREATE TRIGGER update_threads_on_delete_part ") +
                    " AFTER DELETE ON part " +
                    " WHEN old.ct != 'text/plain' AND old.ct != 'application/smil' " +
                    " BEGIN " +
                    "  UPDATE threads SET has_attachment = " +
                    "   CASE " +
                    "    (SELECT COUNT(*) FROM part JOIN pdu " +
                    "     WHERE pdu.thread_id = threads._id " +
                    "     AND part.ct != 'text/plain' AND part.ct != 'application/smil' " +
                    "     AND part.mid = pdu._id)" +
                    "   WHEN 0 THEN 0 " +
                    "   ELSE 1 " +
                    "   END; " +
                    " END";

// When the 'thread_id' column in the pdu table is updated, we need to run the trigger to update
// the threads table's has_attachment column, if the message has an attachment in 'part' table
const String MmsSmsDatabaseHelper::PDU_UPDATE_THREADS_ON_UPDATE_TRIGGER =
                    String("CREATE TRIGGER update_threads_on_update_pdu ") +
                    " AFTER UPDATE of thread_id ON pdu " +
                    " BEGIN " +
                    "  UPDATE threads SET has_attachment=1 WHERE _id IN " +
                    "   (SELECT pdu.thread_id FROM part JOIN pdu " +
                    "     WHERE part.ct != 'text/plain' AND part.ct != 'application/smil' " +
                    "     AND part.mid = pdu._id);" +
                    " END";

AutoPtr<MmsSmsDatabaseHelper> MmsSmsDatabaseHelper::sInstance;
Boolean MmsSmsDatabaseHelper::sTriedAutoIncrement = FALSE;
Boolean MmsSmsDatabaseHelper::sFakeLowStorageTest = FALSE;     // for testing only

const String MmsSmsDatabaseHelper::DATABASE_NAME("mmssms.db");
const Int32 MmsSmsDatabaseHelper::DATABASE_VERSION = 62;
Object MmsSmsDatabaseHelper::THIS;

ECode MmsSmsDatabaseHelper::LowStorageMonitor::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);

    Logger::D(TAG, "[LowStorageMonitor] onReceive intent %s", action.string());

    if (IIntent::ACTION_DEVICE_STORAGE_OK.Equals(action)) {
        MmsSmsDatabaseHelper::sTriedAutoIncrement = FALSE;    // try to upgrade on the next getWriteableDatabase
    }
    return NOERROR;
}

MmsSmsDatabaseHelper::MmsSmsDatabaseHelper(
    /* [in] */ IContext* context)
{
    SQLiteOpenHelper::constructor(context, DATABASE_NAME, NULL, DATABASE_VERSION);

    mContext = context;
}

/**
 * Return a singleton helper for the combined MMS and SMS
 * database.
 */
/* package */
AutoPtr<MmsSmsDatabaseHelper> MmsSmsDatabaseHelper::GetInstance(
    /* [in] */ IContext* context)
{
    AutoLock lock(THIS);
    if (sInstance == NULL) {
        sInstance = new MmsSmsDatabaseHelper(context);
    }
    return sInstance;
}

/**
 * Look through all the recipientIds referenced by the threads and then delete any
 * unreferenced rows from the canonical_addresses table.
 */
ECode MmsSmsDatabaseHelper::RemoveUnferencedCanonicalAddresses(
    /* [in] */ ISQLiteDatabase* db)
{
    AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(1);
    (*ss)[0] = String("recipient_ids");
    AutoPtr<ICursor> c;
    db->Query(IMmsSmsProvider::TABLE_THREADS, ss,
            String(NULL), NULL, String(NULL), String(NULL), String(NULL), (ICursor**)&c);
    if (c != NULL) {
        ECode ec = NOERROR;
        do {
            Int32 count = 0;
            if ((c->GetCount(&count), count) == 0) {
                // no threads, delete all addresses
                Int32 rows = 0;
                ec = db->Delete(String("canonical_addresses"), String(NULL), NULL, &rows);
                if (FAILED(ec)) {
                    break;
                }
            }
            else {
                // Find all the referenced recipient_ids from the threads. recipientIds is
                // a space-separated list of recipient ids: "1 14 21"
                AutoPtr<IHashSet/*<Integer*/> recipientIds;
                CHashSet::New((IHashSet**)&recipientIds);
                Boolean move = FALSE;
                while (c->MoveToNext(&move), move) {
                    AutoPtr<ArrayOf<String> > recips;
                    String s;
                    c->GetString(0, &s);
                    StringUtils::Split(s, String(" "), (ArrayOf<String>**)&recips);
                    Int32 len = recips->GetLength();
                    for (Int32 i = 0; i < len; i++) {
                        String recip = (*recips)[i];
                        // try {
                        Int32 recipientId = StringUtils::ParseInt32(recip);
                        recipientIds->Add(CoreUtils::Convert(recipientId));
                        // } catch (Exception e) {
                        // }
                    }
                }
                // Now build a selection string of all the unique recipient ids
                StringBuilder sb;
                AutoPtr<IIterator> iter;
                recipientIds->GetIterator((IIterator**)&iter);
                Boolean has = FALSE;
                while (iter->HasNext(&has), has) {
                    AutoPtr<IInterface> obj;
                    iter->GetNext((IInterface**)&obj);
                    sb.Append(String("_id != ") + TO_STR(obj));
                    if (iter->HasNext(&has), has) {
                        sb.Append(" AND ");
                    }
                }
                if (sb.GetLength() > 0) {
                    Int32 rows = 0;
                    ec = db->Delete(String("canonical_addresses"), sb.ToString(), NULL, &rows);
                }
            }
        } while(0);
        ICloseable::Probe(c)->Close();
        return ec;
    }
    return NOERROR;
}

void MmsSmsDatabaseHelper::UpdateThread(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int64 thread_id)
{
    if (thread_id < 0) {
        UpdateAllThreads(db, String(NULL), NULL);
        return;
    }

    db->BeginTransaction();
    ECode ec = NOERROR;
    do {
        // Delete the row for this thread in the threads table if
        // there are no more messages attached to it in either
        // the sms or pdu tables.
        Int32 rows = 0;
        AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(1);
        (*ss)[0] = StringUtils::ToString(thread_id);
        ec = db->Delete(IMmsSmsProvider::TABLE_THREADS,
                  String("_id = ? AND _id NOT IN") +
                  "          (SELECT thread_id FROM sms " +
                  "           UNION SELECT thread_id FROM pdu)",
                  ss, &rows);
        if (FAILED(ec)) {
            break;
        }

        if (rows > 0) {
            // If this deleted a row, let's remove orphaned canonical_addresses and get outta here
            RemoveUnferencedCanonicalAddresses(db);
        }
        else {
            // Update the message count in the threads table as the sum
            // of all messages in both the sms and pdu tables.
            ec = db->ExecSQL(
                    String("  UPDATE threads SET message_count = ") +
                            "     (SELECT COUNT(sms._id) FROM sms LEFT JOIN threads " +
                            "      ON threads._id = " + ITelephonyTextBasedSmsColumns::THREAD_ID +
                            "      WHERE " + ITelephonyTextBasedSmsColumns::THREAD_ID
                            + " = " + StringUtils::ToString(thread_id) +
                            "        AND sms." + ITelephonyTextBasedSmsColumns::TYPE + " != 3) + " +
                            "     (SELECT COUNT(pdu._id) FROM pdu LEFT JOIN threads " +
                            "      ON threads._id = " + ITelephonyBaseMmsColumns::THREAD_ID +
                            "      WHERE " + ITelephonyBaseMmsColumns::THREAD_ID
                            + " = " + StringUtils::ToString(thread_id) +
                            "        AND (m_type=132 OR m_type=130 OR m_type=128)" +
                            "        AND " + ITelephonyBaseMmsColumns::MESSAGE_BOX + " != 3) " +
                            "  WHERE threads._id = " + StringUtils::ToString(thread_id) + ";");
            if (FAILED(ec)) {
                break;
            }

            // Update the date and the snippet (and its character set) in
            // the threads table to be that of the most recent message in
            // the thread.
            ec = db->ExecSQL(
            String("  UPDATE threads") +
            "  SET" +
            "  date =" +
            "    (SELECT date FROM" +
            "        (SELECT date * 1000 AS date, thread_id FROM pdu" +
            "         UNION SELECT date, thread_id FROM sms)" +
            "     WHERE thread_id = " + StringUtils::ToString(thread_id) + " ORDER BY date DESC LIMIT 1)," +
            "  snippet =" +
            "    (SELECT snippet FROM" +
            "        (SELECT date * 1000 AS date, sub AS snippet, thread_id FROM pdu" +
            "         UNION SELECT date, body AS snippet, thread_id FROM sms)" +
            "     WHERE thread_id = " + StringUtils::ToString(thread_id) + " ORDER BY date DESC LIMIT 1)," +
            "  snippet_cs =" +
            "    (SELECT snippet_cs FROM" +
            "        (SELECT date * 1000 AS date, sub_cs AS snippet_cs, thread_id FROM pdu" +
            "         UNION SELECT date, 0 AS snippet_cs, thread_id FROM sms)" +
            "     WHERE thread_id = " + StringUtils::ToString(thread_id) + " ORDER BY date DESC LIMIT 1)" +
            "  WHERE threads._id = " + StringUtils::ToString(thread_id) + ";");

            if (FAILED(ec)) {
                break;
            }

            // Update the error column of the thread to indicate if there
            // are any messages in it that have failed to send.
            // First check to see if there are any messages with errors in this thread.
            String query = String("SELECT thread_id FROM sms WHERE type=") +
                    StringUtils::ToString(ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_FAILED) +
                    " AND thread_id = " + StringUtils::ToString(thread_id) +
                    " LIMIT 1";
            Int32 setError = 0;
            AutoPtr<ICursor> c;
            ec = db->RawQuery(query, NULL, (ICursor**)&c);
            if (FAILED(ec)) {
                break;
            }

            if (c != NULL) {
                // try {
                c->GetCount(&setError);    // Because of the LIMIT 1, count will be 1 or 0.
                // } finally {
                ICloseable::Probe(c)->Close();
                // }
            }
            // What's the current state of the error flag in the threads table?
            String errorQuery = String("SELECT error FROM threads WHERE _id = ") + StringUtils::ToString(thread_id);
            c = NULL;
            db->RawQuery(errorQuery, NULL, (ICursor**)&c);
            if (c != NULL) {
                // try {
                Boolean move = FALSE;
                if (c->MoveToNext(&move), move) {
                    Int32 curError = 0;
                    c->GetInt32(0, &curError);
                    if (curError != setError) {
                        // The current thread error column differs, update it.
                        ec = db->ExecSQL(String("UPDATE threads SET error=") + StringUtils::ToString(setError) +
                                " WHERE _id = " + StringUtils::ToString(thread_id));
                    }
                }
                // } finally {
                ICloseable::Probe(c)->Close();
                if (FAILED(ec)) {
                    break;
                }
                // }
            }
        }
        db->SetTransactionSuccessful();
    } while (0);
    // {
    //     Logger::E(TAG, ex.getMessage(), ex);
    // } finally {
    db->EndTransaction();
    // }
}

void MmsSmsDatabaseHelper::UpdateAllThreads(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& _where,
    /* [in] */ ArrayOf<String>* whereArgs)
{
    db->BeginTransaction();
    String where = _where;
    ECode ec = NOERROR;
    do {
        if (where == NULL) {
            where = String("");
        }
        else {
            where = String("WHERE (") + where + ")";
        }
        String query = String("SELECT _id FROM threads WHERE _id IN ") +
                       "(SELECT DISTINCT thread_id FROM sms " + where + ")";
        AutoPtr<ICursor> c;
        ec = db->RawQuery(query, whereArgs, (ICursor**)&c);
        if (FAILED(ec)) {
            break;
        }
        if (c != NULL) {
            // try {
            Boolean move = FALSE;
            Int32 v = 0;
            while (c->MoveToNext(&move), move) {
                c->GetInt32(0, &v);
                UpdateThread(db, v);
            }
            // } finally {
            ICloseable::Probe(c)->Close();
            // }
        }
        // TODO: there are several db operations in this function. Lets wrap them in a
        // transaction to make it faster.
        // remove orphaned threads
        Int32 v = 0;
        ec = db->Delete(IMmsSmsProvider::TABLE_THREADS,
                String("_id NOT IN (SELECT DISTINCT thread_id FROM sms where thread_id NOT NULL ") +
                "UNION SELECT DISTINCT thread_id FROM pdu where thread_id NOT NULL)", NULL, &v);
        if (FAILED(ec)) {
            break;
        }

        // remove orphaned canonical_addresses
        if (FAILED(RemoveUnferencedCanonicalAddresses(db))) {
            break;
        }

        db->SetTransactionSuccessful();
    } while (0);
    // {
    //     Logger::E(TAG, ex.getMessage(), ex);
    // } finally {
    db->EndTransaction();
    // }
}

Int32 MmsSmsDatabaseHelper::DeleteOneSms(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 message_id)
{
    Int32 thread_id = -1;
    // Find the thread ID that the specified SMS belongs to.
    AutoPtr<ICursor> c;
    AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(1);
    (*ss)[0] = String("thread_id");
    db->Query(String("sms"), ss, String("_id=") + StringUtils::ToString(message_id)
        , NULL, String(NULL), String(NULL), String(NULL), (ICursor**)&c);
    if (c != NULL) {
        Boolean move = FALSE;
        if (c->MoveToFirst(&move)) {
            c->GetInt32(0, &thread_id);
        }
        ICloseable::Probe(c)->Close();
    }

    // Delete the specified message.
    Int32 rows = 0;
    db->Delete(String("sms"), String("_id=") + StringUtils::ToString(message_id), NULL, &rows);
    if (thread_id > 0) {
        // Update its thread.
        UpdateThread(db, thread_id);
    }
    return rows;
}

// @Override
ECode MmsSmsDatabaseHelper::OnCreate(
    /* [in] */ ISQLiteDatabase* db)
{
    CreateMmsTables(db);
    CreateSmsTables(db);
    CreateCommonTables(db);
    CreateCommonTriggers(db);
    CreateMmsTriggers(db);
    CreateWordsTables(db);
    CreateIndices(db);
    return NOERROR;
}

// When upgrading the database we need to populate the words
// table with the rows out of sms and part.
void MmsSmsDatabaseHelper::PopulateWordsTable(
    /* [in] */ ISQLiteDatabase* db)
{
    const String TABLE_WORDS = String("words");
    {
        AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(2);
        (*ss)[0] = IBaseColumns::ID;
        (*ss)[0] = ITelephonyTextBasedSmsColumns::BODY;
        AutoPtr<ICursor> smsRows;
        db->Query(
                String("sms"),
                ss,
                String(NULL),
                NULL,
                String(NULL),
                String(NULL),
                String(NULL), (ICursor**)&smsRows);
        // try {
            if (smsRows != NULL) {
                Boolean tmp = FALSE;
                smsRows->MoveToPosition(-1, &tmp);
                AutoPtr<IContentValues> cv;
                CContentValues::New((IContentValues**)&cv);
                Boolean move = FALSE;
                while (smsRows->MoveToNext(&move), move) {
                    cv->Clear();

                    Int64 id = 0, lv = 0;
                    smsRows->GetInt64(0, &id);        // 0 for Sms._ID
                    String body;
                    smsRows->GetString(1, &body);  // 1 for Sms.BODY

                    cv->Put(ITelephonyMmsSmsWordsTable::ID, id);
                    cv->Put(ITelephonyMmsSmsWordsTable::INDEXED_TEXT, body);
                    cv->Put(ITelephonyMmsSmsWordsTable::SOURCE_ROW_ID, id);
                    cv->Put(ITelephonyMmsSmsWordsTable::TABLE_ID, 1);
                    db->Insert(TABLE_WORDS, ITelephonyMmsSmsWordsTable::INDEXED_TEXT, cv, &lv);
                }
            }
        // } finally {
            if (smsRows != NULL) {
                ICloseable::Probe(smsRows)->Close();
            }
        // }
    }

    {
        AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(2);
        (*ss)[0] = IBaseColumns::ID;
        (*ss)[0] = ITelephonyMmsPart::TEXT;
        AutoPtr<ICursor> mmsRows;
        db->Query(
                String("part"),
                ss,
                String("ct = 'text/plain'"),
                NULL,
                String(NULL),
                String(NULL),
                String(NULL), (ICursor**)&mmsRows);
        // try {
            if (mmsRows != NULL) {
                Boolean tmp = FALSE;
                mmsRows->MoveToPosition(-1, &tmp);
                AutoPtr<IContentValues> cv;
                CContentValues::New((IContentValues**)&cv);
                Boolean move = FALSE;
                while (mmsRows->MoveToNext(&move), move) {
                    cv->Clear();

                    Int64 id = 0, lv = 0;
                    mmsRows->GetInt64(0, &id);         // 0 for Part._ID
                    String body;
                    mmsRows->GetString(1, &body);   // 1 for Part.TEXT

                    cv->Put(ITelephonyMmsSmsWordsTable::ID, id);
                    cv->Put(ITelephonyMmsSmsWordsTable::INDEXED_TEXT, body);
                    cv->Put(ITelephonyMmsSmsWordsTable::SOURCE_ROW_ID, id);
                    cv->Put(ITelephonyMmsSmsWordsTable::TABLE_ID, 1);
                    db->Insert(TABLE_WORDS, ITelephonyMmsSmsWordsTable::INDEXED_TEXT, cv, &lv);
                }
            }
        // } finally {
            if (mmsRows != NULL) {
                ICloseable::Probe(mmsRows)->Close();
            }
        // }
    }
}

void MmsSmsDatabaseHelper::CreateWordsTables(
    /* [in] */ ISQLiteDatabase* db)
{
    ECode ec = NOERROR;
    do {
        ec = db->ExecSQL(String("CREATE VIRTUAL TABLE words USING FTS3 (_id INTEGER PRIMARY KEY, index_text TEXT, source_id INTEGER, table_to_use INTEGER);"));
        if (FAILED(ec)) {
            break;
        }
        // monitor the sms table
        // NOTE don't handle inserts using a trigger because it has an unwanted
        // side effect:  the value returned for the last row ends up being the
        // id of one of the trigger insert not the original row insert.
        // Handle inserts manually in the provider.
        ec = db->ExecSQL(String("CREATE TRIGGER sms_words_update AFTER UPDATE ON sms BEGIN UPDATE words ") +
                " SET index_text = NEW.body WHERE (source_id=NEW._id AND table_to_use=1); " +
                " END;");
        if (FAILED(ec)) {
            break;
        }
        ec = db->ExecSQL(String("CREATE TRIGGER sms_words_delete AFTER DELETE ON sms BEGIN DELETE FROM ") +
                "  words WHERE source_id = OLD._id AND table_to_use = 1; END;");
        if (FAILED(ec)) {
            break;
        }

        PopulateWordsTable(db);
    } while (0);
    if (FAILED(ec)) {
        Logger::E(TAG, "got exception creating words table: ");
    }
}

void MmsSmsDatabaseHelper::CreateIndices(
    /* [in] */ ISQLiteDatabase* db)
{
    CreateThreadIdIndex(db);
}

void MmsSmsDatabaseHelper::CreateThreadIdIndex(
    /* [in] */ ISQLiteDatabase* db)
{
    if (FAILED(db->ExecSQL(String("CREATE INDEX IF NOT EXISTS typeThreadIdIndex ON sms (type, thread_id);")))) {
        Logger::E(TAG, "got exception creating indices: ");
    }
}

void MmsSmsDatabaseHelper::CreateMmsTables(
    /* [in] */ ISQLiteDatabase* db)
{
    // N.B.: Whenever the columns here are changed, the columns in
    // {@ref MmsSmsProvider} must be changed to match.
    db->ExecSQL(String("CREATE TABLE ") + IMmsProvider::TABLE_PDU + " (" +
               IBaseColumns::ID + " INTEGER PRIMARY KEY AUTOINCREMENT," +
               ITelephonyBaseMmsColumns::THREAD_ID + " INTEGER," +
               ITelephonyBaseMmsColumns::DATE + " INTEGER," +
               ITelephonyBaseMmsColumns::DATE_SENT + " INTEGER DEFAULT 0," +
               ITelephonyBaseMmsColumns::MESSAGE_BOX + " INTEGER," +
               ITelephonyBaseMmsColumns::READ + " INTEGER DEFAULT 0," +
               ITelephonyBaseMmsColumns::MESSAGE_ID + " TEXT," +
               ITelephonyBaseMmsColumns::SUBJECT + " TEXT," +
               ITelephonyBaseMmsColumns::SUBJECT_CHARSET + " INTEGER," +
               ITelephonyBaseMmsColumns::CONTENT_TYPE + " TEXT," +
               ITelephonyBaseMmsColumns::CONTENT_LOCATION + " TEXT," +
               ITelephonyBaseMmsColumns::EXPIRY + " INTEGER," +
               ITelephonyBaseMmsColumns::MESSAGE_CLASS + " TEXT," +
               ITelephonyBaseMmsColumns::MESSAGE_TYPE + " INTEGER," +
               ITelephonyBaseMmsColumns::MMS_VERSION + " INTEGER," +
               ITelephonyBaseMmsColumns::MESSAGE_SIZE + " INTEGER," +
               ITelephonyBaseMmsColumns::PRIORITY + " INTEGER," +
               ITelephonyBaseMmsColumns::READ_REPORT + " INTEGER," +
               ITelephonyBaseMmsColumns::REPORT_ALLOWED + " INTEGER," +
               ITelephonyBaseMmsColumns::RESPONSE_STATUS + " INTEGER," +
               ITelephonyBaseMmsColumns::STATUS + " INTEGER," +
               ITelephonyBaseMmsColumns::TRANSACTION_ID + " TEXT," +
               ITelephonyBaseMmsColumns::RETRIEVE_STATUS + " INTEGER," +
               ITelephonyBaseMmsColumns::RETRIEVE_TEXT + " TEXT," +
               ITelephonyBaseMmsColumns::RETRIEVE_TEXT_CHARSET + " INTEGER," +
               ITelephonyBaseMmsColumns::READ_STATUS + " INTEGER," +
               ITelephonyBaseMmsColumns::CONTENT_CLASS + " INTEGER," +
               ITelephonyBaseMmsColumns::RESPONSE_TEXT + " TEXT," +
               ITelephonyBaseMmsColumns::DELIVERY_TIME + " INTEGER," +
               ITelephonyBaseMmsColumns::DELIVERY_REPORT + " INTEGER," +
               ITelephonyBaseMmsColumns::LOCKED + " INTEGER DEFAULT 0," +
               ITelephonyBaseMmsColumns::PHONE_ID + " INTEGER DEFAULT -1, " +
               ITelephonyBaseMmsColumns::SEEN + " INTEGER DEFAULT 0," +
               ITelephonyBaseMmsColumns::CREATOR + " TEXT," +
               ITelephonyBaseMmsColumns::TEXT_ONLY + " INTEGER DEFAULT 0" +
               ");");

    db->ExecSQL(String("CREATE TABLE ") + IMmsProvider::TABLE_ADDR + " (" +
               IBaseColumns::ID + " INTEGER PRIMARY KEY," +
               ITelephonyMmsAddr::MSG_ID + " INTEGER," +
               ITelephonyMmsAddr::CONTACT_ID + " INTEGER," +
               ITelephonyMmsAddr::ADDRESS + " TEXT," +
               ITelephonyMmsAddr::TYPE + " INTEGER," +
               ITelephonyMmsAddr::CHARSET + " INTEGER);");

    db->ExecSQL(String("CREATE TABLE ") + IMmsProvider::TABLE_PART + " (" +
               IBaseColumns::ID + " INTEGER PRIMARY KEY AUTOINCREMENT," +
               ITelephonyMmsPart::MSG_ID + " INTEGER," +
               ITelephonyMmsPart::SEQ + " INTEGER DEFAULT 0," +
               ITelephonyMmsPart::CONTENT_TYPE + " TEXT," +
               ITelephonyMmsPart::NAME + " TEXT," +
               ITelephonyMmsPart::CHARSET + " INTEGER," +
               ITelephonyMmsPart::CONTENT_DISPOSITION + " TEXT," +
               ITelephonyMmsPart::FILENAME + " TEXT," +
               ITelephonyMmsPart::CONTENT_ID + " TEXT," +
               ITelephonyMmsPart::CONTENT_LOCATION + " TEXT," +
               ITelephonyMmsPart::CT_START + " INTEGER," +
               ITelephonyMmsPart::CT_TYPE + " TEXT," +
               ITelephonyMmsPart::_DATA + " TEXT," +
               ITelephonyMmsPart::TEXT + " TEXT);");

    db->ExecSQL(String("CREATE TABLE ") + IMmsProvider::TABLE_RATE + " (" +
               ITelephonyMmsRate::SENT_TIME + " INTEGER);");

    db->ExecSQL(String("CREATE TABLE ") + IMmsProvider::TABLE_DRM + " (" +
               IBaseColumns::ID + " INTEGER PRIMARY KEY," +
               "_data TEXT);");
}

// Unlike the other trigger-creating functions, this function can be called multiple times
// without harm.
ECode MmsSmsDatabaseHelper::CreateMmsTriggers(
    /* [in] */ ISQLiteDatabase* db)
{
    // Cleans up parts when a MM is deleted.
    FAIL_RETURN(db->ExecSQL(String("DROP TRIGGER IF EXISTS part_cleanup")));
    FAIL_RETURN(db->ExecSQL(String("CREATE TRIGGER part_cleanup DELETE ON ") + IMmsProvider::TABLE_PDU + " " +
            "BEGIN " +
            "  DELETE FROM " + IMmsProvider::TABLE_PART +
            "  WHERE " + ITelephonyMmsPart::MSG_ID + "=old._id;" +
            "END;"));

    // Cleans up address info when a MM is deleted.
    FAIL_RETURN(db->ExecSQL(String("DROP TRIGGER IF EXISTS addr_cleanup")));
    FAIL_RETURN(db->ExecSQL(String("CREATE TRIGGER addr_cleanup DELETE ON ") + IMmsProvider::TABLE_PDU + " " +
            "BEGIN " +
            "  DELETE FROM " + IMmsProvider::TABLE_ADDR +
            "  WHERE " + ITelephonyMmsAddr::MSG_ID + "=old._id;" +
            "END;"));

    // Delete obsolete delivery-report, read-report while deleting their
    // associated Send.req.
    FAIL_RETURN(db->ExecSQL(String("DROP TRIGGER IF EXISTS cleanup_delivery_and_read_report")));
    FAIL_RETURN(db->ExecSQL(String("CREATE TRIGGER cleanup_delivery_and_read_report ") +
            "AFTER DELETE ON " + IMmsProvider::TABLE_PDU + " " +
            "WHEN old." + ITelephonyBaseMmsColumns::MESSAGE_TYPE
            + "=" + StringUtils::ToString(IPduHeaders::MESSAGE_TYPE_SEND_REQ) + " " +
            "BEGIN " +
            "  DELETE FROM " + IMmsProvider::TABLE_PDU +
            "  WHERE (" + ITelephonyBaseMmsColumns::MESSAGE_TYPE
            + "=" + StringUtils::ToString(IPduHeaders::MESSAGE_TYPE_DELIVERY_IND) +
            "    OR " + ITelephonyBaseMmsColumns::MESSAGE_TYPE
            + "=" + StringUtils::ToString(IPduHeaders::MESSAGE_TYPE_READ_ORIG_IND) +
            ")" +
            "    AND " + ITelephonyBaseMmsColumns::MESSAGE_ID + "=old."
            + ITelephonyBaseMmsColumns::MESSAGE_ID + "; " +
            "END;"));

    FAIL_RETURN(db->ExecSQL(String("DROP TRIGGER IF EXISTS update_threads_on_insert_part")));
    FAIL_RETURN(db->ExecSQL(PART_UPDATE_THREADS_ON_INSERT_TRIGGER));

    FAIL_RETURN(db->ExecSQL(String("DROP TRIGGER IF EXISTS update_threads_on_update_part")));
    FAIL_RETURN(db->ExecSQL(PART_UPDATE_THREADS_ON_UPDATE_TRIGGER));

    FAIL_RETURN(db->ExecSQL(String("DROP TRIGGER IF EXISTS update_threads_on_delete_part")));
    FAIL_RETURN(db->ExecSQL(PART_UPDATE_THREADS_ON_DELETE_TRIGGER));

    FAIL_RETURN(db->ExecSQL(String("DROP TRIGGER IF EXISTS update_threads_on_update_pdu")));
    FAIL_RETURN(db->ExecSQL(PDU_UPDATE_THREADS_ON_UPDATE_TRIGGER));

    // Delete pending status for a message when it is deleted.
    FAIL_RETURN(db->ExecSQL(String("DROP TRIGGER IF EXISTS delete_mms_pending_on_delete")));
    FAIL_RETURN(db->ExecSQL(String("CREATE TRIGGER delete_mms_pending_on_delete ") +
               "AFTER DELETE ON " + IMmsProvider::TABLE_PDU + " " +
               "BEGIN " +
               "  DELETE FROM " + IMmsSmsProvider::TABLE_PENDING_MSG +
               "  WHERE " + ITelephonyMmsSmsPendingMessages::MSG_ID + "=old._id; " +
               "END;"));

    // When a message is moved out of Outbox, delete its pending status.
    FAIL_RETURN(db->ExecSQL(String("DROP TRIGGER IF EXISTS delete_mms_pending_on_update")));
    FAIL_RETURN(db->ExecSQL(String("CREATE TRIGGER delete_mms_pending_on_update ") +
               "AFTER UPDATE ON " + IMmsProvider::TABLE_PDU + " " +
               "WHEN old." + ITelephonyBaseMmsColumns::MESSAGE_BOX
               + "=" + StringUtils::ToString(ITelephonyBaseMmsColumns::MESSAGE_BOX_OUTBOX) +
               "  AND new." + ITelephonyBaseMmsColumns::MESSAGE_BOX
               + "!=" + StringUtils::ToString(ITelephonyBaseMmsColumns::MESSAGE_BOX_OUTBOX) + " " +
               "BEGIN " +
               "  DELETE FROM " + IMmsSmsProvider::TABLE_PENDING_MSG +
               "  WHERE " + ITelephonyMmsSmsPendingMessages::MSG_ID + "=new._id; " +
               "END;"));

    // Insert pending status for M-Notification.ind or M-ReadRec.ind
    // when they are inserted into Inbox/Outbox.
    FAIL_RETURN(db->ExecSQL(String("DROP TRIGGER IF EXISTS insert_mms_pending_on_insert")));
    FAIL_RETURN(db->ExecSQL(String("CREATE TRIGGER insert_mms_pending_on_insert ") +
               "AFTER INSERT ON pdu " +
               "WHEN new." + ITelephonyBaseMmsColumns::MESSAGE_TYPE +
               "=" + StringUtils::ToString(IPduHeaders::MESSAGE_TYPE_NOTIFICATION_IND) +
               "  OR new." + ITelephonyBaseMmsColumns::MESSAGE_TYPE +
               "=" + StringUtils::ToString(IPduHeaders::MESSAGE_TYPE_READ_REC_IND) +
               " " +
               "BEGIN " +
               "  INSERT INTO " + IMmsSmsProvider::TABLE_PENDING_MSG +
               "    (" + ITelephonyMmsSmsPendingMessages::PROTO_TYPE + "," +
               "     " + ITelephonyMmsSmsPendingMessages::MSG_ID + "," +
               "     " + ITelephonyMmsSmsPendingMessages::MSG_TYPE + "," +
               "     " + ITelephonyMmsSmsPendingMessages::ERROR_TYPE + "," +
               "     " + ITelephonyMmsSmsPendingMessages::ERROR_CODE + "," +
               "     " + ITelephonyMmsSmsPendingMessages::RETRY_INDEX + "," +
               "     " + ITelephonyMmsSmsPendingMessages::DUE_TIME + ") " +
               "  VALUES " +
               "    (" + StringUtils::ToString(ITelephonyMmsSms::MMS_PROTO) + "," +
               "      new." + IBaseColumns::ID + "," +
               "      new." + ITelephonyBaseMmsColumns::MESSAGE_TYPE + ",0,0,0,0);" +
               "END;"));


    // Insert pending status for M-Send.req when it is moved into Outbox.
    FAIL_RETURN(db->ExecSQL(String("DROP TRIGGER IF EXISTS insert_mms_pending_on_update")));
    FAIL_RETURN(db->ExecSQL(String("CREATE TRIGGER insert_mms_pending_on_update ") +
               "AFTER UPDATE ON pdu " +
               "WHEN new." + ITelephonyBaseMmsColumns::MESSAGE_TYPE + "="
               + StringUtils::ToString(IPduHeaders::MESSAGE_TYPE_SEND_REQ) +
               "  AND new." + ITelephonyBaseMmsColumns::MESSAGE_BOX + "="
               + StringUtils::ToString(ITelephonyBaseMmsColumns::MESSAGE_BOX_OUTBOX) +
               "  AND old." + ITelephonyBaseMmsColumns::MESSAGE_BOX + "!="
               + StringUtils::ToString(ITelephonyBaseMmsColumns::MESSAGE_BOX_OUTBOX) + " " +
               "BEGIN " +
               "  INSERT INTO " + IMmsSmsProvider::TABLE_PENDING_MSG +
               "    (" + ITelephonyMmsSmsPendingMessages::PROTO_TYPE + "," +
               "     " + ITelephonyMmsSmsPendingMessages::MSG_ID + "," +
               "     " + ITelephonyMmsSmsPendingMessages::MSG_TYPE + "," +
               "     " + ITelephonyMmsSmsPendingMessages::ERROR_TYPE + "," +
               "     " + ITelephonyMmsSmsPendingMessages::ERROR_CODE + "," +
               "     " + ITelephonyMmsSmsPendingMessages::RETRY_INDEX + "," +
               "     " + ITelephonyMmsSmsPendingMessages::DUE_TIME + ") " +
               "  VALUES " +
               "    (" + StringUtils::ToString(ITelephonyMmsSms::MMS_PROTO) + "," +
               "      new." + IBaseColumns::ID + "," +
               "      new." + ITelephonyBaseMmsColumns::MESSAGE_TYPE + ",0,0,0,0);" +
               "END;"));

    // monitor the mms table
    FAIL_RETURN(db->ExecSQL(String("DROP TRIGGER IF EXISTS mms_words_update")));
    FAIL_RETURN(db->ExecSQL(String("CREATE TRIGGER mms_words_update AFTER UPDATE ON part BEGIN UPDATE words ") +
            " SET index_text = NEW.text WHERE (source_id=NEW._id AND table_to_use=2); " +
            " END;"));

    FAIL_RETURN(db->ExecSQL(String("DROP TRIGGER IF EXISTS mms_words_delete")));
    FAIL_RETURN(db->ExecSQL(String("CREATE TRIGGER mms_words_delete AFTER DELETE ON part BEGIN DELETE FROM ") +
            " words WHERE source_id = OLD._id AND table_to_use = 2; END;"));

    // Updates threads table whenever a message in pdu is updated.
    FAIL_RETURN(db->ExecSQL(String("DROP TRIGGER IF EXISTS pdu_update_thread_date_subject_on_update")));
    FAIL_RETURN(db->ExecSQL(String("CREATE TRIGGER pdu_update_thread_date_subject_on_update AFTER") +
               "  UPDATE OF " + ITelephonyBaseMmsColumns::DATE + ", " + ITelephonyBaseMmsColumns::SUBJECT + ", " + ITelephonyBaseMmsColumns::MESSAGE_BOX +
               "  ON " + IMmsProvider::TABLE_PDU + " " +
               PDU_UPDATE_THREAD_CONSTRAINTS +
               PDU_UPDATE_THREAD_DATE_SNIPPET_COUNT_ON_UPDATE));

    // Update threads table whenever a message in pdu is deleted
    FAIL_RETURN(db->ExecSQL(String("DROP TRIGGER IF EXISTS pdu_update_thread_on_delete")));
    FAIL_RETURN(db->ExecSQL(String("CREATE TRIGGER pdu_update_thread_on_delete ") +
                   "AFTER DELETE ON pdu " +
                   "BEGIN " +
                   "  UPDATE threads SET " +
                   "     date = (strftime('%s','now') * 1000)" +
                   "  WHERE threads._id = old." + ITelephonyBaseMmsColumns::THREAD_ID + "; " +
                   UPDATE_THREAD_COUNT_ON_OLD +
                   UPDATE_THREAD_SNIPPET_SNIPPET_CS_ON_DELETE +
                   "END;"));

    // Updates threads table whenever a message is added to pdu.
    FAIL_RETURN(db->ExecSQL(String("DROP TRIGGER IF EXISTS pdu_update_thread_on_insert")));
    FAIL_RETURN(db->ExecSQL(String("CREATE TRIGGER pdu_update_thread_on_insert AFTER INSERT ON ") +
                   IMmsProvider::TABLE_PDU + " " +
                   PDU_UPDATE_THREAD_CONSTRAINTS +
                   PDU_UPDATE_THREAD_DATE_SNIPPET_COUNT_ON_UPDATE));

    // Updates threads table whenever a message in pdu is updated.
    FAIL_RETURN(db->ExecSQL(String("DROP TRIGGER IF EXISTS pdu_update_thread_read_on_update")));
    FAIL_RETURN(db->ExecSQL(String("CREATE TRIGGER pdu_update_thread_read_on_update AFTER") +
                   "  UPDATE OF " + ITelephonyBaseMmsColumns::READ +
                   "  ON " + IMmsProvider::TABLE_PDU + " " +
                   PDU_UPDATE_THREAD_CONSTRAINTS +
                   "BEGIN " +
                   PDU_UPDATE_THREAD_READ_BODY +
                   "END;"));

    // Update the error flag of threads when delete pending message.
    FAIL_RETURN(db->ExecSQL(String("DROP TRIGGER IF EXISTS update_threads_error_on_delete_mms")));
    FAIL_RETURN(db->ExecSQL(String("CREATE TRIGGER update_threads_error_on_delete_mms ") +
                   "  BEFORE DELETE ON pdu" +
                   "  WHEN OLD._id IN (SELECT DISTINCT msg_id" +
                   "                   FROM pending_msgs" +
                   "                   WHERE err_type >= 10) " +
                   "BEGIN " +
                   "  UPDATE threads SET error = error - 1" +
                   "  WHERE _id = OLD.thread_id; " +
                   "END;"));

    // Update the error flag of threads while moving an MM out of Outbox,
    // which was failed to be sent permanently.
    FAIL_RETURN(db->ExecSQL(String("DROP TRIGGER IF EXISTS update_threads_error_on_move_mms")));
    return db->ExecSQL(String("CREATE TRIGGER update_threads_error_on_move_mms ") +
               "  BEFORE UPDATE OF msg_box ON pdu " +
               "  WHEN (OLD.msg_box = 4 AND NEW.msg_box != 4) " +
               "  AND (OLD._id IN (SELECT DISTINCT msg_id" +
               "                   FROM pending_msgs" +
               "                   WHERE err_type >= 10)) " +
               "BEGIN " +
               "  UPDATE threads SET error = error - 1" +
               "  WHERE _id = OLD.thread_id; " +
               "END;");
}

void MmsSmsDatabaseHelper::CreateSmsTables(
    /* [in] */ ISQLiteDatabase* db)
{
    // N.B.: Whenever the columns here are changed, the columns in
    // {@ref MmsSmsProvider} must be changed to match.
    db->ExecSQL(String("CREATE TABLE sms (") +
               "_id INTEGER PRIMARY KEY," +
               "thread_id INTEGER," +
               "address TEXT," +
               "person INTEGER," +
               "date INTEGER," +
               "date_sent INTEGER DEFAULT 0," +
               "protocol INTEGER," +
               "read INTEGER DEFAULT 0," +
               "status INTEGER DEFAULT -1," + // a TP-Status value
                                              // or -1 if it
                                              // status hasn't
                                              // been received
               "type INTEGER," +
               "reply_path_present INTEGER," +
               "subject TEXT," +
               "body TEXT," +
               "service_center TEXT," +
               "locked INTEGER DEFAULT 0," +
               "phone_id INTEGER DEFAULT -1, " +
               "error_code INTEGER DEFAULT 0," +
               "creator TEXT," +
               "seen INTEGER DEFAULT 0," +
               "priority INTEGER DEFAULT -1" +
               ");");

    /**
     * This table is used by the SMS dispatcher to hold
     * incomplete partial messages until all the parts arrive.
     */
    db->ExecSQL(String("CREATE TABLE raw (") +
               "_id INTEGER PRIMARY KEY," +
               "date INTEGER," +
               "reference_number INTEGER," + // one per full message
               "count INTEGER," + // the number of parts
               "sequence INTEGER," + // the part number of this message
               "destination_port INTEGER," +
               "address TEXT," +
               "phone_id INTEGER DEFAULT -1, " +
               "pdu TEXT);"); // the raw PDU for this part

    db->ExecSQL(String("CREATE TABLE attachments (") +
               "sms_id INTEGER," +
               "content_url TEXT," +
               "offset INTEGER);");

    /**
     * This table is used by the SMS dispatcher to hold pending
     * delivery status report intents.
     */
    db->ExecSQL(String("CREATE TABLE sr_pending (") +
               "reference_number INTEGER," +
               "action TEXT," +
               "data TEXT);");
}

void MmsSmsDatabaseHelper::CreateCommonTables(
    /* [in] */ ISQLiteDatabase* db)
{
    // TODO Ensure that each entry is removed when the last use of
    // any address equivalent to its address is removed.

    /**
     * This table maps the first instance seen of any particular
     * MMS/SMS address to an ID, which is then used as its
     * canonical representation.  If the same address or an
     * equivalent address (as determined by our Sqlite
     * PHONE_NUMBERS_EQUAL extension) is seen later, this same ID
     * will be used. The _id is created with AUTOINCREMENT so it
     * will never be reused again if a recipient is deleted.
     */
    db->ExecSQL(String("CREATE TABLE canonical_addresses (") +
               "_id INTEGER PRIMARY KEY AUTOINCREMENT," +
               "address TEXT);");

    /**
     * This table maps the subject and an ordered set of recipient
     * IDs, separated by spaces, to a unique thread ID.  The IDs
     * come from the canonical_addresses table.  This works
     * because messages are considered to be part of the same
     * thread if they have the same subject (or a NULL subject)
     * and the same set of recipients.
     */
    db->ExecSQL(String("CREATE TABLE threads (") +
               IBaseColumns::ID + " INTEGER PRIMARY KEY AUTOINCREMENT," +
               ITelephonyThreadsColumns::DATE + " INTEGER DEFAULT 0," +
               ITelephonyThreadsColumns::MESSAGE_COUNT + " INTEGER DEFAULT 0," +
               ITelephonyThreadsColumns::RECIPIENT_IDS + " TEXT," +
               ITelephonyThreadsColumns::SNIPPET + " TEXT," +
               ITelephonyThreadsColumns::SNIPPET_CHARSET + " INTEGER DEFAULT 0," +
               ITelephonyThreadsColumns::READ + " INTEGER DEFAULT 1," +
               ITelephonyThreadsColumns::ARCHIVED + " INTEGER DEFAULT 0," +
               ITelephonyThreadsColumns::TYPE + " INTEGER DEFAULT 0," +
               ITelephonyThreadsColumns::ERROR + " INTEGER DEFAULT 0," +
               ITelephonyThreadsColumns::HAS_ATTACHMENT + " INTEGER DEFAULT 0);");

    /**
     * This table stores the queue of messages to be sent/downloaded.
     */
    db->ExecSQL(String("CREATE TABLE ") + IMmsSmsProvider::TABLE_PENDING_MSG +" (" +
               IBaseColumns::ID + " INTEGER PRIMARY KEY," +
               ITelephonyMmsSmsPendingMessages::PROTO_TYPE + " INTEGER," +
               ITelephonyMmsSmsPendingMessages::MSG_ID + " INTEGER," +
               ITelephonyMmsSmsPendingMessages::MSG_TYPE + " INTEGER," +
               ITelephonyMmsSmsPendingMessages::ERROR_TYPE + " INTEGER," +
               ITelephonyMmsSmsPendingMessages::ERROR_CODE + " INTEGER," +
               ITelephonyMmsSmsPendingMessages::RETRY_INDEX + " INTEGER NOT NULL DEFAULT 0," +
               ITelephonyMmsSmsPendingMessages::DUE_TIME + " INTEGER," +
               ITelephonyMmsSmsPendingMessages::PHONE_ID + " INTEGER DEFAULT 0, " +
               ITelephonyMmsSmsPendingMessages::LAST_TRY + " INTEGER);");
}

// TODO Check the query plans for these triggers.
void MmsSmsDatabaseHelper::CreateCommonTriggers(
    /* [in] */ ISQLiteDatabase* db)
{
    // Updates threads table whenever a message is added to sms.
    db->ExecSQL(String("CREATE TRIGGER sms_update_thread_on_insert AFTER INSERT ON sms ") +
               SMS_UPDATE_THREAD_DATE_SNIPPET_COUNT_ON_UPDATE);

    // Updates threads table whenever a message in sms is updated.
    db->ExecSQL(String("CREATE TRIGGER sms_update_thread_date_subject_on_update AFTER") +
               "  UPDATE OF " + ITelephonyTextBasedSmsColumns::DATE + ", " + ITelephonyTextBasedSmsColumns::BODY
               + ", " + ITelephonyTextBasedSmsColumns::TYPE +
               "  ON sms " +
               SMS_UPDATE_THREAD_DATE_SNIPPET_COUNT_ON_UPDATE);

    // Updates threads table whenever a message in sms is updated.
    db->ExecSQL(String("CREATE TRIGGER sms_update_thread_read_on_update AFTER") +
               "  UPDATE OF " + ITelephonyTextBasedSmsColumns::READ +
               "  ON sms " +
               "BEGIN " +
               SMS_UPDATE_THREAD_READ_BODY +
               "END;");

    // As of DATABASE_VERSION 55, we've removed these triggers that delete empty threads.
    // These triggers interfere with saving drafts on brand new threads. Instead of
    // triggers cleaning up empty threads, the empty threads should be cleaned up by
    // an explicit call to delete with Threads.OBSOLETE_THREADS_URI.

//        // When the last message in a thread is deleted, these
//        // triggers ensure that the entry for its thread ID is removed
//        // from the threads table.
//        db->ExecSQL("CREATE TRIGGER delete_obsolete_threads_pdu " +
//                   "AFTER DELETE ON pdu " +
//                   "BEGIN " +
//                   "  DELETE FROM threads " +
//                   "  WHERE " +
//                   "    _id = old.thread_id " +
//                   "    AND _id NOT IN " +
//                   "    (SELECT thread_id FROM sms " +
//                   "     UNION SELECT thread_id from pdu); " +
//                   "END;");
//
//        db->ExecSQL("CREATE TRIGGER delete_obsolete_threads_when_update_pdu " +
//                   "AFTER UPDATE OF " + Mms.THREAD_ID + " ON pdu " +
//                   "WHEN old." + Mms.THREAD_ID + " != new." + Mms.THREAD_ID + " " +
//                   "BEGIN " +
//                   "  DELETE FROM threads " +
//                   "  WHERE " +
//                   "    _id = old.thread_id " +
//                   "    AND _id NOT IN " +
//                   "    (SELECT thread_id FROM sms " +
//                   "     UNION SELECT thread_id from pdu); " +
//                   "END;");

    // TODO Add triggers for SMS retry-status management.

    // Update the error flag of threads when the error type of
    // a pending MM is updated.
    db->ExecSQL(String("CREATE TRIGGER update_threads_error_on_update_mms ") +
               "  AFTER UPDATE OF err_type ON pending_msgs " +
               "  WHEN (OLD.err_type < 10 AND NEW.err_type >= 10)" +
               "    OR (OLD.err_type >= 10 AND NEW.err_type < 10) " +
               "BEGIN" +
               "  UPDATE threads SET error = " +
               "    CASE" +
               "      WHEN NEW.err_type >= 10 THEN error + 1" +
               "      ELSE error - 1" +
               "    END " +
               "  WHERE _id =" +
               "   (SELECT DISTINCT thread_id" +
               "    FROM pdu" +
               "    WHERE _id = NEW.msg_id); " +
               "END;");

    // Update the error flag of threads after a text message was
    // failed to send/receive.
    db->ExecSQL(String("CREATE TRIGGER update_threads_error_on_update_sms ") +
               "  AFTER UPDATE OF type ON sms" +
               "  WHEN (OLD.type != 5 AND NEW.type = 5)" +
               "    OR (OLD.type = 5 AND NEW.type != 5) " +
               "BEGIN " +
               "  UPDATE threads SET error = " +
               "    CASE" +
               "      WHEN NEW.type = 5 THEN error + 1" +
               "      ELSE error - 1" +
               "    END " +
               "  WHERE _id = NEW.thread_id; " +
               "END;");
}

// @Override
ECode MmsSmsDatabaseHelper::OnUpgrade(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 oldVersion,
    /* [in] */ Int32 currentVersion)
{
    Logger::W(TAG, "Upgrading database from version %d to %d.", oldVersion, currentVersion);

    switch (oldVersion) {
        case 40:
            if (currentVersion <= 40) {
                return NOERROR;
            }

            db->BeginTransaction();
            // try {
                UpgradeDatabaseToVersion41(db);
                db->SetTransactionSuccessful();
            // } catch (Throwable ex) {
            //     Logger::E(TAG, ex.getMessage(), ex);
            //     break;
            // } finally {
                db->EndTransaction();
            // }
            // fall through
        case 41:
            if (currentVersion <= 41) {
                return NOERROR;
            }

            db->BeginTransaction();
            // try {
                UpgradeDatabaseToVersion42(db);
                db->SetTransactionSuccessful();
            // } catch (Throwable ex) {
            //     Logger::E(TAG, ex.getMessage(), ex);
            //     break;
            // } finally {
                db->EndTransaction();
            // }
            // fall through
        case 42:
            if (currentVersion <= 42) {
                return NOERROR;
            }

            db->BeginTransaction();
            // try {
                UpgradeDatabaseToVersion43(db);
                db->SetTransactionSuccessful();
            // } catch (Throwable ex) {
            //     Logger::E(TAG, ex.getMessage(), ex);
            //     break;
            // } finally {
                db->EndTransaction();
            // }
            // fall through
        case 43:
            if (currentVersion <= 43) {
                return NOERROR;
            }

            db->BeginTransaction();
            // try {
                UpgradeDatabaseToVersion44(db);
                db->SetTransactionSuccessful();
            // } catch (Throwable ex) {
            //     Logger::E(TAG, ex.getMessage(), ex);
            //     break;
            // } finally {
                db->EndTransaction();
            // }
            // fall through
        case 44:
            if (currentVersion <= 44) {
                return NOERROR;
            }

            db->BeginTransaction();
            // try {
                UpgradeDatabaseToVersion45(db);
                db->SetTransactionSuccessful();
            // } catch (Throwable ex) {
            //     Logger::E(TAG, ex.getMessage(), ex);
            //     break;
            // } finally {
                db->EndTransaction();
            // }
            // fall through
        case 45:
            if (currentVersion <= 45) {
                return NOERROR;
            }
            db->BeginTransaction();
            // try {
                UpgradeDatabaseToVersion46(db);
                db->SetTransactionSuccessful();
            // } catch (Throwable ex) {
            //     Logger::E(TAG, ex.getMessage(), ex);
            //     break;
            // } finally {
                db->EndTransaction();
            // }
            // fall through
        case 46:
            if (currentVersion <= 46) {
                return NOERROR;
            }

            db->BeginTransaction();
            // try {
                UpgradeDatabaseToVersion47(db);
                db->SetTransactionSuccessful();
            // } catch (Throwable ex) {
            //     Logger::E(TAG, ex.getMessage(), ex);
            //     break;
            // } finally {
                db->EndTransaction();
            // }
            // fall through
        case 47:
            if (currentVersion <= 47) {
                return NOERROR;
            }

            db->BeginTransaction();
            // try {
                UpgradeDatabaseToVersion48(db);
                db->SetTransactionSuccessful();
            // } catch (Throwable ex) {
            //     Logger::E(TAG, ex.getMessage(), ex);
            //     break;
            // } finally {
                db->EndTransaction();
            // }
            // fall through
        case 48:
            if (currentVersion <= 48) {
                return NOERROR;
            }

            db->BeginTransaction();
            // try {
                CreateWordsTables(db);
                db->SetTransactionSuccessful();
            // } catch (Throwable ex) {
            //     Logger::E(TAG, ex.getMessage(), ex);
            //     break;
            // } finally {
                db->EndTransaction();
            // }
            // fall through
        case 49:
            if (currentVersion <= 49) {
                return NOERROR;
            }
            db->BeginTransaction();
            // try {
                CreateThreadIdIndex(db);
                db->SetTransactionSuccessful();
            // } catch (Throwable ex) {
            //     Logger::E(TAG, ex.getMessage(), ex);
            //     break; // force to destroy all old data;
            // } finally {
                db->EndTransaction();
            // }
            // fall through
        case 50:
            if (currentVersion <= 50) {
                return NOERROR;
            }

            db->BeginTransaction();
            // try {
                UpgradeDatabaseToVersion51(db);
                db->SetTransactionSuccessful();
            // } catch (Throwable ex) {
            //     Logger::E(TAG, ex.getMessage(), ex);
            //     break;
            // } finally {
                db->EndTransaction();
            // }
            // fall through
        case 51:
            if (currentVersion <= 51) {
                return NOERROR;
            }
            // 52 was adding a new meta_data column, but that was removed.
            // fall through
        case 52:
            if (currentVersion <= 52) {
                return NOERROR;
            }

            db->BeginTransaction();
            // try {
                UpgradeDatabaseToVersion53(db);
                db->SetTransactionSuccessful();
            // } catch (Throwable ex) {
            //     Logger::E(TAG, ex.getMessage(), ex);
            //     break;
            // } finally {
                db->EndTransaction();
            // }
            // fall through
        case 53:
            if (currentVersion <= 53) {
                return NOERROR;
            }

            db->BeginTransaction();
            // try {
                UpgradeDatabaseToVersion54(db);
                db->SetTransactionSuccessful();
            // } catch (Throwable ex) {
            //     Logger::E(TAG, ex.getMessage(), ex);
            //     break;
            // } finally {
                db->EndTransaction();
            // }
            // fall through
        case 54:
            if (currentVersion <= 54) {
                return NOERROR;
            }

            db->BeginTransaction();
            // try {
                UpgradeDatabaseToVersion55(db);
                db->SetTransactionSuccessful();
            // } catch (Throwable ex) {
            //     Logger::E(TAG, ex.getMessage(), ex);
            //     break;
            // } finally {
                db->EndTransaction();
            // }
            // fall through
        case 55:
            if (currentVersion <= 55) {
                return NOERROR;
            }

            db->BeginTransaction();
            // try {
                UpgradeDatabaseToVersion56(db);
                db->SetTransactionSuccessful();
            // } catch (Throwable ex) {
            //     Logger::E(TAG, ex.getMessage(), ex);
            //     break;
            // } finally {
                db->EndTransaction();
            // }
            // fall through
        case 56:
            if (currentVersion <= 56) {
                return NOERROR;
            }

            db->BeginTransaction();
            // try {
                UpgradeDatabaseToVersion57(db);
                db->SetTransactionSuccessful();
            // } catch (Throwable ex) {
            //     Logger::E(TAG, ex.getMessage(), ex);
            //     break;
            // } finally {
                db->EndTransaction();
            // }
            // fall through
        case 57:
            if (currentVersion <= 57) {
                return NOERROR;
            }

            db->BeginTransaction();
            // try {
                UpgradeDatabaseToVersion58(db);
                db->SetTransactionSuccessful();
            // } catch (Throwable ex) {
            //     Logger::E(TAG, ex.getMessage(), ex);
            //     break;
            // } finally {
                db->EndTransaction();
            // }
            // fall through
        case 58:
            if (currentVersion <= 58) {
                return NOERROR;
            }

            db->BeginTransaction();
            // try {
                UpgradeDatabaseToVersion59(db);
                db->SetTransactionSuccessful();
            // } catch (Throwable ex) {
            //     Logger::E(TAG, ex.getMessage(), ex);
            //     break;
            // } finally {
                db->EndTransaction();
            // }
            // fall through
        case 59:
            if (currentVersion <= 59) {
                return NOERROR;
            }

            db->BeginTransaction();
            // try {
                UpgradeDatabaseToVersion60(db);
                db->SetTransactionSuccessful();
            // } catch (Throwable ex) {
            //     Logger::E(TAG, ex.getMessage(), ex);
            //     break;
            // } finally {
                db->EndTransaction();
            // }
            // fall through
        case 60:
            if (currentVersion <= 60) {
                return NOERROR;
            }

            db->BeginTransaction();
            // try {
                UpgradeDatabaseToVersion61(db);
                db->SetTransactionSuccessful();
            // } catch (Throwable ex) {
            //     Logger::E(TAG, ex.getMessage(), ex);
            //     break;
            // } finally {
                db->EndTransaction();
            // }
        case 61:
            if (currentVersion <= 61) {
                return NOERROR;
            }

            db->BeginTransaction();
            // try {
                UpgradeDatabaseToVersion62(db);
                db->SetTransactionSuccessful();
            // } catch (Throwable ex) {
            //     Logger::E(TAG, ex.getMessage(), ex);
            //     break;
            // } finally {
                db->EndTransaction();
            // }
            return NOERROR;
    }

    Logger::E(TAG, "Destroying all old data.");
    DropAll(db);
    OnCreate(db);
    return NOERROR;
}

void MmsSmsDatabaseHelper::DropAll(
    /* [in] */ ISQLiteDatabase* db)
{
    // Clean the database out in order to start over from scratch.
    // We don't need to drop our triggers here because SQLite automatically
    // drops a trigger when its attached database is dropped.
    db->ExecSQL(String("DROP TABLE IF EXISTS canonical_addresses"));
    db->ExecSQL(String("DROP TABLE IF EXISTS threads"));
    db->ExecSQL(String("DROP TABLE IF EXISTS ") + IMmsSmsProvider::TABLE_PENDING_MSG);
    db->ExecSQL(String("DROP TABLE IF EXISTS sms"));
    db->ExecSQL(String("DROP TABLE IF EXISTS raw"));
    db->ExecSQL(String("DROP TABLE IF EXISTS attachments"));
    db->ExecSQL(String("DROP TABLE IF EXISTS thread_ids"));
    db->ExecSQL(String("DROP TABLE IF EXISTS sr_pending"));
    db->ExecSQL(String("DROP TABLE IF EXISTS ") + IMmsProvider::TABLE_PDU + ";");
    db->ExecSQL(String("DROP TABLE IF EXISTS ") + IMmsProvider::TABLE_ADDR + ";");
    db->ExecSQL(String("DROP TABLE IF EXISTS ") + IMmsProvider::TABLE_PART + ";");
    db->ExecSQL(String("DROP TABLE IF EXISTS ") + IMmsProvider::TABLE_RATE + ";");
    db->ExecSQL(String("DROP TABLE IF EXISTS ") + IMmsProvider::TABLE_DRM + ";");
}

void MmsSmsDatabaseHelper::UpgradeDatabaseToVersion41(
    /* [in] */ ISQLiteDatabase* db)
{
    db->ExecSQL(String("DROP TRIGGER IF EXISTS update_threads_error_on_move_mms"));
    db->ExecSQL(String("CREATE TRIGGER update_threads_error_on_move_mms ") +
               "  BEFORE UPDATE OF msg_box ON pdu " +
               "  WHEN (OLD.msg_box = 4 AND NEW.msg_box != 4) " +
               "  AND (OLD._id IN (SELECT DISTINCT msg_id" +
               "                   FROM pending_msgs" +
               "                   WHERE err_type >= 10)) " +
               "BEGIN " +
               "  UPDATE threads SET error = error - 1" +
               "  WHERE _id = OLD.thread_id; " +
               "END;");
}

void MmsSmsDatabaseHelper::UpgradeDatabaseToVersion42(
    /* [in] */ ISQLiteDatabase* db)
{
    db->ExecSQL(String("DROP TRIGGER IF EXISTS sms_update_thread_on_delete"));
    db->ExecSQL(String("DROP TRIGGER IF EXISTS delete_obsolete_threads_sms"));
    db->ExecSQL(String("DROP TRIGGER IF EXISTS update_threads_error_on_delete_sms"));
}

void MmsSmsDatabaseHelper::UpgradeDatabaseToVersion43(
    /* [in] */ ISQLiteDatabase* db)
{
    // Add 'has_attachment' column to threads table.
    db->ExecSQL(String("ALTER TABLE threads ADD COLUMN has_attachment INTEGER DEFAULT 0"));

    UpdateThreadsAttachmentColumn(db);

    // Add insert and delete triggers for keeping it up to date.
    db->ExecSQL(PART_UPDATE_THREADS_ON_INSERT_TRIGGER);
    db->ExecSQL(PART_UPDATE_THREADS_ON_DELETE_TRIGGER);
}

void MmsSmsDatabaseHelper::UpgradeDatabaseToVersion44(
    /* [in] */ ISQLiteDatabase* db)
{
    UpdateThreadsAttachmentColumn(db);

    // add the update trigger for keeping the threads up to date.
    db->ExecSQL(PART_UPDATE_THREADS_ON_UPDATE_TRIGGER);
}

void MmsSmsDatabaseHelper::UpgradeDatabaseToVersion45(
    /* [in] */ ISQLiteDatabase* db)
{
    // Add 'locked' column to sms table.
    db->ExecSQL(String("ALTER TABLE sms ADD COLUMN ") + ITelephonyTextBasedSmsColumns::LOCKED + " INTEGER DEFAULT 0");

    // Add 'locked' column to pdu table.
    db->ExecSQL(String("ALTER TABLE pdu ADD COLUMN ") + ITelephonyBaseMmsColumns::LOCKED + " INTEGER DEFAULT 0");
}

void MmsSmsDatabaseHelper::UpgradeDatabaseToVersion46(
    /* [in] */ ISQLiteDatabase* db)
{
    // add the "text" column for caching inline text (e.g. strings) instead of
    // putting them in an external file
    db->ExecSQL(String("ALTER TABLE part ADD COLUMN ") + ITelephonyMmsPart::TEXT + " TEXT");

    AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(3);
    (*ss)[0] = IBaseColumns::ID;
    (*ss)[1] = ITelephonyMmsPart::_DATA;
    (*ss)[2] = ITelephonyMmsPart::TEXT;
    AutoPtr<ICursor> textRows;
    db->Query(
            String("part"),
            ss,
            String("ct = 'text/plain' OR ct == 'application/smil'"),
            NULL,
            String(NULL),
            String(NULL),
            String(NULL), (ICursor**)&textRows);
    AutoPtr<IArrayList/*<String*/> filesToDelete;
    CArrayList::New((IArrayList**)&filesToDelete);
    do {
        db->BeginTransaction();
        if (textRows != NULL) {
            Int32 partDataColumn = 0;
            if (FAILED(textRows->GetColumnIndex(ITelephonyMmsPart::_DATA, &partDataColumn))) {
                break;
            }

            // This code is imperfect in that we can't guarantee that all the
            // backing files get deleted.  For example if the system aborts after
            // the database is updated but before we complete the process of
            // deleting files.
            Boolean tmp = FALSE;
            while (textRows->MoveToNext(&tmp), tmp) {
                String path;
                textRows->GetString(partDataColumn, &path);
                if (path != NULL) {
                    // try {
                    AutoPtr<IInputStream> is;
                    CFileInputStream::New(path, (IInputStream**)&is);
                    Int32 len = 0;
                    is->Available(&len);
                    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(len);
                    Int32 num = 0;
                    is->Read(data, &num);
                    AutoPtr<IEncodedStringValue> v;
                    CEncodedStringValue::New(data, (IEncodedStringValue**)&v);
                    String s;
                    v->GetString(&s);
                    AutoPtr<ArrayOf<IInterface*> > s2 = ArrayOf<IInterface*>::Alloc(1);
                    s2->Set(0, CoreUtils::Convert(s));
                    db->ExecSQL(String("UPDATE part SET ") + ITelephonyMmsPart::_DATA + " = NULL, " +
                            ITelephonyMmsPart::TEXT + " = ?", s2);
                    is->Close();
                    filesToDelete->Add(CoreUtils::Convert(path));
                    // } catch (IOException e) {
                    //     // TODO Auto-generated catch block
                    //     e.printStackTrace();
                    // }
                }
            }
        }
        db->SetTransactionSuccessful();
    } while(0);

    {
        db->EndTransaction();
        Int32 size = 0;
        filesToDelete->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            filesToDelete->Get(i, (IInterface**)&obj);
            String pathToDelete = TO_STR(obj);
            // try {
            AutoPtr<IFile> f;
            CFile::New(pathToDelete, (IFile**)&f);
            f->Delete();
            // } catch (SecurityException ex) {
            //     Logger::E(TAG, "unable to clean up old mms file for " + pathToDelete, ex);
            // }
        }
        if (textRows != NULL) {
            ICloseable::Probe(textRows)->Close();
        }
    }
}

void MmsSmsDatabaseHelper::UpgradeDatabaseToVersion47(
    /* [in] */ ISQLiteDatabase* db)
{
    UpdateThreadsAttachmentColumn(db);

    // add the update trigger for keeping the threads up to date.
    db->ExecSQL(PDU_UPDATE_THREADS_ON_UPDATE_TRIGGER);
}

void MmsSmsDatabaseHelper::UpgradeDatabaseToVersion48(
    /* [in] */ ISQLiteDatabase* db)
{
    // Add 'error_code' column to sms table.
    db->ExecSQL(String("ALTER TABLE sms ADD COLUMN error_code INTEGER DEFAULT 0"));
}

void MmsSmsDatabaseHelper::UpgradeDatabaseToVersion51(
    /* [in] */ ISQLiteDatabase* db)
{
    db->ExecSQL(String("ALTER TABLE sms add COLUMN seen INTEGER DEFAULT 0"));
    db->ExecSQL(String("ALTER TABLE pdu add COLUMN seen INTEGER DEFAULT 0"));

    // try {
        // update the existing sms and pdu tables so the new "seen" column is the same as
        // the "read" column for each row.
        AutoPtr<IContentValues> contentValues;
        CContentValues::New((IContentValues**)&contentValues);
        contentValues->Put(String("seen"), 1);
        Int32 count = 0;
        db->Update(String("sms"), contentValues, String("read=1"), NULL, &count);
        Logger::D(TAG, "[MmsSmsDb] UpgradeDatabaseToVersion51: updated %d rows in sms table to have READ=1"
                , count);
        db->Update(String("pdu"), contentValues, String("read=1"), NULL, &count);
        Logger::D(TAG, "[MmsSmsDb] UpgradeDatabaseToVersion51: updated %d rows in pdu table to have READ=1",
                 count);
    // } catch (Exception ex) {
    //     Logger::E(TAG, "[MmsSmsDb] UpgradeDatabaseToVersion51 caught ", ex);
    // }
}

void MmsSmsDatabaseHelper::UpgradeDatabaseToVersion53(
    /* [in] */ ISQLiteDatabase* db)
{
    db->ExecSQL(String("DROP TRIGGER IF EXISTS pdu_update_thread_read_on_update"));

    // Updates threads table whenever a message in pdu is updated.
    db->ExecSQL(String("CREATE TRIGGER pdu_update_thread_read_on_update AFTER") +
               "  UPDATE OF " + ITelephonyBaseMmsColumns::READ +
               "  ON " + IMmsProvider::TABLE_PDU + " " +
               PDU_UPDATE_THREAD_CONSTRAINTS +
               "BEGIN " +
               PDU_UPDATE_THREAD_READ_BODY +
               "END;");
}

void MmsSmsDatabaseHelper::UpgradeDatabaseToVersion54(
    /* [in] */ ISQLiteDatabase* db)
{
    // Add 'date_sent' column to sms table.
    db->ExecSQL(String("ALTER TABLE sms ADD COLUMN ") + ITelephonyTextBasedSmsColumns::DATE_SENT + " INTEGER DEFAULT 0");

    // Add 'date_sent' column to pdu table.
    db->ExecSQL(String("ALTER TABLE pdu ADD COLUMN ") + ITelephonyBaseMmsColumns::DATE_SENT + " INTEGER DEFAULT 0");
}

void MmsSmsDatabaseHelper::UpgradeDatabaseToVersion55(
    /* [in] */ ISQLiteDatabase* db)
{
    // Drop removed triggers
    db->ExecSQL(String("DROP TRIGGER IF EXISTS delete_obsolete_threads_pdu"));
    db->ExecSQL(String("DROP TRIGGER IF EXISTS delete_obsolete_threads_when_update_pdu"));
}

void MmsSmsDatabaseHelper::UpgradeDatabaseToVersion56(
    /* [in] */ ISQLiteDatabase* db)
{
    // Add 'text_only' column to pdu table.
    db->ExecSQL(String("ALTER TABLE ") + IMmsProvider::TABLE_PDU + " ADD COLUMN " + ITelephonyBaseMmsColumns::TEXT_ONLY +
            " INTEGER DEFAULT 0");
}

void MmsSmsDatabaseHelper::UpgradeDatabaseToVersion57(
    /* [in] */ ISQLiteDatabase* db)
{
    // Clear out bad rows, those with empty threadIds, from the pdu table.
    db->ExecSQL(String("DELETE FROM ") + IMmsProvider::TABLE_PDU + " WHERE " + ITelephonyBaseMmsColumns::THREAD_ID + " IS NULL");
}

void MmsSmsDatabaseHelper::UpgradeDatabaseToVersion58(
    /* [in] */ ISQLiteDatabase* db)
{
    db->ExecSQL(String("ALTER TABLE ") + IMmsProvider::TABLE_PDU +" ADD COLUMN "
            + ITelephonyBaseMmsColumns::PHONE_ID + " INTEGER DEFAULT -1");
    db->ExecSQL(String("ALTER TABLE ") + IMmsSmsProvider::TABLE_PENDING_MSG +" ADD COLUMN "
            + "pending_phone_id" + " INTEGER DEFAULT 0");
    db->ExecSQL(String("ALTER TABLE ") + CSmsProvider::TABLE_SMS +" ADD COLUMN "
            + ITelephonyTextBasedSmsColumns::PHONE_ID + " INTEGER DEFAULT -1");
    db->ExecSQL(String("ALTER TABLE ") + CSmsProvider::TABLE_RAW +" ADD COLUMN "
            + ITelephonyTextBasedSmsColumns::PHONE_ID + " INTEGER DEFAULT -1");
}

void MmsSmsDatabaseHelper::UpgradeDatabaseToVersion59(
    /* [in] */ ISQLiteDatabase* db)
{
    db->ExecSQL(String("ALTER TABLE ") + IMmsProvider::TABLE_PDU +" ADD COLUMN "
            + ITelephonyBaseMmsColumns::CREATOR + " TEXT");
    db->ExecSQL(String("ALTER TABLE ") + CSmsProvider::TABLE_SMS +" ADD COLUMN "
            + ITelephonyTextBasedSmsColumns::CREATOR + " TEXT");
}

void MmsSmsDatabaseHelper::UpgradeDatabaseToVersion60(
    /* [in] */ ISQLiteDatabase* db)
{
    db->ExecSQL(String("ALTER TABLE ") + IMmsSmsProvider::TABLE_THREADS +" ADD COLUMN "
            + ITelephonyThreadsColumns::ARCHIVED + " INTEGER DEFAULT 0");
}

void MmsSmsDatabaseHelper::UpgradeDatabaseToVersion61(
    /* [in] */ ISQLiteDatabase* db)
{
    // try {
        if (FAILED(db->ExecSQL(String("ALTER TABLE ") + CSmsProvider::TABLE_SMS + " ADD COLUMN "
                        + "priority INTEGER DEFAULT -1"))) {
            Logger::E(TAG, "UpgradeDatabaseToVersion61 error.");
        }
    // } catch (Exception e) {
    //     // Some db with version < 61 might already have priority column.
    //     // So gracefully catch exception.
    //     Logger::E(TAG, "UpgradeDatabaseToVersion61: ex. ", e);
    // }
}

void MmsSmsDatabaseHelper::UpgradeDatabaseToVersion62(
    /* [in] */ ISQLiteDatabase* db)
{
    if (IsCM11DB(db)) {
       // CM11 was 60, which means we skipped a few updates...
       // try {
        UpgradeDatabaseToVersion58(db);
       // } catch (Exception e) { }
       // try {
        UpgradeDatabaseToVersion59(db);
       // } catch (Exception e) { }
       // try {
        UpgradeDatabaseToVersion60(db);
       // } catch (Exception e) { }
       // try {
        UpgradeDatabaseToVersion61(db);
       // } catch (Exception e) { }
    }
}

// @Override
ECode MmsSmsDatabaseHelper::GetWritableDatabase(
    /* [out] */ ISQLiteDatabase** result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(THIS);
    AutoPtr<ISQLiteDatabase> db;
    SQLiteOpenHelper::GetWritableDatabase((ISQLiteDatabase**)&db);

    if (!sTriedAutoIncrement) {
        sTriedAutoIncrement = TRUE;
        Boolean hasAutoIncrementThreads = HasAutoIncrement(db, IMmsSmsProvider::TABLE_THREADS);
        Boolean hasAutoIncrementAddresses = HasAutoIncrement(db, String("canonical_addresses"));
        Boolean hasAutoIncrementPart = HasAutoIncrement(db, String("part"));
        Boolean hasAutoIncrementPdu = HasAutoIncrement(db, String("pdu"));
        Logger::D(TAG, "[getWritableDatabase] hasAutoIncrementThreads: %d hasAutoIncrementAddresses: %d hasAutoIncrementPart: %d hasAutoIncrementPdu: %d"
            , hasAutoIncrementThreads, hasAutoIncrementAddresses, hasAutoIncrementPart, hasAutoIncrementPdu);
        Boolean autoIncrementThreadsSuccess = TRUE;
        Boolean autoIncrementAddressesSuccess = TRUE;
        Boolean autoIncrementPartSuccess = TRUE;
        Boolean autoIncrementPduSuccess = TRUE;
        if (!hasAutoIncrementThreads) {
            db->BeginTransaction();
            do {
                if (FALSE && sFakeLowStorageTest) {
                    Logger::D(TAG, "[getWritableDatabase] mFakeLowStorageTest is TRUE  - fake exception");
                    // throw new Exception("FakeLowStorageTest");
                }
                if (FAILED(UpgradeThreadsTableToAutoIncrement(db))) {    // a no-op if already upgraded
                    Logger::E(TAG, "Failed to add autoIncrement to threads;: ");
                    autoIncrementThreadsSuccess = FALSE;
                    break;
                }
                db->SetTransactionSuccessful();
            } while (0);
            db->EndTransaction();
        }
        if (!hasAutoIncrementAddresses) {
            db->BeginTransaction();
            do {
                if (FALSE && sFakeLowStorageTest) {
                    Logger::D(TAG, "[getWritableDatabase] mFakeLowStorageTest is TRUE  - fake exception");
                    // throw new Exception("FakeLowStorageTest");
                }
                if (FAILED(UpgradeAddressTableToAutoIncrement(db))) {     // a no-op if already upgraded
                    Logger::E(TAG, "Failed to add autoIncrement to canonical_addresses: ");
                    autoIncrementAddressesSuccess = FALSE;
                    break;
                }
                db->SetTransactionSuccessful();
            } while (0);
            db->EndTransaction();
        }
        if (!hasAutoIncrementPart) {
            db->BeginTransaction();
            do {
                if (FALSE && sFakeLowStorageTest) {
                    Logger::D(TAG, "[getWritableDatabase] mFakeLowStorageTest is TRUE  - fake exception");
                    // throw new Exception("FakeLowStorageTest");
                }
                if (FAILED(UpgradePartTableToAutoIncrement(db))) {     // a no-op if already upgraded
                    Logger::E(TAG, "Failed to add autoIncrement to part: ");
                    autoIncrementPartSuccess = FALSE;
                    break;
                }
                db->SetTransactionSuccessful();
            } while (0);
            db->EndTransaction();
        }
        if (!hasAutoIncrementPdu) {
            db->BeginTransaction();
            do {
                if (FALSE && sFakeLowStorageTest) {
                    Logger::D(TAG, "[getWritableDatabase] mFakeLowStorageTest is TRUE  - fake exception");
                    // throw new Exception("FakeLowStorageTest");
                }
                if (FAILED(UpgradePduTableToAutoIncrement(db))) {     // a no-op if already upgraded
                    Logger::E(TAG, "Failed to add autoIncrement to pdu: ");
                    autoIncrementPduSuccess = FALSE;
                    break;
                }
                db->SetTransactionSuccessful();
            } while (0);
            db->EndTransaction();
        }
        if (autoIncrementThreadsSuccess &&
                autoIncrementAddressesSuccess &&
                autoIncrementPartSuccess &&
                autoIncrementPduSuccess) {
            if (mLowStorageMonitor != NULL) {
                // We've already updated the database. This receiver is no longer necessary.
                Logger::D(TAG, "Unregistering mLowStorageMonitor - we've upgraded");
                mContext->UnregisterReceiver(mLowStorageMonitor);
                mLowStorageMonitor = NULL;
            }
        }
        else {
            if (sFakeLowStorageTest) {
                sFakeLowStorageTest = FALSE;
            }

            // We failed, perhaps because of low storage. Turn on a receiver to watch for
            // storage space.
            if (mLowStorageMonitor == NULL) {
                Logger::D(TAG, "[getWritableDatabase] turning on storage monitor");
                CLowStorageMonitor::New((IBroadcastReceiver**)&mLowStorageMonitor);
                AutoPtr<IIntentFilter> intentFilter;
                CIntentFilter::New((IIntentFilter**)&intentFilter);
                intentFilter->AddAction(IIntent::ACTION_DEVICE_STORAGE_LOW);
                intentFilter->AddAction(IIntent::ACTION_DEVICE_STORAGE_OK);
                AutoPtr<IIntent> i;
                mContext->RegisterReceiver(mLowStorageMonitor, intentFilter, (IIntent**)&i);
            }
        }
    }
    *result = db;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

// Determine whether a particular table has AUTOINCREMENT in its schema.
Boolean MmsSmsDatabaseHelper::HasAutoIncrement(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& tableName)
{
    Boolean result = FALSE;
    String query = String("SELECT sql FROM sqlite_master WHERE type='table' AND name='") +
                    tableName + "'";
    AutoPtr<ICursor> c;
    db->RawQuery(query, NULL, (ICursor**)&c);
    if (c != NULL) {
        // try {
        Boolean move = FALSE;
        if (c->MoveToFirst(&move), move) {
            String schema;
            c->GetString(0, &schema);
            result = schema != NULL ? schema.Contains("AUTOINCREMENT") : FALSE;
            Logger::D(TAG, "[MmsSmsDb] tableName: %s HasAutoIncrement: %s result: %d"
                    , tableName.string(), schema.string(), result);
        }
        // } finally {
        ICloseable::Probe(c)->Close();
        // }
    }
    return result;
}

// upgradeThreadsTableToAutoIncrement() is called to add the AUTOINCREMENT keyword to
// the threads table. This could fail if the user has a lot of conversations and not enough
// storage to make a copy of the threads table. That's ok. This upgrade is optional. It'll
// be called again next time the device is rebooted.
ECode MmsSmsDatabaseHelper::UpgradeThreadsTableToAutoIncrement(
    /* [in] */ ISQLiteDatabase* db)
{
    if (HasAutoIncrement(db, IMmsSmsProvider::TABLE_THREADS)) {
        Logger::D(TAG, "[MmsSmsDb] upgradeThreadsTableToAutoIncrement: already upgraded");
        return NOERROR;
    }
    Logger::D(TAG, "[MmsSmsDb] upgradeThreadsTableToAutoIncrement: upgrading");

    // Make the _id of the threads table autoincrement so we never re-use thread ids
    // Have to create a new temp threads table. Copy all the info from the old table.
    // Drop the old table and rename the new table to that of the old.
    FAIL_RETURN(db->ExecSQL(String("CREATE TABLE threads_temp (") +
            IBaseColumns::ID + " INTEGER PRIMARY KEY AUTOINCREMENT," +
            ITelephonyThreadsColumns::DATE + " INTEGER DEFAULT 0," +
            ITelephonyThreadsColumns::MESSAGE_COUNT + " INTEGER DEFAULT 0," +
            ITelephonyThreadsColumns::RECIPIENT_IDS + " TEXT," +
            ITelephonyThreadsColumns::SNIPPET + " TEXT," +
            ITelephonyThreadsColumns::SNIPPET_CHARSET + " INTEGER DEFAULT 0," +
            ITelephonyThreadsColumns::READ + " INTEGER DEFAULT 1," +
            ITelephonyThreadsColumns::TYPE + " INTEGER DEFAULT 0," +
            ITelephonyThreadsColumns::ERROR + " INTEGER DEFAULT 0," +
            ITelephonyThreadsColumns::HAS_ATTACHMENT + " INTEGER DEFAULT 0);"));

    FAIL_RETURN(db->ExecSQL(String("INSERT INTO threads_temp SELECT * from threads;")));
    FAIL_RETURN(db->ExecSQL(String("DROP TABLE threads;")));
    return db->ExecSQL(String("ALTER TABLE threads_temp RENAME TO threads;"));
}

// upgradeAddressTableToAutoIncrement() is called to add the AUTOINCREMENT keyword to
// the canonical_addresses table. This could fail if the user has a lot of people they've
// messaged with and not enough storage to make a copy of the canonical_addresses table.
// That's ok. This upgrade is optional. It'll be called again next time the device is rebooted.
ECode MmsSmsDatabaseHelper::UpgradeAddressTableToAutoIncrement(
    /* [in] */ ISQLiteDatabase* db)
{
    if (HasAutoIncrement(db, String("canonical_addresses"))) {
        Logger::D(TAG, "[MmsSmsDb] upgradeAddressTableToAutoIncrement: already upgraded");
        return NOERROR;
    }
    Logger::D(TAG, "[MmsSmsDb] upgradeAddressTableToAutoIncrement: upgrading");

    // Make the _id of the canonical_addresses table autoincrement so we never re-use ids
    // Have to create a new temp canonical_addresses table. Copy all the info from the old
    // table. Drop the old table and rename the new table to that of the old.
    FAIL_RETURN(db->ExecSQL(String("CREATE TABLE canonical_addresses_temp (_id INTEGER PRIMARY KEY AUTOINCREMENT,") +
            "address TEXT);"));

    FAIL_RETURN(db->ExecSQL(String("INSERT INTO canonical_addresses_temp SELECT * from canonical_addresses;")));
    FAIL_RETURN(db->ExecSQL(String("DROP TABLE canonical_addresses;")));
    FAIL_RETURN(db->ExecSQL(String("ALTER TABLE canonical_addresses_temp RENAME TO canonical_addresses;")));
    return NOERROR;
}

// upgradePartTableToAutoIncrement() is called to add the AUTOINCREMENT keyword to
// the part table. This could fail if the user has a lot of sound/video/picture attachments
// and not enough storage to make a copy of the part table.
// That's ok. This upgrade is optional. It'll be called again next time the device is rebooted.
ECode MmsSmsDatabaseHelper::UpgradePartTableToAutoIncrement(
    /* [in] */ ISQLiteDatabase* db)
{
    if (HasAutoIncrement(db, String("part"))) {
        Logger::D(TAG, "[MmsSmsDb] upgradePartTableToAutoIncrement: already upgraded");
        return NOERROR;
    }
    Logger::D(TAG, "[MmsSmsDb] upgradePartTableToAutoIncrement: upgrading");

    // Make the _id of the part table autoincrement so we never re-use ids
    // Have to create a new temp part table. Copy all the info from the old
    // table. Drop the old table and rename the new table to that of the old.
    FAIL_RETURN(db->ExecSQL(String("CREATE TABLE part_temp (") +
            IBaseColumns::ID + " INTEGER PRIMARY KEY AUTOINCREMENT," +
            ITelephonyMmsPart::MSG_ID + " INTEGER," +
            ITelephonyMmsPart::SEQ + " INTEGER DEFAULT 0," +
            ITelephonyMmsPart::CONTENT_TYPE + " TEXT," +
            ITelephonyMmsPart::NAME + " TEXT," +
            ITelephonyMmsPart::CHARSET + " INTEGER," +
            ITelephonyMmsPart::CONTENT_DISPOSITION + " TEXT," +
            ITelephonyMmsPart::FILENAME + " TEXT," +
            ITelephonyMmsPart::CONTENT_ID + " TEXT," +
            ITelephonyMmsPart::CONTENT_LOCATION + " TEXT," +
            ITelephonyMmsPart::CT_START + " INTEGER," +
            ITelephonyMmsPart::CT_TYPE + " TEXT," +
            ITelephonyMmsPart::_DATA + " TEXT," +
            ITelephonyMmsPart::TEXT + " TEXT);"));

    FAIL_RETURN(db->ExecSQL(String("INSERT INTO part_temp SELECT * from part;")));
    FAIL_RETURN(db->ExecSQL(String("DROP TABLE part;")));
    FAIL_RETURN(db->ExecSQL(String("ALTER TABLE part_temp RENAME TO part;")));

    // part-related triggers get tossed when the part table is dropped -- rebuild them.
    return CreateMmsTriggers(db);
}

// upgradePduTableToAutoIncrement() is called to add the AUTOINCREMENT keyword to
// the pdu table. This could fail if the user has a lot of mms messages
// and not enough storage to make a copy of the pdu table.
// That's ok. This upgrade is optional. It'll be called again next time the device is rebooted.
ECode MmsSmsDatabaseHelper::UpgradePduTableToAutoIncrement(
    /* [in] */ ISQLiteDatabase* db)
{
    if (HasAutoIncrement(db, String("pdu"))) {
        Logger::D(TAG, "[MmsSmsDb] upgradePduTableToAutoIncrement: already upgraded");
        return NOERROR;
    }
    Logger::D(TAG, "[MmsSmsDb] upgradePduTableToAutoIncrement: upgrading");

    // Make the _id of the part table autoincrement so we never re-use ids
    // Have to create a new temp part table. Copy all the info from the old
    // table. Drop the old table and rename the new table to that of the old.
    FAIL_RETURN(db->ExecSQL(String("CREATE TABLE pdu_temp (") +
            IBaseColumns::ID + " INTEGER PRIMARY KEY AUTOINCREMENT," +
            ITelephonyBaseMmsColumns::THREAD_ID + " INTEGER," +
            ITelephonyBaseMmsColumns::DATE + " INTEGER," +
            ITelephonyBaseMmsColumns::DATE_SENT + " INTEGER DEFAULT 0," +
            ITelephonyBaseMmsColumns::MESSAGE_BOX + " INTEGER," +
            ITelephonyBaseMmsColumns::READ + " INTEGER DEFAULT 0," +
            ITelephonyBaseMmsColumns::MESSAGE_ID + " TEXT," +
            ITelephonyBaseMmsColumns::SUBJECT + " TEXT," +
            ITelephonyBaseMmsColumns::SUBJECT_CHARSET + " INTEGER," +
            ITelephonyBaseMmsColumns::CONTENT_TYPE + " TEXT," +
            ITelephonyBaseMmsColumns::CONTENT_LOCATION + " TEXT," +
            ITelephonyBaseMmsColumns::EXPIRY + " INTEGER," +
            ITelephonyBaseMmsColumns::MESSAGE_CLASS + " TEXT," +
            ITelephonyBaseMmsColumns::MESSAGE_TYPE + " INTEGER," +
            ITelephonyBaseMmsColumns::MMS_VERSION + " INTEGER," +
            ITelephonyBaseMmsColumns::MESSAGE_SIZE + " INTEGER," +
            ITelephonyBaseMmsColumns::PRIORITY + " INTEGER," +
            ITelephonyBaseMmsColumns::READ_REPORT + " INTEGER," +
            ITelephonyBaseMmsColumns::REPORT_ALLOWED + " INTEGER," +
            ITelephonyBaseMmsColumns::RESPONSE_STATUS + " INTEGER," +
            ITelephonyBaseMmsColumns::STATUS + " INTEGER," +
            ITelephonyBaseMmsColumns::TRANSACTION_ID + " TEXT," +
            ITelephonyBaseMmsColumns::RETRIEVE_STATUS + " INTEGER," +
            ITelephonyBaseMmsColumns::RETRIEVE_TEXT + " TEXT," +
            ITelephonyBaseMmsColumns::RETRIEVE_TEXT_CHARSET + " INTEGER," +
            ITelephonyBaseMmsColumns::READ_STATUS + " INTEGER," +
            ITelephonyBaseMmsColumns::CONTENT_CLASS + " INTEGER," +
            ITelephonyBaseMmsColumns::RESPONSE_TEXT + " TEXT," +
            ITelephonyBaseMmsColumns::DELIVERY_TIME + " INTEGER," +
            ITelephonyBaseMmsColumns::DELIVERY_REPORT + " INTEGER," +
            ITelephonyBaseMmsColumns::LOCKED + " INTEGER DEFAULT 0," +
            ITelephonyBaseMmsColumns::PHONE_ID + " INTEGER DEFAULT -1," +
            ITelephonyBaseMmsColumns::SEEN + " INTEGER DEFAULT 0," +
            ITelephonyBaseMmsColumns::TEXT_ONLY + " INTEGER DEFAULT 0" +
            ");"));

    FAIL_RETURN(db->ExecSQL(String("INSERT INTO pdu_temp SELECT * from pdu;")));
    FAIL_RETURN(db->ExecSQL(String("DROP TABLE pdu;")));
    FAIL_RETURN(db->ExecSQL(String("ALTER TABLE pdu_temp RENAME TO pdu;")));

    // pdu-related triggers get tossed when the part table is dropped -- rebuild them.
    return CreateMmsTriggers(db);
}

void MmsSmsDatabaseHelper::UpdateThreadsAttachmentColumn(
    /* [in] */ ISQLiteDatabase* db)
{
    // Set the values of that column correctly based on the current
    // contents of the database.
    db->ExecSQL(String("UPDATE threads SET has_attachment=1 WHERE _id IN ") +
               "  (SELECT DISTINCT pdu.thread_id FROM part " +
               "   JOIN pdu ON pdu._id=part.mid " +
               "   WHERE part.ct != 'text/plain' AND part.ct != 'application/smil')");
}

// Determine whether this database has CM11 columns...
Boolean MmsSmsDatabaseHelper::IsCM11DB(
    /* [in] */ ISQLiteDatabase* db)
{
    Boolean result = FALSE;
    // try {
    String query("SELECT sub_id, pri FROM sms");
    AutoPtr<ICursor> c;
    db->RawQuery(query, NULL, (ICursor**)&c);
    if (c != NULL) {
        result = TRUE;
    }
    // } catch (Exception e) {}
    return result;
}

} // namespace Telephony
} // namespace Providers
} // namespace Droid
} // namespace Elastos
