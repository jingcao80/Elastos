#include "elastos/providers/mediaprovider/media/MediaProvider.h"
#include "elastos/providers/mediaprovider/media/MediaScannerService.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/Arrays.h>
#include <elastos/droid/os/Process.h>
#include <elastos/droid/os/Looper.h>
#include <elastos/droid/os/Environment.h>
#include <elastos/core/Thread.h>
#include <elastos/core/AutoLock.h>

using Elastos::Droid::App::IService;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::CMediaScanner;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::Provider::CMediaStore;
using Elastos::Droid::Provider::IMediaStore;
using Elastos::Utility::Arrays;
using Elastos::Utility::ILocale;
using Elastos::Utility::Logging::Slogger;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::Core::AutoLock;
using Elastos::Core::CThread;
using Elastos::Core::IThread;
using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Providers {
namespace MediaProvider {
namespace Media {

const String MediaScannerService::TAG(String("MediaScannerService"));
AutoPtr<ArrayOf<String> > MediaScannerService::mExternalStoragePaths;

CAR_INTERFACE_IMPL_2(MediaScannerService, Service, IMediaScannerService, IRunnable)

void MediaScannerService::OpenDatabase(
    /* [in] */ const String& volumeName)
{
    // try {
    ECode ec = NOERROR;
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    ec = values->Put(String("name"), volumeName);
    if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        Slogger::W(TAG, "failed to open media database");
    }
    AutoPtr<IContentResolver> resolver;
    ec = IContext::Probe(this)->GetContentResolver((IContentResolver**)&resolver);
    if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        Slogger::W(TAG, "failed to open media database");
    }
    AutoPtr<IUriHelper> uh;
    CUriHelper::AcquireSingleton((IUriHelper**)&uh);
    AutoPtr<IUri> uri;
    ec = uh->Parse(String("content://media/"), (IUri**)&uri);
    if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        Slogger::W(TAG, "failed to open media database");
    }
    AutoPtr<IUri> oUri;
    ec = resolver->Insert(uri.Get(), values.Get(), (IUri**)&oUri);
    if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        Slogger::W(TAG, "failed to open media database");
    }
// } catch (IllegalArgumentException ex) {
    // Log.w(TAG, "failed to open media database");
// }

}

AutoPtr<IMediaScanner> MediaScannerService::CreateMediaScanner()
{
    AutoPtr<IMediaScanner> scanner;
    CMediaScanner::New(IContext::Probe(this), (IMediaScanner**)&scanner);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IConfiguration> cfg;
    res->GetConfiguration((IConfiguration**)&cfg);
    AutoPtr<ILocale> locale;
    cfg->GetLocale((ILocale**)&locale);

    if (locale != NULL) {
        String language;
        locale->GetLanguage(&language);
        String country;
        locale->GetCountry(&country);
        String localeString;
        if (!language.IsNull()) {
            if (!country.IsNull()) {
                scanner->SetLocale(language + "_" + country);
            } else {
                scanner->SetLocale(language);
            }
        }
    }

    return scanner;
}

void MediaScannerService::Scan(
    /* [in] */ ArrayOf<String>* directories,
    /* [in] */ const String& volumeName)
{
    AutoPtr<IUri> uri;
    AutoPtr<IUriHelper> uh;
    CUriHelper::AcquireSingleton((IUriHelper**)&uh);
    uh->Parse(String("file://") + (*directories)[0], (IUri**)&uri);
    // don't sleep while scanning
    mWakeLock->AcquireLock();

    // try {
        AutoPtr<IContentValues> values;
        CContentValues::New((IContentValues**)&values);
        values->Put(IMediaStore::MEDIA_SCANNER_VOLUME, volumeName);
        // Uri scanUri = getContentResolver().insert(MediaStore.getMediaScannerUri(), values);
        AutoPtr<IContentResolver> resolver;
        GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<IMediaStore> ms;
        CMediaStore::AcquireSingleton((IMediaStore**)&ms);
        AutoPtr<IUri> iUri;
        ms->GetMediaScannerUri((IUri**)&iUri);
        AutoPtr<IUri> scanUri;
        resolver->Insert(iUri.Get(), values.Get(), (IUri**)&scanUri);
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_MEDIA_SCANNER_STARTED, uri, (IIntent**)&intent);
        SendBroadcast(intent.Get());

        // try {
            if (volumeName.Equals(MediaProvider::EXTERNAL_VOLUME)) {
                OpenDatabase(volumeName);
            }

            AutoPtr<IMediaScanner> scanner = CreateMediaScanner();
            ECode ec = scanner->ScanDirectories(directories, volumeName);
        // } catch (Exception e) {
            if (!SUCCEEDED(ec)) {
                Slogger::E(TAG, "exception in MediaScanner.scan()%0x", ec);
            }

        // }
        Int32 vol;
        resolver->Delete(scanUri.Get(), String(NULL), NULL, &vol);

    // } finally {
        intent = NULL;
        CIntent::New(IIntent::ACTION_MEDIA_SCANNER_FINISHED, uri, (IIntent**)&intent);
        SendBroadcast(intent.Get());
        mWakeLock->ReleaseLock();
    // }
}

ECode MediaScannerService::OnCreate()
{
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(obj);
    mWakeLock = NULL;
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, TAG, (IPowerManagerWakeLock**)&mWakeLock);
    obj = NULL;
    GetSystemService(IContext::STORAGE_SERVICE, (IInterface**)&obj);
    AutoPtr<IStorageManager> storageManager = IStorageManager::Probe(obj);
    mExternalStoragePaths = NULL;
    storageManager->GetVolumePaths((ArrayOf<String>**)&mExternalStoragePaths);

    // Start up the thread running the service.  Note that we create a
    // separate thread because the service normally runs in the process's
    // main thread, which we don't want to block.
    AutoPtr<IThread> thr;
    CThread::New(NULL, IRunnable::Probe(this), String("MediaScannerService"), (IThread**)&thr);
    return thr->Start();
}

ECode MediaScannerService::OnStartCommand(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 startId,
    /* [out] */ Int32* result)
{
    while (mServiceHandler == NULL) {
        synchronized(this) {
            // try {
                Wait(100);
            // } catch (InterruptedException e) {
            // }
        }
    }

    if (intent == NULL) {
        Slogger::E(TAG, "Intent is null in onStartCommand: %0x", E_NULL_POINTER_EXCEPTION);
        *result = IService::START_NOT_STICKY;
        return NOERROR;
    }

    AutoPtr<IMessage> msg;
    IHandler::Probe(mServiceHandler)->ObtainMessage((IMessage**)&msg);
    msg->SetArg1(startId);
    AutoPtr<IBundle> bd;
    intent->GetExtras((IBundle**)&bd);
    msg->SetObj(bd.Get());
    Boolean flag = FALSE;
    IHandler::Probe(mServiceHandler)->SendMessage(msg, &flag);

    // Try again later if we are killed before we can finish scanning.
    *result = IService::START_REDELIVER_INTENT;
    return NOERROR;
}

ECode MediaScannerService::OnDestroy()
{
    // Make sure thread has started before telling it to quit.
    while (mServiceLooper == NULL) {
        synchronized(this) {
            // try {
                Wait(100);
            // } catch (InterruptedException e) {
            // }
        }
    }
    return mServiceLooper->Quit();
}

ECode MediaScannerService::Run()
{
    // reduce priority below other background threads to avoid interfering
    // with other services at boot time.
    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_BACKGROUND +
            IProcess::THREAD_PRIORITY_LESS_FAVORABLE);
    Looper::Prepare();
    mServiceLooper = Looper::GetMyLooper();
    mServiceHandler = new ServiceHandler(this);
    return Looper::Loop();
}

ECode MediaScannerService::ScanFile(
    /* [in] */ const String& path,
    /* [in] */ const String& mimeType,
    /* [out] */ IUri** result)
{
    String volumeName = MediaProvider::EXTERNAL_VOLUME;
    OpenDatabase(volumeName);
    AutoPtr<IMediaScanner> scanner = CreateMediaScanner();
    // try {
        // make sure the file path is in canonical form
        AutoPtr<IFile> file;
        CFile::New(path, (IFile**)&file);
        String canonicalPath;
        ECode ec = NOERROR;
        ec = file->GetCanonicalPath(&canonicalPath);
        FAIL_GOTO(ec, _EXIT_);
        ec = scanner->ScanSingleFile(canonicalPath, volumeName, mimeType, result);
        if (SUCCEEDED(ec)) {
            return ec;
        }
        FAIL_GOTO(ec, _EXIT_);

    _EXIT_:
    // } catch (Exception e) {
        Slogger::E(TAG, "bad path %s in scanFile()%0x", path.string(), ec);
        *result = NULL;
        return NOERROR;
    // }
}

ECode MediaScannerService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> obj = TO_IINTERFACE(mBinder);
    *result = IBinder::Probe(obj);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//---------------------------------------------------
//      MediaScannerService::MyMediaScannerService
//---------------------------------------------------
MediaScannerService::MyMediaScannerService::MyMediaScannerService(
    /* [in] */ MediaScannerService* owner)
    : mOwner(owner)
{}

CAR_INTERFACE_IMPL(MediaScannerService::MyMediaScannerService, Object, IIMediaScannerService)

ECode MediaScannerService::MyMediaScannerService::RequestScanFile(
    /* [in] */ const String& path,
    /* [in] */ const String& mimeType,
    /* [in] */ IIMediaScannerListener* listener)
{
    if (FALSE) {
        Slogger::D(TAG, "IMediaScannerService.scanFile: %s mimeType: ", path.string(), mimeType.string());
    }
    AutoPtr<IBundle> args;
    CBundle::New((IBundle**)&args);
    args->PutString(String("filepath"), path);
    args->PutString(String("mimetype"), mimeType);
    if (listener != NULL) {
        AutoPtr<IInterface> obj = TO_IINTERFACE(listener);
        AutoPtr<IBinder> bdListener = IBinder::Probe(obj);
        args->PutIBinder(String("listener"), bdListener.Get());
    }
    AutoPtr<IIntent> intent;
    CIntent::New(IContext::Probe(this), ECLSID_CMediaScannerService, (IIntent**)&intent);
    intent->PutExtras(args.Get());
    AutoPtr<IComponentName> cn;
    return IContext::Probe(mOwner)->StartService(intent.Get(), (IComponentName**)&cn);
}

ECode MediaScannerService::MyMediaScannerService::ScanFile(
    /* [in] */ const String& path,
    /* [in] */ const String& mimeType)
{
    return RequestScanFile(path, mimeType, NULL);
}

//---------------------------------------------------
//      MediaScannerService::ServiceHandler
//---------------------------------------------------
MediaScannerService::ServiceHandler::ServiceHandler(
    /* [in] */ MediaScannerService* owner)
    : mOwner(owner)
{}

CAR_INTERFACE_IMPL(MediaScannerService::ServiceHandler, Handler, IHandler)

ECode MediaScannerService::ServiceHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<IBundle> arguments = IBundle::Probe(obj);
    String filePath;
    arguments->GetString(String("filepath"), &filePath);

    // try {
        if (!filePath.IsNull()) {
            AutoPtr<IBinder> binder;
            arguments->GetIBinder(String("listener"), (IBinder**)&binder);
            AutoPtr<IIMediaScannerListener> listener =
                    (binder == NULL ? NULL : IIMediaScannerListener::Probe(binder));
            AutoPtr<IUri> uri;
            // try {
                String mimeType;
                ECode ec = NOERROR;
                ec = arguments->GetString(String("mimetype"), &mimeType);
                FAIL_GOTO(ec, _EXIT_);
                ec = mOwner->ScanFile(filePath, mimeType, (IUri**)&uri);
                FAIL_GOTO(ec, _EXIT_);
            _EXIT_:
            {
            // } catch (Exception e) {
                // Log.e(TAG, "Exception scanning file", e);
            // }
                Slogger::E(TAG, "Exception scanning file");
            }

            if (listener != NULL) {
                listener->ScanCompleted(filePath, uri.Get());
            }
        } else {
            String volume;
            arguments->GetString(String("volume"), &volume);
            AutoPtr<ArrayOf<String> > directories = ArrayOf<String>::Alloc(2);

            if (MediaProvider::INTERNAL_VOLUME.Equals(volume)) {
                // scan internal media storage
                String rootDirectory;
                AutoPtr<IFile> rdf = Environment::GetRootDirectory();
                rdf->ToString(&rootDirectory);
                String oemDirectory;
                AutoPtr<IFile> odf = Environment::GetOemDirectory();
                odf->ToString(&oemDirectory);
                (*directories)[0] = rootDirectory + String("/media");
                (*directories)[1] = oemDirectory + String("/media");
            }
            else if (MediaProvider::EXTERNAL_VOLUME.Equals(volume)) {
                // scan external storage volumes
                Int32 esLength = mExternalStoragePaths->GetLength();
                for (Int32 i = 0; i < esLength; ++i) {
                    directories->Set(i, (*mExternalStoragePaths)[i]);
                }
            }

            if (directories != NULL) {
                String str;
                str = Arrays::ToString(directories);
                if (FALSE) Slogger::D(TAG, "start scanning volume %s: %s", volume.string(), str.string());
                mOwner->Scan(directories.Get(), volume);
                if (FALSE) Slogger::D(TAG, "done scanning volume %s", volume.string());
            }
        }
    // } catch (Exception e) {
        Slogger::E(TAG, "Exception in handleMessage");
    // }

    Int32 arg1;
    msg->GetArg1(&arg1);
    IService::Probe(this)->StopSelf(arg1);
}

} // namespace Media
} // namespace MediaProvider
} // namespace Providers
} // namespace Elastos