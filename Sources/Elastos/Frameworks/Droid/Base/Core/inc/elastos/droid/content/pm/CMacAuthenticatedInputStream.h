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

#ifndef __ELASTOS_DROID_CONTENT_PM_CMACAUTHENTICATEDINPUTSTREAM_H__
#define __ELASTOS_DROID_CONTENT_PM_CMACAUTHENTICATEDINPUTSTREAM_H__

#include "_Elastos_Droid_Content_Pm_CMacAuthenticatedInputStream.h"
#include "elastos/io/FilterInputStream.h"

using Elastosx::Crypto::IMac;
using Elastos::IO::IInputStream;
using Elastos::IO::FilterInputStream;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * An input stream filter that applies a MAC to the data passing through it. At
 * the end of the data that should be authenticated, the tag can be calculated.
 * After that, the stream should not be used.
 *
 * @hide
 */
CarClass(CMacAuthenticatedInputStream)
    , public FilterInputStream
    , public IMacAuthenticatedInputStream
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CMacAuthenticatedInputStream();

    virtual ~CMacAuthenticatedInputStream();

    CARAPI constructor(
        /* [in] */ IInputStream* inStream,
        /* [in] */ IMac* mac);

    CARAPI IsTagEqual(
        /* [in] */ ArrayOf<Byte>* tag,
        /* [out] */ Boolean* result);

    CARAPI Read(
        /* [out] */ Int32* value);

    CARAPI Read(
        /* [out] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [out] */ Int32* number);

private:
    AutoPtr<IMac> mMac;
};

}
}
}
}

#endif // __ELASTOS_DROID_CONTENT_PM_CMACAUTHENTICATEDINPUTSTREAM_H__
