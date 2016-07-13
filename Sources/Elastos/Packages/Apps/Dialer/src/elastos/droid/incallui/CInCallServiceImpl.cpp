
#include "elastos/droid/incallui/CInCallServiceImpl.h"

namespace Elastos {
namespace Droid {
namespace InCallUI {

CAR_OBJECT_IMPL(CInCallServiceImpl);

ECode CInCallServiceImpl::OnPhoneCreated(
    /* [in] */ IPhone* phone)
{
    assert(0 && "TODO");
    // Log.v(this, "onPhoneCreated");
    // CallList.getInstance().setPhone(phone);
    // AudioModeProvider.getInstance().setPhone(phone);
    // TelecomAdapter.getInstance().setPhone(phone);
    // InCallPresenter.getInstance().setPhone(phone);
    // InCallPresenter.getInstance().setUp(
    //         getApplicationContext(),
    //         CallList.getInstance(),
    //         AudioModeProvider.getInstance());
    // TelecomAdapter.getInstance().setContext(InCallServiceImpl.this);
    return NOERROR;
}

ECode CInCallServiceImpl::OnPhoneDestroyed(
    /* [in] */ IPhone* phone)
{
    assert(0 && "TODO");
    // Log.v(this, "onPhoneDestroyed");
    // // Tear down the InCall system
    // CallList.getInstance().clearPhone();
    // AudioModeProvider.getInstance().clearPhone();
    // TelecomAdapter.getInstance().clearPhone();
    // TelecomAdapter.getInstance().setContext(null);
    // CallList.getInstance().clearOnDisconnect();
    // InCallPresenter.getInstance().tearDown();
    return NOERROR;
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
