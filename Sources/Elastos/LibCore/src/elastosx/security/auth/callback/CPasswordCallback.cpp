
#include "CPasswordCallback.h"

namespace Elastosx {
namespace Security {
namespace Auth {
namespace Callback {

CAR_OBJECT_IMPL(CPasswordCallback)

CAR_INTERFACE_IMPL_2(CPasswordCallback, Object, IPasswordCallback, ICallback)

const Int64 CPasswordCallback::serialVersionUID = 2267422647454909926L;

CPasswordCallback::CPasswordCallback()
    : mEchoOn(FALSE)
{}

ECode CPasswordCallback::GetPrompt(
    /* [out] */ String *prompt)
{
    VALIDATE_NOT_NULL(prompt)
    *prompt = mPrompt;
    return NOERROR;
}

ECode CPasswordCallback::IsEchoOn(
    /* [out] */ Boolean *echoOn)
{
    VALIDATE_NOT_NULL(echoOn)
    *echoOn = mEchoOn;
    return NOERROR;
}

ECode CPasswordCallback::SetPassword(
    /* [in] */ ArrayOf<Char32>* password)
{
    if (password->GetLength() == 0) {
        mInputPassword = password;
    }
    else {
        mInputPassword = ArrayOf<Char32>::Alloc(password->GetLength());
        mInputPassword->Copy(0, password, 0, mInputPassword->GetLength());
    }
    return NOERROR;
}

ECode CPasswordCallback::GetPassword(
    /* [out, callee] */ ArrayOf<Char32> **password)
{
    VALIDATE_NOT_NULL(password)
    if (mInputPassword != NULL) {
        AutoPtr<ArrayOf<Char32> > tmp = ArrayOf<Char32>::Alloc(mInputPassword->GetLength());
        tmp->Copy(0, mInputPassword, 0, tmp->GetLength());
        *password = tmp;
        REFCOUNT_ADD(*password)
    }
    return NOERROR;
}

ECode CPasswordCallback::ClearPassword()
{
    if (mInputPassword != NULL) {
        Char32 *pt = mInputPassword->GetPayload();
        memset((void*)pt, 0, mInputPassword->GetLength() * sizeof(Char32));
    }
    return NOERROR;
}

ECode CPasswordCallback::constructor(
    /* [in] */ const String& prompt,
    /* [in] */ Boolean echoOn)
{
    FAIL_RETURN(SetPrompt(prompt))
    mEchoOn = echoOn;
    return NOERROR;
}

ECode CPasswordCallback::SetPrompt(
        /* [in] */ const String& prompt)
{
    if (prompt.IsNull() || prompt.GetLength() == 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mPrompt = prompt;
    return NOERROR;
}

} // namespace Callback
} // namespace Auth
} // namespace Security
} // namespace Elastosx

