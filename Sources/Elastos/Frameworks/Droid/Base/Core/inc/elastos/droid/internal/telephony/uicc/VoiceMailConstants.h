#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_VOICEMAILCONSTANTS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_VOICEMAILCONSTANTS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// import android.os.Environment;
// import android.util.Xml;
// import android.telephony.Rlog;
// import java.util.HashMap;
// import java.io.FileReader;
// import java.io.File;
// import java.io.FileNotFoundException;
// import java.io.IOException;
// import org.xmlpull.v1.XmlPullParser;
// import org.xmlpull.v1.XmlPullParserException;
// import com.android.internal.util.XmlUtils;
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
     AutoPtr<IHashMap/*<String,String[]*/> CarrierVmMap;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_VOICEMAILCONSTANTS_H__

