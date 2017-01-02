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

#include "stdlib.h"
#include "coredef.h"
#include "Globals.h"
#include "NativeThread.h"

namespace Elastos {
namespace Core {

ELAPI_(Int32) CoreStartup();

CoreGlobals::CoreGlobals()
{
    ASSERT_TRUE(CoreStartup() == 0);
}

/* global state */
INIT_PROI_1 struct CoreGlobals gCore;

/*
 * Set defaults for fields altered or modified by arguments.
 *
 * Globals are initialized to 0 (a/k/a NULL or false).
 */
ELAPI_(void) SetCommandLineDefaults()
{
    const char* envStr;

    // envStr = getenv("CLASSPATH");
    // if (envStr != NULL)
    //     gDvm.classPathStr = strdup(envStr);
    // else
    //     gDvm.classPathStr = strdup(".");
    envStr = getenv("ELBOOTCLASSPATH");
    if (envStr != NULL) {
        gCore.mBootClassPathStr = strdup(envStr);
    }
    else {
        gCore.mBootClassPathStr = strdup(".");
    }

    /* Defaults overridden by -Xms and -Xmx.
     * TODO: base these on a system or application-specific default
     */
    gCore.mHeapSizeStart = 2 * 1024 * 1024;   // Spec says 16MB; too big for us.
    gCore.mHeapSizeMax = 16 * 1024 * 1024;    // Spec says 75% physical mem
    gCore.mStackSize = kDefaultStackSize;

    //gDvm.concurrentMarkSweep = true;

    /* gDvm.jdwpSuspend = true; */

    /* allowed unless zygote config doesn't allow it */
    //gDvm.jdwpAllowed = true;

    /* default verification and optimization modes */
    // gDvm.classVerifyMode = VERIFY_MODE_ALL;
    // gDvm.dexOptMode = OPTIMIZE_MODE_VERIFIED;

    /*
     * Default execution mode.
     *
     * This should probably interact with the mterp code somehow, e.g. if
     * we know we're using the "desktop" build we should probably be
     * using "portable" rather than "fast".
     */
// #if defined(WITH_JIT)
//     gDvm.executionMode = kExecutionModeJit;
// #else
//     gDvm.executionMode = kExecutionModeInterpFast;
// #endif

    /*
     * SMP support is a compile-time define, but we may want to have
     * dexopt target a differently-configured device.
     */
    //gDvm.dexOptForSmp = (ANDROID_SMP != 0);
}

/*
 * All threads have stopped.  Finish the shutdown procedure.
 *
 * We can also be called if startup fails partway through, so be prepared
 * to deal with partially initialized data.
 *
 * Free any storage allocated in gGlobals.
 *
 * We can't dlclose() shared libs we've loaded, because it's possible a
 * thread not associated with the VM is running code in one.
 *
 * This is called from the JNI DestroyJavaVM function, which can be
 * called from any thread.  (In practice, this will usually run in the
 * same thread that started the VM, a/k/a the main thread, but we don't
 * want to assume that.)
 */
ELAPI_(void) CoreShutdown()
{}

/*
 * VM initialization.  Pass in any options provided on the command line.
 * Do not pass in the class name or the options for the class.
 *
 * Returns 0 on success.
 */
ELAPI_(Int32) CoreStartup()
{
    //LOGV("VM init args (%d):\n", argc);
    // for (i = 0; i < argc; i++)
    //     LOGV("  %d: '%s'\n", i, argv[i]);

    SetCommandLineDefaults();

    /*
     * Initialize components.
     */
    if (!NativeThreadStartup()) {
        goto fail;
    }

    /*
     * At this point, the VM is in a pretty good state.  Finish prep on
     * the main thread (specifically, create a java.lang.Thread object to go
     * along with our Thread struct).  Note we will probably be executing
     * some interpreted class initializer code in here.
     */
    if (!NativePrepMainThread()) {
        goto fail;
    }

    // assert(!dvmCheckException(dvmThreadSelf()));
    // gDvm.initExceptionCount = 0;

    return 0;

fail:
    CoreShutdown();
    return 1;
}

} // namespace Core
} // namespace Elastos
