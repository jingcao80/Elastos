#ifndef  __ELASTOS_DROID_PHONE_CTELEPHONYDEBUGSERVICE_H__
#define  __ELASTOS_DROID_PHONE_CTELEPHONYDEBUGSERVICE_H__

#include "_Elastos_Droid_Phone_CTelephonyDebugService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Service.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.IO.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Internal::Telephony::IDebugService;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * A debug service for telephony.
 */
CarClass(CTelephonyDebugService)
    , public Elastos::Droid::App::Service
{
public:
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
    static String TAG;
    AutoPtr<IDebugService> mDebugService;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CTELEPHONYDEBUGSERVICE_H__