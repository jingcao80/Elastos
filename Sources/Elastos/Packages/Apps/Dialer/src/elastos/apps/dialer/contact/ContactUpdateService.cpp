#include "ContactUpdateService.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Contact {

CAR_INTERFACE_IMPL(ContactUpdateService, IntentService, IContactUpdateService)

ECode ContactUpdateService::constructor()
{
    IntentService::constructor(String("ContactUpdateService"));
    SetIntentRedelivery(TRUE);
    return NOERROR;
}

AutoPtr<IIntent> ContactUpdateService::CreateSetSuperPrimaryIntent(
    /* [in] */ IContext* context,
    /* [in] */ Int64 dataId)
{
    AutoPtr<IIntent> serviceIntent;
    CIntent::New(context, ECLSID_CContactUpdateService, (IIntent**)&serviceIntent);
    serviceIntent->PutExtra(IContactUpdateService::EXTRA_PHONE_NUMBER_DATA_ID, dataId);
    return serviceIntent;
}

ECode ContactUpdateService::OnHandleIntent(
    /* [in] */ IIntent* intent)
{
    // Currently this service only handles one type of update.
    Int64 dataId;
    intent->GetLongExtra(IContactUpdateService::EXTRA_PHONE_NUMBER_DATA_ID, -1, &dataId);

    ContactUpdateUtils::SetSuperPrimary((IContext*)this, dataId);
    return NOERROR;
}


} // Contact
} // Dialer
} // Apps
} // Elastos
