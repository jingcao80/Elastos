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

#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_RECURSIVETASK_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_RECURSIVETASK_H__

#include "ForkJoinTask.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {

class RecursiveTask
    : public ForkJoinTask
    , public IRecursiveTask
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetRawResult(
        /* [out] */ IInterface** outface);

    CARAPI SetRawResult(
        /* [in] */ IInterface* value);

    CARAPI Exec(
        /* [out] */ Boolean* res);

public:
    static Int64 mSerialVersionUID;

    /**
     * The result of the computation.
     */
    AutoPtr<IInterface> mResult;
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_RECURSIVETASK_H__
