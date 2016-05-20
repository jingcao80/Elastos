
#ifndef __ELASTOS_SECURITY_CKEYSTORECALLBACKHANDLERPROTECTION_H__
#define __ELASTOS_SECURITY_CKEYSTORECALLBACKHANDLERPROTECTION_H__

#include "_Elastos_Security_CKeyStoreCallbackHandlerProtection.h"

using Elastosx::Security::Auth::Callback::ICallbackHandler;

namespace Elastos {
namespace Security {

CarClass(CKeyStoreCallbackHandlerProtection)
    , public Object
    , public IKeyStoreCallbackHandlerProtection
    , public IKeyStoreProtectionParameter
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCallbackHandler(
        /* [out] */ ICallbackHandler **handler);

    CARAPI constructor(
        /* [in] */ ICallbackHandler *handler);

private:
    // Store CallbackHandler
    const AutoPtr<ICallbackHandler> mCallbackHandler;
};

}
}

#endif // __ELASTOS_SECURITY_CKEYSTORECALLBACKHANDLERPROTECTION_H__
