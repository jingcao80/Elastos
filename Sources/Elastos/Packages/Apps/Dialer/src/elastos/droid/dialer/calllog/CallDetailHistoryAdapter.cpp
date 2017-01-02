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

#include "elastos/droid/dialer/calllog/CallDetailHistoryAdapter.h"
#include "elastos/droid/dialer/util/DialerUtils.h"
#include "elastos/droid/contacts/common/CallUtil.h"
#include "elastos/droid/text/format/DateUtils.h"
#include "elastos/droid/text/format/Formatter.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/core/CoreUtils.h>
#include "R.h"

using Elastos::Droid::Contacts::Common::CallUtil;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Dialer::Util::DialerUtils;
using Elastos::Droid::Provider::ICalls;
using Elastos::Droid::Text::Format::DateUtils;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Text::Format::Formatter;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

const Int32 CallDetailHistoryAdapter::VIEW_TYPE_HEADER;
const Int32 CallDetailHistoryAdapter::VIEW_TYPE_HISTORY_ITEM;

CallDetailHistoryAdapter::CallDetailHistoryAdapter(
    /* [in] */ IContext* context,
    /* [in] */ ILayoutInflater* layoutInflater,
    /* [in] */ CallTypeHelper* callTypeHelper,
    /* [in] */ IArrayOf* phoneCallDetails)
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
    VALIDATE_NOT_NULL(enabled)
    // None of history will be clickable.
    *enabled = FALSE;
    return NOERROR;
}

ECode CallDetailHistoryAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    Int32 len;
    mPhoneCallDetails->GetLength(&len);
    *count = len + 1;
    return NOERROR;
}

ECode CallDetailHistoryAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item)
    if (position == 0) {
        *item = NULL;
        return NOERROR;
    }
    mPhoneCallDetails->Get(position - 1, item);
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
            mLayoutInflater->Inflate(Elastos::Droid::Dialer::R::layout::call_detail_history_header,
                    parent, FALSE, (IView**)&header);
            *bkView = header;
        }
        else {
            *bkView = convertView;
        }
        REFCOUNT_ADD(*bkView);
        return NOERROR;
    }

    // Make sure we have a valid convertView to start with
    AutoPtr<IView> result;
    if (convertView == NULL) {
        mLayoutInflater->Inflate(Elastos::Droid::Dialer::R::layout::call_detail_history_item,
                parent, FALSE, (IView**)&result);
    }
    else {
        result = convertView;
    }

    AutoPtr<IInterface> temp;
    mPhoneCallDetails->Get(position - 1, (IInterface**)&temp);
    AutoPtr<PhoneCallDetails> details = (PhoneCallDetails*)(IObject*)temp.Get();
    AutoPtr<IView> view;
    result->FindViewById(Elastos::Droid::Dialer::R::id::call_type_icon, (IView**)&view);
    AutoPtr<ICallTypeIconsView> callTypeIconView = ICallTypeIconsView::Probe(view);

    view = NULL;
    result->FindViewById(Elastos::Droid::Dialer::R::id::call_type_text, (IView**)&view);
    AutoPtr<ITextView> callTypeTextView = ITextView::Probe(view);
    view = NULL;
    result->FindViewById(Elastos::Droid::Dialer::R::id::date, (IView**)&view);
    AutoPtr<ITextView> dateView = ITextView::Probe(view);
    view = NULL;
    result->FindViewById(R::id::duration, (IView**)&view);
    AutoPtr<ITextView> durationView = ITextView::Probe(view);

    Int32 callType = (*details->mCallTypes)[0];
    Boolean isVideoCall;
    isVideoCall = (details->mFeatures & ICalls::FEATURES_VIDEO)
            == ICalls::FEATURES_VIDEO && CallUtil::IsVideoEnabled(mContext);

    callTypeIconView->Clear();
    callTypeIconView->Add(callType);
    callTypeIconView->SetShowVideo(isVideoCall);
    AutoPtr<ICharSequence> text = mCallTypeHelper->GetCallTypeText(callType, isVideoCall);
    callTypeTextView->SetText(text);
    // Set the date.
    String dateValue = DateUtils::FormatDateRange(mContext,
            details->mDate, details->mDate,
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
                details->mDuration, details->mDataUsage));
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
    args->Set(0, CoreUtils::Convert(minutes));
    args->Set(1, CoreUtils::Convert(seconds));
    String str;
    mContext->GetString(Elastos::Droid::Dialer::R::string::callDetailsDurationFormat, args, &str);

    return CoreUtils::Convert(str);
}

AutoPtr<ICharSequence> CallDetailHistoryAdapter::FormatDurationAndDataUsage(
    /* [in] */ Int64 elapsedSeconds,
    /* [in] */ Int64 dataUsage)
{
    AutoPtr<ICharSequence> duration = FormatDuration(elapsedSeconds);

    // if (dataUsage != NULL) {
    mDurationItems->Clear();
    mDurationItems->Add(duration);
    mDurationItems->Add(CoreUtils::Convert(
            Formatter::FormatShortFileSize(mContext, dataUsage)));

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    return DialerUtils::Join(res, IIterable::Probe(mDurationItems));
    // }
    // else {
    //     return duration;
    // }
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
