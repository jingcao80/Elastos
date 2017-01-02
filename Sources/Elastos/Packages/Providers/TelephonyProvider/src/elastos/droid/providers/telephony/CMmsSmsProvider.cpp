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

#include "elastos/droid/providers/telephony/CMmsSmsProvider.h"
#include "elastos/droid/providers/telephony/CMmsProvider.h"
#include "elastos/droid/providers/telephony/CSmsProvider.h"
#include "elastos/droid/providers/telephony/MmsSmsDatabaseHelper.h"
#include "elastos/droid/provider/Telephony.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/droid/R.h>
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Google.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CUriMatcher;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IUriMatcher;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::CDatabaseUtils;
using Elastos::Droid::Database::IDatabaseUtils;
using Elastos::Droid::Database::Sqlite::CSQLiteQueryBuilder;
using Elastos::Droid::Database::Sqlite::ISQLiteQueryBuilder;
using Elastos::Droid::Google::Mms::Pdu::IPduHeaders;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::ITelephonyBaseMmsColumns;
using Elastos::Droid::Provider::ITelephonyCanonicalAddressesColumns;
using Elastos::Droid::Provider::ITelephonyMmsSms;
using Elastos::Droid::Provider::ITelephonyMmsSmsPendingMessages;
using Elastos::Droid::Provider::ITelephonyTextBasedSmsColumns;
using Elastos::Droid::Provider::ITelephonyThreads;
using Elastos::Droid::Provider::ITelephonyThreadsColumns;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Arrays;
using Elastos::Utility::ICollection;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IIterator;
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

static AutoPtr<ArrayOf<String> > InitMMS_SMS_COLUMNS()
{
    AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(7);
    (*ss)[0] = IBaseColumns::ID;
    (*ss)[1] = ITelephonyBaseMmsColumns::DATE;
    (*ss)[2] = ITelephonyBaseMmsColumns::DATE_SENT;
    (*ss)[3] = ITelephonyBaseMmsColumns::READ;
    (*ss)[4] = ITelephonyBaseMmsColumns::THREAD_ID;
    (*ss)[5] = ITelephonyBaseMmsColumns::LOCKED;
    (*ss)[6] = ITelephonyBaseMmsColumns::PHONE_ID;
    return ss;
}

static AutoPtr<ArrayOf<String> > InitMMS_ONLY_COLUMNS()
{
    AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(24);
    (*ss)[0] = ITelephonyBaseMmsColumns::CONTENT_CLASS;
    (*ss)[1] = ITelephonyBaseMmsColumns::CONTENT_LOCATION;
    (*ss)[2] = ITelephonyBaseMmsColumns::CONTENT_TYPE;
    (*ss)[3] = ITelephonyBaseMmsColumns::DELIVERY_REPORT;
    (*ss)[4] = ITelephonyBaseMmsColumns::EXPIRY;
    (*ss)[5] = ITelephonyBaseMmsColumns::MESSAGE_CLASS;
    (*ss)[6] = ITelephonyBaseMmsColumns::MESSAGE_ID;
    (*ss)[7] = ITelephonyBaseMmsColumns::MESSAGE_SIZE;
    (*ss)[8] = ITelephonyBaseMmsColumns::MESSAGE_TYPE;
    (*ss)[9] = ITelephonyBaseMmsColumns::MESSAGE_BOX;
    (*ss)[10] = ITelephonyBaseMmsColumns::PRIORITY;
    (*ss)[11] = ITelephonyBaseMmsColumns::READ_STATUS;
    (*ss)[12] = ITelephonyBaseMmsColumns::RESPONSE_STATUS;
    (*ss)[13] = ITelephonyBaseMmsColumns::RESPONSE_TEXT;
    (*ss)[14] = ITelephonyBaseMmsColumns::RETRIEVE_STATUS;
    (*ss)[15] = ITelephonyBaseMmsColumns::RETRIEVE_TEXT_CHARSET;
    (*ss)[16] = ITelephonyBaseMmsColumns::REPORT_ALLOWED;
    (*ss)[17] = ITelephonyBaseMmsColumns::READ_REPORT;
    (*ss)[18] = ITelephonyBaseMmsColumns::STATUS;
    (*ss)[19] = ITelephonyBaseMmsColumns::SUBJECT;
    (*ss)[20] = ITelephonyBaseMmsColumns::SUBJECT_CHARSET;
    (*ss)[21] = ITelephonyBaseMmsColumns::TRANSACTION_ID;
    (*ss)[22] = ITelephonyBaseMmsColumns::MMS_VERSION;
    (*ss)[23] = ITelephonyBaseMmsColumns::TEXT_ONLY;
    return ss;
}

static AutoPtr<ArrayOf<String> > InitSMS_ONLY_COLUMNS()
{
    AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(10);
    (*ss)[0] = String("address");
    (*ss)[1] = String("body");
    (*ss)[2] = String("person");
    (*ss)[3] = String("reply_path_present");
    (*ss)[4] = String("service_center");
    (*ss)[5] = String("status");
    (*ss)[6] = String("subject");
    (*ss)[7] = String("type");
    (*ss)[8] = String("error_code");
    (*ss)[9] = String("priority");
    return ss;
}

static AutoPtr<ArrayOf<String> > InitTHREADS_COLUMNS()
{
    AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(4);
    (*ss)[0] = IBaseColumns::ID;
    (*ss)[1] = ITelephonyThreadsColumns::DATE;
    (*ss)[2] = ITelephonyThreadsColumns::RECIPIENT_IDS;
    (*ss)[3] = ITelephonyThreadsColumns::MESSAGE_COUNT;
    return ss;
}

static AutoPtr<ArrayOf<String> > InitCANONICAL_ADDRESSES_COLUMNS_1()
{
    AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(4);
    (*ss)[0] = ITelephonyCanonicalAddressesColumns::ADDRESS;
    return ss;
}

static AutoPtr<ArrayOf<String> > InitCANONICAL_ADDRESSES_COLUMNS_2()
{
    AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(4);
    (*ss)[0] = IBaseColumns::ID;
    (*ss)[1] = ITelephonyCanonicalAddressesColumns::ADDRESS;
    return ss;
}

static AutoPtr<ArrayOf<String> > InitID_PROJECTION()
{
    AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(4);
    (*ss)[0] = IBaseColumns::ID;
    return ss;
}

static AutoPtr<ArrayOf<String> > InitTYPE_COUNT_PROJECTION()
{
    AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(4);
    (*ss)[0] = ITelephonyMmsSms::TYPE_DISCRIMINATOR_COLUMN;
    (*ss)[1] = String("count(*) as ") + ITelephonyMmsSms::TRANSPORT_TYPE_COUNT_COLUMN;
    return ss;
}

static AutoPtr<ISet> InitSet()
{
    AutoPtr<ISet> hs;
    CHashSet::New((ISet**)&hs);
    return hs;
}

AutoPtr<IUriMatcher> CMmsSmsProvider::URI_MATCHER = InitUriMatcher();
const String CMmsSmsProvider::TAG("MmsSmsProvider");
const Boolean CMmsSmsProvider::DEBUG = FALSE;
const String CMmsSmsProvider::NO_DELETES_INSERTS_OR_UPDATES(
        "MmsSmsProvider does not support deletes, inserts, or updates for this URI.");
const Int32 CMmsSmsProvider::URI_CONVERSATIONS                     = 0;
const Int32 CMmsSmsProvider::URI_CONVERSATIONS_MESSAGES            = 1;
const Int32 CMmsSmsProvider::URI_CONVERSATIONS_RECIPIENTS          = 2;
const Int32 CMmsSmsProvider::URI_MESSAGES_BY_PHONE                 = 3;
const Int32 CMmsSmsProvider::URI_THREAD_ID                         = 4;
const Int32 CMmsSmsProvider::URI_CANONICAL_ADDRESS                 = 5;
const Int32 CMmsSmsProvider::URI_PENDING_MSG                       = 6;
const Int32 CMmsSmsProvider::URI_COMPLETE_CONVERSATIONS            = 7;
const Int32 CMmsSmsProvider::URI_UNDELIVERED_MSG                   = 8;
const Int32 CMmsSmsProvider::URI_CONVERSATIONS_SUBJECT             = 9;
const Int32 CMmsSmsProvider::URI_NOTIFICATIONS                     = 10;
const Int32 CMmsSmsProvider::URI_OBSOLETE_THREADS                  = 11;
const Int32 CMmsSmsProvider::URI_DRAFT                             = 12;
const Int32 CMmsSmsProvider::URI_CANONICAL_ADDRESSES               = 13;
const Int32 CMmsSmsProvider::URI_SEARCH                            = 14;
const Int32 CMmsSmsProvider::URI_SEARCH_SUGGEST                    = 15;
const Int32 CMmsSmsProvider::URI_FIRST_LOCKED_MESSAGE_ALL          = 16;
const Int32 CMmsSmsProvider::URI_FIRST_LOCKED_MESSAGE_BY_THREAD_ID = 17;
const Int32 CMmsSmsProvider::URI_MESSAGE_ID_TO_THREAD              = 18;
const Int32 CMmsSmsProvider::URI_MAILBOX_MESSAGES                  = 19;
const Int32 CMmsSmsProvider::URI_CONVERSATION_TYPE_COUNT           = 20;
const Int32 CMmsSmsProvider::URI_SEARCH_MESSAGE                    = 21;
const Char32 CMmsSmsProvider::SEARCH_ESCAPE_CHARACTER = '!';
const String CMmsSmsProvider::TABLE_CANONICAL_ADDRESSES("canonical_addresses");

AutoPtr<ArrayOf<String> > CMmsSmsProvider::MMS_SMS_COLUMNS = InitMMS_SMS_COLUMNS();

// These are the columns that appear only in the MMS message
// table.
AutoPtr<ArrayOf<String> > CMmsSmsProvider::MMS_ONLY_COLUMNS = InitMMS_ONLY_COLUMNS();

// These are the columns that appear only in the SMS message
// table.
AutoPtr<ArrayOf<String> > CMmsSmsProvider::SMS_ONLY_COLUMNS = InitSMS_ONLY_COLUMNS();

// These are all the columns that appear in the "threads" table.
AutoPtr<ArrayOf<String> > CMmsSmsProvider::THREADS_COLUMNS = InitTHREADS_COLUMNS();

AutoPtr<ArrayOf<String> > CMmsSmsProvider::CANONICAL_ADDRESSES_COLUMNS_1 = InitCANONICAL_ADDRESSES_COLUMNS_1();

AutoPtr<ArrayOf<String> > CMmsSmsProvider::CANONICAL_ADDRESSES_COLUMNS_2 = InitCANONICAL_ADDRESSES_COLUMNS_2();

// These are all the columns that appear in the MMS and SMS
// message tables.
AutoPtr<ArrayOf<String> > CMmsSmsProvider::UNION_COLUMNS = ArrayOf<String>::Alloc(
    7/*MMS_SMS_COLUMNS.length*/ + 24/*MMS_ONLY_COLUMNS.length*/ + 10/*SMS_ONLY_COLUMNS.length*/);

// These are all the columns that appear in the MMS table.
AutoPtr<ISet> CMmsSmsProvider::MMS_COLUMNS = InitSet();

// These are all the columns that appear in the SMS table.
AutoPtr<ISet> CMmsSmsProvider::SMS_COLUMNS = InitSet();

const String CMmsSmsProvider::VND_ANDROID_DIR_MMS_SMS("vnd.android-dir/mms-sms");

AutoPtr<ArrayOf<String> > CMmsSmsProvider::ID_PROJECTION = InitID_PROJECTION();

const AutoPtr<ArrayOf<String> > CMmsSmsProvider::EMPTY_STRING_ARRAY = ArrayOf<String>::Alloc(0);

const AutoPtr<ArrayOf<String> > CMmsSmsProvider::SEARCH_STRING = ArrayOf<String>::Alloc(1);
const String CMmsSmsProvider::SEARCH_QUERY = String("SELECT snippet(words, '', ' ', '', 1, 1) as ") +
        "snippet FROM words WHERE index_text MATCH ? ORDER BY snippet LIMIT 50;";

const String CMmsSmsProvider::SMS_CONVERSATION_CONSTRAINT = String("(") +
        ITelephonyTextBasedSmsColumns::TYPE + " != "
        + StringUtils::ToString(ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_DRAFT) + ")";

const String CMmsSmsProvider::MMS_CONVERSATION_CONSTRAINT = String("(") +
        ITelephonyBaseMmsColumns::MESSAGE_BOX + " != "
        + StringUtils::ToString(ITelephonyBaseMmsColumns::MESSAGE_BOX_DRAFTS) + " AND (" +
        ITelephonyBaseMmsColumns::MESSAGE_TYPE + " = " + StringUtils::ToString(IPduHeaders::MESSAGE_TYPE_SEND_REQ) + " OR " +
        ITelephonyBaseMmsColumns::MESSAGE_TYPE + " = " + StringUtils::ToString(IPduHeaders::MESSAGE_TYPE_RETRIEVE_CONF) + " OR " +
        ITelephonyBaseMmsColumns::MESSAGE_TYPE + " = " + StringUtils::ToString(IPduHeaders::MESSAGE_TYPE_NOTIFICATION_IND) + "))";

// Search on the words table but return the rows from the corresponding sms table
const String CMmsSmsProvider::SMS_QUERY =
        String("SELECT sms._id AS _id,thread_id,address,body,date,date_sent,index_text,words._id ") +
        "FROM sms,words WHERE (index_text MATCH ? " +
        "AND sms._id=words.source_id AND words.table_to_use=1)";

// Search on the words table but return the rows from the corresponding parts table
const String CMmsSmsProvider::MMS_QUERY =
        String("SELECT pdu._id,thread_id,addr.address,part.text ") +
        "AS body,pdu.date,pdu.date_sent,index_text,words._id " +
        "FROM pdu,part,addr,words WHERE ((part.mid=pdu._id) AND " +
        "(addr.msg_id=pdu._id) AND " +
        "(addr.type=" + StringUtils::ToString(IPduHeaders::TO) + ") AND " +
        "(part.ct='text/plain') AND " +
        "(index_text MATCH ?) AND " +
        "(part._id = words.source_id) AND " +
        "(words.table_to_use=2))";

AutoPtr<ArrayOf<String> > CMmsSmsProvider::TYPE_COUNT_PROJECTION = InitTYPE_COUNT_PROJECTION();

// This code queries the sms and mms tables and returns a unified result set
// of text matches.  We query the sms table which is pretty simple.  We also
// query the pdu, part and addr table to get the mms result.  Notet we're
// using a UNION so we have to have the same number of result columns from
// both queries.
const String CMmsSmsProvider::SMS_MMS_QUERY =
        SMS_QUERY + " UNION " + MMS_QUERY +
        " GROUP BY thread_id ORDER BY thread_id ASC, date DESC";

const String CMmsSmsProvider::THREADS_BY_PHONE_ID_WHERE =
        String("_id in (select thread_id from sms where phone_id=? ") +
                "union select thread_id from pdu where phone_id=?)";

const String CMmsSmsProvider::SMS_PROJECTION = String("'sms' AS transport_type, _id, thread_id,")
        + "address, body, phone_id, date, date_sent, read, type,"
        + "status, locked, NULL AS error_code,"
        + "NULL AS sub, NULL AS sub_cs, date, date_sent, read,"
        + "NULL as m_type,"
        + "NULL AS msg_box,"
        + "NULL AS d_rpt, NULL AS rr, NULL AS err_type,"
        + "locked, NULL AS st, NULL AS text_only,"
        + "phone_id, NULL AS recipient_ids";

const String CMmsSmsProvider::MMS_PROJECTION = String("'mms' AS transport_type, pdu._id, thread_id,")
        + "addr.address AS address, part.text as body, phone_id,"
        + "pdu.date * 1000 AS date, date_sent, read, NULL AS type,"
        + "NULL AS status, locked, NULL AS error_code,"
        + "sub, sub_cs, date, date_sent, read,"
        + "m_type,"
        + "pdu.msg_box AS msg_box,"
        + "d_rpt, rr, NULL AS err_type,"
        + "locked, NULL AS st, NULL AS text_only,"
        + "phone_id, NULL AS recipient_ids";

const String CMmsSmsProvider::MMS_PROJECTION_FOR_NUMBER_SEARCH =
        String("'mms' AS transport_type, pdu._id, thread_id,")
        + "addr.address AS address, NULL AS body, phone_id,"
        + "pdu.date * 1000 AS date, date_sent, read, NULL AS type,"
        + "NULL AS status, locked, NULL AS error_code,"
        + "sub, sub_cs, date, date_sent, read,"
        + "m_type,"
        + "pdu.msg_box AS msg_box,"
        + "d_rpt, rr, NULL AS err_type,"
        + "locked, NULL AS st, NULL AS text_only,"
        + "phone_id, NULL AS recipient_ids";

const String CMmsSmsProvider::AUTHORITY("mms-sms");
const String CMmsSmsProvider::THREAD_QUERY("SELECT _id FROM threads WHERE recipient_ids=?");
Boolean CMmsSmsProvider::sInit = Init();


CAR_OBJECT_IMPL(CMmsSmsProvider)
CAR_INTERFACE_IMPL(CMmsSmsProvider, ContentProvider, IMmsSmsProvider)

ECode CMmsSmsProvider::OnCreate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    SetAppOps(IAppOpsManager::OP_READ_SMS, IAppOpsManager::OP_WRITE_SMS);
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    mOpenHelper = MmsSmsDatabaseHelper::GetInstance(ctx);
    AutoPtr<IResources> res;
    ctx->GetResources((IResources**)&res);
    res->GetBoolean(Elastos::Droid::R::bool_::config_use_strict_phone_number_comparation
            , &mUseStrictPhoneNumberComparation);
    *result = TRUE;
    return NOERROR;
}

// @Override
ECode CMmsSmsProvider::Query(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& _selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<ICursor> cursor;
    String selection = _selection;
    Int32 match = 0;
    URI_MATCHER->Match(uri, &match);
    switch(match) {
        case URI_COMPLETE_CONVERSATIONS:
            cursor = GetCompleteConversations(projection, selection, sortOrder);
            break;
        case URI_CONVERSATIONS: {
            String simple;
            uri->GetQueryParameter(String("simple"), &simple);
            if ((simple != NULL) && simple.Equals("true")) {
                String threadType;
                uri->GetQueryParameter(String("thread_type"), &threadType);
                if (!TextUtils::IsEmpty(threadType)) {
                    selection = ConcatSelections(
                            selection, ITelephonyThreadsColumns::TYPE + "=" + threadType);
                }

                String phoneId;
                uri->GetQueryParameter(String("phone_id"), &phoneId);
                if (!TextUtils::IsEmpty(phoneId)) {
                    selection = ConcatSelections(selection,
                            THREADS_BY_PHONE_ID_WHERE);
                    AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(2);
                    (*ss)[0] = phoneId;
                    (*ss)[1] = phoneId;
                    selectionArgs = AppendSelectionArgs(selectionArgs, ss);
                }

                cursor = GetSimpleConversations(
                        projection, selection, selectionArgs, sortOrder);
            }
            else {
                cursor = GetConversations(
                        projection, selection, sortOrder);
            }
            break;
        }
        case URI_CONVERSATIONS_MESSAGES: {
            AutoPtr<IList> l;
            uri->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(1, (IInterface**)&obj);
            cursor = GetConversationMessages(TO_STR(obj), projection, selection, sortOrder, FALSE);
            break;
        }
        case URI_MAILBOX_MESSAGES: {
            AutoPtr<IList> l;
            uri->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(1, (IInterface**)&obj);
            cursor = GetMailboxMessages(TO_STR(obj), projection, selection,
                    selectionArgs, sortOrder, FALSE);
            break;
        }
        case URI_CONVERSATIONS_RECIPIENTS: {
            AutoPtr<IList> l;
            uri->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(1, (IInterface**)&obj);
            cursor = GetConversationById(TO_STR(obj), projection, selection,
                    selectionArgs, sortOrder);
            break;
        }
        case URI_CONVERSATIONS_SUBJECT: {
            AutoPtr<IList> l;
            uri->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(1, (IInterface**)&obj);
            cursor = GetConversationById(TO_STR(obj), projection, selection,
                    selectionArgs, sortOrder);
            break;
        }
        case URI_MESSAGES_BY_PHONE: {
            AutoPtr<IList> l;
            uri->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(2, (IInterface**)&obj);
            cursor = GetMessagesByPhoneNumber(TO_STR(obj), projection, selection, sortOrder);
            break;
        }
        case URI_THREAD_ID: {
            AutoPtr<IList> recipients;/*<String*/
            uri->GetQueryParameters(String("recipient"), (IList**)&recipients);

            cursor = GetThreadId(recipients);
            break;
        }
        case URI_CANONICAL_ADDRESS: {
            AutoPtr<IList> l;
            uri->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(1, (IInterface**)&obj);
            String extraSelection = String("_id=") + TO_STR(obj);
            String finalSelection = TextUtils::IsEmpty(selection)
                    ? extraSelection : extraSelection + " AND " + selection;
            db->Query(TABLE_CANONICAL_ADDRESSES,
                    CANONICAL_ADDRESSES_COLUMNS_1,
                    finalSelection,
                    selectionArgs,
                    String(NULL), String(NULL),
                    sortOrder, (ICursor**)&cursor);
            break;
        }
        case URI_CANONICAL_ADDRESSES:
            db->Query(TABLE_CANONICAL_ADDRESSES,
                    CANONICAL_ADDRESSES_COLUMNS_2,
                    selection,
                    selectionArgs,
                    String(NULL), String(NULL),
                    sortOrder, (ICursor**)&cursor);
            break;
        case URI_SEARCH_SUGGEST: {
            String v;
            uri->GetQueryParameter(String("pattern"), &v);
            (*SEARCH_STRING)[0] = v + '*' ;

            // find the words which match the pattern using the snippet function.  The
            // snippet function parameters mainly describe how to format the result.
            // See http://www.sqlite.org/fts3.html#section_4_2 for details.
            if (sortOrder != NULL
                    || selection != NULL
                    || selectionArgs != NULL
                    || projection != NULL) {
                // throw new IllegalArgumentException(
                //         "do not specify sortOrder, selection, selectionArgs, or projection" +
                //         "with this query");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            db->RawQuery(SEARCH_QUERY, SEARCH_STRING, (ICursor**)&cursor);
            break;
        }
        case URI_MESSAGE_ID_TO_THREAD: {
            // Given a message ID and an indicator for SMS vs. MMS return
            // the thread id of the corresponding thread.
            String v;
            // try {
                uri->GetQueryParameter(String("row_id"), &v);
                Int64 id = StringUtils::ParseInt64(v);
                uri->GetQueryParameter(String("table_to_use"), &v);
                switch (StringUtils::ParseInt32(v)) {
                    case 1: { // sms
                        AutoPtr<ArrayOf<String> > s1 = ArrayOf<String>::Alloc(1);
                        (*s1)[0] = String("thread_id");
                        AutoPtr<ArrayOf<String> > s2 = ArrayOf<String>::Alloc(1);
                        (*s2)[0] = StringUtils::ToString(id);
                        db->Query(
                            String("sms"),
                            s1,
                            String("_id=?"),
                            s2,
                            String(NULL),
                            String(NULL),
                            String(NULL), (ICursor**)&cursor);
                        break;
                    }
                    case 2: { // mms
                        String mmsQuery = String("SELECT thread_id FROM pdu,part WHERE ((part.mid=pdu._id) AND (part._id=?))") +
                            "(part._id=?))";
                        AutoPtr<ArrayOf<String> > s2 = ArrayOf<String>::Alloc(1);
                        (*s2)[0] = StringUtils::ToString(id);
                        db->RawQuery(mmsQuery, s2, (ICursor**)&cursor);
                        break;
                    }
                }
            // } catch (NumberFormatException ex) {
            //     // ignore... return empty cursor
            // }
            break;
        }
        case URI_SEARCH: {
            if (       sortOrder != NULL
                    || selection != NULL
                    || selectionArgs != NULL
                    || projection != NULL) {
                // throw new IllegalArgumentException(
                //         "do not specify sortOrder, selection, selectionArgs, or projection" +
                //         "with this query");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            String v;
            uri->GetQueryParameter(String("pattern"), &v);
            String searchString = v + "*";

            AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(2);
            (*ss)[0] = searchString;
            (*ss)[1] = searchString;
            if (FAILED(db->RawQuery(SMS_MMS_QUERY, ss, (ICursor**)&cursor))) {
                Logger::E(TAG, "RawQuery: has error.");
            }

            break;
        }
        case URI_SEARCH_MESSAGE:
            cursor = GetSearchMessages(uri, db);
            break;
        case URI_PENDING_MSG: {
            String protoName;
            uri->GetQueryParameter(String("protocol"), &protoName);
            String msgId;
            uri->GetQueryParameter(String("message"), &msgId);
            Int32 proto = TextUtils::IsEmpty(protoName) ? -1
                    : (protoName.Equals("sms") ? ITelephonyMmsSms::SMS_PROTO : ITelephonyMmsSms::MMS_PROTO);

            String extraSelection = (proto != -1) ?
                    (ITelephonyMmsSmsPendingMessages::PROTO_TYPE + "=" + StringUtils::ToString(proto))
                    : String(" 0=0 ");
            if (!TextUtils::IsEmpty(msgId)) {
                extraSelection += String(" AND ") + ITelephonyMmsSmsPendingMessages::MSG_ID + "=" + msgId;
            }

            String finalSelection = TextUtils::IsEmpty(selection)
                    ? extraSelection : (String("(") + extraSelection + ") AND " + selection);
            String finalOrder = TextUtils::IsEmpty(sortOrder)
                    ? ITelephonyMmsSmsPendingMessages::DUE_TIME : sortOrder;
            db->Query(TABLE_PENDING_MSG, NULL, finalSelection, selectionArgs
                    , String(NULL), String(NULL), finalOrder, (ICursor**)&cursor);
            break;
        }
        case URI_UNDELIVERED_MSG: {
            cursor = GetUndeliveredMessages(projection, selection,
                    selectionArgs, sortOrder);
            break;
        }
        case URI_DRAFT: {
            cursor = GetDraftThread(projection, selection, sortOrder);
            break;
        }
        case URI_FIRST_LOCKED_MESSAGE_BY_THREAD_ID: {
            Int64 threadId;
            String v;
            uri->GetLastPathSegment(&v);
            // try {
            threadId = StringUtils::ParseInt64(v);
            // } catch (NumberFormatException e) {
            //     Logger::E(TAG, "Thread ID must be a Int64.");
            //     break;
            // }
            cursor = GetFirstLockedMessage(projection, String("thread_id=") + StringUtils::ToString(threadId),
                    sortOrder);
            break;
        }
        case URI_FIRST_LOCKED_MESSAGE_ALL: {
            cursor = GetFirstLockedMessage(projection, selection, sortOrder);
            break;
        }
        case URI_CONVERSATION_TYPE_COUNT: {
            AutoPtr<IList> l;
            uri->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(2, (IInterface**)&obj);
            cursor = GetConversationMessages(TO_STR(obj), projection,
                    selection, sortOrder, TRUE);
            break;
        }
        default: {
            // throw new IllegalStateException("Unrecognized URI:" + uri);
            Logger::D(TAG, "Unrecognized URI:%s", TO_CSTR(uri));
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }

    if (cursor != NULL) {
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        AutoPtr<IContentResolver> cr;
        ctx->GetContentResolver((IContentResolver**)&cr);
        cursor->SetNotificationUri(cr, Elastos::Droid::Provider::Telephony::MmsSms::CONTENT_URI);
    }
    *result = cursor;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * Return the canonical address ID for this address.
 */
Int64 CMmsSmsProvider::GetSingleAddressId(
    /* [in] */ const String& address)
{
    Boolean isEmail = FALSE;
    Elastos::Droid::Provider::Telephony::Mms::IsEmailAddress(address, &isEmail);
    Boolean isPhoneNumber = FALSE;
    Elastos::Droid::Provider::Telephony::Mms::IsPhoneNumber(address, &isPhoneNumber);

    // We lowercase all email addresses, but not addresses that aren't numbers, because
    // that would incorrectly turn an address such as "My Vodafone" into "my vodafone"
    // and the thread title would be incorrect when displayed in the UI.
    String refinedAddress = isEmail ? address.ToLowerCase() : address;

    String selection = String("address=?");
    AutoPtr<ArrayOf<String> > selectionArgs;
    Int64 retVal = -1L;

    if (!isPhoneNumber) {
        selectionArgs = ArrayOf<String>::Alloc(1);
        (*selectionArgs)[0] = refinedAddress;
    }
    else {
        selection += String(" OR PHONE_NUMBERS_EQUAL(address, ?, ") +
                    (mUseStrictPhoneNumberComparation ? String("1") : String("0")) + ")";
        selectionArgs = ArrayOf<String>::Alloc(2);
        (*selectionArgs)[0] = refinedAddress;
        (*selectionArgs)[1] = refinedAddress;
    }

    AutoPtr<ICursor> cursor;

    // try {
    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
    db->Query(
            String("canonical_addresses"), ID_PROJECTION,
            selection, selectionArgs, String(NULL), String(NULL), String(NULL), (ICursor**)&cursor);

    Int32 count = 0;
    cursor->GetCount(&count);
    if (count == 0) {
        AutoPtr<IContentValues> contentValues;
        CContentValues::New(1, (IContentValues**)&contentValues);
        contentValues->Put(ITelephonyCanonicalAddressesColumns::ADDRESS, refinedAddress);

        db = NULL;
        mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
        db->Insert(String("canonical_addresses"), ITelephonyCanonicalAddressesColumns::ADDRESS
                , contentValues, &retVal);

        Logger::D(TAG, "GetSingleAddressId: insert new canonical_address for xxxxxx, _id=%d", retVal);

        return retVal;
    }

    Boolean tmp = FALSE;
    if (cursor->MoveToFirst(&tmp), tmp) {
        Int32 columnIndex = 0;
        cursor->GetColumnIndexOrThrow(IBaseColumns::ID, &columnIndex);
        cursor->GetInt64(columnIndex, &retVal);
    }
    // } finally {
    if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }
    // }

    return retVal;
}

/**
 * Return the canonical address IDs for these addresses.
 */
AutoPtr<ISet/*<Long*/> CMmsSmsProvider::GetAddressIds(
    /* [in] */ IList/*<String>*/* addresses)
{
    AutoPtr<ISet> result;/*<Long*/
    Int32 size = 0;
    addresses->GetSize(&size);
    CHashSet::New(size, (ISet**)&result);

    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        addresses->Get(i, (IInterface**)&obj);
        String address = TO_STR(obj);
        if (!address.Equals(IPduHeaders::FROM_INSERT_ADDRESS_TOKEN_STR)) {
            Int64 id = GetSingleAddressId(address);
            if (id != -1L) {
                result->Add(CoreUtils::Convert(id));
            }
            else {
                Logger::E(TAG, "GetAddressIds: address ID not found for %s", address.string());
            }
        }
    }
    return result;
}

/**
 * Return a sorted array of the given Set of Longs.
 */
AutoPtr<ArrayOf<Int64> > CMmsSmsProvider::GetSortedSet(
    /* [in] */ ISet/*<Long>*/* numbers)
{
    Int32 size = 0;
    numbers->GetSize(&size);
    AutoPtr<ArrayOf<Int64> > result = ArrayOf<Int64>::Alloc(size);
    Int32 i = 0;

    AutoPtr<IIterator> it;
    numbers->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        Int64 number = 0;
        IInteger64::Probe(obj)->GetValue(&number);
        (*result)[i++] = number;
    }

    if (size > 1) {
        Arrays::Sort(result);
    }

    return result;
}

/**
 * Return a String of the numbers in the given array, in order,
 * separated by spaces.
 */
String CMmsSmsProvider::GetSpaceSeparatedNumbers(
    /* [in] */ ArrayOf<Int64>* numbers)
{
    Int32 size = numbers->GetLength();
    StringBuilder buffer;

    for (Int32 i = 0; i < size; i++) {
        if (i != 0) {
            buffer.AppendChar(' ');
        }
        buffer.Append((*numbers)[i]);
    }
    return buffer.ToString();
}

/**
 * Insert a record for a new thread.
 */
void CMmsSmsProvider::InsertThread(
    /* [in] */ const String& recipientIds,
    /* [in] */ Int32 numberOfRecipients)
{
    AutoPtr<IContentValues> values;
    CContentValues::New(4, (IContentValues**)&values);

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 date = 0;
    system->GetCurrentTimeMillis(&date);
    values->Put(ITelephonyThreadsColumns::DATE, date - date % 1000);
    values->Put(ITelephonyThreadsColumns::RECIPIENT_IDS, recipientIds);
    if (numberOfRecipients > 1) {
        values->Put(ITelephonyThreadsColumns::TYPE, ITelephonyThreads::BROADCAST_THREAD);
    }
    values->Put(ITelephonyThreadsColumns::MESSAGE_COUNT, 0);

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    Int64 result = 0;
    db->Insert(TABLE_THREADS, String(NULL), values, &result);
    Logger::D(TAG, "InsertThread: created new thread_id %lld for recipientIds xxxxxxx", result);

    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IContentResolver> cr;
    ctx->GetContentResolver((IContentResolver**)&cr);
    cr->NotifyChange(Elastos::Droid::Provider::Telephony::MmsSms::CONTENT_URI, NULL, TRUE, IUserHandle::USER_ALL);
}

/**
 * Return the thread ID for this list of
 * recipients IDs.  If no thread exists with this ID, create
 * one and return it.  Callers should always use
 * Threads.GetThreadId to access this information.
 */
AutoPtr<ICursor> CMmsSmsProvider::GetThreadId(
    /* [in] */ IList/*<String>*/* recipients)
{
    AutoLock lock(THIS);
    AutoPtr<ISet/*<Long*/> addressIds = GetAddressIds(recipients);
    String recipientIds("");

    Int32 size = 0;
    addressIds->GetSize(&size);
    if (size == 0) {
        Logger::E(TAG, "GetThreadId: NO receipients specified -- NOT creating thread");
        return NULL;
    }
    else if (size == 1) {
        // optimize for size==1, which should be most of the cases
        AutoPtr<IIterator> it;
        addressIds->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            recipientIds = TO_STR(obj);
        }
    }
    else {
        recipientIds = GetSpaceSeparatedNumbers(GetSortedSet(addressIds));
    }

    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::D(TAG, "GetThreadId: recipientIds (selectionArgs) =xxxxxxx");
    }

    AutoPtr<ArrayOf<String> > selectionArgs = ArrayOf<String>::Alloc(1);
    (*selectionArgs)[0] = recipientIds;

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
    db->BeginTransaction();
    AutoPtr<ICursor> cursor;
    do {
        // Find the thread with the given recipients
        if (FAILED(db->RawQuery(THREAD_QUERY, selectionArgs, (ICursor**)&cursor))) {
            break;
        }

        Int32 count = 0;
        cursor->GetCount(&count);
        if (count == 0) {
            // No thread with those recipients exists, so create the thread.
            ICloseable::Probe(cursor)->Close();

            Logger::D(TAG, "GetThreadId: create new thread_id for recipients xxxxxxxx");
            Int32 size = 0;
            recipients->GetSize(&size);
            InsertThread(recipientIds, size);

            // The thread was just created, now find it and return it.
            cursor = NULL;
            if (FAILED(db->RawQuery(THREAD_QUERY, selectionArgs, (ICursor**)&cursor))) {
                break;
            }
        }
        db->SetTransactionSuccessful();
    } while (0);
    // {
    //     Logger::E(TAG, ex.getMessage(), ex);
    // } finally {
        db->EndTransaction();
    // }

    Int32 count = 0;
    if (cursor != NULL && (cursor->GetCount(&count), count) > 1) {
        Logger::W(TAG, "GetThreadId: why is cursorCount=%d", count);
    }
    return cursor;
}

String CMmsSmsProvider::ConcatSelections(
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

/**
 * Returns new array of currentArgs with newArgs appended to end
 *
 * @param currentArgs
 * @param newArgs
 * @return
 */
AutoPtr<ArrayOf<String> > CMmsSmsProvider::AppendSelectionArgs(
    /* [in] */ ArrayOf<String>* _currentArgs,
    /* [in] */ ArrayOf<String>* newArgs)
{
    AutoPtr<ArrayOf<String> > currentArgs = _currentArgs;
    if (newArgs == NULL) {
        return currentArgs;
    }
    if(currentArgs == NULL) {
        currentArgs = ArrayOf<String>::Alloc(0);
    }

    Int32 len = newArgs->GetLength();
    Int32 len2 = currentArgs->GetLength();
    AutoPtr<ArrayOf<String> > newArray;
    Arrays::CopyOf(currentArgs, len2 + len, (ArrayOf<String>**)&newArray);
    for (Int32 i=0; i < len; ++i) {
        (*newArray)[len2 + i] = (*newArgs)[i];
    }
    return newArray;
}

/**
 * If a NULL projection is given, return the union of all columns
 * in both the MMS and SMS messages tables.  Otherwise, return the
 * given projection.
 */
AutoPtr<ArrayOf<String> > CMmsSmsProvider::HandleNullMessageProjection(
    /* [in] */ ArrayOf<String>* projection)
{
    return projection == NULL ? UNION_COLUMNS.Get() : projection;
}

/**
 * If a NULL projection is given, return the set of all columns in
 * the threads table.  Otherwise, return the given projection.
 */
AutoPtr<ArrayOf<String> > CMmsSmsProvider::HandleNullThreadsProjection(
    /* [in] */ ArrayOf<String>* projection)
{
    return projection == NULL ? THREADS_COLUMNS.Get() : projection;
}

/**
 * If a NULL sort order is given, return "normalized_date ASC".
 * Otherwise, return the given sort order.
 */
String CMmsSmsProvider::HandleNullSortOrder (
    /* [in] */ const String& sortOrder)
{
    return sortOrder == NULL ? String("normalized_date ASC") : sortOrder;
}

/**
 * Return existing threads in the database.
 */
AutoPtr<ICursor> CMmsSmsProvider::GetSimpleConversations(
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder)
{
    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<ICursor> c;
    db->Query(TABLE_THREADS, projection, selection, selectionArgs
        , String(NULL), String(NULL), String(" date DESC"), (ICursor**)&c);
    return c;
}

/**
 * Return the thread which has draft in both MMS and SMS.
 *
 * Use this query:
 *
 *   SELECT ...
 *     FROM (SELECT _id, thread_id, ...
 *             FROM pdu
 *             WHERE msg_box = 3 AND ...
 *           UNION
 *           SELECT _id, thread_id, ...
 *             FROM sms
 *             WHERE type = 3 AND ...
 *          )
 *   ;
 */
AutoPtr<ICursor> CMmsSmsProvider::GetDraftThread(
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ const String& sortOrder)
{
    AutoPtr<ArrayOf<String> > innerProjection = ArrayOf<String>::Alloc(2);
    (*innerProjection)[0] = IBaseColumns::ID;
    (*innerProjection)[1] = ITelephonyTextBasedSmsColumns::THREAD_ID;
    AutoPtr<ISQLiteQueryBuilder> mmsQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&mmsQueryBuilder);
    AutoPtr<ISQLiteQueryBuilder> smsQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&smsQueryBuilder);

    mmsQueryBuilder->SetTables(IMmsProvider::TABLE_PDU);
    smsQueryBuilder->SetTables(CSmsProvider::TABLE_SMS);

    String mmsSubQuery;
    mmsQueryBuilder->BuildUnionSubQuery(
            ITelephonyMmsSms::TYPE_DISCRIMINATOR_COLUMN, *innerProjection,
            MMS_COLUMNS, 1, String("mms"),
            ConcatSelections(selection, ITelephonyBaseMmsColumns::MESSAGE_BOX
                + "=" + StringUtils::ToString(ITelephonyBaseMmsColumns::MESSAGE_BOX_DRAFTS)),
            String(NULL), String(NULL), &mmsSubQuery);
    String smsSubQuery;
    smsQueryBuilder->BuildUnionSubQuery(
            ITelephonyMmsSms::TYPE_DISCRIMINATOR_COLUMN, *innerProjection,
            SMS_COLUMNS, 1, String("sms"),
            ConcatSelections(selection, ITelephonyTextBasedSmsColumns::TYPE
                + "=" + StringUtils::ToString(ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_DRAFT)),
            String(NULL), String(NULL), &smsSubQuery);
    AutoPtr<ISQLiteQueryBuilder> unionQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&unionQueryBuilder);

    unionQueryBuilder->SetDistinct(TRUE);

    String unionQuery;
    AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(2);
    (*ss)[0] = mmsSubQuery;
    (*ss)[1] = smsSubQuery;
    unionQueryBuilder->BuildUnionQuery(*ss, String(NULL), String(NULL), &unionQuery);

    AutoPtr<ISQLiteQueryBuilder> outerQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&outerQueryBuilder);

    outerQueryBuilder->SetTables(String("(") + unionQuery + ")");

    String outerQuery;
    outerQueryBuilder->BuildQuery(projection, String(NULL), String(NULL)
        , String(NULL), sortOrder, String(NULL), &outerQuery);

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);

    AutoPtr<ICursor> cc;
    db->RawQuery(outerQuery, EMPTY_STRING_ARRAY, (ICursor**)&cc);
    return cc;
}

/**
 * Return the most recent message in each conversation in both MMS
 * and SMS.
 *
 * Use this query:
 *
 *   SELECT ...
 *     FROM (SELECT thread_id AS tid, date * 1000 AS normalized_date, ...
 *             FROM pdu
 *             WHERE msg_box != 3 AND ...
 *             GROUP BY thread_id
 *             HAVING date = MAX(date)
 *           UNION
 *           SELECT thread_id AS tid, date AS normalized_date, ...
 *             FROM sms
 *             WHERE ...
 *             GROUP BY thread_id
 *             HAVING date = MAX(date))
 *     GROUP BY tid
 *     HAVING normalized_date = MAX(normalized_date);
 *
 * The msg_box != 3 comparisons ensure that we don't include draft
 * messages.
 */
AutoPtr<ICursor> CMmsSmsProvider::GetConversations(
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ const String& sortOrder)
{
    AutoPtr<ISQLiteQueryBuilder> mmsQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&mmsQueryBuilder);
    AutoPtr<ISQLiteQueryBuilder> smsQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&smsQueryBuilder);

    mmsQueryBuilder->SetTables(IMmsProvider::TABLE_PDU);
    smsQueryBuilder->SetTables(CSmsProvider::TABLE_SMS);

    AutoPtr<ArrayOf<String> > columns = HandleNullMessageProjection(projection);
    AutoPtr<ArrayOf<String> > innerMmsProjection = MakeProjectionWithDateAndThreadId(
            UNION_COLUMNS, 1000);
    AutoPtr<ArrayOf<String> > innerSmsProjection = MakeProjectionWithDateAndThreadId(
            UNION_COLUMNS, 1);
    String mmsSubQuery;
    mmsQueryBuilder->BuildUnionSubQuery(
            ITelephonyMmsSms::TYPE_DISCRIMINATOR_COLUMN, *innerMmsProjection,
            MMS_COLUMNS, 1, String("mms"),
            ConcatSelections(selection, MMS_CONVERSATION_CONSTRAINT),
            String("thread_id"), String("date = MAX(date)"), &mmsSubQuery);
    String smsSubQuery;
    smsQueryBuilder->BuildUnionSubQuery(
            ITelephonyMmsSms::TYPE_DISCRIMINATOR_COLUMN, *innerSmsProjection,
            SMS_COLUMNS, 1, String("sms"),
            ConcatSelections(selection, SMS_CONVERSATION_CONSTRAINT),
            String("thread_id"), String("date = MAX(date)"), &smsSubQuery);
    AutoPtr<ISQLiteQueryBuilder> unionQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&unionQueryBuilder);

    unionQueryBuilder->SetDistinct(TRUE);

    String unionQuery;
    AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(2);
    (*ss)[0] = mmsSubQuery;
    (*ss)[1] = smsSubQuery;
    unionQueryBuilder->BuildUnionQuery(*ss, String(NULL), String(NULL), &unionQuery);

    AutoPtr<ISQLiteQueryBuilder> outerQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&outerQueryBuilder);

    outerQueryBuilder->SetTables(String("(") + unionQuery + ")");

    String outerQuery;
    outerQueryBuilder->BuildQuery(columns, String(NULL), String("tid"),
            String("normalized_date = MAX(normalized_date)"), sortOrder, String(NULL), &outerQuery);

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<ICursor> cc;
    db->RawQuery(outerQuery, EMPTY_STRING_ARRAY, (ICursor**)&cc);
    return cc;
}

/**
 * Return the first locked message found in the union of MMS
 * and SMS messages.
 *
 * Use this query:
 *
 *  SELECT _id FROM pdu GROUP BY _id HAVING locked=1 UNION SELECT _id FROM sms GROUP
 *      BY _id HAVING locked=1 LIMIT 1
 *
 * We limit by 1 because we're only interested in knowing if
 * there is *any* locked message, not the actual messages themselves.
 */
AutoPtr<ICursor> CMmsSmsProvider::GetFirstLockedMessage(
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ const String& sortOrder)
{
    AutoPtr<ISQLiteQueryBuilder> mmsQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&mmsQueryBuilder);
    AutoPtr<ISQLiteQueryBuilder> smsQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&smsQueryBuilder);

    mmsQueryBuilder->SetTables(IMmsProvider::TABLE_PDU);
    smsQueryBuilder->SetTables(CSmsProvider::TABLE_SMS);

    AutoPtr<ArrayOf<String> > idColumn = ArrayOf<String>::Alloc(1);
    (*idColumn)[0] = IBaseColumns::ID;

    // NOTE: buildUnionSubQuery *ignores* selectionArgs
    String mmsSubQuery;
    mmsQueryBuilder->BuildUnionSubQuery(
            ITelephonyMmsSms::TYPE_DISCRIMINATOR_COLUMN, *idColumn,
            NULL, 1, String("mms"),
            selection,
            IBaseColumns::ID, String("locked=1"), &mmsSubQuery);

    String smsSubQuery;
    smsQueryBuilder->BuildUnionSubQuery(
            ITelephonyMmsSms::TYPE_DISCRIMINATOR_COLUMN, *idColumn,
            NULL, 1, String("sms"),
            selection,
            IBaseColumns::ID, String("locked=1"), &smsSubQuery);

    AutoPtr<ISQLiteQueryBuilder> unionQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&unionQueryBuilder);

    unionQueryBuilder->SetDistinct(TRUE);

    AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(2);
    (*ss)[0] = mmsSubQuery;
    (*ss)[1] = smsSubQuery;
    String unionQuery;
    unionQueryBuilder->BuildUnionQuery(*ss, String(NULL), String("1"), &unionQuery);

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<ICursor> cursor;
    db->RawQuery(unionQuery, EMPTY_STRING_ARRAY, (ICursor**)&cursor);

    if (DEBUG) {
        Logger::V("MmsSmsProvider", "GetFirstLockedMessage query: %s", unionQuery.string());
        Int32 count = 0;
        cursor->GetCount(&count);
        Logger::V("MmsSmsProvider", "cursor count: %d", count);
    }
    return cursor;
}

/**
 * Return every message in each conversation in both MMS
 * and SMS.
 */
AutoPtr<ICursor> CMmsSmsProvider::GetCompleteConversations(
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ const String& sortOrder)
{
    String unionQuery = BuildConversationQuery(projection, selection, sortOrder, FALSE);

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<ICursor> cc;
    db->RawQuery(unionQuery, EMPTY_STRING_ARRAY, (ICursor**)&cc);
    return cc;
}

/**
 * Add normalized date and thread_id to the list of columns for an
 * inner projection.  This is necessary so that the outer query
 * can have access to these columns even if the caller hasn't
 * requested them in the result.
 */
AutoPtr<ArrayOf<String> > CMmsSmsProvider::MakeProjectionWithDateAndThreadId(
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ Int32 dateMultiple)
{
    Int32 projectionSize = projection->GetLength();
    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(projectionSize + 2);

    (*result)[0] = String("thread_id AS tid");
    (*result)[1] = String("date * ") + StringUtils::ToString(dateMultiple) + " AS normalized_date";
    for (Int32 i = 0; i < projectionSize; i++) {
        (*result)[i + 2] = (*projection)[i];
    }
    return result;
}

/**
 * Return the union of MMS and SMS messages for this thread ID.
 */
AutoPtr<ICursor> CMmsSmsProvider::GetConversationMessages(
    /* [in] */ const String& threadIdString,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ const String& sortOrder,
    /* [in] */ Boolean typeCountOnly)
{
    // try {
        StringUtils::ParseInt64(threadIdString);
    // } catch (NumberFormatException exception) {
    //     Logger::E(TAG, "Thread ID must be a Long.");
    //     return NULL;
    // }

    String finalSelection = ConcatSelections(
            selection, String("thread_id = ") + threadIdString);
    String unionQuery = BuildConversationQuery(projection, finalSelection, sortOrder, typeCountOnly);

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<ICursor> cc;
    db->RawQuery(unionQuery, EMPTY_STRING_ARRAY, (ICursor**)&cc);
    return cc;
}

/**
 * Return the union of MMS and SMS messages in one mailbox.
 */
AutoPtr<ICursor> CMmsSmsProvider::GetMailboxMessages(
    /* [in] */ const String& mailboxId,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [in] */ Boolean read)
{
    // try {
        StringUtils::ParseInt32(mailboxId);
    // } catch (NumberFormatException exception) {
    //     Logger::E(TAG, "mailboxId must be a Long.");
    //     return NULL;
    // }
    String unionQuery = BuildMailboxMsgQuery(mailboxId, projection,
            selection, selectionArgs, sortOrder, read);

    if (DEBUG) {
        Logger::W(TAG, "GetMailboxMessages : unionQuery =%s", unionQuery.string());
    }

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<ICursor> cc;
    db->RawQuery(unionQuery, EMPTY_STRING_ARRAY, (ICursor**)&cc);
    return cc;
}

String CMmsSmsProvider::BuildMailboxMsgQuery(
    /* [in] */ const String& mailboxId,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [in] */ Boolean read)
{
    AutoPtr<ArrayOf<String> > mmsProjection = CreateMmsMailboxProjection(projection);
    AutoPtr<ArrayOf<String> > smsProjection = CreateSmsMailboxProjection(projection);

    AutoPtr<ISQLiteQueryBuilder> mmsQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&mmsQueryBuilder);
    AutoPtr<ISQLiteQueryBuilder> smsQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&smsQueryBuilder);

    mmsQueryBuilder->SetDistinct(TRUE);
    smsQueryBuilder->SetDistinct(TRUE);
    mmsQueryBuilder->SetTables(String("threads, ") + JoinPduAndPendingMsgTables());
    smsQueryBuilder->SetTables(CSmsProvider::TABLE_SMS + ", threads ");

    AutoPtr<ArrayOf<String> > smsColumns = HandleNullMessageProjection(smsProjection);
    AutoPtr<ArrayOf<String> > mmsColumns = HandleNullMessageProjection(mmsProjection);
    AutoPtr<ArrayOf<String> > innerMmsProjection = MakeMmsProjectionWithNormalizedDate(
            mmsColumns, 1000);
    AutoPtr<ArrayOf<String> > innerSmsProjection = MakeSmsProjectionWithNormalizedDate(
            smsColumns, 1);

    AutoPtr<ISet> columnsPresentInTable;/*<String*/
    CHashSet::New(ICollection::Probe(MMS_COLUMNS), (ISet**)&columnsPresentInTable);
    columnsPresentInTable->Add(CoreUtils::Convert(String("pdu._id AS _id")));
    columnsPresentInTable->Add(CoreUtils::Convert(String("pdu.date AS date")));
    columnsPresentInTable->Add(CoreUtils::Convert(String("pdu.read AS read")));
    columnsPresentInTable->Add(CoreUtils::Convert(String("pdu.sub_id AS sub_id")));
    columnsPresentInTable->Add(CoreUtils::Convert(String("recipient_ids")));

    columnsPresentInTable->Add(CoreUtils::Convert(ITelephonyMmsSmsPendingMessages::ERROR_TYPE));
    String compare(" = ");
    Int32 boxidInt = StringUtils::ParseInt32(mailboxId);
    if (boxidInt >= 4) {
        compare = String(" >= ");
    }

    String smsSelection;
    String mmsSelection;

    if (!TextUtils::IsEmpty(selection)) {
        mmsSelection = ITelephonyBaseMmsColumns::MESSAGE_BOX + compare + mailboxId
                + " AND thread_id = threads._id AND " + selection;
        smsSelection = String("(sms.") + ITelephonyTextBasedSmsColumns::TYPE + compare + mailboxId
                + " AND thread_id = threads._id" + " AND " + selection
                + ")" + " OR (sms." + ITelephonyTextBasedSmsColumns::TYPE + compare + mailboxId
                + " AND thread_id ISNULL" + " AND " + selection + ")";
    }
    else {
        mmsSelection = ITelephonyBaseMmsColumns::MESSAGE_BOX + compare + mailboxId
                + " AND thread_id = threads._id";
        smsSelection = String("(sms.") + ITelephonyTextBasedSmsColumns::TYPE + compare + mailboxId
                + " AND thread_id = threads._id" + ") OR (sms." + ITelephonyTextBasedSmsColumns::TYPE
                + compare + mailboxId + " AND thread_id ISNULL" + ")";
    }

    String mmsSubQuery;
    mmsQueryBuilder->BuildUnionSubQuery(
            ITelephonyMmsSms::TYPE_DISCRIMINATOR_COLUMN, *innerMmsProjection,
            columnsPresentInTable, 0, String("mms"), mmsSelection, *selectionArgs,
            String(NULL), String(NULL), &mmsSubQuery);

    AutoPtr<ISet> columnsPresentInSmsTable;/*<String*/
    CHashSet::New(ICollection::Probe(SMS_COLUMNS), (ISet**)&columnsPresentInSmsTable);
    columnsPresentInSmsTable->Add(CoreUtils::Convert(String("sms._id AS _id")));
    columnsPresentInSmsTable->Add(CoreUtils::Convert(String("sms.date AS date")));
    columnsPresentInSmsTable->Add(CoreUtils::Convert(String("sms.read AS read")));
    columnsPresentInSmsTable->Add(CoreUtils::Convert(String("sms.type AS type")));
    columnsPresentInSmsTable->Add(CoreUtils::Convert(String("sms.sub_id AS sub_id")));
    columnsPresentInSmsTable->Add(CoreUtils::Convert(String("recipient_ids")));

    String smsSubQuery;
    smsQueryBuilder->BuildUnionSubQuery(
            ITelephonyMmsSms::TYPE_DISCRIMINATOR_COLUMN, *innerSmsProjection,
            columnsPresentInSmsTable, 0, String("sms"), smsSelection,
            *selectionArgs, String(NULL), String(NULL), &smsSubQuery);

    AutoPtr<ISQLiteQueryBuilder> unionQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&unionQueryBuilder);
    String unionQuery;
    AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(2);
    (*ss)[0] = mmsSubQuery;
    (*ss)[1] = smsSubQuery;
    unionQueryBuilder->BuildUnionQuery(*ss, String(NULL), String(NULL), &unionQuery);
    if (DEBUG) {
        Logger::W(TAG, "BuildMailboxMsgQuery : unionQuery = %s", unionQuery.string());
    }

    AutoPtr<ISQLiteQueryBuilder> outerQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&outerQueryBuilder);
    outerQueryBuilder->SetTables(String("(") + unionQuery + ")");

    String v;
    outerQueryBuilder->BuildQuery(projection, String(NULL), NULL, String(NULL), String(NULL),
            sortOrder, String(NULL), &v);
    return v;
}

/**
 * Return the union of MMS and SMS messages whose recipients
 * included this phone number.
 *
 * Use this query:
 *
 * SELECT ...
 *   FROM pdu, (SELECT msg_id AS address_msg_id
 *              FROM addr
 *              WHERE (address='<phoneNumber>' OR
 *              PHONE_NUMBERS_EQUAL(addr.address, '<phoneNumber>', 1/0)))
 *             AS matching_addresses
 *   WHERE pdu._id = matching_addresses.address_msg_id
 * UNION
 * SELECT ...
 *   FROM sms
 *   WHERE (address='<phoneNumber>' OR PHONE_NUMBERS_EQUAL(sms.address, '<phoneNumber>', 1/0));
 */
AutoPtr<ICursor> CMmsSmsProvider::GetMessagesByPhoneNumber(
    /* [in] */ const String& phoneNumber,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ const String& sortOrder)
{
    AutoPtr<IDatabaseUtils> du;
    CDatabaseUtils::AcquireSingleton((IDatabaseUtils**)&du);
    String escapedPhoneNumber;
    du->SqlEscapeString(phoneNumber, &escapedPhoneNumber);
    String finalMmsSelection = ConcatSelections(selection, String("pdu._id = matching_addresses.address_msg_id"));
    String finalSmsSelection = ConcatSelections(selection,
                    String("(address=") + escapedPhoneNumber + " OR PHONE_NUMBERS_EQUAL(address, " +
                    escapedPhoneNumber + (mUseStrictPhoneNumberComparation ? ", 1))" : ", 0))"));
    AutoPtr<ISQLiteQueryBuilder> mmsQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&mmsQueryBuilder);
    AutoPtr<ISQLiteQueryBuilder> smsQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&smsQueryBuilder);

    mmsQueryBuilder->SetDistinct(TRUE);
    smsQueryBuilder->SetDistinct(TRUE);
    mmsQueryBuilder->SetTables(
            IMmsProvider::TABLE_PDU +
            ", (SELECT msg_id AS address_msg_id " +
            "FROM addr WHERE (address=" + escapedPhoneNumber +
            " OR PHONE_NUMBERS_EQUAL(addr.address, " +
            escapedPhoneNumber +
            (mUseStrictPhoneNumberComparation ? ", 1))) " : ", 0))) ") +
            "AS matching_addresses");
    smsQueryBuilder->SetTables(CSmsProvider::TABLE_SMS);

    AutoPtr<ArrayOf<String> > columns = HandleNullMessageProjection(projection);
    String mmsSubQuery;
    mmsQueryBuilder->BuildUnionSubQuery(
            ITelephonyMmsSms::TYPE_DISCRIMINATOR_COLUMN, *columns, MMS_COLUMNS,
            0, String("mms"), finalMmsSelection, String(NULL), String(NULL), &mmsSubQuery);
    String smsSubQuery;
    smsQueryBuilder->BuildUnionSubQuery(
            ITelephonyMmsSms::TYPE_DISCRIMINATOR_COLUMN, *columns, SMS_COLUMNS,
            0, String("sms"), finalSmsSelection, String(NULL), String(NULL), &smsSubQuery);
    AutoPtr<ISQLiteQueryBuilder> unionQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&unionQueryBuilder);

    unionQueryBuilder->SetDistinct(TRUE);

    AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(2);
    (*ss)[0] = mmsSubQuery;
    (*ss)[1] = smsSubQuery;
    String unionQuery;
    unionQueryBuilder->BuildUnionQuery(*ss, sortOrder, String(NULL), &unionQuery);

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<ICursor> cc;
    db->RawQuery(unionQuery, EMPTY_STRING_ARRAY, (ICursor**)&cc);
    return cc;
}

/**
 * Return the conversation of certain thread ID.
 */
AutoPtr<ICursor> CMmsSmsProvider::GetConversationById(
    /* [in] */ const String& threadIdString,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder)
{
    // try {
        StringUtils::ParseInt64(threadIdString);
        Logger::D(TAG, "Thread ID must be a Long: %s", threadIdString.string());
    // } catch (NumberFormatException exception) {
    //     Logger::E(TAG, "Thread ID must be a Long.");
    //     return NULL;
    // }

    String extraSelection = String("_id=") + threadIdString;
    String finalSelection = ConcatSelections(selection, extraSelection);
    AutoPtr<ISQLiteQueryBuilder> queryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&queryBuilder);
    AutoPtr<ArrayOf<String> > columns = HandleNullThreadsProjection(projection);

    queryBuilder->SetDistinct(TRUE);
    queryBuilder->SetTables(TABLE_THREADS);

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<ICursor> c;
    queryBuilder->Query(db, columns, finalSelection,
            selectionArgs, sortOrder, String(NULL), String(NULL), (ICursor**)&c);
    return c;
}

String CMmsSmsProvider::JoinPduAndPendingMsgTables()
{
    return IMmsProvider::TABLE_PDU + " LEFT JOIN " + TABLE_PENDING_MSG
            + " ON pdu._id = pending_msgs.msg_id";
}

AutoPtr<ArrayOf<String> > CMmsSmsProvider::CreateMmsProjection(
    /* [in] */ ArrayOf<String>* old)
{
    Int32 len = old->GetLength();
    AutoPtr<ArrayOf<String> > newProjection = ArrayOf<String>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        if ((*old)[i].Equals(IBaseColumns::ID)) {
            (*newProjection)[i] = "pdu._id";
        }
        else {
            (*newProjection)[i] = (*old)[i];
        }
    }
    return newProjection;
}

AutoPtr<ArrayOf<String> > CMmsSmsProvider::CreateMmsMailboxProjection(
    /* [in] */ ArrayOf<String>* old)
{
    if (old == NULL) {
        return NULL;
    }

    Int32 length = old->GetLength();
    AutoPtr<ArrayOf<String> > newProjection = ArrayOf<String>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        if ((*old)[i].Equals(IBaseColumns::ID)) {
            (*newProjection)[i] = String("pdu._id AS _id");
        }
        else if ((*old)[i].Equals("date")) {
            (*newProjection)[i] = String("pdu.date AS date");
        }
        else if ((*old)[i].Equals("read")) {
            (*newProjection)[i] = String("pdu.read AS read");
        }
        else if ((*old)[i].Equals("sub_id")) {
            (*newProjection)[i] = String("pdu.sub_id AS sub_id");
        }
        else {
            (*newProjection)[i] = (*old)[i];
        }
    }
    return newProjection;
}

AutoPtr<ArrayOf<String> > CMmsSmsProvider::CreateSmsMailboxProjection(
    /* [in] */ ArrayOf<String>* old)
{
    if (old == NULL) {
        return NULL;
    }

    Int32 length = old->GetLength();
    AutoPtr<ArrayOf<String> > newProjection = ArrayOf<String>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        if ((*old)[i].Equals(IBaseColumns::ID)) {
            (*newProjection)[i] = String("sms._id AS _id");
        }
        else if ((*old)[i].Equals("date")) {
            (*newProjection)[i] = String("sms.date AS date");
        }
        else if ((*old)[i].Equals("read")) {
            (*newProjection)[i] = String("sms.read AS read");
        }
        else if ((*old)[i].Equals("type")) {
            (*newProjection)[i] = String("sms.type AS type");
        }
        else if ((*old)[i].Equals("sub_id")) {
            (*newProjection)[i] = String("sms.sub_id AS sub_id");
        }
        else {
            (*newProjection)[i] = (*old)[i];
        }
    }
    return newProjection;
}

AutoPtr<ICursor> CMmsSmsProvider::GetUndeliveredMessages(
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder)
{
    AutoPtr<ArrayOf<String> > mmsProjection = CreateMmsProjection(projection);

    AutoPtr<ISQLiteQueryBuilder> mmsQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&mmsQueryBuilder);
    AutoPtr<ISQLiteQueryBuilder> smsQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&smsQueryBuilder);

    mmsQueryBuilder->SetTables(JoinPduAndPendingMsgTables());
    smsQueryBuilder->SetTables(CSmsProvider::TABLE_SMS);

    String finalMmsSelection = ConcatSelections(
            selection, ITelephonyBaseMmsColumns::MESSAGE_BOX + " = "
            + StringUtils::ToString(ITelephonyBaseMmsColumns::MESSAGE_BOX_OUTBOX));
    String finalSmsSelection = ConcatSelections(
            selection, String("(") + ITelephonyTextBasedSmsColumns::TYPE + " = "
            + StringUtils::ToString(ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_OUTBOX)
            + " OR " + ITelephonyTextBasedSmsColumns::TYPE + " = "
            + StringUtils::ToString(ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_FAILED)
            + " OR " + ITelephonyTextBasedSmsColumns::TYPE + " = "
            + StringUtils::ToString(ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_QUEUED) + ")");

    AutoPtr<ArrayOf<String> > smsColumns = HandleNullMessageProjection(projection);
    AutoPtr<ArrayOf<String> > mmsColumns = HandleNullMessageProjection(mmsProjection);
    AutoPtr<ArrayOf<String> > innerMmsProjection = MakeProjectionWithDateAndThreadId(
            mmsColumns, 1000);
    AutoPtr<ArrayOf<String> > innerSmsProjection = MakeProjectionWithDateAndThreadId(
            smsColumns, 1);

    AutoPtr<ISet> columnsPresentInTable;/*<String*/
    CHashSet::New(ICollection::Probe(MMS_COLUMNS), (ISet**)&columnsPresentInTable);
    columnsPresentInTable->Add(CoreUtils::Convert(String("pdu._id")));
    columnsPresentInTable->Add(CoreUtils::Convert(ITelephonyMmsSmsPendingMessages::ERROR_TYPE));
    String mmsSubQuery;
    mmsQueryBuilder->BuildUnionSubQuery(
            ITelephonyMmsSms::TYPE_DISCRIMINATOR_COLUMN, *innerMmsProjection,
            columnsPresentInTable, 1, String("mms"), finalMmsSelection,
            String(NULL), String(NULL), &mmsSubQuery);
    String smsSubQuery;
    smsQueryBuilder->BuildUnionSubQuery(
            ITelephonyMmsSms::TYPE_DISCRIMINATOR_COLUMN, *innerSmsProjection,
            SMS_COLUMNS, 1, String("sms"), finalSmsSelection,
            String(NULL), String(NULL), &smsSubQuery);
    AutoPtr<ISQLiteQueryBuilder> unionQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&unionQueryBuilder);

    unionQueryBuilder->SetDistinct(TRUE);

    String unionQuery;
    AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(2);
    (*ss)[0] = smsSubQuery;
    (*ss)[1] = mmsSubQuery;
    unionQueryBuilder->BuildUnionQuery(*ss, String(NULL), String(NULL), &unionQuery);

    AutoPtr<ISQLiteQueryBuilder> outerQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&outerQueryBuilder);

    outerQueryBuilder->SetTables(String("(") + unionQuery + ")");

    String outerQuery;
    outerQueryBuilder->BuildQuery(smsColumns, String(NULL), String(NULL)
        , String(NULL), sortOrder, String(NULL), &outerQuery);

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<ICursor> cc;
    db->RawQuery(outerQuery, EMPTY_STRING_ARRAY, (ICursor**)&cc);
    return cc;
}

/**
 * Add normalized date to the list of columns for an inner
 * projection.
 */
AutoPtr<ArrayOf<String> > CMmsSmsProvider::MakeProjectionWithNormalizedDate(
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ Int32 dateMultiple)
{
    Int32 projectionSize = projection->GetLength();
    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(projectionSize + 1);

    (*result)[0] = String("date * ") + StringUtils::ToString(dateMultiple) + " AS normalized_date";
    // System.arraycopy(projection, 0, result, 1, projectionSize);
    result->Copy(1, projection, 0, projectionSize);
    return result;
}

/**
 * Add normalized date to the list of columns for an inner
 * projection.
 */
AutoPtr<ArrayOf<String> > CMmsSmsProvider::MakeSmsProjectionWithNormalizedDate(
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ Int32 dateMultiple)
{
    Int32 projectionSize = projection->GetLength();
    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(projectionSize + 1);

    (*result)[0] = String("sms.date * ") + StringUtils::ToString(dateMultiple) + " AS normalized_date";
    // System.arraycopy(projection, 0, result, 1, projectionSize);
    result->Copy(1, projection, 0, projectionSize);
    return result;
}

AutoPtr<ArrayOf<String> > CMmsSmsProvider::MakeMmsProjectionWithNormalizedDate(
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ Int32 dateMultiple)
{
    Int32 projectionSize = projection->GetLength();
    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(projectionSize + 1);

    (*result)[0] = String("pdu.date * ") + StringUtils::ToString(dateMultiple) + " AS normalized_date";
    // System.arraycopy(projection, 0, result, 1, projectionSize);
    result->Copy(1, projection, 0, projectionSize);
    return result;
}

String CMmsSmsProvider::BuildConversationQuery(
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ const String& sortOrder,
    /* [in] */ Boolean typeCountOnly)
{
    AutoPtr<ArrayOf<String> > mmsProjection = CreateMmsProjection(projection);

    AutoPtr<ISQLiteQueryBuilder> mmsQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&mmsQueryBuilder);
    AutoPtr<ISQLiteQueryBuilder> smsQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&smsQueryBuilder);

    mmsQueryBuilder->SetDistinct(TRUE);
    smsQueryBuilder->SetDistinct(TRUE);
    mmsQueryBuilder->SetTables(JoinPduAndPendingMsgTables());
    smsQueryBuilder->SetTables(CSmsProvider::TABLE_SMS);

    AutoPtr<ArrayOf<String> > smsColumns = HandleNullMessageProjection(projection);
    AutoPtr<ArrayOf<String> > mmsColumns = HandleNullMessageProjection(mmsProjection);
    AutoPtr<ArrayOf<String> > innerMmsProjection = MakeProjectionWithNormalizedDate(mmsColumns, 1000);
    AutoPtr<ArrayOf<String> > innerSmsProjection = MakeProjectionWithNormalizedDate(smsColumns, 1);

    AutoPtr<ISet> columnsPresentInTable;/*<String*/
    CHashSet::New(ICollection::Probe(MMS_COLUMNS), (ISet**)&columnsPresentInTable);
    columnsPresentInTable->Add(CoreUtils::Convert(String("pdu._id")));
    columnsPresentInTable->Add(CoreUtils::Convert(ITelephonyMmsSmsPendingMessages::ERROR_TYPE));

    String mmsSelection = ConcatSelections(selection,
                            ITelephonyBaseMmsColumns::MESSAGE_BOX + " != "
                            + StringUtils::ToString(ITelephonyBaseMmsColumns::MESSAGE_BOX_DRAFTS));
    String mmsSubQuery;
    mmsQueryBuilder->BuildUnionSubQuery(
            ITelephonyMmsSms::TYPE_DISCRIMINATOR_COLUMN, *innerMmsProjection,
            columnsPresentInTable, 0, String("mms"),
            ConcatSelections(mmsSelection, MMS_CONVERSATION_CONSTRAINT),
            String(NULL), String(NULL), &mmsSubQuery);
    String smsSubQuery;
    smsQueryBuilder->BuildUnionSubQuery(
            ITelephonyMmsSms::TYPE_DISCRIMINATOR_COLUMN, *innerSmsProjection, SMS_COLUMNS,
            0, String("sms"), ConcatSelections(selection, SMS_CONVERSATION_CONSTRAINT),
            String(NULL), String(NULL), &smsSubQuery);
    AutoPtr<ISQLiteQueryBuilder> unionQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&unionQueryBuilder);

    unionQueryBuilder->SetDistinct(TRUE);

    String unionQuery;
    AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(2);
    (*ss)[0] = smsSubQuery;
    (*ss)[1] = mmsSubQuery;
    unionQueryBuilder->BuildUnionQuery(*ss, HandleNullSortOrder(sortOrder), String(NULL), &unionQuery);

    AutoPtr<ISQLiteQueryBuilder> outerQueryBuilder;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&outerQueryBuilder);

    outerQueryBuilder->SetTables(String("(") + unionQuery + ")");

    String v;
    outerQueryBuilder->BuildQuery(
            typeCountOnly ? TYPE_COUNT_PROJECTION.Get() : projection, String(NULL),
            typeCountOnly ? ITelephonyMmsSms::TYPE_DISCRIMINATOR_COLUMN : String(NULL)
            , String(NULL), sortOrder, String(NULL), &v);
    return v;
}

// @Override
ECode CMmsSmsProvider::GetType(
    /* [in] */ IUri* uri,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = VND_ANDROID_DIR_MMS_SMS;
    return NOERROR;
}

// @Override
ECode CMmsSmsProvider::Delete(
    /* [in] */ IUri* uri,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    Int32 affectedRows = 0;

    Int32 match = 0;
    switch(URI_MATCHER->Match(uri, &match), match) {
        case URI_CONVERSATIONS_MESSAGES: {
            Int64 threadId = 0;
            // try {
            String v;
            uri->GetLastPathSegment(&v);
            threadId = StringUtils::ParseInt64(v);
            // } catch (NumberFormatException e) {
            //     Logger::E(TAG, "Thread ID must be a Int64.");
            //     break;
            // }
            affectedRows = DeleteConversation(uri, selection, selectionArgs);
            MmsSmsDatabaseHelper::UpdateThread(db, threadId);
            break;
        }
        case URI_CONVERSATIONS: {
            Int32 v = 0;
            db->Delete(String("sms"), selection, selectionArgs, &v);
            affectedRows = CMmsProvider::DeleteMessages(context, db, selection, selectionArgs, uri) + v;
            // Intentionally don't pass the selection variable to updateAllThreads.
            // When we pass in "locked=0" there, the thread will get excluded from
            // the selection and not get updated.
            MmsSmsDatabaseHelper::UpdateAllThreads(db, String(NULL), NULL);
            break;
        }
        case URI_OBSOLETE_THREADS: {
            db->Delete(TABLE_THREADS,
                    String("_id NOT IN (SELECT DISTINCT thread_id FROM sms where thread_id NOT NULL ") +
                    "UNION SELECT DISTINCT thread_id FROM pdu where thread_id NOT NULL)", NULL, &affectedRows);
            break;
        }
        default: {
            // throw new UnsupportedOperationException(NO_DELETES_INSERTS_OR_UPDATES + uri);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }
    }

    if (affectedRows > 0) {
        AutoPtr<IContentResolver> cr;
        context->GetContentResolver((IContentResolver**)&cr);
        cr->NotifyChange(Elastos::Droid::Provider::Telephony::MmsSms::CONTENT_URI, NULL, TRUE,
                IUserHandle::USER_ALL);
    }
    *result = affectedRows;
    return NOERROR;
}

/**
 * Delete the conversation with the given thread ID.
 */
Int32 CMmsSmsProvider::DeleteConversation(
    /* [in] */ IUri* uri,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs)
{
    String threadId;
    uri->GetLastPathSegment(&threadId);

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    String finalSelection = ConcatSelections(selection, String("thread_id = ") + threadId);
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    Int32 v = 0;
    db->Delete(String("sms"), finalSelection, selectionArgs, &v);
    return CMmsProvider::DeleteMessages(ctx, db, finalSelection, selectionArgs, uri) + v;
}

// @Override
ECode CMmsSmsProvider::Insert(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    Int32 match = 0;
    if ((URI_MATCHER->Match(uri, &match), match) == URI_PENDING_MSG) {
        AutoPtr<ISQLiteDatabase> db;
        mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
        Int64 rowId;
        db->Insert(TABLE_PENDING_MSG, String(NULL), values, &rowId);
        AutoPtr<IUriHelper> uh;
        CUriHelper::AcquireSingleton((IUriHelper**)&uh);
        return uh->Parse(TO_STR(uri) + "/" + StringUtils::ToString(rowId), result);
    }
    // throw new UnsupportedOperationException(NO_DELETES_INSERTS_OR_UPDATES + uri);
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

// @Override
ECode CMmsSmsProvider::Update(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String> * selectionArgs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    Int32 affectedRows = 0, match = 0;
    switch(URI_MATCHER->Match(uri, &match), match) {
        case URI_CONVERSATIONS_MESSAGES: {
            AutoPtr<IList> l;
            uri->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(1, (IInterface**)&obj);
            String threadIdString = TO_STR(obj);
            affectedRows = UpdateConversation(threadIdString, values,
                    selection, selectionArgs);
            break;
        }

        case URI_PENDING_MSG:
            db->Update(TABLE_PENDING_MSG, values, selection, NULL, &affectedRows);
            break;

        case URI_CANONICAL_ADDRESS: {
            AutoPtr<IList> l;
            uri->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(1, (IInterface**)&obj);
            String extraSelection = String("_id=") + TO_STR(obj);
            String finalSelection = TextUtils::IsEmpty(selection)
                    ? extraSelection : extraSelection + " AND " + selection;

            db->Update(TABLE_CANONICAL_ADDRESSES, values, finalSelection, NULL, &affectedRows);
            break;
        }

        case URI_CONVERSATIONS: {
            AutoPtr<IContentValues> finalValues;
            CContentValues::New(1, (IContentValues**)&finalValues);
            Boolean tmp = FALSE;
            if (values->ContainsKey(ITelephonyThreadsColumns::ARCHIVED, &tmp), tmp) {
                // Only allow update archived
                AutoPtr<IBoolean> bo;
                values->GetAsBoolean(ITelephonyThreadsColumns::ARCHIVED, (IBoolean**)&bo);
                finalValues->Put(ITelephonyThreadsColumns::ARCHIVED, bo);
            }
            db->Update(TABLE_THREADS, finalValues, selection, selectionArgs, &affectedRows);
            break;
        }

        default: {
            // throw new UnsupportedOperationException(NO_DELETES_INSERTS_OR_UPDATES + uri);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }
    }

    if (affectedRows > 0) {
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        AutoPtr<IContentResolver> cr;
        ctx->GetContentResolver((IContentResolver**)&cr);
        cr->NotifyChange(Elastos::Droid::Provider::Telephony::MmsSms::CONTENT_URI
                , NULL, TRUE, IUserHandle::USER_ALL);
    }
    *result = affectedRows;
    return NOERROR;
}

Int32 CMmsSmsProvider::UpdateConversation(
    /* [in] */ const String& threadIdString,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs)
{
    // try {
        StringUtils::ParseInt64(threadIdString);
        Logger::D(TAG, "Thread ID must be a Long.: %s", threadIdString.string());
    // } catch (NumberFormatException exception) {
    //     Logger::E(TAG, "Thread ID must be a Long.");
    //     return 0;
    // }

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    String finalSelection = ConcatSelections(selection, String("thread_id=") + threadIdString);
    Int32 u1 = 0, u2 = 0;
    db->Update(IMmsProvider::TABLE_PDU, values, finalSelection, selectionArgs, &u1);
    db->Update(String("sms"), values, finalSelection, selectionArgs, &u2);
    return u1 + u2;
}

/**
 * Construct Sets of Strings containing exactly the columns
 * present in each table.  We will use this when constructing
 * UNION queries across the MMS and SMS tables.
 */
void CMmsSmsProvider::InitializeColumnSets()
{
    Int32 commonColumnCount = MMS_SMS_COLUMNS->GetLength();
    Int32 mmsOnlyColumnCount = MMS_ONLY_COLUMNS->GetLength();
    Int32 smsOnlyColumnCount = SMS_ONLY_COLUMNS->GetLength();
    AutoPtr<ISet> unionColumns;/*<String*/
    CHashSet::New((ISet**)&unionColumns);

    for (Int32 i = 0; i < commonColumnCount; i++) {
        MMS_COLUMNS->Add(CoreUtils::Convert((*MMS_SMS_COLUMNS)[i]));
        SMS_COLUMNS->Add(CoreUtils::Convert((*MMS_SMS_COLUMNS)[i]));
        unionColumns->Add(CoreUtils::Convert((*MMS_SMS_COLUMNS)[i]));
    }
    for (Int32 i = 0; i < mmsOnlyColumnCount; i++) {
        MMS_COLUMNS->Add(CoreUtils::Convert((*MMS_ONLY_COLUMNS)[i]));
        unionColumns->Add(CoreUtils::Convert((*MMS_ONLY_COLUMNS)[i]));
    }
    for (Int32 i = 0; i < smsOnlyColumnCount; i++) {
        SMS_COLUMNS->Add(CoreUtils::Convert((*SMS_ONLY_COLUMNS)[i]));
        unionColumns->Add(CoreUtils::Convert((*SMS_ONLY_COLUMNS)[i]));
    }

    Int32 i = 0;
    AutoPtr<IIterator> ator;
    unionColumns->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> columnName;
        ator->GetNext((IInterface**)&columnName);
        (*UNION_COLUMNS)[i++] = TO_STR(columnName);
    }
}

AutoPtr<ICursor> CMmsSmsProvider::GetSearchMessages(
    /* [in] */ IUri* uri,
    /* [in] */ ISQLiteDatabase* db)
{
    String v;
    uri->GetQueryParameter(String("search_mode"), &v);
    Int32 searchMode = StringUtils::ParseInt32(v);
    String keyStr;
    uri->GetQueryParameter(String("key_str"), &keyStr);
    uri->GetQueryParameter(String("match_whole"), &v);
    Int32 matchWhole = StringUtils::ParseInt32(v);
    Logger::D(TAG, "GetSearchMessages : searchMode =%d,keyStr=%s,matchWhole=%d"
            , searchMode, keyStr.string(), matchWhole);

    String searchString = String("%") + AddEscapeCharacter(keyStr) + "%";
    String threadIdString("");

    if (matchWhole == MATCH_BY_THREAD_ID) {
        threadIdString = GetThreadIdString(keyStr);
    }

    String smsMailBoxConstraints("");
    String mmsMailBoxConstraints("");
    String smsQuery("");
    String mmsQuery("");

    if (searchMode == SEARCH_MODE_CONTENT) {
        String ap("SELECT %s FROM sms WHERE (body LIKE ? ESCAPE '");
        ap.Append(SEARCH_ESCAPE_CHARACTER);
        ap.Append("') ");
        smsQuery.AppendFormat(ap.string(), SMS_PROJECTION.string());

        String ap2("SELECT %s FROM pdu,part,addr WHERE ((part.mid=pdu._id) AND ");
        ap2.Append("(addr.msg_id=pdu._id) AND ");
        ap2.Append("(addr.type=%d) AND ");
        ap2.Append("(part.ct='text/plain') AND ");
        ap2.Append("(body like ? escape '");
        ap2.Append(SEARCH_ESCAPE_CHARACTER);
        ap2.Append("')) GROUP BY pdu._id");
        mmsQuery.AppendFormat(ap2.string(), MMS_PROJECTION.string(), IPduHeaders::TO);
    }
    else if (searchMode == SEARCH_MODE_NUMBER && matchWhole == MATCH_BY_ADDRESS) {
        smsQuery.AppendFormat("SELECT %s FROM sms WHERE (address LIKE ?)", SMS_PROJECTION.string());
        mmsQuery.AppendFormat(
                "SELECT %s FROM pdu,addr WHERE ((addr.msg_id=pdu._id) AND (address like ?)) GROUP BY pdu._id",
                MMS_PROJECTION_FOR_NUMBER_SEARCH.string());
    }
    else if (searchMode == SEARCH_MODE_NUMBER && matchWhole == MATCH_BY_THREAD_ID) {
        smsQuery.AppendFormat("SELECT %s FROM sms WHERE (thread_id in (%s))",
                SMS_PROJECTION.string(), threadIdString.string());

        mmsQuery.AppendFormat(
                "SELECT %s FROM pdu,addr WHERE ((thread_id in (%s)) AND (addr.msg_id = pdu._id) AND (addr.type=%d))",
                MMS_PROJECTION_FOR_NUMBER_SEARCH.string(),
                threadIdString.string(),
                IPduHeaders::TO);
    }

    String rawQuery;
    rawQuery.AppendFormat("%s UNION %s  ORDER BY date DESC", smsQuery.string(), mmsQuery.string());
    AutoPtr<ArrayOf<String> > strArray;
    if (searchMode == SEARCH_MODE_CONTENT
            || (searchMode == SEARCH_MODE_NUMBER && matchWhole == MATCH_BY_ADDRESS)) {
        strArray = ArrayOf<String>::Alloc(2);
        (*strArray)[0] = searchString;
        (*strArray)[1] = searchString;
    }
    else {
        strArray = EMPTY_STRING_ARRAY;
    }

    AutoPtr<ICursor> cc;
    db->RawQuery(rawQuery, strArray, (ICursor**)&cc);
    return cc;
}

String CMmsSmsProvider::GetThreadIdString(
    /* [in] */ const String& keyStr)
{
    AutoPtr<ArrayOf<String> > ss;
    StringUtils::Split(keyStr, String(","), (ArrayOf<String>**)&ss);
    AutoPtr<ISet> s = GetAddressIdsByAddressList(ss);
    AutoPtr<ArrayOf<Int64> > addressIdSet = GetSortedSet(s);
    String threadIdString = GetCommaSeparatedId(addressIdSet);
    if (TextUtils::IsEmpty(threadIdString)) {
        threadIdString = String("0");
    }
    return threadIdString;
}

String CMmsSmsProvider::AddEscapeCharacter(
    /* [in] */ const String& keyStr)
{
    if (keyStr == NULL) {
        return keyStr;
    }

    String s;
    s.Append(SEARCH_ESCAPE_CHARACTER);
    if (keyStr.Contains("%") || keyStr.Contains(s)) {
        StringBuilder searchKeyStrBuilder;
        Int32 keyStrLen = keyStr.GetLength();
        for (Int32 i = 0; i < keyStrLen; i++) {
            if (keyStr.GetChar(i) == '%' ||
                    keyStr.GetChar(i) == SEARCH_ESCAPE_CHARACTER) {
                searchKeyStrBuilder.AppendChar(SEARCH_ESCAPE_CHARACTER);
                searchKeyStrBuilder.AppendChar(keyStr.GetChar(i));
                continue;
            }
            searchKeyStrBuilder.AppendChar(keyStr.GetChar(i));
        }
        return searchKeyStrBuilder.ToString();
    }
    return keyStr;
}

AutoPtr<ISet/*<Long*/> CMmsSmsProvider::GetAddressIdsByAddressList(
/* [in] */ ArrayOf<String>* addresses)
{
    Int32 count = addresses->GetLength();
    AutoPtr<ISet> result;/*<Long*/
    CHashSet::New(count, (ISet**)&result);

    for (Int32 i = 0; i < count; i++) {
        String address = (*addresses)[i];
        if (address != NULL && !address.Equals(IPduHeaders::FROM_INSERT_ADDRESS_TOKEN_STR)) {
            Int64 id = GetSingleThreadId(address);
            if (id != -1L) {
                result->Add(CoreUtils::Convert(id));
            }
            else {
                Logger::E(TAG, "Address ID not found for: %s", address.string());
            }
        }
    }
    return result;
}

String CMmsSmsProvider::GetCommaSeparatedId(
    /* [in] */ ArrayOf<Int64>* addrIds)
{
    Int32 size = addrIds->GetLength();
    StringBuilder buffer;

    for (Int32 i = 0; i < size; i++) {
        if (i != 0) {
            buffer.AppendChar(',');
        }
        buffer.Append(GetThreadIds(StringUtils::ToString((*addrIds)[i])));
    }
    return buffer.ToString();
}

Int64 CMmsSmsProvider::GetSingleThreadId(
    /* [in] */ const String& address)
{
    Boolean isEmail = FALSE;
    Elastos::Droid::Provider::Telephony::Mms::IsEmailAddress(address, &isEmail);
    String refinedAddress = isEmail ? address.ToLowerCase() : address;
    String selection("address=?");
    AutoPtr<ArrayOf<String> > selectionArgs;

    if (isEmail) {
        selectionArgs = ArrayOf<String>::Alloc(1);
        (*selectionArgs)[0] = refinedAddress;
    }
    else {
        String v;
        v.AppendFormat("PHONE_NUMBERS_EQUAL(address, ?, %d)", (mUseStrictPhoneNumberComparation ? 1 : 0));
        selection += String(" OR ") + v;
        selectionArgs = ArrayOf<String>::Alloc(1);
        (*selectionArgs)[0] = refinedAddress;
        (*selectionArgs)[1] = refinedAddress;
    }

    AutoPtr<ICursor> cursor;

    // try {
        AutoPtr<ISQLiteDatabase> db;
        mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
        db->Query(
                String("canonical_addresses"), ID_PROJECTION,
                selection, selectionArgs, String(NULL)
                , String(NULL), String(NULL), (ICursor**)&cursor);

        Int32 count = 0;
        cursor->GetCount(&count);
        if (count == 0) {
            if (cursor != NULL) {
                ICloseable::Probe(cursor)->Close();
            }
            return -1L;
        }

        Boolean tmp = FALSE;
        if (cursor->MoveToFirst(&tmp), tmp) {
            Int32 iv = 0;
            cursor->GetColumnIndexOrThrow(IBaseColumns::ID, &iv);
            Int64 lv = 0;
            cursor->GetInt64(iv, &lv);
            if (cursor != NULL) {
                ICloseable::Probe(cursor)->Close();
            }
            return lv;
        }
    // } finally {
        if (cursor != NULL) {
            ICloseable::Probe(cursor)->Close();
        }
    // }

    return -1L;
}

String CMmsSmsProvider::GetThreadIdByRecipientIds(
    /* [in] */ const String& recipientIds)
{
    AutoLock lock(THIS);
    String THREAD_QUERY = String("SELECT _id FROM threads ") +
            "WHERE recipient_ids = ?";
    String resultString("0");

    if (DEBUG) {
        Logger::V(TAG, "GetThreadId THREAD_QUERY: %s, recipientIds=%s"
                , THREAD_QUERY.string(), recipientIds.string());
    }
    AutoPtr<ICursor> cursor;
    // try {
        AutoPtr<ISQLiteDatabase> db;
        mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
        AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(1);
        (*ss)[0] = recipientIds;
        db->RawQuery(THREAD_QUERY, ss, (ICursor**)&cursor);

        Int32 count = 0;
        if (cursor == NULL || (cursor->GetCount(&count), count) == 0) {
            if (cursor != NULL) {
                ICloseable::Probe(cursor)->Close();
            }
            return resultString;
        }

        Boolean move = FALSE;
        if (cursor->MoveToFirst(&move), move) {
            Int64 lv = 0;
            cursor->GetInt64(0, &lv);
            resultString = StringUtils::ToString(lv);
        }
    // } finally {
        if (cursor != NULL) {
            ICloseable::Probe(cursor)->Close();
        }
    // }

    return resultString;
}

String CMmsSmsProvider::GetThreadIds(
    /* [in] */ const String& recipientIds)
{
    AutoLock lock(THIS);
    String THREAD_QUERY = String("SELECT _id FROM threads ") +
            "WHERE recipient_ids = ? or recipient_ids like '" + recipientIds
            + " %' or recipient_ids like '% " + recipientIds
            + "' or recipient_ids like '% " + recipientIds + " %'";
    String resultString("0");
    StringBuilder buffer;

    if (DEBUG) {
        Logger::V(TAG, "GetThreadId THREAD_QUERY: %s, recipientIds=%s"
                , THREAD_QUERY.string(), recipientIds.string());
    }
    AutoPtr<ICursor> cursor;
    // try {
        AutoPtr<ISQLiteDatabase> db;
        mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
        AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(1);
        (*ss)[0] = recipientIds;
        db->RawQuery(THREAD_QUERY, ss, (ICursor**)&cursor);

        Int32 count = 0;
        if (cursor == NULL || (cursor->GetCount(&count), count) == 0) {
            if (cursor != NULL) {
                ICloseable::Probe(cursor)->Close();
            }
            return resultString;
        }
        Int32 i = 0;
        Boolean move = FALSE;
        while (cursor->MoveToNext(&move), move) {
            if (i != 0) {
                buffer.AppendChar(',');
            }
            Int64 lv = 0;
            cursor->GetInt64(0, &lv);
            buffer.Append(StringUtils::ToString(lv));
            i++;
        }
        resultString = buffer.ToString();

    // } finally {
        if (cursor != NULL) {
            ICloseable::Probe(cursor)->Close();
        }
    // }

    return resultString;
}

Boolean CMmsSmsProvider::Init()
{
    assert(URI_MATCHER != NULL);
    URI_MATCHER->AddURI(AUTHORITY, String("conversations"), URI_CONVERSATIONS);
    URI_MATCHER->AddURI(AUTHORITY, String("complete-conversations"), URI_COMPLETE_CONVERSATIONS);

    // In these patterns, "#" is the thread ID.
    URI_MATCHER->AddURI(AUTHORITY, String("conversations/#"), URI_CONVERSATIONS_MESSAGES);
    URI_MATCHER->AddURI(AUTHORITY, String("conversations/#/recipients"), URI_CONVERSATIONS_RECIPIENTS);
    URI_MATCHER->AddURI(AUTHORITY, String("conversations/type/#"), URI_CONVERSATION_TYPE_COUNT);

    URI_MATCHER->AddURI(AUTHORITY, String("conversations/#/subject"), URI_CONVERSATIONS_SUBJECT);

    //"#" is the mailbox name id, such as inbox=1, sent=2, draft = 3 , outbox = 4
    URI_MATCHER->AddURI(AUTHORITY, String("mailbox/#"), URI_MAILBOX_MESSAGES);

    // URI for deleting obsolete threads.
    URI_MATCHER->AddURI(AUTHORITY, String("conversations/obsolete"), URI_OBSOLETE_THREADS);

    // URI for search messages in mailbox mode with obtained search mode
    // such as content, number and name
    URI_MATCHER->AddURI(AUTHORITY, String("search-message"), URI_SEARCH_MESSAGE);

    URI_MATCHER->AddURI(AUTHORITY, String("messages/byphone/*"), URI_MESSAGES_BY_PHONE);

    // In this pattern, two query parameter names are expected:
    // "subject" and "recipient."  Multiple "recipient" parameters
    // may be present.
    URI_MATCHER->AddURI(AUTHORITY, String("threadID"), URI_THREAD_ID);

    // Use this pattern to query the canonical address by given ID.
    URI_MATCHER->AddURI(AUTHORITY, String("canonical-address/#"), URI_CANONICAL_ADDRESS);

    // Use this pattern to query all canonical addresses.
    URI_MATCHER->AddURI(AUTHORITY, String("canonical-addresses"), URI_CANONICAL_ADDRESSES);

    URI_MATCHER->AddURI(AUTHORITY, String("search"), URI_SEARCH);
    URI_MATCHER->AddURI(AUTHORITY, String("searchSuggest"), URI_SEARCH_SUGGEST);

    // In this pattern, two query parameters may be supplied:
    // "protocol" and "message." For example:
    //   content://mms-sms/pending?
    //       -> Return all pending messages;
    //   content://mms-sms/pending?protocol=sms
    //       -> Only return pending SMs;
    //   content://mms-sms/pending?protocol=mms&message=1
    //       -> Return the the pending MM which ID Equals '1'.
    //
    URI_MATCHER->AddURI(AUTHORITY, String("pending"), URI_PENDING_MSG);

    // Use this pattern to get a list of undelivered messages.
    URI_MATCHER->AddURI(AUTHORITY, String("undelivered"), URI_UNDELIVERED_MSG);

    // Use this pattern to see what delivery status reports (for
    // both MMS and SMS) have not been delivered to the user.
    URI_MATCHER->AddURI(AUTHORITY, String("notifications"), URI_NOTIFICATIONS);

    URI_MATCHER->AddURI(AUTHORITY, String("draft"), URI_DRAFT);

    URI_MATCHER->AddURI(AUTHORITY, String("locked"), URI_FIRST_LOCKED_MESSAGE_ALL);

    URI_MATCHER->AddURI(AUTHORITY, String("locked/#"), URI_FIRST_LOCKED_MESSAGE_BY_THREAD_ID);

    URI_MATCHER->AddURI(AUTHORITY, String("messageIdToThread"), URI_MESSAGE_ID_TO_THREAD);
    InitializeColumnSets();
    return TRUE;
}

} // namespace Telephony
} // namespace Providers
} // namespace Droid
} // namespace Elastos
