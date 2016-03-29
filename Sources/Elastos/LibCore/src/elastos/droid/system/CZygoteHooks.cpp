
#include "CZygoteHooks.h"
#include "CFile.h"
#include <elastos/core/Thread.h>
#include <elastos/core/NativeThread.h>
#include <elastos/utility/logging/Logger.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#if defined(HAVE_PRCTL)
#include <sys/prctl.h>
#endif

#include <sys/resource.h>

using Elastos::Core::Thread;
using Elastos::Core::NativeThread;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace System {

//======================================================================
// native codes : art/runtime/signal_catcher.cc
//======================================================================
static const String TAG("CZygoteHooks");
static pthread_t sSignalCatcherHandle;

static void* SignalCatcherThreadStart(void* arg);

/*
 * Crank up the signal catcher thread.
 *
 * Returns immediately.
 */
static Boolean SignalCatcherStartup()
{
    if (!Elastos::Core::NativeCreateInternalThread(&sSignalCatcherHandle,
        "signal catcher thread", SignalCatcherThreadStart, NULL)) {
        return FALSE;
    }

    return TRUE;
}

/*
 * Respond to a SIGQUIT by dumping the thread stacks.  Optionally dump
 * a few other things while we're at it.
 *
 * Thread stacks can either go to the log or to a file designated for holding
 * ANR traces.  If we're writing to a file, we want to do it in one shot,
 * so we can use a single O_APPEND write instead of contending for exclusive
 * access with flock().  There may be an advantage in resuming the VM
 * before doing the file write, so we don't stall the VM if disk I/O is
 * bottlenecked.
 *
 * If JIT tuning is compiled in, dump compiler stats as well.
 */
static void HandleSigQuit()
{
    //TOOD: not implemented
    Logger::W(TAG, "HandleSigQuit is not implemented!");
}

/*
 * Respond to a SIGUSR1 by forcing a GC.
 */
static void HandleSigUsr1()
{
    //TODO: not implemented
    Logger::W(TAG, "HandleSigUsr1 is not implemented!");
}

/*
 * Sleep in sigwait() until a signal arrives.
 */
static void* SignalCatcherThreadStart(void* arg)
{
    NativeThread* self = Elastos::Core::NativeThreadSelf();
    sigset_t mask;
    Int32 cc;

//    UNUSED_PARAMETER(arg);

    Logger::V(TAG, "Signal catcher thread started (threadid=%d)", self->mThreadId);

    /* set up mask with signals we want to handle */
    sigemptyset(&mask);
    sigaddset(&mask, SIGQUIT);
    sigaddset(&mask, SIGUSR1);

    while (TRUE) {
        int signal_number;

        Elastos::Core::NativeChangeStatus(self, Elastos::Core::NTHREAD_VMWAIT);

        /*
         * Signals for sigwait() must be blocked but not ignored.  We
         * block signals like SIGQUIT for all threads, so the condition
         * is met.  When the signal hits, we wake up, without any signal
         * handlers being invoked.
         *
         * When running under GDB we occasionally return from sigwait()
         * with EINTR (e.g. when other threads exit).
         */
loop:
        cc = sigwait(&mask, &signal_number);
        if (cc != 0) {
            if (cc == EINTR) {
                Logger::V(TAG, "sigwait: EINTR");
                goto loop;
            }
            assert(!"bad result from sigwait");
        }

        /* set our status to RUNNING, self-suspending if GC in progress */
        Elastos::Core::NativeChangeStatus(self, Elastos::Core::NTHREAD_RUNNING);

        switch (signal_number) {
        case SIGQUIT:
            HandleSigQuit();
            break;
        case SIGUSR1:
            HandleSigUsr1();
            break;
        default:
            Logger::E(TAG, "unexpected signal %d", signal_number);
            break;
        }
    }

    return NULL;
}

static void Abort()
{
    int result = 0;

    /*
     * If we call abort(), all threads in the process receives a SIBABRT.
     * debuggerd dumps the stack trace of the main thread, whether or not
     * that was the thread that failed.
     *
     * By stuffing a value into a bogus address, we cause a segmentation
     * fault in the current thread, and get a useful log from debuggerd.
     * We can also trivially tell the difference between a VM crash and
     * a deliberate abort by looking at the fault address.
     */
    *((char*)0xdeadd00d) = result;
    abort();

    /* notreached */
}

/*
 * Do non-zygote-mode initialization.  This is done during VM init for
 * standard startup, or after a "zygote fork" when creating a new process.
 */
static Boolean InitAfterZygote()
{
    if (!SignalCatcherStartup()) {
        return FALSE;
    }
    return TRUE;
}

//======================================================================
// CZygoteHooks
//======================================================================
CAR_OBJECT_IMPL(CZygoteHooks)

CAR_INTERFACE_IMPL(CZygoteHooks, Object, IZygoteHooks)

CZygoteHooks::CZygoteHooks()
    : mToken(0)
{
}

ECode CZygoteHooks::constructor()
{
  return NOERROR;
}

ECode CZygoteHooks::PreFork()
{
    // Daemons.stop();
    WaitUntilAllThreadsStopped();
    mToken = NativePreFork();
    return NOERROR;
}

/**
 * Called by the zygote in the child process after every fork. The debug
 * flags from {@code debugFlags} are applied to the child process. The string
 * {@code instructionSet} determines whether to use a native bridge.
 */
ECode CZygoteHooks::PostForkChild(
    /* [in] */ Int32 debugFlags,
    /* [in] */ const String& instructionSet)
{
    NativePostForkChild(mToken, debugFlags, instructionSet);
    return NOERROR;
}

ECode CZygoteHooks::PostForkCommon()
{
    // Daemons.start();
    return NOERROR;
}

Int64 CZygoteHooks::NativePreFork()
{
    // Runtime* runtime = Runtime::Current();
    // CHECK(runtime->IsZygote()) << "runtime instance not started with -Xzygote";

    // runtime->PreZygoteFork();

    // // Grab thread before fork potentially makes Thread::pthread_key_self_ unusable.
    // Thread* self = Thread::Current();
    // return reinterpret_cast<jlong>(self);
    // assert(0 && "CZygoteHooks::NativePreFork is not implemented!");
    return 0;
}

void CZygoteHooks::NativePostForkChild(
    /* [in] */ Int64 token,
    /* [in] */ Int32 debugFlags,
    /* [in] */ const String& instructionSet)
{
    // Thread* thread = reinterpret_cast<Thread*>(token);
    // // Our system thread ID, etc, has changed so reset Thread state.
    // thread->InitAfterFork();
    // EnableDebugFeatures(debug_flags);

    if (!InitAfterZygote()) {
        Logger::E(TAG, "error in post-zygote initialization");
        Abort();
    }


    // if (instruction_set != nullptr) {
    //     InstructionSet isa = GetInstructionSetFromString(instructionSet.string());
    //     Runtime::NativeBridgeAction action = Runtime::NativeBridgeAction::kUnload;
    //     if (isa != kNone && isa != kRuntimeISA) {
    //       action = Runtime::NativeBridgeAction::kInitialize;
    //     }
    //     Runtime::Current()->DidForkFromZygote(env, action, isa_string.c_str());
    // } else {
    //     Runtime::Current()->DidForkFromZygote(env, Runtime::NativeBridgeAction::kUnload, nullptr);
    // }

    // assert(0 && "CZygoteHooks::NativePostForkChild is not implemented!");
}

void CZygoteHooks::WaitUntilAllThreadsStopped()
{
    AutoPtr<IFile> tasks;
    CFile::New(String("/proc/self/task"), (IFile**)&tasks);
    AutoPtr< ArrayOf<String> > files;
    tasks->List((ArrayOf<String>**)&files);
    // if (files != NULL) {
    //     for (Int32 i = 0; i < files->GetLength(); ++i) {
    //         Logger::I("CZygoteHooks", " wait task %s to stop.", (*files)[i].string());
    //     }
    // }
    while (files != NULL && files->GetLength() > 1) {
        // try {
        // Experimentally, booting and playing about with a stingray, I never saw us
        // go round this loop more than once with a 10ms sleep.
        Thread::Sleep(10);
        // } catch (InterruptedException ignored) {
        // }
        files = NULL;
        tasks->List((ArrayOf<String>**)&files);
    }
}

// static void EnableDebugger()
// {
//   // To let a non-privileged gdbserver attach to this
//   // process, we must set our dumpable flag.
// #if defined(HAVE_PRCTL)
//   if (prctl(PR_SET_DUMPABLE, 1, 0, 0, 0) == -1) {
//     PLOG(ERROR) << "prctl(PR_SET_DUMPABLE) failed for pid " << getpid();
//   }
// #endif
//   // We don't want core dumps, though, so set the core dump size to 0.
//   rlimit rl;
//   rl.rlim_cur = 0;
//   rl.rlim_max = RLIM_INFINITY;
//   if (setrlimit(RLIMIT_CORE, &rl) == -1) {
//     PLOG(ERROR) << "setrlimit(RLIMIT_CORE) failed for pid " << getpid();
//   }
// }

// static void EnableDebugFeatures(uint32_t debug_flags)
// {
  // // Must match values in dalvik.system.Zygote.
  // enum {
  //   DEBUG_ENABLE_DEBUGGER           = 1,
  //   DEBUG_ENABLE_CHECKJNI           = 1 << 1,
  //   DEBUG_ENABLE_ASSERT             = 1 << 2,
  //   DEBUG_ENABLE_SAFEMODE           = 1 << 3,
  //   DEBUG_ENABLE_JNI_LOGGING        = 1 << 4,
  // };

  // if ((debug_flags & DEBUG_ENABLE_CHECKJNI) != 0) {
  //   Runtime* runtime = Runtime::Current();
  //   JavaVMExt* vm = runtime->GetJavaVM();
  //   if (!vm->check_jni) {
  //     LOG(INFO) << "Late-enabling -Xcheck:jni";
  //     vm->SetCheckJniEnabled(true);
  //     // There's only one thread running at this point, so only one JNIEnv to fix up.
  //     Thread::Current()->GetJniEnv()->SetCheckJniEnabled(true);
  //   } else {
  //     LOG(INFO) << "Not late-enabling -Xcheck:jni (already on)";
  //   }
  //   debug_flags &= ~DEBUG_ENABLE_CHECKJNI;
  // }

  // if ((debug_flags & DEBUG_ENABLE_JNI_LOGGING) != 0) {
  //   gLogVerbosity.third_party_jni = true;
  //   debug_flags &= ~DEBUG_ENABLE_JNI_LOGGING;
  // }

  // Dbg::SetJdwpAllowed((debug_flags & DEBUG_ENABLE_DEBUGGER) != 0);
  // if ((debug_flags & DEBUG_ENABLE_DEBUGGER) != 0) {
  //   EnableDebugger();
  // }
  // debug_flags &= ~DEBUG_ENABLE_DEBUGGER;

  // // These two are for backwards compatibility with Dalvik.
  // debug_flags &= ~DEBUG_ENABLE_ASSERT;
  // debug_flags &= ~DEBUG_ENABLE_SAFEMODE;

  // if (debug_flags != 0) {
  //   LOG(ERROR) << StringPrintf("Unknown bits set in debug_flags: %#x", debug_flags);
  // }
// }

}// namespace System
}// namespace Droid
}// namespace Elastos
