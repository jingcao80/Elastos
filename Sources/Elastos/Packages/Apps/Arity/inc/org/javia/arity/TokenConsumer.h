
#ifndef _ORG_JAVIA_ARITY_TOKENCONSUMER_H__
#define _ORG_JAVIA_ARITY_TOKENCONSUMER_H__

#include "org/javia/arity/Token.h"

namespace Org {
namespace Javia {
namespace Arity {

class TokenConsumer : public Object
{
public:
    virtual ~TokenConsumer() {}

    virtual CARAPI_(void) Start() {}

    virtual CARAPI Push(
        /* [in] */ Token* token) = 0;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_TOKENCONSUMER_H__