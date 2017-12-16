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

#include "elastos/droid/providers/telephony/CSmsProvider.h"
#include "elastos/droid/providers/telephony/MmsSmsDatabaseHelper.h"
#include "elastos/droid/os/Binder.h"
#include <elastos/droid/provider/Telephony.h>
#include <elastos/droid/telephony/SmsManager.h>
#include "elastos/droid/text/TextUtils.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CUriMatcher;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IUriMatcher;
using Elastos::Droid::Database::CDatabaseUtils;
using Elastos::Droid::Database::CMatrixCursor;
using Elastos::Droid::Database::IDatabaseUtils;
using Elastos::Droid::Database::IMatrixCursor;
using Elastos::Droid::Database::Sqlite::CSQLiteQueryBuilder;
using Elastos::Droid::Database::Sqlite::ISQLiteQueryBuilder;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::CContactsPhones;
using Elastos::Droid::Provider::IContactsPhones;
using Elastos::Droid::Provider::ITelephonySms;
using Elastos::Droid::Provider::ITelephonyMmsSmsWordsTable;
using Elastos::Droid::Provider::ITelephonySmsConversations;
using Elastos::Droid::Provider::ITelephonyTextBasedSmsColumns;
using Elastos::Droid::Telephony::CSubscriptionManager;
using Elastos::Droid::Telephony::ISmsManager;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::SmsManager;
using Elastos::Droid::Telephony::SmsMessageMessageClass;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::CString;
using Elastos::Core::CSystem;
using Elastos::Core::ICharSequence;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Telephony {

static AutoPtr<IUri> InitUri(
    /* [in] */ const String& v)
{
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> uri;
    helper->Parse(v, (IUri**)&uri);
    return uri;
}

static AutoPtr<ArrayOf<String> > InitPROJECTION()
{
    AutoPtr<ArrayOf<String> > vs = ArrayOf<String>::Alloc(1);
    (*vs)[0] = IContactsPhones::PERSON_ID;
    return vs;
}

static AutoPtr<ArrayOf<String> > InitIDProjection()
{
    AutoPtr<ArrayOf<String> > vs = ArrayOf<String>::Alloc(1);
    (*vs)[0] = String("_id");
    return vs;
}

static AutoPtr<ArrayOf<String> > InitICCCOLUMNS()
{
    AutoPtr<ArrayOf<String> > vs = ArrayOf<String>::Alloc(14);
    // N.B.: These columns must appear in the same order as the
    // calls to add appear in convertIccToSms.
    (*vs)[0] = String("service_center_address");       // getServiceCenterAddress
    (*vs)[1] = String("address");                      // getDisplayOriginatingAddress
    (*vs)[2] = String("message_class");                // getMessageClass
    (*vs)[3] = String("body");                         // getDisplayMessageBody
    (*vs)[4] = String("date");                         // getTimestampMillis
    (*vs)[5] = String("status");                       // getStatusOnIcc
    (*vs)[6] = String("index_on_icc");                 // getIndexOnIcc
    (*vs)[7] = String("is_status_report");             // isStatusReportMessage
    (*vs)[8] = String("transport_type");               // Always "sms".
    (*vs)[9] = String("type");                         // Always MESSAGE_TYPE_ALL.
    (*vs)[10] = String("locked");                       // Always 0 (false).
    (*vs)[11] = String("error_code");                   // Always 0
    (*vs)[12] = String("_id");
    (*vs)[13] = String("phone_id");
    return vs;
}

static AutoPtr<IHashMap> InitMap()
{
    AutoPtr<IHashMap> map;
    CHashMap::New((IHashMap**)&map);
    return map;
}

AutoPtr<IUriMatcher> InitUriMatcher()
{
    AutoPtr<IUriMatcher> um;
    CUriMatcher::New(IUriMatcher::NO_MATCH, (IUriMatcher**)&um);
    return um;
}

AutoPtr<IUri> CSmsProvider::NOTIFICATION_URI = InitUri(String("content://sms"));
AutoPtr<IUri> CSmsProvider::ICC_URI = InitUri(String("content://sms/icc"));
const String CSmsProvider::TABLE_SMS("sms");
const String CSmsProvider::TABLE_RAW("raw");
const String CSmsProvider::TABLE_SR_PENDING("sr_pending");
const String CSmsProvider::TABLE_WORDS("words");
const Int32 CSmsProvider::DELETE_SUCCESS = 1;
const Int32 CSmsProvider::DELETE_FAIL = 0;
const Int32 CSmsProvider::MESSAGE_ID = 1;
const Int32 CSmsProvider::PHONE1 = 0;
const Int32 CSmsProvider::PHONE2 = 1;
const Int32 CSmsProvider::ONE = 1;

AutoPtr<ArrayOf<String> > CSmsProvider::CONTACT_QUERY_PROJECTION = InitPROJECTION();
const Int32 CSmsProvider::PERSON_ID_COLUMN = 0;
AutoPtr<ArrayOf<String> > CSmsProvider::ICC_COLUMNS = InitICCCOLUMNS();

const String CSmsProvider::TAG("SmsProvider");
const String CSmsProvider::VND_ANDROID_SMS("vnd.android.cursor.item/sms");
const String CSmsProvider::VND_ANDROID_SMSCHAT("vnd.android.cursor.item/sms-chat");
const String CSmsProvider::VND_ANDROID_DIR_SMS("vnd.android.cursor.dir/sms");
AutoPtr<IHashMap> CSmsProvider::sConversationProjectionMap = InitMap();
AutoPtr<ArrayOf<String> > CSmsProvider::sIDProjection = InitIDProjection();
const Int32 CSmsProvider::SMS_ALL = 0;
const Int32 CSmsProvider::SMS_ALL_ID = 1;
const Int32 CSmsProvider::SMS_INBOX = 2;
const Int32 CSmsProvider::SMS_INBOX_ID = 3;
const Int32 CSmsProvider::SMS_SENT = 4;
const Int32 CSmsProvider::SMS_SENT_ID = 5;
const Int32 CSmsProvider::SMS_DRAFT = 6;
const Int32 CSmsProvider::SMS_DRAFT_ID = 7;
const Int32 CSmsProvider::SMS_OUTBOX = 8;
const Int32 CSmsProvider::SMS_OUTBOX_ID = 9;
const Int32 CSmsProvider::SMS_CONVERSATIONS = 10;
const Int32 CSmsProvider::SMS_CONVERSATIONS_ID = 11;
const Int32 CSmsProvider::SMS_RAW_MESSAGE = 15;
const Int32 CSmsProvider::SMS_ATTACHMENT = 16;
const Int32 CSmsProvider::SMS_ATTACHMENT_ID = 17;
const Int32 CSmsProvider::SMS_NEW_THREAD_ID = 18;
const Int32 CSmsProvider::SMS_QUERY_THREAD_ID = 19;
const Int32 CSmsProvider::SMS_STATUS_ID = 20;
const Int32 CSmsProvider::SMS_STATUS_PENDING = 21;
const Int32 CSmsProvider::SMS_ALL_ICC = 22;
const Int32 CSmsProvider::SMS_ICC = 23;
const Int32 CSmsProvider::SMS_FAILED = 24;
const Int32 CSmsProvider::SMS_FAILED_ID = 25;
const Int32 CSmsProvider::SMS_QUEUED = 26;
const Int32 CSmsProvider::SMS_UNDELIVERED = 27;
const Int32 CSmsProvider::SMS_ALL_ICC1 = 28;
const Int32 CSmsProvider::SMS_ICC1 = 29;
const Int32 CSmsProvider::SMS_ALL_ICC2 = 30;
const Int32 CSmsProvider::SMS_ICC2 = 31;
AutoPtr<IUriMatcher> CSmsProvider::sURLMatcher = InitUriMatcher();
Boolean CSmsProvider::sInit = Init();

CAR_OBJECT_IMPL(CSmsProvider);
Boolean CSmsProvider::Init()
{
    sURLMatcher->AddURI(String("sms"), String(NULL), SMS_ALL);
    sURLMatcher->AddURI(String("sms"), String("#"), SMS_ALL_ID);
    sURLMatcher->AddURI(String("sms"), String("inbox"), SMS_INBOX);
    sURLMatcher->AddURI(String("sms"), String("inbox/#"), SMS_INBOX_ID);
    sURLMatcher->AddURI(String("sms"), String("sent"), SMS_SENT);
    sURLMatcher->AddURI(String("sms"), String("sent/#"), SMS_SENT_ID);
    sURLMatcher->AddURI(String("sms"), String("draft"), SMS_DRAFT);
    sURLMatcher->AddURI(String("sms"), String("draft/#"), SMS_DRAFT_ID);
    sURLMatcher->AddURI(String("sms"), String("outbox"), SMS_OUTBOX);
    sURLMatcher->AddURI(String("sms"), String("outbox/#"), SMS_OUTBOX_ID);
    sURLMatcher->AddURI(String("sms"), String("undelivered"), SMS_UNDELIVERED);
    sURLMatcher->AddURI(String("sms"), String("failed"), SMS_FAILED);
    sURLMatcher->AddURI(String("sms"), String("failed/#"), SMS_FAILED_ID);
    sURLMatcher->AddURI(String("sms"), String("queued"), SMS_QUEUED);
    sURLMatcher->AddURI(String("sms"), String("conversations"), SMS_CONVERSATIONS);
    sURLMatcher->AddURI(String("sms"), String("conversations/*"), SMS_CONVERSATIONS_ID);
    sURLMatcher->AddURI(String("sms"), String("raw"), SMS_RAW_MESSAGE);
    sURLMatcher->AddURI(String("sms"), String("attachments"), SMS_ATTACHMENT);
    sURLMatcher->AddURI(String("sms"), String("attachments/#"), SMS_ATTACHMENT_ID);
    sURLMatcher->AddURI(String("sms"), String("threadID"), SMS_NEW_THREAD_ID);
    sURLMatcher->AddURI(String("sms"), String("threadID/*"), SMS_QUERY_THREAD_ID);
    sURLMatcher->AddURI(String("sms"), String("status/#"), SMS_STATUS_ID);
    sURLMatcher->AddURI(String("sms"), String("sr_pending"), SMS_STATUS_PENDING);
    sURLMatcher->AddURI(String("sms"), String("icc"), SMS_ALL_ICC);
    sURLMatcher->AddURI(String("sms"), String("icc/#"), SMS_ICC);
    sURLMatcher->AddURI(String("sms"), String("icc1"), SMS_ALL_ICC1);
    sURLMatcher->AddURI(String("sms"), String("icc1/#"), SMS_ICC1);
    sURLMatcher->AddURI(String("sms"), String("icc2"), SMS_ALL_ICC2);
    sURLMatcher->AddURI(String("sms"), String("icc2/#"), SMS_ICC2);
    //we keep these for not breaking old applications
    sURLMatcher->AddURI(String("sms"), String("sim"), SMS_ALL_ICC);
    sURLMatcher->AddURI(String("sms"), String("sim/#"), SMS_ICC);

    sConversationProjectionMap->Put(CoreUtils::Convert(ITelephonySmsConversations::SNIPPET)
            , CoreUtils::Convert(String("sms.body AS snippet")));
    sConversationProjectionMap->Put(CoreUtils::Convert(ITelephonyTextBasedSmsColumns::THREAD_ID)
            , CoreUtils::Convert(String("sms.thread_id AS thread_id")));
    sConversationProjectionMap->Put(CoreUtils::Convert(ITelephonySmsConversations::MESSAGE_COUNT)
            , CoreUtils::Convert(String("groups.msg_count AS msg_count")));
    sConversationProjectionMap->Put(CoreUtils::Convert(String("delta")), NULL);
    return TRUE;
}

// @Override
ECode CSmsProvider::OnCreate(
    /* [out] */ Boolean* result)
{
    Logger::D(TAG, "CSmsProvider : OnCreate+");
    VALIDATE_NOT_NULL(result);
    SetAppOps(IAppOpsManager::OP_READ_SMS, IAppOpsManager::OP_WRITE_SMS);
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    mOpenHelper = MmsSmsDatabaseHelper::GetInstance(ctx);
    *result = TRUE;
    Logger::D(TAG, "CSmsProvider : OnCreate-");
    return NOERROR;
}

// @Override
ECode CSmsProvider::Query(
    /* [in] */ IUri* url,
    /* [in] */ ArrayOf<String>* projectionIn,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sort,
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    AutoPtr<ISQLiteQueryBuilder> qb;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&qb);

    // Generate the body of the query.
    Int32 match = 0;
    sURLMatcher->Match(url, &match);
    switch (match) {
        case SMS_ALL:
            ConstructQueryForBox(qb, ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_ALL);
            break;

        case SMS_UNDELIVERED:
            ConstructQueryForUndelivered(qb);
            break;

        case SMS_FAILED:
            ConstructQueryForBox(qb, ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_FAILED);
            break;

        case SMS_QUEUED:
            ConstructQueryForBox(qb, ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_QUEUED);
            break;

        case SMS_INBOX:
            ConstructQueryForBox(qb, ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_INBOX);
            break;

        case SMS_SENT:
            ConstructQueryForBox(qb, ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_SENT);
            break;

        case SMS_DRAFT:
            ConstructQueryForBox(qb, ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_DRAFT);
            break;

        case SMS_OUTBOX:
            ConstructQueryForBox(qb, ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_OUTBOX);
            break;

        case SMS_ALL_ID: {
            qb->SetTables(TABLE_SMS);
            AutoPtr<IList> l;
            url->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(0, (IInterface**)&obj);
            AutoPtr<ICharSequence> cs;
            CString::New(String("(_id = ") + TO_STR(obj) + ")", (ICharSequence**)&cs);
            qb->AppendWhere(cs);
            break;
        }

        case SMS_INBOX_ID:
        case SMS_FAILED_ID:
        case SMS_SENT_ID:
        case SMS_DRAFT_ID:
        case SMS_OUTBOX_ID: {
            qb->SetTables(TABLE_SMS);
            AutoPtr<IList> l;
            url->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(1, (IInterface**)&obj);
            AutoPtr<ICharSequence> cs;
            CString::New(String("(_id = ") + TO_STR(obj) + ")", (ICharSequence**)&cs);
            qb->AppendWhere(cs);
            break;
        }

        case SMS_CONVERSATIONS_ID: {
            Int32 threadID = 0;

            // try {
            AutoPtr<IList> l;
            ECode ec = url->GetPathSegments((IList**)&l);
            if (FAILED(ec) || l == NULL) {
                return NOERROR;
            }
            AutoPtr<IInterface> obj;
            l->Get(1, (IInterface**)&obj);
            threadID = StringUtils::ParseInt32(TO_STR(obj));
            if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
                Logger::D(TAG, "query conversations: threadID=%d", threadID);
            }
            // }
            // catch (Exception ex) {
            //     Logger::E(TAG,
            //           "Bad conversation thread id: "
            //           + url.getPathSegments().get(1));
            //     return NULL;
            // }

            qb->SetTables(TABLE_SMS);
            AutoPtr<ICharSequence> cs;
            CString::New(String("thread_id = ") + StringUtils::ToString(threadID), (ICharSequence**)&cs);
            qb->AppendWhere(cs);
            break;
        }

        case SMS_CONVERSATIONS: {
            qb->SetTables(String("sms, (SELECT thread_id AS group_thread_id, MAX(date)AS group_date,"
                   "COUNT(*) AS msg_count FROM sms GROUP BY thread_id) AS groups"));

            AutoPtr<ICharSequence> cs;
            CString::New(String("sms.thread_id = groups.group_thread_id AND sms.date =groups.group_date")
                    , (ICharSequence**)&cs);
            qb->AppendWhere(cs);
            qb->SetProjectionMap(IMap::Probe(sConversationProjectionMap));
            break;
        }

        case SMS_RAW_MESSAGE:
            qb->SetTables(String("raw"));
            break;

        case SMS_STATUS_PENDING:
            qb->SetTables(String("sr_pending"));
            break;

        case SMS_ATTACHMENT:
            qb->SetTables(String("attachments"));
            break;

        case SMS_ATTACHMENT_ID: {
            qb->SetTables(String("attachments"));

            AutoPtr<IList> l;
            url->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(1, (IInterface**)&obj);
            AutoPtr<ICharSequence> cs;
            CString::New(String("(sms_id = ") + TO_STR(obj) + ")", (ICharSequence**)&cs);
            qb->AppendWhere(cs);
            break;
        }

        case SMS_QUERY_THREAD_ID:
            qb->SetTables(String("canonical_addresses"));
            if (projectionIn == NULL) {
                projectionIn = sIDProjection;
            }
            break;

        case SMS_STATUS_ID: {
            qb->SetTables(TABLE_SMS);

            AutoPtr<IList> l;
            url->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(1, (IInterface**)&obj);
            AutoPtr<ICharSequence> cs;
            CString::New(String("(_id = ") + TO_STR(obj) + ")", (ICharSequence**)&cs);
            qb->AppendWhere(cs);
            break;
        }

        case SMS_ALL_ICC: {
            AutoPtr<ISubscriptionManager> sm;
            CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
            Int64 sid = 0;
            sm->GetDefaultSmsSubId(&sid);
            Int32 phoneId = 0;
            sm->GetPhoneId(sid, &phoneId);
            AutoPtr<ICursor> cr = GetAllMessagesFromIcc(phoneId);
            *result = cr;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
        case SMS_ICC: {
            AutoPtr<IList> l;
            url->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(MESSAGE_ID, (IInterface**)&obj);

            String messageIndexIcc = TO_STR(obj);

            AutoPtr<ISubscriptionManager> sm;
            CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
            Int64 sid = 0;
            sm->GetDefaultSmsSubId(&sid);
            Int32 phoneId = 0;
            sm->GetPhoneId(sid, &phoneId);
            AutoPtr<ICursor> cr;
            FAIL_RETURN(GetSingleMessageFromIcc(messageIndexIcc, phoneId, (ICursor**)&cr));
            *result = cr;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
        case SMS_ALL_ICC1: {
            AutoPtr<ICursor> cr = GetAllMessagesFromIcc(PHONE1);
            *result = cr;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }

        case SMS_ICC1: {
            AutoPtr<IList> l;
            url->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(MESSAGE_ID, (IInterface**)&obj);

            String messageIndexIcc1 = TO_STR(obj);
            AutoPtr<ICursor> cr;
            FAIL_RETURN(GetSingleMessageFromIcc(messageIndexIcc1, PHONE1, (ICursor**)&cr));
            *result = cr;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }

        case SMS_ALL_ICC2: {
            AutoPtr<ICursor> cr = GetAllMessagesFromIcc(PHONE2);
            *result = cr;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }

        case SMS_ICC2: {
            AutoPtr<IList> l;
            url->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(MESSAGE_ID, (IInterface**)&obj);

            String messageIndexIcc2 = TO_STR(obj);
            AutoPtr<ICursor> cr;
            FAIL_RETURN(GetSingleMessageFromIcc(messageIndexIcc2, PHONE2, (ICursor**)&cr));
            *result = cr;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }

        default: {
            Logger::E(TAG, "Invalid request: %s", TO_CSTR(url));
            return NOERROR;
        }
    }

    String orderBy, ts;

    if (!TextUtils::IsEmpty(sort)) {
        orderBy = sort;
    }
    else if ((qb->GetTables(&ts), ts).Equals(TABLE_SMS)) {
        orderBy = ITelephonySms::DEFAULT_SORT_ORDER;
    }

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<ICursor> ret;
    qb->Query(db, projectionIn, selection, selectionArgs,
            String(NULL), String(NULL), orderBy, (ICursor**)&ret);

    // TODO: Since the URLs are a mess, always use content://sms
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IContentResolver> cr;
    ctx->GetContentResolver((IContentResolver**)&cr);
    ret->SetNotificationUri(cr, NOTIFICATION_URI);
    *result = ret;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

AutoPtr<ArrayOf<IInterface*> > CSmsProvider::ConvertIccToSms(
    /* [in] */ ISmsMessage* message,
    /* [in] */ Int32 id,
    /* [in] */ Int32 phoneId)
{
    // N.B.: These calls must appear in the same order as the
    // columns appear in ICC_COLUMNS.
    Int32 statusOnIcc = 0;
    message->GetStatusOnIcc(&statusOnIcc);
    Int32 type = ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_ALL;
    switch (statusOnIcc) {
        case ISmsManager::STATUS_ON_ICC_READ:
        case ISmsManager::STATUS_ON_ICC_UNREAD:
            type = ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_INBOX;
            break;
        case ISmsManager::STATUS_ON_ICC_SENT:
            type = ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_SENT;
            break;
        case ISmsManager::STATUS_ON_ICC_UNSENT:
            type = ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_OUTBOX;
            break;
    }
    AutoPtr<ArrayOf<IInterface*> > row = ArrayOf<IInterface*>::Alloc(14);
    String v;
    message->GetServiceCenterAddress(&v);
    row->Set(0, CoreUtils::Convert(v));

    message->GetDisplayOriginatingAddress(&v);
    row->Set(1, CoreUtils::Convert(v));//(type == ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_INBOX))
            //? message.getDisplayOriginatingAddress()
            //: message.getRecipientAddress();
    SmsMessageMessageClass ms;
    message->GetMessageClass(&ms);
    row->Set(2, CoreUtils::Convert(ms));

    message->GetDisplayMessageBody(&v);
    row->Set(3, CoreUtils::Convert(v));

    Int64 t = 0;
    message->GetTimestampMillis(&t);
    row->Set(4, CoreUtils::Convert(t));
    row->Set(5, CoreUtils::Convert(statusOnIcc));
    Int32 iv = 0;
    message->GetIndexOnIcc(&iv);
    row->Set(6, CoreUtils::Convert(iv));
    Boolean bv = FALSE;
    message->IsStatusReportMessage(&bv);
    row->Set(7, CoreUtils::Convert(bv));
    row->Set(8, CoreUtils::Convert(String("sms")));
    row->Set(9, CoreUtils::Convert(type));
    row->Set(10, CoreUtils::Convert(0));      // locke)d
    row->Set(11, CoreUtils::Convert(0));      // error_cod)e
    row->Set(12, CoreUtils::Convert(id));
    row->Set(13, CoreUtils::Convert(phoneId));
    return row;
}

/**
 * Return a Cursor containing just one message from the ICC.
 */
ECode CSmsProvider::GetSingleMessageFromIcc(
    /* [in] */ const String& messageIndexString,
    /* [in] */ Int32 phoneId,
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IArrayList> messages;/*<SmsMessage*/
    Int32 messageIndex = -1;
    // try {
    // StringUtils::ParseInt(messageIndexString);
    Logger::D(TAG, "[TODO] Check the messageIndexString [%s]", messageIndexString.string());
    // } catch (NumberFormatException exception) {
    //     throw new IllegalArgumentException("Bad SMS ICC ID: " + messageIndexString);
    // }
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    AutoPtr<ArrayOf<Int64> > sub;
    sm->GetSubId(phoneId, (ArrayOf<Int64>**)&sub);
    AutoPtr<ISmsManager> smsManager = SmsManager::GetSmsManagerForSubscriber((*sub)[0]);

    // Use phone id to avoid AppOps uid mismatch in telephony
    Int64 token = Binder::ClearCallingIdentity();
    // try {
    smsManager->GetAllMessagesFromIcc((IArrayList**)&messages);
    // } finally {
    Binder::RestoreCallingIdentity(token);
    // }
    if (messages == NULL) {
        // throw new IllegalArgumentException("ICC message not retrieved");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInterface> obj;
    messages->Get(messageIndex, (IInterface**)&obj);
    AutoPtr<ISmsMessage> message = ISmsMessage::Probe(obj);
    if (message == NULL) {
        // throw new IllegalArgumentException("Message not retrieved. ID: " + messageIndexString);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IMatrixCursor> cursor;
    CMatrixCursor::New(ICC_COLUMNS, 1, (IMatrixCursor**)&cursor);
    AutoPtr<ArrayOf<IInterface*> > as = ConvertIccToSms(message, 0, phoneId);
    cursor->AddRow(*as);
    AutoPtr<ICursor> cr = WithIccNotificationUri(ICursor::Probe(cursor));
    *result = cr;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * Return a Cursor listing all the messages stored on the ICC.
 */
AutoPtr<ICursor> CSmsProvider::GetAllMessagesFromIcc(
    /* [in] */ Int32 phoneId)
{
    AutoPtr<IArrayList> messages;/*<SmsMessage*/
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    AutoPtr<ArrayOf<Int64> > sub;
    sm->GetSubId(phoneId, (ArrayOf<Int64>**)&sub);
    AutoPtr<ISmsManager> smsManager = SmsManager::GetSmsManagerForSubscriber((*sub)[0]);

    // use phone app permissions to avoid UID mismatch in AppOpsManager.noteOp() call
    Int64 token = Binder::ClearCallingIdentity();
    // try {
    smsManager->GetAllMessagesFromIcc((IArrayList**)&messages);
    // } finally {
    Binder::RestoreCallingIdentity(token);
    // }

    Int32 count = 0;
    messages->GetSize(&count);
    AutoPtr<IMatrixCursor> cursor;
    CMatrixCursor::New(ICC_COLUMNS, count, (IMatrixCursor**)&cursor);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        messages->Get(i, (IInterface**)&obj);
        AutoPtr<ISmsMessage> message = ISmsMessage::Probe(obj);
        if (message != NULL) {
            AutoPtr<ArrayOf<IInterface*> > as = ConvertIccToSms(message, i, phoneId);
            cursor->AddRow(*as);
        }
    }
    return WithIccNotificationUri(ICursor::Probe(cursor));
}

AutoPtr<ICursor> CSmsProvider::WithIccNotificationUri(
    /* [in] */ ICursor* cursor)
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IContentResolver> cr;
    ctx->GetContentResolver((IContentResolver**)&cr);
    cursor->SetNotificationUri(cr, ICC_URI);
    return cursor;
}

void CSmsProvider::ConstructQueryForBox(
    /* [in] */ ISQLiteQueryBuilder* qb,
    /* [in] */ Int32 type)
{
    qb->SetTables(TABLE_SMS);

    if (type != ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_ALL) {
        AutoPtr<ICharSequence> cs;
        CString::New(String("type=") + StringUtils::ToString(type), (ICharSequence**)&cs);
        qb->AppendWhere(cs);
    }
}

void CSmsProvider::ConstructQueryForUndelivered(
    /* [in] */ ISQLiteQueryBuilder* qb)
{
    qb->SetTables(TABLE_SMS);

    StringBuilder sb("(type=");
    sb.Append(ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_OUTBOX);
    sb.Append(" OR type=");
    sb.Append(ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_FAILED);
    sb.Append(" OR type=");
    sb.Append(ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_QUEUED);
    sb.Append(")");
    AutoPtr<ICharSequence> cs;
    CString::New(sb.ToString(), (ICharSequence**)&cs);
    qb->AppendWhere(cs);
}

// @Override
ECode CSmsProvider::GetType(
    /* [in] */ IUri* url,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IList> l;
    url->GetPathSegments((IList**)&l);
    Int32 size = 0;
    l->GetSize(&size);

    switch (size) {
        case 0: {
            *result = VND_ANDROID_DIR_SMS;
            return NOERROR;
        }
        case 1: {
            // try {
            AutoPtr<IInterface> obj;
            l->Get(0, (IInterface**)&obj);
            // Integer.parseInt(url.getPathSegments().get(0));
            Logger::D(TAG, "[TODO] Check the Number, the string is [%s]", TO_CSTR(obj));
            *result = VND_ANDROID_SMS;
            return NOERROR;
            // } catch (NumberFormatException ex) {
            //     return VND_ANDROID_DIR_SMS;
            // }
        }
        case 2: {
            // TODO: What about "threadID"?
            AutoPtr<IInterface> obj;
            l->Get(0, (IInterface**)&obj);
            if (TO_STR(obj).Equals("conversations")) {
                *result = VND_ANDROID_SMSCHAT;
                return NOERROR;
            }
            else {
                *result = VND_ANDROID_SMS;
                return NOERROR;
            }
        }
    }
    *result = String(NULL);
    return NOERROR;
}

// @Override
ECode CSmsProvider::Insert(
    /* [in] */ IUri* url,
    /* [in] */ IContentValues* initialValues,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    Int64 token = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IUri> newUri = InsertInner(url, initialValues);
    *result = newUri;
    REFCOUNT_ADD(*result);
    // } finally {
    Binder::RestoreCallingIdentity(token);
    // }
    return NOERROR;
}

AutoPtr<IUri> CSmsProvider::InsertInner(
    /* [in] */ IUri* url,
    /* [in] */ IContentValues* initialValues)
{
    AutoPtr<IContentValues> values;
    Int64 rowID = 0;
    Int32 type = ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_ALL;

    Int32 match = 0;
    sURLMatcher->Match(url, &match);
    String table = TABLE_SMS;

    switch (match) {
        case SMS_ALL: {
            AutoPtr<IInteger32> typeObj;
            initialValues->GetAsInteger32(ITelephonyTextBasedSmsColumns::TYPE, (IInteger32**)&typeObj);
            if (typeObj != NULL) {
                typeObj->GetValue(&type);
            }
            else {
                // default to inbox
                type = ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_INBOX;
            }
            break;
        }

        case SMS_INBOX:
            type = ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_INBOX;
            break;

        case SMS_FAILED:
            type = ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_FAILED;
            break;

        case SMS_QUEUED:
            type = ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_QUEUED;
            break;

        case SMS_SENT:
            type = ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_SENT;
            break;

        case SMS_DRAFT:
            type = ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_DRAFT;
            break;

        case SMS_OUTBOX:
            type = ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_OUTBOX;
            break;

        case SMS_RAW_MESSAGE:
            table = "raw";
            break;

        case SMS_STATUS_PENDING:
            table = "sr_pending";
            break;

        case SMS_ATTACHMENT:
            table = "attachments";
            break;

        case SMS_NEW_THREAD_ID:
            table = "canonical_addresses";
            break;

        default: {
            Logger::E(TAG, "Invalid request: %s", TO_CSTR(url));
            return NULL;
        }
    }

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);

    if (table.Equals(TABLE_SMS)) {
        Boolean addDate = FALSE;
        Boolean addType = FALSE;

        // Make sure that the date and type are set
        if (initialValues == NULL) {
            CContentValues::New(1, (IContentValues**)&values);
            addDate = TRUE;
            addType = TRUE;
        }
        else {
            CContentValues::New(initialValues, (IContentValues**)&values);

            Boolean tmp = FALSE;
            if (initialValues->ContainsKey(ITelephonyTextBasedSmsColumns::DATE, &tmp), !tmp) {
                addDate = TRUE;
            }

            if (initialValues->ContainsKey(ITelephonyTextBasedSmsColumns::TYPE, &tmp), !tmp) {
                addType = TRUE;
            }
        }

        if (addDate) {
            Int64 currentTimeMillis;
            AutoPtr<ISystem> system;
            CSystem::AcquireSingleton((ISystem**)&system);
            system->GetCurrentTimeMillis(&currentTimeMillis);
            values->Put(ITelephonyTextBasedSmsColumns::DATE, currentTimeMillis);
        }

        if (addType && (type != ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_ALL)) {
            values->Put(ITelephonyTextBasedSmsColumns::TYPE, type);
        }

        // thread_id
        AutoPtr<IInteger64> threadId;
        values->GetAsInteger64(ITelephonyTextBasedSmsColumns::THREAD_ID, (IInteger64**)&threadId);
        String address;
        values->GetAsString(ITelephonyTextBasedSmsColumns::ADDRESS, &address);

        if (((threadId == NULL) || (threadId == 0)) && (!TextUtils::IsEmpty(address))) {
            AutoPtr<IContext> ctx;
            GetContext((IContext**)&ctx);
            Int64 lv = 0;
            Elastos::Droid::Provider::Telephony::Threads::GetOrCreateThreadId(ctx, address, &lv);
            values->Put(ITelephonyTextBasedSmsColumns::THREAD_ID, lv);
        }

        // If this message is going in as a draft, it should replace any
        // other draft messages in the thread.  Just delete all draft
        // messages with this thread ID.  We could add an OR REPLACE to
        // the insert below, but we'd have to query to find the old _id
        // to produce a conflict anyway.
        AutoPtr<IInteger32> io;
        values->GetAsInteger32(ITelephonyTextBasedSmsColumns::TYPE, (IInteger32**)&io);
        Int32 iv = 0;
        io->GetValue(&iv);
        if (iv == ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_DRAFT) {
            AutoPtr<ArrayOf<String> > ss = ArrayOf<String>::Alloc(2);
            String v;
            values->GetAsString(ITelephonyTextBasedSmsColumns::THREAD_ID, &v);
            (*ss)[0] = v;
            (*ss)[1] = StringUtils::ToString(ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_DRAFT);
            Int32 tmp = 0;
            db->Delete(TABLE_SMS, String("thread_id=? AND type=?"), ss, &tmp);
        }

        if (type == ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_INBOX) {
            // Look up the person if not already filled in.
            AutoPtr<IInteger64> lo;
            values->GetAsInteger64(ITelephonyTextBasedSmsColumns::PERSON, (IInteger64**)&lo);
            if ((lo == NULL) && (!TextUtils::IsEmpty(address))) {
                AutoPtr<IUriHelper> helper;
                CUriHelper::AcquireSingleton((IUriHelper**)&helper);
                String encode;
                helper->Encode(address, &encode);
                AutoPtr<ICursor> cursor;
                AutoPtr<IUri> uri;
                AutoPtr<IContactsPhones> cp;
                CContactsPhones::AcquireSingleton((IContactsPhones**)&cp);
                AutoPtr<IUri> tu;
                cp->GetCONTENT_FILTER_URL((IUri**)&tu);
                helper->WithAppendedPath(tu, encode, (IUri**)&uri);
                ECode ec = NOERROR;
                do {
                    AutoPtr<IContext> ctx;
                    GetContext((IContext**)&ctx);
                    AutoPtr<IContentResolver> cr;
                    ctx->GetContentResolver((IContentResolver**)&cr);
                    ec = cr->Query(uri, CONTACT_QUERY_PROJECTION,
                            String(NULL), NULL, String(NULL), (ICursor**)&cursor);
                    if (FAILED(ec)) {
                        break;
                    }

                    Boolean move = FALSE;
                    if ((cursor != NULL) && (cursor->MoveToFirst(&move), move)) {
                        Int64 id = 0;
                        cursor->GetInt64(PERSON_ID_COLUMN, &id);
                        values->Put(ITelephonyTextBasedSmsColumns::PERSON, id);
                    }
                } while (0);

                if (FAILED(ec)) {
                    Logger::E(TAG, "insert: query contact uri %s", TO_CSTR(uri));
                }
                // finally {
                if (cursor != NULL) {
                    ICloseable::Probe(cursor)->Close();
                }
                // }
            }
        }
        else {
            // Mark all non-inbox messages read.
            values->Put(ITelephonyTextBasedSmsColumns::READ, ONE);
        }
    }
    else {
        if (initialValues == NULL) {
            CContentValues::New(1, (IContentValues**)&values);
        }
        else {
            values = initialValues;
        }
    }

    db->Insert(table, String("body"), values, &rowID);

    // Don't use a trigger for updating the words table because of a bug
    // in FTS3.  The bug is such that the call to get the last inserted
    // row is incorrect.
    if (table == TABLE_SMS) {
        // Update the words table with a corresponding row.  The words table
        // allows us to search for words quickly, without scanning the whole
        // table;
        AutoPtr<IContentValues> cv;
        CContentValues::New((IContentValues**)&cv);
        cv->Put(ITelephonyMmsSmsWordsTable::ID, rowID);
        String v;
        values->GetAsString(String("body"), &v);
        cv->Put(ITelephonyMmsSmsWordsTable::INDEXED_TEXT, v);
        cv->Put(ITelephonyMmsSmsWordsTable::SOURCE_ROW_ID, rowID);
        cv->Put(ITelephonyMmsSmsWordsTable::TABLE_ID, 1);
        Int64 tv = 0;
        db->Insert(TABLE_WORDS, ITelephonyMmsSmsWordsTable::INDEXED_TEXT, cv, &tv);
    }
    if (rowID > 0) {
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uri;
        helper->Parse(String("content://") + table + "/" + StringUtils::ToString(rowID), (IUri**)&uri);

        if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
            Logger::D(TAG, "insert %s succeeded", TO_CSTR(uri));
        }
        NotifyChange(uri);
        return uri;
    }
    else {
        Logger::E(TAG,"insert: failed! %s", TO_CSTR(values));
    }

    return NULL;
}

// @Override
ECode CSmsProvider::Delete(
    /* [in] */ IUri* url,
    /* [in] */ const String& _where,
    /* [in] */ ArrayOf<String>* whereArgs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    String where = _where;
    Int32 count;
    Int32 match = 0;
    sURLMatcher->Match(url, &match);
    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    switch (match) {
        case SMS_ALL:
            db->Delete(TABLE_SMS, where, whereArgs, &count);
            if (count != 0) {
                // Don't update threads unless something changed.
                MmsSmsDatabaseHelper::UpdateAllThreads(db, where, whereArgs);
            }
            break;

        case SMS_ALL_ID: {
            // try {
            AutoPtr<IList> l;
            url->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(0, (IInterface**)&obj);
            Int32 message_id = StringUtils::ParseInt32(TO_STR(obj));
            Logger::D(TAG, "[TODO] Check message_id, the vlaue is [%s]", TO_CSTR(obj));
            count = MmsSmsDatabaseHelper::DeleteOneSms(db, message_id);
            // } catch (Exception e) {
            //     throw new IllegalArgumentException(
            //         "Bad message id: " + url.getPathSegments().get(0));
            // }
            break;
        }

        case SMS_CONVERSATIONS_ID: {
            Int32 threadID = 0;

            AutoPtr<IList> l;
            url->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(1, (IInterface**)&obj);
            // try {
            threadID = StringUtils::ParseInt32(TO_STR(obj));
            Logger::D(TAG, "[TODO] Check threadID, the vlaue is [%s]", TO_CSTR(obj));
            // } catch (Exception ex) {
            //     throw new IllegalArgumentException("Bad conversation thread id: "
            //             + url.getPathSegments().get(1));
            // }

            // delete the messages from the sms table
            AutoPtr<IDatabaseUtils> du;
            CDatabaseUtils::AcquireSingleton((IDatabaseUtils**)&du);
            du->ConcatenateWhere(String("thread_id=") + StringUtils::ToString(threadID), where, &where);
            db->Delete(TABLE_SMS, where, whereArgs, &count);
            MmsSmsDatabaseHelper::UpdateThread(db, threadID);
            break;
        }

        case SMS_RAW_MESSAGE: {
            db->Delete(String("raw"), where, whereArgs, &count);
            break;
        }

        case SMS_STATUS_PENDING: {
            db->Delete(String("sr_pending"), where, whereArgs, &count);
            break;
        }

        case SMS_ICC: {
            AutoPtr<IList> l;
            url->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(MESSAGE_ID, (IInterface**)&obj);
            String messageIndexIcc = TO_STR(obj);
            AutoPtr<ISubscriptionManager> sm;
            CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
            Int64 sid = 0;
            sm->GetDefaultSmsSubId(&sid);
            Int32 phoneId = 0;
            sm->GetPhoneId(sid, &phoneId);
            *result = DeleteMessageFromIcc(messageIndexIcc, phoneId);
            return NOERROR;
        }
        case SMS_ICC1: {
            AutoPtr<IList> l;
            url->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(MESSAGE_ID, (IInterface**)&obj);
            String messageIndexIcc1 = TO_STR(obj);

            *result = DeleteMessageFromIcc(messageIndexIcc1, PHONE1);
            return NOERROR;
        }
        case SMS_ICC2: {
            AutoPtr<IList> l;
            url->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(MESSAGE_ID, (IInterface**)&obj);
            String messageIndexIcc2 = TO_STR(obj);

            *result = DeleteMessageFromIcc(messageIndexIcc2, PHONE2);
            return NOERROR;
        }

        default: {
            // throw new IllegalArgumentException("Unknown URL");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    if (count > 0) {
        NotifyChange(url);
    }
    *result = count;
    return NOERROR;
}

/**
 * Delete the message at index from ICC.  Return TRUE iff
 * successful.
 */
Int32 CSmsProvider::DeleteMessageFromIcc(
    /* [in] */ const String& messageIndexString,
    /* [in] */ Int32 phoneId)
{
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    AutoPtr<ArrayOf<Int64> > sub;
    sm->GetSubId(phoneId, (ArrayOf<Int64>**)&sub);
    AutoPtr<ISmsManager> smsManager = SmsManager::GetSmsManagerForSubscriber((*sub)[0]);

    // Use phone id to avoid AppOps uid mismatch in telephony
    Int64 token = Binder::ClearCallingIdentity();

    // try {
    Boolean flag = FALSE;
    Logger::D(TAG, "[TODO] Check the Number, the messageIndexString is [%s]", messageIndexString.string());
    smsManager->DeleteMessageFromIcc(StringUtils::ParseInt32(messageIndexString), &flag);
    // } catch (NumberFormatException exception) {
    //     throw new IllegalArgumentException(
    //             "Bad SMS ICC ID: " + messageIndexString);
    // } finally {
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IContentResolver> cr;
    ctx->GetContentResolver((IContentResolver**)&cr);
    cr->NotifyChange(ICC_URI, NULL, TRUE, IUserHandle::USER_ALL);

    Binder::RestoreCallingIdentity(token);
    // }
    return flag ? DELETE_SUCCESS : DELETE_FAIL;
}

// @Override
ECode CSmsProvider::Update(
    /* [in] */ IUri* url,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& _where,
    /* [in] */ ArrayOf<String>* whereArgs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    String where = _where;
    Int32 count = 0;
    String table = TABLE_SMS;
    String extraWhere;
    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);

    Int32 match = 0;
    sURLMatcher->Match(url, &match);
    switch (match) {
        case SMS_RAW_MESSAGE:
            table = TABLE_RAW;
            break;

        case SMS_STATUS_PENDING:
            table = TABLE_SR_PENDING;
            break;

        case SMS_ALL:
        case SMS_FAILED:
        case SMS_QUEUED:
        case SMS_INBOX:
        case SMS_SENT:
        case SMS_DRAFT:
        case SMS_OUTBOX:
        case SMS_CONVERSATIONS:
            break;

        case SMS_ALL_ID: {
            AutoPtr<IList> l;
            url->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(0, (IInterface**)&obj);
            extraWhere = String("_id=") + TO_STR(obj);
            break;
        }

        case SMS_INBOX_ID:
        case SMS_FAILED_ID:
        case SMS_SENT_ID:
        case SMS_DRAFT_ID:
        case SMS_OUTBOX_ID: {
            AutoPtr<IList> l;
            url->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(1, (IInterface**)&obj);
            extraWhere = String("_id=") + TO_STR(obj);
            break;
        }

        case SMS_CONVERSATIONS_ID: {
            AutoPtr<IList> l;
            url->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(1, (IInterface**)&obj);
            String threadId = TO_STR(obj);

            // try {
            // StringUtils::ParseInt32(threadId);
            Logger::D(TAG, "[TODO] Check the thread id [%s]", threadId.string());
            // } catch (Exception ex) {
            //     Logger::E(TAG, "Bad conversation thread id: " + threadId);
            //     break;
            // }

            extraWhere = String("thread_id=") + threadId;
            break;
        }

        case SMS_STATUS_ID: {
            AutoPtr<IList> l;
            url->GetPathSegments((IList**)&l);
            AutoPtr<IInterface> obj;
            l->Get(1, (IInterface**)&obj);
            extraWhere = String("_id=") + TO_STR(obj);
            break;
        }

        default: {
            // throw new UnsupportedOperationException("URI " + url + " not supported");
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }
    }

    AutoPtr<IDatabaseUtils> du;
    CDatabaseUtils::AcquireSingleton((IDatabaseUtils**)&du);
    du->ConcatenateWhere(where, extraWhere, &where);
    db->Update(table, values, where, whereArgs, &count);

    if (count > 0) {
        if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
            Logger::D(TAG, "update %s succeeded", TO_CSTR(url));
        }
        NotifyChange(url);
    }
    *result = count;
    return NOERROR;
}

void CSmsProvider::NotifyChange(
    /* [in] */ IUri* uri)
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IContentResolver> cr;
    ctx->GetContentResolver((IContentResolver**)&cr);
    cr->NotifyChange(uri, NULL, TRUE, IUserHandle::USER_ALL);
    cr->NotifyChange(Elastos::Droid::Provider::Telephony::MmsSms::CONTENT_URI, NULL, TRUE, IUserHandle::USER_ALL);

    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> u;
    helper->Parse(String("content://mms-sms/conversations/"), (IUri**)&u);
    cr->NotifyChange(u, NULL, TRUE, IUserHandle::USER_ALL);
}

} // namespace Telephony
} // namespace Providers
} // namespace Droid
} // namespace Elastos
