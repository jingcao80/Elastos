
#ifndef __ELASTOS_NET_DEFAULTFILENAMEMAP_H__
#define __ELASTOS_NET_DEFAULTFILENAMEMAP_H__

#include "Elastos.CoreLibrary.Net.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Net {

class DefaultFileNameMap
    : public Object
    , public IFileNameMap
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetContentTypeFor(
        /* [in] */ const String& filename,
        /* [out] */ String* type);
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_DEFAULTFILENAMEMAP_H__
