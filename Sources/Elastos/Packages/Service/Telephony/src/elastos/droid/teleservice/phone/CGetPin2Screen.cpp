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

#include "elastos/droid/teleservice/phone/CGetPin2Screen.h"
#include "elastos/droid/text/TextUtils.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::IInputType;
using Elastos::Droid::Text::Method::IKeyListener;
using Elastos::Droid::Text::Method::IDigitsKeyListener;
using Elastos::Droid::Text::Method::IDigitsKeyListenerHelper;
using Elastos::Droid::Text::Method::CDigitsKeyListenerHelper;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::EIID_IOnEditorActionListener;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CAR_INTERFACE_IMPL(CGetPin2Screen::InnerListener, Object, IViewOnClickListener)

ECode CGetPin2Screen::InnerListener::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<ICharSequence> cchar;
    ITextView::Probe(mHost->mPin2Field)->GetText((ICharSequence**)&cchar);
    String str;
    cchar->ToString(&str);
    if (TextUtils::IsEmpty(str)) {
        return NOERROR;
    }

    mHost->ReturnResult();
    return NOERROR;
}


ECode CGetPin2Screen::InnerListener::OnEditorAction(
    /* [in] */ ITextView* v,
    /* [in] */ Int32 actionId,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    return mHost->OnEditorAction(v, actionId, event, result);
}

const String CGetPin2Screen::TAG("CGetPin2Screen");// = PhoneGlobals.TAG;


CAR_OBJECT_IMPL(CGetPin2Screen)

CGetPin2Screen::CGetPin2Screen()
{
}

ECode CGetPin2Screen::constructor()
{
    return Activity::constructor();
}

ECode CGetPin2Screen::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Activity::OnCreate(icicle);

    SetContentView(Elastos::Droid::TeleService::R::layout::get_pin2_screen);

    mInnerListener = new InnerListener(this);

    AutoPtr<IView> view;
    FindViewById(Elastos::Droid::TeleService::R::id::pin, (IView**)&view);
    mPin2Field = IEditText::Probe(view);

    AutoPtr<IDigitsKeyListenerHelper> helper;
    CDigitsKeyListenerHelper::AcquireSingleton((IDigitsKeyListenerHelper**)&helper);
    AutoPtr<IDigitsKeyListener> listener;
    helper->GetInstance((IDigitsKeyListener**)&listener);
    ITextView::Probe(mPin2Field)->SetKeyListener(IKeyListener::Probe(listener));
    ITextView::Probe(mPin2Field)->SetMovementMethod(NULL);
    ITextView::Probe(mPin2Field)->SetOnEditorActionListener(mInnerListener);
    ITextView::Probe(mPin2Field)->SetInputType(IInputType::TYPE_CLASS_NUMBER |
            IInputType::TYPE_NUMBER_VARIATION_PASSWORD);

    AutoPtr<IView> view2;
    FindViewById(Elastos::Droid::TeleService::R::id::ok, (IView**)&view2);
    mOkButton = IButton::Probe(view2);
    return IView::Probe(mOkButton)->SetOnClickListener(mInnerListener);
}

String CGetPin2Screen::GetPin2()
{
    AutoPtr<ICharSequence> cchar;
    ITextView::Probe(mPin2Field)->GetText((ICharSequence**)&cchar);
    String str;
    cchar->ToString(&str);
    return str;
}

void CGetPin2Screen::ReturnResult()
{
    AutoPtr<IBundle> map;
    CBundle::New((IBundle**)&map);
    map->PutString(String("pin2"), GetPin2());

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    AutoPtr<IUri> uri;
    intent->GetData((IUri**)&uri);

    AutoPtr<IIntent> action;
    CIntent::New((IIntent**)&action);

    if (uri != NULL) {
        String str;
        IObject::Probe(uri)->ToString(&str);
        action->SetAction(str);
    }

    action->PutExtras(map);
    SetResult(RESULT_OK, action);
    Finish();
}

ECode CGetPin2Screen::OnEditorAction(
    /* [in] */ ITextView* v,
    /* [in] */ Int32 actionId,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (actionId == IEditorInfo::IME_ACTION_DONE) {
        Boolean tmp;
        IView::Probe(mOkButton)->PerformClick(&tmp);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

void CGetPin2Screen::Log(
    /* [in] */ const String& msg)
{
    StringBuilder sb;
    sb += "[GetPin2] ";
    sb += msg;
    Logger::D(TAG, sb.ToString());
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos