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

#ifndef __ELASTOS_DROID_PROVIDER_TELEPHONY_CMMSSMSPROVIDER_H__
#define __ELASTOS_DROID_PROVIDER_TELEPHONY_CMMSSMSPROVIDER_H__

#include "_Elastos_Droid_Providers_Telephony_CMmsSmsProvider.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/content/ContentProvider.h"
#include "elastos/droid/database/sqlite/SQLiteOpenHelper.h"

using Elastos::Droid::Content::ContentProvider;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IUriMatcher;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::Database::Sqlite::ISQLiteQueryBuilder;
using Elastos::Droid::Database::Sqlite::SQLiteOpenHelper;
using Elastos::Droid::Telephony::ISmsMessage;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Telephony {

/**
 * This class provides the ability to query the MMS and SMS databases
 * at the same time, mixing messages from both in a single thread
 * (A.K.A. conversation).
 *
 * A virtual column, ITelephonyMmsSms::TYPE_DISCRIMINATOR_COLUMN, may be
 * requested in the projection for a query.  Its value is either "mms"
 * or "sms", depending on whether the message represented by the row
 * is an MMS message or an SMS message, respectively.
 *
 * This class also provides the ability to find out what addresses
 * participated in a particular thread.  It doesn't support updates
 * for either of these.
 *
 * This class provides a way to allocate and retrieve thread IDs.
 * This is done atomically through a query.  There is no insert URI
 * for this.
 *
 * Finally, this class provides a way to delete or update all messages
 * in a thread.
 */
CarClass(CMmsSmsProvider)
    , public ContentProvider
    , public IMmsSmsProvider
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    // @Override
    CARAPI OnCreate(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI Query(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [out] */ ICursor** result);

    // @Override
    CARAPI GetType(
        /* [in] */ IUri* uri,
        /* [out] */ String* result);

    // @Override
    CARAPI Delete(
        /* [in] */ IUri* uri,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* result);

    // @Override
    CARAPI Insert(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [out] */ IUri** result);

    // @Override
    CARAPI Update(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String> * selectionArgs,
        /* [out] */ Int32* result);

private:
    /**
     * Return the canonical address ID for this address.
     */
    CARAPI_(Int64) GetSingleAddressId(
        /* [in] */ const String& address);

    /**
     * Return the canonical address IDs for these addresses.
     */
    CARAPI_(AutoPtr<ISet/*<Long*/>) GetAddressIds(
        /* [in] */ IList/*<String>*/* addresses);

    /**
     * Return a sorted array of the given Set of Longs.
     */
    CARAPI_(AutoPtr<ArrayOf<Int64> >) GetSortedSet(
        /* [in] */ ISet/*<Long>*/* numbers);

    /**
     * Return a String of the numbers in the given array, in order,
     * separated by spaces.
     */
    CARAPI_(String) GetSpaceSeparatedNumbers(
        /* [in] */ ArrayOf<Int64>* numbers);

    /**
     * Insert a record for a new thread.
     */
    CARAPI_(void) InsertThread(
        /* [in] */ const String& recipientIds,
        /* [in] */ Int32 numberOfRecipients);

    /**
     * Return the thread ID for this list of
     * recipients IDs.  If no thread exists with this ID, create
     * one and return it.  Callers should always use
     * Threads.getThreadId to access this information.
     */
    CARAPI_(AutoPtr<ICursor>) GetThreadId(
        /* [in] */ IList/*<String>*/* recipients);

    static CARAPI_(String) ConcatSelections(
        /* [in] */ const String& selection1,
        /* [in] */ const String& selection2);

    /**
     * Returns new array of currentArgs with newArgs appended to end
     *
     * @param currentArgs
     * @param newArgs
     * @return
     */
    static CARAPI_(AutoPtr<ArrayOf<String> >) AppendSelectionArgs(
        /* [in] */ ArrayOf<String>* currentArgs,
        /* [in] */ ArrayOf<String>* newArgs);

    /**
     * If a NULL projection is given, return the union of all columns
     * in both the MMS and SMS messages tables.  Otherwise, return the
     * given projection.
     */
    static CARAPI_(AutoPtr<ArrayOf<String> >) HandleNullMessageProjection(
        /* [in] */ ArrayOf<String>* projection);

    /**
     * If a NULL projection is given, return the set of all columns in
     * the threads table.  Otherwise, return the given projection.
     */
    static CARAPI_(AutoPtr<ArrayOf<String> >) HandleNullThreadsProjection(
        /* [in] */ ArrayOf<String>* projection);

    /**
     * If a NULL sort order is given, return "normalized_date ASC".
     * Otherwise, return the given sort order.
     */
    static CARAPI_(String) HandleNullSortOrder (
        /* [in] */ const String& sortOrder);

    /**
     * Return existing threads in the database.
     */
    CARAPI_(AutoPtr<ICursor>) GetSimpleConversations(
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder);

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
    CARAPI_(AutoPtr<ICursor>) GetDraftThread(
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ const String& sortOrder);

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
    CARAPI_(AutoPtr<ICursor>) GetConversations(
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ const String& sortOrder);

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
    CARAPI_(AutoPtr<ICursor>) GetFirstLockedMessage(
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ const String& sortOrder);

    /**
     * Return every message in each conversation in both MMS
     * and SMS.
     */
    CARAPI_(AutoPtr<ICursor>) GetCompleteConversations(
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ const String& sortOrder);

    /**
     * Add normalized date and thread_id to the list of columns for an
     * inner projection.  This is necessary so that the outer query
     * can have access to these columns even if the caller hasn't
     * requested them in the result.
     */
    CARAPI_(AutoPtr<ArrayOf<String> >) MakeProjectionWithDateAndThreadId(
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ Int32 dateMultiple);

    /**
     * Return the union of MMS and SMS messages for this thread ID.
     */
    CARAPI_(AutoPtr<ICursor>) GetConversationMessages(
        /* [in] */ const String& threadIdString,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ const String& sortOrder,
        /* [in] */ Boolean typeCountOnly);

    /**
     * Return the union of MMS and SMS messages in one mailbox.
     */
    CARAPI_(AutoPtr<ICursor>) GetMailboxMessages(
        /* [in] */ const String& mailboxId,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [in] */ Boolean read);

    static CARAPI_(String) BuildMailboxMsgQuery(
        /* [in] */ const String& mailboxId,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [in] */ Boolean read);

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
    CARAPI_(AutoPtr<ICursor>) GetMessagesByPhoneNumber(
        /* [in] */ const String& phoneNumber,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ const String& sortOrder);

    /**
     * Return the conversation of certain thread ID.
     */
    CARAPI_(AutoPtr<ICursor>) GetConversationById(
        /* [in] */ const String& threadIdString,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder);

    static CARAPI_(String) JoinPduAndPendingMsgTables();

    static CARAPI_(AutoPtr<ArrayOf<String> >) CreateMmsProjection(
        /* [in] */ ArrayOf<String>* old);

    static CARAPI_(AutoPtr<ArrayOf<String> >) CreateMmsMailboxProjection(
        /* [in] */ ArrayOf<String>* old);

    static CARAPI_(AutoPtr<ArrayOf<String> >) CreateSmsMailboxProjection(
        /* [in] */ ArrayOf<String>* old);

    CARAPI_(AutoPtr<ICursor>) GetUndeliveredMessages(
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder);

    /**
     * Add normalized date to the list of columns for an inner
     * projection.
     */
    static CARAPI_(AutoPtr<ArrayOf<String> >) MakeProjectionWithNormalizedDate(
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ Int32 dateMultiple);

    /**
     * Add normalized date to the list of columns for an inner
     * projection.
     */
    static CARAPI_(AutoPtr<ArrayOf<String> >) MakeSmsProjectionWithNormalizedDate(
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ Int32 dateMultiple);

    static CARAPI_(AutoPtr<ArrayOf<String> >) MakeMmsProjectionWithNormalizedDate(
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ Int32 dateMultiple);

    static CARAPI_(String) BuildConversationQuery(
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ const String& sortOrder,
        /* [in] */ Boolean typeCountOnly);

    /**
     * Delete the conversation with the given thread ID.
     */
    CARAPI_(Int32) DeleteConversation(
        /* [in] */ IUri* uri,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs);

    CARAPI_(Int32) UpdateConversation(
        /* [in] */ const String& threadIdString,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs);

    /**
     * Construct Sets of Strings containing exactly the columns
     * present in each table.  We will use this when constructing
     * UNION queries across the MMS and SMS tables.
     */
    static CARAPI_(void) InitializeColumnSets();

    CARAPI_(AutoPtr<ICursor>) GetSearchMessages(
        /* [in] */ IUri* uri,
        /* [in] */ ISQLiteDatabase* db);

    CARAPI_(String) GetThreadIdString(
        /* [in] */ const String& keyStr);

    CARAPI_(String) AddEscapeCharacter(
        /* [in] */ const String& keyStr);

    CARAPI_(AutoPtr<ISet/*<Long*/>) GetAddressIdsByAddressList(
    /* [in] */ ArrayOf<String>* addresses);

    CARAPI_(String) GetCommaSeparatedId(
        /* [in] */ ArrayOf<Int64>* addrIds);

    CARAPI_(Int64) GetSingleThreadId(
        /* [in] */ const String& address);

    CARAPI_(String) GetThreadIdByRecipientIds(
        /* [in] */ const String& recipientIds);

    CARAPI_(String) GetThreadIds(
        /* [in] */ const String& recipientIds);

    static CARAPI_(Boolean) Init();

private:
    static AutoPtr<IUriMatcher> URI_MATCHER;
    static const String TAG;
    static const Boolean DEBUG;

    static const String NO_DELETES_INSERTS_OR_UPDATES;
    static const Int32 URI_CONVERSATIONS;
    static const Int32 URI_CONVERSATIONS_MESSAGES;
    static const Int32 URI_CONVERSATIONS_RECIPIENTS;
    static const Int32 URI_MESSAGES_BY_PHONE;
    static const Int32 URI_THREAD_ID;
    static const Int32 URI_CANONICAL_ADDRESS;
    static const Int32 URI_PENDING_MSG;
    static const Int32 URI_COMPLETE_CONVERSATIONS;
    static const Int32 URI_UNDELIVERED_MSG;
    static const Int32 URI_CONVERSATIONS_SUBJECT;
    static const Int32 URI_NOTIFICATIONS;
    static const Int32 URI_OBSOLETE_THREADS;
    static const Int32 URI_DRAFT;
    static const Int32 URI_CANONICAL_ADDRESSES;
    static const Int32 URI_SEARCH;
    static const Int32 URI_SEARCH_SUGGEST;
    static const Int32 URI_FIRST_LOCKED_MESSAGE_ALL;
    static const Int32 URI_FIRST_LOCKED_MESSAGE_BY_THREAD_ID;
    static const Int32 URI_MESSAGE_ID_TO_THREAD;
    static const Int32 URI_MAILBOX_MESSAGES;
    static const Int32 URI_CONVERSATION_TYPE_COUNT;
    static const Int32 URI_SEARCH_MESSAGE;
    // Escape character
    static const Char32 SEARCH_ESCAPE_CHARACTER;

    /**
     * the name of the table that is used to store the canonical addresses for both SMS and MMS.
     */
    static const String TABLE_CANONICAL_ADDRESSES;

    // These constants are used to construct union queries across the
    // MMS and SMS base tables.

    // These are the columns that appear in both the MMS ("pdu") and
    // SMS ("sms") message tables.
    static AutoPtr<ArrayOf<String> > MMS_SMS_COLUMNS;

    // These are the columns that appear only in the MMS message
    // table.
    static AutoPtr<ArrayOf<String> > MMS_ONLY_COLUMNS;

    // These are the columns that appear only in the SMS message
    // table.
    static AutoPtr<ArrayOf<String> > SMS_ONLY_COLUMNS;

    // These are all the columns that appear in the "threads" table.
    static AutoPtr<ArrayOf<String> > THREADS_COLUMNS;

    static AutoPtr<ArrayOf<String> > CANONICAL_ADDRESSES_COLUMNS_1;

    static AutoPtr<ArrayOf<String> > CANONICAL_ADDRESSES_COLUMNS_2;

    // These are all the columns that appear in the MMS and SMS
    // message tables.
    static AutoPtr<ArrayOf<String> > UNION_COLUMNS;

    // These are all the columns that appear in the MMS table.
    static AutoPtr<ISet> MMS_COLUMNS;

    // These are all the columns that appear in the SMS table.
    static AutoPtr<ISet> SMS_COLUMNS;

    static const String VND_ANDROID_DIR_MMS_SMS;

    static AutoPtr<ArrayOf<String> > ID_PROJECTION;

    static const AutoPtr<ArrayOf<String> > EMPTY_STRING_ARRAY;

    static const AutoPtr<ArrayOf<String> > SEARCH_STRING;
    static const String SEARCH_QUERY;

    static const String SMS_CONVERSATION_CONSTRAINT;

    static const String MMS_CONVERSATION_CONSTRAINT;

    // Search on the words table but return the rows from the corresponding sms table
    static const String SMS_QUERY;

    // Search on the words table but return the rows from the corresponding parts table
    static const String MMS_QUERY;

    static AutoPtr<ArrayOf<String> > TYPE_COUNT_PROJECTION;

    // This code queries the sms and mms tables and returns a unified result set
    // of text matches.  We query the sms table which is pretty simple.  We also
    // query the pdu, part and addr table to get the mms result.  Notet we're
    // using a UNION so we have to have the same number of result columns from
    // both queries.
    static const String SMS_MMS_QUERY;

    static const String THREADS_BY_PHONE_ID_WHERE;

    static const String SMS_PROJECTION;

    static const String MMS_PROJECTION;

    static const String MMS_PROJECTION_FOR_NUMBER_SEARCH;

    static const String AUTHORITY;

    AutoPtr<SQLiteOpenHelper> mOpenHelper;

    Boolean mUseStrictPhoneNumberComparation;

    static const String THREAD_QUERY;
    static Boolean sInit;
    Object THIS;
};

} // namespace Telephony
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_TELEPHONY_CMMSSMSPROVIDER_H__
