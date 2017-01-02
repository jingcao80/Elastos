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

#ifndef __ELASTOS_NET_CSOCKETINPUTSTREAM_H__
#define __ELASTOS_NET_CSOCKETINPUTSTREAM_H__

#include "_Elastos_Net_CPlainSocketInputStream.h"
#include "InputStream.h"

using Elastos::IO::InputStream;

namespace Elastos {
namespace Net {

CarClass(CPlainSocketInputStream)
    , public InputStream
{
public:
    CAR_OBJECT_DECL()

    CPlainSocketInputStream();

    CARAPI constructor(
        /* [in] */ ISocketImpl* impl);

    CARAPI Available(
        /* [out] */ Int32* number);

    CARAPI Close();

    CARAPI Read(
        /* [out] */ Int32* value);

    CARAPI Read(
        /* [out] */ ArrayOf<Byte>* buffer,
        /* [out] */ Int32* number);

    CARAPI Read(
        /* [out] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int32* number);

private:
    AutoPtr<ISocketImpl> mSocket;
};

} // namespace Net
} // namespace Elastos

#endif // __ELASTOS_NET_CSOCKETINPUTSTREAM_H__
