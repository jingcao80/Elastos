
#ifndef _ELASTOS_DROID_WEBKIT_NET_PRIVATEKEYTYPE_H_
#define _ELASTOS_DROID_WEBKIT_NET_PRIVATEKEYTYPE_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.net;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Net {

class PrivateKeyType
    : public Object
{
public:
    static const Int32 RSA = 0;
    static const Int32 DSA = 1;
    static const Int32 ECDSA = 2;
    static const Int32 INVALID = 255;
};

} // namespace Net
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_NET_PRIVATEKEYTYPE_H_

