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

#ifndef __ELASTOS_IO_CBYTEORDERHELPER_H__
#define __ELASTOS_IO_CBYTEORDERHELPER_H__

#include "_Elastos_IO_CByteOrderHelper.h"
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace IO {

/*
 * Defines byte order constants
 */
CarClass(CByteOrderHelper)
    , public Singleton
    , public IByteOrderHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns the current platform byte order.
     *
     * @return the byte order object, which is either LITTLE_ENDIAN or
     *         BIG_ENDIAN.
     */
    CARAPI GetNativeOrder(
        /* [out] */ ByteOrder* nativeOrder);

    /**
     * This is the only thing that ByteOrder is really used for: to know whether we need to swap
     * bytes to get this order, given bytes in native order. (That is, this is the opposite of
     * the hypothetical "isNativeOrder".)
     * @hide - needed in libcore.io too.
     */
    CARAPI IsNeedsSwap(
        /* [in] */ ByteOrder order,
        /* [out] */ Boolean* isNeedsSwap);

    static CARAPI_(ByteOrder) _GetNativeOrder();

    static CARAPI_(Boolean) _IsNeedsSwap(
        /* [in] */ ByteOrder order);

private:
    static void Init();

    static Boolean IsLittleEndian();

private:
    static ByteOrder    sNativeOrder;
    static ByteOrder    sBigEndian;
    static ByteOrder    sLittleEndian;
    static Boolean      sIsLittleEndian;
    static Boolean      sIsInited;
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CBYTEORDERHELPER_H__
