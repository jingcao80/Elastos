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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/CParcelFileDescriptorHelper.h"
#include "elastos/droid/os/ParcelFileDescriptor.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CParcelFileDescriptorHelper, Singleton, IParcelFileDescriptorHelper)

CAR_SINGLETON_IMPL(CParcelFileDescriptorHelper)

ECode CParcelFileDescriptorHelper::Open(
    /* [in] */ IFile* file,
    /* [in] */ Int32 mode,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    return ParcelFileDescriptor::Open(file, mode, descriptor);
}

ECode CParcelFileDescriptorHelper::Open(
    /* [in] */ IFile* file,
    /* [in] */ Int32 mode,
    /* [in] */ IHandler* handler,
    /* [in] */ IParcelFileDescriptorOnCloseListener* listener,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    return ParcelFileDescriptor::Open(file, mode, handler, listener, descriptor);
}

ECode CParcelFileDescriptorHelper::Dup(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    return ParcelFileDescriptor::Dup(fd, descriptor);
}

ECode CParcelFileDescriptorHelper::AdoptFd(
    /* [in] */ Int32 fd,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    return ParcelFileDescriptor::AdoptFd(fd, descriptor);
}

ECode CParcelFileDescriptorHelper::FromSocket(
    /* [in] */ ISocket* socket,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    return ParcelFileDescriptor::FromSocket(socket, descriptor);
}

ECode CParcelFileDescriptorHelper::FromDatagramSocket(
    /* [in] */ IDatagramSocket* socket,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    return ParcelFileDescriptor::FromDatagramSocket(socket, descriptor);
}

ECode CParcelFileDescriptorHelper::CreatePipe(
    /* [out, callee] */ ArrayOf<IParcelFileDescriptor*>** pfds)
{
    return ParcelFileDescriptor::CreatePipe(pfds);
}

ECode CParcelFileDescriptorHelper::CreateReliablePipe(
    /* [out, callee] */ ArrayOf<IParcelFileDescriptor*>** pfds)
{
    return ParcelFileDescriptor::CreateReliablePipe(pfds);
}

ECode CParcelFileDescriptorHelper::CreateSocketPair(
    /* [out, callee] */ ArrayOf<IParcelFileDescriptor*>** pfds)
{
    return ParcelFileDescriptor::CreateSocketPair(pfds);
}

ECode CParcelFileDescriptorHelper::CreateReliableSocketPair(
    /* [out, callee] */ ArrayOf<IParcelFileDescriptor*>** pfds)
{
    return ParcelFileDescriptor::CreateReliableSocketPair(pfds);
}

ECode CParcelFileDescriptorHelper::FromData(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ const String& name,
    /* [out] */ IParcelFileDescriptor** pfds)
{
    return ParcelFileDescriptor::FromData(data, name, pfds);
}

ECode CParcelFileDescriptorHelper::ParseMode(
    /* [in] */ const String& name,
    /* [out] */ Int32* result)
{
    return ParcelFileDescriptor::ParseMode(name, result);
}

} // namespace Os
} // namespace Droid
} // namespace Elastos

