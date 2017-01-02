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

#ifndef __ELASTOS_DROID_SERVER_NET_DELAYEDDISKWRITE_H__
#define __ELASTOS_DROID_SERVER_NET_DELAYEDDISKWRITE_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <Elastos.Droid.Os.h>
#include <elastos/core/Runnable.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Core::Runnable;

using Elastos::IO::IDataOutputStream;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

class DelayedDiskWrite
    : public Object
{
    friend class InnerSub_Runnable;

//public:
//    class Writer
//    {
//    public:
//        virtual CARAPI OnWriteCalled(
//            /* [in] */ IDataOutputStream* out) = 0;
//    };

private:
    class InnerSub_Runnable
        : public Runnable
    {
    public:
        InnerSub_Runnable(
            /* [in] */ DelayedDiskWrite* host,
            /* [in] */ const String& filePath,
            /* [in] */ IDelayedDiskWriteWriter* w);

        // @Override
        CARAPI Run();

    private:
        DelayedDiskWrite* mHost;
        const String& mFilePath;
        AutoPtr<IDelayedDiskWriteWriter> mWriter;
    };

public:
    DelayedDiskWrite();

    CARAPI Write(
        /* [in] */ const String& filePath,
        /* [in] */ IDelayedDiskWriteWriter* w);

private:
    CARAPI DoWrite(
        /* [in] */ const String& filePath,
        /* [in] */ IDelayedDiskWriteWriter* w);

    CARAPI Loge(
        /* [in] */ const String& s);

private:
    AutoPtr<IHandlerThread> mDiskWriteHandlerThread;

    AutoPtr<IHandler> mDiskWriteHandler;

    /* Tracks multiple writes on the same thread */
    Int32 mWriteSequence;

    const String TAG;
};

} // namespace Net
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_NET_DELAYEDDISKWRITE_H__
