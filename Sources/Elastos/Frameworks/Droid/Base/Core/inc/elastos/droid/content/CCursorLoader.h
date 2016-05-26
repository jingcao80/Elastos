
#ifndef __ELASTOS_DROID_CONTENT_CCURSORLOADER_H__
#define __ELASTOS_DROID_CONTENT_CCURSORLOADER_H__

#include "_Elastos_Droid_Content_CCursorLoader.h"
#include "elastos/droid/content/AsyncTaskLoader.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Os::ICancellationSignal;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CCursorLoader)
    , public AsyncTaskLoader
    , public ICursorLoader
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CCursorLoader();

    virtual ~CCursorLoader();

    /**
     * Creates an empty unspecified CursorLoader.  You must follow this with
     * calls to {@link #setUri(Uri)}, {@link #setSelection(String)}, etc
     * to specify the query to perform.
     */
    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Creates a fully-specified CursorLoader.  See
     * {@link ContentResolver#query(Uri, String[], String, String[], String)
     * ContentResolver.query()} for documentation on the meaning of the
     * parameters.  These will be passed as-is to that call.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder);

    CARAPI DeliverResult(
        /* [in] */ IInterface* data);

    CARAPI DeliverCancellation();

    CARAPI GetContext(
        /* [out] */ IContext** context);

    CARAPI GetId(
        /* [out] */ Int32* id);

    CARAPI RegisterListener(
        /* [in] */ Int32 id,
        /* [in] */ ILoaderOnLoadCompleteListener* listener);

    CARAPI UnregisterListener(
        /* [in] */ ILoaderOnLoadCompleteListener* listener);

    CARAPI RegisterOnLoadCanceledListener(
        /* [in] */ ILoaderOnLoadCanceledListener* listener);

    CARAPI UnregisterOnLoadCanceledListener(
        /* [in] */ ILoaderOnLoadCanceledListener* listener);

    CARAPI IsStarted(
        /* [out] */ Boolean* isStarted);

    CARAPI IsAbandoned(
        /* [out] */ Boolean* isAbandoned);

    CARAPI IsReset(
        /* [out] */ Boolean* isReset);

    CARAPI StartLoading();

    CARAPI CancelLoad(
        /* [out] */ Boolean* succeeded);

    CARAPI ForceLoad();

    CARAPI StopLoading();

    CARAPI Abandon();

    CARAPI Reset();

    CARAPI TakeContentChanged(
        /* [out] */ Boolean* succeeded);

    CARAPI OnContentChanged();

    CARAPI DataToString(
        /* [in] */ IInterface* data,
        /* [out] */ String* str);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI Dump(
        /* [in] */ const String& prefix,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* writer,
        /* [in] */ ArrayOf<String>* args);

    CARAPI SetUpdateThrottle(
        /* [in] */ Int64 delayMS);

    CARAPI OnCanceled(
        /* [in] */ IInterface* data);

    /* Runs on a worker thread */
    CARAPI LoadInBackground(
        /* [out] */ IInterface** result);

    CARAPI CancelLoadInBackground();

    CARAPI IsLoadInBackgroundCanceled(
        /* [out] */ Boolean* isCanceled);

    CARAPI WaitForLoader();

    CARAPI GetUri(
        /* [out] */ IUri** uri);

    CARAPI SetUri(
        /* [in] */ IUri* uri);

    CARAPI GetProjection(
        /* [out, callee] */ ArrayOf<String>** projection);

    CARAPI SetProjection(
        /* [in] */ ArrayOf<String>* projection);

    CARAPI GetSelection(
        /* [out] */ String* selection);

    CARAPI SetSelection(
        /* [in] */ const String& selection);

    CARAPI GetSelectionArgs(
        /* [out, callee] */ ArrayOf<String>** selectionArgs);

    CARAPI SetSelectionArgs(
        /* [in] */ ArrayOf<String>* selectionArgs);

    CARAPI GetSortOrder(
        /* [out] */ String* sortOrder);

    CARAPI SetSortOrder(
        /* [in] */ const String& sortOrder);

protected:
    CARAPI OnStartLoading();

    /**
     * Must be called from the UI thread
     */
    CARAPI OnStopLoading();

    CARAPI OnReset();

private:
    AutoPtr<ILoaderForceLoadContentObserver> mObserver;
    AutoPtr<IUri> mUri;
    AutoPtr<ArrayOf<String> > mProjection;
    String mSelection;
    AutoPtr<ArrayOf<String> > mSelectionArgs;
    String mSortOrder;
    AutoPtr<ICursor> mCursor;
    AutoPtr<ICancellationSignal> mCancellationSignal;
    Object mCCursorLoaderLock;

};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CCURSORLOADER_H__
