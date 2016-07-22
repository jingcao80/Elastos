#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_USIMFILEHANDLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_USIMFILEHANDLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/telephony/uicc/IccFileHandler.h"

// import android.telephony.Rlog;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * {@hide}
  * This class should be used to access files in USIM ADF
  */
class UsimFileHandler
    : public IccFileHandler
    , public IUsimFileHandler
{
public:
    TO_STRING_IMPL("UsimFileHandler")

    CAR_INTERFACE_DECL();

    UsimFileHandler();

    CARAPI constructor(
        /* [in] */ IUiccCardApplication* app,
        /* [in] */ const String& aid,
        /* [in] */ ICommandsInterface* ci);

protected:
    // @Override
    CARAPI_(String) GetEFPath(
        /* [in] */ Int32 efid);

    // @Override
    CARAPI Logd(
        /* [in] */ const String& msg);

    // @Override
    CARAPI Loge(
        /* [in] */ const String& msg);

public:
    static const String LOGTAG;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_USIMFILEHANDLER_H__
