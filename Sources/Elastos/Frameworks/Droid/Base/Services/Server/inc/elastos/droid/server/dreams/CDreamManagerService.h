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

#ifndef __ELASTOS_DROID_SERVER_DREAMS_CDREAMMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_DREAMS_CDREAMMANAGERSERVICE_H__

#include "_Elastos_Droid_Server_Dreams_CDreamManagerService.h"
#include "elastos/droid/server/dreams/DreamController.h"
#include "elastos/droid/server/SystemService.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/content/BroadcastReceiver.h"
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Service.h>
#include <Elastos.Droid.Os.h>

using Elastos::Droid::Os::Handler;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IPowerManagerInternal;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Service::Dreams::IDreamManagerInternal;
using Elastos::Droid::Service::Dreams::IIDreamManager;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Dreams {

CarClass(CDreamManagerService)
    , public SystemService
{

public:
    class BinderService
        : public Object
        , public IIDreamManager
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        BinderService();

        CARAPI constructor(
            /* [in] */ ISystemService* dreamManagerService);

        //@Override // Binder call
        CARAPI Dump(
            /* [in] */ IFileDescriptor* fd,
            /* [in] */ IPrintWriter* pw,
            /* [in] */ ArrayOf<String>* args);

        //@Override // Binder call
        CARAPI GetDreamComponents(
            /* [out, callee] */ ArrayOf<IComponentName*>** results);

        //@Override // Binder call
        CARAPI SetDreamComponents(
            /* [in] */ ArrayOf<IComponentName*>* componentNames);

        //@Override // Binder call
        CARAPI GetDefaultDreamComponent(
            /* [out] */ IComponentName** result);

        //@Override // Binder call
        CARAPI IsDreaming(
            /* [out] */ Boolean* result);

        //@Override // Binder call
        CARAPI Dream();

        //@Override // Binder call
        CARAPI TestDream(
            /* [in] */ IComponentName* dream);

        //@Override // Binder call
        CARAPI Awaken();

        //@Override // Binder call
        CARAPI FinishSelf(
            /* [in] */ IBinder* token,
            /* [in] */ Boolean immediate);

        //@Override // Binder call
        CARAPI StartDozing(
            /* [in] */ IBinder* token,
            /* [in] */ Int32 screenState,
            /* [in] */ Int32 screenBrightness);

        //@Override // Binder call
        CARAPI StopDozing(
            /* [in] */ IBinder* token);

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        CDreamManagerService* mHost;
    };

    class LocalService
        : public Object
        , public IDreamManagerInternal
    {
    public:
        CAR_INTERFACE_DECL()

        LocalService(
            /* [in] */ CDreamManagerService* host)
            : mHost(host)
        {}

        //@Override
        CARAPI StartDream(
            /* [in] */ Boolean doze);

        //@Override
        CARAPI StopDream(
            /* [in] */ Boolean immediate);

        //@Override
        CARAPI IsDreaming(
            /* [out] */ Boolean* result);

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        CDreamManagerService* mHost;
    };

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

        TO_STRING_IMPL("CDreamManagerService::SRBroadcastReceiver: ")
    private:
        CDreamManagerService* mHost;
    };

    class StartDreamRunnable
        : public Runnable
    {
    public:
        StartDreamRunnable(
            /* [in] */ CDreamManagerService* host,
            /* [in] */ IBinder* token,
            /* [in] */ IComponentName* name,
            /* [in] */ Boolean isTest,
            /* [in] */ Boolean canDoze,
            /* [in] */ Int32 userId)
            : mHost(host)
            , mToken(token)
            , mName(name)
            , mIsTest(isTest)
            , mCanDoze(canDoze)
            , mUserId(userId)
        {}

        CARAPI Run();

    private:
        CDreamManagerService* mHost;
        AutoPtr<IBinder> mToken;
        AutoPtr<IComponentName> mName;
        Boolean mIsTest;
        Boolean mCanDoze;
        Int32 mUserId;
    };

    class StopDreamRunnable
        : public Runnable
    {
    public:
        StopDreamRunnable(
            /* [in] */ CDreamManagerService* host,
            /* [in] */ Boolean immediate)
            : mHost(host)
            , mImmediate(immediate)
        {}

        CARAPI Run();

    private:
        CDreamManagerService* mHost;
        Boolean mImmediate;
    };

    class SystemPropertiesChangedRunnable
        : public Runnable
    {
    public:
        SystemPropertiesChangedRunnable(
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
        : public Handler
    {
    public:
        TO_STRING_IMPL("CDreamManagerService::DreamHandler")

        DreamHandler(
            /* [in] */ ILooper* looper)
            : Handler(looper, TRUE/*async*/)
        {}

        virtual CARAPI HandleMessage(
            /* [in] */ IMessage* msg)
        {
            return NOERROR;
        }
    };

    class DreamControllerListener : public DreamController::Listener
    {
    public:
        DreamControllerListener(
            /* [in] */ CDreamManagerService* host);

        // @Override
        CARAPI OnDreamStopped(
            /* [in] */ IBinder* token);

    private:
        CDreamManagerService* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CDreamManagerService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    //@Override
    CARAPI OnStart();

    //@Override
    CARAPI OnBootPhase(
        /* [in] */ Int32 phase);

protected:
    CARAPI DumpInternal(
        /* [in] */ IPrintWriter* pw);

private:
    Boolean IsDreamingInternal();

    CARAPI RequestDreamInternal();

    CARAPI RequestAwakenInternal();

    CARAPI FinishSelfInternal(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean immediate);

    CARAPI TestDreamInternal(
        /* [in] */ IComponentName* dream,
        /* [in] */ Int32 userId);

    CARAPI StartDreamInternal(
        /* [in] */ Boolean doze);

    CARAPI StopDreamInternal(
        /* [in] */ Boolean immediate);

    CARAPI StartDozingInternal(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 screenState,
        /* [in] */ Int32 screenBrightness);

    CARAPI StopDozingInternal(
        /* [in] */ IBinder* token);

    CARAPI_(AutoPtr<IComponentName>) ChooseDreamForUser(
        /* [in] */ Boolean doze,
        /* [in] */ Int32 userId);

    Boolean ValidateDream(
        /* [in] */ IComponentName* component);

    AutoPtr< ArrayOf<IComponentName*> > GetDreamComponentsForUser(
        /* [in] */ Int32 userId);

    CARAPI SetDreamComponentsForUser(
        /* [in] */ Int32 userId,
        /* [in] */ ArrayOf<IComponentName*>* componentNames);

    AutoPtr<IComponentName> GetDefaultDreamComponentForUser(
        /* [in] */ Int32 userId);

    AutoPtr<IComponentName> GetDozeComponent();

    AutoPtr<IComponentName> GetDozeComponent(
        /* [in] */ Int32 userId);

    AutoPtr<IServiceInfo> GetServiceInfo(
        /* [in] */ IComponentName* name);

    CARAPI StartDreamLocked(
        /* [in] */ IComponentName* name,
        /* [in] */ Boolean isTest,
        /* [in] */ Boolean canDoze,
        /* [in] */ Int32 userId);

    CARAPI StopDreamLocked(
        /* [in] */ Boolean immediate);

    CARAPI CleanupDreamLocked();

    CARAPI CheckPermission(
        /* [in] */ const String& permission);

    static CARAPI_(String) ComponentsToString(
        /* [in] */ ArrayOf<IComponentName*>* componentNames);

    static CARAPI_(AutoPtr< ArrayOf<IComponentName*> >) ComponentsFromString(
        /* [in] */ const String& names);

private:
    friend class BinderService;
    friend class LocalService;

    static const Boolean DEBUG;
    static const String TAG;

    Object mLock;

    AutoPtr<IContext> mContext;
    AutoPtr<IHandler> mHandler;
    AutoPtr<DreamController> mController;
    AutoPtr<IPowerManager> mPowerManager;
    AutoPtr<IPowerManagerInternal> mPowerManagerInternal;
    AutoPtr<IPowerManagerWakeLock> mDozeWakeLock;

    AutoPtr<IBinder> mCurrentDreamToken;
    AutoPtr<IComponentName> mCurrentDreamName;
    Int32 mCurrentDreamUserId;
    Boolean mCurrentDreamIsTest;
    Boolean mCurrentDreamCanDoze;
    Boolean mCurrentDreamIsDozing;
    Boolean mCurrentDreamIsWaking;
    Int32 mCurrentDreamDozeScreenState;// = Display.STATE_UNKNOWN;
    Int32 mCurrentDreamDozeScreenBrightness;// = PowerManager.BRIGHTNESS_DEFAULT;

    AutoPtr<DreamController::Listener> mControllerListener;
    AutoPtr<IRunnable> mSystemPropertiesChanged;
};

} // namespace Dreams
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_DREAMS_CDREAMMANAGERSERVICE_H__
