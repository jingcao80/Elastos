
#ifndef __ELASTOS_DROID_SYSTEMUI_QS_TILES_INTENTTILE_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_TILES_INTENTTILE_H__

#include "elastos/droid/systemui/qs/QSTile.h"
#include "Elastos.Droid.App.h"
#include <elastos/droid/content/BroadcastReceiver.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

class IntentTile: public QSTile
{
private:
    class Receiver: public BroadcastReceiver
    {
    public:
        Receiver(
            /* [in] */ IntentTile* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* str)
        {
            *str = String("IntentTile.Receiver");
            return NOERROR;
        }

    private:
        IntentTile* mHost;
    };

public:
    static CARAPI Create(
        /* [in] */ IQSTileHost* host,
        /* [in] */ const String& spec,
        /* [out] */ IQSTile** tile);

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

private:
    IntentTile(
        /* [in] */ IQSTileHost* host,
        /* [in] */ const String& action);

    CARAPI_(AutoPtr<IDrawable>) GetPackageDrawable(
        /* [in] */ const String& pkg,
        /* [in] */ Int32 id);

public:
    static const String PREFIX;

private:
    AutoPtr<IPendingIntent> mOnClick;
    String mOnClickUri;
    Int32 mCurrentUserId;
    AutoPtr<IBroadcastReceiver> mReceiver;
};

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_TILES_INTENTTILE_H__
