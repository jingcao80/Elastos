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

#include "Elastos.Droid.Service.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/settings/notification/ZenModeDowntimeDaysSelection.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "../R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Service::Notification::IZenModeConfig;
using Elastos::Droid::Service::Notification::IZenModeConfigHelper;
using Elastos::Droid::Service::Notification::CZenModeConfigHelper;
using Elastos::Droid::Utility::CSparseBooleanArray;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::CLinearLayout;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Utility::IDate;
using Elastos::Utility::ICalendar;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

//===============================================================================
//                  ZenModeDowntimeDaysSelection::MyOnCheckedChangeListener
//===============================================================================

CAR_INTERFACE_IMPL(ZenModeDowntimeDaysSelection::MyOnCheckedChangeListener, Object, ICompoundButtonOnCheckedChangeListener)

ZenModeDowntimeDaysSelection::MyOnCheckedChangeListener::MyOnCheckedChangeListener(
    /* [in] */ ZenModeDowntimeDaysSelection* host,
    /* [in] */ Int32 day)
    : mHost(host)
    , mDay(day)
{}

ECode ZenModeDowntimeDaysSelection::MyOnCheckedChangeListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    mHost->mDays->Put(mDay, isChecked);
    mHost->OnChanged(mHost->GetMode());
    return NOERROR;
}

//===============================================================================
//                  ZenModeDowntimeDaysSelection
//===============================================================================

static const AutoPtr< ArrayOf<Int32> > InitDAYS()
{
    AutoPtr< ArrayOf<Int32> > args = ArrayOf<Int32>::Alloc(7);
    (*args)[0] = ICalendar::MONDAY;
    (*args)[1] = ICalendar::TUESDAY;
    (*args)[2] = ICalendar::WEDNESDAY;
    (*args)[3] = ICalendar::THURSDAY;
    (*args)[4] = ICalendar::FRIDAY;
    (*args)[5] = ICalendar::SATURDAY;
    (*args)[6] = ICalendar::SUNDAY;

    return args;
}
const AutoPtr< ArrayOf<Int32> > ZenModeDowntimeDaysSelection::DAYS = InitDAYS();

static const AutoPtr<ISimpleDateFormat> InitDAY_FORMAT()
{
    AutoPtr<ISimpleDateFormat> format;
    CSimpleDateFormat::New(String("EEEE"), (ISimpleDateFormat**)&format);
    return format;
}

const AutoPtr<ISimpleDateFormat> ZenModeDowntimeDaysSelection::DAY_FORMAT = InitDAY_FORMAT();

ZenModeDowntimeDaysSelection::ZenModeDowntimeDaysSelection()
{}

ECode ZenModeDowntimeDaysSelection::constructor(
    /* [in] */ IContext* context,
    /* [in] */ const String& mode)
{
    CSparseBooleanArray::New((ISparseBooleanArray**)&mDays);

    ScrollView::constructor(context);
    CLinearLayout::New(mContext, (ILinearLayout**)&mLayout);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    Int32 hPad;
    resources->GetDimensionPixelSize(R::dimen::zen_downtime_margin, &hPad);
    IView::Probe(mLayout)->SetPadding(hPad, 0, hPad, 0);
    AddView(IView::Probe(mLayout));

    AutoPtr<IZenModeConfigHelper> helper;
    CZenModeConfigHelper::AcquireSingleton((IZenModeConfigHelper**)&helper);
    AutoPtr< ArrayOf<Int32> > days;
    helper->TryParseDays(mode, (ArrayOf<Int32>**)&days);
    if (days != NULL) {
        for (Int32 i = 0; i < days->GetLength(); i++) {
            mDays->Put((*days)[i], TRUE);
        }
    }
    mLayout->SetOrientation(ILinearLayout::VERTICAL);

    AutoPtr<ICalendarHelper> cHelper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&cHelper);
    AutoPtr<ICalendar> c;
    cHelper->GetInstance((ICalendar**)&c);
    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(context, (ILayoutInflater**)&inflater);
    for (Int32 i = 0; i < DAYS->GetLength(); i++) {
        Int32 day = (*DAYS)[i];
        AutoPtr<IView> checkBoxTmp;
        inflater->Inflate(R::layout::zen_downtime_day,
                this, FALSE, (IView**)&checkBoxTmp);
        ICheckBox* checkBox = ICheckBox::Probe(checkBoxTmp);
        c->Set(ICalendar::DAY_OF_WEEK, day);
        AutoPtr<IDate> date;
        c->GetTime((IDate**)&date);
        String str;
        Elastos::Text::IDateFormat::Probe(DAY_FORMAT)->Format(date, &str);
        ITextView::Probe(checkBox)->SetText(CoreUtils::Convert(str));
        Boolean isChecked;
        mDays->Get(day, &isChecked);
        ICheckable::Probe(checkBox)->SetChecked(isChecked);
        AutoPtr<MyOnCheckedChangeListener> listener = new MyOnCheckedChangeListener(this, day);
        ICompoundButton::Probe(checkBox)->SetOnCheckedChangeListener(listener);
        IViewGroup::Probe(mLayout)->AddView(checkBoxTmp);
    }
    return NOERROR;
}

String ZenModeDowntimeDaysSelection::GetMode()
{
    AutoPtr<StringBuilder> sb = new StringBuilder(IZenModeConfig::SLEEP_MODE_DAYS_PREFIX);
    Boolean empty = TRUE;
    Int32 size;
    mDays->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        Int32 day;
        mDays->KeyAt(i, &day);
        Boolean res;
        if (mDays->ValueAt(i, &res), !res) continue;
        if (empty) {
            empty = FALSE;
        }
        else {
            sb->AppendChar(',');
        }
        sb->Append(day);
    }
    return empty ? String(NULL) : sb->ToString();
}

ECode ZenModeDowntimeDaysSelection::OnChanged(
    /* [in] */ const String& mode)
{
    // event hook for subclasses
    return NOERROR;
}

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos