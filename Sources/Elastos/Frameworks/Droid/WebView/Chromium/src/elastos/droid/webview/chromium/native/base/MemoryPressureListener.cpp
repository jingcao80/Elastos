
#include "Elastos.Droid.App.h"
#include "elastos/droid/webkit/webview/chromium/native/base/MemoryPressureListener.h"
#include "elastos/droid/webkit/webview/chromium/native/base/api/MemoryPressureListener_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/base/MemoryPressureLevelList.h"

using Elastos::Droid::App::IApplication;
using Elastos::Droid::Content::IComponentCallbacks;
using Elastos::Droid::Content::EIID_IComponentCallbacks;
using Elastos::Droid::Content::EIID_IComponentCallbacks2;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

//===============================================================
//       MemoryPressureListener::InnerComponentCallbacks2
//===============================================================
CAR_INTERFACE_IMPL(MemoryPressureListener::InnerComponentCallbacks2, Object, IComponentCallbacks2);

MemoryPressureListener::InnerComponentCallbacks2::InnerComponentCallbacks2()
{
}

//@Override
ECode MemoryPressureListener::InnerComponentCallbacks2::OnTrimMemory(
    /* [in] */ Int32 level)
{
    MaybeNotifyMemoryPresure(level);
    return NOERROR;
}

//@Override
ECode MemoryPressureListener::InnerComponentCallbacks2::OnLowMemory()
{
    NativeOnMemoryPressure(MemoryPressureLevelList::MEMORY_PRESSURE_CRITICAL);
    return NOERROR;
}

//@Override
ECode MemoryPressureListener::InnerComponentCallbacks2::OnConfigurationChanged(
    /* [in] */ IConfiguration* configuration)
{
    return NOERROR;
}

//===============================================================
//                    MemoryPressureListener
//===============================================================

const String MemoryPressureListener::ACTION_LOW_MEMORY("org.chromium.base.ACTION_LOW_MEMORY");
const String MemoryPressureListener::ACTION_TRIM_MEMORY("org.chromium.base.ACTION_TRIM_MEMORY");
const String MemoryPressureListener::ACTION_TRIM_MEMORY_RUNNING_CRITICAL("org.chromium.base.ACTION_TRIM_MEMORY_RUNNING_CRITICAL");
const String MemoryPressureListener::ACTION_TRIM_MEMORY_MODERATE("org.chromium.base.ACTION_TRIM_MEMORY_MODERATE");

//@CalledByNative
void MemoryPressureListener::RegisterSystemCallback(
    /* [in] */ IContext* context)
{
    AutoPtr<IComponentCallbacks2> callback2 = new InnerComponentCallbacks2();
    AutoPtr<IComponentCallbacks> callback = IComponentCallbacks::Probe(callback2);
    context->RegisterComponentCallbacks(callback);
}

/**
 * Used by applications to simulate a memory pressure signal. By throwing certain intent
 * actions.
 */
Boolean MemoryPressureListener::HandleDebugIntent(
    /* [in] */ IActivity* activity,
    /* [in] */ const String& action)
{
    if (ACTION_LOW_MEMORY.Equals(action)) {
        SimulateLowMemoryPressureSignal(activity);
    }
    else if (ACTION_TRIM_MEMORY.Equals(action)) {
        SimulateTrimMemoryPressureSignal(activity, IComponentCallbacks2::TRIM_MEMORY_COMPLETE);
    }
    else if (ACTION_TRIM_MEMORY_RUNNING_CRITICAL.Equals(action)) {
        SimulateTrimMemoryPressureSignal(activity, IComponentCallbacks2::TRIM_MEMORY_MODERATE);
    }
    else if (ACTION_TRIM_MEMORY_MODERATE.Equals(action)) {
        SimulateTrimMemoryPressureSignal(activity,
                IComponentCallbacks2::TRIM_MEMORY_RUNNING_CRITICAL);
    }
    else {
        return FALSE;
    }

    return TRUE;
}

void MemoryPressureListener::MaybeNotifyMemoryPresure(
    /* [in] */ Int32 level)
{
    if (level >= IComponentCallbacks2::TRIM_MEMORY_COMPLETE) {
        NativeOnMemoryPressure(MemoryPressureLevelList::MEMORY_PRESSURE_CRITICAL);
    }
    else if (level >= IComponentCallbacks2::TRIM_MEMORY_BACKGROUND ||
            level == IComponentCallbacks2::TRIM_MEMORY_RUNNING_CRITICAL) {
        // Don't notifiy on TRIM_MEMORY_UI_HIDDEN, since this class only
        // dispatches actionable memory pressure signals to native.
        NativeOnMemoryPressure(MemoryPressureLevelList::MEMORY_PRESSURE_MODERATE);
    }
}

void MemoryPressureListener::SimulateLowMemoryPressureSignal(
    /* [in] */ IActivity* activity)
{
    // The Application and the Activity each have a list of callbacks they notify when this
    // method is called.  Notifying these will simulate the event at the App/Activity level
    // as well as trigger the listener bound from native in this process.
    AutoPtr<IApplication> app;
    activity->GetApplication((IApplication**)&app);
    app->OnLowMemory();
    activity->OnLowMemory();
}

void MemoryPressureListener::SimulateTrimMemoryPressureSignal(
    /* [in] */ IActivity* activity,
    /* [in] */ Int32 level)
{
    // The Application and the Activity each have a list of callbacks they notify when this
    // method is called.  Notifying these will simulate the event at the App/Activity level
    // as well as trigger the listener bound from native in this process.
    AutoPtr<IApplication> app;
    activity->GetApplication((IApplication**)&app);
    app->OnTrimMemory(level);
    activity->OnTrimMemory(level);
}

void MemoryPressureListener::NativeOnMemoryPressure(
    /* [in] */ Int32 memoryPressureType)
{
    Elastos_MemoryPressureListener_nativeOnMemoryPressure(memoryPressureType);
}

void MemoryPressureListener::RegisterSystemCallback(
    /* [in] */ IInterface* context)
{
    AutoPtr<IContext> c = IContext::Probe(context);
    RegisterSystemCallback(c);
}

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
