
#ifndef __ELASTOSX_SECURITY_AUTH_CALLBACK_CPASSWORDCALLBACK_H__
#define __ELASTOSX_SECURITY_AUTH_CALLBACK_CPASSWORDCALLBACK_H__

#include "_Elastosx_Security_Auth_Callback_CPasswordCallback.h"
#include "Object.h"

namespace Elastosx {
namespace Security {
namespace Auth {
namespace Callback {

CarClass(CPasswordCallback)
    , public Object
    , public IPasswordCallback
    , public ICallback
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CPasswordCallback();

    CARAPI GetPrompt(
        /* [out] */ String *prompt);

    CARAPI IsEchoOn(
        /* [out] */ Boolean *echoOn);

    CARAPI SetPassword(
        /* [in] */ ArrayOf<Char32>* password);

    CARAPI GetPassword(
        /* [out, callee] */ ArrayOf<Char32>** password);

    CARAPI ClearPassword();

    CARAPI constructor(
        /* [in] */ const String& prompt,
        /* [in] */ Boolean echoOn);

private:
    CARAPI SetPrompt(
        /* [in] */ const String& prompt);

public:
    Boolean mEchoOn;

private:
    static const Int64 serialVersionUID;

    String mPrompt;

    AutoPtr<ArrayOf<Char32> > mInputPassword;
};

} // namespace Callback
} // namespace Auth
} // namespace Security
} // namespace Elastosx

#endif // __ELASTOSX_SECURITY_AUTH_CALLBACK_CPASSWORDCALLBACK_H__
