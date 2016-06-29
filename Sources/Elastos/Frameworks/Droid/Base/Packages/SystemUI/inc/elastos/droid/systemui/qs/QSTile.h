
#ifndef __ELASTOS_DROID_SYSTEMUI_QS_QSTILE_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_QSTILE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include <elastos/droid/os/Handler.h>
#include <elastos/core/Object.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Objects.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::SystemUI::StatusBar::Policy::IListenable;
using Elastos::Core::Object;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

/**
 * Base quick-settings tile, extend this to create a new tile.
 *
 * State management done on a looper provided by the host.  Tiles should update state in
 * handleUpdateState.  Callbacks affecting state should use refreshState to trigger another
 * state update pass on tile looper.
 */
class QSTile
    : public Object
    , public IQSTile
    , public IListenable
{
public:
    class State
        : public Object
        , public IQSTileState
    {
    public:
        CAR_INTERFACE_DECL()

        State();

        CARAPI CopyTo(
            /* [in] */ IQSTileState* other,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    protected:
        virtual CARAPI_(String) GetClass();

        virtual CARAPI_(AutoPtr<StringBuilder>) ToStringBuilder();

    public:
        Boolean mVisible;
        Int32 mIconId;
        AutoPtr<IDrawable> mIcon;
        String mLabel;
        String mContentDescription;
        String mDualLabelContentDescription;
        Boolean mAutoMirrorDrawable;
    };

    class BooleanState: public State
    {
    public:
        BooleanState();

        // @Override
        CARAPI CopyTo(
            /* [in] */ IQSTileState* other,
            /* [out] */ Boolean* result);

    protected:
        virtual CARAPI_(String) GetClass();

        // @Override
        CARAPI_(AutoPtr<StringBuilder>) ToStringBuilder();

    public:
        Boolean mValue;
    };

    class SignalState: public State
    {
    public:
        SignalState();

        // @Override
        CARAPI CopyTo(
            /* [in] */ IQSTileState* other,
            /* [out] */ Boolean* result);

    protected:
        virtual CARAPI_(String) GetClass();

        // @Override
        CARAPI_(AutoPtr<StringBuilder>) ToStringBuilder();

    public:
        Boolean mEnabled;
        Boolean mConnected;
        Boolean mActivityIn;
        Boolean mActivityOut;
        Int32 mOverlayIconId;
        Boolean mFilter;
        Boolean mIsOverlayIconWide;
    };

protected:
    class H: public Handler
    {
    public:
        H(
            /* [in] */ QSTile* host,
            /* [in] */ ILooper* looper);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        static const Int32 SET_CALLBACK = 1;
        static const Int32 CLICK = 2;
        static const Int32 SECONDARY_CLICK = 3;
        static const Int32 REFRESH_STATE = 4;
        static const Int32 SHOW_DETAIL = 5;
        static const Int32 USER_SWITCH = 6;
        static const Int32 TOGGLE_STATE_CHANGED = 7;
        static const Int32 SCAN_STATE_CHANGED = 8;
        static const Int32 DESTROY = 9;
        QSTile* mQsTile;
    };

public:
    CAR_INTERFACE_DECL()

    QSTile();

    CARAPI constructor(
        /* [in] */ IQSTileHost* host);

    CARAPI SupportsDualTargets(
        /* [out] */ Boolean* result);

    CARAPI GetHost(
        /* [out] */ IQSTileHost** host);

    CARAPI CreateTileView(
        /* [in] */ IContext* context,
        /* [out] */ IQSTileView** view);

    CARAPI GetDetailAdapter(
        /* [out] */ IQSTileDetailAdapter** result);

    // safe to call from any thread
    CARAPI SetCallback(
        /* [in] */ IQSTileCallback* callback);

    CARAPI Click();

    CARAPI SecondaryClick();

    CARAPI ShowDetail(
        /* [in] */ Boolean show);

    CARAPI UserSwitch(
        /* [in] */ Int32 newUserId);

    CARAPI FireToggleStateChanged(
        /* [in] */ Boolean state);

    CARAPI FireScanStateChanged(
        /* [in] */ Boolean state);

    CARAPI Destroy();

    CARAPI GetState(
        /* [out] */ IQSTileState** result);

    virtual CARAPI_(void) RefreshState();

    virtual CARAPI_(String) GetSimpleName() = 0;

protected:
    virtual CARAPI_(AutoPtr<State>) NewTileState() = 0;

    virtual CARAPI_(void) HandleClick() = 0;

    virtual CARAPI_(void) HandleUpdateState(
        /* [in] */ State* state,
        /* [in] */ IInterface* arg) = 0;

    virtual CARAPI_(void) RefreshState(
        /* [in] */ IInterface* arg);

    virtual CARAPI_(void) HandleSecondaryClick();

    virtual CARAPI_(void) HandleRefreshState(
        /* [in] */ IInterface* arg);

    virtual CARAPI_(Boolean) ShouldAnnouncementBeDelayed();

    virtual CARAPI_(String) ComposeChangeAnnouncement();

    virtual CARAPI_(void) HandleUserSwitch(
        /* [in] */ Int32 newUserId);

    virtual CARAPI_(void) HandleDestroy();

private:
    // call only on tile worker looper
    CARAPI_(void) HandleSetCallback(
        /* [in] */ IQSTileCallback* callback);

    CARAPI_(void) HandleStateChanged();

    CARAPI_(void) HandleScanStateChanged(
        /* [in] */ Boolean state);

    CARAPI_(void) HandleShowDetail(
        /* [in] */ Boolean show);

    CARAPI_(void) HandleToggleStateChanged(
        /* [in] */ Boolean state);

protected:
    static const Boolean DEBUG;

    AutoPtr<IQSTileHost> mHost;
    AutoPtr<IContext> mContext;
    AutoPtr<H> mHandler;
    AutoPtr<IHandler> mUiHandler;
    AutoPtr<State> mState;

private:
    AutoPtr<IQSTileCallback> mCallback;
    AutoPtr<State> mTmpState;
    Boolean mAnnounceNextStateChange;
};

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_QSTILE_H__
