
#ifndef __ELASTOS_DROID_TELEPHONY_CVOLTESERVICESTATE_H__
#define __ELASTOS_DROID_TELEPHONY_CVOLTESERVICESTATE_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Telephony_CVoLteServiceState.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CVoLteServiceState)
    , public Object
    , public IVoLteServiceState
    , public IParcelable
{
public:
    CVoLteServiceState();

    virtual ~CVoLteServiceState();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 srvccState);

    CARAPI constructor(
        /* [in] */ IVoLteServiceState* s);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ValidateInput();

    CARAPI FillInNotifierBundle(
        /* [in] */ IBundle* m);

    CARAPI GetSrvccState(
        /* [out] */ Int32* state);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* res);

    CARAPI ToString(
        /* [out] */ String* str);

    static CARAPI NewFromBundle(
        /* [in] */ IBundle* m,
        /* [out] */ IVoLteServiceState** state);

protected:
    CARAPI_(void) CopyFrom(
        /* [in] */ IVoLteServiceState* s);

private:
    CARAPI_(void) Initialize();

    CARAPI_(void) SetFromNotifierBundle(
        /* [in] */ IBundle* m);

private:
    static const String TAG;
    static const Boolean DBG = FALSE;

    Int32 mSrvccState;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CVOLTESERVICESTATE_H__
