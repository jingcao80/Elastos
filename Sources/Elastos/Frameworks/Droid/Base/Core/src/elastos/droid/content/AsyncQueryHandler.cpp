
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/content/AsyncQueryHandler.h"
#include "elastos/droid/os/CHandlerThread.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Core::StringUtils;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::IThread;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Content {

const String AsyncQueryHandler::TAG("AsyncQuery");
const Boolean AsyncQueryHandler::localLOGV = FALSE;
const Int32 AsyncQueryHandler::EVENT_ARG_QUERY;
const Int32 AsyncQueryHandler::EVENT_ARG_INSERT;
const Int32 AsyncQueryHandler::EVENT_ARG_UPDATE;
const Int32 AsyncQueryHandler::EVENT_ARG_DELETE;
AutoPtr<ILooper> AsyncQueryHandler::sLooper;

//==============================================================
// AsyncQueryHandler::WorkerArgs
//==============================================================
AsyncQueryHandler::WorkerArgs::WorkerArgs()
{}

AsyncQueryHandler::WorkerArgs::~WorkerArgs()
{}

//==============================================================
// AsyncQueryHandler::WorkerHandler
//==============================================================
AsyncQueryHandler::WorkerHandler::WorkerHandler(
    /* [in] */ IWeakReference* context)
    : mWeakContext(context)
{}

AsyncQueryHandler::WorkerHandler::~WorkerHandler()
{}

ECode AsyncQueryHandler::WorkerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IAsyncQueryHandler> aqh;
    mWeakContext->Resolve(EIID_IAsyncQueryHandler, (IInterface**)&aqh);
    if (NULL == aqh) return NOERROR;

    AsyncQueryHandler* context = (AsyncQueryHandler*)aqh.Get();
    AutoPtr<IInterface> resolverObj;
    context->mResolver->Resolve(EIID_IInterface, (IInterface**)&resolverObj);
    if (NULL == resolverObj) return NOERROR;
    AutoPtr<IContentResolver> resolver = (IContentResolver*)resolverObj->Probe(EIID_IContentResolver);
    if (NULL == resolver) return NOERROR;

    Int32 what, arg1;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    AutoPtr<WorkerArgs> args = (WorkerArgs*)IObject::Probe(obj);
    Int32 token = what;
    Int32 event = arg1;

    switch (event) {
        case AsyncQueryHandler::EVENT_ARG_QUERY:
        {
            AutoPtr<ICursor> cursor;
            // try {
            ECode ec = resolver->Query(args->mUri, args->mProjection,
                    args->mSelection, args->mSelectionArgs, args->mOrderBy, (ICursor**)&cursor);
            if (FAILED(ec)) cursor = NULL;
            // Calling getCount() causes the cursor window to be filled,
            // which will make the first access on the main thread a lot faster.
            if (NULL != cursor) {
                Int32 count = 0;
                ec = cursor->GetCount(&count);
                if (FAILED(ec)) {
                    cursor = NULL;
                    Logger::W(TAG, "Exception thrown during handling EVENT_ARG_QUERY");
                }
            }

            args->mResult = (IInterface*)cursor.Get();
            break;
        }
        case AsyncQueryHandler::EVENT_ARG_INSERT:
        {
            FAIL_RETURN(resolver->Insert(args->mUri, args->mValues, (IUri**)&args->mResult))
            break;
        }
        case AsyncQueryHandler::EVENT_ARG_UPDATE:
        {
            args->mResult = NULL;
            Int32 result = 0;
            FAIL_RETURN(resolver->Update(args->mUri, args->mValues, args->mSelection,
                args->mSelectionArgs, &result))
            FAIL_RETURN(CInteger32::New(result, (IInteger32**)&args->mResult))
            break;
        }
        case AsyncQueryHandler::EVENT_ARG_DELETE:
        {
            args->mResult = NULL;
            Int32 result = 0;
            FAIL_RETURN(resolver->Delete(args->mUri, args->mSelection, args->mSelectionArgs, &result))
            FAIL_RETURN(CInteger32::New(result, (IInteger32**)&args->mResult))
            break;
        }
    }

    if (AsyncQueryHandler::localLOGV) {
        Logger::D(TAG, "WorkerHandler.handleMsg: msg.arg1=%d, reply.what=%d", arg1, what);
    }

    // passing the original token value back to the caller
    // on top of the event values in arg1.
    AutoPtr<IHandler> replyHandler = args->mHandler;

    AutoPtr<IMessage> reply;
    replyHandler->ObtainMessage(token, 1, 0, TO_IINTERFACE(args), (IMessage**)&reply);
    return reply->SendToTarget();
}

//==============================================================
// AsyncQueryHandler::WorkerHandler
//==============================================================
CAR_INTERFACE_IMPL(AsyncQueryHandler, Handler, IAsyncQueryHandler)

AsyncQueryHandler::AsyncQueryHandler()
{
    // must call constructor right after create a AsyncQueryHandler.
}

AsyncQueryHandler::~AsyncQueryHandler()
{}

ECode AsyncQueryHandler::constructor()
{
    return Handler::constructor(FALSE);
}

ECode AsyncQueryHandler::constructor(
    /* [in] */ IContentResolver* cr)
{
    FAIL_RETURN(Handler::constructor(FALSE))

    AutoPtr<IWeakReferenceSource> wrs = (IWeakReferenceSource*)cr->Probe(EIID_IWeakReferenceSource);
    assert(wrs != NULL);
    wrs->GetWeakReference((IWeakReference**)&mResolver);

    {
        AutoLock lock(mAsyncQueryHandlerLock);
        if (NULL == sLooper) {
            AutoPtr<IHandlerThread> thread;
            CHandlerThread::New(String("AsyncQueryWorker"), (IHandlerThread**)&thread);
            IThread::Probe(thread)->Start();
            thread->GetLooper((ILooper**)&sLooper);
        }
    }

    return CreateHandler(sLooper, (IHandler**)&mWorkerThreadHandler);
}

ECode AsyncQueryHandler::CreateHandler(
    /* [in] */ ILooper* looper,
    /* [out] */ IHandler** handler)
{
    VALIDATE_NOT_NULL(handler)
    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    AutoPtr<WorkerHandler> worker = new WorkerHandler(wr);
    worker->constructor(looper);
    *handler = (IHandler*)worker.Get();
    REFCOUNT_ADD(*handler);
    return NOERROR;
}

ECode AsyncQueryHandler::StartQuery(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& orderBy)
{
    // Use the token as what so cancelOperations works properly

    AutoPtr<WorkerArgs> args = new WorkerArgs();
    args->mHandler = this;
    args->mUri = uri;
    args->mProjection = projection;
    args->mSelection = selection;
    args->mSelectionArgs = selectionArgs;
    args->mOrderBy = orderBy;
    args->mCookie = cookie;

    AutoPtr<IMessage> msg;
    mWorkerThreadHandler->ObtainMessage(token, EVENT_ARG_QUERY, 0, TO_IINTERFACE(args), (IMessage**)&msg);
    Boolean result;
    return mWorkerThreadHandler->SendMessage(msg, &result);
}

ECode AsyncQueryHandler::CancelOperation(
    /* [in] */ Int32 token)
{
    return mWorkerThreadHandler->RemoveMessages(token);
}

ECode AsyncQueryHandler::StartInsert(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* initialValues)
{
    // Use the token as what so cancelOperations works properly
    AutoPtr<WorkerArgs> args = new WorkerArgs();
    args->mHandler = this;
    args->mUri = uri;
    args->mCookie = cookie;
    args->mValues = initialValues;

    AutoPtr<IMessage> msg;
    mWorkerThreadHandler->ObtainMessage(token, EVENT_ARG_INSERT, 0, TO_IINTERFACE(args), (IMessage**)&msg);
    Boolean result;
    return mWorkerThreadHandler->SendMessage(msg, &result);
}

ECode AsyncQueryHandler::StartUpdate(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs)
{
    // Use the token as what so cancelOperations works properly
    AutoPtr<WorkerArgs> args = new WorkerArgs();
    args->mHandler = this;
    args->mUri = uri;
    args->mCookie = cookie;
    args->mValues = values;
    args->mSelection = selection;
    args->mSelectionArgs = selectionArgs;

    AutoPtr<IMessage> msg;
    mWorkerThreadHandler->ObtainMessage(token, EVENT_ARG_UPDATE, 0, TO_IINTERFACE(args), (IMessage**)&msg);
    Boolean result;
    return mWorkerThreadHandler->SendMessage(msg, &result);
}

ECode AsyncQueryHandler::StartDelete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ IUri* uri,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs)
{
    // Use the token as what so cancelOperations works properly
    AutoPtr<WorkerArgs> args = new WorkerArgs();
    args->mHandler = this;
    args->mUri = uri;
    args->mCookie = cookie;
    args->mSelection = selection;
    args->mSelectionArgs = selectionArgs;

    AutoPtr<IMessage> msg;
    mWorkerThreadHandler->ObtainMessage(token, EVENT_ARG_DELETE, 0, TO_IINTERFACE(args), (IMessage**)&msg);
    Boolean result;
    return mWorkerThreadHandler->SendMessage(msg, &result);
}

ECode AsyncQueryHandler::OnQueryComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ ICursor* cursor)
{
    return NOERROR;
}

ECode AsyncQueryHandler::OnInsertComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ IUri* uri)
{
    return NOERROR;
}

ECode AsyncQueryHandler::OnUpdateComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ Int32 result)
{
    return NOERROR;
}

ECode AsyncQueryHandler::OnDeleteComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ Int32 result)
{
    return NOERROR;
}

ECode AsyncQueryHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    AutoPtr<WorkerArgs> args = (WorkerArgs*) IObject::Probe(obj);
    if (localLOGV) {
        Logger::D(TAG, "AsyncQueryHandler.handleMessage: msg.what=%d, msg.arg1=%d", what, arg1);
    }

    Int32 token = what;
    Int32 event = arg1;

    // pass token back to caller on each callback.
    switch (event) {
        case EVENT_ARG_QUERY:
            return OnQueryComplete(token, args->mCookie, ICursor::Probe(args->mResult.Get()));

        case EVENT_ARG_INSERT:
            return OnInsertComplete(token, args->mCookie, IUri::Probe(args->mResult));

        case EVENT_ARG_UPDATE:
        {
            Int32 result = 0;
            FAIL_RETURN(IInteger32::Probe(args->mResult)->GetValue(&result))
            return OnUpdateComplete(token, args->mCookie, result);
        }
        case EVENT_ARG_DELETE:
        {
            Int32 result = 0;
            FAIL_RETURN(IInteger32::Probe(args->mResult)->GetValue(&result))
            return OnDeleteComplete(token, args->mCookie, result);
        }
    }

    return NOERROR;
}

}
}
}

