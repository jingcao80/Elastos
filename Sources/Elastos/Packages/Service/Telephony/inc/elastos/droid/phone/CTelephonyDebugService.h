#ifndef  __ELASTOS_DROID_PHONE_CTELEPHONYDEBUGSERVICE_H__
#define  __ELASTOS_DROID_PHONE_CTELEPHONYDEBUGSERVICE_H__

#include "_Elastos_Droid_Phone_CTelephonyDebugService.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * A debug service for telephony.
 */
CarClass(CTelephonyDebugService)
    , public Service
    , public ITelephonyDebugService
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CTelephonyDebugService();

    CARAPI constructor();

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);

protected:
    //@Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* writer,
        /* [in] */ ArrayOf<String>* args);


private:
    static String TAG = "TelephonyDebugService";
    AutoPtr<IDebugService> mDebugService = new DebugService();
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CTELEPHONYDEBUGSERVICE_H__