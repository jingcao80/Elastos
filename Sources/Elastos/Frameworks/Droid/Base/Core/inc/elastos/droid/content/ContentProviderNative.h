
#ifndef __ELASTOS_DROID_CONTENT_CONTENTPROVIDERNATIVE_H__
#define __ELASTOS_DROID_CONTENT_CONTENTPROVIDERNATIVE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "IIContentProviderConstants.h"
//***#include "elastos/droid/os/Binder.h"

using namespace Elastos;
using namespace Elastos::Core;
using namespace Elastos::IO;
using namespace Elastos::Droid::Os;
using namespace Elastos::Droid::Database;
using namespace Elastos::Droid::Content::Res;
using namespace Elastos::Droid::Content::Pm;

namespace Elastos {
namespace Droid {
namespace Content {

class ContentProviderNative
    : public ElRefBase
    , public IObject
//***    , public Binder
    , public IContentProviderNative
{
public:
    class ContentProviderProxy: public ElRefBase, public IIContentProvider
    {
    public:
        ContentProviderProxy(IIBinder* binder);

        ~ContentProviderProxy();

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI AsBinder(
            /* [out] */ IIBinder** binder);

        CARAPI Query(
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
            /* [in] */ IUri* uri,
            /* [in] */ IContentValues* initialValues,
            /* [out] */ IUri** insertedUri);

        CARAPI BulkInsert(
            /* [in] */ IUri* uri,
            /* [in] */ ArrayOf<IContentValues*>* initialValues,
            /* [out] */ Int32* number);

        CARAPI ApplyBatch(
            /* [in] */ IObjectContainer* operations,
            /* [out, callee] */ ArrayOf<IContentProviderResult*>** providerResults);

        CARAPI Delete(
            /* [in] */ IUri* uri,
            /* [in] */ const String& selection,
            /* [in] */ ArrayOf<String>* selectionArgs,
            /* [out] */ Int32* rowsAffected);

        CARAPI Update(
            /* [in] */ IUri* uri,
            /* [in] */ IContentValues* values,
            /* [in] */ const String& selection,
            /* [in] */ ArrayOf<String>* selectionArgs,
            /* [out] */ Int32* rowsAffected);

        CARAPI OpenFile(
            /* [in] */ IUri* uri,
            /* [in] */ const String& mode,
            /* [out] */ IParcelFileDescriptor** fileDescriptor);

        CARAPI OpenAssetFile(
            /* [in] */ IUri* uri,
            /* [in] */ const String& mode,
            /* [out] */ IAssetFileDescriptor** fileDescriptor);

        CARAPI Call(
            /* [in] */ const String& method,
            /* [in] */ const String& request,
            /* [in] */ IBundle* args,
            /* [out] */ IBundle** bundle);

        CARAPI GetStreamTypes(
            /* [in] */ IUri* uri,
            /* [in] */ const String& mimeTypeFilter,
            /* [out, callee] */ ArrayOf<String>** streamTypes);

        CARAPI OpenTypedAssetFile(
            /* [in] */ IUri* uri,
            /* [in] */ const String& mimeType,
            /* [in] */ IBundle* opts,
            /* [out] */ IAssetFileDescriptor** fileDescriptor);

        CARAPI CreateCancellationSignal(
            /* [out] */ IICancellationSignal** cancellationSignal);

    private:
        AutoPtr<IIBinder> mRemote;

    };

public:

    ContentProviderNative();

    virtual ~ContentProviderNative();

    CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    CARAPI Aggregate(
        /* [in] */ AggrType aggrType,
        /* [in] */ PInterface pObject);

    CARAPI GetDomain(
        /* [out] */ PInterface *ppObject);

    CARAPI GetClassID(
        /* [out] */ ClassID *pCLSID);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean * result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI ToString(
        /* [out] */ String* info);

    /**
     * Cast a Binder object into a content resolver interface, generating
     * a proxy if needed.
     */
    static CARAPI AsInterface(
        /* [in] */ IIBinder* obj,
        /* [out] */ IIContentProvider** contentProvider);

    /**
     * Gets the name of the content provider.
     * Should probably be part of the {@link IContentProvider} interface.
     * @return The content provider name.
     */
    virtual CARAPI GetProviderName(
        /* [out] */ String* name) = 0;

    CARAPI OnTransact(
        /* [in] */ Int32 code,
        /* [in] */ IParcel* data,
        /* [in] */ IParcel* reply,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* result);

    CARAPI AsBinder(
        /* [out] */ IIBinder** binder);

    virtual CARAPI Query(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [in] */ IICancellationSignal* cancellationSignal,
        /* [out] */ ICursor** cursor) = 0;

    virtual CARAPI GetType(
        /* [in] */ IUri* uri,
        /* [out] */ String* type) = 0;

    virtual CARAPI Insert(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* initialValues,
        /* [out] */ IUri** insertedUri) = 0;

    virtual CARAPI BulkInsert(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<IContentValues*>* initialValues,
        /* [out] */ Int32* number) = 0;

    virtual CARAPI Delete(
        /* [in] */ IUri* uri,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* rowsAffected) = 0;

    virtual CARAPI Update(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* rowsAffected) = 0;

    virtual CARAPI OpenFile(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [out] */ IParcelFileDescriptor** fileDescriptor) = 0;

    virtual CARAPI OpenAssetFile(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [out] */ IAssetFileDescriptor** fileDescriptor) = 0;

    virtual CARAPI ApplyBatch(
        /* [in] */ IObjectContainer* operations,
        /* [out, callee] */ ArrayOf<IContentProviderResult*>** providerResults) = 0;

    virtual CARAPI Call(
        /* [in] */ const String& method,
        /* [in] */ const String& arg,
        /* [in] */ IBundle* extras,
        /* [out] */ IBundle** bundle) = 0;

    virtual CARAPI CreateCancellationSignal(
        /* [out] */ IICancellationSignal** cancellationSignal) = 0;

    virtual CARAPI GetStreamTypes(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mimeTypeFilter,
        /* [out, callee] */ ArrayOf<String>** streamTypes) = 0;

    virtual CARAPI OpenTypedAssetFile(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mimeType,
        /* [in] */ IBundle* opts,
        /* [out] */ IAssetFileDescriptor** fileDescriptor) = 0;

private:
    CARAPI CreateUri(
        /* [in] */ IParcel* data,
        /* [in] */ IParcel* reply,
        /* [out] */ IUri** url,
        /* [out] */ Boolean* result);
};

}
}
}

#endif //__ELASTOS_DROID_CONTENT_CONTENTPROVIDERNATIVE_H__
