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

#include "elastos/droid/server/CSamplingProfilerService.h"
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/Manifest.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Database.h>
#include <Elastos.CoreLibrary.IO.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

const String CSamplingProfilerService::SNAPSHOT_DIR("/data/snapshots");//SamplingProfilerIntegration::SNAPSHOT_DIR;
const String CSamplingProfilerService::TAG("SamplingProfilerService");
const Boolean CSamplingProfilerService::LOCAL_LOGV = FALSE;

CAR_INTERFACE_IMPL(CSamplingProfilerService, Object, IBinder)

CAR_OBJECT_IMPL(CSamplingProfilerService)

ECode CSamplingProfilerService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    RegisterSettingObserver(context);
    StartWorking(context);
    return NOERROR;
}

ECode CSamplingProfilerService::StartWorking(
    /* [in] */ IContext* context)
{
    if (LOCAL_LOGV) {
        Slogger::V(TAG, "starting SamplingProfilerService!");
    }

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::DROPBOX_SERVICE, (IInterface**)&obj);
    AutoPtr<IDropBoxManager> dropbox = IDropBoxManager::Probe(obj);
    // before FileObserver is ready, there could have already been some snapshots
    // in the directory, we don't want to miss them
    AutoPtr<IFile> file;
    CFile::New(SNAPSHOT_DIR, (IFile**)&file);
    AutoPtr<ArrayOf<IFile*> > snapshotFiles;
    file->ListFiles((ArrayOf<IFile*>**)&snapshotFiles);

    for (Int32 i = 0; snapshotFiles != NULL && i < snapshotFiles->GetLength(); i++) {
        HandleSnapshotFile((*snapshotFiles)[i], dropbox);
    }

    // detect new snapshot and put it in dropbox
    // delete it afterwards no matter what happened before
    // Note: needs listening at event ATTRIB rather than CLOSE_WRITE, because we set the
    // readability of snapshot files after writing them!
    mSnapshotObserver = new MyFileObserver(SNAPSHOT_DIR, IFileObserver::ATTRIB, this, dropbox);
    mSnapshotObserver->StartWatching();

    if (LOCAL_LOGV) {
        Slogger::V(TAG, "SamplingProfilerService activated" );
    }
    return NOERROR;
}

ECode CSamplingProfilerService::HandleSnapshotFile(
    /* [in] */ IFile* file,
    /* [in] */ IDropBoxManager* dropbox)
{
    ECode ec = dropbox->AddFile(TAG, file, 0);
    if (LOCAL_LOGV) {
        String path;
        file->GetPath(&path);
        Slogger::V(TAG, "%s added to dropbox", path.string());
    }
    if (FAILED(ec)) {
        String path;
        file->GetPath(&path);
        Slogger::E(TAG, "Can't add %s to dropbox", path.string());
    }

    Boolean res;
    file->Delete(&res);

    return NOERROR;
}

ECode CSamplingProfilerService::RegisterSettingObserver(
    /* [in] */ IContext *context)
{
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    assert(contentResolver != NULL);

    AutoPtr<IUri> uri;
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    settingsGlobal->GetUriFor(ISettingsGlobal::SAMPLING_PROFILER_MS, (IUri**)&uri);
    AutoPtr<SamplingProfilerSettingsObserver> observer = new SamplingProfilerSettingsObserver(contentResolver);
    observer->constructor(NULL);
    contentResolver->RegisterContentObserver(uri, FALSE, observer.Get());
    return NOERROR;
}

ECode CSamplingProfilerService::Dump(
    /* [in] */ IFileDescriptor *fd,
    /* [in] */ IPrintWriter *pw,
    /* [in] */ ArrayOf<String> *args)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::DUMP, String(TAG)));

    pw->Println(String("SamplingProfilerService:") );
    pw->Println(String("Watching directory: ") + SNAPSHOT_DIR);

    return NOERROR;
}

CSamplingProfilerService::SamplingProfilerSettingsObserver::SamplingProfilerSettingsObserver(
    /* [in] */ IContentResolver *contentResolver)
    : mContentResolver(contentResolver)
{
    OnChange(FALSE);
}

ECode CSamplingProfilerService::SamplingProfilerSettingsObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    Int32 samplingProfilerMs;
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    settingsGlobal->GetInt32(
        mContentResolver, ISettingsGlobal::SAMPLING_PROFILER_MS,
        0, &samplingProfilerMs);
    // setting this secure property will start or stop sampling profiler,
    // as well as adjust the the time between taking snapshots.
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    sysProp->Set(String("persist.sys.profiler_ms"), StringUtils::ToString(samplingProfilerMs));

    return NOERROR;
}

CSamplingProfilerService::MyFileObserver::MyFileObserver(
    /* [in] */ const String& path,
    /* [in] */ Int32 mask,
    /* [in] */ CSamplingProfilerService* owner,
    /* [in] */ IDropBoxManager* dropbox)
    : FileObserver(path, mask)
    , mOwner(owner)
    , mDropBox(dropbox)
{}

ECode CSamplingProfilerService::MyFileObserver::OnEvent(
    /* [in] */ Int32 event,
    /* [in] */ const String& path)
{
    AutoPtr<IFile> file;
    CFile::New(SNAPSHOT_DIR, path, (IFile**)&file);
    mOwner->HandleSnapshotFile(file, mDropBox);
    return NOERROR;
}

ECode CSamplingProfilerService::ToString(
        /* [out] */ String* str)
{
    return E_NOT_IMPLEMENTED;
}

}// Server
}// Droid
}// Elastos
