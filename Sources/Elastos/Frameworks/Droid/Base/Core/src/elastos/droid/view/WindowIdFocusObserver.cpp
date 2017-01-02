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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/WindowIdFocusObserver.h"
#include "elastos/droid/view/CWindowIdFocusObserver.h"

using Elastos::Utility::CHashMap;

namespace Elastos {
namespace Droid {
namespace View {

WindowIdFocusObserver::H::H(
    /* [in] */ WindowIdFocusObserver* host)
    : mHost(host)
{}

ECode WindowIdFocusObserver::H::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    switch (what) {
        case 1: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mHost->OnFocusGained(IWindowId::Probe(obj));
            break;
        }
        case 2: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mHost->OnFocusLost(IWindowId::Probe(obj));
            break;
        }
        default:
            Handler::HandleMessage(msg);
    }
    return NOERROR;
}

WindowIdFocusObserver::WindowIdFocusObserver()
{
}

ECode WindowIdFocusObserver::constructor()
{
    CHashMap::New((IHashMap**)&mRegistrations);
    AutoPtr<H> h = new H(this);
    h->constructor();
    mHandler = h.Get();
    CWindowIdFocusObserver::New(this, (IIWindowFocusObserver**)&mIObserver);
    return NOERROR;
}

} // namespace View
} // namepsace Droid
} // namespace Elastos
