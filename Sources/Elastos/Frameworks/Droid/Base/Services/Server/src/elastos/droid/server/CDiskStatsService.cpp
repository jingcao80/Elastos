
#include "CDiskStatsService.h"
#include "elastos/droid/os/SystemClock.h"
#include "Elastos.Core.h"
#include "elastos/droid/Manifest.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Libcore::IO::CLibcore;
using Libcore::IO::ILibcore;
using Libcore::IO::IOs;
using Libcore::IO::CStructStatFs;
using Libcore::IO::IStructStatFs;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

const String CDiskStatsService::TAG("DiskStatsService");

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
        (IFile **)&tmp);

    AutoPtr<IFileOutputStream> fos;
    ECode ec = NOERROR;

    Int64 before = SystemClock::GetUptimeMillis();

    do {
        ec = CFileOutputStream::New(tmp, (IFileOutputStream**)&fos);
        if (FAILED(ec))
            break;
        ec = fos->WriteBytes(*junk);
    } while (0);

    if (fos != NULL) {
        fos->Close();
    }

    Int64 after = SystemClock::GetUptimeMillis();
    Boolean isExisted;
    tmp->Exists(&isExisted);
    if (isExisted) {
        Boolean isDeleted;
        tmp->Delete(&isDeleted);
    }

    if (FAILED(ec)) {
       pw->PrintString(String("Test-Error: "));
       pw->PrintStringln(StringUtils::Int32ToString(ec, 16));
    }
    else {
       pw->PrintString(String("Latency: "));
       pw->PrintInt64(after - before);
       pw->PrintStringln(String("ms [512B Data Write]"));
    }

    ReportFreeSpace(dataDirectory, String("Data"), pw);
    AutoPtr<IFile> cacheDir;
    env->GetDownloadCacheDirectory((IFile**)&cacheDir);
    ReportFreeSpace(cacheDir, String("Cache"), pw);

    AutoPtr<IFile> newfile;
    CFile::New(String("/system"), (IFile **)&newfile);
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
    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    AutoPtr<IOs> os;
    libcore->GetOs((IOs**)&os);
    AutoPtr<IStructStatFs> statfs;
    os->Statfs(p, (IStructStatFs**)&statfs);

    Int64 bsize, avail, total;
    statfs->GetBsize(&bsize);
    statfs->GetBavail(&avail);
    statfs->GetBlocks(&total);
    if (bsize <= 0 || total <= 0) {
        // throw new IllegalArgumentException(
        //                 "Invalid stat: bsize=" + bsize + " avail=" + avail + " total=" + total);
        Slogger::E(TAG, "Invalid stat: bsize=%d avail=%d total=%d", bsize, avail, total);
        return;
    }
    else {
        pw->PrintString(name);
        pw->PrintString(String("-Free: "));
        pw->PrintDouble(avail * bsize / 1024);
        pw->PrintString(String("K / ") );
        pw->PrintDouble(total * bsize / 1024);
        pw->PrintString(String("K total = ") );
        pw->PrintDouble(avail * 100 / total);
        pw->PrintStringln(String("% free") );
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
