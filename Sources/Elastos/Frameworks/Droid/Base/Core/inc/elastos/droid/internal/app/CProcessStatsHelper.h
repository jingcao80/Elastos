
#ifndef __ELASTOS_DROID_INTERNAL_APP_CPROCESSSTATSHELPER_H__
#define __ELASTOS_DROID_INTERNAL_APP_CPROCESSSTATSHELPER_H__

#include "_Elastos_Droid_Internal_App_CProcessStatsHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CarClass(CProcessStatsHelper)
    , public Singleton
    , public IProcessStatsHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI PrintScreenLabelCsv(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int32 offset);

    CARAPI PrintMemLabelCsv(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int32 offset);

    CARAPI DumpSingleTime(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix,
        /* [in] */ ArrayOf<Int64>* durations,
        /* [in] */ Int32 curState,
        /* [in] */ Int64 curStartTime,
        /* [in] */ Int64 now,
        /* [out] */ Int64* result);

    CARAPI ComputeProcessData(
        /* [in] */ IProcessState* proc,
        /* [in] */ IProcessDataCollection* data,
        /* [in] */ Int64 now);

    CARAPI DumpProcessListCsv(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ IArrayList* procs,
        /* [in] */ Boolean sepScreenStates,
        /* [in] */ ArrayOf<Int32>* screenStates,
        /* [in] */ Boolean sepMemStates,
        /* [in] */ ArrayOf<Int32>* memStates,
        /* [in] */ Boolean sepProcStates,
        /* [in] */ ArrayOf<Int32>* procStates,
        /* [in] */ Int64 now);

    CARAPI DumpSingleServiceTime(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix,
        /* [in] */ IServiceState* service,
        /* [in] */ Int32 serviceType,
        /* [in] */ Int32 curState,
        /* [in] */ Int64 curStartTime,
        /* [in] */ Int64 now,
        /* [out] */ Int64* result);

    CARAPI GetCOMMIT_PERIOD(
        /* [out] */ Int64* result);

    CARAPI GetCOMMIT_UPTIME_PERIOD(
        /* [out] */ Int64* result);

    CARAPI GetALL_MEM_ADJ(
        /* [out, callee] */ ArrayOf<Int32>** result);

    CARAPI GetALL_SCREEN_ADJ(
        /* [out, callee] */ ArrayOf<Int32>** result);

    CARAPI GetNON_CACHED_PROC_STATES(
        /* [out, callee] */ ArrayOf<Int32>** result);

    CARAPI GetBACKGROUND_PROC_STATES(
        /* [out, callee] */ ArrayOf<Int32>** result);

    CARAPI GetALL_PROC_STATES(
        /* [out, callee] */ ArrayOf<Int32>** result);

    CARAPI GetADJ_SCREEN_NAMES_CSV(
        /* [out, callee] */ ArrayOf<String>** result);

    CARAPI GetADJ_MEM_NAMES_CSV(
        /* [out, callee] */ ArrayOf<String>** result);

    CARAPI GetSTATE_NAMES_CSV(
        /* [out, callee] */ ArrayOf<String>** result);
};

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_APP_CPROCESSSTATSHELPER_H__
