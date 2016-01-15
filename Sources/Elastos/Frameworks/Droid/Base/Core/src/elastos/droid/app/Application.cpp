
#include "elastos/droid/app/Application.h"
// #include "elastos/droid/app/CContextImpl.h"
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Content::EIID_IComponentCallbacks;
using Elastos::Droid::Content::EIID_IComponentCallbacks2;

namespace Elastos {
namespace Droid {
namespace App {

const String Application::TAG("Application");

CAR_INTERFACE_IMPL_3(Application, ContextWrapper, IApplication, IComponentCallbacks, IComponentCallbacks2)

Application::Application()
{}

Application::~Application()
{}

ECode Application::constructor()
{
    return ContextWrapper::constructor(NULL);
}

ECode Application::OnCreate()
{
    return NOERROR;
}

ECode Application::OnTerminate()
{
    return NOERROR;
}

ECode Application::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    AutoPtr< ArrayOf<IComponentCallbacks*> > callbacks = CollectComponentCallbacks();
    if (callbacks != NULL) {
        for (Int32 i = 0; i < callbacks->GetLength(); i++) {
            (*callbacks)[i]->OnConfigurationChanged(newConfig);
        }
    }
    return NOERROR;
}

ECode Application::OnLowMemory()
{
    AutoPtr< ArrayOf<IComponentCallbacks*> > callbacks = CollectComponentCallbacks();
    if (callbacks != NULL) {
        for (Int32 i = 0; i < callbacks->GetLength(); i++) {
            (*callbacks)[i]->OnLowMemory();
        }
    }
    return NOERROR;
}

ECode Application::OnTrimMemory(
    /* [in] */ Int32 level)
{
    AutoPtr< ArrayOf<IComponentCallbacks*> > callbacks = CollectComponentCallbacks();
    if (callbacks != NULL) {
        for (Int32 i = 0; i < callbacks->GetLength(); i++) {
            IComponentCallbacks2* cb2 = IComponentCallbacks2::Probe((*callbacks)[i]);
            if (cb2 != NULL) {
                cb2->OnTrimMemory(level);
            }
        }
    }
    return NOERROR;
}

ECode Application::RegisterComponentCallbacks(
    /* [in] */ IComponentCallbacks* componentCallback)
{
    if (componentCallback) {
        AutoLock lock(mComponentCallbacksLock);
        mComponentCallbacks.PushBack(componentCallback);
    }
    return NOERROR;
}

ECode Application::UnregisterComponentCallbacks(
    /* [in] */ IComponentCallbacks* componentCallback)
{
    if (componentCallback) {
        AutoLock lock(mComponentCallbacksLock);
        mComponentCallbacks.Remove(componentCallback);
    }
    return NOERROR;
}

ECode Application::RegisterActivityLifecycleCallbacks(
    /* [in] */ IActivityLifecycleCallbacks* callback)
{
    if (callback) {
        AutoLock lock(mActivityLifecycleCallbacksLock);
        mActivityLifecycleCallbacks.PushBack(callback);
    }
    return NOERROR;
}

ECode Application::UnregisterActivityLifecycleCallbacks(
    /* [in] */ IActivityLifecycleCallbacks* callback)
{
    if (callback) {
        AutoLock lock(mActivityLifecycleCallbacksLock);
        mActivityLifecycleCallbacks.Remove(callback);
    }
    return NOERROR;
}

ECode Application::RegisterOnProvideAssistDataListener(
    /* [in] */ IOnProvideAssistDataListener* callback)
{
    synchronized(this) {
        if (mAssistCallbacks == NULL) {
            mAssistCallbacks = new List<AutoPtr<IOnProvideAssistDataListener> >();
        }
        mAssistCallbacks->PushBack(callback);
    }
    return NOERROR;
}

ECode Application::UnregisterOnProvideAssistDataListener(
    /* [in] */ IOnProvideAssistDataListener* callback)
{
    synchronized(this) {
        if (mAssistCallbacks != NULL) {
            mAssistCallbacks->Remove(callback);
        }
    }
    return NOERROR;
}

ECode Application::Attach(
    /* [in] */ IContext* ctx)
{
    FAIL_RETURN(AttachBaseContext(ctx))
    assert(0 && "TODO");
    // mLoadedPkg = CContextImpl::GetImpl(ctx)->mPackageInfo;
    return NOERROR;
}

ECode Application::DispatchActivityCreated(
    /* [in] */ IActivity* activity,
    /* [in] */ IBundle* savedInstanceState)
{
    AutoPtr< ArrayOf<IActivityLifecycleCallbacks*> > callbacks = CollectActivityLifecycleCallbacks();
    if (callbacks != NULL) {
        for (Int32 i = 0; i < callbacks->GetLength(); i++) {
            (*callbacks)[i]->OnActivityCreated(activity, savedInstanceState);
        }
    }
    return NOERROR;
}

ECode Application::DispatchActivityStarted(
    /* [in] */ IActivity* activity)
{
    AutoPtr< ArrayOf<IActivityLifecycleCallbacks*> > callbacks = CollectActivityLifecycleCallbacks();
    if (callbacks != NULL) {
        for (Int32 i = 0; i < callbacks->GetLength(); i++) {
            (*callbacks)[i]->OnActivityStarted(activity);
        }
    }
    return NOERROR;
}

ECode Application::DispatchActivityResumed(
    /* [in] */ IActivity* activity)
{
    AutoPtr< ArrayOf<IActivityLifecycleCallbacks*> > callbacks = CollectActivityLifecycleCallbacks();
    if (callbacks != NULL) {
        for (Int32 i = 0; i < callbacks->GetLength(); i++) {
            (*callbacks)[i]->OnActivityResumed(activity);
        }
    }
    return NOERROR;
}

ECode Application::DispatchActivityPaused(
    /* [in] */ IActivity* activity)
{
    AutoPtr< ArrayOf<IActivityLifecycleCallbacks*> > callbacks = CollectActivityLifecycleCallbacks();
    if (callbacks != NULL) {
        for (Int32 i = 0; i < callbacks->GetLength(); i++) {
            (*callbacks)[i]->OnActivityPaused(activity);
        }
    }
    return NOERROR;
}

ECode Application::DispatchActivityStopped(
    /* [in] */ IActivity* activity)
{
    AutoPtr< ArrayOf<IActivityLifecycleCallbacks*> > callbacks = CollectActivityLifecycleCallbacks();
    if (callbacks != NULL) {
        for (Int32 i = 0; i < callbacks->GetLength(); i++) {
            (*callbacks)[i]->OnActivityStopped(activity);
        }
    }
    return NOERROR;
}

ECode Application::DispatchActivitySaveInstanceState(
    /* [in] */ IActivity* activity,
    /* [in] */ IBundle* outState)
{
    AutoPtr< ArrayOf<IActivityLifecycleCallbacks*> > callbacks = CollectActivityLifecycleCallbacks();
    if (callbacks != NULL) {
        for (Int32 i = 0; i < callbacks->GetLength(); i++) {
            (*callbacks)[i]->OnActivitySaveInstanceState(activity, outState);
        }
    }
    return NOERROR;
}

ECode Application::DispatchActivityDestroyed(
    /* [in] */ IActivity* activity)
{
    AutoPtr< ArrayOf<IActivityLifecycleCallbacks*> > callbacks = CollectActivityLifecycleCallbacks();
    if (callbacks != NULL) {
        for (Int32 i = 0; i < callbacks->GetLength(); i++) {
            (*callbacks)[i]->OnActivityDestroyed(activity);
        }
    }
    return NOERROR;
}

ECode Application::DispatchOnProvideAssistData(
    /* [in] */ IActivity* activity,
    /* [in] */ IBundle* data)
{
    AutoPtr<ArrayOf<IOnProvideAssistDataListener*> > callbacks;
    synchronized(this) {
        if (mAssistCallbacks == NULL) {
            return NOERROR;
        }

        Int32 size = mAssistCallbacks->GetSize();
        if (size > 0) {
            callbacks = ArrayOf<IOnProvideAssistDataListener*>::Alloc(size);
            List< AutoPtr<IOnProvideAssistDataListener> >::Iterator it;
            Int32 i;
            for (it = mAssistCallbacks->Begin(), i = 0; it != mAssistCallbacks->End(); ++it, ++i) {
                callbacks->Set(i, *it);
            }
        }
    }

    if (callbacks != NULL) {
        for (Int32 i = 0; i< callbacks->GetLength(); i++) {
            (*callbacks)[i]->OnProvideAssistData(activity, data);
        }
    }
    return NOERROR;
}

AutoPtr< ArrayOf<IComponentCallbacks*> > Application::CollectComponentCallbacks()
{
    AutoPtr< ArrayOf<IComponentCallbacks*> > callbacks;
    {
        AutoLock lock(mComponentCallbacksLock);

        Int32 size = mComponentCallbacks.GetSize();
        if (size > 0) {
            callbacks = ArrayOf<IComponentCallbacks*>::Alloc(size);
            List< AutoPtr<IComponentCallbacks> >::Iterator it;
            Int32 i;
            for (it = mComponentCallbacks.Begin(), i = 0; it != mComponentCallbacks.End(); ++it, ++i) {
                callbacks->Set(i, *it);
            }
        }
    }
    return callbacks;
}

AutoPtr< ArrayOf<IActivityLifecycleCallbacks*> > Application::CollectActivityLifecycleCallbacks()
{
    AutoPtr< ArrayOf<IActivityLifecycleCallbacks*> > callbacks;
    {
        AutoLock lock(mActivityLifecycleCallbacksLock);

        Int32 size = mActivityLifecycleCallbacks.GetSize();
        if (size > 0) {
            callbacks = ArrayOf<IActivityLifecycleCallbacks*>::Alloc(size);
            List< AutoPtr<IActivityLifecycleCallbacks> >::Iterator it;
            Int32 i;
            for (it = mActivityLifecycleCallbacks.Begin(), i = 0; it != mActivityLifecycleCallbacks.End(); ++it, ++i) {
                callbacks->Set(i, *it);
            }
        }

    }
    return callbacks;
}


} // namespace App
} // namespace Droid
} // namespace Elastos
