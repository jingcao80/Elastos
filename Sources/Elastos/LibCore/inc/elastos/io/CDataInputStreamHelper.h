
#ifndef __ELASTOS_IO_CDATAINPUTSTREAMHELPER_H__
#define __ELASTOS_IO_CDATAINPUTSTREAMHELPER_H__

#include "_Elastos_IO_CDataInputStreamHelper.h"

namespace Elastos {
namespace IO {

CarClass(CDataInputStreamHelper)
{
public:
    CARAPI ReadUTF(
        /* [in] */ IDataInput* dataInput,
        /* [out] */ String* str);
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CDATAINPUTSTREAMHELPER_H__
