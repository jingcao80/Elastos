
#ifndef __ELASTOS_DROID_SYSTEMUI_RECENT_CRECENTS_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENT_CRECENTS_H__

#include "_Elastos_Droid_SystemUI_Recent_CRecents.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/systemui/SystemUI.h"

using Elastos::Droid::App::IActivityOptionsOnAnimationStartedListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::SystemUI::IRecentsComponent;
using Elastos::Droid::SystemUI::IRecentsComponentCallbacks;
using Elastos::Droid::SystemUI::SystemUI;
using Elastos::Droid::SystemUI::Recents::IAlternateRecentsComponent;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

CarClass(CRecents)
    , public SystemUI
    , public IRecentsComponent
{
private:
    class MyOnAnimationStartedListener
        : public Object
        , public IActivityOptionsOnAnimationStartedListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyOnAnimationStartedListener(
            /* [in] */ CRecents* host);

        CARAPI OnAnimationStarted();

    private:
        CRecents* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CRecents();

    // @Override
    CARAPI Start();

    // @Override
    CARAPI ShowRecents(
        /* [in] */ Boolean triggeredFromAltTab,
        /* [in] */ IView* statusBarView);

    // @Override
    CARAPI HideRecents(
        /* [in] */ Boolean triggeredFromAltTab,
        /* [in] */ Boolean triggeredFromHomeKey);

    // @Override
    CARAPI ToggleRecents(
        /* [in] */ IDisplay* display,
        /* [in] */ Int32 layoutDirection,
        /* [in] */ IView* statusBarView);

    // @Override
    CARAPI PreloadRecents();

    // @Override
    CARAPI CancelPreloadingRecents();

    // @Override
    CARAPI ShowNextAffiliatedTask();

    // @Override
    CARAPI ShowPrevAffiliatedTask();

    // @Override
    CARAPI SetCallback(
        /* [in] */ IRecentsComponentCallbacks* cb);

protected:
    // @Override
    CARAPI OnBootCompleted();

    // @Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

private:
    /**
     * Send broadcast only if BOOT_COMPLETED
     */
    CARAPI_(void) SendBroadcastSafely(
        /* [in] */ IIntent* intent);

    /**
     * Start activity only if BOOT_COMPLETED
     */
    CARAPI_(void) StartActivitySafely(
        /* [in] */ IIntent* intent,
        /* [in] */ IBundle* opts);

private:
    const static String TAG;
    const static Boolean DEBUG;

public:
    // Which recents to use
    Boolean mUseAlternateRecents;
    AutoPtr<IAlternateRecentsComponent> mAlternateRecents;
    Boolean mBootCompleted;
};

} // namespace Recent
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_RECENT_CRECENTS_H__
