
#ifndef _ELASTOS_DROID_WEBKIT_NET_CERTVERIFYSTATUSANDROID_H_
#define _ELASTOS_DROID_WEBKIT_NET_CERTVERIFYSTATUSANDROID_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.net;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Net {

class CertVerifyStatusElastos
    : public Object
{
public:
    static const Int32 VERIFY_OK = 0;
    static const Int32 VERIFY_FAILED = -1;
    static const Int32 VERIFY_NO_TRUSTED_ROOT = -2;
    static const Int32 VERIFY_EXPIRED = -3;
    static const Int32 VERIFY_NOT_YET_VALID = -4;
    static const Int32 VERIFY_UNABLE_TO_PARSE = -5;
    static const Int32 VERIFY_INCORRECT_KEY_USAGE = -6;
};

} // namespace Net
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_NET_CERTVERIFYSTATUSANDROID_H_

