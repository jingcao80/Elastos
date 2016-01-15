#ifndef __ELASTOS_IO_CHARSET_SPI_ELASTOS_IO_CHARSETPROVIDER_H__
#define __ELASTOS_IO_CHARSET_SPI_ELASTOS_IO_CHARSETPROVIDER_H__

#include "Elastos.CoreLibrary.IO.h"
#include <elastos/core/Object.h>

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
