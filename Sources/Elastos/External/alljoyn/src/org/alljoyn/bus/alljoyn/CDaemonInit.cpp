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

#include "org/alljoyn/bus/alljoyn/CDaemonInit.h"
#include "org/alljoyn/bus/alljoyn/DaemonInit.h"

namespace Org {
namespace Alljoyn {
namespace Bus {
namespace Alljoyn {

CAR_SINGLETON_IMPL(CDaemonInit)

CAR_INTERFACE_IMPL(CDaemonInit, Singleton, IDaemonInit)

ECode CDaemonInit::GetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context);
    AutoPtr<IContext> ctx = DaemonInit::GetContext();
    *context = ctx;
    REFCOUNT_ADD(*context);
    return NOERROR;
}

ECode CDaemonInit::PrepareDaemon(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = DaemonInit::PrepareDaemon(context);
    return NOERROR;
}

} // namespace Alljoyn
} // namespace Bus
} // namespace Alljoyn
} // namespace Org
