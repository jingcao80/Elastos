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

#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/dialer/calllog/CallLogQueryHandler.h"
#include "elastos/droid/dialer/calllog/CallLogQuery.h"
#include "elastos/droid/dialer/voicemail/VoicemailStatusHelperImpl.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Dialer::Voicemail::VoicemailStatusHelperImpl;
using Elastos::Droid::Dialer::CallLog::EIID_ICallLogQueryHandlerListener;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Provider::ICalls;
using Elastos::Droid::Provider::CCalls;
using Elastos::Droid::Provider::IVoicemailContractStatus;
using Elastos::Droid::Provider::CVoicemailContractStatus;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Net::CUriBuilder;
using Elastos::Core::AutoLock;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

const Int32 CallLogQueryHandler::CALL_TYPE_ALL;

static AutoPtr<ArrayOf<String> > InitEmptyArray()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(0);
    return array;
}
const AutoPtr<ArrayOf<String> > CallLogQueryHandler::EMPTY_STRING_ARRAY = InitEmptyArray();
const String CallLogQueryHandler::TAG("CallLogQueryHandler");
const Int32 CallLogQueryHandler::NUM_LOGS_TO_DISPLAY;
const Int32 CallLogQueryHandler::QUERY_CALLLOG_TOKEN;
const Int32 CallLogQueryHandler::UPDATE_MARK_AS_OLD_TOKEN;
const Int32 CallLogQueryHandler::UPDATE_MARK_VOICEMAILS_AS_OLD_TOKEN;
const Int32 CallLogQueryHandler::UPDATE_MARK_MISSED_CALL_AS_READ_TOKEN;
const Int32 CallLogQueryHandler::QUERY_VOICEMAIL_STATUS_TOKEN;

//=================================================================
// CallLogQueryHandler::CatchingWorkerHandler
//=================================================================
CallLogQueryHandler::CatchingWorkerHandler::CatchingWorkerHandler(
    /* [in] */ ILooper* looper)
    : AsyncQueryHandler::WorkerHandler(IWeakReference::Probe(looper))
{}

ECode CallLogQueryHandler::CatchingWorkerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    // try {
    // Perform same query while catching any exceptions
    ECode ec = AsyncQueryHandler::WorkerHandler::HandleMessage(msg);
    if (ec == (ECode)E_SQLITE_DISK_IO_EXCEPTION) {
        Logger::W(TAG, "Exception on background worker thread %x", ec);
    }
    else if (ec == (ECode)E_SQLITE_FULL_EXCEPTION) {
        Logger::W(TAG, "Exception on background worker thread %x", ec);
    }
    else if (ec == (ECode)E_SQLITE_DATABASE_CORRUPT_EXCEPTION) {
        Logger::W(TAG, "Exception on background worker thread %x", ec);
    }
    else if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        Logger::W(TAG, "ContactsProvider not present on device %x", ec);
    }
    return NOERROR;
    // } catch (SQLiteDiskIOException e) {
    //     Log.w(TAG, "Exception on background worker thread", e);
    // } catch (SQLiteFullException e) {
    //     Log.w(TAG, "Exception on background worker thread", e);
    // } catch (SQLiteDatabaseCorruptException e) {
    //     Log.w(TAG, "Exception on background worker thread", e);
    // } catch (IllegalArgumentException e) {
    //     Log.w(TAG, "ContactsProvider not present on device", e);
    // }
}

//=================================================================
// CallLogQueryHandler
//=================================================================
CallLogQueryHandler::CallLogQueryHandler()
    : mLogLimit(0)
{}

ECode CallLogQueryHandler::CreateHandler(
    /* [in] */ ILooper* looper,
    /* [out] */ IHandler** handler)
{
    VALIDATE_NOT_NULL(handler)
    // Provide our special handler that catches exceptions
    AutoPtr<CatchingWorkerHandler> cwHandler = new CatchingWorkerHandler(looper);
    *handler = (IHandler*)cwHandler;
    REFCOUNT_ADD(*handler)
    return NOERROR;
}

ECode CallLogQueryHandler::constructor(
    /* [in] */ IContentResolver* contentResolver,
    /* [in] */ ICallLogQueryHandlerListener* listener)
{
    return constructor(contentResolver, listener, -1);
}

ECode CallLogQueryHandler::constructor(
    /* [in] */ IContentResolver* contentResolver,
    /* [in] */ ICallLogQueryHandlerListener* listener,
    /* [in] */ Int32 limit)
{
    NoNullCursorAsyncQueryHandler::constructor(contentResolver);
    if (listener != NULL) {
        AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(listener);
        wrs->GetWeakReference((IWeakReference**)&mListener);
    }
    mLogLimit = limit;
    return NOERROR;
}

void CallLogQueryHandler::FetchCalls(
    /* [in] */ Int32 callType,
    /* [in] */ Int64 newerThan)
{
    CancelFetch();
    FetchCalls(QUERY_CALLLOG_TOKEN, callType, FALSE /* newOnly */, newerThan);
}

void CallLogQueryHandler::FetchCalls(
    /* [in] */ Int32 callType)
{
    FetchCalls(callType, 0);
}

void CallLogQueryHandler::FetchVoicemailStatus()
{
    AutoPtr<IVoicemailContractStatus> vcs;
    CVoicemailContractStatus::AcquireSingleton((IVoicemailContractStatus**)&vcs);
    AutoPtr<IUri> uri;
    vcs->GetCONTENT_URI((IUri**)&uri);
    StartQuery(QUERY_VOICEMAIL_STATUS_TOKEN, NULL, uri,
            VoicemailStatusHelperImpl::PROJECTION, String(NULL), NULL, String(NULL));
}

void CallLogQueryHandler::FetchCalls(
    /* [in] */ Int32 token,
    /* [in] */ Int32 callType,
    /* [in] */ Boolean newOnly,
    /* [in] */ Int64 newerThan)
{
    // We need to check for NULL explicitly otherwise entries with where READ is NULL
    // may not match either the query or its negation.
    // We consider the calls that are not yet consumed (i.e. IS_READ = 0) as "new".
    AutoPtr<StringBuilder> where = new StringBuilder();
    Elastos::Utility::Etl::List<String> selectionArgs;

    if (newOnly) {
        where->Append(ICalls::NEW);
        where->Append(" = 1");
    }

    Int32 length = 0;
    if (callType > CALL_TYPE_ALL) {
        Int32 length;
        if (where->GetLength(&length), length > 0) {
            where->Append(" AND ");
        }
        // Add a clause to fetch only items of type voicemail.
        String str("");
        str.AppendFormat("(%s = ?)", ICalls::TYPE.string());
        where->Append(str);
        // Add a clause to fetch only items newer than the requested date
        selectionArgs.PushBack(StringUtils::ToString(callType));
    }

    if (newerThan > 0) {
        if (where->GetLength(&length), length > 0) {
            where->Append(" AND ");
        }
        String str("");
        str.AppendFormat("(%s > ?)", ICalls::DATE.string());
        where->Append(str);
        selectionArgs.PushBack(StringUtils::ToString(newerThan));
    }

    Int32 limit = (mLogLimit == -1) ? NUM_LOGS_TO_DISPLAY : mLogLimit;
    ICharSequence::Probe(where)->GetLength(&length);
    String selection = (where->GetLength(&length), length > 0) ? where->ToString() : String(NULL);

    AutoPtr<ICalls> calls;
    CCalls::AcquireSingleton((ICalls**)&calls);
    AutoPtr<IUri> contentUri;
    calls->GetCONTENT_URI_WITH_VOICEMAIL((IUri**)&contentUri);
    AutoPtr<IUriBuilder> builder;
    contentUri->BuildUpon((IUriBuilder**)&builder);
    builder->AppendQueryParameter(ICalls::LIMIT_PARAM_KEY, StringUtils::ToString(limit));
    AutoPtr<IUri> uri;
    builder->Build((IUri**)&uri);

    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(selectionArgs.GetSize());
    Elastos::Utility::Etl::List<String>::Iterator it = selectionArgs.Begin();
    for (Int32 i = 0; it != selectionArgs.End(); ++it, ++i) {
        (*array)[i] = *it;
    }
    StartQuery(token, NULL, uri, CallLogQuery::_PROJECTION,
            selection, array, ICalls::DEFAULT_SORT_ORDER);
}

void CallLogQueryHandler::CancelFetch()
{
    CancelOperation(QUERY_CALLLOG_TOKEN);
}

void CallLogQueryHandler::MarkNewCallsAsOld()
{
    // Mark all "new" calls as not new anymore.
    AutoPtr<StringBuilder> where = new StringBuilder();

    where->Append(ICalls::NEW);
    where->Append(" = 1");

    AutoPtr<IContentValues> values;
    CContentValues::New(1, (IContentValues**)&values);
    values->Put(ICalls::NEW, String("0"));

    AutoPtr<ICalls> calls;
    CCalls::AcquireSingleton((ICalls**)&calls);
    AutoPtr<IUri> contentUri;
    calls->GetCONTENT_URI_WITH_VOICEMAIL((IUri**)&contentUri);
    StartUpdate(UPDATE_MARK_AS_OLD_TOKEN,
            NULL, contentUri, values, where->ToString(), NULL);
}

void CallLogQueryHandler::MarkNewVoicemailsAsOld()
{
    // Mark all "new" voicemails as not new anymore.
    AutoPtr<StringBuilder> where = new StringBuilder();
    where->Append(ICalls::NEW);
    where->Append(" = 1 AND ");
    where->Append(ICalls::TYPE);
    where->Append(" = ?");

    AutoPtr<IContentValues> values;
    CContentValues::New(1, (IContentValues**)&values);
    values->Put(ICalls::NEW, String("0"));

    AutoPtr<ICalls> calls;
    CCalls::AcquireSingleton((ICalls**)&calls);
    AutoPtr<IUri> contentUri;
    calls->GetCONTENT_URI_WITH_VOICEMAIL((IUri**)&contentUri);
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    (*array)[0] = StringUtils::ToString(ICalls::VOICEMAIL_TYPE);
    StartUpdate(UPDATE_MARK_VOICEMAILS_AS_OLD_TOKEN,
            NULL, contentUri, values, where->ToString(), array);
}

void CallLogQueryHandler::MarkMissedCallsAsRead()
{
    // Mark all "new" calls as not new anymore.
    AutoPtr<StringBuilder> where = new StringBuilder();
    where->Append(ICalls::IS_READ);
    where->Append(" = 0");
    where->Append(" AND ");
    where->Append(ICalls::TYPE);
    where->Append(" = ");
    where->Append(ICalls::MISSED_TYPE);

    AutoPtr<IContentValues> values;
    CContentValues::New(1, (IContentValues**)&values);
    values->Put(ICalls::IS_READ, String("1"));

    AutoPtr<ICalls> calls;
    CCalls::AcquireSingleton((ICalls**)&calls);
    AutoPtr<IUri> contentUri;
    calls->GetCONTENT_URI((IUri**)&contentUri);
    StartUpdate(UPDATE_MARK_MISSED_CALL_AS_READ_TOKEN,
            NULL, contentUri, values, where->ToString(), NULL);
}

ECode CallLogQueryHandler::OnNotNullableQueryComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ ICursor* cursor)
{
    AutoLock syncLock(this);
    if (cursor == NULL) {
        return NOERROR;
    }
    // try {
    if (token == QUERY_CALLLOG_TOKEN) {
        if (UpdateAdapterData(cursor)) {
            cursor = NULL;
        }
    }
    else if (token == QUERY_VOICEMAIL_STATUS_TOKEN) {
        UpdateVoicemailStatus(cursor);
    }
    else {
        Logger::W(TAG, "Unknown query completed: ignoring: %d", token);
    }

    if (cursor != NULL) {
        ICloseable::Probe((IObject*)cursor)->Close();
    }
    // } finally {
    //     if (cursor != null) {
    //         cursor.close();
    //     }
    // }
    return NOERROR;
}

Boolean CallLogQueryHandler::UpdateAdapterData(
    /* [in] */ ICursor* cursor)
{
    AutoPtr<IInterface> listener;
    mListener->Resolve(EIID_ICallLogQueryHandlerListener, (IInterface**)&listener);
    if (listener != NULL) {
        Boolean result;
        ICallLogQueryHandlerListener::Probe(listener)->OnCallsFetched(cursor, &result);
        return result;
    }
    return FALSE;
}

void CallLogQueryHandler::UpdateVoicemailStatus(
    /* [in] */ ICursor* statusCursor)
{
    AutoPtr<IInterface> listener;
    mListener->Resolve(EIID_ICallLogQueryHandlerListener, (IInterface**)&listener);
    if (listener != NULL) {
        ICallLogQueryHandlerListener::Probe(listener)->OnVoicemailStatusFetched(statusCursor);
    }
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
