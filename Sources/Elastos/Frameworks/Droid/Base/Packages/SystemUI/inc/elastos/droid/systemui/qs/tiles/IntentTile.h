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
        TO_STRING_IMPL("IntentTile.Receiver")

        Receiver(
            /* [in] */ IntentTile* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

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
