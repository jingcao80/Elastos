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

#ifndef __ELASTOS_DROID_SERVER_AM_ACTIVITYSTATE_H__
#define __ELASTOS_DROID_SERVER_AM_ACTIVITYSTATE_H__

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

enum ActivityState
{
    ActivityState_INITIALIZING,
    ActivityState_RESUMED,
    ActivityState_PAUSING,
    ActivityState_PAUSED,
    ActivityState_STOPPING,
    ActivityState_STOPPED,
    ActivityState_FINISHING,
    ActivityState_DESTROYING,
    ActivityState_DESTROYED
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_ACTIVITYSTATE_H__
