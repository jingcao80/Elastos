
#ifndef __ELASTOS_IO_CDATAINPUTSTREAMHELPER_H__
#define __ELASTOS_IO_CDATAINPUTSTREAMHELPER_H__

#include "_Elastos_IO_CDataInputStreamHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace IO {

CarClass(CDataInputStreamHelper)
    , public Singleton
    , public IDataInputStreamHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI ReadUTF(
        /* [in] */ IDataInput* dataInput,
        /* [out] */ String* str);
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CDATAINPUTSTREAMHELPER_H__
