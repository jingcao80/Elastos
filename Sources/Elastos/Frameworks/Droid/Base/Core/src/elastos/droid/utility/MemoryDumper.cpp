
#include "elastos/droid/utility/MemoryDumper.h"
#include "elastos/droid/os/Debug.h"
#include "elastos/droid/system/OsConstants.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Libcore.h>

using Elastos::Droid::Os::Debug;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IFileDescriptor;
using Elastos::Droid::System::OsConstants;

using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;
using Libcore::IO::IOs;
using Libcore::IO::IIoBridge;
using Libcore::IO::CIoBridge;

namespace Elastos {
namespace Droid {
namespace Utility {

ECode MemoryDumper::Dump()
{
    return Dump(String("memo_dump.txt"));
}

ECode MemoryDumper::Dump(
    /* [in] */ const String& filename)
{
    String path("/data/debug/");
    path += filename;

    AutoPtr<IFile> file;
    CFile::New(String("/data/debug"), (IFile**)&file);
    Boolean bval;
    file->Exists(&bval);
    if (!bval) {
       file->Mkdirs(&bval);
    }

    AutoPtr<IIoBridge> ioBridge;
    CIoBridge::AcquireSingleton((IIoBridge**)&ioBridge);
    AutoPtr<IFileDescriptor> ifd;
    ioBridge->Open(path, OsConstants::_O_RDWR | OsConstants::_O_CREAT | OsConstants::_O_TRUNC,
        (IFileDescriptor**)&ifd);
    return Debug::DumpHeap(ifd);
}

} // namespace Utility
} // namepsace Droid
} // namespace Elastos