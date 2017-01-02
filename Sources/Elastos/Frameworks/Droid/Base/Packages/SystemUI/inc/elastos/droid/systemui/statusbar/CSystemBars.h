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

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CSYSTEMBARS_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CSYSTEMBARS_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CSystemBars.h"
#include "elastos/droid/systemui/statusbar/ServiceMonitor.h"
#include "elastos/droid/systemui/SystemUI.h"

using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

/**
 * Ensure a single status bar service implementation is running at all times.
 *
 * <p>The implementation either comes from a service component running in a remote process (defined
 * using a secure setting), else falls back to using the in-process implementation according
 * to the product config.
 */
CarClass(CSystemBars)
    , public SystemUI
    , public IServiceMonitorCallbacks
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    // @Override
    CARAPI Start();

    // @Override
    CARAPI OnNoService();

    // @Override
    CARAPI OnServiceStartAttempt(
        /* [out] */ Int64* result);

    // @Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

protected:
    // @Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

private:
    CARAPI_(void) CreateStatusBarFromConfig();

    // private RuntimeException andLog(String msg, Throwable t) {
    //     Log.w(TAG, msg, t);
    //     throw new RuntimeException(msg, t);
    // }

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const Int32 WAIT_FOR_BARS_TO_DIE;

    // manages the implementation coming from the remote process
    AutoPtr<ServiceMonitor> mServiceMonitor;

    // in-process fallback implementation, per the product config
    AutoPtr<IBaseStatusBar> mStatusBar;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CSYSTEMBARS_H__
