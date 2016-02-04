
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/ContentResolver.h"
#include "elastos/droid/content/CContentResolverOpenResourceIdResult.h"
#include "elastos/droid/content/CContentProviderClient.h"
#include "elastos/droid/content/ContentProvider.h"
#include "elastos/droid/content/res/CAssetFileDescriptor.h"
//#include "elastos/droid/accounts/CAccount.h"
//#include "elastos/droid/app/ActivityManagerNative.h"
//#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/SystemClock.h"
//#include "elastos/droid/privacy/surrogate/PrivacyContentResolver.h"
//#include "elastos/droid/text/TextUtils.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>

// BEGIN privacy-added
//#include "elastos/droid/privacy/surrogate/PrivacyContentResolver.h"
// END privacy-added

//using Elastos::Droid::Accounts::CAccount;
//using Elastos::Droid::App::ActivityManagerNative;
//using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IParceledListSlice;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::CAssetFileDescriptor;
using Elastos::Droid::Database::IIContentObserver;
using Elastos::Droid::Database::EIID_ICrossProcessCursorWrapper;
using Elastos::Droid::Database::EIID_ICursorWrapper;
using Elastos::Droid::Os::IICancellationSignal;
using Elastos::Droid::Os::EIID_IParcelFileDescriptor;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IBaseBundle;
//using Elastos::Droid::Privacy::Surrogate::PrivacyContentResolver;
//using Elastos::Droid::Text::TextUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::IBoolean;
using Elastos::Core::IFloat;
using Elastos::Core::IDouble;
using Elastos::Core::StringBuilder;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::Utility::CRandom;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Content {

//=========================================================================
// ContentResolver::ParcelFileDescriptorInner
//=========================================================================
ContentResolver::CursorWrapperInner::CursorWrapperInner(
    /* [in] */ ICursor* cursor,
    /* [in] */ IIContentProvider* icp,
    /* [in] */ ContentResolver* contentResolver)
    : mContentProvider(icp)
    , mProviderReleased(FALSE)
    , mContentResolver(contentResolver)
{
    assert(0 && "TODO init parent class");
    //Init(cursor);
    mCloseGuard->Open(String("close"));
}

ContentResolver::CursorWrapperInner::~CursorWrapperInner()
{
    Finalize();
}

ECode ContentResolver::CursorWrapperInner::Close()
{
    //FAIL_RETURN(CrossProcessCursorWrapper::Close())

    Boolean result = FALSE;
    mContentResolver->ReleaseProvider(mContentProvider, &result);
    mProviderReleased = TRUE;

    if (mCloseGuard != NULL) {
        mCloseGuard->WarnIfOpen();
    }
    return NOERROR;
}

ECode ContentResolver::CursorWrapperInner::Finalize()
{
    return NOERROR;
}

//=========================================================================
// ContentResolver::ParcelFileDescriptorInner
//=========================================================================
ContentResolver::ParcelFileDescriptorInner::ParcelFileDescriptorInner()
    :  mProviderReleased(FALSE)
{}

ECode ContentResolver::ParcelFileDescriptorInner::constructor(
    /* [in] */ IParcelFileDescriptor* pfd,
    /* [in] */ IIContentProvider* icp,
    /* [in] */ ContentResolver* contentResolver)
{
    FAIL_RETURN(ParcelFileDescriptor::constructor(pfd))
    mContentProvider = icp;
    mContentResolver = contentResolver;
    return NOERROR;
}

ECode ContentResolver::ParcelFileDescriptorInner::ReleaseResources()
{
    if (!mProviderReleased) {
        Boolean result;
        mContentResolver->ReleaseProvider(mContentProvider, &result);
        mProviderReleased = TRUE;
    }
    return NOERROR;
}

//========================================================================
// ContentResolver
//========================================================================
static AutoPtr<ArrayOf<String> > InitSYNC_ERROR_NAMES()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(8);

    array->Set(0, String("already-in-progress"));
    array->Set(1, String("authentication-error"));
    array->Set(2, String("io-error"));
    array->Set(3, String("parse-error"));
    array->Set(4, String("conflict"));
    array->Set(5, String("too-many-deletions"));
    array->Set(6, String("too-many-retries"));
    array->Set(7, String("internal-error"));
    return array;
}

AutoPtr<IIntent> InitACTION_SYNC_CONN_STATUS_CHANGED()
{
    AutoPtr<IIntent> intent;
    CIntent::New(String("com.android.sync.SYNC_CONN_STATUS_CHANGED"), (IIntent**)&intent);
    return intent;
}

const AutoPtr<ArrayOf<String> > ContentResolver::SYNC_ERROR_NAMES = InitSYNC_ERROR_NAMES();

AutoPtr<IIntent> ContentResolver::ACTION_SYNC_CONN_STATUS_CHANGED = InitACTION_SYNC_CONN_STATUS_CHANGED();

const String ContentResolver::TAG("ContentResolver");

// Always log queries which take 500ms+; shorter queries are
// sampled accordingly.
const Boolean ContentResolver::ENABLE_CONTENT_SAMPLE = FALSE;
const Int32 ContentResolver::SLOW_THRESHOLD_MILLIS = 500;

AutoPtr<IIContentService> ContentResolver::sContentService;

CAR_INTERFACE_IMPL(ContentResolver, Object, IContentResolver)

ContentResolver::ContentResolver()
{
}

ContentResolver::~ContentResolver()
{
    mContext = NULL;
}

ECode ContentResolver::constructor(
    /* [in] */ IContext* context)
{
    CRandom::New((IRandom**)&mRandom);

    assert(0 && "TODO");
    if (context != NULL) {
        mContext = context;
    }
    else {
        // mContext = ActivityThread::CurrentApplication();
    }

    mContext->GetOpPackageName(&mPackageName);
    return NOERROR;
}

String ContentResolver::SyncErrorToString(
    /* [in] */ Int32 error)
{
    if (error < 1 || error > SYNC_ERROR_NAMES->GetLength()) {
        return StringUtils::ToString(error);
    }
    return (*SYNC_ERROR_NAMES)[error - 1];
}

Int32 ContentResolver::SyncErrorStringToInt(
    /* [in] */ const String& error)
{
    for (Int32 i = 0, n = SYNC_ERROR_NAMES->GetLength(); i < n; i++) {
        if ((*SYNC_ERROR_NAMES)[i].Equals(error)) {
            return i + 1;
        }
    }
    if (!error.IsNull()) {
        return StringUtils::ParseInt32(error);
    }

    return 0;
}

ECode ContentResolver::AcquireExistingProvider(
    /* [in] */ IContext* c,
    /* [in] */ const String& name,
    /* [out] */ IIContentProvider** contentProvider)
{
    return AcquireProvider(c, name, contentProvider);
}

ECode ContentResolver::AppNotRespondingViaProvider(
    /* [in] */ IIContentProvider* icp)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ContentResolver::GetType(
    /* [in] */ IUri* uri,
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type)
    *type = String(NULL);

    // XXX would like to have an acquireExistingUnstableProvider for this.
    AutoPtr<IIContentProvider> provider;
    FAIL_RETURN(AcquireExistingProvider(uri, (IIContentProvider**)&provider))
    if (NULL != provider) {
//        try {
        ECode ec = provider->GetType(uri, type);
//        } catch (RemoteException e) {
//            return null;
//        } catch (java.lang.Exception e) {
//            return null;
//        } finally {
        Boolean result;
        ReleaseProvider(provider, &result);
//        }
        FAIL_RETURN(ec)
    }

    String scheme;
    FAIL_RETURN(uri->GetScheme(&scheme))
    if (!IContentResolver::SCHEME_CONTENT.Equals(scheme)) {
        return NOERROR;
    }

//    try {
    AutoPtr<IUri> newUri;
    ContentProvider::GetUriWithoutUserId(uri, (IUri**)&newUri);
    Int32 userId = ResolveUserId(uri);
    assert(0 && "TODO");
    AutoPtr<IIActivityManager> am;// = ActivityManagerNative::GetDefault();
    return am->GetProviderMimeType(newUri, userId, type);
//    } catch (RemoteException e) {
//        // Arbitrary and not worth documenting, as Activity
//        // Manager will kill this process shortly anyway.
//        return null;
//    } catch (java.lang.Exception e) {
//        Log.w(TAG, "Failed to get type for: " + url + " (" + e.getMessage() + ")");
//        return null;
//    }
}

ECode ContentResolver::GetStreamTypes(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mimeTypeFilter,
    /* [out, callee] */ ArrayOf<String>** streamTypes)
{
    VALIDATE_NOT_NULL(streamTypes);
    *streamTypes = NULL;

    AutoPtr<IIContentProvider> provider;
    FAIL_RETURN(AcquireProvider(uri, (IIContentProvider**)&provider));
    if (NULL == provider) {
        return NOERROR;
    }

//    try {
    ECode ec = provider->GetStreamTypes(uri, mimeTypeFilter, streamTypes);
//    } catch (RemoteException e) {
//        // Arbitrary and not worth documenting, as Activity
//        // Manager will kill this process shortly anyway.
//        return null;
//    } finally {
    Boolean isRelease = FALSE;
    ReleaseProvider(provider, &isRelease);
//    }
    return ec;
}

ECode ContentResolver::Query(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [out] */ ICursor** cursor)
{
    return Query(uri, projection, selection, selectionArgs, sortOrder, NULL, cursor);
}

ECode ContentResolver::Query(
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

    AutoPtr<IIContentProvider> unstableProvider;
    FAIL_RETURN(AcquireUnstableProvider(uri, (IIContentProvider**)&unstableProvider))
    if (NULL == unstableProvider) {
        return NOERROR;
    }

    AutoPtr<IIContentProvider> stableProvider;
    AutoPtr<CursorWrapperInner> wrapper;
    Int32 count = 0;
    Boolean isRelease = FALSE;

//    try {
    Int64 startTime = SystemClock::GetUptimeMillis();
    Int64 durationMillis;

    AutoPtr<IICancellationSignal> remoteCancellationSignal;
    if (NULL != cancellationSignal) {
        cancellationSignal->ThrowIfCanceled();
        unstableProvider->CreateCancellationSignal((IICancellationSignal**)&remoteCancellationSignal);
        cancellationSignal->SetRemote(remoteCancellationSignal);
    }

    AutoPtr<ICursor> qCursor, temp;
    // try {
    ECode ec = unstableProvider->Query(mPackageName, uri, projection,
            selection, selectionArgs, sortOrder, remoteCancellationSignal, (ICursor**)&temp);

    if (ec == (ECode)E_DEAD_OBJECT_EXCEPTION) {
        // The remote process has died...  but we only hold an unstable
        // reference though, so we might recover!!!  Let's try!!!!
        // This is exciting!!1!!1!!!!1
        UnstableProviderDied(unstableProvider);
        stableProvider = NULL;
        AcquireProvider(uri, (IIContentProvider**)&stableProvider);
        if (stableProvider == NULL) {
            goto __EXIT__;
        }

        temp = NULL;
        ec = stableProvider->Query(mPackageName, uri, projection, selection, selectionArgs,
            sortOrder, remoteCancellationSignal, (ICursor**)&temp);
        FAIL_GOTO(ec, __EXIT__)
    }
    else if (FAILED(ec)) {
        goto __EXIT__;
    }

    if (qCursor == NULL) {
        goto __EXIT__;
    }

    // Force query execution.  Might fail and throw a runtime exception here.
    ec = qCursor->GetCount(&count);
    FAIL_GOTO(ec, __EXIT__)

    durationMillis = SystemClock::GetUptimeMillis() - startTime;
    MaybeLogQueryToEventLog(durationMillis, uri, projection, selection, sortOrder);

    // Wrap the cursor object into CursorWrapperInner object
    if (NULL == stableProvider) {
        ec = AcquireProvider(uri, (IIContentProvider**)&stableProvider);
        FAIL_GOTO(ec, __EXIT__)
    }
    assert(0 && "TODO");
    //wrapper = new CursorWrapperInner(qCursor, stableProvider, this);
    stableProvider = NULL;
    qCursor = NULL;
    goto __EXIT__;

__EXIT__:
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
       // Arbitrary and not worth documenting, as Activity
       // Manager will kill this process shortly anyway.
        return NOERROR;
    }

    if (qCursor != NULL) {
        ICloseable::Probe(qCursor)->Close();
    }

    if (cancellationSignal != NULL) {
        cancellationSignal->SetRemote(NULL);
    }

    if (NULL != unstableProvider) {
        ReleaseUnstableProvider(unstableProvider, &isRelease);
    }
    if (NULL != stableProvider) {
        ReleaseProvider(stableProvider, &isRelease);
    }
//    }
    *cursor = ICursor::Probe(wrapper);
    REFCOUNT_ADD(*cursor);
    return ec;
}

ECode ContentResolver::Canonicalize(
    /* [in] */ IUri* uri,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IIContentProvider> provider;
    AcquireProvider(uri, (IIContentProvider**)&provider);
    if (provider == NULL) {
        return NOERROR;
    }

    // try {
    ECode ec = provider->Canonicalize(mPackageName, uri, result);
    // } catch (RemoteException e) {
    //     // Arbitrary and not worth documenting, as Activity
    //     // Manager will kill this process shortly anyway.
    //     return null;
    // } finally {
    Boolean bval;
    ReleaseProvider(provider, &bval);
    // }

    return ec;
}

ECode ContentResolver::Uncanonicalize(
    /* [in] */ IUri* uri,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IIContentProvider> provider;
    AcquireProvider(uri, (IIContentProvider**)&provider);
    if (provider == NULL) {
        return NOERROR;
    }

    // try {
    ECode ec = provider->Uncanonicalize(mPackageName, uri, result);
    // } catch (RemoteException e) {
    //     // Arbitrary and not worth documenting, as Activity
    //     // Manager will kill this process shortly anyway.
    //     return null;
    // } finally {
    Boolean bval;
    ReleaseProvider(provider, &bval);
    // }

    return ec;
}

ECode ContentResolver::OpenInputStream(
    /* [in] */ IUri* uri,
    /* [out] */ IInputStream** inStream)
{
    VALIDATE_NOT_NULL(inStream);
    *inStream = NULL;

    String scheme;
    FAIL_RETURN(uri->GetScheme(&scheme));
    if (IContentResolver::SCHEME_ANDROID_RESOURCE.Equals(scheme)) {
        // Note: left here to avoid breaking compatibility.  May be removed
        // with sufficient testing.
        AutoPtr<IContentResolverOpenResourceIdResult> r;
        FAIL_RETURN(GetResourceId(uri, (IContentResolverOpenResourceIdResult**)&r));
//        try {
        AutoPtr<IResources> resources;
        FAIL_RETURN(r->GetResources((IResources**)&resources));
        Int32 id = 0;
        FAIL_RETURN(r->GetResourceId(&id));
        return resources->OpenRawResource(id, inStream);
//        } catch (Resources.NotFoundException ex) {
//            throw new FileNotFoundException("Resource does not exist: " + uri);
//        }
    }
    else if (SCHEME_FILE.Equals(scheme)) {
        // Note: left here to avoid breaking compatibility.  May be removed
        // with sufficient testing.
        String path;
        FAIL_RETURN(uri->GetPath(&path));
        AutoPtr<IFileInputStream> fi;
        CFileInputStream::New(path, (IFileInputStream**)&fi);
        *inStream = IInputStream::Probe(fi);
        REFCOUNT_ADD(*inStream);
        return NOERROR;
    }
    else {
        AutoPtr<IAssetFileDescriptor> fd;
        FAIL_RETURN(OpenAssetFileDescriptor(uri, String("r"), (IAssetFileDescriptor**)&fd));
//        try {
        if (fd == NULL) {
            return NOERROR;
        }
        else {
            AutoPtr<IFileInputStream> fi;
            fd->CreateInputStream((IFileInputStream**)&fi);
            *inStream = IInputStream::Probe(fi);
            REFCOUNT_ADD(*inStream);
            return NOERROR;
        }
//        } catch (IOException e) {
//            throw new FileNotFoundException("Unable to create stream");
//        }
    }
}

ECode ContentResolver::OpenOutputStream(
    /* [in] */ IUri* uri,
    /* [out] */ IOutputStream** outStream)
{
    return OpenOutputStream(uri, String("w"), outStream);
}

ECode ContentResolver::OpenOutputStream(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [out] */ IOutputStream** outStream)
{
    VALIDATE_NOT_NULL(outStream)
    *outStream = NULL;

    AutoPtr<IAssetFileDescriptor> fd;
    FAIL_RETURN(OpenAssetFileDescriptor(uri, mode, (IAssetFileDescriptor**)&fd))
//    try {
    if (NULL != fd) {
        AutoPtr<IFileOutputStream> fo;
        FAIL_RETURN(fd->CreateOutputStream((IFileOutputStream**)&fo))
        *outStream = IOutputStream::Probe(fo);
        REFCOUNT_ADD(*outStream)
    }

    return NOERROR;
//    } catch (IOException e) {
//        throw new FileNotFoundException("Unable to create stream");
//    }
}

ECode ContentResolver::OpenFileDescriptor(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [out] */ IParcelFileDescriptor** fileDescriptor)
{
    return OpenFileDescriptor(uri, mode, fileDescriptor);
}

ECode ContentResolver::OpenFileDescriptor(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [in] */ ICancellationSignal* signal,
    /* [out] */ IParcelFileDescriptor** fileDescriptor)
{
    VALIDATE_NOT_NULL(fileDescriptor)
    *fileDescriptor = NULL;

    AutoPtr<IAssetFileDescriptor> afd;
    FAIL_RETURN(OpenAssetFileDescriptor(uri, mode, signal, (IAssetFileDescriptor**)&afd))
    if (NULL == afd) {
        return NOERROR;
    }

    Int64 length = 0;
    FAIL_RETURN(afd->GetDeclaredLength(&length))
    if (length < 0) {
        // This is a full file!
        return afd->GetParcelFileDescriptor(fileDescriptor);
    }

    // Client can't handle a sub-section of a file, so close what
    // we got and bail with an exception.
    ICloseable::Probe(afd)->Close();

    // throw new FileNotFoundException("Not a whole file");
    return E_FILE_NOT_FOUND_EXCEPTION;
}

ECode ContentResolver::OpenAssetFileDescriptor(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [out] */ IAssetFileDescriptor** fileDescriptor)
{
    return OpenAssetFileDescriptor(uri, mode, NULL, fileDescriptor);
}

ECode ContentResolver::OpenAssetFileDescriptor(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ IAssetFileDescriptor** fileDescriptor)
{
    VALIDATE_NOT_NULL(fileDescriptor)
    String scheme;
    FAIL_RETURN(uri->GetScheme(&scheme))

    if (IContentResolver::SCHEME_ANDROID_RESOURCE.Equals(scheme)) {
        if (!mode.Equals("r")) {
            // throw new FileNotFoundException("Can't write resources: " + uri);
            return E_FILE_NOT_FOUND_EXCEPTION;
        }
        AutoPtr<IContentResolverOpenResourceIdResult> r;
        FAIL_RETURN(GetResourceId(uri, (IContentResolverOpenResourceIdResult**)&r));
//        try {
        AutoPtr<IResources> resources;
        FAIL_RETURN(r->GetResources((IResources**)&resources));
        Int32 id = 0;
        FAIL_RETURN(r->GetResourceId(&id));
        return resources->OpenRawResourceFd(id, fileDescriptor);
//        } catch (Resources.NotFoundException ex) {
//            throw new FileNotFoundException("Resource does not exist: " + uri);
//        }
    }
    else if (IContentResolver::SCHEME_FILE.Equals(scheme)) {
        AutoPtr<IFile> file;
        AutoPtr<IParcelFileDescriptor> pfd;
        String path;
        FAIL_RETURN(uri->GetPath(&path));
        FAIL_RETURN(CFile::New(path, (IFile**)&file));
        Int32 nMode;
        FAIL_RETURN(ParcelFileDescriptor::ParseMode(mode, &nMode));
        FAIL_RETURN(ParcelFileDescriptor::Open(file, nMode, (IParcelFileDescriptor**)&pfd));
        return CAssetFileDescriptor::New(pfd, 0, -1, fileDescriptor);
    }
    else {
        if (mode.Equals("r")) {
            return OpenTypedAssetFileDescriptor(uri, String("*/*"), NULL, cancellationSignal, fileDescriptor);
        }
        else {
            AutoPtr<ParcelFileDescriptorInner> pfdi;
            AutoPtr<IParcelFileDescriptor> pfd, pfd2;
            Int64 startOffset = 0, length = 0;

            AutoPtr<IIContentProvider> unstableProvider;
            FAIL_RETURN(AcquireUnstableProvider(uri, (IIContentProvider**)&unstableProvider));
            if (NULL == unstableProvider) {
                //throw new FileNotFoundException("No content provider: " + uri);
                return E_FILE_NOT_FOUND_EXCEPTION;
            }
            AutoPtr<IIContentProvider> stableProvider;
            AutoPtr<IAssetFileDescriptor> fd;
            Boolean isRelease = FALSE;

//            try {
            AutoPtr<IICancellationSignal> remoteCancellationSignal;
            if (cancellationSignal != NULL) {
                FAIL_RETURN(cancellationSignal->ThrowIfCanceled())
                FAIL_RETURN(unstableProvider->CreateCancellationSignal((IICancellationSignal**)&remoteCancellationSignal))
                FAIL_RETURN(cancellationSignal->SetRemote(remoteCancellationSignal))
            }
//                try {
            ECode ec = unstableProvider->OpenAssetFile(mPackageName, uri, mode, remoteCancellationSignal, (IAssetFileDescriptor**)&fd);
            if (ec == (ECode)E_DEAD_OBJECT_EXCEPTION) {
                // The remote process has died...  but we only hold an unstable
                // reference though, so we might recover!!!  Let's try!!!!
                // This is exciting!!1!!1!!!!1
                UnstableProviderDied(unstableProvider);
                stableProvider = NULL;
                AcquireProvider(uri, (IIContentProvider**)&stableProvider);
                if (stableProvider == NULL) {
                    // throw new FileNotFoundException("No content provider: " + uri);
                    return E_FILE_NOT_FOUND_EXCEPTION;
                }
                fd = NULL;
                stableProvider->OpenAssetFile(mPackageName, uri, mode, remoteCancellationSignal, (IAssetFileDescriptor**)&fd);
                if (fd == NULL) {
                    // The provider will be released by the finally{} clause
                    *fileDescriptor = NULL;
                    ec = NOERROR;
                    goto __RETURN__;
                }
            }
            else if (FAILED(ec)) {
                // The provider will be released by the finally{} clause
                *fileDescriptor = NULL;
                goto __EXIT__;
            }

            if (NULL == fd) {
                // The provider will be released by the finally{} clause
                *fileDescriptor = NULL;
                ec = NOERROR;
                goto __RETURN__;
            }

            if (NULL == stableProvider) {
                AcquireProvider(uri, (IIContentProvider**)&stableProvider);
            }

            ReleaseUnstableProvider(unstableProvider, &isRelease);

            fd->GetParcelFileDescriptor((IParcelFileDescriptor**)&pfd2);
            pfdi = new ParcelFileDescriptorInner();
            pfdi->constructor(pfd2, stableProvider, this);
            pfd = IParcelFileDescriptor::Probe(pfdi);

            // Success!  Don't release the provider when exiting, let
            // ParcelFileDescriptorInner do that when it is closed.
            stableProvider = NULL;

            fd->GetStartOffset(&startOffset);
            fd->GetDeclaredLength(&length);
//            } catch (RemoteException e) {
//                // Whatever, whatever, we'll go away.
//                throw new FileNotFoundException(
//                      array->Set(0, "Failed opening content provider: " + uri);
//            } catch (FileNotFoundException e) {
//                throw e;
//            } finally {

__EXIT__:
            if (NULL != stableProvider) {
                ReleaseProvider(stableProvider, &isRelease);
            }
            if (NULL != unstableProvider) {
                ReleaseUnstableProvider(unstableProvider, &isRelease);
            }

            FAIL_RETURN(ec)
//            }
            assert(0 && "TODO");
            // return CAssetFileDescriptor::New(pfd, startOffset, length, fileDescriptor);

__RETURN__:
            if (NULL != stableProvider) {
                ReleaseProvider(stableProvider, &isRelease);
            }
            if (NULL != unstableProvider) {
                ReleaseUnstableProvider(unstableProvider, &isRelease);
            }
            return ec;
        }
    }
}

ECode ContentResolver::OpenTypedAssetFileDescriptor(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mimeType,
    /* [in] */ IBundle* opts,
    /* [out] */ IAssetFileDescriptor** fileDescriptor)
{
    return OpenTypedAssetFileDescriptor(uri, mimeType, opts, NULL, fileDescriptor);
}

ECode ContentResolver::OpenTypedAssetFileDescriptor(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mimeType,
    /* [in] */ IBundle* opts,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ IAssetFileDescriptor** fileDescriptor)
{
    VALIDATE_NOT_NULL(fileDescriptor)
    *fileDescriptor = NULL;

    AutoPtr<IIContentProvider> unstableProvider;
    FAIL_RETURN(AcquireUnstableProvider(uri, (IIContentProvider**)&unstableProvider))
    if (NULL == unstableProvider) {
        //throw new FileNotFoundException("No content provider: " + uri);
        return E_FILE_NOT_FOUND_EXCEPTION;
    }
    AutoPtr<IIContentProvider> stableProvider;
    AutoPtr<IAssetFileDescriptor> fd;
    Boolean isRelease = FALSE;
    AutoPtr<ParcelFileDescriptorInner> pfdi;
    AutoPtr<IParcelFileDescriptor> pfd, pfd2;
    Int64 startOffset = 0;
    Int64 length = 0;

//    try {
    AutoPtr<IICancellationSignal> remoteCancellationSignal;
    if (cancellationSignal != NULL) {
        FAIL_RETURN(cancellationSignal->ThrowIfCanceled())
        FAIL_RETURN(unstableProvider->CreateCancellationSignal((IICancellationSignal**)&remoteCancellationSignal))
        FAIL_RETURN(cancellationSignal->SetRemote(remoteCancellationSignal))
    }
 //       try {
    ECode ec = unstableProvider->OpenTypedAssetFile(mPackageName, uri, mimeType, opts,
        remoteCancellationSignal, (IAssetFileDescriptor**)&fd);
    if (ec == (ECode)E_DEAD_OBJECT_EXCEPTION) {
        // The remote process has died...  but we only hold an unstable
        // reference though, so we might recover!!!  Let's try!!!!
        // This is exciting!!1!!1!!!!1
        UnstableProviderDied(unstableProvider);
        stableProvider = NULL;
        AcquireProvider(uri, (IIContentProvider**)&stableProvider);
        if (stableProvider == NULL) {
            // throw new FileNotFoundException("No content provider: " + uri);
            ec = E_FILE_NOT_FOUND_EXCEPTION;
            goto __EXIT__;
        }
        stableProvider->OpenTypedAssetFile(mPackageName,uri, mimeType, opts,
            remoteCancellationSignal, (IAssetFileDescriptor**)&fd);
        if (fd == NULL) {
            // The provider will be released by the finally{} clause
            goto __RETURN__;
        }
    }
    else if (FAILED(ec)) {
        goto __EXIT__;
    }

    if (fd == NULL) {
        // The provider will be released by the finally{} clause
        goto __RETURN__;
    }

    if (NULL == stableProvider) {
        AcquireProvider(uri, (IIContentProvider**)&stableProvider);
    }
    ReleaseUnstableProvider(unstableProvider, &isRelease);
    fd->GetParcelFileDescriptor((IParcelFileDescriptor**)&pfd2);

    pfdi = new ParcelFileDescriptorInner();
    pfdi->constructor(pfd2, stableProvider, this);
    pfd = IParcelFileDescriptor::Probe(pfdi);

    // Success!  Don't release the provider when exiting, let
    // ParcelFileDescriptorInner do that when it is closed.
    stableProvider = NULL;

    fd->GetStartOffset(&startOffset);
    fd->GetDeclaredLength(&length);
//    } catch (RemoteException e) {
//        // Whatever, whatever, we'll go away.
//        throw new FileNotFoundException(
//                "Failed opening content provider: " + uri);
//    } catch (FileNotFoundException e) {
//        throw e;
//    } finally {

__EXIT__:
    assert(0 && "TODO");
    if (FAILED(ec)) {
        ec = E_FILE_NOT_FOUND_EXCEPTION;
    }
    else {
        // ec = CAssetFileDescriptor::New(pfd, startOffset, length, fileDescriptor);
    }

    if (NULL != stableProvider) {
        ReleaseProvider(stableProvider, &isRelease);
    }
    if (NULL != unstableProvider) {
        ReleaseUnstableProvider(unstableProvider, &isRelease);
    }
//    }
    return ec;

__RETURN__:
    if (NULL != stableProvider) {
        ReleaseProvider(stableProvider, &isRelease);
    }
    if (NULL != unstableProvider) {
        ReleaseUnstableProvider(unstableProvider, &isRelease);
    }
    return NOERROR;
}

ECode ContentResolver::GetResourceId(
    /* [in] */ IUri* uri,
    /* [out] */ IContentResolverOpenResourceIdResult** resourceIdResult)
{
    VALIDATE_NOT_NULL(resourceIdResult)
    *resourceIdResult = NULL;

    String authority;
    FAIL_RETURN(uri->GetAuthority(&authority))
    AutoPtr<IResources> r;
    if (authority.IsNullOrEmpty()) {
        // throw new FileNotFoundException("No authority: " + uri);
        return E_FILE_NOT_FOUND_EXCEPTION;
    }
    else {
        AutoPtr<IPackageManager> packageManager;
        FAIL_RETURN(mContext->GetPackageManager((IPackageManager**)&packageManager))
        ECode ecode = packageManager->GetResourcesForApplication(authority, (IResources**)&r);
        if ((ECode)E_NAME_NOT_FOUND_EXCEPTION == ecode) {
            // throw new FileNotFoundException("No package found for authority: " + uri);
            return E_FILE_NOT_FOUND_EXCEPTION;
        }
    }
    AutoPtr<IList> path;
    FAIL_RETURN(uri->GetPathSegments((IList**)&path))
    if (NULL == path) {
        // throw new FileNotFoundException("No path: " + uri);
        return E_FILE_NOT_FOUND_EXCEPTION;
    }
    Int32 len;
    path->GetSize(&len);
    Int32 id = 0;
    if (len == 1) {
        AutoPtr<IInterface> obj;
        path->Get(0, (IInterface**)&obj);
        String str = Object::ToString(obj);
        // try {
        id = StringUtils::ParseInt32(str);
        // } catch (NumberFormatException e) {
        //     throw new FileNotFoundException("Single path segment is not a resource ID: " + uri);
        // }
    }
    else if (len == 2) {
        AutoPtr<IInterface> obj0, obj1;
        path->Get(0, (IInterface**)&obj0);
        path->Get(1, (IInterface**)&obj1);
        String str0 = Object::ToString(obj0);
        String str1 = Object::ToString(obj1);
        FAIL_RETURN(r->GetIdentifier(str0, str1, authority, &id));
    }
    else {
        // throw new FileNotFoundException("More than two path segments: " + uri);
        return E_FILE_NOT_FOUND_EXCEPTION;
    }
    if (id == 0) {
        // throw new FileNotFoundException("No resource found for: " + uri);
        return E_FILE_NOT_FOUND_EXCEPTION;
    }

    AutoPtr<IContentResolverOpenResourceIdResult> res;
    FAIL_RETURN(CContentResolverOpenResourceIdResult::New(
        r, id, (IContentResolverOpenResourceIdResult**)&res));
    *resourceIdResult = res;
    REFCOUNT_ADD(*resourceIdResult)
    return NOERROR;
}

ECode ContentResolver::Insert(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [out] */ IUri** insertedUri)
{
    VALIDATE_NOT_NULL(insertedUri)
    *insertedUri = NULL;

    AutoPtr<IIContentProvider> provider;
    FAIL_RETURN(AcquireProvider(uri, (IIContentProvider**)&provider))
    if (NULL == provider) {
        // throw new IllegalArgumentException("Unknown URL " + url);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
//    try {
    Int64 startTime = SystemClock::GetUptimeMillis();
    AutoPtr<IUri> createdRow;
    ECode ec = provider->Insert(mPackageName, uri, values, (IUri**)&createdRow);
    if (SUCCEEDED(ec)) {
        Int64 durationMillis = SystemClock::GetUptimeMillis() - startTime;
        MaybeLogUpdateToEventLog(durationMillis, uri, String("insert"), String(NULL) /* where */);
        *insertedUri = createdRow;
        REFCOUNT_ADD(*insertedUri)
    }
    else {
        *insertedUri = NULL;
    }
//    } catch (RemoteException e) {
//        // Arbitrary and not worth documenting, as Activity
//        // Manager will kill this process shortly anyway.
//        return null;
//    } finally {
    Boolean isRelease = FALSE;
    ReleaseProvider(provider, &isRelease);
//    }
    return (ec == (ECode)E_REMOTE_EXCEPTION) ? NOERROR : ec;
}

ECode ContentResolver::ApplyBatch(
    /* [in] */ const String& authority,
    /* [in] */ IArrayList* operations,
    /* [out, callee] */ ArrayOf<IContentProviderResult*>** providerResults)
{
    AutoPtr<IContentProviderClient> providerClient;
    *providerResults = NULL;

    FAIL_RETURN(AcquireContentProviderClient(authority, (IContentProviderClient**)&providerClient))
    if (NULL == providerClient) {
        //throw new IllegalArgumentException("Unknown authority " + authority);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    ECode ec = providerClient->ApplyBatch(operations, providerResults);
    Boolean isRelease = FALSE;
    providerClient->ReleaseProvider(&isRelease);
    return ec;
}

ECode ContentResolver::BulkInsert(
    /* [in] */ IUri* url,
    /* [in] */ ArrayOf<IContentValues*>* values,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = 0;

    AutoPtr<IIContentProvider> provider;
    FAIL_RETURN(AcquireProvider(url, (IIContentProvider**)&provider))
    if (NULL == provider) {
        // throw new IllegalArgumentException("Unknown URL " + url);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
//    try {
    Int64 startTime = SystemClock::GetUptimeMillis();
    Int32 rowsCreated = 0;
    provider->BulkInsert(mPackageName, url, values, &rowsCreated);
    Int64 durationMillis = SystemClock::GetUptimeMillis() - startTime;
    MaybeLogUpdateToEventLog(durationMillis, url, String("bulkinsert"), String(NULL) /* where */);
    *number = rowsCreated;
    Boolean isRelease = FALSE;
    ReleaseProvider(provider, &isRelease);
    return NOERROR;
//    } catch (RemoteException e) {
//        // Arbitrary and not worth documenting, as Activity
//        // Manager will kill this process shortly anyway.
//        return 0;
//    } finally {
//        releaseProvider(provider);
//    }
}

ECode ContentResolver::Delete(
    /* [in] */ IUri* uri,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* rowsAffected)
{
    VALIDATE_NOT_NULL(rowsAffected);
    *rowsAffected = -1;

    AutoPtr<IIContentProvider> provider;
    FAIL_RETURN(AcquireProvider(uri, (IIContentProvider**)&provider))
    if (NULL == provider) {
        // throw new IllegalArgumentException("Unknown URL " + url);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
//    try {
    Int64 startTime = SystemClock::GetUptimeMillis();
    Int32 rowsDeleted = 0;
    ECode ec = provider->Delete(mPackageName, uri, where, selectionArgs, &rowsDeleted);
    if (SUCCEEDED(ec)) {
        Int64 durationMillis = SystemClock::GetUptimeMillis() - startTime;
        MaybeLogUpdateToEventLog(durationMillis, uri, String("delete"), where);
        *rowsAffected = rowsDeleted;
    }

    Boolean isRelease = FALSE;
    ReleaseProvider(provider, &isRelease);
    return (ec == (ECode)E_REMOTE_EXCEPTION) ? NOERROR : ec;
//    } catch (RemoteException e) {
//        // Arbitrary and not worth documenting, as Activity
//        // Manager will kill this process shortly anyway.
//        return -1;
//    } finally {
//        releaseProvider(provider);
//    }
}

ECode ContentResolver::Update(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* rowsAffected)
{
    VALIDATE_NOT_NULL(rowsAffected);
    *rowsAffected = -1;

    AutoPtr<IIContentProvider> provider;
    FAIL_RETURN(AcquireProvider(uri, (IIContentProvider**)&provider))
    if (NULL == provider) {
        // throw new IllegalArgumentException("Unknown URL " + url);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
//    try {
    Int64 startTime = SystemClock::GetUptimeMillis();
    Int32 rowsUpdated = 0;
    ECode ec = provider->Update(mPackageName, uri, values, where, selectionArgs, &rowsUpdated);
    if (SUCCEEDED(ec)) {
        Int64 durationMillis = SystemClock::GetUptimeMillis() - startTime;
        MaybeLogUpdateToEventLog(durationMillis, uri, String("update"), where);
        *rowsAffected = rowsUpdated;
    }
    else {
        *rowsAffected = -1;
    }
    Boolean isRelease = FALSE;
    ReleaseProvider(provider, &isRelease);
    return (ec == (ECode)E_REMOTE_EXCEPTION) ? NOERROR : ec;
//    } catch (RemoteException e) {
//        // Arbitrary and not worth documenting, as Activity
//        // Manager will kill this process shortly anyway.
//        return -1;
//    } finally {
//        releaseProvider(provider);
//    }
}

ECode ContentResolver::Call(
    /* [in] */ IUri* uri,
    /* [in] */ const String& method,
    /* [in] */ const String& arg,
    /* [in] */ IBundle* extras,
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle)
    *bundle = NULL;

    if (NULL == uri) {
        //throw new NullPointerException("uri == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (method.IsNull()) {
        //throw new NullPointerException("method == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IIContentProvider> provider;
    FAIL_RETURN(AcquireProvider(uri, (IIContentProvider**)&provider))
    if (NULL == provider) {
        //throw new IllegalArgumentException("Unknown URI " + uri);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
//    try {
    ECode ec = provider->Call(mPackageName, method, arg, extras, bundle);
    Boolean isRelease = FALSE;
    ReleaseProvider(provider, &isRelease);
//    } catch (RemoteException e) {
//        // Arbitrary and not worth documenting, as Activity
//        // Manager will kill this process shortly anyway.
//        return null;
//    } finally {
//        releaseProvider(provider);
//    }
    return ec;
}

ECode ContentResolver::AcquireProvider(
    /* [in] */ IUri* uri,
    /* [out] */ IIContentProvider** contentProvider)
{
    VALIDATE_NOT_NULL(contentProvider)
    *contentProvider = NULL;

    String scheme;
    uri->GetScheme(&scheme);
    if (!IContentResolver::SCHEME_CONTENT.Equals(scheme)) {
        return NOERROR;
    }

    String auth;
    uri->GetAuthority(&auth);
    if (!auth.IsNull()) {
        return AcquireProvider(mContext, auth, contentProvider);
    }

    return NOERROR;
}

ECode ContentResolver::AcquireExistingProvider(
    /* [in] */ IUri* uri,
    /* [out] */ IIContentProvider** contentProvider)
{
    VALIDATE_NOT_NULL(contentProvider);
    *contentProvider = NULL;

    String scheme;
    uri->GetScheme(&scheme);
    if (!IContentResolver::SCHEME_CONTENT.Equals(scheme)) {
        return NOERROR;
    }

    String auth;
    uri->GetAuthority(&auth);
    if (!auth.IsNull()) {
        return AcquireExistingProvider(mContext, auth, contentProvider);
    }

    return NOERROR;
}

ECode ContentResolver::AcquireProvider(
    /* [in] */ const String& name,
    /* [out] */ IIContentProvider** contentProvider)
{
    VALIDATE_NOT_NULL(contentProvider)
    *contentProvider = NULL;

    if (name.IsNull()) {
        return NOERROR;
    }
    return AcquireProvider(mContext, name, contentProvider);
}

ECode ContentResolver::AcquireUnstableProvider(
    /* [in] */ IUri* uri,
    /* [out] */ IIContentProvider** contentProvider)
{
    VALIDATE_NOT_NULL(contentProvider)
    *contentProvider = NULL;

    String scheme;
    uri->GetScheme(&scheme);
    if (!IContentResolver::SCHEME_CONTENT.Equals(scheme)) {
        return NOERROR;
    }

    String auth;
    uri->GetAuthority(&auth);
    if (!auth.IsNull()) {
        return AcquireUnstableProvider(mContext, auth, contentProvider);
    }

    return NOERROR;
}

ECode ContentResolver::AcquireUnstableProvider(
    /* [in] */ const String& name,
    /* [out] */ IIContentProvider** contentProvider)
{
    VALIDATE_NOT_NULL(contentProvider)
    if (name.IsNull()) {
        *contentProvider = NULL;
        return NOERROR;
    }
    return AcquireUnstableProvider(mContext, name, contentProvider);
}

ECode ContentResolver::AcquireContentProviderClient(
    /* [in] */ IUri* uri,
    /* [out] */ IContentProviderClient** client)
{
    VALIDATE_NOT_NULL(client)
    *client = NULL;

    AutoPtr<IIContentProvider> provider;
    FAIL_RETURN(AcquireProvider(uri, (IIContentProvider**)&provider))
    if (NULL != provider) {
        return CContentProviderClient::New(THIS_PROBE(IContentResolver), provider, TRUE, client);
    }

    return NOERROR;
}

ECode ContentResolver::AcquireContentProviderClient(
    /* [in] */ const String& name,
    /* [out] */ IContentProviderClient** client)
{
    VALIDATE_NOT_NULL(client)
    *client = NULL;

    AutoPtr<IIContentProvider> provider;
    FAIL_RETURN(AcquireProvider(name, (IIContentProvider**)&provider))
    if (NULL != provider) {
        return CContentProviderClient::New(THIS_PROBE(IContentResolver), provider, TRUE, client);
    }

    return NOERROR;
}

ECode ContentResolver::AcquireUnstableContentProviderClient(
    /* [in] */ IUri* uri,
    /* [out] */ IContentProviderClient** client)
{
    VALIDATE_NOT_NULL(client)
    *client = NULL;

    AutoPtr<IIContentProvider> provider;
    FAIL_RETURN(AcquireUnstableProvider(uri, (IIContentProvider**)&provider))
    if (NULL != provider) {
        return CContentProviderClient::New(THIS_PROBE(IContentResolver), provider, FALSE, client);
    }

    return NOERROR;
}

ECode ContentResolver::AcquireUnstableContentProviderClient(
    /* [in] */ const String& name,
    /* [out] */ IContentProviderClient** client)
{
    VALIDATE_NOT_NULL(client);
    *client = NULL;

    AutoPtr<IIContentProvider> provider;
    FAIL_RETURN(AcquireUnstableProvider(name, (IIContentProvider**)&provider))
    if (NULL != provider) {
        return CContentProviderClient::New(THIS_PROBE(IContentResolver), provider, FALSE, client);
    }

    return NOERROR;
}

ECode ContentResolver::RegisterContentObserver(
    /* [in] */ IUri* uri,
    /* [in] */ Boolean notifyForDescendents,
    /* [in] */ IContentObserver* observer)
{
    return RegisterContentObserver(uri, notifyForDescendents, observer, UserHandle::GetMyUserId());
}

ECode ContentResolver::RegisterContentObserver(
    /* [in] */ IUri* uri,
    /* [in] */ Boolean notifyForDescendents,
    /* [in] */ IContentObserver* observer,
    /* [in] */ Int32 userHandle)
{
    VALIDATE_NOT_NULL(observer)
    AutoPtr<IIContentService> contentService;
    FAIL_RETURN(GetContentService((IIContentService**)&contentService));
    assert(contentService != NULL && "Error: Content Service not started.");
    AutoPtr<IIContentObserver> contentObserver;
    FAIL_RETURN(observer->GetContentObserver((IIContentObserver**)&contentObserver));

    return contentService->RegisterContentObserver(uri, notifyForDescendents, contentObserver, userHandle);
}

ECode ContentResolver::UnregisterContentObserver(
    /* [in] */ IContentObserver* observer)
{
    VALIDATE_NOT_NULL(observer);
    AutoPtr<IIContentObserver> contentObserver;
    FAIL_RETURN(observer->ReleaseContentObserver((IIContentObserver**)&contentObserver));
    if (NULL != contentObserver) {
        AutoPtr<IIContentService> contentService;
        FAIL_RETURN(GetContentService((IIContentService**)&contentService))
        return contentService->UnregisterContentObserver(contentObserver);
    }
    return NOERROR;
}

ECode ContentResolver::NotifyChange(
    /* [in] */ IUri* uri,
    /* [in] */ IContentObserver* observer)
{
    return NotifyChange(uri, observer, TRUE /* sync to network */);
}

ECode ContentResolver::NotifyChange(
    /* [in] */ IUri* uri,
    /* [in] */ IContentObserver* observer,
    /* [in] */ Boolean syncToNetwork)
{
    return NotifyChange(uri, observer, syncToNetwork, UserHandle::GetCallingUserId());
}

ECode ContentResolver::NotifyChange(
    /* [in] */ IUri* uri,
    /* [in] */ IContentObserver* observer,
    /* [in] */ Boolean syncToNetwork,
    /* [in] */ Int32 userHandle)
{
    AutoPtr<IIContentService> contentService;
    FAIL_RETURN(GetContentService((IIContentService**)&contentService))
    AutoPtr<IIContentObserver> contentObserver;
    Boolean deliverSelfNotification = FALSE;
    if (NULL != observer) {
        FAIL_RETURN(observer->GetContentObserver((IIContentObserver**)&contentObserver));
        FAIL_RETURN(observer->DeliverSelfNotifications(&deliverSelfNotification));
    }
    return contentService->NotifyChange(uri, contentObserver,
        (observer != NULL && deliverSelfNotification), syncToNetwork, userHandle);
}

ECode ContentResolver::TakePersistableUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags)
{
    AutoPtr<IIActivityManager> am;
    assert(0 && "TODO");
    // am = ActivityManagerNative::GetDefault();
    // try {
    AutoPtr<IUri> newUri;
    ContentProvider::GetUriWithoutUserId(uri, (IUri**)&newUri);
    return am->TakePersistableUriPermission(newUri, modeFlags, ResolveUserId(uri));
    // } catch (RemoteException e) {
    // }
}

ECode ContentResolver::ReleasePersistableUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags)
{
    AutoPtr<IIActivityManager> am;
    assert(0 && "TODO");
    // am = ActivityManagerNative::GetDefault();
    // try {
    AutoPtr<IUri> newUri;
    ContentProvider::GetUriWithoutUserId(uri, (IUri**)&newUri);
    return am->ReleasePersistableUriPermission(newUri, modeFlags, ResolveUserId(uri));
    // } catch (RemoteException e) {
    // }
    return NOERROR;
}

ECode ContentResolver::GetPersistedUriPermissions(
    /* [out] */ IList** perms)
{
    AutoPtr<IIActivityManager> am;
    assert(0 && "TODO");
    // am = ActivityManagerNative::GetDefault();
    // try {
    AutoPtr<IParceledListSlice> pls;
    FAIL_RETURN(am->GetPersistedUriPermissions(mPackageName, TRUE, (IParceledListSlice**)&pls))
    return pls->GetList(perms);
    // } catch (RemoteException e) {
    //     throw new RuntimeException("Activity manager has died", e);
    // }
}

ECode ContentResolver::GetOutgoingPersistedUriPermissions(
    /* [out] */ IList** perms)
{
    VALIDATE_NOT_NULL(perms)
    *perms = NULL;

    AutoPtr<IIActivityManager> am;
    assert(0 && "TODO");
    // am = ActivityManagerNative::GetDefault();
    // try {
    AutoPtr<IParceledListSlice> pls;
    FAIL_RETURN(am->GetPersistedUriPermissions(mPackageName, FALSE, (IParceledListSlice**)&pls))
    return pls->GetList(perms);
    // } catch (RemoteException e) {
    //     throw new RuntimeException("Activity manager has died", e);
    // }
    // return NOERROR;
}

ECode ContentResolver::StartSync(
    /* [in] */ IUri* uri,
    /* [in] */ IBundle* extras)
{
    AutoPtr<IAccount> account;
    if (NULL != extras) {
        String accountName;
        FAIL_RETURN(IBaseBundle::Probe(extras)->GetString(IContentResolver::SYNC_EXTRAS_ACCOUNT, &accountName))
        if (!accountName.IsNullOrEmpty()) {
            assert(0 && "TODO");
            //CAccount::New(accountName, String("com.google"), (IAccount**)&account);
        }
        FAIL_RETURN(IBaseBundle::Probe(extras)->Remove(IContentResolver::SYNC_EXTRAS_ACCOUNT))
    }
    String authority;
    if (NULL != uri) {
        uri->GetAuthority(&authority);
    }
    return RequestSync(account, authority, extras);
}

ECode ContentResolver::RequestSync(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ IBundle* extras)
{
    FAIL_RETURN(ValidateSyncExtrasBundle(extras))
    AutoPtr<IIContentService> contentService;
    FAIL_RETURN(GetContentService((IIContentService**)&contentService))
    return contentService->RequestSync(account, authority, extras);
}

ECode ContentResolver::RequestSyncAsUser(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ Int32 userId,
    /* [in] */ IBundle* extras)
{
    VALIDATE_NOT_NULL(extras)

    assert(0 && "TODO");
    AutoPtr<ISyncRequestBuilder> builder;
    // CSyncRequestBuilder::New((ISyncRequestBuilder**)&builder);
    builder->SetSyncAdapter(account, authority);
    builder->SetExtras(extras);
    builder->SyncOnce();
    AutoPtr<ISyncRequest> request;
    builder->Build((ISyncRequest**)&request);

    // try {
    AutoPtr<IIContentService> cs;
    GetContentService((IIContentService**)&cs);
    return cs->SyncAsUser(request, userId);
    // } catch(RemoteException e) {
    //     // Shouldn't happen.
    // }
}

ECode ContentResolver::RequestSync(
    /* [in] */ ISyncRequest* request)
{
    // try {
    AutoPtr<IIContentService> cs;
    GetContentService((IIContentService**)&cs);
    return cs->Sync(request);
    // } catch(RemoteException e) {
    //     // Shouldn't happen.
    // }
}

ECode ContentResolver::ValidateSyncExtrasBundle(
    /* [in] */ IBundle* extras)
{
    VALIDATE_NOT_NULL(extras)
    // try {
    AutoPtr<ISet> keySet;
    FAIL_RETURN(IBaseBundle::Probe(extras)->GetKeySet((ISet**)&keySet))
    if (NULL != keySet) {
        AutoPtr<IIterator> it;
        keySet->GetIterator((IIterator**)&it);
        Boolean hasNext = FALSE;
        while ((it->HasNext(&hasNext), hasNext)) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            String key = Object::ToString(obj);
            AutoPtr<IInterface> valueObj;
            FAIL_RETURN(IBaseBundle::Probe(extras)->Get(key, (IInterface**)&valueObj))
            if (NULL == valueObj) continue;
            if (IInteger64::Probe(valueObj) != NULL) continue;
            if (IInteger32::Probe(valueObj) != NULL) continue;
            if (IBoolean::Probe(valueObj) != NULL) continue;
            if (IFloat::Probe(valueObj) != NULL) continue;
            if (IDouble::Probe(valueObj) != NULL) continue;
            if (ICharSequence::Probe(valueObj) != NULL) continue;
            if (IAccount::Probe(valueObj) != NULL) continue;
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    return NOERROR;
    // } catch (IllegalArgumentException e) {
    //     throw e;
    // } catch (RuntimeException exc) {
    //     throw new IllegalArgumentException("error unparceling Bundle", exc);
    // }
}

ECode ContentResolver::CancelSync(
    /* [in] */ IUri* uri)
{
    String authority;
    if (NULL != uri) {
        uri->GetAuthority(&authority);
    }
    return CancelSync(NULL /* all accounts */, authority);
}

ECode ContentResolver::CancelSync(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority)
{
    AutoPtr<IIContentService> contentService;
    FAIL_RETURN(GetContentService((IIContentService**)&contentService))
    return contentService->CancelSync(account, authority, NULL);
}

ECode ContentResolver::CancelSyncAsUser(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ Int32 userId)
{
    // try {
    AutoPtr<IIContentService> cs;
    FAIL_RETURN(GetContentService((IIContentService**)&cs))
    return cs->CancelSyncAsUser(account, authority, NULL, userId);
    // } catch (RemoteException e) {
    // }
}

ECode ContentResolver::GetSyncAdapterTypes(
    /* [out, callee] */ ArrayOf<ISyncAdapterType*>** types)
{
    VALIDATE_NOT_NULL(types)
    AutoPtr<IIContentService> contentService;
    FAIL_RETURN(GetContentService((IIContentService**)&contentService))
    return contentService->GetSyncAdapterTypes(types);
}

ECode ContentResolver::GetSyncAdapterTypesAsUser(
    /* [in] */ Int32 userId,
    /* [out, callee] */ ArrayOf<ISyncAdapterType*>** types)
{
    VALIDATE_NOT_NULL(types)
    *types = NULL;
    // try {
    AutoPtr<IIContentService> cs;
    FAIL_RETURN(GetContentService((IIContentService**)&cs))
    return cs->GetSyncAdapterTypesAsUser(userId, types);
    // } catch (RemoteException e) {
    //     throw new RuntimeException("the ContentService should always be reachable", e);
    // }
    return NOERROR;
}

ECode ContentResolver::GetSyncAutomatically(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [out] */ Boolean* isSynced)
{
    VALIDATE_NOT_NULL(isSynced)
    AutoPtr<IIContentService> contentService;
    FAIL_RETURN(GetContentService((IIContentService**)&contentService))
    return contentService->GetSyncAutomatically(account, authority, isSynced);
}

ECode ContentResolver::GetSyncAutomaticallyAsUser(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* isSynced)
{
    VALIDATE_NOT_NULL(isSynced)
    // try {
    AutoPtr<IIContentService> cs;
    FAIL_RETURN(GetContentService((IIContentService**)&cs))
    return cs->GetSyncAutomaticallyAsUser(account, authority, userId, isSynced);
    // } catch (RemoteException e) {
    //     throw new RuntimeException("the ContentService should always be reachable", e);
    // }
}

ECode ContentResolver::SetSyncAutomatically(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ Boolean sync)
{
    AutoPtr<IIContentService> contentService;
    FAIL_RETURN(GetContentService((IIContentService**)&contentService))
    return contentService->SetSyncAutomatically(account, authority, sync);
}

ECode ContentResolver::SetSyncAutomaticallyAsUser(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean sync)
{
    // try {
    AutoPtr<IIContentService> cs;
    FAIL_RETURN(GetContentService((IIContentService**)&cs))
    return cs->SetSyncAutomaticallyAsUser(account, authority, sync, userId);
    // } catch (RemoteException e) {
    //     // exception ignored; if this is thrown then it means the runtime is in the midst of
    //     // being restarted
    // }
}

ECode ContentResolver::AddPeriodicSync(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ IBundle* extras,
    /* [in] */ Int64 pollFrequency)
{
    FAIL_RETURN(ValidateSyncExtrasBundle(extras))
    if (NULL == account) {
        ///throw new IllegalArgumentException("account must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (authority.IsNull()) {
        //throw new IllegalArgumentException("authority must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Boolean value = FALSE;
    if ((IBaseBundle::Probe(extras)->GetBoolean(IContentResolver::SYNC_EXTRAS_MANUAL, FALSE, &value), value)
        || (IBaseBundle::Probe(extras)->GetBoolean(IContentResolver::SYNC_EXTRAS_DO_NOT_RETRY, FALSE, &value), value)
        || (IBaseBundle::Probe(extras)->GetBoolean(IContentResolver::SYNC_EXTRAS_IGNORE_BACKOFF, FALSE, &value), value)
        || (IBaseBundle::Probe(extras)->GetBoolean(IContentResolver::SYNC_EXTRAS_IGNORE_SETTINGS, FALSE, &value), value)
        || (IBaseBundle::Probe(extras)->GetBoolean(IContentResolver::SYNC_EXTRAS_INITIALIZE, FALSE, &value), value)
        || (IBaseBundle::Probe(extras)->GetBoolean(IContentResolver::SYNC_EXTRAS_FORCE, FALSE, &value), value)
        || (IBaseBundle::Probe(extras)->GetBoolean(IContentResolver::SYNC_EXTRAS_EXPEDITED, FALSE, &value), value)) {
        //throw new IllegalArgumentException("illegal extras were set");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IIContentService> contentService;
    FAIL_RETURN(GetContentService((IIContentService**)&contentService))
    return contentService->AddPeriodicSync(account, authority, extras, pollFrequency);
}

ECode ContentResolver::InvalidPeriodicExtras(
    /* [in] */ IBundle* extras,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Boolean value;
    if ((IBaseBundle::Probe(extras)->GetBoolean(IContentResolver::SYNC_EXTRAS_MANUAL, FALSE, &value), value)
        || (IBaseBundle::Probe(extras)->GetBoolean(IContentResolver::SYNC_EXTRAS_DO_NOT_RETRY, FALSE, &value), value)
        || (IBaseBundle::Probe(extras)->GetBoolean(IContentResolver::SYNC_EXTRAS_IGNORE_BACKOFF, FALSE, &value), value)
        || (IBaseBundle::Probe(extras)->GetBoolean(IContentResolver::SYNC_EXTRAS_IGNORE_SETTINGS, FALSE, &value), value)
        || (IBaseBundle::Probe(extras)->GetBoolean(IContentResolver::SYNC_EXTRAS_INITIALIZE, FALSE, &value), value)
        || (IBaseBundle::Probe(extras)->GetBoolean(IContentResolver::SYNC_EXTRAS_FORCE, FALSE, &value), value)
        || (IBaseBundle::Probe(extras)->GetBoolean(IContentResolver::SYNC_EXTRAS_EXPEDITED, FALSE, &value), value)) {
        *result = TRUE;
    }

    return NOERROR;
}

ECode ContentResolver::RemovePeriodicSync(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ IBundle* extras)
{
    FAIL_RETURN(ValidateSyncExtrasBundle(extras))
    if (NULL == account) {
        ///throw new IllegalArgumentException("account must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (authority.IsNull()) {
        //throw new IllegalArgumentException("authority must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IIContentService> contentService;
    FAIL_RETURN(GetContentService((IIContentService**)&contentService))
    return contentService->RemovePeriodicSync(account, authority, extras);
}

ECode ContentResolver::CancelSync(
    /* [in] */ ISyncRequest* request)
{
    VALIDATE_NOT_NULL(request)

    // try {
    AutoPtr<IIContentService> cs;
    FAIL_RETURN(GetContentService((IIContentService**)&cs))
    return cs->CancelRequest(request);
    // } catch (RemoteException e) {
    //     // exception ignored; if this is thrown then it means the runtime is in the midst of
    //     // being restarted
    // }
}

ECode ContentResolver::GetPeriodicSyncs(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [out] */ IList** periodicSyncs)
{
    VALIDATE_NOT_NULL(periodicSyncs)
    *periodicSyncs = NULL;

    if (NULL == account) {
        // throw new IllegalArgumentException("account must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (authority.IsNull()) {
        // throw new IllegalArgumentException("authority must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IIContentService> contentService;
    FAIL_RETURN(GetContentService((IIContentService**)&contentService))
    return contentService->GetPeriodicSyncs(account, authority, periodicSyncs);
}

ECode ContentResolver::GetIsSyncable(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [out] */ Int32* isSyncable)
{
    VALIDATE_NOT_NULL(isSyncable)
    *isSyncable = 0;

    AutoPtr<IIContentService> contentService;
    FAIL_RETURN(GetContentService((IIContentService**)&contentService))
    return contentService->GetIsSyncable(account, authority, isSyncable);
}

ECode ContentResolver::GetIsSyncableAsUser(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* isSyncable)
{
    VALIDATE_NOT_NULL(isSyncable)
    *isSyncable = 0;

    // try {
    AutoPtr<IIContentService> cs;
    FAIL_RETURN(GetContentService((IIContentService**)&cs))
    return cs->GetIsSyncableAsUser(account, authority, userId, isSyncable);
    // } catch (RemoteException e) {
    //     throw new RuntimeException("the ContentService should always be reachable", e);
    // }
}

ECode ContentResolver::SetIsSyncable(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ Int32 syncable)
{
    AutoPtr<IIContentService> contentService;
    FAIL_RETURN(GetContentService((IIContentService**)&contentService))
    return contentService->SetIsSyncable(account, authority, syncable);
}

ECode ContentResolver::GetMasterSyncAutomatically(
    /* [out] */ Boolean* result)
{
    AutoPtr<IIContentService> contentService;
    FAIL_RETURN(GetContentService((IIContentService**)&contentService))
    return contentService->GetMasterSyncAutomatically(result);
}

ECode ContentResolver::GetMasterSyncAutomaticallyAsUser(
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    // try {
    AutoPtr<IIContentService> cs;
    FAIL_RETURN(GetContentService((IIContentService**)&cs))
    return cs->GetMasterSyncAutomaticallyAsUser(userId, result);
    // } catch (RemoteException e) {
    //     throw new RuntimeException("the ContentService should always be reachable", e);
    // }
}

ECode ContentResolver::SetMasterSyncAutomatically(
    /* [in] */ Boolean sync)
{
    AutoPtr<IIContentService> contentService;
    FAIL_RETURN(GetContentService((IIContentService**)&contentService))
    return contentService->SetMasterSyncAutomatically(sync);
}

ECode ContentResolver::SetMasterSyncAutomaticallyAsUser(
    /* [in] */ Int32 userId,
    /* [in] */ Boolean sync)
{
    // try {
    AutoPtr<IIContentService> cs;
    FAIL_RETURN(GetContentService((IIContentService**)&cs))
    return cs->SetMasterSyncAutomaticallyAsUser(sync, userId);
    // } catch (RemoteException e) {
    //     // exception ignored; if this is thrown then it means the runtime is in the midst of
    //     // being restarted
    // }
}

ECode ContentResolver::IsSyncActive(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [out] */ Boolean* isSyncActive)
{
    VALIDATE_NOT_NULL(isSyncActive)
    *isSyncActive = FALSE;

    AutoPtr<IIContentService> contentService;
    FAIL_RETURN(GetContentService((IIContentService**)&contentService))
    return contentService->IsSyncActive(account, authority, NULL, isSyncActive);
}

ECode ContentResolver::GetCurrentSync(
    /* [out] */ ISyncInfo** syncInfo)
{
    VALIDATE_NOT_NULL(syncInfo)
    *syncInfo = NULL;

    AutoPtr<IIContentService> contentService;
    FAIL_RETURN(GetContentService((IIContentService**)&contentService))
    AutoPtr<IList> syncInfoList;
    FAIL_RETURN(contentService->GetCurrentSyncs((IList**)&syncInfoList))
    Int32 count = 0;
    syncInfoList->GetSize(&count);
    if (0 == count) {
        *syncInfo = NULL;
        return NOERROR;
    }
    AutoPtr<IIterator> it;
    syncInfoList->GetIterator((IIterator**)&it);
    Boolean hasNext = FALSE;
    while ((it->HasNext(&hasNext), hasNext)) {
        AutoPtr<IInterface> valueObj;
        it->GetNext((IInterface**)&valueObj);
        *syncInfo = ISyncInfo::Probe(valueObj);
        REFCOUNT_ADD(*syncInfo);
        break;
    }
    return NOERROR;
}

ECode ContentResolver::GetCurrentSyncs(
    /* [out] */ IList** syncInfoList)
{
    VALIDATE_NOT_NULL(syncInfoList)
    *syncInfoList = NULL;

    AutoPtr<IIContentService> contentService;
    FAIL_RETURN(GetContentService((IIContentService**)&contentService))
    return contentService->GetCurrentSyncs(syncInfoList);
}

ECode ContentResolver::GetCurrentSyncsAsUser(
    /* [in] */ Int32 userId,
    /* [out] */ IList** syncInfoList)
{
    VALIDATE_NOT_NULL(syncInfoList)
    *syncInfoList = NULL;

    AutoPtr<IIContentService> cs;
    FAIL_RETURN(GetContentService((IIContentService**)&cs))
    // try {
    return cs->GetCurrentSyncsAsUser(userId, syncInfoList);
    // } catch (RemoteException e) {
    //     throw new RuntimeException("the ContentService should always be reachable", e);
    // }
}

ECode ContentResolver::GetSyncStatus(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [out] */ ISyncStatusInfo** syncStatusInfo)
{
    AutoPtr<IIContentService> contentService;
    FAIL_RETURN(GetContentService((IIContentService**)&contentService))
    return contentService->GetSyncStatus(account, authority, NULL, syncStatusInfo);
}

ECode ContentResolver::GetSyncStatusAsUser(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ Int32 userId,
    /* [out] */ ISyncStatusInfo** syncStatusInfo)
{
    VALIDATE_NOT_NULL(syncStatusInfo)
    *syncStatusInfo = NULL;
    // try {
    AutoPtr<IIContentService> cs;
    FAIL_RETURN(GetContentService((IIContentService**)&cs))
    return cs->GetSyncStatusAsUser(account, authority, NULL, userId, syncStatusInfo);
    // } catch (RemoteException e) {
    //     throw new RuntimeException("the ContentService should always be reachable", e);
    // }
}

ECode ContentResolver::IsSyncPending(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [out] */ Boolean* isSyncPending)
{
    return IsSyncPendingAsUser(account, authority, UserHandle::GetMyUserId(), isSyncPending);
}

ECode ContentResolver::IsSyncPendingAsUser(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* isSyncPending)
{
    VALIDATE_NOT_NULL(isSyncPending)
    *isSyncPending = FALSE;

    // try {
    AutoPtr<IIContentService> cs;
    FAIL_RETURN(GetContentService((IIContentService**)&cs))
    return cs->IsSyncPendingAsUser(account, authority, NULL, userId, isSyncPending);
    // } catch (RemoteException e) {
    //     throw new RuntimeException("the ContentService should always be reachable", e);
    // }
}

ECode ContentResolver::AddStatusChangeListener(
    /* [in] */ Int32 mask,
    /* [in] */ ISyncStatusObserver* observerCallback,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item)
    *item = NULL;

    if (NULL == observerCallback) {
        //throw new IllegalArgumentException("you passed in a null callback");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

//    try {
//        ISyncStatusObserver.Stub observer = new ISyncStatusObserver.Stub() {
//            public void onStatusChanged(int which) throws RemoteException {
//                callback.onStatusChanged(which);
//            }
//        };
//        getContentService().addStatusChangeListener(mask, observer);
//        return observer;
//    } catch (RemoteException e) {
//        throw new RuntimeException("the ContentService should always be reachable", e);
//    }
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode ContentResolver::RemoveStatusChangeListener(
    /* [in] */ IInterface* handle)
{
    if (NULL == handle) {
        //throw new IllegalArgumentException("you passed in a null handle");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IIContentService> contentService;
    FAIL_RETURN(GetContentService((IIContentService**)&contentService))
    return contentService->RemoveStatusChangeListener(IISyncStatusObserver::Probe(handle));
}

Int32 ContentResolver::SamplePercentForDuration(
    /* [in] */ Int64 durationMillis)
{
    if (durationMillis >= SLOW_THRESHOLD_MILLIS) {
        return 100;
    }
    return (Int32) (100 * durationMillis / SLOW_THRESHOLD_MILLIS) + 1;
}

ECode ContentResolver::MaybeLogQueryToEventLog(
    /* [in] */ Int64 durationMillis,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ const String& sortOrder)
{
    Int32 samplePercent = SamplePercentForDuration(durationMillis);
    if (samplePercent < 100) {
        AutoLock lock(ISynchronize::Probe(mRandom));

        Int32 rando;
        mRandom->NextInt32(100, &rando);
        if (rando >= samplePercent) {
            return NOERROR;
        }
    }

    StringBuilder projectionBuffer(100);
    if (NULL != projection) {
        for (Int32 i = 0; i < projection->GetLength(); ++i) {
            // Note: not using a comma delimiter here, as the
            // multiple arguments to EventLog.writeEvent later
            // stringify with a comma delimiter, which would make
            // parsing uglier later.
            if (i != 0) projectionBuffer += '/';
            projectionBuffer += (*projection)[i];
        }
    }

    // ActivityThread.currentPackageName() only returns non-null if the
    // current thread is an application main thread.  This parameter tells
    // us whether an event loop is blocked, and if so, which app it is.
    // String blockingPackage = AppGlobals::GetInitialPackage();

    // TODO:
//    EventLog.writeEvent(
//        EventLogTags.CONTENT_QUERY_SAMPLE,
//        uri.toString(),
//        projectionBuffer.toString(),
//        selection != null ? selection : "",
//        sortOrder != null ? sortOrder : "",
//        durationMillis,
//        blockingPackage != null ? blockingPackage : "",
//        samplePercent);
    return NOERROR;
}

ECode ContentResolver::MaybeLogUpdateToEventLog(
    /* [in] */ Int64 durationMillis,
    /* [in] */ IUri* uri,
    /* [in] */ const String& operation,
    /* [in] */ const String& selection)
{
    Int32 samplePercent = SamplePercentForDuration(durationMillis);
    if (samplePercent < 100) {
        AutoLock lock(ISynchronize::Probe(mRandom));

        Int32 rando;
        mRandom->NextInt32(100, &rando);
        if (rando >= samplePercent) {
            return NOERROR;
        }
    }
    // String blockingPackage = AppGlobals::GetInitialPackage();

    //TODO:
//    EventLog.writeEvent(
//        EventLogTags.CONTENT_UPDATE_SAMPLE,
//        uri.toString(),
//        operation,
//        selection != null ? selection : "",
//        durationMillis,
//        blockingPackage != null ? blockingPackage : "",
//        samplePercent);
    return NOERROR;
}

ECode ContentResolver::GetContentService(
    /* [out] */ IIContentService** contentService)
{
    VALIDATE_NOT_NULL(contentService)

    if (NULL == sContentService) {
        sContentService = IIContentService::Probe(ServiceManager::GetService(IContentResolver::CONTENT_SERVICE_NAME));
    }

    // if (false) Log.v("ContentService", "default service binder = " + b);
    *contentService = sContentService;
    // if (false) Log.v("ContentService", "default service = " + sContentService);
    REFCOUNT_ADD(*contentService)
    return NOERROR;
}

String ContentResolver::GetPackageName()
{
    return mPackageName;
}

Int32 ContentResolver::ResolveUserId(
    /* [in] */ IUri* uri)
{
    Int32 userId;
    mContext->GetUserId(&userId);
    return ContentProvider::GetUserIdFromUri(uri, userId);
}

} // namespace Content
} // namespace Droid
} // namespace Elastos
