
#ifndef __ELASTOS_DROID_SYSTEMUI_QS_TILES_COLORINVERSIONTILE_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_TILES_COLORINVERSIONTILE_H__

#include "elastos/droid/systemui/qs/QSTile.h"
#include "elastos/droid/systemui/qs/SecureSetting.h"
#include "elastos/droid/systemui/qs/UsageTracker.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

/** Quick settings tile: Invert colors **/
class ColorInversionTile
    : public QSTile
    , public IColorInversionTile
{
private:
    class ColorSettings: public SecureSetting
    {
    public:
        ColorSettings(
            /* [in] */ IContext* context,
            /* [in] */ IHandler* handler,
            /* [in] */ const String& settingName,
            /* [in] */ ColorInversionTile* host);

    protected:
        // @Override
        CARAPI_(void) HandleValueChanged(
            /* [in] */ Int32 value);

    private:
        ColorInversionTile* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    ColorInversionTile(
        /* [in] */ IQSTileHost* host);

    // @Override
    CARAPI SetListening(
        /* [in] */ Boolean listening);

    CARAPI_(String) GetSimpleName();

protected:
    // @Override
    CARAPI_(void) HandleDestroy();

    // @Override
    CARAPI_(AutoPtr<State>) NewTileState();

    // @Override
    CARAPI_(void) HandleUserSwitch(
        /* [in] */ Int32 newUserId);

    // @Override
    CARAPI_(void) HandleClick();

    // @Override
    CARAPI_(void) HandleUpdateState(
        /* [in] */ State* state,
        /* [in] */ IInterface* arg);

    // @Override
    CARAPI_(String) ComposeChangeAnnouncement();

private:
    AutoPtr<SecureSetting> mSetting;
    AutoPtr<UsageTracker> mUsageTracker;

    Boolean mListening;
};

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_TILES_COLORINVERSIONTILE_H__
