
#ifndef _ORG_JAVIA_ARITY_DECLARATION_H__
#define _ORG_JAVIA_ARITY_DECLARATION_H__

#include "org/javia/arity/Lexer.h"
#include "org/javia/arity/DeclarationParser.h"

namespace Org {
namespace Javia {
namespace Arity {

class Declaration : public Object
{
public:
    Declaration()
        : mArity(0)
    {}

    CARAPI Parse(
        /* [in] */ const String& source,
        /* [in] */ Lexer* lexer,
        /* [in] */ DeclarationParser* declParser);

public:
    String mName;
    AutoPtr<ArrayOf<String> > mArgs;
    Int32 mArity;
    String mExpression;

private:
    static const AutoPtr<ArrayOf<String> > NO_ARGS;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_DECLARATION_H__