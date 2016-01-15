
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_APP_CONTENTMAIN_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_APP_CONTENTMAIN_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IContext;

// import org.chromium.base.JNINamespace;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace App {

/**
 * This class is used to initialize all types of process. It corresponds to
 * content/public/app/content_main.h which is not used in Android as it has
 * the different initialization process.
 *
 * TODO(michaelbai): Refactorying the BrowserProcessMain.java and the
 * ChildProcessService.java to start ContentMain, and run the process
 * specific initialization code in ContentMainRunner::Initialize.
 *
 **/
//@JNINamespace("content")
class ContentMain
{
public:
    /**
     * Initialize application context in native side.
     **/
    static CARAPI_(void) InitApplicationContext(
        /* [in] */ IContext* context);

    /**
     * Start the ContentMainRunner in native side.
     **/
    static CARAPI_(Int32) Start();

private:
    static CARAPI_(void) NativeInitApplicationContext(
        /* [in] */ IContext* context);

    static CARAPI_(Int32) NativeStart();
};

} // namespace App
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_APP_CONTENTMAIN_H__
