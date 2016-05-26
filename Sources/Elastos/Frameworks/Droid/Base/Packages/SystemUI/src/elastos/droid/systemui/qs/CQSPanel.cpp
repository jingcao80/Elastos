
#include "elastos/droid/systemui/qs/CQSPanel.h"
#include "elastos/droid/systemui/settings/CToggleSlider.h"
#include "elastos/droid/systemui/FontSizeUtils.h"
#include "../R.h"
#include <elastos/droid/R.h>
#include <elastos/droid/view/LayoutInflater.h>
#include <elastos/core/Math.h>

using Elastos::Droid::SystemUI::Settings::IToggleSlider;
using Elastos::Droid::SystemUI::Settings::CToggleSlider;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

const Int32 CQSPanel::H::SHOW_DETAIL = 1;
const Int32 CQSPanel::H::SET_TILE_VISIBILITY = 2;
CQSPanel::H::H(
    /* [in] */ CQSPanel* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CQSPanel::H::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    if (what == SHOW_DETAIL) {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        Int32 arg1 = 0;
        msg->GetArg1(&arg1);
        mHost->HandleShowDetail((Record*)IQSPanelRecord::Probe(obj), arg1 != 0);
    }
    else if (what == SET_TILE_VISIBILITY) {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        Int32 arg1 = 0;
        msg->GetArg1(&arg1);
        mHost->HandleSetTileVisibility(IView::Probe(obj), arg1);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CQSPanel::Record, Object, IQSPanelRecord)
CQSPanel::Record::Record()
{}

CAR_INTERFACE_IMPL(CQSPanel::TileRecord, Object, IQSPanelTileRecord)
CQSPanel::TileRecord::TileRecord()
    : mRow(0)
    , mCol(0)
    , mScanState(FALSE)
{}

CQSPanel::TeardownDetailWhenDone::TeardownDetailWhenDone(
    /* [in] */ CQSPanel* host)
    : mHost(host)
{}

ECode CQSPanel::TeardownDetailWhenDone::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mDetailContent->RemoveAllViews();
    mHost->SetDetailRecord(NULL);
    return NOERROR;
}

CQSPanel::HideGridContentWhenDone::HideGridContentWhenDone(
    /* [in] */ CQSPanel* host)
    : mHost(host)
{}

ECode CQSPanel::HideGridContentWhenDone::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    // If we have been cancelled, remove the listener so that onAnimationEnd doesn't get
    // called, this will avoid accidentally turning off the grid when we don't want to.
    animation->RemoveListener(this);
    return NOERROR;
}

ECode CQSPanel::HideGridContentWhenDone::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->SetGridContentVisibility(FALSE);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CQSPanel::DetailDoneButtonOnClickListener, Object, IViewOnClickListener)
CQSPanel::DetailDoneButtonOnClickListener::DetailDoneButtonOnClickListener(
    /* [in] */ CQSPanel* host)
    : mHost(host)
{}

ECode CQSPanel::DetailDoneButtonOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    mHost->CloseDetail();
    return NOERROR;
}

CAR_INTERFACE_IMPL(CQSPanel::QSTileCallback, Object, IQSTileCallback)
CQSPanel::QSTileCallback::QSTileCallback(
    /* [in] */ CQSPanel* host,
    /* [in] */ TileRecord* r)
    : mHost(host)
    , mR(r)
{}

ECode CQSPanel::QSTileCallback::OnStateChanged(
    /* [in] */ IQSTileState* state)
{
    Boolean visible = ((QSTile::State*)state)->mVisible;
    Int32 visibility = visible ? IView::VISIBLE : IView::GONE;
    if (visible && !mHost->mGridContentVisible) {

        // We don't want to show it if the content is hidden,
        // then we just set it to invisible, to ensure that it gets visible again
        visibility = INVISIBLE;
    }
    mHost->SetTileVisibility(IView::Probe(mR->mTileView), visibility);
    mR->mTileView->OnStateChanged(state);
    return NOERROR;
}

ECode CQSPanel::QSTileCallback::OnShowDetail(
    /* [in] */ Boolean show)
{
    mHost->ShowDetail(show, mR);
    return NOERROR;
}

ECode CQSPanel::QSTileCallback::OnToggleStateChanged(
    /* [in] */ Boolean state)
{
    if (mHost->mDetailRecord == mR) {
        mHost->FireToggleStateChanged(state);
    }
    return NOERROR;
}

ECode CQSPanel::QSTileCallback::OnScanStateChanged(
    /* [in] */ Boolean state)
{
    mR->mScanState = state;
    if (mHost->mDetailRecord == mR) {
        mHost->FireScanStateChanged(mR->mScanState);
    }
    return NOERROR;
}

ECode CQSPanel::QSTileCallback::OnAnnouncementRequested(
    /* [in] */ ICharSequence* announcement)
{
    mHost->AnnounceForAccessibility(announcement);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CQSPanel::OnClickListener1, Object, IViewOnClickListener)
CQSPanel::OnClickListener1::OnClickListener1(
    /* [in] */ TileRecord* r)
    : mR(r)
{}

ECode CQSPanel::OnClickListener1::OnClick(
    /* [in] */ IView* v)
{
    mR->mTile->Click();
    return NOERROR;
}

CAR_INTERFACE_IMPL(CQSPanel::OnClickListener2, Object, IViewOnClickListener)
CQSPanel::OnClickListener2::OnClickListener2(
    /* [in] */ TileRecord* r)
    : mR(r)
{}

ECode CQSPanel::OnClickListener2::OnClick(
    /* [in] */ IView* v)
{
    mR->mTile->SecondaryClick();
    return NOERROR;
}

CAR_INTERFACE_IMPL(CQSPanel::OnClickListener3, Object, IViewOnClickListener)
CQSPanel::OnClickListener3::OnClickListener3(
    /* [in] */ CQSPanel* host,
    /* [in] */ IIntent* settingsIntent)
    : mLocalHost(host)
    , mSettingsIntent(settingsIntent)
{}

ECode CQSPanel::OnClickListener3::OnClick(
    /* [in] */ IView* v)
{
    IQSTileHost::Probe(mLocalHost->mHost)->StartSettingsActivity(mSettingsIntent);
    return NOERROR;
}

const Float CQSPanel::TILE_ASPECT = 1.2f;

CAR_OBJECT_IMPL(CQSPanel)

CAR_INTERFACE_IMPL(CQSPanel, ViewGroup, IQSPanel)

CQSPanel::CQSPanel()
    : mColumns(0)
    , mCellWidth(0)
    , mCellHeight(0)
    , mLargeCellWidth(0)
    , mLargeCellHeight(0)
    , mPanelPaddingBottom(0)
    , mDualTileUnderlap(0)
    , mBrightnessPaddingTop(0)
    , mExpanded(FALSE)
    , mListening(FALSE)
    , mGridContentVisible(TRUE)
{
}

ECode CQSPanel::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CQSPanel::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    ViewGroup::constructor(context, attrs);
    mContext = context;

    CArrayList::New((IArrayList**)&mRecords);
    mHandler = new H(this);
    mTeardownDetailWhenDone = new TeardownDetailWhenDone(this);
    mHideGridContentWhenDone = new HideGridContentWhenDone(this);

    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(context, (ILayoutInflater**)&inflater);
    inflater->Inflate(R::layout::qs_detail, this, FALSE, (IView**)&mDetail);

    AutoPtr<IView> view;
    mDetail->FindViewById(Elastos::Droid::R::id::content, (IView**)&view);
    mDetailContent = IViewGroup::Probe(view);
    view = NULL;
    mDetail->FindViewById(Elastos::Droid::R::id::button2, (IView**)&view);
    mDetailSettingsButton = ITextView::Probe(view);
    view = NULL;
    mDetail->FindViewById(Elastos::Droid::R::id::button1, (IView**)&view);
    mDetailDoneButton = ITextView::Probe(view);
    UpdateDetailText();
    mDetail->SetVisibility(GONE);
    mDetail->SetClickable(TRUE);
    inflater->Inflate(R::layout::quick_settings_brightness_dialog, this, FALSE, (IView**)&mBrightnessView);
    mFooter = new QSFooter(this, context);
    AddView(mDetail);
    AddView(mBrightnessView);
    AddView(mFooter->GetView());
    mClipper = new QSDetailClipper(mDetail);
    UpdateResources();

    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    view = NULL;
    FindViewById(R::id::brightness_icon, (IView**)&view);
    AutoPtr<IView> v;
    FindViewById(R::id::brightness_slider, (IView**)&v);
    mBrightnessController = new BrightnessController(ctx, IImageView::Probe(view),
        IToggleSlider::Probe(v));

    AutoPtr<DetailDoneButtonOnClickListener> cl = new DetailDoneButtonOnClickListener(this);
    IView::Probe(mDetailDoneButton)->SetOnClickListener(cl);
    return NOERROR;
}

void CQSPanel::UpdateDetailText()
{
    mDetailDoneButton->SetText(R::string::quick_settings_done);
    mDetailSettingsButton->SetText(R::string::quick_settings_more_settings);
}

ECode CQSPanel::SetBrightnessMirror(
    /* [in] */ IBrightnessMirrorController* c)
{
    ViewGroup::OnFinishInflate();
    AutoPtr<IView> view;
    FindViewById(R::id::brightness_slider, (IView**)&view);
    AutoPtr<IToggleSlider> brightnessSlider = IToggleSlider::Probe(view);
    AutoPtr<IView> m;
    c->GetMirror((IView**)&m);
    view = NULL;
    m->FindViewById(R::id::brightness_slider, (IView**)&view);
    AutoPtr<IToggleSlider> mirror = IToggleSlider::Probe(view);
    brightnessSlider->SetMirror(mirror);
    brightnessSlider->SetMirrorController(c);
    return NOERROR;
}

ECode CQSPanel::SetCallback(
    /* [in] */ IQSPanelCallback* callback)
{
    mCallback = callback;
    return NOERROR;
}

ECode CQSPanel::SetHost(
    /* [in] */ IPhoneQSTileHost* host)
{
    mHost = host;
    mFooter->SetHost(host);
    return NOERROR;
}

ECode CQSPanel::GetHost(
    /* [out] */ IPhoneQSTileHost** host)
{
    VALIDATE_NOT_NULL(host);
    *host = mHost;
    REFCOUNT_ADD(*host);
    return NOERROR;
}

ECode CQSPanel::UpdateResources()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Int32 v = 0;
    res->GetInteger(R::integer::quick_settings_num_columns, &v);
    const Int32 columns = Elastos::Core::Math::Max(1, v);
    res->GetDimensionPixelSize(R::dimen::qs_tile_height, &mCellHeight);
    mCellWidth = (Int32)(mCellHeight * TILE_ASPECT);
    res->GetDimensionPixelSize(R::dimen::qs_dual_tile_height, &mLargeCellHeight);
    mLargeCellWidth = (Int32)(mLargeCellHeight * TILE_ASPECT);
    res->GetDimensionPixelSize(R::dimen::qs_panel_padding_bottom, &mPanelPaddingBottom);
    res->GetDimensionPixelSize(R::dimen::qs_dual_tile_padding_vertical, &mDualTileUnderlap);
    res->GetDimensionPixelSize(R::dimen::qs_brightness_padding_top, &mBrightnessPaddingTop);
    if (mColumns != columns) {
        mColumns = columns;
        PostInvalidate();
    }
    if (mListening) {
        RefreshAllTiles();
    }
    UpdateDetailText();
    return NOERROR;
}

void CQSPanel::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    ViewGroup::OnConfigurationChanged(newConfig);
    FontSizeUtils::UpdateFontSize(mDetailDoneButton, R::dimen::qs_detail_button_text_size);
    FontSizeUtils::UpdateFontSize(mDetailSettingsButton, R::dimen::qs_detail_button_text_size);

    // We need to poke the detail views as well as they might not be attached to the view
    // hierarchy but reused at a later point.
    Int32 count = 0;
    mRecords->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        mRecords->Get(i, (IInterface**)&obj);
        AutoPtr<IView> detailView = ((TileRecord*)IQSPanelTileRecord::Probe(obj))->mDetailView;
        if (detailView != NULL) {
            detailView->DispatchConfigurationChanged(newConfig);
        }
    }
    mFooter->OnConfigurationChanged();
}

ECode CQSPanel::SetExpanded(
    /* [in] */ Boolean expanded)
{
    if (mExpanded == expanded) return NOERROR;
    mExpanded = expanded;
    if (!mExpanded) {
        CloseDetail();
    }
    return NOERROR;
}

ECode CQSPanel::SetListening(
    /* [in] */ Boolean listening)
{
    if (mListening == listening) return NOERROR;
    mListening = listening;

    AutoPtr<IIterator> ator;
    mRecords->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        TileRecord* r = (TileRecord*)IQSPanelTileRecord::Probe(obj);
        r->mTile->SetListening(mListening);
    }
    mFooter->SetListening(mListening);
    if (mListening) {
        RefreshAllTiles();
    }
    if (listening) {
        mBrightnessController->RegisterCallbacks();
    }
    else {
        mBrightnessController->UnregisterCallbacks();
    }
    return NOERROR;
}

void CQSPanel::RefreshAllTiles()
{
    AutoPtr<IIterator> ator;
    mRecords->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        TileRecord* r = (TileRecord*)IQSPanelTileRecord::Probe(obj);
        r->mTile->RefreshState();
    }

    mFooter->RefreshState();
}

ECode CQSPanel::ShowDetailAdapter(
    /* [in] */ Boolean show,
    /* [in] */ IQSTileDetailAdapter* adapter)
{
    AutoPtr<Record> r = new Record();
    r->mDetailAdapter = adapter;
    ShowDetail(show, r);
    return NOERROR;
}

void CQSPanel::ShowDetail(
    /* [in] */ Boolean show,
    /* [in] */ Record* r)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(H::SHOW_DETAIL, show ? 1 : 0, 0, IQSPanelRecord::Probe(r), (IMessage**)&msg);
    msg->SendToTarget();
}

void CQSPanel::SetTileVisibility(
    /* [in] */ IView* v,
    /* [in] */ Int32 visibility)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(H::SET_TILE_VISIBILITY, visibility, 0, v, (IMessage**)&msg);
    msg->SendToTarget();
}

void CQSPanel::HandleSetTileVisibility(
    /* [in] */ IView* v,
    /* [in] */ Int32 visibility)
{
    Int32 value = 0;
    v->GetVisibility(&value);
    if (visibility == value) return;
    v->SetVisibility(visibility);
}

ECode CQSPanel::SetTiles(
    /* [in] */ ICollection/*<QSTile<?>>*/* tiles)
{
    AutoPtr<IIterator> ator;
    mRecords->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        TileRecord* record = (TileRecord*)IQSPanelTileRecord::Probe(obj);
        RemoveView(IView::Probe(record->mTileView));
    }
    mRecords->Clear();

    ator = NULL;
    tiles->GetIterator((IIterator**)&ator);
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> tile;
        ator->GetNext((IInterface**)&tile);
        AddTile(IQSTile::Probe(tile));
    }

    Boolean tmp = FALSE;
    if (IsShowingDetail(&tmp), tmp) {
        mDetail->BringToFront();
    }
    return NOERROR;
}

void CQSPanel::AddTile(
    /* [in] */ IQSTile/*<?>*/* tile)
{
    AutoPtr<TileRecord> r = new TileRecord();
    r->mTile = (QSTile*)tile;
    tile->CreateTileView(mContext, (IQSTileView**)&r->mTileView);
    IView::Probe(r->mTileView)->SetVisibility(IView::GONE);
    AutoPtr<QSTileCallback> callback = new QSTileCallback(this, r);
    r->mTile->SetCallback(callback);
    AutoPtr<IViewOnClickListener> click = new OnClickListener1(r);
    AutoPtr<IViewOnClickListener> clickSecondary = new OnClickListener2(r);
    r->mTileView->Init(click, clickSecondary);
    r->mTile->SetListening(mListening);

    AutoPtr<IQSTileState> s;
    r->mTile->GetState((IQSTileState**)&s);
    callback->OnStateChanged(s);
    r->mTile->RefreshState();
    mRecords->Add(r->Probe(EIID_IQSPanelTileRecord));

    AddView(IView::Probe(r->mTileView));
}

ECode CQSPanel::IsShowingDetail(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDetailRecord != NULL;
    return NOERROR;
}

ECode CQSPanel::CloseDetail()
{
    ShowDetail(FALSE, mDetailRecord);
    return NOERROR;
}

void CQSPanel::HandleShowDetail(
    /* [in] */ Record* r,
    /* [in] */ Boolean show)
{
    if (IQSPanelTileRecord::Probe(r) != NULL) {
        HandleShowDetailTile((TileRecord*)r, show);
    }
    else {
        Int32 v = 0;
        GetWidth(&v);
        HandleShowDetailImpl(r, show, v /* x */, 0/* y */);
    }
}

void CQSPanel::HandleShowDetailTile(
    /* [in] */ TileRecord* r,
    /* [in] */ Boolean show)
{
    if ((mDetailRecord != NULL) == show) return;

    if (show) {
        r->mTile->GetDetailAdapter((IQSTileDetailAdapter**)&r->mDetailAdapter);
        if (r->mDetailAdapter == NULL) return;
    }
    Int32 v1 = 0, v2 = 0;
    Int32 x = (IView::Probe(r->mTileView)->GetLeft(&v1), v1) + (IView::Probe(r->mTileView)->GetWidth(&v2), v2) / 2;
    Int32 y = (IView::Probe(r->mTileView)->GetTop(&v1), v1) + (IView::Probe(r->mTileView)->GetHeight(&v2), v2) / 2;
    HandleShowDetailImpl(r, show, x, y);
}

void CQSPanel::HandleShowDetailImpl(
    /* [in] */ Record* r,
    /* [in] */ Boolean show,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    if ((mDetailRecord != NULL) == show) return;  // already in right state
    AutoPtr<IQSTileDetailAdapter> detailAdapter;
    AutoPtr<IAnimatorListener> listener;
    if (show) {
        detailAdapter = r->mDetailAdapter;
        detailAdapter->CreateDetailView(mContext, r->mDetailView, mDetailContent, (IView**)&r->mDetailView);
        if (r->mDetailView == NULL) {
            // throw new IllegalStateException("Must return detail view");
            assert(0 && "Must return detail view");
        }

        AutoPtr<IIntent> settingsIntent;
        detailAdapter->GetSettingsIntent((IIntent**)&settingsIntent);
        IView::Probe(mDetailSettingsButton)->SetVisibility(settingsIntent != NULL ? IView::VISIBLE : IView::GONE);

        AutoPtr<OnClickListener3> cl3 = new OnClickListener3(this, settingsIntent);
        IView::Probe(mDetailSettingsButton)->SetOnClickListener(cl3);

        mDetailContent->RemoveAllViews();
        mDetail->BringToFront();
        mDetailContent->AddView(r->mDetailView);
        SetDetailRecord(r);
        listener = mHideGridContentWhenDone;
    }
    else {
        SetGridContentVisibility(TRUE);
        listener = mTeardownDetailWhenDone;
        FireScanStateChanged(FALSE);
    }
    SendAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED);
    FireShowingDetail(show ? detailAdapter : NULL);
    mClipper->AnimateCircularClip(x, y, show, listener);
}

void CQSPanel::SetGridContentVisibility(
    /* [in] */ Boolean visible)
{
    Int32 newVis = visible ? IView::VISIBLE : IView::INVISIBLE;
    Int32 size = 0;
    mRecords->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mRecords->Get(i, (IInterface**)&obj);
        TileRecord* tileRecord = (TileRecord*)IQSPanelTileRecord::Probe(obj);
        Int32 v = 0;
        if ((IView::Probe(tileRecord->mTileView)->GetVisibility(&v), v) != IView::GONE) {
            IView::Probe(tileRecord->mTileView)->SetVisibility(newVis);
        }
    }
    mBrightnessView->SetVisibility(newVis);
    mGridContentVisible = visible;
}

void CQSPanel::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    const Int32 width = MeasureSpec::GetSize(widthMeasureSpec);
    mBrightnessView->Measure(Exactly(width), MeasureSpec::UNSPECIFIED);
    Int32 v = 0;
    const Int32 brightnessHeight = (mBrightnessView->GetMeasuredHeight(&v), v) + mBrightnessPaddingTop;
    mFooter->GetView()->Measure(Exactly(width), MeasureSpec::UNSPECIFIED);
    Int32 r = -1;
    Int32 c = -1;
    Int32 rows = 0;
    Boolean rowIsDual = FALSE;

    AutoPtr<IIterator> ator;
    mRecords->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE, tmp = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        TileRecord* record = (TileRecord*)IQSPanelTileRecord::Probe(obj);

        if ((IView::Probe(record->mTileView)->GetVisibility(&v), v) == IView::GONE) continue;
        // wrap to next column if we've reached the max # of columns
        // also don't allow dual + single tiles on the same row
        if (r == -1 || c == (mColumns - 1) || rowIsDual != (record->mTile->SupportsDualTargets(&tmp), tmp)) {
            r++;
            c = 0;
            record->mTile->SupportsDualTargets(&rowIsDual);
        }
        else {
            c++;
        }
        record->mRow = r;
        record->mCol = c;
        rows = r + 1;
    }

    ator = NULL;
    mRecords->GetIterator((IIterator**)&ator);
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        TileRecord* record = (TileRecord*)IQSPanelTileRecord::Probe(obj);
        record->mTile->SupportsDualTargets(&tmp);
        record->mTileView->SetDual(tmp);
        if ((IView::Probe(record->mTileView)->GetVisibility(&v), v) == IView::GONE) continue;
        const Int32 cw = record->mRow == 0 ? mLargeCellWidth : mCellWidth;
        const Int32 ch = record->mRow == 0 ? mLargeCellHeight : mCellHeight;
        IView::Probe(record->mTileView)->Measure(Exactly(cw), Exactly(ch));
    }

    Int32 h = rows == 0 ? brightnessHeight : (GetRowTop(rows) + mPanelPaddingBottom);
    if (mFooter->HasFooter()) {
        mFooter->GetView()->GetMeasuredHeight(&v);
        h += v;
    }
    mDetail->Measure(Exactly(width), MeasureSpec::UNSPECIFIED);
    if ((mDetail->GetMeasuredHeight(&v), v) < h) {
        mDetail->Measure(Exactly(width), Exactly(h));
    }
    SetMeasuredDimension(width, Elastos::Core::Math::Max(h, (mDetail->GetMeasuredHeight(&v), v)));
}

Int32 CQSPanel::Exactly(
    /* [in] */ Int32 size)
{
    return MeasureSpec::MakeMeasureSpec(size, MeasureSpec::EXACTLY);
}

ECode CQSPanel::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    Int32 w = 0, v = 0, h = 0;
    GetWidth(&w);
    mBrightnessView->Layout(0, mBrightnessPaddingTop,
            (mBrightnessView->GetMeasuredWidth(&v), v),
            mBrightnessPaddingTop + (mBrightnessView->GetMeasuredHeight(&h), h));
    Boolean isRtl = (GetLayoutDirection(&v), v) == LAYOUT_DIRECTION_RTL;

    AutoPtr<IIterator> ator;
    mRecords->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        TileRecord* record = (TileRecord*)IQSPanelTileRecord::Probe(obj);

        if ((IView::Probe(record->mTileView)->GetVisibility(&v), v) == GONE) continue;
        const Int32 cols = GetColumnCount(record->mRow);
        const Int32 cw = record->mRow == 0 ? mLargeCellWidth : mCellWidth;
        const Int32 extra = (w - cw * cols) / (cols + 1);
        Int32 left = record->mCol * cw + (record->mCol + 1) * extra;
        const Int32 top = GetRowTop(record->mRow);
        Int32 right;
        Int32 tileWith = 0;
        IView::Probe(record->mTileView)->GetMeasuredWidth(&tileWith);
        if (isRtl) {
            right = w - left;
            left = right - tileWith;
        }
        else {
            right = left + tileWith;
        }
        IView::Probe(record->mTileView)->Layout(left, top, right, top + (IView::Probe(record->mTileView)->GetMeasuredHeight(&v), v));
    }

    mDetail->GetMeasuredHeight(&v);
    GetMeasuredHeight(&h);
    const Int32 dh = Elastos::Core::Math::Max(v, h);
    mDetail->Layout(0, 0, (mDetail->GetMeasuredWidth(&v), v), dh);
    if (mFooter->HasFooter()) {
        AutoPtr<IView> footer = mFooter->GetView();

        Int32 fh = 0, fw = 0;
        GetMeasuredHeight(&h);
        footer->Layout(0, h - (footer->GetMeasuredHeight(&fh), fh),
                (footer->GetMeasuredWidth(&fw), fw), h);
    }
    return NOERROR;
}

Int32 CQSPanel::GetRowTop(
    /* [in] */ Int32 row)
{
    Int32 v = 0;
    if (row <= 0) return (mBrightnessView->GetMeasuredHeight(&v), v) + mBrightnessPaddingTop;
    return (mBrightnessView->GetMeasuredHeight(&v), v) + mBrightnessPaddingTop
            + mLargeCellHeight - mDualTileUnderlap + (row - 1) * mCellHeight;
}

Int32 CQSPanel::GetColumnCount(
    /* [in] */ Int32 row)
{
    Int32 cols = 0, v = 0;
    AutoPtr<IIterator> ator;
    mRecords->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        TileRecord* record = (TileRecord*)IQSPanelTileRecord::Probe(obj);
        if ((IView::Probe(record->mTileView)->GetVisibility(&v), v) == GONE) continue;
        if (record->mRow == row) cols++;
    }

    return cols;
}

void CQSPanel::FireShowingDetail(
    /* [in] */ IQSTileDetailAdapter* detail)
{
    if (mCallback != NULL) {
        mCallback->OnShowingDetail(detail);
    }
}

void CQSPanel::FireToggleStateChanged(
    /* [in] */ Boolean state)
{
    if (mCallback != NULL) {
        mCallback->OnToggleStateChanged(state);
    }
}

void CQSPanel::FireScanStateChanged(
    /* [in] */ Boolean state)
{
    if (mCallback != NULL) {
        mCallback->OnScanStateChanged(state);
    }
}

void CQSPanel::SetDetailRecord(
    /* [in] */ Record* r)
{
    if (r == mDetailRecord) return;
    mDetailRecord = r;
    const Boolean scanState = IQSPanelTileRecord::Probe(mDetailRecord) != NULL
            && ((TileRecord*) mDetailRecord.Get())->mScanState;
    FireScanStateChanged(scanState);
}

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
