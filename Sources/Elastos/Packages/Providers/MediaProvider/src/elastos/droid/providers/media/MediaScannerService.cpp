//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/providers/media/MediaProvider.h"
#include "elastos/droid/providers/media/MediaScannerService.h"
#include "elastos/droid/providers/media/CIMediaScannerService.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/utility/logging/Logger.h>
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
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Provider::CMediaStore;
using Elastos::Droid::Provider::IMediaStore;
using Elastos::Utility::Arrays;
using Elastos::Utility::ILocale;
using Elastos::Utility::Logging::Logger;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::Core::AutoLock;
using Elastos::Core::CThread;
using Elastos::Core::IThread;
using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

const String MediaScannerService::TAG("MediaScannerService");

AutoPtr<ArrayOf<String> > MediaScannerService::mExternalStoragePaths;

CAR_INTERFACE_IMPL_2(MediaScannerService, Service, IMediaScannerService, IRunnable)

ECode MediaScannerService::constructor()
{
    CIMediaScannerService::New(this, (IIMediaScannerService**)&mBinder);
    return Service::constructor();
}

void MediaScannerService::OpenDatabase(
    /* [in] */ const String& volumeName)
{
    // try {
    ECode ec = NOERROR;
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    ec = values->Put(String("name"), volumeName);
    if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        Logger::W(TAG, "failed to open media database");
    }
    AutoPtr<IContentResolver> resolver;
    ec = GetContentResolver((IContentResolver**)&resolver);
    if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        Logger::W(TAG, "failed to open media database");
    }
    AutoPtr<IUriHelper> uh;
    CUriHelper::AcquireSingleton((IUriHelper**)&uh);
    AutoPtr<IUri> uri;
    ec = uh->Parse(String("content://media/"), (IUri**)&uri);
    if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        Logger::W(TAG, "failed to open media database");
    }
    AutoPtr<IUri> oUri;
    ec = resolver->Insert(uri, values, (IUri**)&oUri);
    if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        Logger::W(TAG, "failed to open media database");
    }
// } catch (IllegalArgumentException ex) {
    // Log.w(TAG, "failed to open media database");
// }

}

AutoPtr<IMediaScanner> MediaScannerService::CreateMediaScanner()
{
    AutoPtr<IMediaScanner> scanner;
    CMediaScanner::New((IContext*)this, (IMediaScanner**)&scanner);
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

    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(IMediaStore::MEDIA_SCANNER_VOLUME, volumeName);
    AutoPtr<IContentResolver> resolver;
    GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IMediaStore> ms;
    CMediaStore::AcquireSingleton((IMediaStore**)&ms);
    AutoPtr<IUri> iUri;
    ms->GetMediaScannerUri((IUri**)&iUri);
    AutoPtr<IUri> scanUri;
    resolver->Insert(iUri, values, (IUri**)&scanUri);
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_MEDIA_SCANNER_STARTED, uri, (IIntent**)&intent);
    SendBroadcast(intent);

    // try {
        if (volumeName.Equals(MediaProvider::EXTERNAL_VOLUME)) {
            OpenDatabase(volumeName);
        }

        AutoPtr<IMediaScanner> scanner = CreateMediaScanner();
        ECode ec = scanner->ScanDirectories(directories, volumeName);
    // } catch (Exception e) {
        if (FAILED(ec)) {
            Logger::E(TAG, "exception in MediaScanner.scan()%0x", ec);
        }

    // }
    Int32 vol;
    resolver->Delete(scanUri, String(NULL), NULL, &vol);

    intent = NULL;
    CIntent::New(IIntent::ACTION_MEDIA_SCANNER_FINISHED, uri, (IIntent**)&intent);
    SendBroadcast(intent);
    mWakeLock->ReleaseLock();
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
    CThread::New(NULL, (IRunnable*)this, String("MediaScannerService"), (IThread**)&thr);
    return thr->Start();
}

ECode MediaScannerService::OnStartCommand(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 startId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    while (mServiceHandler == NULL) {
        {
            AutoLock syncLock(this);
            // try {
                Wait(100);
            // } catch (InterruptedException e) {
            // }
        }
    }

    if (intent == NULL) {
        Logger::E(TAG, "Intent is null in onStartCommand: %0x", E_NULL_POINTER_EXCEPTION);
        *result = IService::START_NOT_STICKY;
        return NOERROR;
    }

    AutoPtr<IMessage> msg;
    mServiceHandler->ObtainMessage((IMessage**)&msg);
    msg->SetArg1(startId);
    AutoPtr<IBundle> bd;
    intent->GetExtras((IBundle**)&bd);
    msg->SetObj(bd);
    Boolean flag = FALSE;
    mServiceHandler->SendMessage(msg, &flag);

    // Try again later if we are killed before we can finish scanning.
    *result = IService::START_REDELIVER_INTENT;
    return NOERROR;
}

ECode MediaScannerService::OnDestroy()
{
    // Make sure thread has started before telling it to quit.
    while (mServiceLooper == NULL) {
        {
            AutoLock syncLock(this);
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
    mServiceHandler->constructor();
    return Looper::Loop();
}

ECode MediaScannerService::ScanFile(
    /* [in] */ const String& path,
    /* [in] */ const String& mimeType,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
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
        Logger::E(TAG, "bad path %s in scanFile()%0x", path.string(), ec);
        *result = NULL;
        return NOERROR;
    // }
}

ECode MediaScannerService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** result)
{
    VALIDATE_NOT_NULL(result)
    *result = IBinder::Probe(mBinder);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//===========================================================
// MediaScannerService::MyMediaScannerService
//===========================================================
CAR_INTERFACE_IMPL_2(MediaScannerService::MyMediaScannerService, Object, IIMediaScannerService, IBinder)

MediaScannerService::MyMediaScannerService::MyMediaScannerService()
{}

MediaScannerService::MyMediaScannerService::~MyMediaScannerService()
{}

ECode MediaScannerService::MyMediaScannerService::constructor(
    /* [in] */ IMediaScannerService* owner)
{
    mOwner = (MediaScannerService*)owner;
    return NOERROR;
}

ECode MediaScannerService::MyMediaScannerService::RequestScanFile(
    /* [in] */ const String& path,
    /* [in] */ const String& mimeType,
    /* [in] */ IIMediaScannerListener* listener)
{
    if (FALSE) {
        Logger::D(TAG, "IMediaScannerService.scanFile: %s mimeType: ", path.string(), mimeType.string());
    }
    AutoPtr<IBundle> args;
    CBundle::New((IBundle**)&args);
    args->PutString(String("filepath"), path);
    args->PutString(String("mimetype"), mimeType);
    if (listener != NULL) {
        AutoPtr<IBinder> bdListener = IBinder::Probe(listener);
        args->PutIBinder(String("listener"), bdListener);
    }
    AutoPtr<IIntent> intent;
    CIntent::New((IContext*)this, ECLSID_CMediaScannerService, (IIntent**)&intent);
    intent->PutExtras(args);
    AutoPtr<IComponentName> cn;
    return ((IContext*)((IMediaScannerService*)mOwner))->StartService(intent, (IComponentName**)&cn);
}

ECode MediaScannerService::MyMediaScannerService::ScanFile(
    /* [in] */ const String& path,
    /* [in] */ const String& mimeType)
{
    return RequestScanFile(path, mimeType, NULL);
}

ECode MediaScannerService::MyMediaScannerService::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return NOERROR;
}

//===========================================================
// MediaScannerService::ServiceHandler
//===========================================================
MediaScannerService::ServiceHandler::ServiceHandler(
    /* [in] */ MediaScannerService* owner)
    : mOwner(owner)
{
    Handler::constructor();
}

ECode MediaScannerService::ServiceHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<IBundle> arguments = IBundle::Probe(obj);
    String filePath;
    arguments->GetString(String("filepath"), &filePath);

    if (!filePath.IsNull()) {
        AutoPtr<IBinder> binder;
        arguments->GetIBinder(String("listener"), (IBinder**)&binder);
        AutoPtr<IIMediaScannerListener> listener = IIMediaScannerListener::Probe(binder);
        AutoPtr<IUri> uri;
        String mimeType;
        ECode ec = arguments->GetString(String("mimetype"), &mimeType);
        FAIL_GOTO(ec, _EXIT_);
        ec = mOwner->ScanFile(filePath, mimeType, (IUri**)&uri);
        FAIL_GOTO(ec, _EXIT_);

        _EXIT_:
        if (FAILED(ec)) {
            Logger::E(TAG, "Exception scanning file, ec=%08x", ec);
        }

        if (listener != NULL) {
            listener->ScanCompleted(filePath, uri);
        }
    }
    else {
        String volume;
        arguments->GetString(String("volume"), &volume);
        AutoPtr<ArrayOf<String> > directories = ArrayOf<String>::Alloc(2);

        if (MediaProvider::INTERNAL_VOLUME.Equals(volume)) {
            // scan internal media storage
            String rootDirectory, oemDirectory;
            AutoPtr<IFile> rdf = Environment::GetRootDirectory();
            AutoPtr<IFile> odf = Environment::GetOemDirectory();
            rdf->ToString(&rootDirectory);
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
            if (FALSE) Logger::D(TAG, "start scanning volume %s: %s", volume.string(), str.string());
            mOwner->Scan(directories, volume);
            if (FALSE) Logger::D(TAG, "done scanning volume %s", volume.string());
        }
    }


    Int32 arg1;
    msg->GetArg1(&arg1);
    mOwner->StopSelf(arg1);
    return NOERROR;
}

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos