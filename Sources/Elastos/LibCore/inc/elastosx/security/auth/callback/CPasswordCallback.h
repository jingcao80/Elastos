//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
