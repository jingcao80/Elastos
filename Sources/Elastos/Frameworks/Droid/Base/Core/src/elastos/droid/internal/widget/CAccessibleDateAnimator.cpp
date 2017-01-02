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

#include "elastos/droid/internal/widget/CAccessibleDateAnimator.h"
#include "elastos/droid/text/format/DateUtils.h"
#include "Elastos.Droid.Text.h"

using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::Text::Format::DateUtils;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CAR_OBJECT_IMPL(CAccessibleDateAnimator);
CAR_INTERFACE_IMPL(CAccessibleDateAnimator, ViewAnimator, IAccessibleDateAnimator);
CAccessibleDateAnimator::CAccessibleDateAnimator()
    : mDateMillis(0)
{}

ECode CAccessibleDateAnimator::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ViewAnimator::constructor(context, attrs);
}

ECode CAccessibleDateAnimator::SetDateMillis(
    /* [in] */ Int64 dateMillis)
{
    mDateMillis = dateMillis;
    return NOERROR;
}

ECode CAccessibleDateAnimator::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 type = 0;
    event->GetEventType(&type);
    if (type == IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED) {
        // Clear the event's current text so that only the current date will be spoken.
        AutoPtr<IList> list;
        IAccessibilityRecord::Probe(event)->GetText((IList**)&list);
        list->Clear();
        Int32 flags = IDateUtils::FORMAT_SHOW_DATE | IDateUtils::FORMAT_SHOW_YEAR |
                IDateUtils::FORMAT_SHOW_WEEKDAY;

        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        String dateString = DateUtils::FormatDateTime(context, mDateMillis, flags);
        AutoPtr<ICharSequence> seq;
        CString::New(dateString, (ICharSequence**)&seq);
        list->Add(seq);
        *result = TRUE;
        return NOERROR;
    }
    return ViewAnimator::DispatchPopulateAccessibilityEvent(event, result);
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
