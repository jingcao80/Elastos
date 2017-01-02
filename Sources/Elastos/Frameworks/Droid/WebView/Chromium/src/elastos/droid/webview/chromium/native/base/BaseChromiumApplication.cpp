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

#include "elastos/droid/webkit/webview/chromium/native/base/BaseChromiumApplication.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ApplicationStatus.h"

using Elastos::Droid::App::EIID_IActivityLifecycleCallbacks;
using Elastos::Droid::View::IWindow;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

//============================================================================================
//   BaseChromiumApplication::InnerActivityLifecycleCallbacks::InnerWindowCallbackWrapper
//============================================================================================

CAR_INTERFACE_IMPL(BaseChromiumApplication::InnerActivityLifecycleCallbacks, Object, IActivityLifecycleCallbacks);

BaseChromiumApplication::InnerActivityLifecycleCallbacks::InnerWindowCallbackWrapper::InnerWindowCallbackWrapper(
    /* [in] */ BaseChromiumApplication::InnerActivityLifecycleCallbacks* owner,
    /* [in] */ IWindowCallback* callback)
    : WindowCallbackWrapper(callback)
    , mOwner(owner)
{
}

//@Override
ECode BaseChromiumApplication::InnerActivityLifecycleCallbacks::InnerWindowCallbackWrapper::OnWindowFocusChanged(
    /* [in] */ IActivity* activity,
    /* [in] */ Boolean hasFocus)
{
    WindowCallbackWrapper::OnWindowFocusChanged(hasFocus);

    AutoPtr<IIterator> iter;
    mOwner->mOwner->mWindowFocusListeners.GetIterator((IIterator**)&iter);
    Boolean bNext;
    for (iter->HasNext(&bNext); bNext; iter->HasNext(&bNext)) {
        AutoPtr<IInterface> listener;
        iter->GetNext((IInterface**)&listener);
        ((WindowFocusChangedListener*)IObject::Probe(listener))->OnWindowFocusChanged(activity, hasFocus);
    }

    return NOERROR;
}

//===============================================================
//   BaseChromiumApplication::InnerActivityLifecycleCallbacks
//===============================================================

BaseChromiumApplication::InnerActivityLifecycleCallbacks::InnerActivityLifecycleCallbacks(
    /* [in] */ BaseChromiumApplication* owner)
    : mOwner(owner)
{
}

//@Override
ECode BaseChromiumApplication::InnerActivityLifecycleCallbacks::OnActivityCreated(
    /* [in] */ IActivity* activity,
    /* [in] */ IBundle* savedInstanceState)
{
    AutoPtr<IWindow> window;
    activity->GetWindow((IWindow**)&window);
    AutoPtr<IWindowCallback> callback;
    window->GetCallback((IWindowCallback**)&callback);

    AutoPtr<IWindowCallback> cb = new InnerWindowCallbackWrapper(this, callback);
    window->SetCallback(cb);

    return NOERROR;
}

//@Override
ECode BaseChromiumApplication::InnerActivityLifecycleCallbacks::OnActivityDestroyed(
    /* [in] */ IActivity* activity)
{
    //AutoPtr<IWindow> window;
    //activity->GetWindow((IWindow**)&window);
    //AutoPtr<IWindowCallback> callback;
    //window->GetCallback((IWindowCallback**)&callback);
    //TODO
    //assert(callback->Probe(EIID_WindowCallbackWrapper));

    return NOERROR;
}

ECode BaseChromiumApplication::InnerActivityLifecycleCallbacks::OnActivityPaused(
    /* [in] */ IActivity* activity)
{
    //AutoPtr<IWindow> window;
    //activity->GetWindow((IWindow**)&window);
    //AutoPtr<IWindowCallback> callback;
    //window->GetCallback((IWindowCallback**)&callback);
    //TODO
    //assert(callback->Probe(EIID_WindowCallbackWrapper));

    return NOERROR;
}

ECode BaseChromiumApplication::InnerActivityLifecycleCallbacks::OnActivityResumed(
    /* [in] */ IActivity* activity)
{
    //AutoPtr<IWindow> window;
    //activity->GetWindow((IWindow**)&window);
    //AutoPtr<IWindowCallback> callback;
    //window->GetCallback((IWindowCallback**)&callback);
    //TODO
    //assert(callback->Probe(EIID_WindowCallbackWrapper));

    return NOERROR;
}

ECode BaseChromiumApplication::InnerActivityLifecycleCallbacks::OnActivitySaveInstanceState(
    /* [in] */ IActivity* activity,
    /* [in] */ IBundle* outState)
{
    //AutoPtr<IWindow> window;
    //activity->GetWindow((IWindow**)&window);
    //AutoPtr<IWindowCallback> callback;
    //window->GetCallback((IWindowCallback**)&callback);
    //TODO
    //assert(callback->Probe(EIID_WindowCallbackWrapper));

    return NOERROR;
}

//@Override
ECode BaseChromiumApplication::InnerActivityLifecycleCallbacks::OnActivityStarted(
    /* [in] */ IActivity* activity)
{
    //AutoPtr<IWindow> window;
    //activity->GetWindow((IWindow**)&window);
    //AutoPtr<IWindowCallback> callback;
    //window->GetCallback((IWindowCallback**)&callback);
    //TODO
    //assert(callback->Probe(EIID_WindowCallbackWrapper));

    return NOERROR;
}

//@Override
ECode BaseChromiumApplication::InnerActivityLifecycleCallbacks::OnActivityStopped(
    /* [in] */ IActivity* activity)
{
    //AutoPtr<IWindow> window;
    //activity->GetWindow((IWindow**)&window);
    //AutoPtr<IWindowCallback> callback;
    //window->GetCallback((IWindowCallback**)&callback);
    //TODO
    //assert(callback->Probe(EIID_WindowCallbackWrapper));

    return NOERROR;
}

//===============================================================
//                   BaseChromiumApplication
//===============================================================

//@Override
ECode BaseChromiumApplication::OnCreate()
{
//    Application::OnCreate();
    ApplicationStatus::Initialize(this);

    AutoPtr<IActivityLifecycleCallbacks> callback = new InnerActivityLifecycleCallbacks(this);
    RegisterActivityLifecycleCallbacks(callback);

    return NOERROR;
}

/**
 * Registers a listener to receive window focus updates on activities in this application.
 * @param listener Listener to receive window focus events.
 */
void BaseChromiumApplication::RegisterWindowFocusChangedListener(
    /* [in] */ WindowFocusChangedListener* listener)
{
    mWindowFocusListeners.AddObserver((IObject*)listener);
}

/**
 * Unregisters a listener from receiving window focus updates on activities in this application.
 * @param listener Listener that doesn't want to receive window focus events.
 */
void BaseChromiumApplication::UnregisterWindowFocusChangedListener(
    /* [in] */ WindowFocusChangedListener* listener)
{
    mWindowFocusListeners.RemoveObserver((IObject*)listener);
}

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
