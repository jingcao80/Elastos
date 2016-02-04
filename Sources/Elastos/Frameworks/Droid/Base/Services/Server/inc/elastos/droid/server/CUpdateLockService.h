#ifndef __ELASTOS_DROID_SERVER_CUPDATELOCKSERVICE_H__
#define __ELASTOS_DROID_SERVER_CUPDATELOCKSERVICE_H__

#include "_Elastos_Droid_Server_CUpdateLockService.h"
#include "elastos/droid/os/TokenWatcher.h"
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Content.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IIUpdateLock;
using Elastos::Droid::Os::TokenWatcher;
using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CUpdateLockService)
    , public Object
    , public IIUpdateLock
    , public IBinder
{
public:
    class LockWatcher
        : public TokenWatcher
    {
    public:
        LockWatcher(
            /* [in] */ IHandler* h,
            /* [in] */ const String& tag,
            /* [in] */ CUpdateLockService* owner);

        CARAPI Acquired();

        CARAPI Released();

    private:
        CUpdateLockService* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    //@Override
    CARAPI AcquireUpdateLock(
        /* [in] */ IBinder* token,
        /* [in] */ const String& tag);

    //@Override
    CARAPI ReleaseUpdateLock(
        /* [in] */ IBinder* token);

    //@Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    CARAPI SendLockChangedBroadcast(
        /* [in] */ Boolean state);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI_(String) MakeTag(
        /* [in] */ const String& tag);

protected:
    static const Boolean DEBUG;
    static const String TAG;

    // signatureOrSystem required to use update locks
    static const String PERMISSION;

    AutoPtr<IContext> mContext;
    AutoPtr<LockWatcher> mLocks;
};

}//Server
}//Droid
}//Elastos

#endif //__ELASTOS_DROID_SERVER_CUPDATELOCKSERVICE_H__
