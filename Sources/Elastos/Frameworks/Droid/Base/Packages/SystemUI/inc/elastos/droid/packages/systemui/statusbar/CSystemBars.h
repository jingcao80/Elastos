
#ifndef  __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_CSYSTEMBARS_H__
#define  __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_CSYSTEMBARS_H__

#include "_Elastos_Droid_Packages_SystemUI_StatusBar_CSystemBars.h"
#include "elastos/droid/packages/systemui/statusbar/ServiceMonitor.h"
#include "elastos/droid/packages/systemui/SystemUI.h"

using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Packages {
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
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

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
} // namespace Packages
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_CSYSTEMBARS_H__
