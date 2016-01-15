
#ifndef __ELASTOS_DROID_SERVER_DREAMS_CDREAMMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_DREAMS_CDREAMMANAGERSERVICE_H__

#include "_Elastos_Droid_Server_Dreams_CDreamManagerService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "dreams/DreamController.h"
#include "elastos/droid/os/HandlerBase.h"

using Elastos::IO::IPrintWriter;
using Elastos::IO::IFileDescriptor;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::BroadcastReceiver;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Dreams {

CarClass(CDreamManagerService)
{
private:
    class SRBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        SRBroadcastReceiver(
            /* [in] */ CDreamManagerService* host)
            : mHost(host)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CDreamManagerService::SRBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CDreamManagerService* mHost;
    };

    class StartDreamRunnable
        : public ElRefBase
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        StartDreamRunnable(
            /* [in] */ CDreamManagerService* host,
            /* [in] */ IBinder* token,
            /* [in] */ IComponentName* name,
            /* [in] */ Boolean isTest,
            /* [in] */ Int32 userId)
            : mHost(host)
            , mToken(token)
            , mName(name)
            , mIsTest(isTest)
            , mUserId(userId)
        {}

        CARAPI Run();

    private:
        CDreamManagerService* mHost;
        IBinder* mToken;
        IComponentName* mName;
        Boolean mIsTest;
        Int32 mUserId;
    };

    class StopDreamRunnable
        : public ElRefBase
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        StopDreamRunnable(
            /* [in] */ CDreamManagerService* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        CDreamManagerService* mHost;
    };

    /**
     * Handler for asynchronous operations performed by the dream manager.
     * Ensures operations to {@link DreamController} are single-threaded.
     */
    class DreamHandler
        : public HandlerBase
    {
    public:
        DreamHandler(
            /* [in] */ ILooper* looper)
            : HandlerBase(looper, TRUE/*async*/)
        {}

        virtual CARAPI HandleMessage(
            /* [in] */ IMessage* msg)
        {
            return NOERROR;
        }
    };

public:
    CDreamManagerService()
        : mCurrentDreamUserId(0)
        , mCurrentDreamIsTest(FALSE)
    {}

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* mainHandler);

    CARAPI SystemReady();

    CARAPI GetDreamComponents(
        /* [out,callee] */ ArrayOf<IComponentName*>** result);

    CARAPI SetDreamComponents(
        /* [in] */ ArrayOf<IComponentName*>* componentNames);

    CARAPI GetDefaultDreamComponent(
        /* [out] */ IComponentName** component);

    CARAPI IsDreaming(
        /* [out] */ Boolean* result);

    CARAPI Dream();

    CARAPI TestDream(
        /* [in] */ IComponentName* dream);

    CARAPI Awaken();

    CARAPI FinishSelf(
        /* [in] */ IBinder* token);

    /**
     * Called by the power manager to start a dream.
     */
    CARAPI StartDream();

    /**
     * Called by the power manager to stop a dream.
     */
    CARAPI StopDream();

protected:
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    CARAPI_(AutoPtr<IComponentName>) ChooseDreamForUser(
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr< ArrayOf<IComponentName*> >) GetDreamComponentsForUser(
        /* [in] */ Int32 userId);

    CARAPI_(Boolean) ServiceExists(
        /* [in] */ IComponentName* name);

    CARAPI_(void) StartDreamLocked(
        /* [in] */ IComponentName* name,
        /* [in] */ Boolean isTest,
        /* [in] */ Int32 userId);

    CARAPI_(void) StopDreamLocked();

    CARAPI_(void) CleanupDreamLocked();

    CARAPI CheckPermission(
        /* [in] */ const String& permission);

    static CARAPI_(String) ComponentsToString(
        /* [in] */ ArrayOf<IComponentName*>* componentNames);

    static CARAPI_(AutoPtr< ArrayOf<IComponentName*> >) ComponentsFromString(
        /* [in] */ const String& names);

private:
    static const Boolean DEBUG;
    static const String TAG;

    Object mLock;

    AutoPtr<IContext> mContext;
    AutoPtr<IHandler> mHandler;
    AutoPtr<DreamController> mController;
    AutoPtr<IPowerManager> mPowerManager;

    AutoPtr<IBinder> mCurrentDreamToken;
    AutoPtr<IComponentName> mCurrentDreamName;
    Int32 mCurrentDreamUserId;
    Boolean mCurrentDreamIsTest;

    AutoPtr<DreamController::Listener> mControllerListener;
};

} // namespace Dreams
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_DREAMS_CDREAMMANAGERSERVICE_H__
