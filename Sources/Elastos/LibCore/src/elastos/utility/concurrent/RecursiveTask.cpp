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

#include "RecursiveTask.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {

//===============================================================================
// RecursiveTask::
//===============================================================================
Int64 RecursiveTask::mSerialVersionUID = 5232453952276485270L;

CAR_INTERFACE_IMPL(RecursiveTask, ForkJoinTask, IRecursiveTask)

ECode RecursiveTask::GetRawResult(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    *outface = mResult;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}

ECode RecursiveTask::SetRawResult(
    /* [in] */ IInterface* value)
{
    mResult = value;
    return NOERROR;
}

ECode RecursiveTask::Exec(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    Compute((IInterface**)&mResult);
    *res = TRUE;
    return NOERROR;
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
