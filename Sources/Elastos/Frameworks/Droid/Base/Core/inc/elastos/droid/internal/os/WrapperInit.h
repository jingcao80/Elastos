
#ifndef __ELASTOS_DROID_INTERNAL_OS_WRAPPER_INIT_H__
#define __ELASTOS_DROID_INTERNAL_OS_WRAPPER_INIT_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

/**
 * Startup class for the wrapper process.
 * @hide
 */
class WrapperInit
{
public:
    /**
     * The main function called when starting a runtime application through a
     * wrapper process instead of by forking Zygote.
     *
     * The first argument specifies the file descriptor for a pipe that should receive
     * the pid of this process, or 0 if none.
     *
     * The second argument is the target SDK version for the app.
     *
     * The remaining arguments are passed to the runtime.
     *
     * @param args The command-line arguments.
     */
     static CARAPI Main(
        /* [in] */ ArrayOf<String>* args);

    /**
     * Executes a runtime application with a wrapper command.
     * This method never returns.
     *
     * @param invokeWith The wrapper command.
     * @param niceName The nice name for the application, or null if none.
     * @param targetSdkVersion The target SDK version for the app.
     * @param pipeFd The pipe to which the application's pid should be written, or null if none.
     * @param args Arguments for {@link RuntimeInit#main}.
     */
     static CARAPI ExecApplication(
        /* [in] */ const String& invokeWith,
        /* [in] */ const String& niceName,
        /* [in] */ Int32 targetSdkVersion,
        /* [in] */ IFileDescriptor* pipeFd,
        /* [in] */ ArrayOf<String>* args);

    /**
     * Executes a standalone application with a wrapper command.
     * This method never returns.
     *
     * @param invokeWith The wrapper command.
     * @param classPath The class path.
     * @param className The class name to invoke.
     * @param args Arguments for the main() method of the specified class.
     */
     static CARAPI ExecStandalone(
        /* [in] */ const String& invokeWith,
        /* [in] */ const String& moduleName,
        /* [in] */ const String& className,
        /* [in] */ ArrayOf<String>* args);

private:
    /**
     * Class not instantiable.
     */
    WrapperInit();
    WrapperInit(const WrapperInit&);
};

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_OS_WRAPPER_INIT_H__
