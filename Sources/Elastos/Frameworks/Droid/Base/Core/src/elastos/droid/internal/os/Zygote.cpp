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

#include "elastos/droid/internal/os/Zygote.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/Thread.h>
#include <elastos/droid/system/Os.h>
#include <elastos/utility/logging/Logger.h>

// sys/mount.h has to come before linux/fs.h due to redefinition of MS_RDONLY, MS_BIND, etc
#include <sys/mount.h>
#include <linux/fs.h>

#include <grp.h>
#include <fcntl.h>
#include <paths.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/capability.h>
#include <sys/personality.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>

#include <cutils/fs.h>
#include <cutils/multiuser.h>
#include <cutils/sched_policy.h>
#include <private/android_filesystem_config.h>
#include <utils/String8.h>
#include <selinux/android.h>
#include <processgroup/processgroup.h>
#include <inttypes.h>
#include "nativebridge/native_bridge.h"

using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::System::CZygoteHooks;
using Elastos::Core::StringUtils;
using Elastos::Core::Thread;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

static AutoPtr<IZygoteHooks> InitZygoteHooks()
{
    AutoPtr<IZygoteHooks> hooks;
    CZygoteHooks::New((IZygoteHooks**)&hooks);
    return hooks;
}

const AutoPtr<IZygoteHooks> Zygote::VM_HOOKS = InitZygoteHooks();
static const String TAG("Zygote");
static const Boolean DEBUG = TRUE;

Int32 Zygote::ForkAndSpecialize(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid,
    /* [in] */ ArrayOf<Int32>* gids,
    /* [in] */ Int32 debugFlags,
    /* [in] */ ArrayOf< Int32Array >* rlimits, //int[][] rlimits,
    /* [in] */ Int32 mountExternal,
    /* [in] */ const String& seInfo,
    /* [in] */ const String& niceName,
    /* [in] */ ArrayOf<Int32>* fdsToClose,
    /* [in] */ const String& instructionSet,
    /* [in] */ const String& appDataDir)
{
    if (DEBUG) Logger::I(TAG, "Zygote::ForkAndSpecialize: %s", niceName.string());

    Int32 startTime = SystemClock::GetElapsedRealtime();
    VM_HOOKS->PreFork();
    CheckTime(startTime, String("Zygote.preFork"));
    Int32 pid = NativeForkAndSpecialize(
        uid, gid, gids, debugFlags, rlimits, mountExternal,
        seInfo, niceName, fdsToClose, instructionSet, appDataDir);
    CheckTime(startTime, String("Zygote.nativeForkAndSpecialize"));
    VM_HOOKS->PostForkCommon();
    CheckTime(startTime, String("Zygote.postForkCommon"));
    return pid;
}

/**
 * Temporary hack: check time since start time and log if over a fixed threshold.
 *
 */
void Zygote::CheckTime(
    /* [in] */ Int64 startTime,
    /* [in] */ const String& where)
{
    Int32 now = SystemClock::GetElapsedRealtime();
    if ((now-startTime) > 1000) {
        // If we are taking more than a second, log about it.
        Logger::W(TAG, "Slow operation: %lldms so far, now at %s",  (now-startTime), where.string());
    }
}

Int32 Zygote::ForkSystemServer(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid,
    /* [in] */ ArrayOf<Int32>* gids,
    /* [in] */ Int32 debugFlags,
    /* [in] */ ArrayOf< Int32Array >* rlimits, //int[][] rlimits,
    /* [in] */ Int64 permittedCapabilities,
    /* [in] */ Int64 effectiveCapabilities)
{
    if (DEBUG) Logger::I(TAG, "Zygote::ForkSystemServer");

    VM_HOOKS->PreFork();
    Int32 pid = NativeForkSystemServer(
            uid, gid, gids, debugFlags, rlimits, permittedCapabilities, effectiveCapabilities);
    VM_HOOKS->PostForkCommon();
    return pid;
}

void Zygote::CallPostForkChildHooks(
    /* [in] */ Int32 debugFlags,
    /* [in] */ const String& instructionSet)
{
    long startTime = SystemClock::GetElapsedRealtime();
    VM_HOOKS->PostForkChild(debugFlags, instructionSet);
    CheckTime(startTime, String("Zygote.callPostForkChildHooks"));
}

ECode Zygote::ExecShell(
    /* [in] */ const String& command)
{
    if (DEBUG) Logger::I(TAG, "Zygote::ExecShell: %s", command.string());
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(3);
    (*args)[0] = "/system/bin/sh";
    (*args)[1] = "-c";
    (*args)[2] = command;
    ECode ec = Elastos::Droid::System::Os::Execv((*args)[0], args);
    if (ec == (ECode)E_ERRNO_EXCEPTION) {
        ec = E_RUNTIME_EXCEPTION;
    }
    return ec;
}

void Zygote::AppendQuotedShellArgs(
    /* [in] */ StringBuilder* command,
    /* [in] */ ArrayOf<String>* args)
{
    if (args) {
        for (Int32 i = 0; i < args->GetLength(); i++) {
            command->Append(" '");
            String rarg;
            StringUtils::ReplaceAll((*args)[i], "'", "'\\''", &rarg);
            command->Append(rarg);
            command->Append("'");
        }
    }
}

using android::String8;
static pid_t sSystemServerPid = 0;

// Must match values in com.android.internal.os.Zygote.
enum MountExternalKind {
  MOUNT_EXTERNAL_NONE = 0,
  MOUNT_EXTERNAL_SINGLEUSER = 1,
  MOUNT_EXTERNAL_MULTIUSER = 2,
  MOUNT_EXTERNAL_MULTIUSER_ALL = 3,
};

// This signal handler is for zygote mode, since the zygote must reap its children
static void SigChldHandler(int /*signal_number*/)
{
    pid_t pid;
    int status;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // Log process-death status that we care about.  In general it is
        // not safe to call LOG(...) from a signal handler because of
        // possible reentrancy.  However, we know a priori that the
        // current implementation of LOG() is safe to call from a SIGCHLD
        // handler in the zygote process.  If the LOG() implementation
        // changes its locking strategy or its use of syscalls within the
        // lazy-init critical section, its use here may become unsafe.
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status)) {
                Logger::I(TAG, "Process %d exited cleanly (%d)", pid, WEXITSTATUS(status));
            }
        } else if (WIFSIGNALED(status)) {
            if (WTERMSIG(status) != SIGKILL) {
                Logger::I(TAG, "Process %d exited due to signal (%d)", pid, WTERMSIG(status));
            }
            if (WCOREDUMP(status)) {
                Logger::I(TAG, "Process %d dumped core.", pid);
            }
        }

        // If the just-crashed process is the system_server, bring down zygote
        // so that it is restarted by init and system server will be restarted
        // from there.
        if (pid == sSystemServerPid) {
            Logger::E(TAG, "Exit zygote because system server (%d) has terminated");
            kill(getpid(), SIGKILL);
        }
    }

    // Note that we shouldn't consider ECHILD an error because
    // the secondary zygote might have no children left to wait for.
    if (pid < 0 && errno != ECHILD) {
        Logger::W(TAG, "Zygote SIGCHLD error in waitpid: %s", strerror(errno));
    }
}

// Configures the SIGCHLD handler for the zygote process. This is configured
// very late, because earlier in the runtime we may fork() and exec()
// other processes, and we want to waitpid() for those rather than
// have them be harvested immediately.
//
// This ends up being called repeatedly before each fork(), but there's
// no real harm in that.
static void SetSigChldHandler()
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SigChldHandler;

    int err = sigaction(SIGCHLD, &sa, NULL);
    if (err < 0) {
        Logger::W(TAG, "Error setting SIGCHLD handler: %d", errno);
    }
}

// Sets the SIGCHLD handler back to default behavior in zygote children.
static void UnsetSigChldHandler()
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_DFL;

    int err = sigaction(SIGCHLD, &sa, NULL);
    if (err < 0) {
        Logger::W(TAG, "Error unsetting SIGCHLD handler: %d", errno);
    }
}

// Calls POSIX setgroups() using the int[] object as an argument.
// A NULL argument is tolerated.
static void SetGids(
    /* [in] */ ArrayOf<Int32>* gidArray)
{
    if (gidArray == NULL) {
        return;
    }

    Int32 len = gidArray->GetLength();
    /* just in case gid_t and u4 are different... */
    gid_t* gids = (gid_t *)alloca(sizeof(gid_t) * len);

    for (Int32 i = 0; i < len; i++) {
       gids[i] = (gid_t)((*gidArray)[i]);
    }
    int rc = setgroups(len, gids);
    if (rc == -1) {
        Logger::E(TAG, "setgroups failed");
    }
}

// Sets the resource limits via setrlimit(2) for the values in the
// two-dimensional array of integers that's passed in. The second dimension
// contains a tuple of length 3: (resource, rlim_cur, rlim_max). NULL is
// treated as an empty array.
static void SetRLimits(
    /* [in] */ ArrayOf< Int32Array >* rlimits)
{
    if (rlimits == NULL) {
        return;
    }

    rlimit rlim;
    memset(&rlim, 0, sizeof(rlim));

    for (int i = 0; i < rlimits->GetLength(); ++i) {
        Int32Array array = (*rlimits)[i];
        rlim.rlim_cur = (*array)[1];
        rlim.rlim_max = (*array)[2];

        int rc = setrlimit((*array)[0], &rlim);
        if (rc == -1) {
            Logger::E(TAG, "setrlimit(%d, {%d, %d}) failed", (*array)[0], rlim.rlim_cur, rlim.rlim_max);
        }
    }
}

// The debug malloc library needs to know whether it's the zygote or a child.
extern "C" int gMallocLeakZygoteChild;

static void EnableKeepCapabilities()
{
    int rc = prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0);
    if (rc == -1) {
        Logger::E(TAG, "prctl(PR_SET_KEEPCAPS) failed");
    }
}

static void DropCapabilitiesBoundingSet()
{
    for (int i = 0; prctl(PR_CAPBSET_READ, i, 0, 0, 0) >= 0; i++) {
        int rc = prctl(PR_CAPBSET_DROP, i, 0, 0, 0);
        if (rc == -1) {
            if (errno == EINVAL) {
                Logger::E(TAG, "prctl(PR_CAPBSET_DROP) failed with EINVAL. Please verify "
                    "your kernel is compiled with file capabilities support");
            } else {
                Logger::E(TAG, "prctl(PR_CAPBSET_DROP) failed");
            }
        }
    }
}

static void SetCapabilities(
    /* [in] */ int64_t permitted,
    /* [in] */ int64_t effective)
{
    __user_cap_header_struct capheader;
    memset(&capheader, 0, sizeof(capheader));
    capheader.version = _LINUX_CAPABILITY_VERSION_3;
    capheader.pid = 0;

    __user_cap_data_struct capdata[2];
    memset(&capdata, 0, sizeof(capdata));
    capdata[0].effective = effective;
    capdata[1].effective = effective >> 32;
    capdata[0].permitted = permitted;
    capdata[1].permitted = permitted >> 32;

    if (capset(&capheader, &capdata[0]) == -1) {
        Logger::E(TAG, "capset(%lld, %lld) failed", permitted, effective);
    }
}

static void SetSchedulerPolicy()
{
    errno = -set_sched_policy(0, SP_DEFAULT);
    if (errno != 0) {
        Logger::E(TAG, "set_sched_policy(0, SP_DEFAULT) failed");
    }
}

// Create a private mount namespace and bind mount appropriate emulated
// storage for the given user.
static bool MountEmulatedStorage(
    /* [in] */ uid_t uid,
    /* [in] */ Int32 mount_mode,
    /* [in] */ bool force_mount_namespace)
{
    if (mount_mode == MOUNT_EXTERNAL_NONE && !force_mount_namespace) {
        return true;
    }

    // Create a second private mount namespace for our process
    if (unshare(CLONE_NEWNS) == -1) {
        Logger::W(TAG, "Failed to unshare(): %d", errno);
        return false;
    }

    if (mount_mode == MOUNT_EXTERNAL_NONE) {
        return true;
    }

    // See storage config details at http://source.android.com/tech/storage/
    userid_t user_id = multiuser_get_user_id(uid);

    // Create bind mounts to expose external storage
    if (mount_mode == MOUNT_EXTERNAL_MULTIUSER || mount_mode == MOUNT_EXTERNAL_MULTIUSER_ALL) {
        // These paths must already be created by init.rc
        const char* source = getenv("EMULATED_STORAGE_SOURCE");
        const char* target = getenv("EMULATED_STORAGE_TARGET");
        const char* legacy = getenv("EXTERNAL_STORAGE");
        if (source == NULL || target == NULL || legacy == NULL) {
            Logger::W(TAG, "Storage environment undefined; unable to provide external storage");
            return false;
        }

        // Prepare source paths

        // /mnt/shell/emulated/0
        const String8 source_user(String8::format("%s/%d", source, user_id));
        // /storage/emulated/0
        const String8 target_user(String8::format("%s/%d", target, user_id));

        if (fs_prepare_dir(source_user.string(), 0000, 0, 0) == -1
            || fs_prepare_dir(target_user.string(), 0000, 0, 0) == -1) {
            return false;
        }

        if (mount_mode == MOUNT_EXTERNAL_MULTIUSER_ALL) {
            // Mount entire external storage tree for all users
            if (TEMP_FAILURE_RETRY(mount(source, target, NULL, MS_BIND, NULL)) == -1) {
                Logger::W(TAG, "Failed to mount %s to %s :%d", source, target, errno);
                return false;
            }
        } else {
            // Only mount user-specific external storage
            if (TEMP_FAILURE_RETRY(
                mount(source_user.string(), target_user.string(), NULL, MS_BIND, NULL)) == -1) {
                Logger::W(TAG, "Failed to mount %s to %s: %d", source_user.string(), target_user.string(), errno);
                return false;
            }
        }

        if (fs_prepare_dir(legacy, 0000, 0, 0) == -1) {
            return false;
        }

        // Finally, mount user-specific path into place for legacy users
        if (TEMP_FAILURE_RETRY(
                mount(target_user.string(), legacy, NULL, MS_BIND | MS_REC, NULL)) == -1) {
            Logger::W(TAG, "Failed to mount %s to %s: %d", target_user.string(), legacy, errno);
            return false;
        }
    } else {
        Logger::W(TAG, "Mount mode %d unsupported", mount_mode);
        return false;
    }

    return true;
}

static bool NeedsNoRandomizeWorkaround()
{
#if !defined(__arm__)
    return false;
#else
    int major;
    int minor;
    struct utsname uts;
    if (uname(&uts) == -1) {
        return false;
    }

    if (sscanf(uts.release, "%d.%d", &major, &minor) != 2) {
        return false;
    }

    // Kernels before 3.4.* need the workaround.
    return (major < 3) || ((major == 3) && (minor < 4));
#endif
}

// Utility to close down the Zygote socket file descriptors while
// the child is still running as root with Zygote's privileges.  Each
// descriptor (if any) is closed via dup2(), replacing it with a valid
// (open) descriptor to /dev/null.

static void DetachDescriptors(
    /* [in] */ ArrayOf<Int32>* fdsToClose)
{
    if (!fdsToClose) {
        return;
    }

    Int32 count = fdsToClose->GetLength();
    Int32 i;
    int devnull;
    for (i = 0; i < count; i++) {
        devnull = open("/dev/null", O_RDWR);
        if (devnull < 0) {
            Logger::E(TAG, "Failed to open /dev/null");
            continue;
        }
        Logger::V(TAG, "Switching descriptor %d to /dev/null: %d", (*fdsToClose)[i], errno);
        if (dup2(devnull, (*fdsToClose)[i]) < 0) {
            Logger::E(TAG, "Failed dup2() on descriptor %d", (*fdsToClose)[i]);
        }
        close(devnull);
    }
}

void SetThreadName(
    /* [in] */ const char* thread_name)
{
    bool hasAt = false;
    bool hasDot = false;
    const char* s = thread_name;
    while (*s) {
        if (*s == '.') {
            hasDot = true;
        } else if (*s == '@') {
            hasAt = true;
        }
        s++;
    }
    const int len = s - thread_name;
    if (len < 15 || hasAt || !hasDot) {
        s = thread_name;
    } else {
        s = thread_name + len - 15;
    }
    // pthread_setname_np fails rather than truncating long strings.
    char buf[16];       // MAX_TASK_COMM_LEN=16 is hard-coded into bionic
    strlcpy(buf, s, sizeof(buf)-1);
    errno = pthread_setname_np(pthread_self(), buf);
    if (errno != 0) {
        Logger::W(TAG, "Unable to set the name of current thread to '%s'", buf);
    }
}

// Temporary timing check.
uint64_t MsTime()
{
    timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return static_cast<uint64_t>(now.tv_sec) * UINT64_C(1000) + now.tv_nsec / UINT64_C(1000000);
}


void ckTime(uint64_t start, const char* where)
{
    uint64_t now = MsTime();
    if ((now-start) > 1000) {
        // If we are taking more than a second, log about it.
        Logger::W(TAG, "Slow operation: %llu ms in %s", (uint64_t)(now-start), where);
    }
}

// Utility routine to fork zygote and specialize the child process.
static pid_t ForkAndSpecializeCommon(
    /* [in] */ uid_t uid,
    /* [in] */ gid_t gid,
    /* [in] */ ArrayOf<Int32>* gids,
    /* [in] */ Int32 debugFlags,
    /* [in] */ ArrayOf< Int32Array >* rlimits, //int[][] rlimits,
    /* [in] */ Int64 permittedCapabilities,
    /* [in] */ Int64 effectiveCapabilities,
    /* [in] */ Int32 mountExternal,
    /* [in] */ const String& seInfo,
    /* [in] */ const String& niceName,
    /* [in] */ Boolean isSystemServer,
    /* [in] */ ArrayOf<Int32>* fdsToClose,
    /* [in] */ const String& instructionSet,
    /* [in] */ const String& dataDir)
{
    uint64_t start = MsTime();
    SetSigChldHandler();
    ckTime(start, "ForkAndSpecializeCommon:SetSigChldHandler");

    pid_t pid = fork();

    if (pid == 0) {
        // The child process.
        gMallocLeakZygoteChild = 1;


        // Clean up any descriptors which must be closed immediately
        DetachDescriptors(fdsToClose);

        ckTime(start, "ForkAndSpecializeCommon:Fork and detach");

        // Keep capabilities across UID change, unless we're staying root.
        if (uid != 0) {
            EnableKeepCapabilities();
        }

        DropCapabilitiesBoundingSet();

        bool need_native_bridge = false;
        if (instructionSet != NULL) {
            need_native_bridge = android::NeedsNativeBridge(instructionSet.string());
        }

        if (!MountEmulatedStorage(uid, mountExternal, need_native_bridge)) {
            Logger::W(TAG, "Failed to mount emulated storage: %d", errno);
            if (errno == ENOTCONN || errno == EROFS) {
                // When device is actively encrypting, we get ENOTCONN here
                // since FUSE was mounted before the framework restarted.
                // When encrypted device is booting, we get EROFS since
                // FUSE hasn't been created yet by init.
                // In either case, continue without external storage.
            } else {
                Logger::E(TAG, "Cannot continue without emulated storage");
            }
        }

        if (!isSystemServer) {
            int rc = createProcessGroup(uid, getpid());
            if (rc != 0) {
                if (rc == -EROFS) {
                    Logger::W(TAG, "createProcessGroup failed, kernel missing CONFIG_CGROUP_CPUACCT?");
                } else {
                    Logger::E(TAG, "createProcessGroup(%d, %d) failed: %s", uid, pid, strerror(-rc));
                }
            }
        }

        SetGids(gids);

        SetRLimits(rlimits);

        if (!isSystemServer && need_native_bridge) {
            // Set the environment for the apps running with native bridge.
            if (dataDir == NULL) {
                android::PreInitializeNativeBridge(NULL, instructionSet.string());
            } else {
                android::PreInitializeNativeBridge(dataDir.string(), instructionSet.string());
            }
        }

        int rc = setresgid(gid, gid, gid);
        if (rc == -1) {
            Logger::E(TAG, "setresgid(%d) failed", gid);
        }

        rc = setresuid(uid, uid, uid);
        if (rc == -1) {
            Logger::E(TAG, "setresuid(%d) failed", uid);
        }

        if (NeedsNoRandomizeWorkaround()) {
            // Work around ARM kernel ASLR lossage (http://b/5817320).
            int old_personality = personality(0xffffffff);
            int new_personality = personality(old_personality | ADDR_NO_RANDOMIZE);
            if (new_personality == -1) {
                Logger::W(TAG, "personality(%d) failed", new_personality);
            }
        }

        SetCapabilities(permittedCapabilities, effectiveCapabilities);

        SetSchedulerPolicy();

        const char* se_info_c_str = NULL;
        if (seInfo != NULL) {
            se_info_c_str = seInfo.string();
            if (se_info_c_str == NULL) {
                Logger::E(TAG, "se_info_c_str == NULL");
            }
        }
        const char* se_name_c_str = NULL;
        if (niceName != NULL) {
            se_name_c_str = niceName.string();
            if (se_name_c_str == NULL) {
                Logger::E(TAG, "se_name_c_str == NULL");
            }
        }
        rc = selinux_android_setcontext(uid, isSystemServer, se_info_c_str, se_name_c_str);
        if (rc == -1) {
            Logger::E(TAG, "selinux_android_setcontext(%d, %d, \"%s\", \"%s\") failed", uid,
                isSystemServer, se_info_c_str, se_name_c_str);
        }

        // Make it easier to debug audit logs by setting the main thread's name to the
        // nice name rather than "app_process".
        if (se_info_c_str == NULL && isSystemServer) {
            se_name_c_str = "system_server";
        }
        if (se_info_c_str != NULL) {
            SetThreadName(se_name_c_str);
        }

        UnsetSigChldHandler();

        ckTime(start, "ForkAndSpecializeCommon:child process setup");

        Zygote::CallPostForkChildHooks(
            debugFlags, isSystemServer ? String(NULL) : instructionSet);
        ckTime(start, "ForkAndSpecializeCommon:PostForkChildHooks returns");

    } else if (pid > 0) {
    // the parent process
    }
    return pid;
}


Int32 Zygote::NativeForkAndSpecialize(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid,
    /* [in] */ ArrayOf<Int32>* gids,
    /* [in] */ Int32 debugFlags,
    /* [in] */ ArrayOf< Int32Array >* rlimits, //int[][] rlimits,
    /* [in] */ Int32 mountExternal,
    /* [in] */ const String& seInfo,
    /* [in] */ const String& niceName,
    /* [in] */ ArrayOf<Int32>* fdsToClose,
    /* [in] */ const String& instructionSet,
    /* [in] */ const String& appDataDir)
{
    Int64 capabilities = 0;
    if (uid == AID_BLUETOOTH) {
        capabilities |= (1LL << CAP_WAKE_ALARM);
    }

    return ForkAndSpecializeCommon(uid, gid, gids, debugFlags,
            rlimits, capabilities, capabilities, mountExternal, seInfo,
            niceName, FALSE, fdsToClose, instructionSet, appDataDir);
}

Int32 Zygote::NativeForkSystemServer(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid,
    /* [in] */ ArrayOf<Int32>* gids,
    /* [in] */ Int32 debugFlags,
    /* [in] */ ArrayOf< Int32Array >* rlimits, //int[][] rlimits,
    /* [in] */ Int64 permittedCapabilities,
    /* [in] */ Int64 effectiveCapabilities)
{
    pid_t pid = ForkAndSpecializeCommon(
        uid, gid, gids, debugFlags, rlimits, permittedCapabilities, effectiveCapabilities,
        MOUNT_EXTERNAL_NONE, String(NULL), String(NULL), TRUE, NULL, String(NULL), String(NULL));
    if (pid > 0) {
        // The zygote process checks whether the child process has died or not.
        Logger::I(TAG, "System server process %d has been created", pid);
        sSystemServerPid = pid;
        // There is a slight window that the system server process has crashed
        // but it went unnoticed because we haven't published its pid yet. So
         // we recheck here just to make sure that all is well.
        int status;
        if (waitpid(pid, &status, WNOHANG) == pid) {
            Logger::E(TAG, "System server process %d has died. Restarting Zygote!", pid);
            // RuntimeAbort(env);
        }
    }
    return pid;
}

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos
