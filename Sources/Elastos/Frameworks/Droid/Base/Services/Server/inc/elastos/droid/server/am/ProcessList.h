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

#ifndef __ELASTOS_DROID_SERVER_WM_PROCESSLIST_H__
#define __ELASTOS_DROID_SERVER_WM_PROCESSLIST_H__

#include "Elastos.Droid.Net.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::ILocalSocket;
using Elastos::Core::IStringBuilder;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IOutputStream;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {
    class CWindowManagerService;
}}}}

using Elastos::Droid::Server::Wm::CWindowManagerService;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class ProcessList : public Object
{
public:
    ProcessList();

    CARAPI ApplyDisplaySize(
        /* [in] */ CWindowManagerService* wm);

    static CARAPI_(Int32) ComputeEmptyProcessLimit(
        /* [in] */ Int32 totalProcessLimit);

    static CARAPI_(String) MakeOomAdjString(
        /* [in] */ Int32 setAdj);

    static CARAPI_(String) MakeProcStateString(
        /* [in] */ Int32 curProcState);

    static CARAPI_(void) AppendRamKb(
        /* [in] */ IStringBuilder* sb,
        /* [in] */ Int64 ramKb);

    static CARAPI_(Boolean) ProcStatesDifferForMem(
        /* [in] */ Int32 procState1,
        /* [in] */ Int32 procState2);

    static CARAPI_(Int64) ComputeNextPssTime(
        /* [in] */ Int32 procState,
        /* [in] */ Boolean first,
        /* [in] */ Boolean sleeping,
        /* [in] */ Int64 now);

    CARAPI_(Int64) GetMemLevel(
        /* [in] */ Int32 adjustment);

    /**
     * Return the maximum pss size in kb that we consider a process acceptable to
     * restore from its cached state for running in the background when RAM is low.
     */
    CARAPI_(Int64) GetCachedRestoreThresholdKb();

    /**
     * Set the out-of-memory badness adjustment for a process.
     *
     * @param pid The process identifier to set.
     * @param uid The uid of the app
     * @param amt Adjustment value -- lmkd allows -16 to +15.
     *
     * {@hide}
     */
    static CARAPI_(void) SetOomAdj(
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 amt);

    /*
     * {@hide}
     */
    static CARAPI_(void) Remove(
        /* [in] */ Int32 pid);

    using Object::ToString;

    CARAPI_(String) ToString();

private:
    CARAPI UpdateOomLevels(
        /* [in] */ Int32 displayWidth,
        /* [in] */ Int32 displayHeight,
        /* [in] */ Boolean write);

    static CARAPI_(String) BuildOomTag(
        /* [in] */ const String& prefix,
        /* [in] */ const String& space,
        /* [in] */ Int32 val,
        /* [in] */ Int32 base);

    static CARAPI_(Boolean) OpenLmkdSocket();

    static CARAPI_(void) WriteLmkd(
        /* [in] */ IByteBuffer* buf);

public:
    // The minimum time we allow between crashes, for us to consider this
    // application to be bad and stop and its services and reject broadcasts.
    static const Int32 MIN_CRASH_INTERVAL = 60*1000;

    // OOM adjustments for processes in various states:

    // Adjustment used in certain places where we don't know it yet.
    // (Generally this is something that is going to be cached, but we
    // don't know the exact value in the cached range to assign yet.)
    static const Int32 UNKNOWN_ADJ = 16;

    // This is a process only hosting activities that are not visible,
    // so it can be killed without any disruption.
    static const Int32 CACHED_APP_MAX_ADJ = 15;
    static const Int32 CACHED_APP_MIN_ADJ = 9;

    // The B list of SERVICE_ADJ -- these are the old and decrepit
    // services that aren't as shiny and interesting as the ones in the A list.
    static const Int32 SERVICE_B_ADJ = 8;

    // This is the process of the previous application that the user was in.
    // This process is kept above other things, because it is very common to
    // switch back to the previous app.  This is important both for recent
    // task switch (toggling between the two top recent apps) as well as normal
    // UI flow such as clicking on a URI in the e-mail app to view in the browser,
    // and then pressing back to return to e-mail.
    static const Int32 PREVIOUS_APP_ADJ = 7;

    // This is a process holding the home application -- we want to try
    // avoiding killing it, even if it would normally be in the background,
    // because the user interacts with it so much.
    static const Int32 HOME_APP_ADJ = 6;

    // This is a process holding an application service -- killing it will not
    // have much of an impact as far as the user is concerned.
    static const Int32 SERVICE_ADJ = 5;

    // This is a process with a heavy-weight application.  It is in the
    // background, but we want to try to avoid killing it.  Value set in
    // system/rootdir/init.rc on startup.
    static const Int32 HEAVY_WEIGHT_APP_ADJ = 4;

    // This is a process currently hosting a backup operation.  Killing it
    // is not entirely fatal but is generally a bad idea.
    static const Int32 BACKUP_APP_ADJ = 3;

    // This is a process only hosting components that are perceptible to the
    // user, and we really want to avoid killing them, but they are not
    // immediately visible. An example is background music playback.
    static const Int32 PERCEPTIBLE_APP_ADJ = 2;

    // This is a process only hosting activities that are visible to the
    // user, so we'd prefer they don't disappear.
    static const Int32 VISIBLE_APP_ADJ = 1;

    // This is the process running the current foreground app.  We'd really
    // rather not kill it!
    static const Int32 FOREGROUND_APP_ADJ = 0;

    // This is a process that the system or a persistent process has bound to,
    // and indicated it is important.
    static const Int32 PERSISTENT_SERVICE_ADJ = -11;

    // This is a system persistent process, such as telephony.  Definitely
    // don't want to kill it, but doing so is not completely fatal.
    static const Int32 PERSISTENT_PROC_ADJ = -12;

    // The system process runs at the default adjustment.
    static const Int32 SYSTEM_ADJ = -16;

    // Special code for native processes that are not being managed by the system (so
    // don't have an oom adj assigned by the system).
    static const Int32 NATIVE_ADJ = -17;

    // Memory pages are 4K.
    static const Int32 MEMORY_PAGE_SIZE = 4 * 1024;

    // The minimum number of cached apps we want to be able to keep around,
    // without empty apps being able to push them out of memory.
    static const Int32 MIN_CACHED_APPS = 2;

    // The maximum number of cached processes we will keep around before killing them.
    // NOTE: this constant is *only* a control to not let us go too crazy with
    // keeping around processes on devices with large amounts of RAM.  For devices that
    // are tighter on RAM, the out of memory killer is responsible for killing background
    // processes as RAM is needed, and we should *never* be relying on this limit to
    // kill them.  Also note that this limit only applies to cached background processes;
    // we have no limit on the number of service, visible, foreground, or other such
    // processes and the number of those processes does not count against the cached
    // process limit.
    static const Int32 MAX_CACHED_APPS;

    // We allow empty processes to stick around for at most 30 minutes.
    static const Int64 MAX_EMPTY_TIME = 30*60*1000;

    // The number of empty apps at which we don't consider it necessary to do
    // memory trimming.
    static const Int32 TRIM_EMPTY_APPS;

    // The number of cached at which we don't consider it necessary to do
    // memory trimming.
    static const Int32 TRIM_CACHED_APPS;

    // Threshold of number of cached+empty where we consider memory critical.
    static const Int32 TRIM_CRITICAL_THRESHOLD = 3;

    // Threshold of number of cached+empty where we consider memory critical.
    static const Int32 TRIM_LOW_THRESHOLD = 5;

    // Low Memory Killer Daemon command codes.
    // These must be kept in sync with the definitions in lmkd.c
    //
    // LMK_TARGET <minfree> <minkillprio> ... (up to 6 pairs)
    // LMK_PROCPRIO <pid> <prio>
    // LMK_PROCREMOVE <pid>
    static const Byte LMK_TARGET = 0;
    static const Byte LMK_PROCPRIO = 1;
    static const Byte LMK_PROCREMOVE = 2;

    // The minimum amount of time after a state change it is safe ro collect PSS.
    static const Int32 PSS_MIN_TIME_FROM_STATE_CHANGE = 15*1000;

    // The maximum amount of time we want to go between PSS collections.
    static const Int32 PSS_MAX_INTERVAL = 30*60*1000;

    // The minimum amount of time between successive PSS requests for *all* processes.
    static const Int32 PSS_ALL_INTERVAL = 10*60*1000;

    static const Int32 PROC_MEM_PERSISTENT = 0;
    static const Int32 PROC_MEM_TOP = 1;
    static const Int32 PROC_MEM_IMPORTANT = 2;
    static const Int32 PROC_MEM_SERVICE = 3;
    static const Int32 PROC_MEM_CACHED = 4;

private:
    // The maximum number of empty app processes we will let sit around.
    static const Int32 MAX_EMPTY_APPS;

    // The minimum amount of time between successive PSS requests for a process.
    static const Int32 PSS_SHORT_INTERVAL = 2*60*1000;

    // The amount of time until PSS when a process first becomes top.
    static const Int32 PSS_FIRST_TOP_INTERVAL = 10*1000;

    // The amount of time until PSS when a process first goes into the background.
    static const Int32 PSS_FIRST_BACKGROUND_INTERVAL = 20*1000;

    // The amount of time until PSS when a process first becomes cached.
    static const Int32 PSS_FIRST_CACHED_INTERVAL = 30*1000;

    // The amount of time until PSS when an important process stays in the same state.
    static const Int32 PSS_SAME_IMPORTANT_INTERVAL = 15*60*1000;

    // The amount of time until PSS when a service process stays in the same state.
    static const Int32 PSS_SAME_SERVICE_INTERVAL = 20*60*1000;

    // The amount of time until PSS when a cached process stays in the same state.
    static const Int32 PSS_SAME_CACHED_INTERVAL = 30*60*1000;

    static const Int32 sProcStateToProcMem[14];
    static const Int64 sFirstAwakePssTimes[14];
    static const Int64 sSameAwakePssTimes[14];

    // These are the various interesting memory levels that we will give to
    // the OOM killer.  Note that the OOM killer only supports 6 slots, so we
    // can't give it a different value for every possible kind of process.
    AutoPtr< ArrayOf<Int32> > mOomAdj;
    // These are the low-end OOM level limits.  This is appropriate for an
    // HVGA or smaller phone with less than 512MB.  Values are in KB.
    AutoPtr< ArrayOf<Int32> > mOomMinFreeLow;
    // These are the high-end OOM level limits.  This is appropriate for a
    // 1280x800 or larger screen with around 1GB RAM.  Values are in KB.
    AutoPtr< ArrayOf<Int32> > mOomMinFreeHigh;
    // The actual OOM killer memory levels we are using.
    AutoPtr< ArrayOf<Int32> > mOomMinFree;
    // Optimal OOM killer memory levels for Low-Tier devices.
    AutoPtr< ArrayOf<Int32> > mOomMinFreeLowRam;

    Int64 mTotalMemMb;

    Int64 mCachedRestoreLevel;

    Boolean mHaveDisplaySize;

    static AutoPtr<ILocalSocket> sLmkdSocket;
    static AutoPtr<IOutputStream> sLmkdOutputStream;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_WM_PROCESSLIST_H__
