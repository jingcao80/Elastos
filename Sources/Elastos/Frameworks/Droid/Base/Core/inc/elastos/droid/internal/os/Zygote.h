
#ifndef __ELASTOS_DROID_INTERNAL_OS_ZYGOTE_H__
#define __ELASTOS_DROID_INTERNAL_OS_ZYGOTE_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::System::IZygoteHooks;
using Elastos::Core::IStringBuilder;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

class Zygote
{
private:
    Zygote() {}

public:
    /**
     * Forks a new VM instance.  The current VM must have been started
     * with the -Xzygote flag. <b>NOTE: new instance keeps all
     * root capabilities. The new process is expected to call capset()</b>.
     *
     * @param uid the UNIX uid that the new process should setuid() to after
     * fork()ing and and before spawning any threads.
     * @param gid the UNIX gid that the new process should setgid() to after
     * fork()ing and and before spawning any threads.
     * @param gids null-ok; a list of UNIX gids that the new process should
     * setgroups() to after fork and before spawning any threads.
     * @param debugFlags bit flags that enable debugging features.
     * @param rlimits null-ok an array of rlimit tuples, with the second
     * dimension having a length of 3 and representing
     * (resource, rlim_cur, rlim_max). These are set via the posix
     * setrlimit(2) call.
     * @param seInfo null-ok a string specifying SELinux information for
     * the new process.
     * @param niceName null-ok a string specifying the process name.
     * @param fdsToClose an array of ints, holding one or more POSIX
     * file descriptor numbers that are to be closed by the child
     * (and replaced by /dev/null) after forking.  An integer value
     * of -1 in any entry in the array means "ignore this one".
     * @param instructionSet null-ok the instruction set to use.
     * @param appDataDir null-ok the data directory of the app.
     *
     * @return 0 if this is the child, pid of the child
     * if this is the parent, or -1 on error.
     */
    static CARAPI_(Int32) ForkAndSpecialize(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 gid,
        /* [in] */ ArrayOf<Int32>* gids,
        /* [in] */ Int32 debugFlags,
        /* [in] */ ArrayOf<Int32*>* rlimits, //int[][] rlimits,
        /* [in] */ Int32 mountExternal,
        /* [in] */ const String& seInfo,
        /* [in] */ const String& niceName,
        /* [in] */ ArrayOf<Int32>* fdsToClose,
        /* [in] */ const String& instructionSet,
        /* [in] */ const String& appDataDir);

    static CARAPI_(Int32) NativeForkAndSpecialize(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 gid,
        /* [in] */ ArrayOf<Int32>* gids,
        /* [in] */ Int32 debugFlags,
        /* [in] */ ArrayOf<Int32*>* rlimits, //int[][] rlimits,
        /* [in] */ Int32 mountExternal,
        /* [in] */ const String& seInfo,
        /* [in] */ const String& niceName,
        /* [in] */ ArrayOf<Int32>* fdsToClose,
        /* [in] */ const String& instructionSet,
        /* [in] */ const String& appDataDir);

    /**
     * Special method to start the system server process. In addition to the
     * common actions performed in forkAndSpecialize, the pid of the child
     * process is recorded such that the death of the child process will cause
     * zygote to exit.
     *
     * @param uid the UNIX uid that the new process should setuid() to after
     * fork()ing and and before spawning any threads.
     * @param gid the UNIX gid that the new process should setgid() to after
     * fork()ing and and before spawning any threads.
     * @param gids null-ok; a list of UNIX gids that the new process should
     * setgroups() to after fork and before spawning any threads.
     * @param debugFlags bit flags that enable debugging features.
     * @param rlimits null-ok an array of rlimit tuples, with the second
     * dimension having a length of 3 and representing
     * (resource, rlim_cur, rlim_max). These are set via the posix
     * setrlimit(2) call.
     * @param permittedCapabilities argument for setcap()
     * @param effectiveCapabilities argument for setcap()
     *
     * @return 0 if this is the child, pid of the child
     * if this is the parent, or -1 on error.
     */
    static CARAPI_(Int32) ForkSystemServer(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 gid,
        /* [in] */ ArrayOf<Int32>* gids,
        /* [in] */ Int32 debugFlags,
        /* [in] */ ArrayOf<Int32*>* rlimits, //int[][] rlimits,
        /* [in] */ Int64 permittedCapabilities,
        /* [in] */ Int64 effectiveCapabilities);

    static CARAPI_(Int32) NativeForkSystemServer(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 gid,
        /* [in] */ ArrayOf<Int32>* gids,
        /* [in] */ Int32 debugFlags,
        /* [in] */ ArrayOf<Int32*>* rlimits, //int[][] rlimits,
        /* [in] */ Int64 permittedCapabilities,
        /* [in] */ Int64 effectiveCapabilities);

    /**
     * Executes "/system/bin/sh -c &lt;command&gt;" using the exec() system call.
     * This method throws a runtime exception if exec() failed, otherwise, this
     * method never returns.
     *
     * @param command The shell command to execute.
     */
    static CARAPI ExecShell(
        /* [in] */ const String& command);

    /**
     * Appends quotes shell arguments to the specified string builder.
     * The arguments are quoted using single-quotes, escaped if necessary,
     * prefixed with a space, and appended to the command.
     *
     * @param command A string builder for the shell command being constructed.
     * @param args An array of argument strings to be quoted and appended to the command.
     * @see #execShell(String)
     */
    static CARAPI_(void) AppendQuotedShellArgs(
        /* [in] */ IStringBuilder* command,
        /* [in] */ const ArrayOf<String>& args);


    static CARAPI_(void) CallPostForkChildHooks(
        /* [in] */ Int32 debugFlags,
        /* [in] */ const String& instructionSet);

private:
    /**
     * Temporary hack: check time since start time and log if over a fixed threshold.
     *
     */
    static CARAPI_(void) CheckTime(
        /* [in] */ Int64 startTime,
        /* [in] */ const String& where);

private:
    static const AutoPtr<IZygoteHooks> VM_HOOKS;
};

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_OS_ZYGOTE_H__
