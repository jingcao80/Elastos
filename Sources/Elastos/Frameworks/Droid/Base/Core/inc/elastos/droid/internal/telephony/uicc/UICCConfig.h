#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_UICCCONFIG_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_UICCCONFIG_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * A class that stores various UICC Settings/values.
  * @hide
  */
class UICCConfig
    : public Object
    , public IUICCConfig
{
public:
    CAR_INTERFACE_DECL();

    UICCConfig();

    /**
      * A method to get the stored Imsi.
      * @hide
      */
    virtual CARAPI GetImsi(
        /* [out] */ String* result);

    /**
      * A method to set the stored Imsi.
      * @hide
      */
    virtual CARAPI SetImsi(
        /* [in] */ const String& lImsi);

    /**
      * A method to get the stored MncLength.
      * @hide
      */
    virtual CARAPI GetMncLength(
        /* [out] */ Int32* result);

    /**
      * A method to set the stored MncLength.
      * @hide
      */
    virtual CARAPI SetMncLength(
        /* [in] */ Int32 lMncLength);

private:
    CARAPI_(void) Logd(
        /* [in] */ const String& sLog);

    CARAPI_(void) Loge(
        /* [in] */ const String& sLog);

private:
    /*const*/ String PREFERENCE_NAME;
    /*const*/ String TAG;
    /*const*/ Boolean LOG_DEBUG;
    String mImsi;
    Int32 mMncLength;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_UICCCONFIG_H__

