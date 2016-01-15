
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
    CHashMap::New((IHashMap**)&mRegistrations);
    mHandler = new H(this);
    CWindowIdFocusObserver::New(this, (IIWindowFocusObserver**)&mIObserver);
}

} // namespace View
} // namepsace Droid
} // namespace Elastos
