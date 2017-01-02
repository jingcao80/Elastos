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

#ifndef __ELASTOS_DROID_OS_CPARCELFILEDESCRIPTORHELPER_H__
#define __ELASTOS_DROID_OS_CPARCELFILEDESCRIPTORHELPER_H__

#include "_Elastos_Droid_Os_CParcelFileDescriptorHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::Net::ISocket;
using Elastos::Net::IDatagramSocket;

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CParcelFileDescriptorHelper)
    , public Singleton
    , public IParcelFileDescriptorHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Open(
        /* [in] */ IFile* file,
        /* [in] */ Int32 mode,
        /* [out] */ IParcelFileDescriptor** descriptor);

    CARAPI Open(
        /* [in] */ IFile* file,
        /* [in] */ Int32 mode,
        /* [in] */ IHandler* handler,
        /* [in] */ IParcelFileDescriptorOnCloseListener* listener,
        /* [out] */ IParcelFileDescriptor** descriptor);

    CARAPI Dup(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ IParcelFileDescriptor** descriptor);

    CARAPI AdoptFd(
        /* [in] */ Int32 fd,
        /* [out] */ IParcelFileDescriptor** descriptor);

    CARAPI FromSocket(
        /* [in] */ ISocket* socket,
        /* [out] */ IParcelFileDescriptor** descriptor);

    CARAPI FromDatagramSocket(
        /* [in] */ IDatagramSocket* socket,
        /* [out] */ IParcelFileDescriptor** descriptor);

    CARAPI CreatePipe(
        /* [out, callee] */ ArrayOf<IParcelFileDescriptor*>** pfds);

    CARAPI CreateReliablePipe(
        /* [out, callee] */ ArrayOf<IParcelFileDescriptor*>** pfds);

    CARAPI CreateSocketPair(
        /* [out, callee] */ ArrayOf<IParcelFileDescriptor*>** pfds);

    CARAPI CreateReliableSocketPair(
        /* [out, callee] */ ArrayOf<IParcelFileDescriptor*>** pfds);

    CARAPI FromData(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ const String& name,
        /* [out] */ IParcelFileDescriptor** pfds);

    CARAPI ParseMode(
        /* [in] */ const String& name,
        /* [out] */ Int32* result);
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CPARCELFILEDESCRIPTORHELPER_H__
