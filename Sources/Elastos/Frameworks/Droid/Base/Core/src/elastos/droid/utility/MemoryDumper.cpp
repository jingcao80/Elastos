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

#include "elastos/droid/utility/MemoryDumper.h"
#include "elastos/droid/os/Debug.h"
#include "elastos/droid/system/OsConstants.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::Debug;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IFileDescriptor;
using Elastos::Droid::System::OsConstants;
using Elastos::Utility::Logging::Logger;

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
    ECode ec = ioBridge->Open(path, OsConstants::_O_RDWR | OsConstants::_O_CREAT | OsConstants::_O_TRUNC,
        (IFileDescriptor**)&ifd);
    if (FAILED(ec) || ifd == NULL) {
        Logger::E("MemoryDumper", "Failed to open %s, ec=%08x", path.string(), ec);
        return ec;
    }
    return Debug::DumpHeap(ifd);
}

} // namespace Utility
} // namepsace Droid
} // namespace Elastos