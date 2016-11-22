
#ifndef __ELASTOS_DROID_CONTENT_CONTENT_PROVIDER_NATIVE_H__
#define __ELASTOS_DROID_CONTENT_CONTENT_PROVIDER_NATIVE_H__

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IICancellationSignal;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::IIContentObserver;
using Elastos::Droid::Database::IBulkCursorDescriptor;
using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Content {

class ContentProviderProxy
    : public Object
    , public IIContentProvider
    , public IContentProviderProxy
{
public:
    CAR_INTERFACE_DECL()

    ContentProviderProxy(
        /* [in] */ IBinder* binder);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI AsBinder(
        /* [out] */ IBinder** binder);

    CARAPI Query(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [in] */ IICancellationSignal* cancellationSignal,
        /* [out] */ ICursor** cursor);

    CARAPI Query(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [in] */ IIContentObserver* observer,
        /* [in] */ IICancellationSignal* cancellationSignal,
        /* [out] */ IBulkCursorDescriptor** descriptor);

    CARAPI GetType(
        /* [in] */ IUri* uri,
        /* [out] */ String* type);

    CARAPI Insert(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* initialValues,
        /* [out] */ IUri** insertedUri);

    CARAPI BulkInsert(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<IContentValues*>* initialValues,
        /* [out] */ Int32* number);

    CARAPI ApplyBatch(
        /* [in] */ const String& callingPkg,
        /* [in] */ IArrayList* operations,
        /* [out, callee] */ ArrayOf<IContentProviderResult*>** providerResults);

    CARAPI Delete(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* rowsAffected);

    CARAPI Update(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* rowsAffected);

    CARAPI OpenFile(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [in] */ IICancellationSignal* cancellationSignal,
        /* [out] */ IParcelFileDescriptor** fileDescriptor);

    CARAPI OpenAssetFile(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [in] */ IICancellationSignal* cancellationSignal,
        /* [out] */ IAssetFileDescriptor** fileDescriptor);

    CARAPI Call(
        /* [in] */ const String& callingPkg,
        /* [in] */ const String& method,
        /* [in] */ const String& arg,
        /* [in] */ IBundle* extras,
        /* [out] */ IBundle** bundle);

    CARAPI GetStreamTypes(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mimeTypeFilter,
        /* [out, callee] */ ArrayOf<String>** streamTypes);

    CARAPI OpenTypedAssetFile(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [in] */ const String& mimeType,
        /* [in] */ IBundle* opts,
        /* [in] */ IICancellationSignal* cancellationSignal,
        /* [out] */ IAssetFileDescriptor** fileDescriptor);

    CARAPI CreateCancellationSignal(
        /* [out] */ IICancellationSignal** cancellationSignal);

    CARAPI Canonicalize(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [out] */ IUri** result);

    CARAPI Uncanonicalize(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [out] */ IUri** result);

private:
    AutoPtr<IIContentProvider> mRemote;
};

class ContentProviderNative
    : public Object
    , public IIContentProvider
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("ContentProviderNative")

    /**
     * Gets the name of the content provider.
     * Should probably be part of the {@link IContentProvider} interface.
     * @return The content provider name.
     */
    virtual CARAPI GetProviderName(
        /* [out] */ String* name) = 0;

    /**
     * Cast a Binder object into a content resolver interface, generating
     * a proxy if needed.
     */
    static AutoPtr<IIContentProvider> AsInterface(
        /* [in] */ IBinder* binder);

    virtual CARAPI Query(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [in] */ IICancellationSignal* cancellationSignal,
        /* [out] */ ICursor** cursor) = 0;

    CARAPI Query(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [in] */ IIContentObserver* observer,
        /* [in] */ IICancellationSignal* cancellationSignal,
        /* [out] */ IBulkCursorDescriptor** descriptor);
};

} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTENT_CONTENT_PROVIDER_NATIVE_H__
