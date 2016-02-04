
#include "CZygoteHooks.h"
#include "CFile.h"
#include <elastos/core/Thread.h>

#if defined(HAVE_PRCTL)
#include <sys/prctl.h>
#endif

#include <sys/resource.h>

using Elastos::Core::Thread;
using Elastos::IO::IFile;
using Elastos::IO::CFile;

namespace Elastos {
namespace Droid {
namespace System {

CAR_OBJECT_IMPL(CZygoteHooks)

CAR_INTERFACE_IMPL(CZygoteHooks, Object, IZygoteHooks)

CZygoteHooks::CZygoteHooks()
    : mToken(0)
{
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
    assert(0 && "CZygoteHooks::NativePreFork is not implemented!");
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

    assert(0 && "CZygoteHooks::NativePostForkChild is not implemented!");
}

void CZygoteHooks::WaitUntilAllThreadsStopped()
{
    AutoPtr<IFile> tasks;
    CFile::New(String("/proc/self/task"), (IFile**)&tasks);
    AutoPtr< ArrayOf<String> > files;
    tasks->List((ArrayOf<String>**)&files);
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
