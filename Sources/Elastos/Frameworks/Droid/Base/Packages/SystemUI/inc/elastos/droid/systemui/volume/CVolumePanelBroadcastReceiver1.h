#ifndef __ELASTOS_DROID_SYSTEMUI_VOLUME_CVOLUMEPANELBROADCASTRECEIVER1_H__
#define __ELASTOS_DROID_SYSTEMUI_VOLUME_CVOLUMEPANELBROADCASTRECEIVER1_H__

#include "_Elastos_Droid_SystemUI_Volume_CVolumePanelBroadcastReceiver1.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::BroadcastReceiver;

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Volume{

class VolumePanel;

CarClass(CVolumePanelBroadcastReceiver1), public BroadcastReceiver
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor(
        /* [in] */ IVolumePanel* panel);

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    VolumePanel* mHost;
};

} // namespace Volume
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_VOLUME_CVOLUMEPANELBROADCASTRECEIVER1_H__
