
#include "elastos/droid/systemui/qs/CQSDetailItems.h"
#include "elastos/droid/systemui/FontSizeUtils.h"
#include "../R.h"
#include <elastos/droid/R.h>
#include <elastos/droid/os/Looper.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/droid/view/LayoutInflater.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::Looper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

CAR_INTERFACE_IMPL(CQSDetailItems::Item, Object, IQSDetailItemsItem);
CQSDetailItems::Item::Item()
    : mIcon(0)
    , mCanDisconnect(FALSE)
{}

const Int32 CQSDetailItems::H::SET_ITEMS;
const Int32 CQSDetailItems::H::SET_CALLBACK;
const Int32 CQSDetailItems::H::SET_ITEMS_VISIBLE;
CQSDetailItems::H::H(
    /* [in] */ CQSDetailItems* host)
    : mHost(host)
{
    Handler::constructor(Looper::GetMainLooper());
}

ECode CQSDetailItems::H::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    if (what == SET_ITEMS) {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        mHost->HandleSetItems(IArrayList::Probe(obj));
    }
    else if (what == SET_CALLBACK) {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        mHost->HandleSetCallback(IQSDetailItemsCallback::Probe(obj));
    }
    else if (what == SET_ITEMS_VISIBLE) {
        Int32 arg1 = 0;
        msg->GetArg1(&arg1);
        mHost->HandleSetItemsVisible(arg1 != 0);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CQSDetailItems::OnClickListener1, Object, IViewOnClickListener);
CQSDetailItems::OnClickListener1::OnClickListener1(
    /* [in] */ CQSDetailItems* host,
    /* [in] */ IQSDetailItemsItem* item)
    : mHost(host)
    , mItem(item)
{}

ECode CQSDetailItems::OnClickListener1::OnClick(
    /* [in] */ IView* v)
{
    if (mHost->mCallback != NULL) {
        mHost->mCallback->OnDetailItemClick(mItem);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CQSDetailItems::OnClickListener2, Object, IViewOnClickListener);
CQSDetailItems::OnClickListener2::OnClickListener2(
    /* [in] */ CQSDetailItems* host,
    /* [in] */ IQSDetailItemsItem* item)
    : mHost(host)
    , mItem(item)
{}

ECode CQSDetailItems::OnClickListener2::OnClick(
    /* [in] */ IView* v)
{
    if (mHost->mCallback != NULL) {
        mHost->mCallback->OnDetailItemDisconnect(mItem);
    }
    return NOERROR;
}


const String CQSDetailItems::TAG("QSDetailItems");
const Boolean CQSDetailItems::DEBUG = Logger::IsLoggable(TAG, Logger::___DEBUG);
CAR_OBJECT_IMPL(CQSDetailItems);
CAR_INTERFACE_IMPL(CQSDetailItems, FrameLayout, IQSDetailItems);
CQSDetailItems::CQSDetailItems()
    : mItemsVisible(TRUE)
{
    mHandler = new H(this);
}

ECode CQSDetailItems::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FrameLayout::constructor(context, attrs);
    mContext = context;
    mTag = TAG;
    return NOERROR;
}

AutoPtr<IQSDetailItems> CQSDetailItems::ConvertOrInflate(
    /* [in] */ IContext* context,
    /* [in] */ IView* convert,
    /* [in] */ IViewGroup* parent)
{
    if (IQSDetailItems::Probe(convert)) {
        return IQSDetailItems::Probe(convert);
    }

    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(context, (ILayoutInflater**)&inflater);
    AutoPtr<IView> view;
    inflater->Inflate(R::layout::qs_detail_items, parent, FALSE, (IView**)&view);
    return IQSDetailItems::Probe(view);
}

ECode CQSDetailItems::OnFinishInflate()
{
    FrameLayout::OnFinishInflate();
    AutoPtr<IView> view;
    FindViewById(Elastos::Droid::R::id::list, (IView**)&view);
    mItems = ILinearLayout::Probe(view);
    IView::Probe(mItems)->SetVisibility(IView::GONE);
    FindViewById(Elastos::Droid::R::id::empty, (IView**)&mEmpty);
    mEmpty->SetVisibility(IView::GONE);
    view = NULL;
    mEmpty->FindViewById(Elastos::Droid::R::id::title, (IView**)&view);
    mEmptyText = ITextView::Probe(view);
    view = NULL;
    mEmpty->FindViewById(Elastos::Droid::R::id::icon, (IView**)&view);
    mEmptyIcon = IImageView::Probe(view);
    return NOERROR;
}

void CQSDetailItems::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    FrameLayout::OnConfigurationChanged(newConfig);
    FontSizeUtils::UpdateFontSize(mEmptyText, R::dimen::qs_detail_empty_text_size);
    Int32 count = 0;
    IViewGroup::Probe(mItems)->GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> item;
        IViewGroup::Probe(mItems)->GetChildAt(i, (IView**)&item);
        FontSizeUtils::UpdateFontSize(item, Elastos::Droid::R::id::title,
                R::dimen::qs_detail_item_primary_text_size);
        FontSizeUtils::UpdateFontSize(item, Elastos::Droid::R::id::summary,
                R::dimen::qs_detail_item_secondary_text_size);
    }
}

ECode CQSDetailItems::SetTagSuffix(
    /* [in] */ const String& suffix)
{
    mTag = TAG + "." + suffix;
    return NOERROR;
}

ECode CQSDetailItems::SetEmptyState(
    /* [in] */ Int32 icon,
    /* [in] */ Int32 text)
{
    mEmptyIcon->SetImageResource(icon);
    mEmptyText->SetText(text);
    return NOERROR;
}

ECode CQSDetailItems::OnAttachedToWindow()
{
    FrameLayout::OnAttachedToWindow();
    if (DEBUG) Logger::D(mTag, "onAttachedToWindow");
    return NOERROR;
}

ECode CQSDetailItems::OnDetachedFromWindow()
{
    FrameLayout::OnDetachedFromWindow();
    if (DEBUG) Logger::D(mTag, "onDetachedFromWindow");
    mCallback = NULL;
    return NOERROR;
}

ECode CQSDetailItems::SetCallback(
    /* [in] */ IQSDetailItemsCallback* callback)
{
    mHandler->RemoveMessages(H::SET_CALLBACK);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(H::SET_CALLBACK, callback, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CQSDetailItems::SetItems(
    /* [in] */ IArrayList* items)
{
    mHandler->RemoveMessages(H::SET_ITEMS);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(H::SET_ITEMS, items, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CQSDetailItems::SetItemsVisible(
    /* [in] */ Boolean visible)
{
    mHandler->RemoveMessages(H::SET_ITEMS_VISIBLE);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(H::SET_ITEMS_VISIBLE, visible ? 1 : 0, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

void CQSDetailItems::HandleSetCallback(
    /* [in] */ IQSDetailItemsCallback* callback)
{
    mCallback = callback;
}

void CQSDetailItems::HandleSetItems(
    /* [in] */ IArrayList* items)
{
    Int32 length = 0;
    Int32 itemCount = items != NULL ? (items->GetSize(&length), length) : 0;
    mEmpty->SetVisibility(itemCount == 0 ? IView::VISIBLE : IView::GONE);
    IView::Probe(mItems)->SetVisibility(itemCount == 0 ? IView::GONE : IView::VISIBLE);
    Int32 count = 0;
    IViewGroup::Probe(mItems)->GetChildCount(&count);
    for (Int32 i = count - 1; i >= itemCount; i--) {
        IViewGroup::Probe(mItems)->RemoveViewAt(i);
    }
    for (Int32 i = 0; i < itemCount; i++) {
        AutoPtr<IInterface> obj;
        items->Get(i, (IInterface**)&obj);
        AutoPtr<IView> view;
        IViewGroup::Probe(mItems)->GetChildAt(i, (IView**)&view);
        Bind(IQSDetailItemsItem::Probe(obj), view);
    }
}

void CQSDetailItems::HandleSetItemsVisible(
    /* [in] */ Boolean visible)
{
    if (mItemsVisible == visible) return;
    mItemsVisible = visible;
    Int32 count = 0;
    IViewGroup::Probe(mItems)->GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> view;
        IViewGroup::Probe(mItems)->GetChildAt(i, (IView**)&view);
        view->SetVisibility(mItemsVisible ? IView::VISIBLE : IView::INVISIBLE);
    }
}

void CQSDetailItems::Bind(
    /* [in] */ IQSDetailItemsItem* _item,
    /* [in] */ IView* _view)
{
    Item* item = (Item*)_item;
    AutoPtr<IView> view = _view;
    if (view == NULL) {
        AutoPtr<ILayoutInflater> inflater;
        LayoutInflater::From(mContext, (ILayoutInflater**)&inflater);
        inflater->Inflate(R::layout::qs_detail_item, this, FALSE, (IView**)&view);
        IViewGroup::Probe(mItems)->AddView(view);
    }
    view->SetVisibility(mItemsVisible ? IView::VISIBLE : IView::INVISIBLE);
    AutoPtr<IView> v;
    view->FindViewById(Elastos::Droid::R::id::icon, (IView**)&v);
    AutoPtr<IImageView> iv = IImageView::Probe(v);
    iv->SetImageResource(item->mIcon);
    v = NULL;
    view->FindViewById(Elastos::Droid::R::id::title, (IView**)&v);
    AutoPtr<ITextView> title = ITextView::Probe(v);

    AutoPtr<ICharSequence> t;
    CString::New(item->mLine1, (ICharSequence**)&t);
    title->SetText(t);
    v = NULL;
    view->FindViewById(Elastos::Droid::R::id::summary, (IView**)&v);
    AutoPtr<ITextView> summary = ITextView::Probe(v);
    const Boolean twoLines = !TextUtils::IsEmpty(item->mLine2);
    IView::Probe(summary)->SetVisibility(twoLines ? IView::VISIBLE : IView::GONE);

    t = NULL;
    if (twoLines) {
        CString::New(item->mLine2, (ICharSequence**)&t);
    }
    summary->SetText(t);
    Int32 value = 0;
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetDimensionPixelSize(twoLines ? R::dimen::qs_detail_item_height_twoline
            : R::dimen::qs_detail_item_height, &value);
    view->SetMinimumHeight(value);

    AutoPtr<OnClickListener1> cl1 = new OnClickListener1(this, _item);
    view->SetOnClickListener(cl1);
    v = NULL;
    view->FindViewById(Elastos::Droid::R::id::icon2, (IView**)&v);
    AutoPtr<IImageView> disconnect = IImageView::Probe(v);
    IView::Probe(disconnect)->SetVisibility(item->mCanDisconnect ? IView::VISIBLE : IView::GONE);

    AutoPtr<OnClickListener2> cl2 = new OnClickListener2(this, _item);
    IView::Probe(disconnect)->SetOnClickListener(cl2);
}

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
