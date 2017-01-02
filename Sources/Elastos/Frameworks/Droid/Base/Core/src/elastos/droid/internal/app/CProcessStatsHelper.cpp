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

#include "elastos/droid/internal/app/CProcessStatsHelper.h"
#include "elastos/droid/internal/app/ProcessStats.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CAR_INTERFACE_IMPL(CProcessStatsHelper, Singleton, IProcessStatsHelper)
CAR_SINGLETON_IMPL(CProcessStatsHelper)

ECode CProcessStatsHelper::PrintScreenLabelCsv(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int32 offset)
{
    ProcessStats::PrintScreenLabelCsv(pw, offset);
    return NOERROR;
}

ECode CProcessStatsHelper::PrintMemLabelCsv(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int32 offset)
{
    ProcessStats::PrintMemLabelCsv(pw, offset);
    return NOERROR;
}

ECode CProcessStatsHelper::DumpSingleTime(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in] */ ArrayOf<Int64>* durations,
    /* [in] */ Int32 curState,
    /* [in] */ Int64 curStartTime,
    /* [in] */ Int64 now,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ProcessStats::DumpSingleTime(pw, prefix, *durations, curState, curStartTime, now);
    return NOERROR;
}

ECode CProcessStatsHelper::ComputeProcessData(
    /* [in] */ IProcessState* proc,
    /* [in] */ IProcessDataCollection* data,
    /* [in] */ Int64 now)
{
    return ProcessStats::ComputeProcessData(proc, data, now);
}

ECode CProcessStatsHelper::DumpProcessListCsv(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ IArrayList* procs,
    /* [in] */ Boolean sepScreenStates,
    /* [in] */ ArrayOf<Int32>* screenStates,
    /* [in] */ Boolean sepMemStates,
    /* [in] */ ArrayOf<Int32>* memStates,
    /* [in] */ Boolean sepProcStates,
    /* [in] */ ArrayOf<Int32>* procStates,
    /* [in] */ Int64 now)
{
    return ProcessStats::DumpProcessListCsv(pw, procs, sepScreenStates, screenStates,
        sepMemStates, memStates, sepProcStates, procStates, now);
}

ECode CProcessStatsHelper::DumpSingleServiceTime(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in] */ IServiceState* service,
    /* [in] */ Int32 serviceType,
    /* [in] */ Int32 curState,
    /* [in] */ Int64 curStartTime,
    /* [in] */ Int64 now,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ProcessStats::DumpSingleServiceTime(
        pw, prefix, service, serviceType, curState, curStartTime, now);
    return NOERROR;
}

ECode CProcessStatsHelper::GetCOMMIT_PERIOD(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ProcessStats::COMMIT_PERIOD;
    return NOERROR;
}

ECode CProcessStatsHelper::GetCOMMIT_UPTIME_PERIOD(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ProcessStats::COMMIT_UPTIME_PERIOD;
    return NOERROR;
}

ECode CProcessStatsHelper::GetALL_MEM_ADJ(
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = ProcessStats::ALL_MEM_ADJ;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CProcessStatsHelper::GetALL_SCREEN_ADJ(
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = ProcessStats::ALL_SCREEN_ADJ;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CProcessStatsHelper::GetNON_CACHED_PROC_STATES(
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = ProcessStats::NON_CACHED_PROC_STATES;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CProcessStatsHelper::GetBACKGROUND_PROC_STATES(
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = ProcessStats::BACKGROUND_PROC_STATES;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CProcessStatsHelper::GetALL_PROC_STATES(
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = ProcessStats::ALL_PROC_STATES;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CProcessStatsHelper::GetADJ_SCREEN_NAMES_CSV(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = ProcessStats::ADJ_SCREEN_NAMES_CSV;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CProcessStatsHelper::GetADJ_MEM_NAMES_CSV(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = ProcessStats::ADJ_MEM_NAMES_CSV;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CProcessStatsHelper::GetSTATE_NAMES_CSV(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = ProcessStats::STATE_NAMES_CSV;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos
