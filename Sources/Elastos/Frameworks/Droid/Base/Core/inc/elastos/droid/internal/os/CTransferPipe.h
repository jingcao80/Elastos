
#ifndef __ELASTOS_DROID_INTERNAL_OS_CTRANSFERPIPE_H__
#define __ELASTOS_DROID_INTERNAL_OS_CTRANSFERPIPE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Os.h"
#include <Elastos.CoreLibrary.IO.h>
#include "_Elastos_Droid_Internal_Os_CTransferPipe.h"
#include <elastos/core/Object.h>

using Elastos::IO::IFileDescriptor;
using Elastos::Core::IRunnable;
using Elastos::Core::IThread;
using Elastos::Core::CThread;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

/**
 * Helper for transferring data through a pipe from a client app.
 */
CarClass(CTransferPipe)
    , public Object
    , public ITransferPipe
    , public IRunnable
{
public:
    interface ICaller : public IInterface
    {
        virtual CARAPI Go(
            /* [in] */ IInterface* iface,
            /* [in] */ IFileDescriptor* fd,
            /* [in] */ const String& prefix,
            /* [in] */ ArrayOf<String>* args);
    };

public:
    CTransferPipe();

    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI Run();

    CARAPI_(AutoPtr<IParcelFileDescriptor>) GetReadFd();

    CARAPI GetWriteFd(
        /* [out] */ IParcelFileDescriptor** writeFd);

    CARAPI SetBufferPrefix(
        /* [in] */ const String& prefix);

    static CARAPI Go(
        /* [in] */ ICaller* caller,
        /* [in] */ IInterface* iface,
        /* [in] */ IFileDescriptor* out,
        /* [in] */ const String& prefix,
        /* [in] */ ArrayOf<String>* args);

    static CARAPI Go(
        /* [in] */ ICaller* caller,
        /* [in] */ IInterface* iface,
        /* [in] */ IFileDescriptor* out,
        /* [in] */ const String& prefix,
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ Int64 timeout);

    static CARAPI GoDump(
        /* [in] */ IBinder* binder,
        /* [in] */ IFileDescriptor* out,
        /* [in] */ ArrayOf<String>* args);

    static CARAPI GoDump(
        /* [in] */ IBinder* binder,
        /* [in] */ IFileDescriptor* out,
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ Int64 timeout);

    CARAPI Go(
        /* [in] */ IFileDescriptor* out);

    CARAPI Go(
        /* [in] */ IFileDescriptor* out,
        /* [in] */ Int64 timeout);

    CARAPI CloseFd(
        /* [in] */ Int32 num);

    CARAPI Kill();

public:
    static const String TAG;
    static const Boolean DEBUG = FALSE;

    static const Int64 DEFAULT_TIMEOUT = 5000;  // 5 seconds

    AutoPtr<IThread> mThread;
    AutoPtr<ArrayOf<IParcelFileDescriptor*> > mFds;

    AutoPtr<IFileDescriptor> mOutFd;
    Int64 mEndTime;
    String mFailure;
    Boolean mComplete;

    String mBufferPrefix;
};

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_OS_CTRANSFERPIPE_H__
