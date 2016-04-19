
#include "CallDetailHistoryAdapter.h"
#include "elastos/droid/text/format/DateUtils.h"
#include "elastos/droid/text/format/Formatter.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Text::Format::DateUtils;
using Elastos::Droid::Text::Format::Formatter;

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace CallLog {

const CallDetailHistoryAdapter::Int32 VIEW_TYPE_HEADER = 0;
const CallDetailHistoryAdapter::Int32 VIEW_TYPE_HISTORY_ITEM = 1;

CallDetailHistoryAdapter::CallDetailHistoryAdapter(
    /* [in] */ IContext* context,
    /* [in] */ ILayoutInflater* layoutInflater,
    /* [in] */ ICallTypeHelper* callTypeHelper,
    /* [in] */ ArrayOf<IPhoneCallDetails>* phoneCallDetails)
    : mContext(context)
    , mLayoutInflater(layoutInflater)
    , mCallTypeHelper(callTypeHelper)
    , mPhoneCallDetails(phoneCallDetails)
{
    CArrayList::New((IArrayList**)&mDurationItems);
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

    Int32 callType = ((CPhoneCallDetails*)details)->mCallTypes[0];
    Boolean isVideoCall = (((CPhoneCallDetails*)details)->features & ICalls::FEATURES_VIDEO)
            == ICalls::FEATURES_VIDEO && CallUtil::IsVideoEnabled(mContext);

    callTypeIconView->Clear();
    callTypeIconView->Add(callType);
    callTypeIconView->SetShowVideo(isVideoCall);
    callTypeTextView->SetText(mCallTypeHelper->GetCallTypeText(callType, isVideoCall));
    // Set the date.
    String dateValue = DateUtils::FormatDateRange(mContext,
            ((CPhoneCallDetails*)details)->date, ((CPhoneCallDetails*)details)->date,
            IDateUtils::FORMAT_SHOW_TIME | IDateUtils::FORMAT_SHOW_DATE |
            IDateUtils::FORMAT_SHOW_WEEKDAY | IDateUtils::FORMAT_SHOW_YEAR);
    dateView->SetText(dateValue);
    // Set the duration
    if (ICalls::VOICEMAIL_TYPE == callType || CallTypeHelper::IsMissedCallType(callType)) {
        durationView->SetVisibility(IView::GONE);
    }
    else {
        durationView->SetVisibility(IView::VISIBLE);
        durationView->SetText(FormatDurationAndDataUsage(
                ((CPhoneCallDetails*)details)->duration,
                ((CPhoneCallDetails*)details)->dataUsage));
    }

    *bkView = result;
    REFCOUNT_ADD(*bkView);
    return NOERROR;
}

String CallDetailHistoryAdapter::FormatDuration(
    /* [in] */ Int64 elapsedSeconds)
{
    Int64 minutes = 0;
    Int64 seconds = 0;

    if (elapsedSeconds >= 60) {
        minutes = elapsedSeconds / 60;
        elapsedSeconds -= minutes * 60;
    }
    seconds = elapsedSeconds;

    ArrayOf<IInterface> args = ArrayOf<IInterface>::Alloc(2);
    args->Set(0, CoreUtils::Convert(minutes));
    args->Set(1, CoreUtils::Convert(seconds));
    String str;
    mContext->GetString(R::string::callDetailsDurationFormat, args, &str);

    return str;
}

String CallDetailHistoryAdapter::FormatDurationAndDataUsage(
    /* [in] */ Int64 elapsedSeconds,
    /* [in] */ Int64 dataUsage)
{
    String duration = FormatDuration(elapsedSeconds);

    if (!dataUsage.IsNull()) {
        mDurationItems->Clear();
        mDurationItems->Add(duration);
        mDurationItems->Add(Formatter::FormatShortFileSize(mContext, dataUsage));

        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        return DialerUtils::Join(, mDurationItems);
    }
    else {
        return duration;
    }
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
