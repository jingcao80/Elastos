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

#ifndef __ELASTOS_DROID_GRAPHICS_TEMPORARYBUFFER_H__
#define __ELASTOS_DROID_GRAPHICS_TEMPORARYBUFFER_H__

#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Graphics {

class TemporaryBuffer
{
public:
    static CARAPI_(AutoPtr< ArrayOf<Char32> >) Obtain(
       /* [in] */ Int32 len);

    static CARAPI_(void) Recycle(
       /*  [in] */ ArrayOf<Char32>* temp);

private:
    static AutoPtr< ArrayOf<Char32> > sTemp;
    static Object sObject;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __CTEMPORARYBUFFER_H__
