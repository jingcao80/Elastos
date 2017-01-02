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

#include "CInet4Address.h"
#include "CInet6Address.h"
#include "CInetUnixAddress.h"
#include "CDatagramSocket.h"
#include "CPlainServerSocketImpl.h"
#include "CPlainSocketImpl.h"
#include "CSocks4Message.h"
#include "CServerSocket.h"
#include "CSocket.h"
#include "elastos/net/CURI.h"

namespace Elastos {
namespace Net {

CAR_OBJECT_IMPL(CInet4Address)

CAR_OBJECT_IMPL(CInet6Address)

CAR_OBJECT_IMPL(CInetUnixAddress)

CAR_OBJECT_IMPL(CDatagramSocket)

CAR_OBJECT_IMPL(CPlainServerSocketImpl)

CAR_OBJECT_IMPL(CPlainSocketImpl)

CAR_OBJECT_IMPL(CSocks4Message)

CAR_OBJECT_IMPL(CServerSocket)

CAR_OBJECT_IMPL(CSocket)

CAR_OBJECT_IMPL(CURI)

} // namespace Net
} // namespace Elastos