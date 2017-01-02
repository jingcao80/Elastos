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

#ifndef __LIBCORE_ICU_NATIVEIDN_H__
#define __LIBCORE_ICU_NATIVEIDN_H__

#include <elastos.h>
#include <elatypes.h>

using namespace Elastos;

namespace Libcore {
namespace ICU {

class NativeIDN
{
public:
    static CARAPI_(String) ToASCII(
        /* [in] */ const String& s,
        /* [in] */ Int32 flags);

    static CARAPI_(String) ToUnicode(
        /* [in] */ const String& s,
        /* [in] */ Int32 flags);

private:
    NativeIDN();

    static CARAPI Convert(
        /* [in] */ const String& s,
        /* [in] */ Int32 flags,
        /* [in] */ Boolean toAscii,
        /* [out] */ String* result);

    static CARAPI ConvertImpl(
        /* [in] */ const String& s,
        /* [in] */ Int32 flags,
        /* [in] */ Boolean toAscii,
        /* [out] */ String* result);
};

} // namespace ICU
} // namespace Libcore

#endif //__LIBCORE_ICU_NATIVEIDN_H__
