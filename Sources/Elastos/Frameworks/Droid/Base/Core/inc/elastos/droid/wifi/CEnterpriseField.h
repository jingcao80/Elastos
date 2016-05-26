
#ifndef __ELASTOS_DROID_NET_WIFI_CENTERPRISEFIELD_H__
#define __ELASTOS_DROID_NET_WIFI_CENTERPRISEFIELD_H__

#include "_Elastos_Droid_Wifi_CEnterpriseField.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CEnterpriseField)
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ const String& varName);

    CARAPI SetValue(
        /* [in] */ const String& value);

    CARAPI GetVarName(
        /* [out] */ String* varName);

    CARAPI GetValue(
        /* [out] */ String* value);

private:
    String mVarName;
    String mValue;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_CENTERPRISEFIELD_H__
