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

#include "elastos/droid/systemui/statusbar/CNotificationOverflowIconsView.h"
#include "elastos/droid/systemui/statusbar/CStatusBarIconView.h"
#include "R.h"
#include "Elastos.Droid.Service.h"
#include <elastos/droid/R.h>

using Elastos::Droid::Graphics::PorterDuffMode_MULTIPLY;
using Elastos::Droid::Internal::Utility::CNotificationColorUtilHelper;
using Elastos::Droid::Internal::Utility::INotificationColorUtilHelper;
using Elastos::Droid::Service::Notification::IStatusBarNotification;
using Elastos::Droid::Widget::ImageViewScaleType_CENTER_INSIDE;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::IInteger32;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CAR_OBJECT_IMPL(CNotificationOverflowIconsView)

CAR_INTERFACE_IMPL(CNotificationOverflowIconsView, IconMerger, INotificationOverflowIconsView)

CNotificationOverflowIconsView::CNotificationOverflowIconsView()
    : mTintColor(0)
    , mIconSize(0)
{}

ECode CNotificationOverflowIconsView::OnFinishInflate()
{
    IconMerger::OnFinishInflate();
    AutoPtr<INotificationColorUtilHelper> helper;
    CNotificationColorUtilHelper::AcquireSingleton((INotificationColorUtilHelper**)&helper);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    helper->GetInstance(context, (INotificationColorUtil**)&mNotificationColorUtil);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    res->GetColor(R::color::keyguard_overflow_content_color, &mTintColor);
    res->GetDimensionPixelSize(Elastos::Droid::R::dimen::status_bar_icon_size, &mIconSize);
    return NOERROR;
}

ECode CNotificationOverflowIconsView::SetMoreText(
    /* [in] */ ITextView* moreText)
{
    mMoreText = moreText;
    return NOERROR;
}

ECode CNotificationOverflowIconsView::AddNotification(
    /* [in] */ INotificationDataEntry* notification)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);

    AutoPtr<IStatusBarNotification> n;
    notification->GetNotification((IStatusBarNotification**)&n);
    AutoPtr<INotification> value;
    n->GetNotification((INotification**)&value);
    AutoPtr<IStatusBarIconView> v;
    CStatusBarIconView::New(context, String(""), value, (IStatusBarIconView**)&v);
    IImageView::Probe(v)->SetScaleType(ImageViewScaleType_CENTER_INSIDE);
    AddView(IView::Probe(v), mIconSize, mIconSize);

    AutoPtr<IStatusBarIconView> icon;
    notification->GetIcon((IStatusBarIconView**)&icon);
    AutoPtr<IStatusBarIcon> i;
    icon->GetStatusBarIcon((IStatusBarIcon**)&i);
    Boolean succeeded = FALSE;
    v->Set(i, &succeeded);

    ApplyColor(value, v);
    UpdateMoreText();
    return NOERROR;
}

void CNotificationOverflowIconsView::ApplyColor(
    /* [in] */ INotification* notification,
    /* [in] */ IStatusBarIconView* view)
{
    IImageView::Probe(view)->SetColorFilter(mTintColor, PorterDuffMode_MULTIPLY);
}

void CNotificationOverflowIconsView::UpdateMoreText()
{
    Int32 count = 0;
    GetChildCount(&count);
    AutoPtr<IInteger32> obj;
    CInteger32::New(count, (IInteger32**)&obj);
    AutoPtr<ArrayOf<IInterface*> > a = ArrayOf<IInterface*>::Alloc(1);
    a->Set(0, obj);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    String str;
    res->GetString(R::string::keyguard_more_overflow_text, a, &str);

    AutoPtr<ICharSequence> text;
    CString::New(str, (ICharSequence**)&text);
    mMoreText->SetText(text);
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
