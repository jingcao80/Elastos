#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_SPNOVERRIDE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_SPNOVERRIDE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// import java.io.File;
// import java.io.FileNotFoundException;
// import java.io.FileReader;
// import java.io.IOException;
// import java.util.HashMap;
// import org.xmlpull.v1.XmlPullParser;
// import org.xmlpull.v1.XmlPullParserException;
// import android.os.Environment;
// import android.telephony.Rlog;
// import android.util.Xml;
// import com.android.internal.util.XmlUtils;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

class SpnOverride
    : public Object
    , public ISpnOverride
{
public:
    CAR_INTERFACE_DECL();

    SpnOverride();

    virtual CARAPI ContainsCarrier(
        /* [in] */ const String& carrier,
        /* [out] */ Boolean* result);

    virtual CARAPI GetSpn(
        /* [in] */ const String& carrier,
        /* [out] */ String* result);

private:
    CARAPI_(void) LoadSpnOverrides();

public:
    static const String LOGTAG;
    static const String PARTNER_SPN_OVERRIDE_PATH;

private:
     AutoPtr<IHashMap/*<String, String*/> mCarrierSpnMap;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_SPNOVERRIDE_H__
