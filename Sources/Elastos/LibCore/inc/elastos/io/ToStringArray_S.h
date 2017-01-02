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

#ifndef __ELASTOS_IO_TOSTRINGARRAY_S_H__
#define __ELASTOS_IO_TOSTRINGARRAY_S_H__

#include <elquintet.h>
#include <elastos/utility/etl/Vector.h>

using Elastos::Utility::Etl::Vector;

namespace Elastos {
namespace IO {

template <typename Counter, typename Getter>
AutoPtr< ArrayOf<String> > ToStringArray(Counter* counter, Getter* getter)
{
    size_t count = (*counter)();
    AutoPtr< ArrayOf<String> > result = ArrayOf<String>::Alloc(count);
    if (result == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < count; ++i) {
        String s((*getter)(i));
        (*result)[i] = s;
    }
    return result;
}

AutoPtr< ArrayOf<String> > ToStringArray(const Vector<String>& strings);

AutoPtr< ArrayOf<String> > ToStringArray(const char* const* strings);
}
}
#endif //__TOSTRINGARRAY_H__
