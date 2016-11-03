
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Net.h"
#include "elastos/droid/content/CursorLoader.h"
#include "elastos/droid/content/CLoaderForceLoadContentObserver.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/Arrays.h>

using Elastos::Droid::Database::IContentObserver;
using Elastos::Core::AutoLock;
using Elastos::Utility::Arrays;
using Elastos::IO::ICloseable;

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CursorLoader, AsyncTaskLoader, ICursorLoader)

CursorLoader::CursorLoader()
{}

CursorLoader::~CursorLoader()
{}

ECode CursorLoader::constructor(
    /* [in] */ IContext* context)
{
    FAIL_RETURN(AsyncTaskLoader::constructor(context));
    FAIL_RETURN(CLoaderForceLoadContentObserver::New(this, (ILoaderForceLoadContentObserver**)&mObserver));
    return NOERROR;
}

ECode CursorLoader::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder)
{
    FAIL_RETURN(AsyncTaskLoader::constructor(context));
    FAIL_RETURN(CLoaderForceLoadContentObserver::New(this, (ILoaderForceLoadContentObserver**)&mObserver));
    mUri = uri;
    mProjection = projection;
    mSelection = selection;
    mSelectionArgs = selectionArgs;
    mSortOrder = sortOrder;
    return NOERROR;
}

ECode CursorLoader::DeliverResult(
    /* [in] */ IInterface* data)
{
    if (ICursor::Probe(data) == NULL) return E_ILLEGAL_ARGUMENT_EXCEPTION;
    Boolean isReset = FALSE;

    if ((IsReset(&isReset), isReset)) {
        // An async query came in while the loader is stopped
        if (NULL != data) {
            FAIL_RETURN(ICloseable::Probe(data)->Close());
        }
        return NOERROR;
    }

    AutoPtr<ICursor> oldCursor = mCursor;
    mCursor = ICursor::Probe(data);
    Boolean isStarted = FALSE;
    if ((IsStarted(&isStarted), isStarted)) {
        FAIL_RETURN(Loader::DeliverResult(mCursor))
    }
    Boolean isClosed = FALSE;
    if (NULL != oldCursor && !Object::Equals(oldCursor, mCursor)
        && !(oldCursor->IsClosed(&isClosed), isClosed)) {
        FAIL_RETURN(ICloseable::Probe(oldCursor)->Close());
    }

    return NOERROR;
}

ECode CursorLoader::DeliverCancellation()
{
    return Loader::DeliverCancellation();
}

ECode CursorLoader::GetContext(
    /* [out] */ IContext** context)
{
    return Loader::GetContext(context);
}

ECode CursorLoader::GetId(
    /* [out] */ Int32* id)
{
    return Loader::GetId(id);
}

ECode CursorLoader::RegisterListener(
    /* [in] */ Int32 id,
    /* [in] */ ILoaderOnLoadCompleteListener* listener)
{
    return Loader::RegisterListener(id, listener);
}

ECode CursorLoader::UnregisterListener(
    /* [in] */ ILoaderOnLoadCompleteListener* listener)
{
    return Loader::UnregisterListener(listener);
}

ECode CursorLoader::RegisterOnLoadCanceledListener(
    /* [in] */ ILoaderOnLoadCanceledListener* listener)
{
    return Loader::RegisterOnLoadCanceledListener(listener);
}

ECode CursorLoader::UnregisterOnLoadCanceledListener(
    /* [in] */ ILoaderOnLoadCanceledListener* listener)
{
    return Loader::UnregisterOnLoadCanceledListener(listener);
}

ECode CursorLoader::IsStarted(
    /* [out] */ Boolean* isStarted)
{
    return Loader::IsStarted(isStarted);
}

ECode CursorLoader::IsAbandoned(
    /* [out] */ Boolean* isAbandoned)
{
    return Loader::IsAbandoned(isAbandoned);
}

ECode CursorLoader::IsReset(
    /* [out] */ Boolean* isReset)
{
    return Loader::IsReset(isReset);
}

ECode CursorLoader::StartLoading()
{
    return Loader::StartLoading();
}

ECode CursorLoader::CancelLoad(
    /* [out] */ Boolean* succeeded)
{
    return Loader::CancelLoad(succeeded);
}

ECode CursorLoader::ForceLoad()
{
    return Loader::ForceLoad();
}

ECode CursorLoader::StopLoading()
{
    return Loader::StopLoading();
}

ECode CursorLoader::Abandon()
{
    return Loader::Abandon();
}

ECode CursorLoader::Reset()
{
    return Loader::Reset();
}

ECode CursorLoader::TakeContentChanged(
    /* [out] */ Boolean* succeeded)
{
    return Loader::TakeContentChanged(succeeded);
}

ECode CursorLoader::OnContentChanged()
{
    return Loader::OnContentChanged();
}

ECode CursorLoader::DataToString(
    /* [in] */ IInterface* data,
    /* [out] */ String* str)
{
    return Loader::DataToString(data, str);
}

ECode CursorLoader::ToString(
    /* [out] */ String* str)
{
    return Loader::ToString(str);
}

ECode CursorLoader::Dump(
    /* [in] */ const String& prefix,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ ArrayOf<String>* args)
{
    FAIL_RETURN(AsyncTaskLoader::Dump(prefix, fd, writer, args));

    writer->Print(prefix);
    writer->Print(String("mUri="));
    writer->Println(Object::ToString(mUri));
    writer->Print(prefix);
    writer->Print(String("mProjection="));
    writer->Println(Arrays::ToString(mProjection));
    writer->Print(prefix);
    writer->Print(String("mSelection="));
    writer->Println(mSelection);
    writer->Print(prefix);
    writer->Print(String("mSelectionArgs="));
    writer->Println(Arrays::ToString(mSelectionArgs));
    writer->Print(prefix);
    writer->Print(String("mSortOrder="));
    writer->Println(mSortOrder);
    writer->Print(prefix);
    writer->Print(String("mCursor="));
    writer->Println(Object::ToString(mCursor));
    writer->Print(prefix);
    writer->Print(String("mContentChanged="));
    writer->Println(mContentChanged);

    return NOERROR;
}

ECode CursorLoader::SetUpdateThrottle(
    /* [in] */ Int64 delayMS)
{
    return AsyncTaskLoader::SetUpdateThrottle(delayMS);
}

ECode CursorLoader::OnCanceled(
    /* [in] */ IInterface* data)
{
    if (ICursor::Probe(data) == NULL) return E_ILLEGAL_ARGUMENT_EXCEPTION;
    Boolean isClosed = FALSE;
    if (!(ICursor::Probe(data)->IsClosed(&isClosed), isClosed)) {
        FAIL_RETURN(ICloseable::Probe(data)->Close());
    }
    return NOERROR;
}

ECode CursorLoader::LoadInBackground(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    AutoLock lock(mCursorLoaderLock);
    Boolean isCanceled = FALSE;
    if ((IsLoadInBackgroundCanceled(&isCanceled), isCanceled)) {
//        throw new OperationCanceledException();
        return E_RUNTIME_EXCEPTION;
    }

//***    FAIL_RETURN(CCancellationSignal::New((ICancellationSignal**)&mCancellationSignal));
    // try {
    AutoPtr<IContext> context;
    ECode ec = Loader::GetContext((IContext**)&context);
    if (FAILED(ec)) {
        mCancellationSignal = NULL;
        return ec;
    }

    AutoPtr<IContentResolver> resolver;
    ec = context->GetContentResolver((IContentResolver**)&resolver);
    if (FAILED(ec)) {
        mCancellationSignal = NULL;
        return ec;
    }

    AutoPtr<ICursor> cursor;
    ec = resolver->Query(mUri, mProjection, mSelection, mSelectionArgs, mSortOrder, mCancellationSignal,
        (ICursor**)&cursor);
    if (FAILED(ec)) {
        mCancellationSignal = NULL;
        return ec;
    }

    if (NULL != cursor) {
        // Ensure the cursor window is filled
        Int32 count = 0;
        ec = cursor->GetCount(&count);
        if (FAILED(ec)) {
            ICloseable::Probe(cursor)->Close();
            mCancellationSignal = NULL;
            return ec;
        }

        ec = cursor->RegisterContentObserver(IContentObserver::Probe(mObserver));
        if (FAILED(ec)) {
            ICloseable::Probe(cursor)->Close();
            mCancellationSignal = NULL;
            return ec;
        }
    }

    *result = cursor;
    REFCOUNT_ADD(*result);
    mCancellationSignal = NULL;
    return NOERROR;
    // } finally {
    //     {    AutoLock syncLock(this);
    //         mCancellationSignal = null;
    //     }
    // }
}

ECode CursorLoader::CancelLoadInBackground()
{
    AsyncTaskLoader::CancelLoadInBackground();
    AutoLock lock(mCursorLoaderLock);

    if (NULL != mCancellationSignal) {
        FAIL_RETURN(mCancellationSignal->Cancel());
    }

    return NOERROR;
}

ECode CursorLoader::IsLoadInBackgroundCanceled(
    /* [out] */ Boolean* isCanceled)
{
    return AsyncTaskLoader::IsLoadInBackgroundCanceled(isCanceled);
}

ECode CursorLoader::WaitForLoader()
{
    return AsyncTaskLoader::WaitForLoader();
}

ECode CursorLoader::GetUri(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    *uri = mUri;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CursorLoader::SetUri(
    /* [in] */ IUri* uri)
{
    mUri = uri;
    return NOERROR;
}

ECode CursorLoader::GetProjection(
    /* [out, callee] */ ArrayOf<String>** projection)
{
    VALIDATE_NOT_NULL(projection)
    *projection = mProjection;
    REFCOUNT_ADD(*projection);
    return NOERROR;
}

ECode CursorLoader::SetProjection(
    /* [in] */ ArrayOf<String>* projection)
{
    mProjection = projection;
    return NOERROR;
}

ECode CursorLoader::GetSelection(
    /* [out] */ String* selection)
{
    VALIDATE_NOT_NULL(selection)
    *selection = mSelection;
    return NOERROR;
}

ECode CursorLoader::SetSelection(
    /* [in] */ const String& selection)
{
    mSelection = selection;
    return NOERROR;
}

ECode CursorLoader::GetSelectionArgs(
    /* [out, callee] */ ArrayOf<String>** selectionArgs)
{
    VALIDATE_NOT_NULL(selectionArgs)
    *selectionArgs = mSelectionArgs;
    REFCOUNT_ADD(*selectionArgs);
    return NOERROR;
}

ECode CursorLoader::SetSelectionArgs(
    /* [in] */ ArrayOf<String>* selectionArgs)
{
    mSelectionArgs = selectionArgs;
    return NOERROR;
}

ECode CursorLoader::GetSortOrder(
    /* [out] */ String* sortOrder)
{
    VALIDATE_NOT_NULL(sortOrder)
    *sortOrder = mSortOrder;
    return NOERROR;
}

ECode CursorLoader::SetSortOrder(
    /* [in] */ const String& sortOrder)
{
    mSortOrder = sortOrder;
    return NOERROR;
}

ECode CursorLoader::OnStartLoading()
{
    if (NULL != mCursor) {
        FAIL_RETURN(DeliverResult(TO_IINTERFACE(mCursor)));
    }
    Boolean ret = FALSE;
    if ((TakeContentChanged(&ret), ret) || mCursor == NULL) {
        FAIL_RETURN(ForceLoad());
    }
    return NOERROR;
}

ECode CursorLoader::OnStopLoading()
{
    // Attempt to cancel the current load task if possible.
    Boolean ret = FALSE;
    return CancelLoad(&ret);
}

ECode CursorLoader::OnReset()
{
    FAIL_RETURN(Loader::OnReset());
    // Ensure the loader is stopped
    FAIL_RETURN(OnStopLoading());
    Boolean isClosed = FALSE;
    if (NULL != mCursor && !(mCursor->IsClosed(&isClosed), isClosed)) {
        FAIL_RETURN(ICloseable::Probe(mCursor)->Close());
    }
    mCursor = NULL;
    return NOERROR;
}

}
}
}

