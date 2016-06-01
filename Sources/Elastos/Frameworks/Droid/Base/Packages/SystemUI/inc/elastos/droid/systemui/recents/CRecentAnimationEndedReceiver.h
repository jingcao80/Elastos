
#ifndef __ELASTOS_DROID_SYSTEMUI_RECENTS_CRECENT_ANIMATION_ENDED_RECEIVER_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENTS_CRECENT_ANIMATION_ENDED_RECEIVER_H__

#include "_Elastos_Droid_SystemUI_Recents_CRecentAnimationEndedReceiver.h"
#include "elastos/droid/systemui/recents/AlternateRecentsComponent.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {

CarClass(CRecentAnimationEndedReceiver)
    , public AlternateRecentsComponent::RecentAnimationEndedReceiver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_CRECENT_ANIMATION_ENDED_RECEIVER_H__