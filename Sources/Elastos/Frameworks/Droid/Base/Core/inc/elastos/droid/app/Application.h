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

#ifndef __ELASTOS_DROID_APP_APPLICATION_H__
#define __ELASTOS_DROID_APP_APPLICATION_H__

#include <elastos/droid/ext/frameworkext.h>
#include "Elastos.Droid.App.h"
#include <elastos/droid/content/ContextWrapper.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::ContextWrapper;
using Elastos::Droid::Content::IComponentCallbacks;
using Elastos::Droid::Content::IComponentCallbacks2;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace App {

class ECO_PUBLIC Application
    : public ContextWrapper
    , public IApplication
    , public IComponentCallbacks
    , public IComponentCallbacks2
{
public:
    CAR_INTERFACE_DECL()

    Application();

    virtual ~Application();

    CARAPI constructor();

    /**
     * Called when the application is starting, before any activity, service,
     * or receiver objects (excluding content providers) have been created.
     * Implementations should be as quick as possible (for example using
     * lazy initialization of state) since the time spent in this function
     * directly impacts the performance of starting the first activity,
     * service, or receiver in a process.
     * If you override this method, be sure to call super.onCreate().
     */
    CARAPI OnCreate();

    /**
     * This method is for use in emulated process environments.  It will
     * never be called on a production Android device, where processes are
     * removed by simply killing them; no user code (including this callback)
     * is executed when doing so.
     */
    CARAPI OnTerminate();

    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    CARAPI OnLowMemory();

    CARAPI OnTrimMemory(
        /* [in] */ Int32 level);

    /**
     * Add a new {@sa ComponentCallbacks} to the base application of the
     * Context, which will be called at the same times as the ComponentCallbacks
     * methods of activities and other components are called.  Note that you
     * <em>must</em> be sure to use {@sa #unregisterComponentCallbacks} when
     * appropriate in the future; this will not be removed for you.
     *
     * @param callback The interface to call.  This can be either a
     * {@sa ComponentCallbacks} or {@sa ComponentCallbacks2} interface.
     */
    CARAPI RegisterComponentCallbacks(
        /* [in] */ IComponentCallbacks* componentCallback);

    /**
     * Remove a {@sa ComponentCallbacks} objec that was previously registered
     * with {@sa #registerComponentCallbacks(ComponentCallbacks)}.
     */
    CARAPI UnregisterComponentCallbacks(
        /* [in] */ IComponentCallbacks* componentCallback);

    CARAPI RegisterActivityLifecycleCallbacks(
        /* [in] */ IActivityLifecycleCallbacks* callback);

    CARAPI UnregisterActivityLifecycleCallbacks(
        /* [in] */ IActivityLifecycleCallbacks* callback);

    CARAPI RegisterOnProvideAssistDataListener(
        /* [in] */ IOnProvideAssistDataListener* callback);

    CARAPI UnregisterOnProvideAssistDataListener(
        /* [in] */ IOnProvideAssistDataListener* callback);

    /**
     * @hide
     */
    /* package */
    CARAPI Attach(
        /* [in] */ IContext* ctx);

    /* package */
    CARAPI DispatchActivityCreated(
        /* [in] */ IActivity* activity,
        /* [in] */ IBundle* b);

    /* package */
    CARAPI DispatchActivityStarted(
        /* [in] */ IActivity* activity);

    /* package */
    CARAPI DispatchActivityResumed(
        /* [in] */ IActivity* activity);

    /* package */
    CARAPI DispatchActivityPaused(
        /* [in] */ IActivity* activity);

    /* package */
    CARAPI DispatchActivityStopped(
        /* [in] */ IActivity* activity);

    /* package */
    CARAPI DispatchActivitySaveInstanceState(
        /* [in] */ IActivity* activity,
        /* [in] */ IBundle* b);

    /* package */
    CARAPI DispatchActivityDestroyed(
        /* [in] */ IActivity* activity);

    /* package */
    CARAPI DispatchOnProvideAssistData(
        /* [in] */ IActivity* activity,
        /* [in] */ IBundle* data);

private:
    CARAPI_(AutoPtr< ArrayOf<IComponentCallbacks*> >) CollectComponentCallbacks();

    CARAPI_(AutoPtr< ArrayOf<IActivityLifecycleCallbacks*> >) CollectActivityLifecycleCallbacks();

public:
    AutoPtr<ILoadedPkg> mLoadedPkg;

private:
    List< AutoPtr<IComponentCallbacks> > mComponentCallbacks;
    Object mComponentCallbacksLock;
    List< AutoPtr<IActivityLifecycleCallbacks> > mActivityLifecycleCallbacks;
    Object mActivityLifecycleCallbacksLock;

    AutoPtr<List<AutoPtr<IOnProvideAssistDataListener> > > mAssistCallbacks;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_APPLICATION_H__
