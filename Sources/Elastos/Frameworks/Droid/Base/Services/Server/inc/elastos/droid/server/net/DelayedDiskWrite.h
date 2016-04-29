
#ifndef __ELASTOS_DROID_SERVER_NET_DELAYEDDISKWRITE_H__
#define __ELASTOS_DROID_SERVER_NET_DELAYEDDISKWRITE_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <Elastos.Droid.Os.h>
#include <elastos/droid/os/Runnable.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::Runnable;

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
