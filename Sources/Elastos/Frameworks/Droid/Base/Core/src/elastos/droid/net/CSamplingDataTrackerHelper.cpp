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
#include "elastos/droid/net/CSamplingDataTracker.h"
#include "elastos/droid/net/CSamplingDataTrackerHelper.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CSamplingDataTrackerHelper, Singleton, ISamplingDataTrackerHelper)

CAR_SINGLETON_IMPL(CSamplingDataTrackerHelper)

ECode CSamplingDataTrackerHelper::GetSamplingSnapshots(
    /* [in] */ IMap* mapIfaceToSample)
{
    return CSamplingDataTracker::GetSamplingSnapshots(mapIfaceToSample);
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
