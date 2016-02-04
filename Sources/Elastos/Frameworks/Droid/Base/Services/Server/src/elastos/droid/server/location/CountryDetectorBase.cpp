#include "elastos/droid/server/location/CountryDetectorBase.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::Os::CHandler;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

CountryDetectorBase::CountryDetectorBase(
    /* [in] */ IContext* ctx)
{
    mContext = ctx;
    CHandler::New((IHandler**)&mHandler);
}

ECode CountryDetectorBase::SetCountryListener(
    /* [in] */ ICountryListener* listener)
{
    mListener = listener;
    return NOERROR;
}

ECode CountryDetectorBase::NotifyListener(
    /* [in] */ ICountry* country)
{
    if (mListener != NULL) {
        mListener->OnCountryDetected(country);
    }
    return NOERROR;
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos