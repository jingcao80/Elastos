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

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/http/CErrorStrings.h"
#include "elastos/droid/net/http/ErrorStrings.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

CAR_INTERFACE_IMPL(CErrorStrings, Singleton, IErrorStrings)

CAR_SINGLETON_IMPL(CErrorStrings)

ECode CErrorStrings::GetString(
        /* [in] */ Int32 errorCode,
        /* [in] */ IContext* context,
        /* [out] */ String* result)
{
    return ErrorStrings::GetString(errorCode, context, result);
}

ECode CErrorStrings::GetResource(
        /* [in] */ Int32 errorCode,
        /* [out] */ Int32* result)
{
    return ErrorStrings::GetResource(errorCode, result);
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

