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

#include "elastos/droid/service/dreams/CSandman.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Dreams {

CAR_SINGLETON_IMPL(CSandman)
CAR_INTERFACE_IMPL(CSandman, Singleton, ISandman)

ECode CSandman::ShouldStartDockApp(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent,
    /* [out] */ Boolean* result)
{
    return Sandman::ShouldStartDockApp(context, intent, result);
}

ECode CSandman::StartDreamByUserRequest(
    /* [in] */ IContext* context)
{
    return Sandman::StartDreamByUserRequest(context);
}

ECode CSandman::StartDreamWhenDockedIfAppropriate(
    /* [in] */ IContext* context)
{
    return Sandman::StartDreamWhenDockedIfAppropriate(context);
}

} // namespace Dreams
} // namespace Service
} // namepsace Droid
} // namespace Elastos
