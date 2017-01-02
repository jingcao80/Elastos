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

#include "elastos/droid/server/CDiskStatsService.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/Manifest.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.IO.h"

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::IStatFs;
using Elastos::Droid::Os::CStatFs;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::ICloseable;


namespace Elastos {
namespace Droid {
namespace Server {

const String CDiskStatsService::TAG("DiskStatsService");

CAR_INTERFACE_IMPL(CDiskStatsService, Object, IBinder)

CAR_OBJECT_IMPL(CDiskStatsService)

ECode CDiskStatsService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    return NOERROR;
}

ECode CDiskStatsService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String> *args)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::DUMP, TAG));

    // Run a quick-and-dirty performance test: write 512 bytes
    Int32 len = 512;
    AutoPtr<ArrayOf<Byte> > junk = ArrayOf<Byte>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        (*junk)[i] = (Byte)i; // Write nonzero bytes
    }

    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> dataDirectory;
    env->GetDataDirectory((IFile**)&dataDirectory);
    AutoPtr<IFile> tmp;
    CFile::New(dataDirectory, String("system/perftest.tmp"),
        (IFile**)&tmp);

    AutoPtr<IOutputStream> fos;
    ECode ec = NOERROR;

    Int64 before = SystemClock::GetUptimeMillis();

    do {
        ec = CFileOutputStream::New(tmp, (IOutputStream**)&fos);
        if (FAILED(ec))
            break;
        ec = fos->Write(junk);
    } while (0);

    if (fos != NULL) {
        ICloseable::Probe(fos)->Close();
    }

    Int64 after = SystemClock::GetUptimeMillis();
    Boolean isExisted;
    tmp->Exists(&isExisted);
    if (isExisted) {
        Boolean isDeleted;
        tmp->Delete(&isDeleted);
    }

    if (FAILED(ec)) {
       pw->Print(String("Test-Error: "));
       pw->Println(StringUtils::ToString(ec, 16));
    }
    else {
       pw->Print(String("Latency: "));
       pw->Print(after - before);
       pw->Println(String("ms [512B Data Write]"));
    }

    ReportFreeSpace(dataDirectory, String("Data"), pw);
    AutoPtr<IFile> cacheDir;
    env->GetDownloadCacheDirectory((IFile**)&cacheDir);
    ReportFreeSpace(cacheDir, String("Cache"), pw);

    AutoPtr<IFile> newfile;
    CFile::New(String("/system"), (IFile**)&newfile);
    ReportFreeSpace(newfile, String("System"), pw);

    return NOERROR;
    // TODO: Read /proc/yaffs and report interesting values;
    // add configurable (through args) performance test parameters.
}

void CDiskStatsService::ReportFreeSpace(
    /* [in] */ IFile* path,
    /* [in] */ const String& name,
    /* [in] */ IPrintWriter* pw)
{
    //try {
    String p;
    path->GetPath(&p);

    AutoPtr<IStatFs> statfs;
    CStatFs::New(p, (IStatFs**)&statfs);

    Int64 bsize, avail, total;
    statfs->GetBlockSize(&bsize);
    statfs->GetAvailableBlocks(&avail);
    statfs->GetBlockCount(&total);
    if (bsize <= 0 || total <= 0) {
        // throw new IllegalArgumentException(
        //                 "Invalid stat: bsize=" + bsize + " avail=" + avail + " total=" + total);
        Slogger::E(TAG, "Invalid stat: bsize=%d avail=%d total=%d", bsize, avail, total);
        return;
    }
    else {
        pw->Print(name);
        pw->Print(String("-Free: "));
        pw->Print(avail * bsize / 1024);
        pw->Print(String("K / ") );
        pw->Print(total * bsize / 1024);
        pw->Print(String("K total = ") );
        pw->Print(avail * 100 / total);
        pw->Println(String("% free") );
    }
    // } catch (IllegalArgumentException e) {
    //     pw.print(name);
    //     pw.print("-Error: ");
    //     pw.println(e.toString());
    //     return;
    // }
}

ECode CDiskStatsService::ToString(
        /* [out] */ String* str)
{
    return E_NOT_IMPLEMENTED;
}

}//Server
}//Droid
}//Elastos
