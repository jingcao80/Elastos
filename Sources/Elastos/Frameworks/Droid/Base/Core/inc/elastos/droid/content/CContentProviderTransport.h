
#ifndef __ELASTOS_DROID_CONTENT_CCONTENTPROVIDERTRANSPORT_H__
#define __ELASTOS_DROID_CONTENT_CCONTENTPROVIDERTRANSPORT_H__

#include "Elastos.Droid.App.h"
#include "_Elastos_Droid_Content_CContentProviderTransport.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IICancellationSignal;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CContentProviderTransport)
    , public Object
    , public IContentProviderTransport
    , public IIContentProvider
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CContentProviderTransport();

    CARAPI constructor(
        /* [in] */ IContentProvider* owner);

    CARAPI GetContentProvider(
        /* [out] */ IContentProvider** provider);

    CARAPI GetProviderName(
        /* [out] */ String* name);

    CARAPI Query(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [in] */ IICancellationSignal* cancellationSignal,
        /* [out] */ ICursor** cursor);

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

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI EnforceFilePermission(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode);

    CARAPI EnforceReadPermission(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [out] */ Int32* result);

    CARAPI EnforceWritePermission(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [out] */ Int32* result);

private:
    friend class  ContentProvider;

    AutoPtr<IWeakReference> mWeakContentProvider;
    //ContentProvider* mContentProvider;

    AutoPtr<IAppOpsManager> mAppOpsManager;
    Int32 mReadOp;// = AppOpsManager.OP_NONE;
    Int32 mWriteOp;// = AppOpsManager.OP_NONE;

};

} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTENT_CCONTENTPROVIDERTRANSPORT_H__
