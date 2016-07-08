
#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/providers/downloads/CDownloadIdleService.h"
#include "elastos/droid/providers/downloads/CStorageUtils.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/Environment.h"
#include <elastos/core/StringBuilder.h>
#include "elastos/utility/logging/Slogger.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::IDownloadsImpl;
using Elastos::Droid::Provider::CDownloadsImpl;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::IThread;
using Elastos::Core::CThread;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::IHashSet;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::ICollection;
using Elastos::IO::CFile;
using Elastos::IO::ICloseable;

using Libcore::IO::IIoUtils;
using Libcore::IO::CIoUtils;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

//===============================================================
// CDownloadIdleService::StaleQuery::
//===============================================================

static AutoPtr<ArrayOf<String> > InitPro()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(4);
    (*arr)[0] = IBaseColumns::ID;
    (*arr)[1] = IDownloadsImpl::COLUMN_STATUS;
    (*arr)[2] = IDownloadsImpl::COLUMN_LAST_MODIFICATION;
    (*arr)[3] = IDownloadsImpl::COLUMN_IS_VISIBLE_IN_DOWNLOADS_UI;
    return arr;
}

AutoPtr<ArrayOf<String> > CDownloadIdleService::StaleQuery::PROJECTION = InitPro();

Int32 CDownloadIdleService::StaleQuery::_ID = 0;

//===============================================================
// CDownloadIdleService::OrphanQuery::
//===============================================================

static AutoPtr<ArrayOf<String> > InitArr()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(2);
    (*arr)[0] = IBaseColumns::ID;
    (*arr)[1] = IDownloadsImpl::_DATA;
    return arr;
}

AutoPtr<ArrayOf<String> > CDownloadIdleService::OrphanQuery::PROJECTION = InitArr();

Int32 CDownloadIdleService::OrphanQuery::_ID = 0;

Int32 CDownloadIdleService::OrphanQuery::_DATA = 1;

//===============================================================
// CDownloadIdleService::IdleRunnable::
//===============================================================
CAR_INTERFACE_IMPL(CDownloadIdleService::IdleRunnable, Object, IRunnable)

CDownloadIdleService::IdleRunnable::IdleRunnable(
    /* [in] */ IJobParameters* params,
    /* [in] */ CDownloadIdleService* host)
{
    mParams = params;
    mHost = host;
}

ECode CDownloadIdleService::IdleRunnable::Run()
{
    mHost->CleanStale();
    mHost->CleanOrphans();
    mHost->JobFinished(mParams, FALSE);
    return NOERROR;
}

//===============================================================
// CDownloadIdleService::
//===============================================================
CAR_OBJECT_IMPL(CDownloadIdleService)

CAR_INTERFACE_IMPL(CDownloadIdleService, JobService, IDownloadIdleService)

ECode CDownloadIdleService::OnStartJob(
    /* [in] */ IJobParameters* params,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IdleRunnable> run = new IdleRunnable(params, this);
    AutoPtr<IThread> t;
    CThread::New(run, (IThread**)&t);
    t->Start();
    *result = TRUE;
    return NOERROR;
}

ECode CDownloadIdleService::OnStopJob(
    /* [in] */ IJobParameters* params,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // We're okay being killed at any point, so we don't worry about
    // checkpointing before tearing down.
    *result = FALSE;
    return NOERROR;
}

ECode CDownloadIdleService::CleanStale()
{
    AutoPtr<IContentResolver> resolver;
    GetContentResolver((IContentResolver**)&resolver);

    Int64 currentTimeMillis = 0;
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    sys->GetCurrentTimeMillis(&currentTimeMillis);
    Int64 modifiedBefore = currentTimeMillis - IDateUtils::WEEK_IN_MILLIS;
    AutoPtr<IDownloadsImpl> impl;
    CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&impl);
    AutoPtr<IUri> uri;
    impl->GetALL_DOWNLOADS_CONTENT_URI((IUri**)&uri);
    StringBuilder sb(IDownloadsImpl::COLUMN_STATUS);
    sb += " >= '200' AND "; sb += IDownloadsImpl::COLUMN_LAST_MODIFICATION;
    sb += " <= '"; sb += modifiedBefore;
    sb += "' AND "; sb += IDownloadsImpl::COLUMN_IS_VISIBLE_IN_DOWNLOADS_UI;
    sb += " == '0'";
    AutoPtr<ICursor> cursor;
    resolver->Query(uri, StaleQuery::PROJECTION, sb.ToString(),
            NULL, String(NULL), (ICursor**)&cursor);

    Int32 count = 0;
    Boolean bHasNext = FALSE;
    while ((cursor->MoveToNext(&bHasNext), bHasNext)) {
        Int64 id = 0;
        cursor->GetInt64(StaleQuery::_ID, &id);
        AutoPtr<IContentUris> cu;
        CContentUris::AcquireSingleton((IContentUris**)&cu);
        AutoPtr<IUri> uri;
        cu->WithAppendedId(uri, id, (IUri**)&uri);
        Int32 res = 0;
        resolver->Delete(uri, String(NULL), NULL, &res);
        count++;
    }
    AutoPtr<IIoUtils> iou;
    CIoUtils::AcquireSingleton((IIoUtils**)&iou);
    iou->CloseQuietly(ICloseable::Probe(cursor));

    Slogger::D("CDownloadIdleService", "Removed %d stale downloads", count);
    return NOERROR;
}

ECode CDownloadIdleService::CleanOrphans()
{
    AutoPtr<IContentResolver> resolver;
    GetContentResolver((IContentResolver**)&resolver);

    // Collect known files from database
    AutoPtr<IHashSet> fromDb;
    CHashSet::New((IHashSet**)&fromDb);
    AutoPtr<IDownloadsImpl> impl;
    CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&impl);
    AutoPtr<IUri> uri;
    impl->GetALL_DOWNLOADS_CONTENT_URI((IUri**)&uri);
    AutoPtr<ICursor> cursor;
    resolver->Query(uri,
            OrphanQuery::PROJECTION, String(NULL), NULL, String(NULL), (ICursor**)&cursor);
    // try {
        Boolean bMN = FALSE;
        while ((cursor->MoveToNext(&bMN), bMN)) {
            String path;
            cursor->GetString(OrphanQuery::_DATA, &path);
            if (TextUtils::IsEmpty(path)) continue;

            AutoPtr<IFile> file;
            CFile::New(path, (IFile**)&file);
            // try {
            AutoPtr<CStorageUtils::ConcreteFile> cf = new CStorageUtils::ConcreteFile(file);
            fromDb->Add((IObject*)cf);
            // } catch (ErrnoException e) {
            //     // File probably no longer exists
            //     String state = Environment->GetExternalStorageState(file);
            //     if (Environment.MEDIA_UNKNOWN.Equals(state)
            //             || Environment.MEDIA_MOUNTED.Equals(state)) {
            //         // File appears to live on internal storage, or a
            //         // currently mounted device, so remove it from database.
            //         // This logic preserves files on external storage while
            //         // media is removed.
            //         Int64 id = cursor->GetLong(OrphanQuery::_ID);
            //         Slogger::D(TAG, "Missing %p, deleting %d", file, id);
            //         resolver->Delete(ContentUris::WithAppendedId(
            //                 Downloads.Impl.ALL_DOWNLOADS_CONTENT_URI, id), NULL, NULL);
            //     }
            // }
        }
    // } finally {
        AutoPtr<IIoUtils> iou;
        CIoUtils::AcquireSingleton((IIoUtils**)&iou);
        iou->CloseQuietly(ICloseable::Probe(cursor));
    // }

    // Collect known files from disk
    Int32 uid = Process::MyUid();
    AutoPtr<IArrayList> fromDisk;
    CArrayList::New((IArrayList**)&fromDisk);
    AutoPtr<IFile> cacheDir;
    GetCacheDir((IFile**)&cacheDir);
    AutoPtr<IStorageUtils> su;
    CStorageUtils::AcquireSingleton((IStorageUtils**)&su);
    AutoPtr<IList> l;
    su->ListFilesRecursive(cacheDir, String(NULL), uid, (IList**)&l);
    fromDisk->AddAll(ICollection::Probe(l));
    AutoPtr<IFile> fileDir;
    GetFilesDir((IFile**)&fileDir);
    AutoPtr<IList> l2;
    su->ListFilesRecursive(fileDir, String(NULL), uid, (IList**)&l2);
    fromDisk->AddAll(ICollection::Probe(l2));
    AutoPtr<IList> l3;
    su->ListFilesRecursive(Environment::GetDownloadCacheDirectory(), String(NULL), uid, (IList**)&l3);
    fromDisk->AddAll(ICollection::Probe(l3));

    Int32 dbSize = 0, diskSize = 0;
    fromDb->GetSize(&dbSize);
    fromDisk->GetSize(&diskSize);
    Slogger::D("CDownloadIdleService", "Found %d files in database", dbSize);
    Slogger::D("CDownloadIdleService", "Found %d files on disk", diskSize);

    // Delete files no longer referenced by database
    Int32 size = 0;
    fromDisk->GetSize(&size);
    for (Int32 i = 0;i < size; i++) {
        AutoPtr<IInterface> p;
        fromDisk->Get(i, (IInterface**)&p);
        AutoPtr<CStorageUtils::ConcreteFile> file = (CStorageUtils::ConcreteFile*)(IObject*)p.Get();
        Boolean bCont = FALSE;
        if (!(fromDb->Contains(p, &bCont), bCont)) {
            Slogger::D("CDownloadIdleService", "Missing db entry, deleting %p", file->mFile.Get());
            file->mFile->Delete();
        }
    }
    return NOERROR;
}

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos
