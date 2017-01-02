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

#include "elastos/droid/teleservice/phone/IccPanel.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "Elastos.Droid.App.h"
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

const String IccPanel::TAG("IccPanel");// = IPhoneGlobals.TAG;

ECode IccPanel::constructor(
    /* [in] */ IContext* context)
{
    return Dialog::constructor(context, Elastos::Droid::TeleService::R::style::IccPanel);
}

ECode IccPanel::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Dialog::OnCreate(savedInstanceState);
    AutoPtr<IWindow> winP;
    GetWindow((IWindow**)&winP);
    winP->SetType(IWindowManagerLayoutParams::TYPE_PRIORITY_PHONE);
    winP->SetLayout(IViewGroupLayoutParams::MATCH_PARENT,
            IViewGroupLayoutParams::MATCH_PARENT);
    winP->SetGravity(IGravity::CENTER);

    // TODO: Ideally, we'd like this dialog to be visible in front of the
    // keyguard, so the user will see it immediately after boot (without
    // needing to enter the lock pattern or dismiss the keyguard first.)
    //
    // However that's not easy to do.  It's not just a matter of setting
    // the FLAG_SHOW_WHEN_LOCKED and FLAG_DISMISS_KEYGUARD flags on our
    // window, since we're a Dialog (not an Activity), and the framework
    // won't ever let a dialog hide the keyguard (because there could
    // possibly be stuff behind it that shouldn't be seen.)
    //
    // So for now, we'll live with the fact that the user has to enter the
    // lock pattern (or dismiss the keyguard) *before* being able to type
    // a SIM network unlock PIN.  That's not ideal, but still OK.
    // (And eventually this will be a moot point once this UI moves
    // from the phone app to the framework; see bug 1804111).

    // TODO: we shouldn't need the mStatusBarManager calls here either,
    // once this dialog gets moved into the framework and becomes a truly
    // full-screen UI.
    AutoPtr<PhoneGlobals> app;
    PhoneGlobals::GetInstance((PhoneGlobals**)&app);

    AutoPtr<IInterface> obj;
    app->GetSystemService(IContext::STATUS_BAR_SERVICE, (IInterface**)&obj);
    mStatusBarManager = IStatusBarManager::Probe(obj);

    Boolean res;
    return RequestWindowFeature(IWindow::FEATURE_NO_TITLE, &res);
}

ECode IccPanel::OnStart()
{
    Dialog::OnStart();
    return mStatusBarManager->Disable(IStatusBarManager::DISABLE_EXPAND);
}

ECode IccPanel::OnStop()
{
    Dialog::OnStop();
    return mStatusBarManager->Disable(IStatusBarManager::DISABLE_NONE);
}

ECode IccPanel::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (keyCode == IKeyEvent::KEYCODE_BACK) {
        *result = TRUE;
        return NOERROR;
    }

    return Dialog::OnKeyDown(keyCode, event, result);
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos