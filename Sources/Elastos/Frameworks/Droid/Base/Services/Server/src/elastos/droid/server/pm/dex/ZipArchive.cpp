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

#include "elastos/droid/server/pm/dex/ZipArchive.h"
#include <elastos/utility/logging/Slogger.h>
#include <fcntl.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {
namespace Dex {

uint32_t ZipEntry::GetCrc32()
{
    return mZipEntry->crc32;
}

ZipEntry::~ZipEntry()
{
    delete mZipEntry;
}



static void SetCloseOnExec(int fd)
{
    // This dance is more portable than Linux's O_CLOEXEC open(2) flag.
    int flags = fcntl(fd, F_GETFD);
    if (flags == -1) {
        Slogger::W("ZipArchive", "fcntl(%d, F_GETFD) failed", fd);
        return;
    }
    int rc = fcntl(fd, F_SETFD, flags | FD_CLOEXEC);
    if (rc == -1) {
        Slogger::W("ZipArchive", "fcntl(%d, F_SETFD %d) failed", fd, flags);
        return;
    }
}

AutoPtr<ZipArchive> ZipArchive::OpenFromFd(
    /* [in] */ Int32 fd,
    /* [in] */ const char* filename,
    /* [in] */ String* error_msg)
{
    assert(filename != NULL);
    assert(fd > 0);

    ZipArchiveHandle handle;
    const int32_t error = OpenArchiveFd(fd, filename, &handle);
    if (error) {
        *error_msg = ErrorCodeString(error);
        CloseArchive(handle);
        return NULL;
    }

    SetCloseOnExec(GetFileDescriptor(handle));
    return new ZipArchive(handle);
}

AutoPtr<ZipEntry> ZipArchive::Find(
    /* [in] */ const char* name,
    /* [in] */ String* error_msg)
{
    assert(name != NULL);

    // Resist the urge to delete the space. <: is a bigraph sequence.
    ::ZipEntry* zip_entry = new ::ZipEntry;
    const int32_t error = FindEntry(mHandle, ZipString(name), zip_entry);
    if (error) {
        *error_msg = ErrorCodeString(error);
        delete zip_entry;
        return NULL;
    }

    return new ZipEntry(mHandle, zip_entry);
}

} // namespace Dex
} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
