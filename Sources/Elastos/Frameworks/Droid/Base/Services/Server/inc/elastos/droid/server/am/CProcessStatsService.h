
#ifndef __ELASTOS_DROID_SERVER_AM_CPROCESSSTATSSERVICE_H__
#define __ELASTOS_DROID_SERVER_AM_CPROCESSSTATSSERVICE_H__

#include "_Elastos_Droid_Server_Am_CProcessStatsService.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include <elastos/droid/os/Runnable.h>
#include <elastos/core/Object.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Internal::App::IIProcessStats;
using Elastos::Droid::Internal::App::IProcessStats;
using Elastos::Droid::Internal::App::IProcessState;
using Elastos::Droid::Internal::App::IServiceState;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Droid::View::IApplicationToken;
using Elastos::Core::Thread;
using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Concurrent::Locks::ILock;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class CActivityManagerService;

CarClass(CProcessStatsService)
    , public Object
    , public IIProcessStats
    , public IBinder
{
private:
    class CallbackRunnable : public Runnable
    {
    public:
        CallbackRunnable(
            /* [in] */ CProcessStatsService* host)
            : mHost(host)
        {}

        // @Override
        CARAPI Run();

    private:
        CProcessStatsService* mHost;
    };

    class WriteStateRunnable : public Runnable
    {
    public:
        WriteStateRunnable(
            /* [in] */ CProcessStatsService* host)
            : mHost(host)
        {}

        // @Override
        CARAPI Run();

    private:
        CProcessStatsService* mHost;
    };

    class PipeOutputThread : public Thread
    {
    public:
        PipeOutputThread(
            /* [in] */ IParcelFileDescriptor* fd,
            /* [in] */ ArrayOf<Byte>* outData);

        CARAPI Run();

    private:
        AutoPtr<IParcelFileDescriptor> mFd;
        AutoPtr<ArrayOf<Byte> > mOutData;
    };

public:
    CProcessStatsService();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IIActivityManager* am,
        /* [in] */ IFile* file);

    CARAPI_(AutoPtr<IProcessState>) GetProcessStateLocked(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 versionCode,
        /* [in] */ const String& processName);

    CARAPI_(AutoPtr<IServiceState>) GetServiceStateLocked(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 versionCode,
        /* [in] */ const String& processName,
        /* [in] */ const String& className);

    CARAPI_(Boolean) IsMemFactorLowered();

    CARAPI_(Boolean) SetMemFactorLocked(
        /* [in] */ Int32 memFactor,
        /* [in] */ Boolean screenOn,
        /* [in] */ Int64 now);

    CARAPI_(Int32) GetMemFactorLocked();

    CARAPI_(void) AddSysMemUsageLocked(
        /* [in] */ Int64 cachedMem,
        /* [in] */ Int64 freeMem,
        /* [in] */ Int64 zramMem,
        /* [in] */ Int64 kernelMe,
        /* [in] */ Int64 nativeMem);

    CARAPI_(Boolean) ShouldWriteNowLocked(
        /* [in] */ Int64 now);

    CARAPI_(void) ShutdownLocked();

    CARAPI_(void) WriteStateAsyncLocked();

    CARAPI_(void) WriteStateSyncLocked();

    CARAPI_(void) WriteStateLocked(
        /* [in] */ Boolean sync,
        /* [in] */ Boolean commit);

    CARAPI_(void) PerformWriteState();

    CARAPI_(Boolean) ReadLocked(
        /* [in] */ IProcessStats* stats,
        /* [in] */ IAtomicFile* file);

    CARAPI_(void) TrimHistoricStatesWriteLocked();

    CARAPI_(Boolean) DumpFilteredProcessesCsvLocked(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& header,
        /* [in] */ Boolean sepScreenStates,
        /* [in] */ ArrayOf<Int32>* screenStates,
        /* [in] */ Boolean sepMemStates,
        /* [in] */ ArrayOf<Int32>* memStates,
        /* [in] */ Boolean sepProcStates,
        /* [in] */ ArrayOf<Int32>* procStates,
        /* [in] */ Int64 now,
        /* [in] */ const String& reqPackage);

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) ParseStateList(
        /* [in] */ ArrayOf<String>* states,
        /* [in] */ Int32 mult,
        /* [in] */ const String& arg,
        /* [in] */ ArrayOf<Boolean>* outSep,
        /* [in] */ ArrayOf<String>* outError);

    CARAPI GetCurrentStats(
        /* [out, callee] */ ArrayOf<IParcelFileDescriptor*>** historic,
        /* [out, callee] */ ArrayOf<Byte>** bytes);

    CARAPI GetStatsOverTime(
        /* [in] */ Int64 minTime,
        /* [out] */ IParcelFileDescriptor** pfd);

    CARAPI GetCurrentMemoryState(
        /* [out] */ Int32* state);

    CARAPI ToString(
        /* [out] */ String* str);

protected:
    // @Override
    CARAPI_(void) Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    CARAPI_(void) WriteStateLocked(
        /* [in] */ Boolean sync);

    CARAPI_(void) UpdateFile();

    CARAPI_(AutoPtr<List<String> >) GetCommittedFiles(
        /* [in] */ Int32 minNum,
        /* [in] */ Boolean inclCurrent,
        /* [in] */ Boolean inclCheckedIn);

    CARAPI_(void) DumpAggregatedStats(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int64 aggregateHours,
        /* [in] */ Int64 now,
        /* [in] */ const String& reqPackage,
        /* [in] */ Boolean isCompact,
        /* [in] */ Boolean dumpDetails,
        /* [in] */ Boolean dumpFullDetails,
        /* [in] */ Boolean dumpAll,
        /* [in] */ Boolean activeOnly);

    static CARAPI_(void) DumpHelp(
        /* [in] */ IPrintWriter* pw);

    CARAPI_(void) DumpInner(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

public:
    static const String TAG;
    static const Boolean DEBUG;

    // Most data is kept in a sparse data structure: an integer array which integer
    // holds the type of the entry, and the identifier for a Int64 array that data
    // exists in and the offset into the array to find it.  The constants below
    // define the encoding of that data in an integer.

    static const Int32 MAX_HISTORIC_STATES = 8;   // Maximum number of historic states we will keep.
    static const String STATE_FILE_PREFIX; // Prefix to use for state filenames.
    static const String STATE_FILE_SUFFIX; // Suffix to use for state filenames.
    static const String STATE_FILE_CHECKIN_SUFFIX; // State files that have checked in.
    static Int64 WRITE_PERIOD;      // Write file every 30 minutes or so.

    CActivityManagerService* mAm;
    AutoPtr<IFile> mBaseDir;
    AutoPtr<IProcessStats> mProcessStats;
    AutoPtr<IAtomicFile> mFile;
    Boolean mCommitPending;
    Boolean mShuttingDown;
    Int32 mLastMemOnlyState;
    Boolean mMemFactorLowered;

    AutoPtr<ILock> mWriteLock; // IReentrantLock
    Object mPendingWriteLock;
    AutoPtr<IAtomicFile> mPendingWriteFile;
    AutoPtr<IParcel> mPendingWrite;
    Boolean mPendingWriteCommitted;
    Int64 mLastWriteTime;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_CPROCESSSTATSSERVICE_H__
