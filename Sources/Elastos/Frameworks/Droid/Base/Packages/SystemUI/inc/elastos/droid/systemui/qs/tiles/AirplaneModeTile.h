
#ifndef __ELASTOS_DROID_SYSTEMUI_QS_TILES_AIRPLANEMODETILE_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_TILES_AIRPLANEMODETILE_H__

#include "elastos/droid/systemui/qs/GlobalSetting.h"
#include "elastos/droid/systemui/qs/QSTile.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IIntent;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

/** Quick settings tile: Airplane mode **/
class AirplaneModeTile
    : public QSTile
{
private:
    class Setting: public GlobalSetting
    {
    public:
        Setting(
            /* [in] */ IContext* context,
            /* [in] */ IHandler* handler,
            /* [in] */ const String& settingName,
            /* [in] */ AirplaneModeTile* host);

    protected:
        // @Override
        CARAPI_(void) HandleValueChanged(
            /* [in] */ Int32 value);

    private:
        AirplaneModeTile* mHost;
    };

    class Receiver: public BroadcastReceiver
    {
    public:
        Receiver(
            /* [in] */ AirplaneModeTile* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        AirplaneModeTile* mHost;
    };

public:
    AirplaneModeTile(
        /* [in] */ IQSTileHost* host);

    // @Override
    CARAPI_(void) HandleClick();

    CARAPI SetListening(
        /* [in] */ Boolean listening);

    CARAPI_(String) GetSimpleName();

protected:
    // @Override
    CARAPI_(AutoPtr<State>) NewTileState();

    // @Override
    CARAPI_(void) HandleUpdateState(
        /* [in] */ State* state,
        /* [in] */ IInterface* arg);

    // @Override
    CARAPI_(String) ComposeChangeAnnouncement();

private:
    CARAPI_(void) SetEnabled(
        /* [in] */ Boolean enabled);

private:
    AutoPtr<GlobalSetting> mSetting;

    Boolean mListening;
    AutoPtr<IBroadcastReceiver> mReceiver;
};

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_TILES_AIRPLANEMODETILE_H__
