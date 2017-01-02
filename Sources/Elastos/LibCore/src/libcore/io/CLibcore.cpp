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

#include "CLibcore.h"
#include "CPosix.h"

namespace Libcore {
namespace IO {

static AutoPtr<IOs> InitsOs()
{
    AutoPtr<IOs> os;
    CPosix::New((IOs**)&os);
    return os;
}

INIT_PROI_1 AutoPtr<IOs> CLibcore::sOs = InitsOs();

CAR_SINGLETON_IMPL(CLibcore)

CAR_INTERFACE_IMPL(CLibcore, Singleton, ILibcore)

ECode CLibcore::GetOs(
    /* [out] */ IOs ** os)
{
    VALIDATE_NOT_NULL(os);

    *os = sOs;
    REFCOUNT_ADD(*os);

    return NOERROR;
}

} // namespace IO
} // namespace Libcore
