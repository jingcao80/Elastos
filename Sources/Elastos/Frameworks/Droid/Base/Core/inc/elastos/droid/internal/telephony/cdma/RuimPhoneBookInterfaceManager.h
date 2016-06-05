#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_RUIMPHONEBOOKINTERFACEMANAGER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_RUIMPHONEBOOKINTERFACEMANAGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
//TODO #include "elastos/droid/internal/telephony/IccPhoneBookInterfaceManager.h"

// import java.util.concurrent.atomic.AtomicBoolean;
// import android.os.Message;
// import android.telephony.Rlog;
// import com.android.internal.telephony.uicc.IccFileHandler;

//TODO using Elastos::Droid::Internal::Telephony::IccPhoneBookInterfaceManager;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

/**
  * RuimPhoneBookInterfaceManager to provide an inter-process communication to
  * access ADN-like SIM records.
  */
class RuimPhoneBookInterfaceManager
    : public Object //TODO IccPhoneBookInterfaceManager
    , public IRuimPhoneBookInterfaceManager
{
public:
    CAR_INTERFACE_DECL();

    RuimPhoneBookInterfaceManager(
        /* [in] */ ICDMAPhone* phone);

    // @Override
    CARAPI Dispose();

    // @Override
    CARAPI GetAdnRecordsSize(
        /* [in] */ Int32 efid,
        /* [out] */ ArrayOf<Int32>** result);

protected:
    // @Override
    CARAPI_(void) Finalize();

    // @Override
    CARAPI Logd(
        /* [in] */ const String& msg);

    // @Override
    CARAPI Loge(
        /* [in] */ const String& msg);

public:
    static const String LOGTAG;
};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_RUIMPHONEBOOKINTERFACEMANAGER_H__

