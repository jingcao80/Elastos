
#ifndef __CCONTENTRESOLVERNATIVE_H__
#define __CCONTENTRESOLVERNATIVE_H__

#include "_CContentResolverNative.h"
#include <jni.h>

using Elastos::Droid::Content::IContentProviderClient;
using Elastos::Droid::Content::IContentResolverOpenResourceIdResult;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IIContentProvider;
using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ICancellationSignal;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CContentResolverNative)
{
public:
    ~CContentResolverNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI ReleaseProvider(
        /* [in] */ IIContentProvider* icp,
        /* [out] */ Boolean* isRelease);

    CARAPI ReleaseUnstableProvider(
        /* [in] */ IIContentProvider* icp,
        /* [out] */ Boolean* isRelease);

    CARAPI UnstableProviderDied(
        /* [in] */ IIContentProvider* icp);

    CARAPI GetType(
        /* [in] */ IUri* uri,
        /* [out] */ String* type);

    CARAPI GetStreamTypes(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mimeTypeFilter,
        /* [out, callee] */ ArrayOf<String>** streamTypes);

    CARAPI Query(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [out] */ ICursor** cursor);

    CARAPI QueryEx(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [in] */ ICancellationSignal* cancellationSignal,
        /* [out] */ ICursor** cursor);

    CARAPI OpenInputStream(
        /* [in] */ IUri* uri,
        /* [out] */ IInputStream** inStream);

    CARAPI OpenOutputStream(
        /* [in] */ IUri* uri,
        /* [out] */ IOutputStream** outStream);

    CARAPI OpenOutputStreamEx(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [out] */ IOutputStream** outStream);

    CARAPI OpenFileDescriptor(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [out] */ IParcelFileDescriptor** fileDescriptor);

    CARAPI OpenAssetFileDescriptor(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [out] */ IAssetFileDescriptor** fileDescriptor);

    CARAPI OpenTypedAssetFileDescriptor(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mimeType,
        /* [in] */ IBundle* opts,
        /* [out] */ IAssetFileDescriptor** fileDescriptor);

    CARAPI GetResourceId(
        /* [in] */ IUri* uri,
        /* [out] */ IContentResolverOpenResourceIdResult** resourceIdResult);

    CARAPI Insert(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [out] */ IUri** insertedUri);

    CARAPI ApplyBatch(
        /* [in] */ const String& authority,
        /* [in] */ IObjectContainer* operations,
        /* [out, callee] */ ArrayOf<Elastos::Droid::Content::IContentProviderResult *>** providerResults);

    CARAPI BulkInsert(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<IContentValues *>* values,
        /* [out] */ Int32* number);

    CARAPI Delete(
        /* [in] */ IUri* uri,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* rowsAffected);

    CARAPI Update(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* rowsAffected);

    CARAPI Call(
        /* [in] */ IUri* uri,
        /* [in] */ const String& method,
        /* [in] */ const String& arg,
        /* [in] */ IBundle* extras,
        /* [out] */ IBundle** bundle);

    CARAPI AcquireProvider(
        /* [in] */ IUri* uri,
        /* [out] */ IIContentProvider** contentProvider);

    CARAPI AcquireExistingProvider(
        /* [in] */ IUri* uri,
        /* [out] */ IIContentProvider** contentProvider);

    CARAPI AcquireProviderEx(
        /* [in] */ const String& name,
        /* [out] */ IIContentProvider** contentProvider);

    CARAPI AcquireUnstableProvider(
        /* [in] */ IUri* uri,
        /* [out] */ IIContentProvider** contentProvider);

    CARAPI AcquireUnstableProviderEx(
        /* [in] */ const String& name,
        /* [out] */ IIContentProvider** contentProvider);

    CARAPI AcquireContentProviderClient(
        /* [in] */ IUri* uri,
        /* [out] */ IContentProviderClient** client);

    CARAPI AcquireContentProviderClientEx(
        /* [in] */ const String& name,
        /* [out] */ IContentProviderClient** client);

    CARAPI AcquireUnstableContentProviderClient(
        /* [in] */ IUri* uri,
        /* [out] */ IContentProviderClient** client);

    CARAPI AcquireUnstableContentProviderClientEx(
        /* [in] */ const String& name,
        /* [out] */ IContentProviderClient** client);

    CARAPI RegisterContentObserver(
        /* [in] */ IUri* uri,
        /* [in] */ Boolean notifyForDescendents,
        /* [in] */ IContentObserver* observer);

    CARAPI RegisterContentObserverEx(
        /* [in] */ IUri* uri,
        /* [in] */ Boolean notifyForDescendents,
        /* [in] */ IContentObserver* observer,
        /* [in] */ Int32 userHandle);

    CARAPI UnregisterContentObserver(
        /* [in] */ IContentObserver* observer);

    CARAPI NotifyChange(
        /* [in] */ IUri* uri,
        /* [in] */ IContentObserver* observer);

    CARAPI NotifyChangeEx(
        /* [in] */ IUri* uri,
        /* [in] */ IContentObserver* observer,
        /* [in] */ Boolean syncToNetwork);

    CARAPI NotifyChangeEx2(
        /* [in] */ IUri* uri,
        /* [in] */ IContentObserver* observer,
        /* [in] */ Boolean syncToNetwork,
        /* [in] */ Int32 userHandle);

    CARAPI StartSync(
        /* [in] */ IUri* uri,
        /* [in] */ IBundle* extras);

    CARAPI CancelSync(
        /* [in] */ IUri* uri);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __CCONTENTRESOLVERNATIVE_H__
