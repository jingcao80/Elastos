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

#include "CIoUtils.h"
#include "IoUtils.h"

namespace Libcore {
namespace IO {

CAR_INTERFACE_IMPL(CIoUtils, Singleton, IIoUtils)

CAR_SINGLETON_IMPL(CIoUtils)

ECode CIoUtils::Close(
    /* [in] */ IFileDescriptor* fd)
{
    return IoUtils::Close(fd);
}

ECode CIoUtils::CloseQuietly(
    /* [in] */ ICloseable* closeable)
{
    return IoUtils::CloseQuietly(closeable);
}

ECode CIoUtils::CloseQuietly(
    /* [in] */ ISocket* socket)
{
    return IoUtils::CloseQuietly(socket);
}

ECode CIoUtils::CloseQuietly(
    /* [in] */ IFileDescriptor* fd)
{
    return IoUtils::CloseQuietly(fd);
}

ECode CIoUtils::SetBlocking(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Boolean blocking)
{
    return IoUtils::SetBlocking(fd, blocking);
}

ECode CIoUtils::ReadFileAsByteArray(
    /* [in] */ const String& path,
    /* [out, callee] */ ArrayOf<Byte>** bytes)
{
    return IoUtils::ReadFileAsByteArray(path, bytes);
}

ECode CIoUtils::ReadFileAsString(
    /* [in] */ const String& path,
    /* [out] */ String* result)
{
    return IoUtils::ReadFileAsString(path, result);
}

// TODO: this should specify paths as Strings rather than as Files
ECode CIoUtils::DeleteContents(
    /* [in] */ IFile* dir)
{
    return IoUtils::DeleteContents(dir);
}

ECode CIoUtils::CreateTemporaryDirectory(
    /* [in] */ const String& prefix,
    /* [out] */ IFile** file)
{
    return IoUtils::CreateTemporaryDirectory(prefix, file);
}

ECode CIoUtils::CanOpenReadOnly(
    /* [in] */ const String& path,
    /* [out] */ Boolean* result)
{
    return IoUtils::CanOpenReadOnly(path, result);
}

} // namespace IO
} // namespace Libcore
