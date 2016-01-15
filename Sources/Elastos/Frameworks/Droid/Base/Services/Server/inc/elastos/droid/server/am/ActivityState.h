
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
