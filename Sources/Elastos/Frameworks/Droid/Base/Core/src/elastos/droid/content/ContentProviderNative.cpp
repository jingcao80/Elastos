
#include "elastos/droid/content/ContentProviderNative.h"
//#include "elastos/droid/content/CContentProviderOperation.h"
//#include "elastos/droid/content/CContentProviderResult.h"
//***#include "elastos/droid/content/res/CAssetFileDescriptor.h"
//***#include "elastos/droid/database/CBulkCursorToCursorAdaptor.h"
//***#include "elastos/droid/database/CCursorToBulkCursorAdaptor.h"
//***#include "elastos/droid/database/DatabaseUtils.h"
//***#include "elastos/droid/net/CStringUri.h"
//***#include "elastos/droid/net/COpaqueUri.h"
//***#include "elastos/droid/net/CHierarchicalUri.h"
//***#include "elastos/droid/net/CPart.h"
//***#include "elastos/droid/net/CPathPart.h"
//***#include "utils/CParcelableObjectContainer.h"

namespace Elastos {
namespace Droid {
namespace Content {

#define FAILED_WITH_RETURN(ec, reply, result) do { \
                    ECode e = ec; \
                 if (FAILED(e)) {   \
                    /**DatabaseUtils::WriteExceptionToParcel(reply, ec); */ \
                     *result = TRUE; \
                     return e; \
                 } \
            } while (0);

ContentProviderNative::ContentProviderProxy::ContentProviderProxy(
    /* [in] */ IIBinder* binder)
    : mRemote(binder)
{}

ContentProviderNative::ContentProviderProxy::~ContentProviderProxy()
{}

PInterface ContentProviderNative::ContentProviderProxy::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IIContentProvider*)this;
    }
    else if (riid == EIID_IObject) {
        return (IObject*)this;
    }
    if (riid == EIID_IIContentProvider) {
        return (IIContentProvider *)this;
    }

    return NULL;
}

UInt32 ContentProviderNative::ContentProviderProxy::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 ContentProviderNative::ContentProviderProxy::Release()
{
    return ElRefBase::Release();
}

ECode ContentProviderNative::ContentProviderProxy::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (NULL == pIID) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface *)(IIContentProvider *)this) {
        *pIID = EIID_IIContentProvider;
        return NOERROR;
    } else if (pObject == (IInterface *)(IObject *)this) {
        *pIID = EIID_IObject;
        return NOERROR;
    }

    return E_INVALID_ARGUMENT;
}

ECode ContentProviderNative::ContentProviderProxy::AsBinder(
    /* [out] */ IIBinder** binder)
{
    VALIDATE_NOT_NULL(binder);
    *binder = mRemote;
    REFCOUNT_ADD(*binder);
    return NOERROR;
}

ECode ContentProviderNative::ContentProviderProxy::Query(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [in] */ IICancellationSignal* cancellationSignal,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor);
    AutoPtr<IBulkCursorToCursorAdaptor> adaptor;
    FAIL_RETURN(CBulkCursorToCursorAdaptor::New((IBulkCursorToCursorAdaptor**)&adaptor));
    AutoPtr<IParcel> data, reply; // Parcel.obtain();
    AutoPtr<IParcel> reply; // Parcel.obtain();
    CParcel::New((IParcel**)&data);
    CParcel::New((IParcel**)&reply);
//    try {
    assert(0 && "TODO");
//        data.writeInterfaceToken(IContentProvider.descriptor);
    AutoPtr<IParcelable> parcelable = (IParcelable*) uri->Probe(EIID_IParcelable);
    FAIL_RETURN(parcelable->WriteToParcel(reply));
    Int32 length = 0;
    if (NULL != projection) length = projection->GetLength();
    FAIL_RETURN(data->WriteInt32(length));

    for (Int32 i = 0; i < length; i++) {
        FAIL_RETURN(data->WriteString((*projection)[i]));
    }

    FAIL_RETURN(data->WriteString(selection));

    if (NULL != selectionArgs) {
        length = selectionArgs->GetLength();
    }
    else {
        length = 0;
    }

    FAIL_RETURN(data->WriteInt32(length));

    for (Int32 i = 0; i < length; i++) {
        FAIL_RETURN(data->WriteString((*selectionArgs)[i]));
    }

    FAIL_RETURN(data->WriteString(sortOrder));
//        data.writeStrongBinder(adaptor.getObserver().asBinder());
//        data.writeStrongBinder(cancellationSignal != null ? cancellationSignal.asBinder() : null);
    Boolean ret = FALSE;
    FAIL_RETURN(mRemote->Transact(IIContentProviderConstants::QUERY_TRANSACTION, data, reply, 0, &ret));
//***    FAIL_RETURN(DatabaseUtils::ReadExceptionFromParcel(reply));
    Int32 number = 0;
    FAIL_RETURN(reply->ReadInt32(&number));

    if (number != 0) {
//         AutoPtr<IBulkCursorDescriptor> d;
// //***        FAIL_RETURN(CBulkCursorDescriptor::New((IBulkCursorDescriptor**)&d));
//         AutoPtr<IParcelable> bulkCursorParcelable = (IParcelable*) d->Probe(EIID_IParcelable);
//         FAIL_RETURN(bulkCursorParcelable->ReadFromParcel(reply));
//         FAIL_RETURN(adaptor->Initialize(d));
    }
    else {
        FAIL_RETURN(adaptor->Close());
        adaptor = NULL;
    }

    *cursor = adaptor;
    REFCOUNT_ADD(*cursor);
    return NOERROR;
//    } catch (RemoteException ex) {
//        adaptor.close();
//        throw ex;
//    } catch (RuntimeException ex) {
//        adaptor.close();
//        throw ex;
//    } finally {
//        data.recycle();
//        reply.recycle();
//    }
}

ECode ContentProviderNative::ContentProviderProxy::GetType(
    /* [in] */ IUri* uri,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(uri);
    VALIDATE_NOT_NULL(str);
    AutoPtr<IParcel> data; //Parcel.obtain();
    AutoPtr<IParcel> reply; //Parcel.obtain();
//    data.writeInterfaceToken(IContentProvider.descriptor);
    AutoPtr<IParcelable> parcelable = (IParcelable*) uri->Probe(EIID_IParcelable);
    FAIL_RETURN(parcelable->WriteToParcel(data));
    Boolean ret = FALSE;
    FAIL_RETURN(mRemote->Transact(IIContentProviderConstants::GET_TYPE_TRANSACTION, data, reply, 0, &ret));
//***    FAIL_RETURN(DatabaseUtils::ReadExceptionFromParcel(reply));
    FAIL_RETURN(reply->ReadString(str));
//    data.recycle();
//    reply.recycle();
    return NOERROR;
}

ECode ContentProviderNative::ContentProviderProxy::Insert(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* initialValues,
    /* [out] */ IUri** insertUri)
{
    VALIDATE_NOT_NULL(uri);
    VALIDATE_NOT_NULL(initialValues);
    VALIDATE_NOT_NULL(insertUri);
    AutoPtr<IParcel> data;    //Parcel.obtain();
    AutoPtr<IParcel> reply;    //Parcel.obtain();
//    data.writeInterfaceToken(IContentProvider.descriptor);
    AutoPtr<IParcelable> uriParcelable = (IParcelable*) uri->Probe(EIID_IParcelable);
    FAIL_RETURN(uriParcelable->WriteToParcel(data));
    AutoPtr<IParcelable> valuesParcelable = (IParcelable*) initialValues->Probe(EIID_IParcelable);
    FAIL_RETURN(valuesParcelable->WriteToParcel(data));
    Boolean ret = FALSE;
    mRemote->Transact(IIContentProviderConstants::INSERT_TRANSACTION, data, reply, 0, &ret);
//***    FAIL_RETURN(DatabaseUtils::ReadExceptionFromParcel(reply));
    Int32 type = 0;
    FAIL_RETURN(reply->ReadInt32(&type));
    String str1;

    if (1 == type) {
        FAIL_RETURN(reply->ReadString(&str1));
//***        FAIL_RETURN(CStringUri::New(str1, insertUri));
    }
    else if (2 == type) {
        FAIL_RETURN(reply->ReadString(&str1));
//***        AutoPtr<IPart> part1;
//***        FAIL_RETURN(Part::ReadFrom(reply, (IPart**)&part1));
//***        AutoPtr<IPart> part2;
//***        FAIL_RETURN(Part::ReadFrom(reply, (IPart**)&part2));
//***        FAIL_RETURN(COpaqueUri::New(str1, part1, part2, insertUri));
    }
    else if (3 == type) {
        FAIL_RETURN(reply->ReadString(&str1));
//***        AutoPtr<IPart> part1;
//***        FAIL_RETURN(Part::ReadFrom(reply, (IPart**)&part1));
//***        AutoPtr<IPathPart> pathPart1;
//***        FAIL_RETURN(PathPart::ReadFrom(reply, (IPathPart**)&pathPart1));
//***        AutoPtr<IPart> part2;
//***        FAIL_RETURN(Part::ReadFrom(reply, (IPart**)&part2));
 //***       AutoPtr<IPart> part3;
//***        FAIL_RETURN(Part::ReadFrom(reply, (IPart**)&part3));
//***        FAIL_RETURN(CHierarchicalUri::New(str1, part1, pathPart1, part2, part3, insertUri));
    }

//    data.recycle();
//    reply.recycle();
    return NOERROR;
}

ECode ContentProviderNative::ContentProviderProxy::BulkInsert(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<IContentValues*>* initialValues,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(uri);
    VALIDATE_NOT_NULL(number);
    AutoPtr<IParcel> data;    //Parcel.obtain();
    AutoPtr<IParcel> reply;    //Parcel.obtain();
//    data.writeInterfaceToken(IContentProvider.descriptor);
    AutoPtr<IParcelable> uriParcelable = (IParcelable*) uri->Probe(EIID_IParcelable);
    FAIL_RETURN(uriParcelable->WriteToParcel(data));

    if (NULL != initialValues) {
        Int32 N = initialValues->GetLength();
        FAIL_RETURN(data->WriteInt32(N));

        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IContentValues> item = (*initialValues)[i];
            if (NULL != item) {
                FAIL_RETURN(data->WriteInt32(1));
                AutoPtr<IParcelable> itemParcelable = (IParcelable*) item->Probe(EIID_IParcelable);
                FAIL_RETURN(itemParcelable->WriteToParcel(data));
            }
            else {
                FAIL_RETURN(data->WriteInt32(0));
            }
        }

    }
    else {
        FAIL_RETURN(data->WriteInt32(-1));
    }

    Boolean ret = FALSE;
    mRemote->Transact(IIContentProviderConstants::BULK_INSERT_TRANSACTION, data, reply, 0, &ret);
//***    FAIL_RETURN(DatabaseUtils::ReadExceptionFromParcel(reply));
    FAIL_RETURN(reply->ReadInt32(number));
//    data.recycle();
//    reply.recycle();
    return NOERROR;
}

ECode ContentProviderNative::ContentProviderProxy::ApplyBatch(
    /* [in] */ IObjectContainer* operations,
    /* [out, callee] */ ArrayOf<IContentProviderResult*>** providerResults)
{
    VALIDATE_NOT_NULL(operations);
    VALIDATE_NOT_NULL(providerResults);
    AutoPtr<IParcel> data;    //Parcel.obtain();
    AutoPtr<IParcel> reply;    //Parcel.obtain();
//    data.writeInterfaceToken(IContentProvider.descriptor);
    Int32 count;
    FAIL_RETURN(operations->GetObjectCount(&count));
    FAIL_RETURN(data->WriteInt32(count));
    Boolean hasNext;
    AutoPtr<IContentProviderOperation> operation;
    AutoPtr<IObjectEnumerator> ObjEnumerator;
    FAIL_RETURN(operations->GetObjectEnumerator((IObjectEnumerator**)&ObjEnumerator));

    while ((ObjEnumerator->MoveNext(&hasNext), hasNext)) {
        operation = NULL;
        FAIL_RETURN(ObjEnumerator->Current((IInterface**)&operation));
        AutoPtr<IParcelable> operationParcelable = (IParcelable*) operation->Probe(EIID_IParcelable);
        FAIL_RETURN(operationParcelable->WriteToParcel(data));
    }

    Boolean ret = FALSE;
    FAIL_RETURN(mRemote->Transact(IIContentProviderConstants::APPLY_BATCH_TRANSACTION, data, reply, 0, &ret));
//***    FAIL_RETURN(DatabaseUtils::ReadExceptionWithOperationApplicationExceptionFromParcel(reply));
    Int32 N = 0;
    Int32 value = 0;
    FAIL_RETURN(reply->ReadInt32(&N));

    if (N < 0) {
        *providerResults = NULL;
        return NOERROR;
    }

    AutoPtr<IContentProviderResult> tmpProviderResult;
    *providerResults = ArrayOf<IContentProviderResult*>::Alloc(N);
    REFCOUNT_ADD(*providerResults);

    for (Int32 i = 0; i < N; i++) {
        FAIL_RETURN(reply->ReadInt32(&value));
        if (0 != value) {
            tmpProviderResult = NULL;
            FAIL_RETURN(CContentProviderResult::New(reply, (IContentProviderResult**)&tmpProviderResult));
            (*providerResults)->Set(i, tmpProviderResult);
        }
    }

//    data.recycle();
//    reply.recycle();
    return NOERROR;
}

ECode ContentProviderNative::ContentProviderProxy::Delete(
    /* [in] */ IUri* uri,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* rowsAffected)
{
    VALIDATE_NOT_NULL(uri);
    VALIDATE_NOT_NULL(rowsAffected);
    AutoPtr<IParcel> data;    //Parcel.obtain();
    AutoPtr<IParcel> reply;    //Parcel.obtain();
//    data.writeInterfaceToken(IContentProvider.descriptor);
    AutoPtr<IParcelable> uriParcelable = (IParcelable*) uri->Probe(EIID_IParcelable);
    FAIL_RETURN(uriParcelable->WriteToParcel(data));
    FAIL_RETURN(data->WriteString(selection));
    FAIL_RETURN(data->WriteArrayOfString(selectionArgs));
    Boolean ret = FALSE;
    FAIL_RETURN(mRemote->Transact(IIContentProviderConstants::DELETE_TRANSACTION, data, reply, 0, &ret));
//***    FAIL_RETURN(DatabaseUtils::ReadExceptionFromParcel(reply));
    FAIL_RETURN(reply->ReadInt32(rowsAffected));
//    data.recycle();
//    reply.recycle();
    return NOERROR;
}

ECode ContentProviderNative::ContentProviderProxy::Update(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(uri);
    VALIDATE_NOT_NULL(values);
    VALIDATE_NOT_NULL(number);
    AutoPtr<IParcel> data;    //Parcel.obtain();
    AutoPtr<IParcel> reply;    //Parcel.obtain();
//    data.writeInterfaceToken(IContentProvider.descriptor);
    AutoPtr<IParcelable> uriParcelable = (IParcelable*) uri->Probe(EIID_IParcelable);
    FAIL_RETURN(uriParcelable->WriteToParcel(data));
    AutoPtr<IParcelable> valuesParcelable = (IParcelable*) values->Probe(EIID_IParcelable);
    FAIL_RETURN(valuesParcelable->WriteToParcel(data));
    FAIL_RETURN(data->WriteString(selection));
    FAIL_RETURN(data->WriteArrayOfString(selectionArgs));
    Boolean ret = FALSE;
    FAIL_RETURN(mRemote->Transact(IIContentProviderConstants::UPDATE_TRANSACTION, data, reply, 0, &ret));
//***    FAIL_RETURN(DatabaseUtils::ReadExceptionFromParcel(reply));
    FAIL_RETURN(reply->ReadInt32(number));
//    data.recycle();
//    reply.recycle();
    return NOERROR;
}

ECode ContentProviderNative::ContentProviderProxy::OpenFile(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [out] */ IParcelFileDescriptor** fileDescriptor)
{
    VALIDATE_NOT_NULL(uri);
    VALIDATE_NOT_NULL(fileDescriptor);
    AutoPtr<IParcel> data;    //Parcel.obtain();
    AutoPtr<IParcel> reply;    //Parcel.obtain();
//    data.writeInterfaceToken(IContentProvider.descriptor);
    AutoPtr<IParcelable> uriParcelable = (IParcelable*) uri->Probe(EIID_IParcelable);
    FAIL_RETURN(uriParcelable->WriteToParcel(data));
    FAIL_RETURN(data->WriteString(mode));
    Boolean ret = FALSE;
    FAIL_RETURN(mRemote->Transact(IIContentProviderConstants::OPEN_FILE_TRANSACTION, data, reply, 0, &ret));
//***    FAIL_RETURN(DatabaseUtils::ReadExceptionWithFileNotFoundExceptionFromParcel(reply));
    Int32 has = 0;
    FAIL_RETURN(reply->ReadInt32(&has));
    Int32 fdInt = 0;

    if (has != 0) {
        FAIL_RETURN(reply->ReadFileDescriptor(&fdInt));
        AutoPtr<IFileDescriptor> fileDes;
//***        FAIL_RETURN(CFileDescriptor::New(fdInt, (IFileDescriptor**)&fileDes));
        AutoPtr<IParcelFileDescriptor> fd;
//***        FAIL_RETURN(CFileDescriptor::New(fileDes, (IParcelFileDescriptor**)&fd));
        *fileDescriptor = fd;
        REFCOUNT_ADD(*fileDescriptor);
    }
    else {
        *fileDescriptor = NULL;
    }

//    data.recycle();
//    reply.recycle();
    return NOERROR;
}

ECode ContentProviderNative::ContentProviderProxy::OpenAssetFile(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [out] */ IAssetFileDescriptor** fileDescriptor)
{
    VALIDATE_NOT_NULL(uri);
    VALIDATE_NOT_NULL(fileDescriptor);
    AutoPtr<IParcel> data;    //Parcel.obtain();
    AutoPtr<IParcel> reply;    //Parcel.obtain();
//    data.writeInterfaceToken(IContentProvider.descriptor);
    AutoPtr<IParcelable> uriParcelable = (IParcelable*) uri->Probe(EIID_IParcelable);
    FAIL_RETURN(uriParcelable->WriteToParcel(data));
    FAIL_RETURN(data->WriteString(mode));
    Boolean ret = FALSE;
    FAIL_RETURN(mRemote->Transact(IIContentProviderConstants::OPEN_ASSET_FILE_TRANSACTION, data, reply, 0, &ret));
//***    FAIL_RETURN(DatabaseUtils::ReadExceptionWithFileNotFoundExceptionFromParcel(reply));
    Int32 has = 0;
    FAIL_RETURN(reply->ReadInt32(&has));

    if (has != 0) {
//***        FAIL_RETURN(CAssetFileDescriptor::New(reply, fileDescriptor));
    }
    else {
        *fileDescriptor = NULL;
    }

//    data.recycle();
//    reply.recycle();
    return NOERROR;
}

ECode ContentProviderNative::ContentProviderProxy::Call(
    /* [in] */ const String& method,
    /* [in] */ const String& request,
    /* [in] */ IBundle* args,
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle);
    AutoPtr<IParcel> data;    //Parcel.obtain();
    AutoPtr<IParcel> reply;    //Parcel.obtain();
//    data.writeInterfaceToken(IContentProvider.descriptor);
    FAIL_RETURN(data->WriteString(method));
    FAIL_RETURN(data->WriteString(request));

    if (NULL == args) {
        FAIL_RETURN(data->WriteInt32(-1));
    }
    else {
        FAIL_RETURN(data->WriteInterfacePtr(args));
    }

    Boolean ret = FALSE;
    FAIL_RETURN(mRemote->Transact(IIContentProviderConstants::CALL_TRANSACTION, data, reply, 0, &ret));
//***    FAIL_RETURN(DatabaseUtils::ReadExceptionFromParcel(reply));
    AutoPtr<IInterface> obj;
    FAIL_RETURN(reply->ReadInterfacePtr((Handle32*)(IInterface*)obj));
    *bundle = obj != NULL ? (IBundle*) obj->Probe(EIID_IBundle) : NULL;
    REFCOUNT_ADD(*bundle);
//    data.recycle();
//    reply.recycle();
    return NOERROR;
}

ECode ContentProviderNative::ContentProviderProxy::GetStreamTypes(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mimeTypeFilter,
    /* [out, callee] */ ArrayOf<String>** streamTypes)
{
    VALIDATE_NOT_NULL(streamTypes);
    AutoPtr<IParcel> data;    //Parcel.obtain();
    AutoPtr<IParcel> reply;    //Parcel.obtain();
//    data.writeInterfaceToken(IContentProvider.descriptor);
    AutoPtr<IParcelable> parcelable = (IParcelable*) uri->Probe(EIID_IParcelable);
    FAIL_RETURN(parcelable->WriteToParcel(data));
    FAIL_RETURN(data->WriteString(mimeTypeFilter));
    Boolean ret = FALSE;
    FAIL_RETURN(mRemote->Transact(IIContentProviderConstants::GET_STREAM_TYPES_TRANSACTION, data, reply, 0, &ret));
//***    FAIL_RETURN(DatabaseUtils::ReadExceptionFromParcel(reply));
    FAIL_RETURN(reply->ReadArrayOfString(streamTypes));
//    data.recycle();
//    reply.recycle();
    return NOERROR;
}

ECode ContentProviderNative::ContentProviderProxy::OpenTypedAssetFile(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mimeType,
    /* [in] */ IBundle* opts,
    /* [out] */ IAssetFileDescriptor** fileDescriptor)
{
    VALIDATE_NOT_NULL(fileDescriptor);
    AutoPtr<IParcel> data;    //Parcel.obtain();
    AutoPtr<IParcel> reply;    //Parcel.obtain();
//    data.writeInterfaceToken(IContentProvider.descriptor);
    AutoPtr<IParcelable> parcelable = (IParcelable*) uri->Probe(EIID_IParcelable);
    FAIL_RETURN(parcelable->WriteToParcel(data));
    FAIL_RETURN(data->WriteString(mimeType));

    if (NULL == opts) {
        FAIL_RETURN(data->WriteInt32(-1));
    }
    else {
        FAIL_RETURN(data->WriteInterfacePtr(opts));
    }

    Boolean ret = FALSE;
    FAIL_RETURN(mRemote->Transact(IIContentProviderConstants::OPEN_TYPED_ASSET_FILE_TRANSACTION, data, reply, 0, &ret));
//***    FAIL_RETURN(DatabaseUtils::ReadExceptionWithFileNotFoundExceptionFromParcel(reply));
    Int32 has = 0;
    FAIL_RETURN(reply->ReadInt32(&has));

    if (has != 0) {
        AutoPtr<IAssetFileDescriptor> fd;
//***        FAIL_RETURN(CAssetFileDescriptor::New(reply, (IAssetFileDescriptor**)&fd));
        *fileDescriptor = fd;
        REFCOUNT_ADD(*fileDescriptor);
    }
    else {
        *fileDescriptor = NULL;
    }

//    data.recycle();
//    reply.recycle();
    return NOERROR;

}

ECode ContentProviderNative::ContentProviderProxy::CreateCancellationSignal(
    /* [out] */ IICancellationSignal** cancellationSignal)
{
    VALIDATE_NOT_NULL(cancellationSignal);
    AutoPtr<IParcel> data;    //Parcel.obtain();
    AutoPtr<IParcel> reply;    //Parcel.obtain();
//    data.writeInterfaceToken(IContentProvider.descriptor);
    Boolean ret = FALSE;
    FAIL_RETURN(mRemote->Transact(IIContentProviderConstants::CREATE_CANCELATION_SIGNAL_TRANSACTION, data, reply, 0, &ret));
//***    FAIL_RETURN(DatabaseUtils::ReadExceptionFromParcel(reply));
//    ICancellationSignal cancellationSignal = ICancellationSignal.Stub.asInterface(reply.readStrongBinder());

//    data.recycle();
//    reply.recycle();
    return E_NOT_IMPLEMENTED;
}

ContentProviderNative::ContentProviderNative()
{
//    attachInterface(this, descriptor);
}

ContentProviderNative::~ContentProviderNative()
{}

PInterface ContentProviderNative::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface) (IContentProviderNative*) this;
    } else if (riid == EIID_IObject) {
        return (IObject*) this;
    }

    if (riid == EIID_IContentProviderNative) {
        return (IContentProviderNative *) this;
    } else if (riid == EIID_IIBinder) {
        return (IIBinder*) this;
    }

    return NULL;
}

UInt32 ContentProviderNative::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 ContentProviderNative::Release()
{
    return ElRefBase::Release();
}

ECode ContentProviderNative::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (NULL == pIID){
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface *)(IContentProviderNative *)this) {
        *pIID = EIID_IContentProviderNative;
        return NOERROR;
    } else if (pObject == (IInterface *)(IObject *)this) {
        *pIID = EIID_IObject;
        return NOERROR;
    }

    return E_INVALID_ARGUMENT;
}

ECode ContentProviderNative::Aggregate(
    /* [in] */ AggrType aggrType,
    /* [in] */ PInterface pObject)
{
    return E_NOT_IMPLEMENTED;
}

ECode ContentProviderNative::GetDomain(
    /* [out] */ PInterface *ppObject)
{
    return E_NOT_IMPLEMENTED;
}

ECode ContentProviderNative::GetClassID(
    /* [out] */ ClassID *pCLSID)
{
    return E_NOT_IMPLEMENTED;
}

ECode ContentProviderNative::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(other);

    IContentProviderNative * o = IContentProviderNative::Probe(other);
    if (o != NULL) {
        *result = (o == THIS_PROBE(IContentProviderNative));
    }
    return NOERROR;
}

ECode ContentProviderNative::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash);
    *hash = (Int32)THIS_PROBE(IContentProviderNative);
    return NOERROR;
}

ECode ContentProviderNative::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    StringBuilder sb("ContentProviderNative:(");
    sb += (Int32)THIS_PROBE(IContentProviderNative);
    sb += ")";
    sb.ToString(info);
    return NOERROR;
}

ECode ContentProviderNative::AsInterface(
    /* [in] */ IIBinder* obj,
    /* [out] */ IIContentProvider** contentProvider)
{
    VALIDATE_NOT_NULL(contentProvider);

    if (NULL == obj) {
        *contentProvider = NULL;
        return NOERROR;
    }

    AutoPtr<IIContentProvider> provider;
    FAIL_RETURN(obj->QueryLocalInterface(IIContentProviderConstants::DESCRIPTOR, (IInterface**)&provider));

    if (NULL != provider) {
        *contentProvider = provider;
        REFCOUNT_ADD(*contentProvider);
        return NOERROR;
    }

    AutoPtr<ContentProviderNative::ContentProviderProxy> proxy = new ContentProviderNative::ContentProviderProxy(obj);
    provider = (IIContentProvider*) proxy->Probe(EIID_IIContentProvider);
    *contentProvider = provider;
    REFCOUNT_ADD(*contentProvider);
    return NOERROR;
}

ECode ContentProviderNative::OnTransact(
    /* [in] */ Int32 code,
    /* [in] */ IParcel* data,
    /* [in] */ IParcel* reply,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(data);
    VALIDATE_NOT_NULL(reply);
    VALIDATE_NOT_NULL(result);
    ECode ec = NOERROR;

    switch (code) {
        case IIContentProviderConstants::QUERY_TRANSACTION:
        {
//            data.enforceInterface(IContentProvider.descriptor);
            AutoPtr<IUri> url;
            FAIL_RETURN(CreateUri(data, reply, (IUri**)&url, result));

            // String[] projection
            Int32 num = 0;
            ec = data->ReadInt32(&num);
            FAILED_WITH_RETURN(ec, reply, result)

            AutoPtr<ArrayOf<String> > projection;

            if (num > 0) {
                projection = ArrayOf<String>::Alloc(num);
                for (Int32 i = 0; i < num; i++) {
                    String str;
                    ec = data->ReadString(&str);
                    FAILED_WITH_RETURN(ec, reply, result)
                    (*projection)[i] = str;
                }
            }

            // String selection, String[] selectionArgs...
            String selection;
            ec = data->ReadString(&selection);
            FAILED_WITH_RETURN(ec, reply, result)
            ec = data->ReadInt32(&num);
            FAILED_WITH_RETURN(ec, reply, result)
            AutoPtr<ArrayOf<String> > selectionArgs;

            if (num > 0) {
                selectionArgs = ArrayOf<String>::Alloc(num);
                for (Int32 i = 0; i < num; i++) {
                    String str;
                    ec = data->ReadString(&str);
                    FAILED_WITH_RETURN(ec, reply, result)
                    (*selectionArgs)[i] = str;
                }
            }

            String sortOrder;
            ec = data->ReadString(&sortOrder);
            FAILED_WITH_RETURN(ec, reply, result)
//            IContentObserver observer = IContentObserver.Stub.asInterface(
//                    data.readStrongBinder());
//            ICancellationSignal cancellationSignal = ICancellationSignal.Stub.asInterface(
//                    data.readStrongBinder());
            AutoPtr<IIContentObserver> observer;
            AutoPtr<IICancellationSignal> cancellationSignal;
            AutoPtr<ICursor> cursor;
            ec = Query(url, projection, selection, selectionArgs, sortOrder, cancellationSignal, (ICursor**)&cursor);
            FAILED_WITH_RETURN(ec, reply, result)

            if (NULL != cursor) {
//                 AutoPtr<ICursorToBulkCursorAdaptor> adaptor;
//                 String providerName;
//                 ec = GetProviderName(&providerName);
//                 FAILED_WITH_RETURN(ec, reply, result)
// //***                ec = CCursorToBulkCursorAdaptor::New(cursor, observer, providerName, (ICursorToBulkCursorAdaptor**)&adaptor);
// //***                FAILED_WITH_RETURN(ec, reply, result)
//                 AutoPtr<IBulkCursorDescriptor> d;
//                 ec = adaptor->GetBulkCursorDescriptor((IBulkCursorDescriptor**)&d);
//                 FAILED_WITH_RETURN(ec, reply, result)
// //                reply.writeNoException();
//                 ec = reply->WriteInt32(1);
//                 FAILED_WITH_RETURN(ec, reply, result)
//                 AutoPtr<IParcelable> parcelable = (IParcelable*) d->Probe(EIID_IParcelable);
//                 ec = parcelable->WriteToParcel(reply);//d.writeToParcel(reply, Parcelable.PARCELABLE_WRITE_RETURN_VALUE);
//                 FAILED_WITH_RETURN(ec, reply, result)
            }
            else {
//                reply.writeNoException();
                ec = reply->WriteInt32(0);
                FAILED_WITH_RETURN(ec, reply, result)
            }

            *result = TRUE;
            return NOERROR;
        }
        case IIContentProviderConstants::GET_TYPE_TRANSACTION:
        {
//            data.enforceInterface(IContentProvider.descriptor);
            AutoPtr<IUri> url;
            FAIL_RETURN(CreateUri(data, reply, (IUri**)&url, result));
            String typeStr;
            ec = GetType(url, &typeStr);
            FAILED_WITH_RETURN(ec, reply, result)
//            reply.writeNoException();
            ec = reply->WriteString(typeStr);
            FAILED_WITH_RETURN(ec, reply, result)
            *result = TRUE;
            return NOERROR;
        }
        case IIContentProviderConstants::INSERT_TRANSACTION:
        {
//            data.enforceInterface(IContentProvider.descriptor);
            AutoPtr<IUri> url;
            FAIL_RETURN(CreateUri(data, reply, (IUri**)&url, result));
            AutoPtr<IContentValues> values;
//            ContentValues.CREATOR.createFromParcel(data);
            AutoPtr<IUri> out;
            ec = Insert(url, values, (IUri**)&out);
            FAILED_WITH_RETURN(ec, reply, result)
//            reply.writeNoException();

            if (NULL == out) {
                ec = reply->WriteInt32(0);
                FAILED_WITH_RETURN(ec, reply, result)
            }
            else {
                AutoPtr<IParcelable> parcelable = (IParcelable*) out->Probe(EIID_IParcelable);
                ec = parcelable->WriteToParcel(reply);
                FAILED_WITH_RETURN(ec, reply, result)
            }

            *result = TRUE;
            return NOERROR;
        }
        case IIContentProviderConstants::APPLY_BATCH_TRANSACTION:
        {
//            data.enforceInterface(IContentProvider.descriptor);
            Int32 numOperations = 0;
            ec = data->ReadInt32(&numOperations);
            FAILED_WITH_RETURN(ec, reply, result)
            AutoPtr<IObjectContainer> operations;
//***            FAIL_RETURN(CParcelableObjectContainer::New((IObjectContainer**)&operations));

            for (Int32 i = 0; i < numOperations; i++) {
                AutoPtr<IContentProviderOperation> oper;
                ec = CContentProviderOperation::New(data, (IContentProviderOperation**)&oper);
                FAILED_WITH_RETURN(ec, reply, result)
                ec = operations->Add(oper.Get());
                FAILED_WITH_RETURN(ec, reply, result)
            }

            AutoPtr<ArrayOf<IContentProviderResult*> > results;
            ec = ApplyBatch(operations, (ArrayOf<IContentProviderResult*>**)&results);
            FAILED_WITH_RETURN(ec, reply, result)
//            reply.writeNoException();

            if (NULL != results) {
                Int32 N = results->GetLength();
                ec = reply->WriteInt32(N);
                FAILED_WITH_RETURN(ec, reply, result)

                for (Int32 i=0; i<N; i++) {
                    AutoPtr<IContentProviderResult> item = (*results)[i];

                    if (NULL != item) {
                        ec = reply->WriteInt32(1);
                        FAILED_WITH_RETURN(ec, reply, result)
                        AutoPtr<IParcelable> itemParcelable = (IParcelable*) item->Probe(EIID_IParcelable);
                        ec = itemParcelable->WriteToParcel(reply);
                        FAILED_WITH_RETURN(ec, reply, result)
                    }
                    else {
                        ec = reply->WriteInt32(0);
                        FAILED_WITH_RETURN(ec, reply, result)
                    }

                }

            }
            else {
                ec = reply->WriteInt32(-1);
                FAILED_WITH_RETURN(ec, reply, result)
            }

            *result = TRUE;
            return NOERROR;
        }
        case IIContentProviderConstants::DELETE_TRANSACTION:
        {
//            data.enforceInterface(IContentProvider.descriptor);
            AutoPtr<IUri> url;
            FAIL_RETURN(CreateUri(data, reply, (IUri**)&url, result));
            String selection;
            ec = data->ReadString(&selection);
            FAILED_WITH_RETURN(ec, reply, result)
            AutoPtr<ArrayOf<String> > selectionArgs;
            ec = data->ReadArrayOfString((ArrayOf<String>**)&selectionArgs);
            FAILED_WITH_RETURN(ec, reply, result)
            Int32 count = 0;
            ec = Delete(url, selection, selectionArgs, &count);
            FAILED_WITH_RETURN(ec, reply, result)
//            reply.writeNoException();
            ec = reply->WriteInt32(count);
            FAILED_WITH_RETURN(ec, reply, result)
            *result = TRUE;
            return NOERROR;
        }
        case IIContentProviderConstants::UPDATE_TRANSACTION:
        {
//            data.enforceInterface(IContentProvider.descriptor);
            AutoPtr<IUri> url;
            FAIL_RETURN(CreateUri(data, reply, (IUri**)&url, result));
            AutoPtr<IContentValues> values;
//            ContentValues.CREATOR.createFromParcel(data);
            String selection;
            ec = data->ReadString(&selection);
            FAILED_WITH_RETURN(ec, reply, result)
            AutoPtr<ArrayOf<String> > selectionArgs;
            ec = data->ReadArrayOfString((ArrayOf<String>**)&selectionArgs);
            FAILED_WITH_RETURN(ec, reply, result)
            Int32 count = 0;
            ec = Update(url, values, selection, selectionArgs, &count);
            FAILED_WITH_RETURN(ec, reply, result)
//            reply.writeNoException();
            ec = reply->WriteInt32(count);
            FAILED_WITH_RETURN(ec, reply, result)
            *result = TRUE;
            return NOERROR;
        }
        case IIContentProviderConstants::OPEN_FILE_TRANSACTION:
        {
//            data.enforceInterface(IContentProvider.descriptor);
            AutoPtr<IUri> url;
            FAIL_RETURN(CreateUri(data, reply, (IUri**)&url, result));
            String mode;
            ec = data->ReadString(&mode);
            FAILED_WITH_RETURN(ec, reply, result)
            AutoPtr<IParcelFileDescriptor> fd;
            ec = OpenFile(url, mode, (IParcelFileDescriptor**)&fd);
            FAILED_WITH_RETURN(ec, reply, result)
//            reply.writeNoException();

            if (NULL != fd) {
                ec = reply->WriteInt32(1);
                FAILED_WITH_RETURN(ec, reply, result)
                AutoPtr<IParcelable> parcelable = (IParcelable*) fd->Probe(EIID_IParcelable);
                ec = parcelable->WriteToParcel(reply);//fd.writeToParcel(reply,Parcelable.PARCELABLE_WRITE_RETURN_VALUE)
                FAILED_WITH_RETURN(ec, reply, result)
            } else {
                ec = reply->WriteInt32(0);
                FAILED_WITH_RETURN(ec, reply, result)
            }

            *result = TRUE;
            return NOERROR;
        }
        case IIContentProviderConstants::OPEN_ASSET_FILE_TRANSACTION:
        {
//            data.enforceInterface(IContentProvider.descriptor);
            AutoPtr<IUri> url;
            FAIL_RETURN(CreateUri(data, reply, (IUri**)&url, result));
            String mode;
            ec = data->ReadString(&mode);
            FAILED_WITH_RETURN(ec, reply, result)
            AutoPtr<IAssetFileDescriptor> fd;
            ec = OpenAssetFile(url, mode, (IAssetFileDescriptor**)&fd);
            FAILED_WITH_RETURN(ec, reply, result)
//            reply.writeNoException();

            if (NULL != fd) {
                ec = reply->WriteInt32(1);
                FAILED_WITH_RETURN(ec, reply, result)
                AutoPtr<IParcelable> parcelable = (IParcelable*) fd->Probe(EIID_IParcelable);
                ec = parcelable->WriteToParcel(reply);//fd.writeToParcel(reply,Parcelable.PARCELABLE_WRITE_RETURN_VALUE)
                FAILED_WITH_RETURN(ec, reply, result)
            }
            else {
                ec = reply->WriteInt32(0);
                FAILED_WITH_RETURN(ec, reply, result)
            }

            *result = TRUE;
            return NOERROR;
        }
        case IIContentProviderConstants::CALL_TRANSACTION:
        {
//            data.enforceInterface(IContentProvider.descriptor);
            String method;
            ec = data->ReadString(&method);
            FAILED_WITH_RETURN(ec, reply, result)
            String stringArg;
            ec = data->ReadString(&stringArg);
            FAILED_WITH_RETURN(ec, reply, result)
            AutoPtr<IBundle> args;
            ec = data->ReadInterfacePtr((Handle32*) (IBundle*)&args);
            FAILED_WITH_RETURN(ec, reply, result)
            AutoPtr<IBundle> responseBundle;
            ec = Call(method, stringArg, args, (IBundle**)&responseBundle);
            FAILED_WITH_RETURN(ec, reply, result)
//            reply.writeNoException();
            ec = reply->WriteInterfacePtr((IInterface*)responseBundle);
            FAILED_WITH_RETURN(ec, reply, result)
            *result = TRUE;
            return NOERROR;
        }
        case IIContentProviderConstants::GET_STREAM_TYPES_TRANSACTION:
        {
//            data.enforceInterface(IContentProvider.descriptor);
            AutoPtr<IUri> url;
            FAIL_RETURN(CreateUri(data, reply, (IUri**)&url, result));
            String mimeTypeFilter;
            ec = data->ReadString(&mimeTypeFilter);
            FAILED_WITH_RETURN(ec, reply, result)
            AutoPtr<ArrayOf<String> > types;
            ec = GetStreamTypes(url, mimeTypeFilter, (ArrayOf<String>**)&types);
            FAILED_WITH_RETURN(ec, reply, result)
//            reply.writeNoException();
            ec = reply->WriteArrayOfString(types);
            FAILED_WITH_RETURN(ec, reply, result)
            *result = TRUE;
            return NOERROR;
        }

        case IIContentProviderConstants::OPEN_TYPED_ASSET_FILE_TRANSACTION:
        {
//            data.enforceInterface(IContentProvider.descriptor);
            AutoPtr<IUri> url;
            FAIL_RETURN(CreateUri(data, reply, (IUri**)&url, result));
            String mimeType;
            ec = data->ReadString(&mimeType);
            FAILED_WITH_RETURN(ec, reply, result)
            AutoPtr<IBundle> opts;
            ec = data->ReadInterfacePtr((Handle32*) (IBundle*)&opts);
            FAILED_WITH_RETURN(ec, reply, result)
            AutoPtr<IAssetFileDescriptor> fd;
            ec = OpenTypedAssetFile(url, mimeType, opts, (IAssetFileDescriptor**)&fd);
            FAILED_WITH_RETURN(ec, reply, result)
//            reply.writeNoException();
            if (NULL != fd) {
                ec = reply->WriteInt32(1);
                FAILED_WITH_RETURN(ec, reply, result)
                AutoPtr<IParcelable> parcelable = (IParcelable*) fd->Probe(EIID_IParcelable);
                ec = parcelable->WriteToParcel(reply);//fd.writeToParcel(reply,Parcelable.PARCELABLE_WRITE_RETURN_VALUE)
            }
            else {
                ec = reply->WriteInt32(0);
                FAILED_WITH_RETURN(ec, reply, result)
            }

            *result = TRUE;
            return NOERROR;
        }

        case IIContentProviderConstants::CREATE_CANCELATION_SIGNAL_TRANSACTION:
        {
//            data.enforceInterface(IContentProvider.descriptor);
            AutoPtr<IICancellationSignal> cancellationSignal;
            ec = CreateCancellationSignal((IICancellationSignal**)&cancellationSignal);
            FAILED_WITH_RETURN(ec, reply, result)
//            reply.writeNoException();
//            reply.writeStrongBinder(cancellationSignal.asBinder());
            *result = TRUE;
            return NOERROR;
        }

    }

    //super.onTransact(code, data, reply, flags);
    //*result = Binder::onTransact(code, data, reply, flags);
    return E_NOT_IMPLEMENTED;
}

ECode ContentProviderNative::AsBinder(
    /* [out] */ IIBinder** binder)
{
    assert(0 && "TODO delete ??");
    VALIDATE_NOT_NULL(binder);
    *binder = (IIBinder*) this;
    REFCOUNT_ADD(*binder);
    return NOERROR;
}

ECode ContentProviderNative::CreateUri(
    /* [in] */ IParcel* data,
    /* [in] */ IParcel* reply,
    /* [out] */ IUri** url,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(url);
    Int32 type = 0;
    ECode ecode = data->ReadInt32(&type);
    FAILED_WITH_RETURN(ecode, reply, result)
    String str1;

    if (1 == type) {
        ecode = data->ReadString(&str1);
        FAILED_WITH_RETURN(ecode, reply, result)
//***        ecode = CStringUri::New(str1, url);
//***        FAILED_WITH_RETURN(ecode, reply, result);
    }
    else if (2 == type) {
        ecode = data->ReadString(&str1);
        FAILED_WITH_RETURN(ecode, reply, result)
//***        AutoPtr<IPart> part1;
//***        ecode = Part::ReadFrom(data, (IPart**)&part1);
//***        FAILED_WITH_RETURN(ecode, reply, result);
//***        AutoPtr<IPart> part2;
//***        ecode = Part::ReadFrom(data, (IPart**)&part2);
//***        FAILED_WITH_RETURN(ecode, reply, result);
//***        ecode = COpaqueUri::New(str1, part1, part2, url);
//***        FAILED_WITH_RETURN(ecode, reply, result);
    }
    else if (3 == type) {
        ecode = data->ReadString(&str1);
        FAILED_WITH_RETURN(ecode, reply, result)
//***        AutoPtr<IPart> part1;
//***        ecode = Part::ReadFrom(data, (IPart**)&part1);
//***        FAILED_WITH_RETURN(ecode, reply, result);
//***        AutoPtr<IPathPart> pathPart1;
//***        ecode = PathPart::ReadFrom(data, (IPathPart**)&pathPart1);
//***        FAILED_WITH_RETURN(ecode, reply, result);
//***        AutoPtr<IPart> part2;
//***        ecode = Part::ReadFrom(data, (IPart**)&part2);
//***        FAILED_WITH_RETURN(ecode, reply, result);
//***        AutoPtr<IPart> part3;
//***        ecode = Part::ReadFrom(data, (IPart**)&part3);
//***        FAILED_WITH_RETURN(ecode, reply, result);
//***        ecode = CHierarchicalUri::New(str1, part1, pathPart1, part2, part3, url);
//***        FAILED_WITH_RETURN(ecode, reply, result);
    }

    return NOERROR;
}

}
}
}
