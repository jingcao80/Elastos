
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
