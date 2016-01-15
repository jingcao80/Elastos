
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Net.h"
#include "elastos/droid/content/CCursorLoader.h"
#include "elastos/droid/content/CLoaderForceLoadContentObserver.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/Arrays.h>

using Elastos::Droid::Database::IContentObserver;

using Elastos::Utility::Arrays;
using Elastos::IO::ICloseable;

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL_3(CCursorLoader, AsyncTaskLoader, ICursorLoader, IAsyncTaskLoader, ILoader)

CCursorLoader::CCursorLoader()
{}

CCursorLoader::~CCursorLoader()
{}

ECode CCursorLoader::constructor(
    /* [in] */ IContext* context)
{
    FAIL_RETURN(AsyncTaskLoader::constructor(context));
    FAIL_RETURN(CLoaderForceLoadContentObserver::New(THIS_PROBE(ILoader), (ILoaderForceLoadContentObserver**)&mObserver));
    return NOERROR;
}

ECode CCursorLoader::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder)
{
    FAIL_RETURN(AsyncTaskLoader::constructor(context));
    FAIL_RETURN(CLoaderForceLoadContentObserver::New(THIS_PROBE(ILoader), (ILoaderForceLoadContentObserver**)&mObserver));
    mUri = uri;
    mProjection = projection;
    mSelection = selection;
    mSelectionArgs = selectionArgs;
    mSortOrder = sortOrder;
    return NOERROR;
}

ECode CCursorLoader::DeliverResult(
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

ECode CCursorLoader::DeliverCancellation()
{
    return Loader::DeliverCancellation();
}

ECode CCursorLoader::GetContext(
    /* [out] */ IContext** context)
{
    return Loader::GetContext(context);
}

ECode CCursorLoader::GetId(
    /* [out] */ Int32* id)
{
    return Loader::GetId(id);
}

ECode CCursorLoader::RegisterListener(
    /* [in] */ Int32 id,
    /* [in] */ ILoaderOnLoadCompleteListener* listener)
{
    return Loader::RegisterListener(id, listener);
}

ECode CCursorLoader::UnregisterListener(
    /* [in] */ ILoaderOnLoadCompleteListener* listener)
{
    return Loader::UnregisterListener(listener);
}

ECode CCursorLoader::RegisterOnLoadCanceledListener(
    /* [in] */ ILoaderOnLoadCanceledListener* listener)
{
    return Loader::RegisterOnLoadCanceledListener(listener);
}

ECode CCursorLoader::UnregisterOnLoadCanceledListener(
    /* [in] */ ILoaderOnLoadCanceledListener* listener)
{
    return Loader::UnregisterOnLoadCanceledListener(listener);
}

ECode CCursorLoader::IsStarted(
    /* [out] */ Boolean* isStarted)
{
    return Loader::IsStarted(isStarted);
}

ECode CCursorLoader::IsAbandoned(
    /* [out] */ Boolean* isAbandoned)
{
    return Loader::IsAbandoned(isAbandoned);
}

ECode CCursorLoader::IsReset(
    /* [out] */ Boolean* isReset)
{
    return Loader::IsReset(isReset);
}

ECode CCursorLoader::StartLoading()
{
    return Loader::StartLoading();
}

ECode CCursorLoader::CancelLoad(
    /* [out] */ Boolean* succeeded)
{
    return Loader::CancelLoad(succeeded);
}

ECode CCursorLoader::ForceLoad()
{
    return Loader::ForceLoad();
}

ECode CCursorLoader::StopLoading()
{
    return Loader::StopLoading();
}

ECode CCursorLoader::Abandon()
{
    return Loader::Abandon();
}

ECode CCursorLoader::Reset()
{
    return Loader::Reset();
}

ECode CCursorLoader::TakeContentChanged(
    /* [out] */ Boolean* succeeded)
{
    return Loader::TakeContentChanged(succeeded);
}

ECode CCursorLoader::OnContentChanged()
{
    return Loader::OnContentChanged();
}

ECode CCursorLoader::DataToString(
    /* [in] */ IInterface* data,
    /* [out] */ String* str)
{
    return Loader::DataToString(data, str);
}

ECode CCursorLoader::ToString(
    /* [out] */ String* str)
{
    return Loader::ToString(str);
}

ECode CCursorLoader::Dump(
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

ECode CCursorLoader::SetUpdateThrottle(
    /* [in] */ Int64 delayMS)
{
    return AsyncTaskLoader::SetUpdateThrottle(delayMS);
}

ECode CCursorLoader::OnCanceled(
    /* [in] */ IInterface* data)
{
    if (ICursor::Probe(data) == NULL) return E_ILLEGAL_ARGUMENT_EXCEPTION;
    Boolean isClosed = FALSE;
    if (!(ICursor::Probe(data)->IsClosed(&isClosed), isClosed)) {
        FAIL_RETURN(ICloseable::Probe(data)->Close());
    }
    return NOERROR;
}

ECode CCursorLoader::LoadInBackground(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    AutoLock lock(mCCursorLoaderLock);
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
    //     synchronized(this) {
    //         mCancellationSignal = null;
    //     }
    // }
}

ECode CCursorLoader::CancelLoadInBackground()
{
    AsyncTaskLoader::CancelLoadInBackground();
    AutoLock lock(mCCursorLoaderLock);

    if (NULL != mCancellationSignal) {
        FAIL_RETURN(mCancellationSignal->Cancel());
    }

    return NOERROR;
}

ECode CCursorLoader::IsLoadInBackgroundCanceled(
    /* [out] */ Boolean* isCanceled)
{
    return AsyncTaskLoader::IsLoadInBackgroundCanceled(isCanceled);
}

ECode CCursorLoader::WaitForLoader()
{
    return AsyncTaskLoader::WaitForLoader();
}

ECode CCursorLoader::GetUri(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    *uri = mUri;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CCursorLoader::SetUri(
    /* [in] */ IUri* uri)
{
    mUri = uri;
    return NOERROR;
}

ECode CCursorLoader::GetProjection(
    /* [out, callee] */ ArrayOf<String>** projection)
{
    VALIDATE_NOT_NULL(projection)
    *projection = mProjection;
    REFCOUNT_ADD(*projection);
    return NOERROR;
}

ECode CCursorLoader::SetProjection(
    /* [in] */ ArrayOf<String>* projection)
{
    mProjection = projection;
    return NOERROR;
}

ECode CCursorLoader::GetSelection(
    /* [out] */ String* selection)
{
    VALIDATE_NOT_NULL(selection)
    *selection = mSelection;
    return NOERROR;
}

ECode CCursorLoader::SetSelection(
    /* [in] */ const String& selection)
{
    mSelection = selection;
    return NOERROR;
}

ECode CCursorLoader::GetSelectionArgs(
    /* [out, callee] */ ArrayOf<String>** selectionArgs)
{
    VALIDATE_NOT_NULL(selectionArgs)
    *selectionArgs = mSelectionArgs;
    REFCOUNT_ADD(*selectionArgs);
    return NOERROR;
}

ECode CCursorLoader::SetSelectionArgs(
    /* [in] */ ArrayOf<String>* selectionArgs)
{
    mSelectionArgs = selectionArgs;
    return NOERROR;
}

ECode CCursorLoader::GetSortOrder(
    /* [out] */ String* sortOrder)
{
    VALIDATE_NOT_NULL(sortOrder)
    *sortOrder = mSortOrder;
    return NOERROR;
}

ECode CCursorLoader::SetSortOrder(
    /* [in] */ const String& sortOrder)
{
    mSortOrder = sortOrder;
    return NOERROR;
}

ECode CCursorLoader::OnStartLoading()
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

ECode CCursorLoader::OnStopLoading()
{
    // Attempt to cancel the current load task if possible.
    Boolean ret = FALSE;
    return CancelLoad(&ret);
}

ECode CCursorLoader::OnReset()
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

