#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_VOICEMAILCONSTANTS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_VOICEMAILCONSTANTS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * {@hide}
  */
class VoiceMailConstants
    : public Object
{
public:
    VoiceMailConstants();

    virtual CARAPI ContainsCarrier(
        /* [in] */ const String& carrier,
        /* [out] */ Boolean* result);

    virtual CARAPI GetCarrierName(
        /* [in] */ const String& carrier,
        /* [out] */ String* result);

    virtual CARAPI GetVoiceMailNumber(
        /* [in] */ const String& carrier,
        /* [out] */ String* result);

    virtual CARAPI GetVoiceMailTag(
        /* [in] */ const String& carrier,
        /* [out] */ String* result);

private:
    CARAPI_(void) LoadVoiceMail();

public:
    static const String LOGTAG;
    static const String PARTNER_VOICEMAIL_PATH;
    static const Int32 NAME = 0;
    static const Int32 NUMBER = 1;
    static const Int32 TAG = 2;
    static const Int32 SIZE = 3;

private:
     AutoPtr<IHashMap> mCarrierVmMap;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_VOICEMAILCONSTANTS_H__

