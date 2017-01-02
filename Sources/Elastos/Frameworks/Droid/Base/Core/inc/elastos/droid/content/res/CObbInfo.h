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

#ifndef __ELASTOS_DROID_CONTENT_RES_COBBINFO_H__
#define __ELASTOS_DROID_CONTENT_RES_COBBINFO_H__

#include "_Elastos_Droid_Content_Res_CObbInfo.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CarClass(CObbInfo)
    , public Object
    , public IObbInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CObbInfo();

    CARAPI GetFilename(
        /* [out] */ String* filename);

    CARAPI SetFilename(
        /* [in] */ const String& filename);

    CARAPI GetPackageName(
        /* [out] */ String* name);

    CARAPI SetPackageName(
        /* [in] */ const String& name);

    CARAPI GetVersion(
        /* [out] */ Int32 *ver);

    CARAPI SetVersion(
        /* [in] */ Int32 ver);

    CARAPI GetFlags(
        /* [out] */ Int32* flags);

    CARAPI SetFlags(
        /* [in] */ Int32 flags);

    CARAPI GetSalt(
        /* [out, callee] */ ArrayOf<Byte>** salt);

    CARAPI SetSalt(
        /* [in] */ ArrayOf<Byte>* salt);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI constructor();

public:
    /**
     * The canonical filename of the OBB.
     */
    String mFilename;

    /**
     * The name of the package to which the OBB file belongs.
     */
    String mPackageName;

    /**
     * The version of the package to which the OBB file belongs.
     */
    Int32  mVersion;

    /**
     * The flags relating to the OBB.
     */
    Int32  mFlags;

    /**
     * The salt for the encryption algorithm.
     *
     * @hide
     */
    AutoPtr<ArrayOf<Byte> > mSalt;
};

} // namespace Elastos
} // namespace Droid
} // namespace Content
} // namespace Res

#endif // __ELASTOS_DROID_CONTENT_RES_COBBINFO_H__
