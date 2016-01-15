
#ifndef __ELASTOS_DROID_CONTENT_CCONTENTPROVIDERCLIENT_H__
#define __ELASTOS_DROID_CONTENT_CCONTENTPROVIDERCLIENT_H__

#include "_Elastos_Droid_Content_CContentProviderClient.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ICancellationSignal;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::Runnable;

using Elastos::Core::ICloseGuard;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CContentProviderClient)
    , public Object
    , public IContentProviderClient
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CContentProviderClient();

    virtual ~CContentProviderClient();

    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IIContentProvider* provider,
        /* [in] */ Boolean stable);

    /** {@hide} */
    CARAPI SetDetectNotResponding(
        /* [in] */ Int64 timeoutMillis);

    /** See {@link ContentProvider#query ContentProvider.query} */
    CARAPI Query(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [out] */ ICursor** cursor);

    /** See {@link ContentProvider#query ContentProvider.query} */
    CARAPI Query(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [in] */ ICancellationSignal* cancellationSignal,
        /* [out] */ ICursor** cursor);

    /** See {@link ContentProvider#getType ContentProvider.getType} */
    CARAPI GetType(
        /* [in] */ IUri* uri,
        /* [out] */ String* type);

    /** See {@link ContentProvider#getStreamTypes ContentProvider.getStreamTypes} */
    CARAPI GetStreamTypes(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mimeTypeFilter,
        /* [out, callee] */ ArrayOf<String>** streamTypes);

    /** See {@link ContentProvider#canonicalize} */
    CARAPI Canonicalize(
        /* [in] */ IUri* uri,
        /* [out] */ IUri** result);

    /** See {@link ContentProvider#uncanonicalize} */
    CARAPI Uncanonicalize(
        /* [in] */ IUri* uri,
        /* [out] */ IUri** result);

    /** See {@link ContentProvider#insert ContentProvider.insert} */
    CARAPI Insert(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* initialValues,
        /* [out] */ IUri** insertedUri);

    /** See {@link ContentProvider#bulkInsert ContentProvider.bulkInsert} */
    CARAPI BulkInsert(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<IContentValues *>* initialValues,
        /* [out] */ Int32* pNumber);

    /** See {@link ContentProvider#delete ContentProvider.delete} */
    CARAPI Delete(
        /* [in] */ IUri* uri,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* rowsAffected);

    /** See {@link ContentProvider#update ContentProvider.update} */
    CARAPI Update(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* rowsAffected);

    /**
     * See {@link ContentProvider#openFile ContentProvider.openFile}.  Note that
     * this <em>does not</em>
     * take care of non-content: URIs such as file:.  It is strongly recommended
     * you use the {@link ContentResolver#openFileDescriptor
     * ContentResolver.openFileDescriptor} API instead.
     */
    CARAPI OpenFile(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [out] */ IParcelFileDescriptor** fileDescriptor);

    /**
     * See {@link ContentProvider#openFile ContentProvider.openFile}.  Note that
     * this <em>does not</em>
     * take care of non-content: URIs such as file:.  It is strongly recommended
     * you use the {@link ContentResolver#openFileDescriptor
     * ContentResolver.openFileDescriptor} API instead.
     */
    CARAPI OpenFile(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [in] */ ICancellationSignal* cancellationSignal,
        /* [out] */ IParcelFileDescriptor** fileDescriptor);

    /**
     * See {@link ContentProvider#openAssetFile ContentProvider.openAssetFile}.
     * Note that this <em>does not</em>
     * take care of non-content: URIs such as file:.  It is strongly recommended
     * you use the {@link ContentResolver#openAssetFileDescriptor
     * ContentResolver.openAssetFileDescriptor} API instead.
     */
    CARAPI OpenAssetFile(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [out] */ IAssetFileDescriptor** fileDescriptor);

    /**
     * See {@link ContentProvider#openAssetFile ContentProvider.openAssetFile}.
     * Note that this <em>does not</em>
     * take care of non-content: URIs such as file:.  It is strongly recommended
     * you use the {@link ContentResolver#openAssetFileDescriptor
     * ContentResolver.openAssetFileDescriptor} API instead.
     */
    CARAPI OpenAssetFile(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [in] */ ICancellationSignal* cancellationSignal,
        /* [out] */ IAssetFileDescriptor** fileDescriptor);


    /** See {@link ContentProvider#openTypedAssetFile ContentProvider.openTypedAssetFile} */
    CARAPI OpenTypedAssetFileDescriptor(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mimeType,
        /* [in] */ IBundle* opts,
        /* [out] */ IAssetFileDescriptor** fileDescriptor);

    /** See {@link ContentProvider#openTypedAssetFile ContentProvider.openTypedAssetFile} */
    CARAPI OpenTypedAssetFileDescriptor(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mimeType,
        /* [in] */ IBundle* opts,
        /* [in] */ ICancellationSignal* cancellationSignal,
        /* [out] */ IAssetFileDescriptor** fileDescriptor);


    /** See {@link ContentProvider#applyBatch ContentProvider.applyBatch} */
    CARAPI ApplyBatch(
        /* [in] */ IArrayList* operations,
        /* [out, callee] */ ArrayOf<IContentProviderResult *>** providerResults);

    /** See {@link ContentProvider#call(String, String, Bundle)} */
    CARAPI Call(
        /* [in] */ const String& method,
        /* [in] */ const String& arg,
        /* [in] */ IBundle* extras,
        /* [out] */ IBundle** bundle);

    /**
     * Call this to indicate to the system that the associated {@link ContentProvider} is no
     * longer needed by this {@link ContentProviderClient}.
     * @return true if this was release, false if it was already released
     */
    CARAPI ReleaseProvider(
        /* [out] */ Boolean* isRelease);

    /**
     * Get a reference to the {@link ContentProvider} that is associated with this
     * client. If the {@link ContentProvider} is running in a different process then
     * null will be returned. This can be used if you know you are running in the same
     * process as a provider, and want to get direct access to its implementation details.
     *
     * @return If the associated {@link ContentProvider} is local, returns it.
     * Otherwise returns null.
     */
    CARAPI GetLocalContentProvider(
        /* [out] */ IContentProvider** localContentProvider);

    /** {@hide} */
    static CARAPI ReleaseQuietly(
        /* [in] */ IContentProviderClient* client);

private:
    CARAPI BeforeRemote();

    CARAPI AfterRemote();

private:

    class NotRespondingRunnable : public Runnable {
    public:
        NotRespondingRunnable(
            /* [in] */ IWeakReference* host);

        CARAPI Run();
    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

private:
    friend class NotRespondingRunnable;

    static const String TAG;

    static AutoPtr<IHandler> sAnrHandler;

    AutoPtr<IIContentProvider> mContentProvider;
    AutoPtr<IContentResolver> mContentResolver;
    String mPackageName;
    Boolean mStable;

    AutoPtr<ICloseGuard> mGuard;

    Int64 mAnrTimeout;
    AutoPtr<IRunnable> mAnrRunnable;

    Boolean mReleased;
    static Object mContentProviderClientLock;
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CCONTENTPROVIDERCLIENT_H__
