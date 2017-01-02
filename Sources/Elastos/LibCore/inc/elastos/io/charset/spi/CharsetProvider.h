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

#ifndef __ELASTOS_IO_CHARSET_SPI_ELASTOS_IO_CHARSETPROVIDER_H__
#define __ELASTOS_IO_CHARSET_SPI_ELASTOS_IO_CHARSETPROVIDER_H__

#include "Elastos.CoreLibrary.IO.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace IO {
namespace Charset {
namespace Spi {

class CharsetProvider
    : public Object
    , public ICharsetProvider
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Constructor for subclassing with concrete types.
     */
    CharsetProvider();

    virtual ~CharsetProvider();

};

} // namespace Spi
} // namespace CharSet
} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CHARSET_SPI_ELASTOS_IO_CHARSETPROVIDER_H__
