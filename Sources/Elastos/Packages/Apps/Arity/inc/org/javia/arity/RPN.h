
#ifndef _ORG_JAVIA_ARITY_RPN_H__
#define _ORG_JAVIA_ARITY_RPN_H__

#include "org/javia/arity/TokenConsumer.h"
#include "org/javia/arity/Token.h"

using Elastos::Utility::IStack;

namespace Org {
namespace Javia {
namespace Arity {

/* Reverse Polish Notation
   reads tokens in normal infix order (e.g.: 1 + 2)
   and outputs them in Reverse Polish order (e.g.: 1 2 +).
   See Dijkstra's Shunting Yard algorithm:
   http://en.wikipedia.org/wiki/Shunting_yard_algorithm
 */
class RPN : public TokenConsumer
{
public:
    RPN();

    CARAPI_(void) SetConsumer(
        /* [in] */ TokenConsumer* consumer);

    CARAPI_(void) Start();

    static CARAPI_(Boolean) IsOperand(
        /* [in] */ Int32 id);

    CARAPI Push(
        /* [in] */ Token* token);

private:
    CARAPI_(AutoPtr<Token>) Top();

    CARAPI PopHigher(
        /* [in] */ Int32 priority);

public:
    AutoPtr<IStack> mStack;
    Int32 mPrevTokenId;
    AutoPtr<TokenConsumer> mConsumer;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_RPN_H__