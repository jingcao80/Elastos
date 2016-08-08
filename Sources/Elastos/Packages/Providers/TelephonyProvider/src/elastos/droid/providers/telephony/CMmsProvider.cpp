
#include "elastos/droid/providers/telephony/CMmsProvider.h"
#include "elastos/droid/providers/telephony/MmsSmsDatabaseHelper.h"
#include "elastos/droid/provider/Telephony.h"
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/text/TextUtils.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Google.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CUriMatcher;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IUriMatcher;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Database::Sqlite::CSQLiteQueryBuilder;
using Elastos::Droid::Google::Mms::Pdu::IPduHeaders;
using Elastos::Droid::Google::Mms::Utility::CDownloadDrmHelper;
using Elastos::Droid::Google::Mms::Utility::IDownloadDrmHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::ITelephonyBaseMmsColumns;
using Elastos::Droid::Provider::ITelephonyCanonicalAddressesColumns;
using Elastos::Droid::Provider::ITelephonyMmsAddr;
using Elastos::Droid::Provider::ITelephonyMmsIntents;
using Elastos::Droid::Provider::ITelephonyMmsPart;
using Elastos::Droid::Provider::ITelephonyMmsRate;
using Elastos::Droid::Provider::ITelephonyMmsSms;
using Elastos::Droid::Provider::ITelephonyMmsSmsWordsTable;
using Elastos::Droid::Provider::ITelephonyTextBasedSmsColumns;
using Elastos::Droid::Provider::ITelephonyThreadsColumns;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CString;
using Elastos::Core::CSystem;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::ISystem;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::IO::ICloseable;
using Elastos::IO::IFile;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Telephony {

static AutoPtr<IUriMatcher> InitUriMatcher()
{
    AutoPtr<IUriMatcher> um;
    CUriMatcher::New(IUriMatcher::NO_MATCH, (IUriMatcher**)&um);
    return um;
}

const String CMmsProvider::TAG("MmsProvider");
const String CMmsProvider::VND_ANDROID_MMS("vnd.android/mms");
const String CMmsProvider::VND_ANDROID_DIR_MMS("vnd.android-dir/mms");
const Boolean CMmsProvider::DEBUG = FALSE;
const Boolean CMmsProvider::LOCAL_LOGV = FALSE;
const Int32 CMmsProvider::MMS_ALL                      = 0;
const Int32 CMmsProvider::MMS_ALL_ID                   = 1;
const Int32 CMmsProvider::MMS_INBOX                    = 2;
const Int32 CMmsProvider::MMS_INBOX_ID                 = 3;
const Int32 CMmsProvider::MMS_SENT                     = 4;
const Int32 CMmsProvider::MMS_SENT_ID                  = 5;
const Int32 CMmsProvider::MMS_DRAFTS                   = 6;
const Int32 CMmsProvider::MMS_DRAFTS_ID                = 7;
const Int32 CMmsProvider::MMS_OUTBOX                   = 8;
const Int32 CMmsProvider::MMS_OUTBOX_ID                = 9;
const Int32 CMmsProvider::MMS_ALL_PART                 = 10;
const Int32 CMmsProvider::MMS_MSG_PART                 = 11;
const Int32 CMmsProvider::MMS_PART_ID                  = 12;
const Int32 CMmsProvider::MMS_MSG_ADDR                 = 13;
const Int32 CMmsProvider::MMS_SENDING_RATE             = 14;
const Int32 CMmsProvider::MMS_REPORT_STATUS            = 15;
const Int32 CMmsProvider::MMS_REPORT_REQUEST           = 16;
const Int32 CMmsProvider::MMS_DRM_STORAGE              = 17;
const Int32 CMmsProvider::MMS_DRM_STORAGE_ID           = 18;
const Int32 CMmsProvider::MMS_THREADS                  = 19;
const Int32 CMmsProvider::MMS_PART_RESET_FILE_PERMISSION = 20;
const Int32 CMmsProvider::MMS_THREAD_ID = 21;
AutoPtr<IUriMatcher> CMmsProvider::sURLMatcher = InitUriMatcher();
Boolean CMmsProvider::sInit = Init();

CAR_OBJECT_IMPL(CMmsProvider)
CAR_INTERFACE_IMPL(CMmsProvider, ContentProvider, IMmsProvider)

ECode CMmsProvider::OnCreate(
    /* [out] */ Boolean* result)
{
    SetAppOps(IAppOpsManager::OP_READ_MMS, IAppOpsManager::OP_WRITE_MMS);
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    mOpenHelper = MmsSmsDatabaseHelper::GetInstance(ctx);
    *result = TRUE;
    return NOERROR;
}

// @Override
ECode CMmsProvider::Query(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    AutoPtr<ISQLiteQueryBuilder> qb;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&qb);

    // Generate the body of the query.
    Int32 match = 0;
    sURLMatcher->Match(uri, &match);
    if (LOCAL_LOGV) {
        Logger::V(TAG, "Query uri=%s, match=%d", TO_CSTR(uri), match);
    }

    switch (match) {
        case MMS_ALL:
            ConstructQueryForBox(qb, ITelephonyBaseMmsColumns::MESSAGE_BOX_ALL);
            break;
        case MMS_INBOX:
            ConstructQueryForBox(qb, ITelephonyBaseMmsColumns::MESSAGE_BOX_INBOX);
            break;
        case MMS_SENT:
            ConstructQueryForBox(qb, ITelephonyBaseMmsColumns::MESSAGE_BOX_SENT);
            break;
        case MMS_DRAFTS:
            ConstructQueryForBox(qb, ITelephonyBaseMmsColumns::MESSAGE_BOX_DRAFTS);
            break;
        case MMS_OUTBOX:
            ConstructQueryForBox(qb, ITelephonyBaseMmsColumns::MESSAGE_BOX_OUTBOX);
            break;
        case MMS_ALL_ID: {
            qb->SetTables(TABLE_PDU);
            AutoPtr<IList> l;
            uri->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(0, (IInterface**)&obj);
            AutoPtr<ICharSequence> cs;
            CString::New(IBaseColumns::ID + "=" + TO_STR(obj), (ICharSequence**)&cs);
            qb->AppendWhere(cs);
            break;
        }
        case MMS_INBOX_ID:
        case MMS_SENT_ID:
        case MMS_DRAFTS_ID:
        case MMS_OUTBOX_ID: {
            qb->SetTables(TABLE_PDU);
            AutoPtr<IList> l;
            uri->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(1, (IInterface**)&obj);
            AutoPtr<ICharSequence> cs;
            CString::New(IBaseColumns::ID + "=" + TO_STR(obj), (ICharSequence**)&cs);
            qb->AppendWhere(cs);
            Int32 v = 0;
            FAIL_RETURN(GetMessageBoxByMatch(match, &v));
            cs = NULL;
            CString::New(String(" AND ") + ITelephonyBaseMmsColumns::MESSAGE_BOX + "=" + StringUtils::ToString(v)
                    , (ICharSequence**)&cs);
            qb->AppendWhere(cs);
            break;
        }
        case MMS_ALL_PART:
            qb->SetTables(TABLE_PART);
            break;
        case MMS_MSG_PART: {
            qb->SetTables(TABLE_PART);
            AutoPtr<IList> l;
            uri->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(0, (IInterface**)&obj);
            AutoPtr<ICharSequence> cs;
            CString::New(ITelephonyMmsPart::MSG_ID + "=" + TO_STR(obj), (ICharSequence**)&cs);
            qb->AppendWhere(cs);
            break;
        }
        case MMS_PART_ID: {
            qb->SetTables(TABLE_PART);
            AutoPtr<IList> l;
            uri->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(1, (IInterface**)&obj);
            AutoPtr<ICharSequence> cs;
            CString::New(IBaseColumns::ID + "=" + TO_STR(obj), (ICharSequence**)&cs);
            qb->AppendWhere(cs);
            break;
        }
        case MMS_MSG_ADDR: {
            qb->SetTables(TABLE_ADDR);
            AutoPtr<IList> l;
            uri->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(0, (IInterface**)&obj);
            AutoPtr<ICharSequence> cs;
            CString::New(ITelephonyMmsAddr::MSG_ID + "=" + TO_STR(obj), (ICharSequence**)&cs);
            qb->AppendWhere(cs);
            break;
        }
        case MMS_REPORT_STATUS: {
            /*
               SELECT DISTINCT address,
                               T.delivery_status AS delivery_status,
                               T.read_status AS read_status
               FROM addr
               INNER JOIN (SELECT P1._id AS id1, P2._id AS id2, P3._id AS id3,
                                  ifnull(P2.st, 0) AS delivery_status,
                                  ifnull(P3.read_status, 0) AS read_status
                           FROM pdu P1
                           INNER JOIN pdu P2
                           ON P1.m_id = P2.m_id AND P2.m_type = 134
                           LEFT JOIN pdu P3
                           ON P1.m_id = P3.m_id AND P3.m_type = 136
                           UNION
                           SELECT P1._id AS id1, P2._id AS id2, P3._id AS id3,
                                  ifnull(P2.st, 0) AS delivery_status,
                                  ifnull(P3.read_status, 0) AS read_status
                           FROM pdu P1
                           INNER JOIN pdu P3
                           ON P1.m_id = P3.m_id AND P3.m_type = 136
                           LEFT JOIN pdu P2
                           ON P1.m_id = P2.m_id AND P2.m_type = 134) T
               ON (msg_id = id2 AND type = 151)
               OR (msg_id = id3 AND type = 137)
               WHERE T.id1 = ?;
             */
            qb->SetTables(String("addr INNER JOIN (SELECT P1._id AS id1, P2._id") +
                         " AS id2, P3._id AS id3, ifnull(P2.st, 0) AS" +
                         " delivery_status, ifnull(P3.read_status, 0) AS" +
                         " read_status FROM pdu P1 INNER JOIN pdu P2 ON" +
                         " P1.m_id=P2.m_id AND P2.m_type=134 LEFT JOIN" +
                         " pdu P3 ON P1.m_id=P3.m_id AND P3.m_type=136" +
                         " UNION SELECT P1._id AS id1, P2._id AS id2, P3._id" +
                         " AS id3, ifnull(P2.st, 0) AS delivery_status," +
                         " ifnull(P3.read_status, 0) AS read_status FROM" +
                         " pdu P1 INNER JOIN pdu P3 ON P1.m_id=P3.m_id AND" +
                         " P3.m_type=136 LEFT JOIN pdu P2 ON P1.m_id=P2.m_id" +
                         " AND P2.m_type=134) T ON (msg_id=id2 AND type=151)" +
                         " OR (msg_id=id3 AND type=137)");
            String v;
            uri->GetLastPathSegment(&v);
            AutoPtr<ICharSequence> cs;
            CString::New(String("T.id1 = ") + v, (ICharSequence**)&cs);
            qb->AppendWhere(cs);
            qb->SetDistinct(TRUE);
            break;
        }
        case MMS_REPORT_REQUEST: {
            /*
               SELECT address, d_rpt, rr
               FROM addr join pdu on pdu._id = addr.msg_id
               WHERE pdu._id = messageId AND addr.type = 151
             */
            qb->SetTables(TABLE_ADDR + " join " + TABLE_PDU + " on pdu._id = addr.msg_id");
            String v;
            uri->GetLastPathSegment(&v);
            AutoPtr<ICharSequence> cs;
            CString::New(String("pdu._id = ") + v, (ICharSequence**)&cs);
            qb->AppendWhere(cs);

            cs = NULL;
            CString::New(String(" AND ") + "addr.type = " + StringUtils::ToString(IPduHeaders::TO),
                    (ICharSequence**)&cs);
            qb->AppendWhere(cs);
            break;
        }
        case MMS_SENDING_RATE:
            qb->SetTables(TABLE_RATE);
            break;
        case MMS_DRM_STORAGE_ID: {
            qb->SetTables(TABLE_DRM);
            String v;
            uri->GetLastPathSegment(&v);
            AutoPtr<ICharSequence> cs;
            CString::New(IBaseColumns::ID + "=" + v, (ICharSequence**)&cs);
            qb->AppendWhere(cs);
            break;
        }
        case MMS_THREADS:
            qb->SetTables(String("pdu group by thread_id"));
            break;
        default: {
            Logger::E(TAG, "query: invalid request: %s", TO_CSTR(uri));
            return NOERROR;
        }
    }

    String finalSortOrder;
    if (TextUtils::IsEmpty(sortOrder)) {
        String t;
        qb->GetTables(&t);
        if (t.Equals(TABLE_PDU)) {
            finalSortOrder = ITelephonyBaseMmsColumns::DATE + " DESC";
        }
        else if (t.Equals(TABLE_PART)) {
            finalSortOrder = ITelephonyMmsPart::SEQ;
        }
    }
    else {
        finalSortOrder = sortOrder;
    }

    AutoPtr<ICursor> ret;
    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
    if (FAILED(qb->Query(db, projection, selection, selectionArgs
            , String(NULL), String(NULL), finalSortOrder, (ICursor**)&ret))) {
        Logger::E(TAG, "returning NULL cursor, query: %s", TO_CSTR(uri));
        return NOERROR;
    }

    // TODO: Does this need to be a URI for this provider.
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IContentResolver> cr;
    ctx->GetContentResolver((IContentResolver**)&cr);
    ret->SetNotificationUri(cr, uri);
    *result = ret;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

void CMmsProvider::ConstructQueryForBox(
    /* [in] */ ISQLiteQueryBuilder* qb,
    /* [in] */ Int32 msgBox)
{
    qb->SetTables(TABLE_PDU);

    if (msgBox != ITelephonyBaseMmsColumns::MESSAGE_BOX_ALL) {
        AutoPtr<ICharSequence> cs;
        CString::New(ITelephonyBaseMmsColumns::MESSAGE_BOX + "=" + StringUtils::ToString(msgBox),
                (ICharSequence**)&cs);
        qb->AppendWhere(cs);
    }
}

// @Override
ECode CMmsProvider::GetType(
    /* [in] */ IUri* uri,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 match = 0;
    sURLMatcher->Match(uri, &match);
    switch (match) {
        case MMS_ALL:
        case MMS_INBOX:
        case MMS_SENT:
        case MMS_DRAFTS:
        case MMS_OUTBOX: {
            *result = VND_ANDROID_DIR_MMS;
            return NOERROR;
        }
        case MMS_ALL_ID:
        case MMS_INBOX_ID:
        case MMS_SENT_ID:
        case MMS_DRAFTS_ID:
        case MMS_OUTBOX_ID: {
            *result = VND_ANDROID_MMS;
            return NOERROR;
        }
        case MMS_PART_ID: {
            AutoPtr<ISQLiteDatabase> db;
            mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
            AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(1);
            (*ss)[0] = ITelephonyMmsPart::CONTENT_TYPE;
            AutoPtr<ArrayOf<String> > s2 = ArrayOf<String>::Alloc(1);
            String v;
            uri->GetLastPathSegment(&v);
            (*s2)[0] = v;
            AutoPtr<ICursor> cursor;
            db->Query(
                    TABLE_PART, ss,
                    IBaseColumns::ID + " = ?", s2,
                    String(NULL), String(NULL), String(NULL), (ICursor**)&cursor);
            if (cursor != NULL) {
                Int32 count = 0;
                cursor->GetCount(&count);
                Boolean move = FALSE;
                if ((count == 1) && (cursor->MoveToFirst(&move), move)) {
                    ICloseable::Probe(cursor)->Close();
                    return cursor->GetString(0, result);
                }
                else {
                    Logger::E(TAG, "cursor.count() != 1: %s", TO_CSTR(uri));
                }
                ICloseable::Probe(cursor)->Close();
            }
            else {
                Logger::E(TAG, "cursor == NULL: %s", TO_CSTR(uri));
            }
            *result = String("*/*");
            return NOERROR;
        }
        case MMS_ALL_PART:
        case MMS_MSG_PART:
        case MMS_MSG_ADDR:
        default: {
            *result = String("*/*");
            return NOERROR;
        }
    }
    return NOERROR;
}

// @Override
ECode CMmsProvider::Insert(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [out] */ IUri** _result)
{
    VALIDATE_NOT_NULL(_result);
    *_result = NULL;
    // Don't let anyone insert anything with the _data column
    Boolean tmp = FALSE;
    if (values != NULL && (values->ContainsKey(ITelephonyMmsPart::_DATA, &tmp), tmp)) {
        return NOERROR;
    }
    Int32 msgBox = ITelephonyBaseMmsColumns::MESSAGE_BOX_ALL;
    Boolean notify = TRUE;

    Int32 match = 0;
    sURLMatcher->Match(uri, &match);
    if (LOCAL_LOGV) {
        Logger::V(TAG, "Insert uri=%s, match=%d", TO_CSTR(uri), match);
    }

    String table = TABLE_PDU;
    switch (match) {
        case MMS_ALL: {
            AutoPtr<IInteger32> msgBoxObj;
            values->GetAsInteger32(ITelephonyBaseMmsColumns::MESSAGE_BOX, (IInteger32**)&msgBoxObj);
            if (msgBoxObj != NULL) {
                msgBoxObj->GetValue(&msgBox);
            }
            else {
                // default to inbox
                msgBox = ITelephonyBaseMmsColumns::MESSAGE_BOX_INBOX;
            }
            break;
        }
        case MMS_INBOX:
            msgBox = ITelephonyBaseMmsColumns::MESSAGE_BOX_INBOX;
            break;
        case MMS_SENT:
            msgBox = ITelephonyBaseMmsColumns::MESSAGE_BOX_SENT;
            break;
        case MMS_DRAFTS:
            msgBox = ITelephonyBaseMmsColumns::MESSAGE_BOX_DRAFTS;
            break;
        case MMS_OUTBOX:
            msgBox = ITelephonyBaseMmsColumns::MESSAGE_BOX_OUTBOX;
            break;
        case MMS_MSG_PART:
            notify = FALSE;
            table = TABLE_PART;
            break;
        case MMS_MSG_ADDR:
            notify = FALSE;
            table = TABLE_ADDR;
            break;
        case MMS_SENDING_RATE:
            notify = FALSE;
            table = TABLE_RATE;
            break;
        case MMS_DRM_STORAGE:
            notify = FALSE;
            table = TABLE_DRM;
            break;
        default: {
            Logger::E(TAG, "insert: invalid request: %s", TO_CSTR(uri));
            return NOERROR;
        }
    }

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<IContentValues> finalValues;
    AutoPtr<IUri> res = Elastos::Droid::Provider::Telephony::Mms::CONTENT_URI;
    Int64 rowId = 0;

    if (table.Equals(TABLE_PDU)) {
        Boolean tmp = FALSE;
        Boolean addDate = !(values->ContainsKey(ITelephonyBaseMmsColumns::DATE, &tmp), tmp);
        Boolean addMsgBox = !(values->ContainsKey(ITelephonyBaseMmsColumns::MESSAGE_BOX, &tmp), tmp);

        // Filter keys we don't support yet.
        FilterUnsupportedKeys(values);

        // TODO: Should initialValues be validated, e.g. if it
        // missed some significant keys?
        CContentValues::New(values, (IContentValues**)&finalValues);

        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        Int64 timeInMillis = 0;
        system->GetCurrentTimeMillis(&timeInMillis);

        if (addDate) {
            finalValues->Put(ITelephonyBaseMmsColumns::DATE, timeInMillis / 1000L);
        }

        if (addMsgBox && (msgBox != ITelephonyBaseMmsColumns::MESSAGE_BOX_ALL)) {
            finalValues->Put(ITelephonyBaseMmsColumns::MESSAGE_BOX, msgBox);
        }

        if (msgBox != ITelephonyBaseMmsColumns::MESSAGE_BOX_INBOX) {
            // Mark all non-inbox messages read.
            finalValues->Put(ITelephonyBaseMmsColumns::READ, 1);
        }

        // thread_id
        AutoPtr<IInteger64> threadId;
        values->GetAsInteger64(ITelephonyBaseMmsColumns::THREAD_ID, (IInteger64**)&threadId);
        String address;
        values->GetAsString(ITelephonyCanonicalAddressesColumns::ADDRESS, &address);

        if (((threadId == NULL) || (threadId == 0)) && (!TextUtils::IsEmpty(address))) {
            AutoPtr<IContext> ctx;
            GetContext((IContext**)&ctx);
            Int64 id = 0;
            Elastos::Droid::Provider::Telephony::Threads::GetOrCreateThreadId(ctx, address, &id);
            finalValues->Put(ITelephonyBaseMmsColumns::THREAD_ID, id);
        }

        db->Insert(table, String(NULL), finalValues, &rowId);
        if (rowId <= 0) {
            Logger::E(TAG, "MmsProvider.insert: failed! %s", TO_CSTR(finalValues));
            return NOERROR;
        }

        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uu;
        helper->Parse(TO_STR(res) + "/" + StringUtils::ToString(rowId), (IUri**)&uu);
        res = uu;
    }
    else if (table.Equals(TABLE_ADDR)) {
        CContentValues::New(values, (IContentValues**)&finalValues);
        AutoPtr<IList> l;
        uri->GetPathSegments((IList**)&l);
        AutoPtr<IInterface> obj;
        l->Get(0, (IInterface**)&obj);
        finalValues->Put(ITelephonyMmsAddr::MSG_ID, TO_STR(obj));

        db->Insert(table, String(NULL), finalValues, &rowId);
        if (rowId <= 0) {
            Logger::E(TAG, "Failed to insert address: %s", TO_CSTR(finalValues));
            return NOERROR;
        }

        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uu;
        helper->Parse(TO_STR(res) + "/addr/" + StringUtils::ToString(rowId), (IUri**)&uu);
        res = uu;
    }
    else if (table.Equals(TABLE_PART)) {
        CContentValues::New(values, (IContentValues**)&finalValues);

        if (match == MMS_MSG_PART) {
            AutoPtr<IList> l;
            uri->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(0, (IInterface**)&obj);
            finalValues->Put(ITelephonyMmsPart::MSG_ID, TO_STR(obj));
        }

        String contentType;
        values->GetAsString(String("ct"), &contentType);

        // text/plain and app application/smil store their "data" inline in the
        // table so there's no need to create the file
        Boolean plainText = FALSE;
        Boolean smilText = FALSE;
        if (String("text/plain").Equals(contentType)) {
            plainText = TRUE;
        }
        else if (String("application/smil").Equals(contentType)) {
            smilText = TRUE;
        }
        if (!plainText && !smilText) {
            // Use the filename if possible, otherwise use the current time as the name.
            String contentLocation;
            values->GetAsString(String("cl"), &contentLocation);
            if (!TextUtils::IsEmpty(contentLocation)) {
                AutoPtr<IFile> f;
                CFile::New(contentLocation, (IFile**)&f);
                String v;
                f->GetName(&v);
                contentLocation = String("_") + v;
            }
            else {
                contentLocation = String("");
            }

            // Generate the '_data' field of the part with default
            // permission settings.
            AutoPtr<IContext> ctx;
            GetContext((IContext**)&ctx);
            AutoPtr<IFile> dir;
            ctx->GetDir(String("parts"), 0, (IFile**)&dir);
            String p;
            dir->GetPath(&p);
            AutoPtr<ISystem> system;
            CSystem::AcquireSingleton((ISystem**)&system);
            Int64 timeInMillis = 0;
            system->GetCurrentTimeMillis(&timeInMillis);
            String path = p + "/PART_" + StringUtils::ToString(timeInMillis) + contentLocation;

            AutoPtr<IDownloadDrmHelper> ddHelper;
            CDownloadDrmHelper::AcquireSingleton((IDownloadDrmHelper**)&ddHelper);
            Boolean tmp = FALSE;
            if (ddHelper->IsDrmConvertNeeded(contentType, &tmp), tmp) {
                // Adds the .fl extension to the filename if contentType is
                // "application/vnd.oma.drm.message"
                ddHelper->ModifyDrmFwLockFileExtension(path, &path);
            }

            finalValues->Put(ITelephonyMmsPart::_DATA, path);

            AutoPtr<IFile> partFile;
            CFile::New(path, (IFile**)&partFile);
            if (partFile->Exists(&tmp), !tmp) {
                ECode ec = NOERROR;
                if (ec = partFile->CreateNewFile(&tmp), !tmp) {
                    // throw new IllegalStateException("Unable to create new partFile: " + path);
                    Logger::E(TAG, "Unable to create new partFile: %s", path.string());
                    return E_ILLEGAL_STATE_EXCEPTION;
                }
                if (FAILED(ec)) {
                    Logger::E(TAG, "createNewFile error.");
                    return E_ILLEGAL_STATE_EXCEPTION;
                }

                // Give everyone rw permission until we encrypt the file
                // (in PduPersister.persistData). Once the file is encrypted, the
                // permissions will be set to 0644.
                Int32 result = FileUtils::SetPermissions(path, 0666, -1, -1);
                if (LOCAL_LOGV) {
                    Logger::D(TAG, "MmsProvider.insert setPermissions result: %d", result);
                }
            }
        }

        db->Insert(table, String(NULL), finalValues, &rowId);
        if (rowId <= 0) {
            Logger::E(TAG, "MmsProvider.insert: failed! %s", TO_CSTR(finalValues));
            return NOERROR;
        }

        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uu;
        helper->Parse(TO_STR(res) + "/part/" + StringUtils::ToString(rowId), (IUri**)&uu);
        res = uu;

        // Don't use a trigger for updating the words table because of a bug
        // in FTS3.  The bug is such that the call to get the last inserted
        // row is incorrect.
        if (plainText) {
            // Update the words table with a corresponding row.  The words table
            // allows us to search for words quickly, without scanning the whole
            // table;
            AutoPtr<IContentValues> cv;
            CContentValues::New((IContentValues**)&cv);

            // we're using the row id of the part table row but we're also using ids
            // from the sms table so this divides the space into two large chunks.
            // The row ids from the part table start at 2 << 32.
            cv->Put(ITelephonyMmsSmsWordsTable::ID, (((Int64)2) << 32) + rowId);
            String sv;
            values->GetAsString(String("text"), &sv);
            cv->Put(ITelephonyMmsSmsWordsTable::INDEXED_TEXT, sv);
            cv->Put(ITelephonyMmsSmsWordsTable::SOURCE_ROW_ID, rowId);
            cv->Put(ITelephonyMmsSmsWordsTable::TABLE_ID, 2);
            Int64 lv = 0;
            db->Insert(TABLE_WORDS, ITelephonyMmsSmsWordsTable::INDEXED_TEXT, cv, &lv);
        }

    }
    else if (table.Equals(TABLE_RATE)) {
        AutoPtr<IInteger64> lo;
        values->GetAsInteger64(ITelephonyMmsRate::SENT_TIME, (IInteger64**)&lo);
        Int64 now = 0;
        lo->GetValue(&now);
        Int64 oneHourAgo = now - 1000 * 60 * 60;
        // Delete all unused rows (time earlier than one hour ago).
        Int32 dv = 0;
        db->Delete(table, ITelephonyMmsRate::SENT_TIME + "<=" + StringUtils::ToString(oneHourAgo), NULL, &dv);
        Int64 lv = 0;
        db->Insert(table, String(NULL), values, &lv);
    }
    else if (table.Equals(TABLE_DRM)) {
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        AutoPtr<IFile> dir;
        ctx->GetDir(String("parts"), 0, (IFile**)&dir);
        String p;
        dir->GetPath(&p);
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        Int64 timeInMillis = 0;
        system->GetCurrentTimeMillis(&timeInMillis);
        String path = p + "/PART_" + StringUtils::ToString(timeInMillis);
        CContentValues::New(1, (IContentValues**)&finalValues);
        finalValues->Put(String("_data"), path);

        AutoPtr<IFile> partFile;
        CFile::New(path, (IFile**)&partFile);
        Boolean tmp = FALSE;
        if (partFile->Exists(&tmp), !tmp) {
            ECode ec = NOERROR;
            if (ec = partFile->CreateNewFile(&tmp), !tmp) {
                // throw new IllegalStateException("Unable to create new file: " + path);
                Logger::E(TAG, "Unable to create new file: %s", path.string());
                return E_ILLEGAL_STATE_EXCEPTION;
            }
            if (FAILED(ec)) {
                Logger::E(TAG, "createNewFile error.");
                return E_ILLEGAL_STATE_EXCEPTION;
            }
        }

        db->Insert(table, String(NULL), finalValues, &rowId);
        if (rowId <= 0) {
            Logger::E(TAG, "MmsProvider.insert: failed! %s", TO_CSTR(finalValues));
            return NOERROR;
        }

        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uu;
        helper->Parse(TO_STR(res) + "/drm/" + StringUtils::ToString(rowId), (IUri**)&uu);
        res = uu;
    }
    else {
        // throw new AssertionError("Unknown table type: " + table);
        Logger::D(TAG, "Unknown table type: %s", table.string());
        return E_ASSERTION_ERROR;
    }

    if (notify) {
        NotifyChange();
    }
    *_result = res;
    REFCOUNT_ADD(*_result);
    return NOERROR;
}

ECode CMmsProvider::GetMessageBoxByMatch(
    /* [in] */ Int32 match,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    switch (match) {
        case MMS_INBOX_ID:
        case MMS_INBOX: {
            *result = ITelephonyBaseMmsColumns::MESSAGE_BOX_INBOX;
            return NOERROR;
        }
        case MMS_SENT_ID:
        case MMS_SENT: {
            *result = ITelephonyBaseMmsColumns::MESSAGE_BOX_SENT;
            return NOERROR;
        }
        case MMS_DRAFTS_ID:
        case MMS_DRAFTS: {
            *result = ITelephonyBaseMmsColumns::MESSAGE_BOX_DRAFTS;
            return NOERROR;
        }
        case MMS_OUTBOX_ID:
        case MMS_OUTBOX: {
            *result = ITelephonyBaseMmsColumns::MESSAGE_BOX_OUTBOX;
            return NOERROR;
        }
        default: {
            // throw new IllegalArgumentException("bad Arg: " + match);
            Logger::D(TAG, "bad Arg: %d", match);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    return NOERROR;
}

// @Override
ECode CMmsProvider::Delete(
    /* [in] */ IUri* uri,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    Int32 match = 0;
    sURLMatcher->Match(uri, &match);
    if (LOCAL_LOGV) {
        Logger::V(TAG, "Delete uri=%s, match%d", TO_CSTR(uri), match);
    }

    String table, extraSelection;
    Boolean notify = FALSE;

    switch (match) {
        case MMS_ALL_ID:
        case MMS_INBOX_ID:
        case MMS_SENT_ID:
        case MMS_DRAFTS_ID:
        case MMS_OUTBOX_ID: {
            notify = TRUE;
            table = TABLE_PDU;
            String v;
            uri->GetLastPathSegment(&v);
            extraSelection = IBaseColumns::ID + "=" + v;
            break;
        }
        case MMS_ALL:
        case MMS_INBOX:
        case MMS_SENT:
        case MMS_DRAFTS:
        case MMS_OUTBOX: {
            notify = TRUE;
            table = TABLE_PDU;
            if (match != MMS_ALL) {
                Int32 msgBox = 0;
                FAIL_RETURN(GetMessageBoxByMatch(match, &msgBox));
                extraSelection = ITelephonyBaseMmsColumns::MESSAGE_BOX + "=" + StringUtils::ToString(msgBox);
            }
            break;
        }
        case MMS_ALL_PART:
            table = TABLE_PART;
            break;
        case MMS_MSG_PART: {
            table = TABLE_PART;
            AutoPtr<IList> l;
            uri->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(0, (IInterface**)&obj);
            extraSelection = ITelephonyMmsPart::MSG_ID + "=" + TO_STR(obj);
            break;
        }
        case MMS_PART_ID: {
            table = TABLE_PART;
            AutoPtr<IList> l;
            uri->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(1, (IInterface**)&obj);
            extraSelection = IBaseColumns::ID + "=" + TO_STR(obj);
            break;
        }
        case MMS_MSG_ADDR: {
            table = TABLE_ADDR;
            AutoPtr<IList> l;
            uri->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(0, (IInterface**)&obj);
            extraSelection = ITelephonyMmsAddr::MSG_ID + "=" + TO_STR(obj);
            break;
        }
        case MMS_DRM_STORAGE:
            table = TABLE_DRM;
            break;
        case MMS_THREAD_ID: {
            notify = TRUE;
            table = TABLE_PDU;
            String v;
            uri->GetLastPathSegment(&v);
            extraSelection = ITelephonyBaseMmsColumns::THREAD_ID + "=" + v;
            break;
        }
        default: {
            Logger::W(TAG, "No match for URI '%s'", TO_CSTR(uri));
            *result = 0;
            return NOERROR;
        }
    }

    String finalSelection = ConcatSelections(selection, extraSelection);
    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    Int32 deletedRows = 0;

    if (TABLE_PDU.Equals(table)) {
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        deletedRows = DeleteMessages(ctx, db, finalSelection, selectionArgs, uri);
    }
    else if (TABLE_PART.Equals(table)) {
        deletedRows = DeleteParts(db, finalSelection, selectionArgs);
    }
    else if (TABLE_DRM.Equals(table)) {
        deletedRows = DeleteTempDrmData(db, finalSelection, selectionArgs);
    }
    else {
        db->Delete(table, finalSelection, selectionArgs, &deletedRows);
    }

    if ((deletedRows > 0) && notify) {
        NotifyChange();
    }
    *result = deletedRows;
    return NOERROR;
}

Int32 CMmsProvider::DeleteMessages(
    /* [in] */ IContext* context,
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ IUri* uri)
{
    AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(1);
    (*ss)[0] = IBaseColumns::ID;
    AutoPtr<ICursor> cursor;
    db->Query(TABLE_PDU, ss,
            selection, selectionArgs, String(NULL)
            , String(NULL), String(NULL), (ICursor**)&cursor);
    if (cursor == NULL) {
        return 0;
    }

    do {
        assert(cursor != NULL);
        Int32 count = 0;
        ASSERT_SUCCEEDED(cursor->GetCount(&count));
        if (count == 0) {
            return 0;
        }

        Boolean move = FALSE;
        while (cursor->MoveToNext(&move), move) {
            AutoPtr<ArrayOf<String> > s2 = ArrayOf<String>::Alloc(1);
            Int64 lv = 0;
            cursor->GetInt64(0, &lv);
            (*s2)[0] = StringUtils::ToString(lv);
            DeleteParts(db, ITelephonyMmsPart::MSG_ID + " = ?", s2);
        }
    } while(0);
    ICloseable::Probe(cursor)->Close();

    Int32 count = 0;
    db->Delete(TABLE_PDU, selection, selectionArgs, &count);
    if (count > 0) {
        AutoPtr<IIntent> intent;
        CIntent::New(ITelephonyMmsIntents::CONTENT_CHANGED_ACTION, (IIntent**)&intent);
        intent->PutExtra(ITelephonyMmsIntents::DELETED_CONTENTS, IParcelable::Probe(uri));
        if (LOCAL_LOGV) {
            Logger::V(TAG, "Broadcasting intent: %s", TO_CSTR(intent));
        }
        context->SendBroadcast(intent);
    }
    return count;
}

Int32 CMmsProvider::DeleteParts(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs)
{
    return DeleteDataRows(db, TABLE_PART, selection, selectionArgs);
}

Int32 CMmsProvider::DeleteTempDrmData(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs)
{
    return DeleteDataRows(db, TABLE_DRM, selection, selectionArgs);
}

Int32 CMmsProvider::DeleteDataRows(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& table,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs)
{
    AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(1);
    (*ss)[0] = String("_data");
    AutoPtr<ICursor> cursor;
    db->Query(table, ss,
            selection, selectionArgs
            , String(NULL), String(NULL), String(NULL), (ICursor**)&cursor);
    if (cursor == NULL) {
        // FIXME: This might be an error, ignore it may cause
        // unpredictable result.
        return 0;
    }

    // try {
        assert(cursor != NULL);
        Int32 count = 0;
        cursor->GetCount(&count);
        if (count == 0) {
            return 0;
        }

        Boolean tmp = FALSE;
        while (cursor->MoveToNext(&tmp), tmp) {
            // try {
                // Delete the associated files saved on file-system.
                String path;
                cursor->GetString(0, &path);
                if (path != NULL) {
                    AutoPtr<IFile> f;
                    CFile::New(path, (IFile**)&f);
                    f->Delete();
                }
            // } catch (Throwable ex) {
            //     Logger::E(TAG, ex.getMessage(), ex);
            // }
        }
    // } finally {
        ICloseable::Probe(cursor)->Close();
    // }

    Int32 v = 0;
    db->Delete(table, selection, selectionArgs, &v);
    return v;
}

// @Override
ECode CMmsProvider::Update(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    // Don't let anyone update the _data column
    Boolean tmp = FALSE;
    if (values != NULL && (values->ContainsKey(ITelephonyMmsPart::_DATA, &tmp), tmp)) {
        return NOERROR;
    }
    Int32 match = 0;
    sURLMatcher->Match(uri, &match);
    if (LOCAL_LOGV) {
        Logger::V(TAG, "Update uri=%s, match=%d", TO_CSTR(uri), match);
    }

    Boolean notify = FALSE;
    String msgId;
    String table;

    switch (match) {
        case MMS_ALL_ID:
        case MMS_INBOX_ID:
        case MMS_SENT_ID:
        case MMS_DRAFTS_ID:
        case MMS_OUTBOX_ID: {
            uri->GetLastPathSegment(&msgId);
        }
        // fall-through
        case MMS_ALL:
        case MMS_INBOX:
        case MMS_SENT:
        case MMS_DRAFTS:
        case MMS_OUTBOX:
            notify = TRUE;
            table = TABLE_PDU;
            break;

        case MMS_MSG_PART:
        case MMS_PART_ID:
            table = TABLE_PART;
            break;

        case MMS_PART_RESET_FILE_PERMISSION: {
            AutoPtr<IContext> ctx;
            GetContext((IContext**)&ctx);
            AutoPtr<IFile> dir;
            ctx->GetDir(String("parts"), 0, (IFile**)&dir);
            String p;
            dir->GetPath(&p);
            AutoPtr<IList> l;
            uri->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(1, (IInterface**)&obj);

            String path = p + "/" + TO_STR(obj);
            // Reset the file permission back to read for everyone but me.
            Int32 result = FileUtils::SetPermissions(path, 0644, -1, -1);
            if (LOCAL_LOGV) {
                Logger::D(TAG, "MmsProvider.update setPermissions result: %d for path: %s"
                    , result, path.string());
            }
            return NOERROR;
        }
        default: {
            Logger::W(TAG, "Update operation for '%s' not implemented.", TO_CSTR(uri));
            return NOERROR;
        }
    }

    String extraSelection;
    AutoPtr<IContentValues> finalValues;
    if (table.Equals(TABLE_PDU)) {
        // Filter keys that we don't support yet.
        FilterUnsupportedKeys(values);
        CContentValues::New(values, (IContentValues**)&finalValues);

        if (msgId != NULL) {
            extraSelection = IBaseColumns::ID + "=" + msgId;
        }
    }
    else if (table.Equals(TABLE_PART)) {
        CContentValues::New(values, (IContentValues**)&finalValues);

        switch (match) {
            case MMS_MSG_PART: {
                AutoPtr<IList> l;
                uri->GetPathSegments((IList**)&l);
                AutoPtr<IInterface> obj;
                l->Get(0, (IInterface**)&obj);
                extraSelection = ITelephonyMmsPart::MSG_ID + "=" + TO_STR(obj);
                break;
            }
            case MMS_PART_ID: {
                AutoPtr<IList> l;
                uri->GetPathSegments((IList**)&l);
                AutoPtr<IInterface> obj;
                l->Get(1, (IInterface**)&obj);
                extraSelection = IBaseColumns::ID + "=" + TO_STR(obj);
                break;
            }
            default:
                break;
        }
    }
    else {
        return NOERROR;
    }

    String finalSelection = ConcatSelections(selection, extraSelection);
    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    Int32 count = 0;
    db->Update(table, finalValues, finalSelection, selectionArgs, &count);
    if (notify && (count > 0)) {
        NotifyChange();
    }
    *result = count;
    return NOERROR;
}

// @Override
ECode CMmsProvider::OpenFile(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [out] */ IParcelFileDescriptor** result) /*throws FileNotFoundException*/
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    Int32 match = 0;
    sURLMatcher->Match(uri, &match);

    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::D(TAG, "openFile: uri=%s, mode=%s, match=%d", TO_CSTR(uri), mode.string(), match);
    }

    if (match != MMS_PART_ID) {
        return NOERROR;
    }

    // Verify that the _data path points to mms data
    AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(1);
    (*ss)[0] = String("_data");
    AutoPtr<ICursor> c;
    Query(uri, ss, String(NULL), NULL, String(NULL), (ICursor**)&c);
    Int32 v = 0;
    Int32 count = (c != NULL) ? (c->GetCount(&v), v) : 0;
    if (count != 1) {
        // If there is not exactly one result, throw an appropriate
        // exception.
        if (c != NULL) {
            ICloseable::Probe(c)->Close();
        }
        if (count == 0) {
            // throw new FileNotFoundException("No entry for " + uri);
            return E_FILE_NOT_FOUND_EXCEPTION;
        }
        // throw new FileNotFoundException("Multiple items at " + uri);
        return E_FILE_NOT_FOUND_EXCEPTION;
    }

    Boolean tmp = FALSE;
    c->MoveToFirst(&tmp);
    Int32 i = 0;
    c->GetColumnIndex(String("_data"), &i);
    String sv;
    String path = (i >= 0 ? (c->GetString(i, &sv), sv) : String(NULL));
    ICloseable::Probe(c)->Close();

    if (path == NULL) {
        return NOERROR;
    }
    ECode ec = NOERROR;
    do {
        AutoPtr<IFile> filePath;
        ec = CFile::New(path, (IFile**)&filePath);
        if (FAILED(ec)) {
            break;
        }
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        AutoPtr<IApplicationInfo> info;
        ec = ctx->GetApplicationInfo((IApplicationInfo**)&info);
        if (FAILED(ec)) {
            break;
        }
        String dataDir;
        ec = info->GetDataDir(&dataDir);
        if (FAILED(ec)) {
            break;
        }
        String path;
        ec = filePath->GetCanonicalPath(&path);
        if (FAILED(ec)) {
            break;
        }
        if (!path.StartWith(dataDir + "/app_parts/")) {
            return NOERROR;
        }
    } while (0);
    if (FAILED(ec)) {
        return NOERROR;
    }

    return OpenFileHelper(uri, mode, result);
}

void CMmsProvider::FilterUnsupportedKeys(
    /* [in] */ IContentValues* values)
{
    // Some columns are unsupported.  They should therefore
    // neither be inserted nor updated.  Filter them out.
    values->Remove(ITelephonyBaseMmsColumns::DELIVERY_TIME_TOKEN);
    values->Remove(ITelephonyBaseMmsColumns::SENDER_VISIBILITY);
    values->Remove(ITelephonyBaseMmsColumns::REPLY_CHARGING);
    values->Remove(ITelephonyBaseMmsColumns::REPLY_CHARGING_DEADLINE_TOKEN);
    values->Remove(ITelephonyBaseMmsColumns::REPLY_CHARGING_DEADLINE);
    values->Remove(ITelephonyBaseMmsColumns::REPLY_CHARGING_ID);
    values->Remove(ITelephonyBaseMmsColumns::REPLY_CHARGING_SIZE);
    values->Remove(ITelephonyBaseMmsColumns::PREVIOUSLY_SENT_BY);
    values->Remove(ITelephonyBaseMmsColumns::PREVIOUSLY_SENT_DATE);
    values->Remove(ITelephonyBaseMmsColumns::STORE);
    values->Remove(ITelephonyBaseMmsColumns::MM_STATE);
    values->Remove(ITelephonyBaseMmsColumns::MM_FLAGS_TOKEN);
    values->Remove(ITelephonyBaseMmsColumns::MM_FLAGS);
    values->Remove(ITelephonyBaseMmsColumns::STORE_STATUS);
    values->Remove(ITelephonyBaseMmsColumns::STORE_STATUS_TEXT);
    values->Remove(ITelephonyBaseMmsColumns::STORED);
    values->Remove(ITelephonyBaseMmsColumns::TOTALS);
    values->Remove(ITelephonyBaseMmsColumns::MBOX_TOTALS);
    values->Remove(ITelephonyBaseMmsColumns::MBOX_TOTALS_TOKEN);
    values->Remove(ITelephonyBaseMmsColumns::QUOTAS);
    values->Remove(ITelephonyBaseMmsColumns::MBOX_QUOTAS);
    values->Remove(ITelephonyBaseMmsColumns::MBOX_QUOTAS_TOKEN);
    values->Remove(ITelephonyBaseMmsColumns::MESSAGE_COUNT);
    values->Remove(ITelephonyBaseMmsColumns::START);
    values->Remove(ITelephonyBaseMmsColumns::DISTRIBUTION_INDICATOR);
    values->Remove(ITelephonyBaseMmsColumns::ELEMENT_DESCRIPTOR);
    values->Remove(ITelephonyBaseMmsColumns::LIMIT);
    values->Remove(ITelephonyBaseMmsColumns::RECOMMENDED_RETRIEVAL_MODE);
    values->Remove(ITelephonyBaseMmsColumns::RECOMMENDED_RETRIEVAL_MODE_TEXT);
    values->Remove(ITelephonyBaseMmsColumns::STATUS_TEXT);
    values->Remove(ITelephonyBaseMmsColumns::APPLIC_ID);
    values->Remove(ITelephonyBaseMmsColumns::REPLY_APPLIC_ID);
    values->Remove(ITelephonyBaseMmsColumns::AUX_APPLIC_ID);
    values->Remove(ITelephonyBaseMmsColumns::DRM_CONTENT);
    values->Remove(ITelephonyBaseMmsColumns::ADAPTATION_ALLOWED);
    values->Remove(ITelephonyBaseMmsColumns::REPLACE_ID);
    values->Remove(ITelephonyBaseMmsColumns::CANCEL_ID);
    values->Remove(ITelephonyBaseMmsColumns::CANCEL_STATUS);

    // Keys shouldn't be inserted or updated.
    values->Remove(IBaseColumns::ID);
}

void CMmsProvider::NotifyChange()
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IContentResolver> cr;
    ctx->GetContentResolver((IContentResolver**)&cr);
    cr->NotifyChange(Elastos::Droid::Provider::Telephony::MmsSms::CONTENT_URI, NULL, TRUE, IUserHandle::USER_ALL);
}

Boolean CMmsProvider::Init()
{
    assert(sURLMatcher != NULL);
    sURLMatcher->AddURI(String("mms"), String(NULL),         MMS_ALL);
    sURLMatcher->AddURI(String("mms"), String("#"),          MMS_ALL_ID);
    sURLMatcher->AddURI(String("mms"), String("inbox"),      MMS_INBOX);
    sURLMatcher->AddURI(String("mms"), String("inbox/#"),    MMS_INBOX_ID);
    sURLMatcher->AddURI(String("mms"), String("sent"),       MMS_SENT);
    sURLMatcher->AddURI(String("mms"), String("sent/#"),     MMS_SENT_ID);
    sURLMatcher->AddURI(String("mms"), String("drafts"),     MMS_DRAFTS);
    sURLMatcher->AddURI(String("mms"), String("drafts/#"),   MMS_DRAFTS_ID);
    sURLMatcher->AddURI(String("mms"), String("outbox"),     MMS_OUTBOX);
    sURLMatcher->AddURI(String("mms"), String("outbox/#"),   MMS_OUTBOX_ID);
    sURLMatcher->AddURI(String("mms"), String("part"),       MMS_ALL_PART);
    sURLMatcher->AddURI(String("mms"), String("#/part"),     MMS_MSG_PART);
    sURLMatcher->AddURI(String("mms"), String("part/#"),     MMS_PART_ID);
    sURLMatcher->AddURI(String("mms"), String("#/addr"),     MMS_MSG_ADDR);
    sURLMatcher->AddURI(String("mms"), String("rate"),       MMS_SENDING_RATE);
    sURLMatcher->AddURI(String("mms"), String("report-status/#"),  MMS_REPORT_STATUS);
    sURLMatcher->AddURI(String("mms"), String("report-request/#"), MMS_REPORT_REQUEST);
    sURLMatcher->AddURI(String("mms"), String("drm"),        MMS_DRM_STORAGE);
    sURLMatcher->AddURI(String("mms"), String("drm/#"),      MMS_DRM_STORAGE_ID);
    sURLMatcher->AddURI(String("mms"), String("threads"),    MMS_THREADS);
    sURLMatcher->AddURI(String("mms"), String("resetFilePerm/*"),    MMS_PART_RESET_FILE_PERMISSION);
    sURLMatcher->AddURI(String("mms"), String("thread/#"), MMS_THREAD_ID);
    return TRUE;
}

String CMmsProvider::ConcatSelections(
    /* [in] */ const String& selection1,
    /* [in] */ const String& selection2)
{
    if (TextUtils::IsEmpty(selection1)) {
        return selection2;
    }
    else if (TextUtils::IsEmpty(selection2)) {
        return selection1;
    }
    else {
        return selection1 + " AND " + selection2;
    }
}

} // namespace Telephony
} // namespace Providers
} // namespace Droid
} // namespace Elastos
