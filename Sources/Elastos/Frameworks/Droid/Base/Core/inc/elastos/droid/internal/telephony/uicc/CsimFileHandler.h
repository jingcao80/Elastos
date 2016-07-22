#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CSIMFILEHANDLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CSIMFILEHANDLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/telephony/uicc/IccFileHandler.h"

// import android.telephony.Rlog;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * {@hide}
  * This class should be used to access files in CSIM ADF
  */
class CsimFileHandler
    : public IccFileHandler
    , public ICsimFileHandler
{
public:
    TO_STRING_IMPL("CsimFileHandler")

    CAR_INTERFACE_DECL();

    CsimFileHandler();

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

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CSIMFILEHANDLER_H__

