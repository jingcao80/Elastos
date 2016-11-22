
#include "elastos/droid/content/ContentProviderNative.h"
#include "elastos/droid/database/BulkCursorToCursorAdaptor.h"
#include "elastos/droid/database/CCursorToBulkCursorAdaptor.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Database::IBulkCursor;
using Elastos::Droid::Database::BulkCursorToCursorAdaptor;
using Elastos::Droid::Database::ICursorToBulkCursorAdaptor;
using Elastos::Droid::Database::CCursorToBulkCursorAdaptor;
using Elastos::Droid::Database::IIContentObserver;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Content {

//==================================================================================
// ContentProviderProxy
//==================================================================================
CAR_INTERFACE_IMPL_2(ContentProviderProxy, Object, IIContentProvider, IContentProviderProxy)

ContentProviderProxy::ContentProviderProxy(
    /* [in] */ IBinder* binder)
    : mRemote(IIContentProvider::Probe(binder))
{
}

ECode ContentProviderProxy::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    StringBuilder sb("ContentProviderProxy{0x");
    sb += StringUtils::ToHexString((Int32)this);
    sb += ", remote:";
    sb += TO_CSTR(mRemote);
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

ECode ContentProviderProxy::AsBinder(
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder)
    *binder = IBinder::Probe(mRemote);
    REFCOUNT_ADD(*binder);
    return NOERROR;
}

ECode ContentProviderProxy::Query(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [in] */ IICancellationSignal* cancellationSignal,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor)
    *cursor = NULL;

    Logger::D("ContentProviderProxy", " >> Query: %s, uri: %s", callingPkg.string(), TO_CSTR(uri));

    return mRemote->Query(callingPkg, uri, projection, selection, selectionArgs,
        sortOrder, cancellationSignal, cursor);

    // AutoPtr<BulkCursorToCursorAdaptor> adaptor = new BulkCursorToCursorAdaptor();
    // adaptor->constructor();

    // AutoPtr<IIContentObserver> observer;
    // adaptor->GetObserver((IIContentObserver**)&observer);

    // AutoPtr<IBulkCursorDescriptor> descriptor;
    // ECode ec = mRemote->Query(callingPkg, uri, projection, selection, selectionArgs,
    //     sortOrder, observer, cancellationSignal, (IBulkCursorDescriptor**)&descriptor);
    // if (FAILED(ec) || descriptor == NULL) {
    //     Logger::D("ContentProviderProxy", " Error: Failed to Query, ec=%08x", ec);
    //     IBulkCursor::Probe(adaptor)->Close();
    //     return ec;
    // }

    // Logger::D("ContentProviderProxy", " Query: %s", TO_CSTR(descriptor));
    // adaptor->Initialize(descriptor);
    // *cursor = adaptor.Get();
    // REFCOUNT_ADD(*cursor)
    // return NOERROR;
}

ECode ContentProviderProxy::Query(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [in] */ IIContentObserver* observer,
    /* [in] */ IICancellationSignal* cancellationSignal,
    /* [out] */ IBulkCursorDescriptor** descriptor)
{
    return mRemote->Query(callingPkg, uri, projection, selection, selectionArgs,
        sortOrder, observer, cancellationSignal, descriptor);
}

ECode ContentProviderProxy::GetType(
    /* [in] */ IUri* uri,
    /* [out] */ String* type)
{
    return mRemote->GetType(uri, type);
}

ECode ContentProviderProxy::Insert(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* initialValues,
    /* [out] */ IUri** insertedUri)
{
    return mRemote->Insert(callingPkg, uri, initialValues, insertedUri);
}

ECode ContentProviderProxy::BulkInsert(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<IContentValues*>* initialValues,
    /* [out] */ Int32* number)
{
    return mRemote->BulkInsert(callingPkg, uri, initialValues, number);
}

ECode ContentProviderProxy::ApplyBatch(
    /* [in] */ const String& callingPkg,
    /* [in] */ IArrayList* operations,
    /* [out, callee] */ ArrayOf<IContentProviderResult*>** providerResults)
{
    return mRemote->ApplyBatch(callingPkg, operations, providerResults);
}

ECode ContentProviderProxy::Delete(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* uri,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* rowsAffected)
{
    return mRemote->Delete(callingPkg, uri, selection, selectionArgs, rowsAffected);
}

ECode ContentProviderProxy::Update(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* rowsAffected)
{
    return mRemote->Update(callingPkg, uri, values, selection, selectionArgs, rowsAffected);
}

ECode ContentProviderProxy::OpenFile(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [in] */ IICancellationSignal* cancellationSignal,
    /* [out] */ IParcelFileDescriptor** fileDescriptor)
{
    return mRemote->OpenFile(callingPkg, uri, mode, cancellationSignal, fileDescriptor);
}

ECode ContentProviderProxy::OpenAssetFile(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [in] */ IICancellationSignal* cancellationSignal,
    /* [out] */ IAssetFileDescriptor** fileDescriptor)
{
    return mRemote->OpenAssetFile(callingPkg, uri, mode, cancellationSignal, fileDescriptor);
}

ECode ContentProviderProxy::Call(
    /* [in] */ const String& callingPkg,
    /* [in] */ const String& method,
    /* [in] */ const String& arg,
    /* [in] */ IBundle* extras,
    /* [out] */ IBundle** bundle)
{
    return mRemote->Call(callingPkg, method, arg, extras, bundle);
}

ECode ContentProviderProxy::GetStreamTypes(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mimeTypeFilter,
    /* [out, callee] */ ArrayOf<String>** streamTypes)
{
    return mRemote->GetStreamTypes(uri, mimeTypeFilter, streamTypes);
}

ECode ContentProviderProxy::OpenTypedAssetFile(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* uri,
    /* [in] */ const String& mimeType,
    /* [in] */ IBundle* opts,
    /* [in] */ IICancellationSignal* cancellationSignal,
    /* [out] */ IAssetFileDescriptor** fileDescriptor)
{
    return mRemote->OpenTypedAssetFile(callingPkg, uri, mimeType, opts,
        cancellationSignal, fileDescriptor);
}

ECode ContentProviderProxy::CreateCancellationSignal(
    /* [out] */ IICancellationSignal** cancellationSignal)
{
    return mRemote->CreateCancellationSignal(cancellationSignal);
}

ECode ContentProviderProxy::Canonicalize(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* uri,
    /* [out] */ IUri** result)
{
    return mRemote->Canonicalize(callingPkg, uri, result);
}

ECode ContentProviderProxy::Uncanonicalize(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* uri,
    /* [out] */ IUri** result)
{
    return mRemote->Uncanonicalize(callingPkg, uri, result);
}

//==================================================================================
// ContentProviderNative
//==================================================================================
CAR_INTERFACE_IMPL_2(ContentProviderNative, Object, IIContentProvider, IBinder)

AutoPtr<IIContentProvider> ContentProviderNative::AsInterface(
    /* [in] */ IBinder* binder)
{
    if (binder != NULL) {
        IProxy* proxy = (IProxy*)binder->Probe(EIID_IProxy);
        if (proxy != NULL) {
            AutoPtr<IIContentProvider> provider = new ContentProviderProxy(binder);
            return provider;
        }
    }

    return IIContentProvider::Probe(binder);
}

ECode ContentProviderNative::Query(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [in] */ IIContentObserver* observer,
    /* [in] */ IICancellationSignal* cancellationSignal,
    /* [out] */ IBulkCursorDescriptor** descriptor)
{
    VALIDATE_NOT_NULL(descriptor)
    *descriptor = NULL;

    Logger::D("ContentProviderNative", " >> Query: %s, uri: %s", callingPkg.string(), TO_CSTR(uri));
    ECode ec = NOERROR;
    AutoPtr<ICursor> cursor;
    AutoPtr<ICursorToBulkCursorAdaptor> adaptor;

    FAIL_RETURN(Query(callingPkg, uri, projection, selection, selectionArgs,
        sortOrder, cancellationSignal, (ICursor**)&cursor))
    if (cursor != NULL) {
        String name;
        GetProviderName(&name);

        ECode ec = CCursorToBulkCursorAdaptor::New(cursor, observer, name, (ICursorToBulkCursorAdaptor**)&adaptor);
        FAIL_GOTO(ec, _EXIT_)
        cursor = NULL;

        ec = adaptor->GetBulkCursorDescriptor(descriptor);
        FAIL_GOTO(ec, _EXIT_)
        adaptor = NULL;
    }

_EXIT_:
    if (adaptor != NULL) {
        IBulkCursor::Probe(adaptor)->Close();
    }

    if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }

    return ec;
}

}
}
}

