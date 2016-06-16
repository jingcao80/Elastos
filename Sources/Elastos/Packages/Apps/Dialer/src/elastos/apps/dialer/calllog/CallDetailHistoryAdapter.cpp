
#include "elastos/droid/text/format/DateUtils.h"
#include "elastos/droid/text/format/Formatter.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/core/CoreUtils.h>

#include "R.h"
#include "elastos/apps/dialer/calllog/CallDetailHistoryAdapter.h"
#include "elastos/apps/dialer/calllog/CallTypeHelper.h"
#include "elastos/apps/dialer/util/DialerUtils.h"
#include "elastos/apps/dialer/CPhoneCallDetails.h"


using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Provider::ICalls;
using Elastos::Droid::Text::Format::DateUtils;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Text::Format::Formatter;
using Elastos::Droid::Widget::ITextView;
using Elastos::Utility::CArrayList;
using Elastos::Apps::Dialer::Util::DialerUtils;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

const Int32 CallDetailHistoryAdapter::VIEW_TYPE_HEADER = 0;
const Int32 CallDetailHistoryAdapter::VIEW_TYPE_HISTORY_ITEM = 1;

CAR_INTERFACE_IMPL(CallDetailHistoryAdapter, BaseAdapter, ICallDetailHistoryAdapter);

CallDetailHistoryAdapter::CallDetailHistoryAdapter()
{
    CArrayList::New((IArrayList**)&mDurationItems);
}

ECode CallDetailHistoryAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ILayoutInflater* layoutInflater,
    /* [in] */ ICallTypeHelper* callTypeHelper,
    /* [in] */ ArrayOf<IPhoneCallDetails*>* phoneCallDetails)
{
    mContext = context;
    mLayoutInflater = layoutInflater;
    mCallTypeHelper = callTypeHelper;
    mPhoneCallDetails = phoneCallDetails;
    return NOERROR;
}

ECode CallDetailHistoryAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    // None of history will be clickable.
    *enabled = FALSE;
    return NOERROR;
}

ECode CallDetailHistoryAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = mPhoneCallDetails->GetLength() + 1;
    return NOERROR;
}

ECode CallDetailHistoryAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item);
    if (position == 0) {
        *item = NULL;
        return NOERROR;
    }
    *item = (*mPhoneCallDetails)[position - 1];
    REFCOUNT_ADD(*item);
    return NOERROR;
}

ECode CallDetailHistoryAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
     if (position == 0) {
        *id = -1;
        return NOERROR;
    }
    *id = position - 1;
    return NOERROR;
}

ECode CallDetailHistoryAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = 2;
    return NOERROR;
}

ECode CallDetailHistoryAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    if (position == 0) {
        *type = VIEW_TYPE_HEADER;
        return NOERROR;
    }
    *type = VIEW_TYPE_HISTORY_ITEM;
    return NOERROR;
}

// @Override
ECode CallDetailHistoryAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** bkView)
{
    VALIDATE_NOT_NULL(bkView);
    if (position == 0) {
        if (convertView == NULL) {
            AutoPtr<IView> header;
            mLayoutInflater->Inflate(R::layout::call_detail_history_header,
                    parent, FALSE, (IView**)&header);
            *bkView = header;
            REFCOUNT_ADD(*bkView);
        }
        else {
            *bkView = convertView;
            REFCOUNT_ADD(*bkView);
        }

        return NOERROR;
    }

    // Make sure we have a valid convertView to start with
    AutoPtr<IView> result;
    if (convertView == NULL) {
        mLayoutInflater->Inflate(R::layout::call_detail_history_item,
                parent, FALSE, (IView**)&result);
    }
    else {
        result = convertView;
    }

    AutoPtr<IPhoneCallDetails> details = (*mPhoneCallDetails)[position - 1];
    AutoPtr<IView> view;
    result->FindViewById(R::id::call_type_icon, (IView**)&view);
    AutoPtr<ICallTypeIconsView> callTypeIconView =ICallTypeIconsView::Probe(view);

    view = NULL;
    result->FindViewById(R::id::call_type_text, (IView**)&view);
    AutoPtr<ITextView> callTypeTextView = ITextView::Probe(view);
    view = NULL;
    result->FindViewById(R::id::date, (IView**)&view);
    AutoPtr<ITextView> dateView = ITextView::Probe(view);
    view = NULL;
    result->FindViewById(R::id::duration, (IView**)&view);
    AutoPtr<ITextView> durationView = ITextView::Probe(view);

    CPhoneCallDetails* cDetails = (CPhoneCallDetails*)details.Get();
    Int32 callType = (*cDetails->mCallTypes)[0];
    Boolean isVideoCall;
    assert(0 && "TODO");
    // isVideoCall = (cDetails->mFeatures & ICalls::FEATURES_VIDEO)
    //         == ICalls::FEATURES_VIDEO && CallUtil::IsVideoEnabled(mContext);

    callTypeIconView->Clear();
    callTypeIconView->Add(callType);
    callTypeIconView->SetShowVideo(isVideoCall);
    AutoPtr<ICharSequence> text;
    mCallTypeHelper->GetCallTypeText(callType, isVideoCall, (ICharSequence**)&text);
    callTypeTextView->SetText(text);
    // Set the date.
    String dateValue = DateUtils::FormatDateRange(mContext,
            cDetails->mDate, cDetails->mDate,
            IDateUtils::FORMAT_SHOW_TIME | IDateUtils::FORMAT_SHOW_DATE |
            IDateUtils::FORMAT_SHOW_WEEKDAY | IDateUtils::FORMAT_SHOW_YEAR);
    dateView->SetText(CoreUtils::Convert(dateValue));
    // Set the duration
    if (ICalls::VOICEMAIL_TYPE == callType || CallTypeHelper::IsMissedCallType(callType)) {
        IView::Probe(durationView)->SetVisibility(IView::GONE);
    }
    else {
        IView::Probe(durationView)->SetVisibility(IView::VISIBLE);
        durationView->SetText(FormatDurationAndDataUsage(
                cDetails->mDuration, cDetails->mDataUsage));
    }

    *bkView = result;
    REFCOUNT_ADD(*bkView);
    return NOERROR;
}

AutoPtr<ICharSequence> CallDetailHistoryAdapter::FormatDuration(
    /* [in] */ Int64 elapsedSeconds)
{
    Int64 minutes = 0;
    Int64 seconds = 0;

    if (elapsedSeconds >= 60) {
        minutes = elapsedSeconds / 60;
        elapsedSeconds -= minutes * 60;
    }
    seconds = elapsedSeconds;

    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    args->Set(0, CoreUtils::Convert(minutes).Get());
    args->Set(1, CoreUtils::Convert(seconds).Get());
    String str;
    mContext->GetString(R::string::callDetailsDurationFormat, args, &str);

    return CoreUtils::Convert(str);
}

AutoPtr<ICharSequence> CallDetailHistoryAdapter::FormatDurationAndDataUsage(
    /* [in] */ Int64 elapsedSeconds,
    /* [in] */ IInteger64* dataUsage)
{
    AutoPtr<ICharSequence> duration = FormatDuration(elapsedSeconds);

    if (dataUsage != NULL) {
        mDurationItems->Clear();
        mDurationItems->Add(duration);
        Int64 value;
        dataUsage->GetValue(&value);
        mDurationItems->Add(CoreUtils::Convert(
                Formatter::FormatShortFileSize(mContext, value)));

        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        return DialerUtils::Join(res, IIterable::Probe(mDurationItems));
    }
    else {
        return duration;
    }
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
