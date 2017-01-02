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

#ifndef __LIBCORE_ICU_TRANSLITERATOR__
#define __LIBCORE_ICU_TRANSLITERATOR__

#include "Elastos.CoreLibrary.Libcore.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Libcore {
namespace ICU {

class Transliterator
    : public Object
    , public ITransliterator
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ const String& id);

protected:
    virtual ~Transliterator();

public:
    CARAPI GetAvailableIDs(
        /* [out, callee] */ ArrayOf<String>** ids);

    CARAPI Transliterate(
        /* [in] */ const String& s,
        /* [out] */ String* rev);

    static CARAPI_(Int64) Create(
        /* [in] */ const String& id);

    static CARAPI_(void) Destroy(
        /* [in] */ Int64 peer);

    static CARAPI_(String) Transliterate(
        /* [in] */ Int64 peer,
        /* [in] */ const String& s);

private:
    Int64 mPeer;
};

} // ICU
} // Libcore

#endif // __LIBCORE_ICU_TRANSLITERATOR__