
#ifndef __ELASTOS_DROID_SYSTEMUI_QS_CQSPANEL_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_CQSPANEL_H__

#include "_Elastos_Droid_SystemUI_Qs_CQSPanel.h"
#include "elastos/droid/systemui/qs/QSDetailClipper.h"
#include "elastos/droid/systemui/qs/QSFooter.h"
#include "elastos/droid/systemui/qs/QSTile.h"
#include "elastos/droid/systemui/settings/BrightnessController.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::SystemUI::Settings::BrightnessController;
using Elastos::Droid::SystemUI::StatusBar::Phone::IPhoneQSTileHost;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Core::Object;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

/** View that represents the quick settings tile panel. **/
CarClass(CQSPanel)
    , public ViewGroup
    , public IQSPanel
{
private:
    class H: public Handler
    {
    public:
        H(
            /* [in] */ CQSPanel* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        static const Int32 SHOW_DETAIL;
        static const Int32 SET_TILE_VISIBILITY;

    private:
        CQSPanel* mHost;
    };

    class Record
        : public Object
        , public IQSPanelRecord
    {
    public:
        CAR_INTERFACE_DECL();

        Record();

    public:
        AutoPtr<IView> mDetailView;
        AutoPtr<IQSTileDetailAdapter> mDetailAdapter;
    };

    class TileRecord
        : public Record
        , public IQSPanelTileRecord
    {
    public:
        CAR_INTERFACE_DECL();

        TileRecord();

    public:
        AutoPtr<QSTile> mTile;  /*<?*/
        AutoPtr<IQSTileView> mTileView;
        Int32 mRow;
        Int32 mCol;
        Boolean mScanState;
    };

    class TeardownDetailWhenDone: public AnimatorListenerAdapter
    {
    public:
        TeardownDetailWhenDone(
            /* [in] */ CQSPanel* host);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CQSPanel* mHost;
    };

    class HideGridContentWhenDone: public AnimatorListenerAdapter
    {
    public:
        HideGridContentWhenDone(
            /* [in] */ CQSPanel* host);

        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CQSPanel* mHost;
    };

    class DetailDoneButtonOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        DetailDoneButtonOnClickListener(
            /* [in] */ CQSPanel* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CQSPanel* mHost;
    };

    class QSTileCallback
        : public Object
        , public IQSTileCallback
    {
    public:
        CAR_INTERFACE_DECL();

        QSTileCallback(
            /* [in] */ CQSPanel* host,
            /* [in] */ TileRecord* r);

        // @Override
        CARAPI OnStateChanged(
            /* [in] */ IQSTileState* state);

        // @Override
        CARAPI OnShowDetail(
            /* [in] */ Boolean show);

        // @Override
        CARAPI OnToggleStateChanged(
            /* [in] */ Boolean state);

        // @Override
        CARAPI OnScanStateChanged(
            /* [in] */ Boolean state);

        // @Override
        CARAPI OnAnnouncementRequested(
            /* [in] */ ICharSequence* announcement);

    private:
        CQSPanel* mHost;
        TileRecord* mR;
    };

    class OnClickListener1
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        OnClickListener1(
            /* [in] */ TileRecord* r);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        TileRecord* mR;
    };

    class OnClickListener2
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        OnClickListener2(
            /* [in] */ TileRecord* r);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        TileRecord* mR;
    };

    class OnClickListener3
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        OnClickListener3(
            /* [in] */ CQSPanel* host,
            /* [in] */ IIntent* settingsIntent);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CQSPanel* mLocalHost;
        AutoPtr<IIntent> mSettingsIntent;
    };

public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CQSPanel();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetBrightnessMirror(
        /* [in] */ IBrightnessMirrorController* c);

    CARAPI SetCallback(
        /* [in] */ IQSPanelCallback* callback);

    CARAPI SetHost(
        /* [in] */ IPhoneQSTileHost* host);

    CARAPI GetHost(
        /* [out] */ IPhoneQSTileHost** host);

    CARAPI UpdateResources();

    CARAPI SetExpanded(
        /* [in] */ Boolean expanded);

    CARAPI SetListening(
        /* [in] */ Boolean listening);

    CARAPI ShowDetailAdapter(
        /* [in] */ Boolean show,
        /* [in] */ IQSTileDetailAdapter* adapter);

    CARAPI SetTiles(
        /* [in] */ ICollection/*<QSTile<?>>*/* tiles);

    CARAPI IsShowingDetail(
        /* [out] */ Boolean* result);

    CARAPI CloseDetail();

protected:
    // @Override
    CARAPI_(void) OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    // @Override
    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

private:
    CARAPI_(void) UpdateDetailText();

    CARAPI_(void) RefreshAllTiles();

    CARAPI_(void) ShowDetail(
        /* [in] */ Boolean show,
        /* [in] */ Record* r);

    CARAPI_(void) SetTileVisibility(
        /* [in] */ IView* v,
        /* [in] */ Int32 visibility);

    CARAPI_(void) HandleSetTileVisibility(
        /* [in] */ IView* v,
        /* [in] */ Int32 visibility);

    CARAPI_(void) AddTile(
        /* [in] */ IQSTile/*<?>*/* tile);

    CARAPI_(void) HandleShowDetail(
        /* [in] */ Record* r,
        /* [in] */ Boolean show);

    CARAPI_(void) HandleShowDetailTile(
        /* [in] */ TileRecord* r,
        /* [in] */ Boolean show);

    CARAPI_(void) HandleShowDetailImpl(
        /* [in] */ Record* r,
        /* [in] */ Boolean show,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI_(void) SetGridContentVisibility(
        /* [in] */ Boolean visible);

    static CARAPI_(Int32) Exactly(
        /* [in] */ Int32 size);

    CARAPI_(Int32) GetRowTop(
        /* [in] */ Int32 row);

    CARAPI_(Int32) GetColumnCount(
        /* [in] */ Int32 row);

    CARAPI_(void) FireShowingDetail(
        /* [in] */ IQSTileDetailAdapter* detail);

    CARAPI_(void) FireToggleStateChanged(
        /* [in] */ Boolean state);

    CARAPI_(void) FireScanStateChanged(
        /* [in] */ Boolean state);

    CARAPI_(void) SetDetailRecord(
        /* [in] */ Record* r);

private:
    static const Float TILE_ASPECT;

    AutoPtr<IContext> mContext;
    AutoPtr<IArrayList> mRecords;  /*<TileRecord*/
    AutoPtr<IView> mDetail;
    AutoPtr<IViewGroup> mDetailContent;
    AutoPtr<ITextView> mDetailSettingsButton;
    AutoPtr<ITextView> mDetailDoneButton;
    AutoPtr<IView> mBrightnessView;
    AutoPtr<QSDetailClipper> mClipper;
    AutoPtr<H> mHandler;

    Int32 mColumns;
    Int32 mCellWidth;
    Int32 mCellHeight;
    Int32 mLargeCellWidth;
    Int32 mLargeCellHeight;
    Int32 mPanelPaddingBottom;
    Int32 mDualTileUnderlap;
    Int32 mBrightnessPaddingTop;
    Boolean mExpanded;
    Boolean mListening;

    AutoPtr<Record> mDetailRecord;
    AutoPtr<IQSPanelCallback> mCallback;
    AutoPtr<BrightnessController> mBrightnessController;
    AutoPtr<IPhoneQSTileHost> mHost;

    AutoPtr<QSFooter> mFooter;
    Boolean mGridContentVisible;
    AutoPtr<TeardownDetailWhenDone> mTeardownDetailWhenDone;
    AutoPtr<HideGridContentWhenDone> mHideGridContentWhenDone;
};

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_CQSPANEL_H__
