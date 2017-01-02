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

#ifndef __ELASTOS_NET_CSOCKETOUTPUTSTREAM_H__
#define __ELASTOS_NET_CSOCKETOUTPUTSTREAM_H__

#include "_Elastos_Net_CPlainSocketOutputStream.h"
#include "OutputStream.h"

using Elastos::IO::OutputStream;

namespace Elastos {
namespace Net {

CarClass(CPlainSocketOutputStream)
    , public OutputStream
{
public:
    CAR_OBJECT_DECL()

    CPlainSocketOutputStream();

    CARAPI constructor(
        /* [in] */ ISocketImpl * pImpl);

    CARAPI Close();


    CARAPI Write(
        /* [in] */ Int32 oneByte);

    CARAPI Write(
        /* [in] */ ArrayOf<Byte> * buffer);

    CARAPI Write(
        /* [in] */ ArrayOf<Byte> * buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

private:
    AutoPtr<ISocketImpl> mSocket;
};

} // namespace Net
} // namespace Elastos

#endif // __ELASTOS_NET_CSOCKETOUTPUTSTREAM_H__
