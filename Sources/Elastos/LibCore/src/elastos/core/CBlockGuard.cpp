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

#include "CBlockGuard.h"
#include "BlockGuard.h"

namespace Elastos {
namespace Core {

CAR_SINGLETON_IMPL(CBlockGuard)

CAR_INTERFACE_IMPL(CBlockGuard, Singleton, IBlockGuard)

ECode CBlockGuard::GetThreadPolicy(
    /* [out] */ IBlockGuardPolicy** policy)
{
    VALIDATE_NOT_NULL(policy);

    return BlockGuard::GetThreadPolicy(policy);
}

ECode CBlockGuard::SetThreadPolicy(
    /* [in] */ IBlockGuardPolicy* policy)
{
    return BlockGuard::SetThreadPolicy(policy);
}

CARAPI CBlockGuard::GetLaxPolicy(
    /* [out] */ IBlockGuardPolicy** policy)
{
    VALIDATE_NOT_NULL(policy);

    return BlockGuard::GetLaxPolicy(policy);
}

} // namespace Core
} // namespace Elastos
