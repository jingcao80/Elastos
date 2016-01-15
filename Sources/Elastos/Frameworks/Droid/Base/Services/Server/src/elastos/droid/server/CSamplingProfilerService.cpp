
#include "CSamplingProfilerService.h"
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/Manifest.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

const String CSamplingProfilerService::SNAPSHOT_DIR("/data/snapshots");//SamplingProfilerIntegration::SNAPSHOT_DIR;
const String CSamplingProfilerService::TAG("SamplingProfilerService");
const Boolean CSamplingProfilerService::LOCAL_LOGV = FALSE;

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

    AutoPtr<IDropBoxManager> dropbox;
    context->GetSystemService(IContext::DROPBOX_SERVICE, (IInterface**)&dropbox);

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
    AutoPtr<IContentObserver> observer = new SamplingProfilerSettingsObserver(contentResolver);
    contentResolver->RegisterContentObserver(uri, FALSE, observer);
    return NOERROR;
}

ECode CSamplingProfilerService::Dump(
    /* [in] */ IFileDescriptor *fd,
    /* [in] */ IPrintWriter *pw,
    /* [in] */ ArrayOf<String> *args)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::DUMP, String(TAG)));

    pw->PrintStringln(String("SamplingProfilerService:") );
    pw->PrintStringln(String("Watching directory: ") + SNAPSHOT_DIR);

    return NOERROR;
}

CSamplingProfilerService::SamplingProfilerSettingsObserver::SamplingProfilerSettingsObserver(
    /* [in] */ IContentResolver *contentResolver)
    : ContentObserver(NULL)
    , mContentResolver(contentResolver)
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
    sysProp->Set(String("persist.sys.profiler_ms"), StringUtils::Int32ToString(samplingProfilerMs));

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
