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

#ifndef __ELASTOS_DROID_NET_LOCALSOCKETADDRESS_H__
#define __ELASTOS_DROID_NET_LOCALSOCKETADDRESS_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * A UNIX-domain (AF_LOCAL) socket address. For use with
 * android.net.LocalSocket and android.net.LocalServerSocket.
 *
 * On the Android system, these names refer to names in the Linux
 * abstract (non-filesystem) UNIX domain namespace.
 */
class LocalSocketAddress
    : public Object
    , public ILocalSocketAddress
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates an instance with a given name.
     *
     * @param name non-null name
     * @param namespace namespace the name should be created in.
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ LocalSocketAddressNamespace ns);

    /**
     * Creates an instance with a given name in the {@link Namespace#ABSTRACT}
     * namespace
     *
     * @param name non-null name
     */
    CARAPI constructor(
        /* [in] */ const String& name);

    /**
     * Retrieves the string name of this address
     * @return string name
     */
    CARAPI GetName(
        /* [out] */ String* result);

    /**
     * Returns the namespace used by this address.
     *
     * @return non-null a namespace
     */
    CARAPI GetNamespace(
        /* [out] */ LocalSocketAddressNamespace* result);

private:
    String mName;

    LocalSocketAddressNamespace mNamespace;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_LOCALSOCKETADDRESS_H__
