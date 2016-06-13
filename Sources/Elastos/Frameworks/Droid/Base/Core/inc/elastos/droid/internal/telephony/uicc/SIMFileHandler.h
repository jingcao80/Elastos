#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_SIMFILEHANDLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_SIMFILEHANDLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/telephony/uicc/IccFileHandler.h"

// import android.telephony.Rlog;
// import com.android.internal.telephony.CommandsInterface;

using Elastos::Droid::Internal::Telephony::ICommandsInterface;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * {@hide}
  */
class SIMFileHandler
    : public IccFileHandler
{
public:
    SIMFileHandler();

    //***** Instance Variables
    //***** Constructor
    CARAPI constructor(
        /* [in] */ IUiccCardApplication* app,
        /* [in] */ const String& aid,
        /* [in] */ ICommandsInterface* ci);

protected:
    //***** Overridden from IccFileHandler
    // @Override
    CARAPI GetEFPath(
        /* [in] */ Int32 efid,
        /* [out] */ String* result);

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

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_SIMFILEHANDLER_H__
