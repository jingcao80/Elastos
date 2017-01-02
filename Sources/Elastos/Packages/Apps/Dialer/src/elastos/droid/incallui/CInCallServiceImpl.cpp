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

#include "elastos/droid/incallui/AudioModeProvider.h"
#include "elastos/droid/incallui/CallList.h"
#include "elastos/droid/incallui/CInCallServiceImpl.h"
#include "elastos/droid/incallui/InCallPresenter.h"
#include "elastos/droid/incallui/TelecomAdapter.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

CAR_OBJECT_IMPL(CInCallServiceImpl);

ECode CInCallServiceImpl::OnPhoneCreated(
    /* [in] */ IPhone* phone)
{
    Logger::V("CInCallServiceImpl", "OnPhoneCreated");
    CallList::GetInstance()->SetPhone(phone);
    AudioModeProvider::GetInstance()->SetPhone(phone);
    TelecomAdapter::GetInstance()->SetPhone(phone);
    InCallPresenter::GetInstance()->SetPhone(phone);
    AutoPtr<IContext> context;
    GetApplicationContext((IContext**)&context);
    InCallPresenter::GetInstance()->SetUp(
            context, CallList::GetInstance(),
            AudioModeProvider::GetInstance());
    TelecomAdapter::GetInstance()->SetContext(this);
    return NOERROR;
}

ECode CInCallServiceImpl::OnPhoneDestroyed(
    /* [in] */ IPhone* phone)
{
    Logger::V("CInCallServiceImpl", "OnPhoneDestroyed");
    // Tear down the InCall system
    CallList::GetInstance()->ClearPhone();
    AudioModeProvider::GetInstance()->ClearPhone();
    TelecomAdapter::GetInstance()->ClearPhone();
    TelecomAdapter::GetInstance()->SetContext(NULL);
    CallList::GetInstance()->ClearOnDisconnect();
    InCallPresenter::GetInstance()->TearDown();
    return NOERROR;
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
