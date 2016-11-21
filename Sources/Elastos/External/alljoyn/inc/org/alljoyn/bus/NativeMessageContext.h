
#ifndef __ORG_ALLJOYN_BUS_NATIVEMESSAGECONTEXT_H__
#define __ORG_ALLJOYN_BUS_NATIVEMESSAGECONTEXT_H__

#include "_Org.Alljoyn.Bus.h"
#include <alljoyn/Message.h>

namespace Org {
namespace Alljoyn {
namespace Bus {

class NativeMessageContext
{
public:
    static ajn::Message GetMessage();

    NativeMessageContext(
        /* [in] */ const ajn::Message& msg);

    ~NativeMessageContext();

private:
    NativeMessageContext(const NativeMessageContext& other);
    NativeMessageContext& operator =(const NativeMessageContext& other);
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVEMESSAGECONTEXT_H__
