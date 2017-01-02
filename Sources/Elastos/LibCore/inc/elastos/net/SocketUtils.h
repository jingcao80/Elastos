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

#ifndef __ELASTOS_NET_SOCKETUTILS_H__
#define __ELASTOS_NET_SOCKETUTILS_H__

#include "Socket.h"

namespace Elastos {
namespace Net {

/**
 * @hide internal use only
 */
class SocketUtils
{
public:
    /**
     * Helps us reuse more of Socket's implementation in SocketChannelImpl.SocketAdapter.
     * It's not the case that we should set isCreated in the Socket(SocketImpl) constructor;
     * SocketImpl.create should be called in general. But for SocketChannelImpl.SocketAdapter's
     * SocketImpl, we don't want that behavior.
     */
    static CARAPI_(void) SetCreated(
        /* [in] */ ISocket* s);

private:
    SocketUtils();
    SocketUtils(const SocketUtils&);
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_SOCKETUTILS_H__