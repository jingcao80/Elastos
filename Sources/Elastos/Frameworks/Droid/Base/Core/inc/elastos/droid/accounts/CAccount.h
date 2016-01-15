
#ifndef __ELASTOS_DROID_ACCOUNTS_CACCOUNT_H__
#define __ELASTOS_DROID_ACCOUNTS_CACCOUNT_H__

#include "_Elastos_Droid_Accounts_CAccount.h"

namespace Elastos {
namespace Droid {
namespace Accounts {

CarClass(CAccount)
{
public:
    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI SetName(
        /* [in] */ const String& name);

    CARAPI GetType(
        /* [out] */ String* type);

    CARAPI SetType(
        /* [in] */ const String& type);

    CARAPI ToString(
        /* [out] */ String* s);

    CARAPI Equals(
        /* [in] */ IInterface *obj,
        /* [out] */ Boolean *equal);

    CARAPI GetHashCode(
        /* [out] */ Int32 *hashCode);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& type);

    CARAPI constructor();

public:
    String mName;
    String mType;

private:
    static const String TAG;
};

} // namespace Accounts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_ACCOUNTS_CACCOUNT_H__
