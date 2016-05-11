
#ifndef __ELASTOS_NET_CONTENTHANDLER_H__
#define __ELASTOS_NET_CONTENTHANDLER_H__

#include "Elastos.CoreLibrary.Net.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Net {

class ContentHandler
    : public Object
    , public IContentHandler
{
public:
    CAR_INTERFACE_DECL()

    ContentHandler();

    virtual ~ContentHandler();

    virtual CARAPI GetContent(
        /* [in] */ IURLConnection* uConn,
        /* [out] */ IInterface** obj) = 0;

    virtual CARAPI GetContent(
        /* [in] */ IURLConnection* uConn,
        /* [in] */ ArrayOf<InterfaceID> * types,
        /* [out] */ IInterface** obj);
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CONTENTHANDLER_H__
