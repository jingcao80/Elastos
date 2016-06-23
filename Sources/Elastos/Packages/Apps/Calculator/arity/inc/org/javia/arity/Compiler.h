
#ifndef _ORG_JAVIA_ARITY_COMPILER_H__
#define _ORG_JAVIA_ARITY_COMPILER_H__

#include "org/javia/arity/Lexer.h"
#include "org/javia/arity/RPN.h"
#include "org/javia/arity/DeclarationParser.h"
#include "org/javia/arity/OptCodeGen.h"
#include "org/javia/arity/Declaration.h"

using Elastos::Utility::IStack;

namespace Org {
namespace Javia {
namespace Arity {

/**
   Compiles a textual arithmetic expression to a {@link Function}.<p>
*/
class Compiler : public Object
{
public:
    Compiler();

    CARAPI CompileSimple(
        /* [in] */ CSymbols* symbols,
        /* [in] */ const String& expression,
        /* [out] */ IFunction** func);

    CARAPI Compile(
        /* [in] */ CSymbols* symbols,
        /* [in] */ const String& source,
        /* [out] */ IFunction** func);

    CARAPI CompileWithName(
        /* [in] */ CSymbols* symbols,
        /* [in] */ const String& source,
        /* [out] */ IFunctionAndName** func);

private::
    AutoPtr<Lexer> mLexer;
    AutoPtr<RPN> mRpn;
    AutoPtr<DeclarationParser> mDeclParser;
    AutoPtr<OptCodeGen> mCodeGen;
    AutoPtr<SimpleCodeGen> mSimpleCodeGen;
    AutoPtr<Declaration> mDecl;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_COMPILER_H__