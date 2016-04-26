
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Net.h"

#include "elastos/droid/content/CContentProviderOperation.h"
#include "elastos/droid/content/CContentProviderResult.h"
#include "elastos/droid/content/CContentProviderTransport.h"
#include "elastos/droid/content/ContentProvider.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/CCancellationSignalHelper.h"
#include "elastos/droid/app/AppOpsManager.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::ICancellationSignal;
using Elastos::Droid::Os::ICancellationSignalHelper;
using Elastos::Droid::Os::CCancellationSignalHelper;
using Elastos::Droid::App::AppOpsManager;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Content {

static const String TAG("CContentProviderTransport");

CAR_INTERFACE_IMPL_3(CContentProviderTransport, Object, IContentProviderTransport, IIContentProvider, IBinder)

CAR_OBJECT_IMPL(CContentProviderTransport)

CContentProviderTransport::CContentProviderTransport()
    : mReadOp(IAppOpsManager::OP_NONE)
    , mWriteOp(IAppOpsManager::OP_NONE)
{
}

ECode CContentProviderTransport::constructor(
    /* [in] */ IContentProvider* owner)
{
    AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(owner);
    AutoPtr<IWeakReference> wr;
    wrs->GetWeakReference((IWeakReference**)&wr);
    mWeakContentProvider = wr;
    return NOERROR;
}

ECode CContentProviderTransport::GetContentProvider(
    /* [out] */ IContentProvider** provider)
{
    VALIDATE_NOT_NULL(provider)
    *provider = NULL;
    AutoPtr<IContentProvider> obj;
    mWeakContentProvider->Resolve(EIID_IContentProvider, (IInterface**)&obj);
    *provider = obj;
    REFCOUNT_ADD(*provider)
    return NOERROR;
}

ECode CContentProviderTransport::GetProviderName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    AutoPtr<IContentProvider> provider;
    GetContentProvider((IContentProvider**)&provider);
    *name = Object::ToString(provider);
    return NOERROR;
}

ECode CContentProviderTransport::Query(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* inUri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [in] */ IICancellationSignal* cancellationSignal,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor);
    *cursor = NULL;

    AutoPtr<IContentProvider> contentProvider;
    mWeakContentProvider->Resolve(EIID_IContentProvider, (IInterface**)&contentProvider);
    if (contentProvider == NULL)  return NOERROR;

    ContentProvider* cp = (ContentProvider*)contentProvider.Get();
    FAIL_RETURN(cp->ValidateIncomingUri(inUri))

    AutoPtr<IUri> uri;
    ContentProvider::GetUriWithoutUserId(inUri, (IUri**)&uri);

    Int32 ival;
    FAIL_RETURN(EnforceReadPermission(callingPkg, uri, &ival))

    AutoPtr<ICancellationSignalHelper> helper;
    CCancellationSignalHelper::AcquireSingleton((ICancellationSignalHelper**)&helper);
    AutoPtr<ICancellationSignal> sig;
    helper->FromTransport(cancellationSignal, (ICancellationSignal**)&sig);

    if (ival != IAppOpsManager::MODE_ALLOWED) {
        return cp->RejectQuery(uri, projection, selection, selectionArgs, sortOrder, sig, cursor);
    }

    String original = cp->SetCallingPackage(callingPkg);
    ECode ec = contentProvider->Query(uri, projection, selection, selectionArgs, sortOrder, sig, cursor);
    cp->SetCallingPackage(original);
    return ec;
}

ECode CContentProviderTransport::GetType(
    /* [in] */ IUri* inUri,
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type);
    *type = String(NULL);

    AutoPtr<IContentProvider> contentProvider;
    mWeakContentProvider->Resolve(EIID_IContentProvider, (IInterface**)&contentProvider);
    if (contentProvider == NULL)  return NOERROR;

    ContentProvider* cp = (ContentProvider*)contentProvider.Get();
    FAIL_RETURN(cp->ValidateIncomingUri(inUri))
    AutoPtr<IUri> uri;
    ContentProvider::GetUriWithoutUserId(inUri, (IUri**)&uri);

    return contentProvider->GetType(uri, type);
}

ECode CContentProviderTransport::Insert(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* inUri,
    /* [in] */ IContentValues* initialValues,
    /* [out] */ IUri** insertedUri)
{
    VALIDATE_NOT_NULL(insertedUri);
    *insertedUri = NULL;

    AutoPtr<IContentProvider> contentProvider;
    mWeakContentProvider->Resolve(EIID_IContentProvider, (IInterface**)&contentProvider);
    if (contentProvider == NULL)  return NOERROR;

    ContentProvider* cp = (ContentProvider*)contentProvider.Get();
    FAIL_RETURN(cp->ValidateIncomingUri(inUri))

    Int32 userId = ContentProvider::GetUserIdFromUri(inUri);
    AutoPtr<IUri> uri;
    ContentProvider::GetUriWithoutUserId(inUri, (IUri**)&uri);

    Int32 ival;
    FAIL_RETURN(EnforceWritePermission(callingPkg, uri, &ival))
    if (ival != IAppOpsManager::MODE_ALLOWED) {
        return cp->RejectInsert(uri, initialValues, insertedUri);
    }

    String original = cp->SetCallingPackage(callingPkg);
    AutoPtr<IUri> tmpUri, resultUri;
    ECode ec = contentProvider->Insert(uri, initialValues, (IUri**)&tmpUri);
    FAIL_GOTO(ec, _Exit_)

    resultUri = ContentProvider::MaybeAddUserId(tmpUri, userId);
    *insertedUri = resultUri;
    REFCOUNT_ADD(*insertedUri)

_Exit_:
    cp->SetCallingPackage(original);
    return ec;
}

ECode CContentProviderTransport::BulkInsert(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* inUri,
    /* [in] */ ArrayOf<IContentValues*>* initialValues,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = 0;

    AutoPtr<IContentProvider> contentProvider;
    mWeakContentProvider->Resolve(EIID_IContentProvider, (IInterface**)&contentProvider);
    if (contentProvider == NULL)  return NOERROR;

    ContentProvider* cp = (ContentProvider*)contentProvider.Get();
    FAIL_RETURN(cp->ValidateIncomingUri(inUri))
    AutoPtr<IUri> uri;
    ContentProvider::GetUriWithoutUserId(inUri, (IUri**)&uri);

    Int32 ival;
    FAIL_RETURN(EnforceWritePermission(callingPkg, uri, &ival))
    if (ival != IAppOpsManager::MODE_ALLOWED) {
        return NOERROR;
    }

    String original = cp->SetCallingPackage(callingPkg);
    ECode ec = contentProvider->BulkInsert(uri, initialValues, number);
    cp->SetCallingPackage(original);
    return ec;
}

ECode CContentProviderTransport::ApplyBatch(
    /* [in] */ const String& callingPkg,
    /* [in] */ IArrayList* operations,
    /* [out, callee] */ ArrayOf<IContentProviderResult*>** providerResults)
{
    VALIDATE_NOT_NULL(providerResults)
    *providerResults = NULL;
    VALIDATE_NOT_NULL(operations)

    AutoPtr<IContentProvider> contentProvider;
    mWeakContentProvider->Resolve(EIID_IContentProvider, (IInterface**)&contentProvider);
    if (contentProvider == NULL)  return NOERROR;
    ContentProvider* cp = (ContentProvider*)contentProvider.Get();

    Int32 numOperations;
    operations->GetSize(&numOperations);

    Boolean isRead = FALSE;
    Boolean isWrite = FALSE;
    IContentProviderOperation* operation;

    AutoPtr<ArrayOf<Int32> > userIds = ArrayOf<Int32>::Alloc(numOperations);
    for (Int32 i = 0; i < numOperations; i++) {
        AutoPtr<IInterface> obj;
        operations->Get(i, (IInterface**)&obj);
        operation = IContentProviderOperation::Probe(obj);

        AutoPtr<IUri> uri;
        operation->GetUri((IUri**)&uri);
        FAIL_RETURN(cp->ValidateIncomingUri(uri))
        (*userIds)[i] = ContentProvider::GetUserIdFromUri(uri);

        if ((*userIds)[i] != IUserHandle::USER_CURRENT) {
          // Removing the user id from the uri.
            AutoPtr<IContentProviderOperation> newOp;
            assert(0 && "TODO");
            // CContentProviderOperation::New(operation, TRUE, (IContentProviderOperation**)&newOp);
            operations->Set(i, newOp);
            operation = newOp;
        }

        if (operation->IsReadOperation(&isRead), isRead) {
            Int32 ival;
            FAIL_RETURN(EnforceWritePermission(callingPkg, uri, &ival))
            if (ival != IAppOpsManager::MODE_ALLOWED) {
                // throw new OperationApplicationException("App op not allowed", 0);
                return E_OPERATION_APPLICATION_EXCEPTION;
            }
        }

        if (operation->IsWriteOperation(&isWrite), isWrite) {
            Int32 ival;
            FAIL_RETURN(EnforceWritePermission(callingPkg, uri, &ival))
            if (ival != IAppOpsManager::MODE_ALLOWED) {
                // throw new OperationApplicationException("App op not allowed", 0);
                return E_OPERATION_APPLICATION_EXCEPTION;
            }
        }
    }

    String original = cp->SetCallingPackage(callingPkg);
    AutoPtr<ArrayOf<IContentProviderResult*> > temp;
    ECode ec = contentProvider->ApplyBatch(operations, (ArrayOf<IContentProviderResult*>**)&temp);
    FAIL_GOTO(ec, _Exit_)

    if (temp != NULL) {
        for (Int32 i = 0; i < temp->GetLength(); ++i) {
            if ((*userIds)[i] != IUserHandle::USER_CURRENT) {
                // Adding the userId to the uri.
                AutoPtr<IContentProviderResult> newResult;
                assert(0 && "TODO");
                // CContentProviderResult::New((*temp)[i], (*userIds)[i],
                //     (IContentProviderResult**)&newResult);
                temp->Set(i, newResult);
            }
        }
    }

    *providerResults = temp;
    REFCOUNT_ADD(*providerResults)

_Exit_:
    cp->SetCallingPackage(original);
    return ec;
}

ECode CContentProviderTransport::Delete(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* inUri,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* rowsAffected)
{
    VALIDATE_NOT_NULL(rowsAffected);
    *rowsAffected = 0;

    AutoPtr<IContentProvider> contentProvider;
    mWeakContentProvider->Resolve(EIID_IContentProvider, (IInterface**)&contentProvider);
    if (contentProvider == NULL)  return NOERROR;

    ContentProvider* cp = (ContentProvider*)contentProvider.Get();
    FAIL_RETURN(cp->ValidateIncomingUri(inUri))
    AutoPtr<IUri> uri;
    ContentProvider::GetUriWithoutUserId(inUri, (IUri**)&uri);

    Int32 ival;
    FAIL_RETURN(EnforceWritePermission(callingPkg, uri, &ival))

    if (ival != IAppOpsManager::MODE_ALLOWED) {
        return NOERROR;
    }

    String original = cp->SetCallingPackage(callingPkg);
    ECode ec = contentProvider->Delete(uri, selection, selectionArgs, rowsAffected);
    cp->SetCallingPackage(original);
    return ec;
}

ECode CContentProviderTransport::Update(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* inUri,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* rowsAffected)
{
    VALIDATE_NOT_NULL(rowsAffected);
    *rowsAffected = 0;

    AutoPtr<IContentProvider> contentProvider;
    mWeakContentProvider->Resolve(EIID_IContentProvider, (IInterface**)&contentProvider);
    if (contentProvider == NULL)  return NOERROR;

    ContentProvider* cp = (ContentProvider*)contentProvider.Get();
    FAIL_RETURN(cp->ValidateIncomingUri(inUri))
    AutoPtr<IUri> uri;
    ContentProvider::GetUriWithoutUserId(inUri, (IUri**)&uri);

    Int32 ival;
    FAIL_RETURN(EnforceWritePermission(callingPkg, uri, &ival))

    if (ival != IAppOpsManager::MODE_ALLOWED) {
        return NOERROR;
    }

    String original = cp->SetCallingPackage(callingPkg);
    ECode ec = contentProvider->Update(uri, values, selection, selectionArgs, rowsAffected);
    cp->SetCallingPackage(original);
    return ec;
}

ECode CContentProviderTransport::OpenFile(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* inUri,
    /* [in] */ const String& mode,
    /* [in] */ IICancellationSignal* cancellationSignal,
    /* [out] */ IParcelFileDescriptor** fileDescriptor)
{
    VALIDATE_NOT_NULL(fileDescriptor);
    *fileDescriptor = NULL;

    AutoPtr<IContentProvider> contentProvider;
    mWeakContentProvider->Resolve(EIID_IContentProvider, (IInterface**)&contentProvider);
    if (contentProvider == NULL)  return NOERROR;

    ContentProvider* cp = (ContentProvider*)contentProvider.Get();
    FAIL_RETURN(cp->ValidateIncomingUri(inUri))
    AutoPtr<IUri> uri;
    ContentProvider::GetUriWithoutUserId(inUri, (IUri**)&uri);

    Int32 ival;
    FAIL_RETURN(EnforceWritePermission(callingPkg, uri, &ival))

    AutoPtr<ICancellationSignalHelper> helper;
    CCancellationSignalHelper::AcquireSingleton((ICancellationSignalHelper**)&helper);
    AutoPtr<ICancellationSignal> sig;
    helper->FromTransport(cancellationSignal, (ICancellationSignal**)&sig);

    String original = cp->SetCallingPackage(callingPkg);
    ECode ec = contentProvider->OpenFile(uri, mode, sig, fileDescriptor);
    cp->SetCallingPackage(original);
    return ec;
}

ECode CContentProviderTransport::OpenAssetFile(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* inUri,
    /* [in] */ const String& mode,
    /* [in] */ IICancellationSignal* cancellationSignal,
    /* [out] */ IAssetFileDescriptor** fileDescriptor)
{
    VALIDATE_NOT_NULL(fileDescriptor);
    *fileDescriptor = NULL;

    AutoPtr<IContentProvider> contentProvider;
    mWeakContentProvider->Resolve(EIID_IContentProvider, (IInterface**)&contentProvider);
    if (contentProvider == NULL)  return NOERROR;

    ContentProvider* cp = (ContentProvider*)contentProvider.Get();
    FAIL_RETURN(cp->ValidateIncomingUri(inUri))
    AutoPtr<IUri> uri;
    ContentProvider::GetUriWithoutUserId(inUri, (IUri**)&uri);

    Int32 ival;
    FAIL_RETURN(EnforceWritePermission(callingPkg, uri, &ival))

    AutoPtr<ICancellationSignalHelper> helper;
    CCancellationSignalHelper::AcquireSingleton((ICancellationSignalHelper**)&helper);
    AutoPtr<ICancellationSignal> sig;
    helper->FromTransport(cancellationSignal, (ICancellationSignal**)&sig);

    String original = cp->SetCallingPackage(callingPkg);
    ECode ec = contentProvider->OpenAssetFile(uri, mode, sig, fileDescriptor);
    cp->SetCallingPackage(original);
    return ec;
}

ECode CContentProviderTransport::Call(
    /* [in] */ const String& callingPkg,
    /* [in] */ const String& method,
    /* [in] */ const String& arg,
    /* [in] */ IBundle* extras,
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle);
    *bundle = NULL;

    AutoPtr<IContentProvider> contentProvider;
    mWeakContentProvider->Resolve(EIID_IContentProvider, (IInterface**)&contentProvider);
    if (contentProvider == NULL)  return NOERROR;
    ContentProvider* cp = (ContentProvider*)contentProvider.Get();

    String original = cp->SetCallingPackage(callingPkg);
    ECode ec = contentProvider->Call(method, arg, extras, bundle);
    cp->SetCallingPackage(original);
    return ec;
}

ECode CContentProviderTransport::GetStreamTypes(
    /* [in] */ IUri* inUri,
    /* [in] */ const String& mimeTypeFilter,
    /* [out, callee] */ ArrayOf<String>** streamTypes)
{
    VALIDATE_NOT_NULL(streamTypes);
    *streamTypes = NULL;

    AutoPtr<IContentProvider> contentProvider;
    mWeakContentProvider->Resolve(EIID_IContentProvider, (IInterface**)&contentProvider);
    if (contentProvider == NULL)  return NOERROR;

    ContentProvider* cp = (ContentProvider*)contentProvider.Get();
    FAIL_RETURN(cp->ValidateIncomingUri(inUri))
    AutoPtr<IUri> uri;
    ContentProvider::GetUriWithoutUserId(inUri, (IUri**)&uri);

    return contentProvider->GetStreamTypes(uri, mimeTypeFilter, streamTypes);
}

ECode CContentProviderTransport::OpenTypedAssetFile(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* inUri,
    /* [in] */ const String& mimeType,
    /* [in] */ IBundle* opts,
    /* [in] */ IICancellationSignal* cancellationSignal,
    /* [out] */ IAssetFileDescriptor** fileDescriptor)
{
    VALIDATE_NOT_NULL(fileDescriptor);
    *fileDescriptor = NULL;

    AutoPtr<IContentProvider> contentProvider;
    mWeakContentProvider->Resolve(EIID_IContentProvider, (IInterface**)&contentProvider);
    if (contentProvider == NULL)  return NOERROR;

    ContentProvider* cp = (ContentProvider*)contentProvider.Get();
    FAIL_RETURN(cp->ValidateIncomingUri(inUri))
    AutoPtr<IUri> uri;
    ContentProvider::GetUriWithoutUserId(inUri, (IUri**)&uri);

    Int32 ival;
    FAIL_RETURN(EnforceWritePermission(callingPkg, uri, &ival))

    AutoPtr<ICancellationSignalHelper> helper;
    CCancellationSignalHelper::AcquireSingleton((ICancellationSignalHelper**)&helper);
    AutoPtr<ICancellationSignal> sig;
    helper->FromTransport(cancellationSignal, (ICancellationSignal**)&sig);

    String original = cp->SetCallingPackage(callingPkg);
    ECode ec = contentProvider->OpenTypedAssetFile(uri, mimeType, opts, sig, fileDescriptor);
    cp->SetCallingPackage(original);
    return ec;
}

ECode CContentProviderTransport::CreateCancellationSignal(
    /* [out] */ IICancellationSignal** cancellationSignal)
{
    VALIDATE_NOT_NULL(cancellationSignal)
    AutoPtr<ICancellationSignalHelper> signalHelper;
    CCancellationSignalHelper::AcquireSingleton((ICancellationSignalHelper**)&signalHelper);
    return signalHelper->CreateTransport(cancellationSignal);
}

ECode CContentProviderTransport::Canonicalize(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* inUri,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IContentProvider> contentProvider;
    mWeakContentProvider->Resolve(EIID_IContentProvider, (IInterface**)&contentProvider);
    if (contentProvider == NULL)  return NOERROR;

    ContentProvider* cp = (ContentProvider*)contentProvider.Get();
    FAIL_RETURN(cp->ValidateIncomingUri(inUri))
    Int32 userId = ContentProvider::GetUserIdFromUri(inUri);
    AutoPtr<IUri> uri;
    ContentProvider::GetUriWithoutUserId(inUri, (IUri**)&uri);

    Int32 ival;
    FAIL_RETURN(EnforceWritePermission(callingPkg, uri, &ival))

    if (ival != IAppOpsManager::MODE_ALLOWED) {
        return NOERROR;
    }

    String original = cp->SetCallingPackage(callingPkg);
    AutoPtr<IUri> temp, tmpUri;
    ECode ec = contentProvider->Canonicalize(uri, (IUri**)&uri);
    FAIL_GOTO(ec, _Exit_)

    tmpUri = ContentProvider::MaybeAddUserId(temp, userId);
    *result = tmpUri;
    REFCOUNT_ADD(*result)

_Exit_:
    cp->SetCallingPackage(original);
    return ec;
}

ECode CContentProviderTransport::Uncanonicalize(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* inUri,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IContentProvider> contentProvider;
    mWeakContentProvider->Resolve(EIID_IContentProvider, (IInterface**)&contentProvider);
    if (contentProvider == NULL)  return NOERROR;

    ContentProvider* cp = (ContentProvider*)contentProvider.Get();
    FAIL_RETURN(cp->ValidateIncomingUri(inUri))
    Int32 userId = ContentProvider::GetUserIdFromUri(inUri);
    AutoPtr<IUri> uri;
    ContentProvider::GetUriWithoutUserId(inUri, (IUri**)&uri);

    Int32 ival;
    FAIL_RETURN(EnforceWritePermission(callingPkg, uri, &ival))

    if (ival != IAppOpsManager::MODE_ALLOWED) {
        return NOERROR;
    }

    String original = cp->SetCallingPackage(callingPkg);
    AutoPtr<IUri> temp,  tmpUri;
    ECode ec = contentProvider->Uncanonicalize(uri, (IUri**)&uri);
    FAIL_GOTO(ec, _Exit_)

    tmpUri = ContentProvider::MaybeAddUserId(temp, userId);
    *result = tmpUri;
    REFCOUNT_ADD(*result)

_Exit_:
    cp->SetCallingPackage(original);
    return ec;
}

ECode CContentProviderTransport::EnforceFilePermission(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode)
{
    if (!mode.IsNull() && mode.IndexOf('w') != -1) {
        Int32 ivalue;
        FAIL_RETURN(EnforceWritePermission(callingPkg, uri, &ivalue))
        if (ivalue != IAppOpsManager::MODE_ALLOWED) {
            // throw new FileNotFoundException("App op not allowed");
            return E_FILE_NOT_FOUND_EXCEPTION;
        }
    }
    else {
        Int32 ivalue;
        FAIL_RETURN(EnforceReadPermission(callingPkg, uri, &ivalue))

        if (ivalue != IAppOpsManager::MODE_ALLOWED) {
            // throw new FileNotFoundException("App op not allowed");
            return E_FILE_NOT_FOUND_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode CContentProviderTransport::EnforceReadPermission(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* uri,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 op = IAppOpsManager::OP_NONE;
    *result = op;

    AutoPtr<IContentProvider> contentProvider;
    mWeakContentProvider->Resolve(EIID_IContentProvider, (IInterface**)&contentProvider);
    if (contentProvider == NULL)  return NOERROR;

    ContentProvider* cp = (ContentProvider*)contentProvider.Get();

    FAIL_RETURN(cp->EnforceReadPermissionInner(uri))

    if (mReadOp != op) {
        return mAppOpsManager->NoteOp(mReadOp, Binder::GetCallingUid(), callingPkg, result);
    }

    *result =  IAppOpsManager::MODE_ALLOWED;
    return NOERROR;
}

ECode CContentProviderTransport::EnforceWritePermission(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* uri,
        /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 op = IAppOpsManager::OP_NONE;
    *result = op;

    AutoPtr<IContentProvider> contentProvider;
    mWeakContentProvider->Resolve(EIID_IContentProvider, (IInterface**)&contentProvider);
    if (contentProvider == NULL)  return NOERROR;

    ContentProvider* cp = (ContentProvider*)contentProvider.Get();

    FAIL_RETURN(cp->EnforceWritePermissionInner(uri))

    if (mWriteOp != op) {
        return mAppOpsManager->NoteOp(mWriteOp, Binder::GetCallingUid(), callingPkg, result);
    }

    *result =  IAppOpsManager::MODE_ALLOWED;
    return NOERROR;
}

} // namespace Content
} // namespace Droid
} // namespace Elastos
