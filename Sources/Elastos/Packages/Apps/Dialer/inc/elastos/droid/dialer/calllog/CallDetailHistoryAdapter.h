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

#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CALLDETAILHISTORYADAPTER_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CALLDETAILHISTORYADAPTER_H__

#include "elastos/droid/dialer/PhoneCallDetails.h"
#include "elastos/droid/dialer/calllog/CallTypeHelper.h"
#include "elastos/droid/widget/BaseAdapter.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Utility::IArrayList;
using Elastos::Core::IArrayOf;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger64;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

class CallDetailHistoryAdapter
    : public BaseAdapter
{
public:
    CallDetailHistoryAdapter(
        /* [in] */ IContext* context,
        /* [in] */ ILayoutInflater* layoutInflater,
        /* [in] */ CallTypeHelper* callTypeHelper,
        /* [in] */ IArrayOf* phoneCallDetails);

    // @Override
    CARAPI IsEnabled(
        /* [in] */ Int32 position,
        /* [out] */ Boolean* enabled);

    // @Override
    CARAPI GetCount(
        /* [out] */ Int32* count);

    // @Override
    CARAPI GetItem(
        /* [in] */ Int32 position,
        /* [out] */ IInterface** item);

    // @Override
    CARAPI GetItemId(
        /* [in] */ Int32 position,
        /* [out] */ Int64* id);

    // @Override
    CARAPI GetViewTypeCount(
        /* [out] */ Int32* count);

    // @Override
    CARAPI GetItemViewType(
        /* [in] */ Int32 position,
        /* [out] */ Int32* type);

    // @Override
    CARAPI GetView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** bkView);

private:
    CARAPI_(AutoPtr<ICharSequence>) FormatDuration(
        /* [in] */ Int64 elapsedSeconds);

    /**
     * Formats a string containing the call duration and the data usage (if specified).
     *
     * @param elapsedSeconds Total elapsed seconds.
     * @param dataUsage Data usage in bytes, or null if not specified.
     * @return String containing call duration and data usage.
     */
    CARAPI_(AutoPtr<ICharSequence>) FormatDurationAndDataUsage(
        /* [in] */ Int64 elapsedSeconds,
        /* [in] */ Int64 dataUsage);

private:
    /** The top element is a blank header, which is hidden under the rest of the UI. */
    static const Int32 VIEW_TYPE_HEADER = 0;
    /** Each history item shows the detail of a call. */
    static const Int32 VIEW_TYPE_HISTORY_ITEM = 1;

    AutoPtr<IContext> mContext;
    AutoPtr<ILayoutInflater> mLayoutInflater;
    AutoPtr<CallTypeHelper> mCallTypeHelper;
    AutoPtr<IArrayOf> mPhoneCallDetails;

    /**
     * List of items to be concatenated together for duration strings.
     */
    AutoPtr<IArrayList> mDurationItems; // = Lists.newArrayList();
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CALLDETAILHISTORYADAPTER_H__
