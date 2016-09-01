
#ifndef __ELASTOS_DROID_ACCOUNTS_CACCOUNT_H__
#define __ELASTOS_DROID_ACCOUNTS_CACCOUNT_H__

#include "_Elastos_Droid_Accounts_CAccount.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Accounts {

CarClass(CAccount)
    , public Object
    , public IAccount
    , public IParcelable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& type);

    CARAPI constructor();

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
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* equal);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

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
