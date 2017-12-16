
#include "elastos/droid/content/IContentProvider.h"
#include "elastos/droid/os/AndroidParcelUtils.h"

using Elastos::Droid::Os::AndroidParcelUtils;

namespace Elastos {
namespace Droid {
namespace Content {

const String IContentProviderProxy2::TAG("IContentProviderProxy2");
const String IContentProviderProxy2::DESCRIPTOR("android.content.IContentProvider");
const Int32 IContentProviderProxy2::CALL_TRANSACTION = android::IBinder::FIRST_CALL_TRANSACTION + 20;

CAR_INTERFACE_IMPL(IContentProviderProxy2, Object, IIContentProvider);

IContentProviderProxy2::IContentProviderProxy2(
    /* [in] */ android::sp<android::IBinder> remote)
    : mRemote(remote)
{}

ECode IContentProviderProxy2::Query(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [in] */ IICancellationSignal* cancellationSignal,
    /* [out] */ ICursor** cursor)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

// internal use
ECode IContentProviderProxy2::Query(
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
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentProviderProxy2::GetType(
    /* [in] */ IUri* uri,
    /* [out] */ String* type)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentProviderProxy2::Insert(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* initialValues,
    /* [out] */ IUri** insertedUri)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentProviderProxy2::BulkInsert(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<IContentValues*>* initialValues,
    /* [out] */ Int32* number)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentProviderProxy2::Delete(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* uri,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* rowsAffected)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentProviderProxy2::Update(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* rowsAffected)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentProviderProxy2::OpenFile(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [in] */ IICancellationSignal* cancellationSignal,
    /* [out] */ IParcelFileDescriptor** fileDescriptor)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentProviderProxy2::OpenAssetFile(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [in] */ IICancellationSignal* cancellationSignal,
    /* [out] */ IAssetFileDescriptor** fileDescriptor)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentProviderProxy2::ApplyBatch(
    /* [in] */ const String& callingPkg,
    /* [in] */ IArrayList* operations,
    /* [out, callee] */ ArrayOf<IContentProviderResult*>** providerResults)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentProviderProxy2::Call(
    /* [in] */ const String& callingPkg,
    /* [in] */ const String& method,
    /* [in] */ const String& arg,
    /* [in] */ IBundle* extras,
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle);

    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));

    AndroidParcelUtils::WriteString(data, callingPkg);
    AndroidParcelUtils::WriteString(data, method);
    AndroidParcelUtils::WriteString(data, arg);
    AndroidParcelUtils::WriteBundle(data, extras);
    mRemote->transact(CALL_TRANSACTION, data, &reply, 0);
    ECode ec = reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
    AutoPtr<IBundle> res = AndroidParcelUtils::ReadBundle(reply);
    *bundle = res;
    REFCOUNT_ADD(*bundle);
    return ec;
}

ECode IContentProviderProxy2::CreateCancellationSignal(
    /* [out] */ IICancellationSignal** cancellationSignal)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentProviderProxy2::Canonicalize(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* uri,
    /* [out] */ IUri** result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentProviderProxy2::Uncanonicalize(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* uri,
    /* [out] */ IUri** result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentProviderProxy2::GetStreamTypes(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mimeTypeFilter,
    /* [out, callee] */ ArrayOf<String>** streamTypes)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentProviderProxy2::OpenTypedAssetFile(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* uri,
    /* [in] */ const String& mimeType,
    /* [in] */ IBundle* opts,
    /* [in] */ IICancellationSignal* cancellationSignal,
    /* [out] */ IAssetFileDescriptor** fileDescriptor)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

} // namespace Content
} // namespace Droid
} // namespace Elastos
