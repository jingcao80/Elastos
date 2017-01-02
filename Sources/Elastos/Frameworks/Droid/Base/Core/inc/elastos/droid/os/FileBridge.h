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

#ifndef __ELASTOS_DROID_OS_FILE_BRIDGE_H__
#define __ELASTOS_DROID_OS_FILE_BRIDGE_H__

#include "Elastos.Droid.Os.h"
#include <elastos/core/Thread.h>
#include <elastos/io/OutputStream.h>

using Elastos::Core::Thread;
using Elastos::IO::OutputStream;
using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * Simple bridge that allows file access across process boundaries without
 * returning the underlying {@link FileDescriptor}. This is useful when the
 * server side needs to strongly assert that a client side is completely
 * hands-off.
 *
 * @hide
 */
class FileBridge
    : public Thread
{
public:
    class FileBridgeOutputStream
        : public OutputStream
        , public IFileBridgeOutputStream
    {
    public:
        CAR_INTERFACE_DECL()

        FileBridgeOutputStream(
            /* [in] */ IParcelFileDescriptor* clientPfd);

        FileBridgeOutputStream(
            /* [in] */ IFileDescriptor* client);

        CARAPI Close();

        CARAPI Fsync();

        CARAPI Write(
            /* [in] */ ArrayOf<Byte>* buffer,
            /* [in] */ Int32 byteOffset,
            /* [in] */ Int32 byteCount);

        CARAPI Write(
            /* [in] */ Int32 oneByte);

    private:
        CARAPI WriteCommandAndBlock(
            /* [in] */ Int32 cmd,
            /* [in] */ const String& cmdString);

    private:
        AutoPtr<IParcelFileDescriptor> mClientPfd;
        AutoPtr<IFileDescriptor> mClient;
        AutoPtr<ArrayOf<Byte> > mTemp;// = new byte[MSG_LENGTH];
    };

public:
    FileBridge();

    CARAPI_(Boolean) IsClosed();

    CARAPI_(void) ForceClose();

    CARAPI_(void) SetTargetFile(
        /* [in] */ IFileDescriptor* target);

    AutoPtr<IFileDescriptor> GetClientSocket();

    CARAPI Run();

private:
    static const String TAG;// == "FileBridge";

    // TODO: consider extending to support bidirectional IO

    static const Int32 MSG_LENGTH;// == 8;

    /** CMD_WRITE [len] [data] */
    static const Int32 CMD_WRITE;// == 1;
    /** CMD_FSYNC */
    static const Int32 CMD_FSYNC;// == 2;
    /** CMD_CLOSE */
    static const Int32 CMD_CLOSE;// == 3;

    AutoPtr<IFileDescriptor> mTarget;

    const AutoPtr<IFileDescriptor> mServer;// == new FileDescriptor();
    const AutoPtr<IFileDescriptor> mClient;// == new FileDescriptor();

    volatile Boolean mClosed;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_FILE_BRIDGE_H__
