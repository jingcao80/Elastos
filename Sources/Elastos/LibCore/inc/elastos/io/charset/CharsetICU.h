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

#ifndef __ELASTOS_IO_CHARSETICU_H__
#define __ELASTOS_IO_CHARSETICU_H__

#include "Charset.h"

namespace Elastos {
namespace IO {
namespace Charset {

class CharsetICU
    : public Charset
{
public:
    CharsetICU();

    CARAPI constructor(
        /* [in] */ const String& canonicalName,
        /* [in] */ const String& icuCanonName,
        /* [in] */ ArrayOf<String>* aliases);

    CARAPI NewEncoder(
        /* [out] */ ICharsetEncoder** charsetEncoder);

    CARAPI NewDecoder(
        /* [out] */ ICharsetDecoder** charsetDecoder);

    CARAPI Contains(
        /* [in] */ ICharset* charset,
        /* [out] */ Boolean* result);

    CARAPI GetName(
        /* [out] */ String* name);

private:
    String mIcuCanonicalName;
};

} // namespace Charset
} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CHARSETICU_H__