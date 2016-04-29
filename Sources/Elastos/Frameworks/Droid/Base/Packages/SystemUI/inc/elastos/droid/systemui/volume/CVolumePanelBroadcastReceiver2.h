#ifndef __ELASTOS_DROID_SYSTEMUI_VOLUME_CVOLUMEPANELBROADCASTRECEIVER2_H__
#define __ELASTOS_DROID_SYSTEMUI_VOLUME_CVOLUMEPANELBROADCASTRECEIVER2_H__

#include "_Elastos_Droid_SystemUI_Volume_CVolumePanelBroadcastReceiver2.h"
#include "elastos/droid/systemui/volume/VolumePanel.h"

using Elastos::Droid::SystemUI::StatusBar::Phone::ISystemUIDialog;

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Volume{

CarClass(CVolumePanelBroadcastReceiver2), public BroadcastReceiver
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor(
        /* [in] */ ISystemUIDialog* dlg);

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    VolumePanel::SafetyWarning* mHost;
};

} // namespace Volume
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_VOLUME_CVOLUMEPANELBROADCASTRECEIVER2_H__
