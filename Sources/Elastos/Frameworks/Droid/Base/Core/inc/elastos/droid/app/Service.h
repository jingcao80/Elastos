#ifndef __ELASTOS_DROID_APP_SERVICE_H__
#define __ELASTOS_DROID_APP_SERVICE_H__

#include "Elastos.Droid.App.h"
#include <elastos/droid/content/ContextWrapper.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Content::ContextWrapper;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IComponentCallbacks;
using Elastos::Droid::Content::IComponentCallbacks2;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace App {

class ECO_PUBLIC Service
    : public ContextWrapper
    , public IService
    , public IComponentCallbacks
    , public IComponentCallbacks2
{
public:
    CAR_INTERFACE_DECL()

    Service();

    virtual ~Service();

    CARAPI constructor();

    CARAPI ToString(
        /* [out] */ String* info);

    CARAPI OnCreate();

    CARAPI OnStart(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 startId);

    CARAPI OnStartCommand(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 startId,
        /* [out] */ Int32* result);

    CARAPI OnDestroy();

    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    CARAPI OnLowMemory();

    CARAPI OnTrimMemory(
        /* [in] */ Int32 level);

    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);

    CARAPI OnUnbind(
        /* [in] */ IIntent* intent,
        /* [out] */ Boolean* succeeded);

    CARAPI OnRebind(
        /* [in] */ IIntent* intent);

    CARAPI GetClassName(
        /* [out] */ String* className);

    /** Return the application that owns this service. */
    CARAPI GetApplication(
        /* [out] */ IApplication** application);

    CARAPI OnTaskRemoved(
        /* [in] */ IIntent* rootIntent);

    CARAPI StopSelf();

    CARAPI StopSelf(
        /* [in] */ Int32 startId);

    CARAPI StopSelfResult(
        /* [in] */ Int32 startId,
        /* [out] */ Boolean* res);

    CARAPI SetForeground(
        /* [in] */ Boolean isForeground);

    CARAPI StartForeground(
        /* [in] */ Int32 id,
        /* [in] */ INotification* notification);

    CARAPI StopForeground(
        /* [in] */ Boolean removeNotification);

    CARAPI Attach(
        /* [in] */ IContext* ctx,
        /* [in] */ IActivityThread* thread,
        /* [in] */ const String& className,
        /* [in] */ IBinder* token,
        /* [in] */ IApplication* application,
        /* [in] */ IIActivityManager* activityManager);

protected:
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* writer,
        /* [in] */ ArrayOf<String>* args);

private:
    // set by the thread after the constructor and before onCreate(Bundle icicle) is called.
    AutoPtr<IActivityThread> mThread;
    String mClassName;
    AutoPtr<IBinder> mToken;
    AutoPtr<IApplication> mApplication;
    AutoPtr<IIActivityManager> mActivityManager;
    Boolean mStartCompatibility;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_SERVICE_H__
