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

#ifndef __ELASTOS_DROID_PROVIDER_TELEPHONY_CSMSPROVIDER_H__
#define __ELASTOS_DROID_PROVIDER_TELEPHONY_CSMSPROVIDER_H__

#include "_Elastos_Droid_Providers_Telephony_CSmsProvider.h"
#include "elastos/droid/content/ContentProvider.h"
#include "elastos/droid/database/sqlite/SQLiteOpenHelper.h"

using Elastos::Droid::Content::ContentProvider;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IUriMatcher;
using Elastos::Droid::Database::Sqlite::ISQLiteQueryBuilder;
using Elastos::Droid::Database::Sqlite::SQLiteOpenHelper;
using Elastos::Droid::Telephony::ISmsMessage;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Telephony {

CarClass(CSmsProvider)
    , public ContentProvider
{
public:
    CAR_OBJECT_DECL()

    // @Override
    CARAPI OnCreate(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI Query(
        /* [in] */ IUri* url,
        /* [in] */ ArrayOf<String>* projectionIn,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sort,
        /* [out] */ ICursor** result);

    // @Override
    CARAPI GetType(
        /* [in] */ IUri* url,
        /* [out] */ String* result);

    // @Override
    CARAPI Insert(
        /* [in] */ IUri* url,
        /* [in] */ IContentValues* initialValues,
        /* [out] */ IUri** result);

    // @Override
    CARAPI Delete(
        /* [in] */ IUri* url,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* whereArgs,
        /* [out] */ Int32* result);

    // @Override
    CARAPI Update(
        /* [in] */ IUri* url,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* whereArgs,
        /* [out] */ Int32* result);

private:
    CARAPI_(AutoPtr<ArrayOf<IInterface*> >) ConvertIccToSms(
        /* [in] */ ISmsMessage* message,
        /* [in] */ Int32 id,
        /* [in] */ Int32 phoneId);

    /**
     * Return a Cursor containing just one message from the ICC.
     */
    CARAPI GetSingleMessageFromIcc(
        /* [in] */ const String& messageIndexString,
        /* [in] */ Int32 phoneId,
        /* [out] */ ICursor** result);

    /**
     * Return a Cursor listing all the messages stored on the ICC.
     */
    CARAPI_(AutoPtr<ICursor>) GetAllMessagesFromIcc(
        /* [in] */ Int32 phoneId);

    CARAPI_(AutoPtr<ICursor>) WithIccNotificationUri(
        /* [in] */ ICursor* cursor);

    CARAPI_(void) ConstructQueryForBox(
        /* [in] */ ISQLiteQueryBuilder* qb,
        /* [in] */ Int32 type);

    CARAPI_(void) ConstructQueryForUndelivered(
        /* [in] */ ISQLiteQueryBuilder* qb);

    CARAPI_(AutoPtr<IUri>) InsertInner(
        /* [in] */ IUri* url,
        /* [in] */ IContentValues* initialValues);

    /**
     * Delete the message at index from ICC.  Return TRUE iff
     * successful.
     */
    CARAPI_(Int32) DeleteMessageFromIcc(
        /* [in] */ const String& messageIndexString,
        /* [in] */ Int32 phoneId);

    CARAPI_(void) NotifyChange(
        /* [in] */ IUri* uri);

    static CARAPI_(Boolean) Init();

public:
    static AutoPtr<IUri> NOTIFICATION_URI;
    static AutoPtr<IUri> ICC_URI;
    static const String TABLE_SMS;
    static const String TABLE_RAW;
    static const String TABLE_SR_PENDING;
    static const String TABLE_WORDS;
    static const Int32 DELETE_SUCCESS;
    static const Int32 DELETE_FAIL;
    static const Int32 MESSAGE_ID;
    static const Int32 PHONE1;
    static const Int32 PHONE2;
    static const Int32 ONE;

    static AutoPtr<ArrayOf<String> > CONTACT_QUERY_PROJECTION;
    static const Int32 PERSON_ID_COLUMN;

    /**
     * These are the columns that are available when reading SMS
     * messages from the ICC.  Columns whose names begin with "is_"
     * have either "TRUE" or "FALSE" as their values.
     */
    static AutoPtr<ArrayOf<String> > ICC_COLUMNS;

    AutoPtr<SQLiteOpenHelper> mOpenHelper;

    static const String TAG;
    static const String VND_ANDROID_SMS;
    static const String VND_ANDROID_SMSCHAT;
    static const String VND_ANDROID_DIR_SMS;

    static AutoPtr<IHashMap> sConversationProjectionMap;
    static AutoPtr<ArrayOf<String> > sIDProjection;

    static const Int32 SMS_ALL;
    static const Int32 SMS_ALL_ID;
    static const Int32 SMS_INBOX;
    static const Int32 SMS_INBOX_ID;
    static const Int32 SMS_SENT;
    static const Int32 SMS_SENT_ID;
    static const Int32 SMS_DRAFT;
    static const Int32 SMS_DRAFT_ID;
    static const Int32 SMS_OUTBOX;
    static const Int32 SMS_OUTBOX_ID;
    static const Int32 SMS_CONVERSATIONS;
    static const Int32 SMS_CONVERSATIONS_ID;
    static const Int32 SMS_RAW_MESSAGE;
    static const Int32 SMS_ATTACHMENT;
    static const Int32 SMS_ATTACHMENT_ID;
    static const Int32 SMS_NEW_THREAD_ID;
    static const Int32 SMS_QUERY_THREAD_ID;
    static const Int32 SMS_STATUS_ID;
    static const Int32 SMS_STATUS_PENDING;
    static const Int32 SMS_ALL_ICC;
    static const Int32 SMS_ICC;
    static const Int32 SMS_FAILED;
    static const Int32 SMS_FAILED_ID;
    static const Int32 SMS_QUEUED;
    static const Int32 SMS_UNDELIVERED;
    static const Int32 SMS_ALL_ICC1;
    static const Int32 SMS_ICC1;
    static const Int32 SMS_ALL_ICC2;
    static const Int32 SMS_ICC2;

    static AutoPtr<IUriMatcher> sURLMatcher;
    static Boolean sInit;
};

} // namespace Telephony
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_TELEPHONY_CSMSPROVIDER_H__
