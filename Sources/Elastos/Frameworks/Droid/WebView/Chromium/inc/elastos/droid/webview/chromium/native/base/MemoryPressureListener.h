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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_MEMORYPRESSURELISTENER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_MEMORYPRESSURELISTENER_H__

#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IComponentCallbacks2;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

/**
 * This is an internal implementation of the C++ counterpart.
 * It registers a ComponentCallbacks2 with the system, and dispatches into
 * native for levels that are considered actionable.
 */
class MemoryPressureListener
    : public Object
{
private:
    class InnerComponentCallbacks2
        : public Object
        , public IComponentCallbacks2
    {
    public:
        InnerComponentCallbacks2();

        CAR_INTERFACE_DECL()

        //@Override
        CARAPI OnTrimMemory(
            /* [in] */ Int32 level);

        //@Override
        CARAPI OnLowMemory();

        //@Override
        CARAPI OnConfigurationChanged(
            /* [in] */ IConfiguration* configuration);
    };

public:
    /**
     * Used by applications to simulate a memory pressure signal. By throwing certain intent
     * actions.
     */
    static CARAPI_(Boolean) HandleDebugIntent(
        /* [in] */ IActivity* activity,
        /* [in] */ const String& action);

    static CARAPI_(void) MaybeNotifyMemoryPresure(
        /* [in] */ Int32 level);

    static CARAPI_(void*) ElaMemoryPressureListenerCallback_Init();

private:
    static CARAPI_(void) RegisterSystemCallback(
        /* [in] */ IInterface* context);

    //@CalledByNative
    static CARAPI_(void) RegisterSystemCallback(
        /* [in] */ IContext* context);

    static CARAPI_(void) SimulateLowMemoryPressureSignal(
        /* [in] */ IActivity* activity);

    static CARAPI_(void) SimulateTrimMemoryPressureSignal(
        /* [in] */ IActivity* activity,
        /* [in] */ Int32 level);

    static CARAPI_(void) NativeOnMemoryPressure(
        /* [in] */ Int32 memoryPressureType);

private:
    /**
     * Sending an intent with this action to Chrome will cause it to issue a call to onLowMemory
     * thus simulating a low memory situations.
     */
    static const String ACTION_LOW_MEMORY;

    /**
     * Sending an intent with this action to Chrome will cause it to issue a call to onTrimMemory
     * thus simulating a low memory situations.
     */
    static const String ACTION_TRIM_MEMORY;

    /**
     * Sending an intent with this action to Chrome will cause it to issue a call to onTrimMemory
     * with notification level TRIM_MEMORY_RUNNING_CRITICAL thus simulating a low memory situation
     */
    static const String ACTION_TRIM_MEMORY_RUNNING_CRITICAL;

    /**
     * Sending an intent with this action to Chrome will cause it to issue a call to onTrimMemory
     * with notification level TRIM_MEMORY_MODERATE thus simulating a low memory situation
     */
    static const String ACTION_TRIM_MEMORY_MODERATE;
};

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_MEMORYPRESSURELISTENER_H__
