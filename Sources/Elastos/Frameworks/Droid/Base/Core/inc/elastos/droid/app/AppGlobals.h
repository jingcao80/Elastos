
#ifndef __ELASTOS_DROID_APP_APPGLOBALS_H__
#define __ELASTOS_DROID_APP_APPGLOBALS_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::App::IApplication;
using Elastos::Droid::Content::Pm::IIPackageManager;

namespace Elastos {
namespace Droid {
namespace App {

class ECO_PUBLIC AppGlobals
{
public:
    /**
     * Return the first Application object made in the process.
     * NOTE: Only works on the main thread.
     */
    static CARAPI_(AutoPtr<IApplication>) GetInitialApplication();

    /**
     * Return the package name of the first .apk loaded into the process.
     * NOTE: Only works on the main thread.
     */
    static CARAPI_(String) GetInitialPackage();

    /**
     * Return the raw interface to the package manager.
     * @return The package manager.
     */
    static CARAPI_(AutoPtr<IIPackageManager>) GetPackageManager();

    /**
     * Gets the value of an integer core setting.
     *
     * @param key The setting key.
     * @param defaultValue The setting default value.
     * @return The core settings.
     */
    static CARAPI_(Int32) GetIntCoreSetting(
        /* [in] */ const String& key,
        /* [in] */ Int32 defaultValue);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_APPGLOBALS_H__
