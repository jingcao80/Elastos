
#ifndef __ELASTOS_DROID_INTERNAL_OS_RUNTIMEINIT_H__
#define __ELASTOS_DROID_INTERNAL_OS_RUNTIMEINIT_H__

#include "Elastos.Droid.Core.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

class RuntimeInit
    : public Singleton
{
public:
    /**
     * Handles argument parsing for args related to the runtime.
     *
     * Current recognized args:
     * <ul>
     *   <li> <code> [--] &lt;start class name&gt;  &lt;args&gt;
     * </ul>
     */
    class Arguments : public ElRefBase
    {
    public:
        /**
         * Constructs instance and parses args
         * @param args runtime command-line args
         * @throws IllegalArgumentException
         */
        Arguments(
            /* [in] */ const ArrayOf<String>& args);

    private:
        /**
         * Parses the commandline arguments intended for the Runtime.
         */
        CARAPI ParseArgs(
            /* [in] */ const ArrayOf<String>& args);

    public:
        /** first non-option argument */
        String mStartClass;

        /** all following arguments */
        AutoPtr< ArrayOf<String> > mStartArgs;

        String mModule;
    };

public:
    /**
     * The main function called when started through the zygote process. This
     * could be unified with main(), if the native code in nativeFinishInit()
     * were rationalized with Zygote startup.<p>
     *
     * Current recognized args:
     * <ul>
     *   <li> <code> [--] &lt;start class name&gt;  &lt;args&gt;
     * </ul>
     *
     * @param targetSdkVersion target SDK version
     * @param argv arg strings
     */
    static CARAPI ZygoteInit(
        /* [in] */ Int32 targetSdkVersion,
        /* [in] */ ArrayOf<String>* argv,
        /* [out] */ IRunnable** task);

    static CARAPI Main(
        /* [in] */ const ArrayOf<String>& args);

    /**
     * Redirect System.out and System.err to the Android log.
     */
    static CARAPI_(void) RedirectLogStreams();

private:
    /**
     * Invokes a static "main(argv[]) method on class "className".
     * Converts various failing exceptions into RuntimeExceptions, with
     * the assumption that they will then cause the VM instance to exit.
     *
     * @param className Fully-qualified class name
     * @param argv Argument vector for main()
     */
    static CARAPI InvokeStaticMain(
        /* [in] */ const String& moduleName,
        /* [in] */ const String& className,
        /* [in] */ ArrayOf<String>* argv,
        /* [out] */ IRunnable** task);

    static CARAPI ApplicationInit(
        /* [in] */ Int32 targetSdkVersion,
        /* [in] */ ArrayOf<String>* argv,
        /* [out] */ IRunnable** task);

private:
    static const String TAG;
    static const Boolean DEBUG;
};

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_OS_RUNTIMEINIT_H__
