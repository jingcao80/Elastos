
#include "elastos/droid/content/CContentProviderClient.h"
#include "elastos/droid/content/CContentProviderHelper.h"
#include "elastos/droid/content/ContentResolver.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/Looper.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::IICancellationSignal;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::CHandler;
using Elastos::Core::ICloseGuardHelper;
using Elastos::Core::CCloseGuardHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Content {

#define HANDLE_CATCH(ec)                                                    \
    if ((ECode)E_DEAD_OBJECT_EXCEPTION == ec) {                             \
        if (!mStable) {                                                     \
            mContentResolver->UnstableProviderDied(mContentProvider);       \
        }                                                                   \
    }                                                                       \
    AfterRemote();                                                          \
    return ec;

//===========================================================================
// CContentProviderClient::NotRespondingRunnable
//===========================================================================
CContentProviderClient::NotRespondingRunnable::NotRespondingRunnable(
    /* [in] */ IWeakReference* host)
    : mWeakHost(host)
{}

ECode CContentProviderClient::NotRespondingRunnable::Run()
{
    AutoPtr<IContentProviderClient> obj;
    mWeakHost->Resolve(EIID_IContentProviderClient, (IInterface**)&obj);
    if (obj == NULL) return NOERROR;

    CContentProviderClient* cpc = (CContentProviderClient*)obj.Get();
    Logger::W("CContentProviderClient", "Detected provider not responding: %s",
        TO_CSTR(cpc->mContentProvider));
    ContentResolver* cp = (ContentResolver*)cpc->mContentResolver.Get();
    cp->AppNotRespondingViaProvider(cpc->mContentProvider);
    return NOERROR;
}

//===========================================================================
// CContentProviderClient
//===========================================================================
const String CContentProviderClient::TAG("ContentProviderClient");
Object CContentProviderClient::mContentProviderClientLock;
AutoPtr<IHandler> CContentProviderClient::sAnrHandler;

CAR_INTERFACE_IMPL(CContentProviderClient, Object, IContentProviderClient)

CAR_OBJECT_IMPL(CContentProviderClient)

CContentProviderClient::CContentProviderClient()
    : mStable(FALSE)
    , mAnrTimeout(0)
    , mReleased(FALSE)
{
    AutoPtr<ICloseGuardHelper> helper;
    CCloseGuardHelper::AcquireSingleton((ICloseGuardHelper**)&helper);
    helper->Get((ICloseGuard**)&mGuard);
}

CContentProviderClient::~CContentProviderClient()
{
    if (mGuard != NULL) {
        mGuard->WarnIfOpen();
    }
}

ECode CContentProviderClient::constructor(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IIContentProvider* provider,
    /* [in] */ Boolean stable)
{
    VALIDATE_NOT_NULL(resolver)
    VALIDATE_NOT_NULL(provider)
    mContentProvider = provider;
    mContentResolver = resolver;
    resolver->GetPackageName(&mPackageName);
    mStable = stable;

    mGuard->Open(String("CContentProviderClient::Release"));
    return NOERROR;
}

ECode CContentProviderClient::SetDetectNotResponding(
    /* [in] */ Int64 timeoutMillis)
{
    synchronized(mContentProviderClientLock) {
        mAnrTimeout = timeoutMillis;

        if (timeoutMillis > 0) {
            if (mAnrRunnable == NULL) {
                AutoPtr<IWeakReference> wr;
                GetWeakReference((IWeakReference**)&wr);
                mAnrRunnable = new NotRespondingRunnable(wr);
            }

            if (sAnrHandler == NULL) {
                AutoPtr<ILooper> looper = Looper::GetMainLooper();
                CHandler::New(looper, NULL, TRUE /* async */, (IHandler**)&sAnrHandler);
            }
        }
        else {
            mAnrRunnable = NULL;
        }
    }
    return NOERROR;
}

ECode CContentProviderClient::BeforeRemote()
{
    if (mAnrRunnable != NULL) {
        Boolean result;
        sAnrHandler->PostDelayed(mAnrRunnable, mAnrTimeout, &result);
    }
    return NOERROR;
}

ECode CContentProviderClient::AfterRemote()
{
    if (mAnrRunnable != NULL) {
        sAnrHandler->RemoveCallbacks(mAnrRunnable);
    }
    return NOERROR;
}

ECode CContentProviderClient::Query(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [out] */ ICursor** cursor)
{
    return Query(uri, projection, selection, selectionArgs, sortOrder, NULL, cursor);
}

ECode CContentProviderClient::Query(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor)
    *cursor = NULL;

    BeforeRemote();

    AutoPtr<IICancellationSignal> remoteCancellationSignal;
    ECode ec = NOERROR;
    if (NULL != cancellationSignal) {
        ec = cancellationSignal->ThrowIfCanceled();
        FAIL_GOTO(ec, _EXIT_)

        ec = mContentProvider->CreateCancellationSignal((IICancellationSignal**)&remoteCancellationSignal);
        FAIL_GOTO(ec, _EXIT_)

        ec = cancellationSignal->SetRemote(remoteCancellationSignal);
        FAIL_GOTO(ec, _EXIT_)
    }

    ec = mContentProvider->Query(mPackageName, uri, projection, selection,
        selectionArgs, sortOrder, remoteCancellationSignal, cursor);

_EXIT_:
    HANDLE_CATCH(ec)
}

ECode CContentProviderClient::GetType(
    /* [in] */ IUri* uri,
    /* [out] */ String* type)
{
    ECode ec = mContentProvider->GetType(uri, type);
    HANDLE_CATCH(ec)
}

ECode CContentProviderClient::GetStreamTypes(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mimeTypeFilter,
    /* [out, callee] */ ArrayOf<String> ** streamTypes)
{
    BeforeRemote();
    ECode ec = mContentProvider->GetStreamTypes(uri, mimeTypeFilter, streamTypes);
    HANDLE_CATCH(ec)
}

ECode CContentProviderClient::Canonicalize(
    /* [in] */ IUri* uri,
    /* [out] */ IUri** resultUri)
{
    BeforeRemote();
    ECode ec = mContentProvider->Canonicalize(mPackageName, uri, resultUri);
    HANDLE_CATCH(ec)
}

ECode CContentProviderClient::Uncanonicalize(
    /* [in] */ IUri* uri,
    /* [out] */ IUri** resultUri)
{
    BeforeRemote();
    ECode ec = mContentProvider->Uncanonicalize(mPackageName, uri, resultUri);
    HANDLE_CATCH(ec)
}

ECode CContentProviderClient::Insert(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues * initialValues,
    /* [out] */ IUri** insertedUri)
{
    BeforeRemote();
    ECode ec = mContentProvider->Insert(mPackageName, uri, initialValues, insertedUri);
    HANDLE_CATCH(ec)
}

ECode CContentProviderClient::BulkInsert(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<IContentValues *>* initialValues,
    /* [out] */ Int32* number)
{
    BeforeRemote();
    ECode ec = mContentProvider->BulkInsert(mPackageName, uri, initialValues, number);
    HANDLE_CATCH(ec)
}

ECode CContentProviderClient::Delete(
    /* [in] */ IUri* uri,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* rowsAffected)
{
    BeforeRemote();
    ECode ec = mContentProvider->Delete(mPackageName, uri, selection, selectionArgs, rowsAffected);
    HANDLE_CATCH(ec)
}

ECode CContentProviderClient::Update(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* rowsAffected)
{
    BeforeRemote();
    ECode ec = mContentProvider->Update(mPackageName, uri, values, selection, selectionArgs, rowsAffected);
    HANDLE_CATCH(ec)
}

ECode CContentProviderClient::OpenFile(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [out] */ IParcelFileDescriptor** fileDescriptor)
{
    return OpenFile(uri, mode, NULL, fileDescriptor);
}

ECode CContentProviderClient::OpenFile(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [in] */ ICancellationSignal* signal,
    /* [out] */ IParcelFileDescriptor** fileDescriptor)
{
    BeforeRemote();
    ECode ec = NOERROR;

    AutoPtr<IICancellationSignal> remoteSignal;
    if (signal != NULL) {
        ec = signal->ThrowIfCanceled();
        FAIL_GOTO(ec, _EXIT_)

        ec = mContentProvider->CreateCancellationSignal((IICancellationSignal**)&remoteSignal);
        FAIL_GOTO(ec, _EXIT_)

        ec = signal->SetRemote(remoteSignal);
        FAIL_GOTO(ec, _EXIT_)
    }

    ec = mContentProvider->OpenFile(mPackageName, uri, mode, remoteSignal, fileDescriptor);

_EXIT_:
    HANDLE_CATCH(ec)
}

ECode CContentProviderClient::OpenAssetFile(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [out] */ IAssetFileDescriptor** fileDescriptor)
{
    return OpenAssetFile(uri, mode, NULL, fileDescriptor);
}

ECode CContentProviderClient::OpenAssetFile(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [in] */ ICancellationSignal* signal,
    /* [out] */ IAssetFileDescriptor** fileDescriptor)
{
    BeforeRemote();
    ECode ec = NOERROR;

    AutoPtr<IICancellationSignal> remoteSignal;
    if (signal != NULL) {
        ec = signal->ThrowIfCanceled();
        FAIL_GOTO(ec, _EXIT_)

        ec = mContentProvider->CreateCancellationSignal((IICancellationSignal**)&remoteSignal);
        FAIL_GOTO(ec, _EXIT_)

        ec = signal->SetRemote(remoteSignal);
        FAIL_GOTO(ec, _EXIT_)
    }

    ec = mContentProvider->OpenAssetFile(mPackageName, uri, mode, remoteSignal, fileDescriptor);

_EXIT_:
    HANDLE_CATCH(ec)
}

ECode CContentProviderClient::OpenTypedAssetFileDescriptor(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mimeType,
    /* [in] */ IBundle* opts,
    /* [out] */ IAssetFileDescriptor** fileDescriptor)
{
    return OpenTypedAssetFileDescriptor(uri, mimeType, opts, NULL, fileDescriptor);
}

ECode CContentProviderClient::OpenTypedAssetFileDescriptor(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mimeType,
    /* [in] */ IBundle* opts,
    /* [in] */ ICancellationSignal* signal,
    /* [out] */ IAssetFileDescriptor** fileDescriptor)
{
    BeforeRemote();
    ECode ec = NOERROR;

    AutoPtr<IICancellationSignal> remoteSignal;
    if (signal != NULL) {
        ec = signal->ThrowIfCanceled();
        FAIL_GOTO(ec, _EXIT_)

        ec = mContentProvider->CreateCancellationSignal((IICancellationSignal**)&remoteSignal);
        FAIL_GOTO(ec, _EXIT_)

        ec = signal->SetRemote(remoteSignal);
        FAIL_GOTO(ec, _EXIT_)
    }

    ec = mContentProvider->OpenTypedAssetFile(mPackageName, uri, mimeType, opts, remoteSignal, fileDescriptor);

_EXIT_:
    HANDLE_CATCH(ec)
}

ECode CContentProviderClient::ApplyBatch(
    /* [in] */ IArrayList* operations,
    /* [out, callee] */ ArrayOf<IContentProviderResult *>** providerResults)
{
    BeforeRemote();
    ECode ec = mContentProvider->ApplyBatch(mPackageName, operations, providerResults);
    HANDLE_CATCH(ec)
}

ECode CContentProviderClient::Call(
    /* [in] */ const String& method,
    /* [in] */ const String& arg,
    /* [in] */ IBundle* extras,
    /* [out] */ IBundle** bundle)
{
    BeforeRemote();
    ECode ec = mContentProvider->Call(mPackageName, method, arg, extras, bundle);
    HANDLE_CATCH(ec)
}

ECode CContentProviderClient::ReleaseProvider(
    /* [out] */ Boolean* isRelease)
{
    VALIDATE_NOT_NULL(isRelease)
    AutoLock lock(mContentProviderClientLock);
    if (mReleased) return E_ILLEGAL_STATE_EXCEPTION;
    mReleased = TRUE;

    if (mStable) {
        return mContentResolver->ReleaseProvider(mContentProvider, isRelease);
    }
    else {
        return mContentResolver->ReleaseUnstableProvider(mContentProvider, isRelease);
    }
}

ECode CContentProviderClient::GetLocalContentProvider(
    /* [out] */ IContentProvider** localContentProvider)
{
    VALIDATE_NOT_NULL(localContentProvider)
    AutoPtr<IContentProviderHelper> providerHelper;
    FAIL_RETURN(CContentProviderHelper::AcquireSingleton((IContentProviderHelper**)&providerHelper))
    FAIL_RETURN(providerHelper->CoerceToLocalContentProvider(mContentProvider, localContentProvider))
    return NOERROR;
}

ECode CContentProviderClient::ReleaseQuietly(
    /* [in] */ IContentProviderClient* client)
{
    if (client != NULL) {
        // try {
        Boolean result;
        client->ReleaseProvider(&result);
        // } catch (Exception ignored) {
        // }
    }
    return NOERROR;
}

}
}
}

