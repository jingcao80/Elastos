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

#ifndef __ELASTOS_DROID_DATABASE_CHARARRAYBUFFER_H__
#define __ELASTOS_DROID_DATABASE_CHARARRAYBUFFER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Database.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Database {
/**
 * This is used for {@link Cursor#copyStringToBuffer}
 */
class CharArrayBuffer
    : public Object
    , public ICharArrayBuffer
{
public:
    CAR_INTERFACE_DECL()

    CharArrayBuffer();

    CARAPI constructor(
        /* [in] */ Int32 size);

    CARAPI constructor(
        /* [in] */ ArrayOf<Char32>* buf);

    virtual CARAPI GetData(
        /* [out, callee] */ ArrayOf<Char32>** data);

    virtual CARAPI SetData(
        /* [in] */ ArrayOf<Char32>* data);

    virtual CARAPI GetSizeCopied(
        /* [out] */ Int32* sizeCopied);

    virtual CARAPI SetSizeCopied(
        /* [in] */ Int32 sizeCopied);

public:
    AutoPtr< ArrayOf<Char32> > mData;
    Int32 mSizeCopied;
};

} //Database
} //Droid
} //Elastoss

#endif //_CHARARRAYBUFFER_H_
