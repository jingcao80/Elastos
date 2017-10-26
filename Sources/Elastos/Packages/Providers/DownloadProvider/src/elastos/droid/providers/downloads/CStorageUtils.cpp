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

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Text.h"
#include "elastos/droid/providers/downloads/CStorageUtils.h"
#include "elastos/droid/providers/downloads/Constants.h"
#include "elastos/droid/system/Os.h"
#include "elastos/droid/os/Environment.h"
#include "elastos/droid/os/Process.h"
#include <elastos/core/Math.h>
#include "elastos/utility/logging/Slogger.h"

using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::EIID_IIPackageDataObserver;
using Elastos::Droid::Net::ITrafficStats;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Core::EIID_IComparator;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Concurrent::ITimeUnit;
using Elastos::Utility::Concurrent::ITimeUnitHelper;
using Elastos::Utility::Concurrent::CTimeUnitHelper;
using Elastos::Utility::Concurrent::CCountDownLatch;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

//===============================================================
// CStorageUtils::ConcreteFile::
//===============================================================

CStorageUtils::ConcreteFile::ConcreteFile(
    /* [in] */ IFile* file)
{
    mFile = file;
    String path;
    file->GetAbsolutePath(&path);
    Elastos::Droid::System::Os::Lstat(path, (IStructStat**)&mStat);
}

ECode CStorageUtils::ConcreteFile::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    Int32 result = 1;
    Int64 dev = 0, ino = 0;
    mStat->GetDev(&dev);
    mStat->GetIno(&ino);
    result = 31 * result + (Int32) (dev ^ (dev >> 32));
    result = 31 * result + (Int32) (ino ^ (ino >> 32));
    *hashCode = result;
    return NOERROR;
}

ECode CStorageUtils::ConcreteFile::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* equals)
{
    VALIDATE_NOT_NULL(equals);
    if (o != NULL) {
        AutoPtr<ConcreteFile> f = (ConcreteFile*)(IObject*)o;
        Int64 fDev = 0, dev = 0, fino = 0, ino = 0;
        f->mStat->GetDev(&fDev);
        mStat->GetDev(&dev);
        f->mStat->GetIno(&fino);
        mStat->GetIno(&ino);
        *equals = (fDev == dev) && (fino == ino);
        return NOERROR;
    }
    *equals = FALSE;
    return NOERROR;
}

//===============================================================
// CStorageUtils::ObserverLatch::
//===============================================================
CAR_INTERFACE_IMPL(CStorageUtils::ObserverLatch, Object, IIPackageDataObserver)

CStorageUtils::ObserverLatch::ObserverLatch()
{
    CCountDownLatch::New(1, (ICountDownLatch**)&mLatch);
}

ECode CStorageUtils::ObserverLatch::OnRemoveCompleted(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean succeeded)
{
    mLatch->CountDown();
    return NOERROR;
}

//===============================================================
// CStorageUtils::MyComparator::
//===============================================================
CAR_INTERFACE_IMPL(CStorageUtils::MyComparator, Object, IComparator)

ECode CStorageUtils::MyComparator::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ConcreteFile> lcf = (ConcreteFile*)(IObject*)lhs;
    AutoPtr<ConcreteFile> rcf = (ConcreteFile*)(IObject*)rhs;
    Int64 lm = 0, rm = 0;
    lcf->mFile->GetLastModified(&lm);
    rcf->mFile->GetLastModified(&rm);
    *result = (Int32) (lm - rm);
    return NOERROR;
}

//===============================================================
// CStorageUtils::
//===============================================================
Int64 CStorageUtils::MIN_DELETE_AGE = IDateUtils::DAY_IN_MILLIS;

Int64 CStorageUtils::RESERVED_BYTES = 32 * ITrafficStats::MB_IN_BYTES;

Boolean CStorageUtils::sForceFullEviction = FALSE;

CAR_SINGLETON_IMPL(CStorageUtils)

CAR_INTERFACE_IMPL(CStorageUtils, Singleton, IStorageUtils)

ECode CStorageUtils::EnsureAvailableSpace(
    /* [in] */ IContext* context,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 bytes)
{
    Int64 availBytes = GetAvailableBytes(fd);
    if (availBytes >= bytes) {
        // Underlying partition has enough space; go ahead
        return NOERROR;
    }

    // Not enough space, let's try freeing some up. Start by tracking down
    // the backing partition.
    AutoPtr<IStructStat> ss;
    Elastos::Droid::System::Os::Fstat(fd, (IStructStat**)&ss);
    Int64 dev = 0;
    ss->GetDev(&dev);

    Int64 dataDev = GetDeviceId(Environment::GetDataDirectory());
    Int64 cacheDev = GetDeviceId(Environment::GetDownloadCacheDirectory());
    Int64 externalDev = GetDeviceId(Environment::GetExternalStorageDirectory());

    if (dev == dataDev || (dev == externalDev && Environment::IsExternalStorageEmulated())) {
        // File lives on internal storage; ask PackageManager to try freeing
        // up space from cache directories.
        AutoPtr<IPackageManager> pm;
        context->GetPackageManager((IPackageManager**)&pm);
        AutoPtr<ObserverLatch> observer = new ObserverLatch();
        pm->FreeStorageAndNotify(sForceFullEviction ? Elastos::Core::Math::INT64_MAX_VALUE : bytes, observer);

        // try {
            AutoPtr<ITimeUnitHelper> thlp;
            CTimeUnitHelper::AcquireSingleton((ITimeUnitHelper**)&thlp);
            AutoPtr<ITimeUnit> tu;
            thlp->GetSECONDS((ITimeUnit**)&tu);
            Boolean bWait = FALSE;
            if (!(observer->mLatch->Await(30, tu, &bWait), bWait)) {
                // throw new IOException("Timeout while freeing disk space");
                return E_IO_EXCEPTION;
            }
        // } catch (InterruptedException e) {
        //     Thread.currentThread().interrupt();
        // }

    }
    else if (dev == cacheDev) {
        // Try removing old files on cache partition
        FreeCacheStorage(bytes);
    }

    // Did we free enough space?
    availBytes = GetAvailableBytes(fd);
    if (availBytes < bytes) {
        // throw new StopRequestException(STATUS_INSUFFICIENT_SPACE_ERROR,
        //         "Not enough free space; " + bytes + " requested, " + availBytes + " available");
        return NOERROR;
    }
    return NOERROR;
}

void CStorageUtils::FreeCacheStorage(
    /* [in] */ Int64 bytes)
{
    // Only consider finished downloads
    AutoPtr<IList> files;
    ListFilesRecursive(
            Environment::GetDownloadCacheDirectory(), Constants::DIRECTORY_CACHE_RUNNING,
            Process::MyUid(), (IList**)&files);

    Int32 s = 0;
    files->GetSize(&s);
    Slogger::D(Constants::TAG, "Found %d downloads on cache", s);

    AutoPtr<IComparator> cmp = new MyComparator();
    AutoPtr<ICollections> cls;
    CCollections::AcquireSingleton((ICollections**)&cls);
    cls->Sort(files, cmp);

    Int64 now = 0;
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    sys->GetCurrentTimeMillis(&now);
    Int32 size = 0;
    files->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> p;
        files->Get(i, (IInterface**)&p);
        AutoPtr<ConcreteFile> file = (ConcreteFile*)(IObject*)p.Get();
        if (bytes <= 0) break;

        Int64 lm = 0;
        file->mFile->GetLastModified(&lm);
        if (now - lm < MIN_DELETE_AGE) {
            Slogger::D(Constants::TAG, "Skipping recently modified %p", file->mFile.Get());
        }
        else {
            Int64 len = 0;
            file->mFile->GetLength(&len);
            Slogger::D(Constants::TAG, "Deleting %p to reclaim %d", file->mFile.Get(), len);
            bytes -= len;
            file->mFile->Delete();
        }
    }
}

Int64 CStorageUtils::GetAvailableBytes(
    /* [in] */ IFileDescriptor* fd)
{
    // try {
    AutoPtr<IStructStatVfs> stat;
    Elastos::Droid::System::Os::Fstatvfs(fd, (IStructStatVfs**)&stat);
    Int64 avail = 0, bsize = 0;
    stat->GetBavail(&avail);
    stat->GetBsize(&bsize);
    return (avail * bsize) - RESERVED_BYTES;
    // } catch (ErrnoException e) {
    //     throw e.rethrowAsIOException();
    // }
}

Int64 CStorageUtils::GetDeviceId(
    /* [in] */ IFile* file)
{
    String path;
    file->GetAbsolutePath(&path);
    AutoPtr<IStructStat> stat;
    Elastos::Droid::System::Os::Stat(path, (IStructStat**)&stat);
    Int64 dev = 0;
    stat->GetDev(&dev);
    return dev;
}

ECode CStorageUtils::ListFilesRecursive(
    /* [in] */ IFile* startDir,
    /* [in] */ const String& exclude,
    /* [in] */ Int32 uid,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IArrayList> files;
    CArrayList::New((IArrayList**)&files);
    AutoPtr<ILinkedList> dirs;
    CLinkedList::New((ILinkedList**)&dirs);
    dirs->Add(startDir);
    Boolean bEmp = FALSE;
    while (!(dirs->IsEmpty(&bEmp), bEmp)) {
        AutoPtr<IInterface> _dir;
        dirs->RemoveFirst((IInterface**)&_dir);
        AutoPtr<IFile> dir = IFile::Probe(_dir);
        String name;
        dir->GetName(&name);
        if (name.Equals(exclude)) continue;

        AutoPtr<ArrayOf<IFile*> > children;
        dir->ListFiles((ArrayOf<IFile*>**)&children);
        if (children == NULL) continue;

        for (Int32 i = 0; i < children->GetLength();i++) {
            AutoPtr<IFile> child = (*children)[i];
            Boolean bIsDir = FALSE, bIsFile = FALSE;
            if ((child->IsDirectory(&bIsDir), bIsDir)) {
                dirs->Add(child);
            }
            else if ((child->IsFile(&bIsFile), bIsFile)) {
                // try {
                    AutoPtr<ConcreteFile> file = new ConcreteFile(child);
                    Int32 suid = 0;
                    file->mStat->GetUid(&suid);
                    if (uid == -1 || suid == uid) {
                        files->Add((IObject*)file.Get());
                    }
                // } catch (ErrnoException ignored) {
                // }
            }
        }
    }
    *result = IList::Probe(files);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos
