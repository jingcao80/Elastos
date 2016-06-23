
#include "org/javia/arity/Declaration.h"

namespace Org {
namespace Javia {
namespace Arity {

static AutoPtr<ArrayOf<String> > InitArgs()
{
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(0);
    return args;
}
const AutoPtr<ArrayOf<String> > Declaration::NO_ARGS = InitArgs();

ECode Declaration::Parse(
    /* [in] */ const String& source,
    /* [in] */ Lexer* lexer,
    /* [in] */ DeclarationParser* declParser)
{
    Int32 equalPos = source.IndexOf('=');
    String decl;

    if (equalPos == -1) {
        decl = String(NULL);
        mExpression = source;
        mName = String(NULL);
        mArgs = NO_ARGS;
        mArity = DeclarationParser::UNKNOWN_ARITY;
    }
    else {
        decl = source.Substring(0, equalPos);
        mExpression = source.Substring(equalPos + 1);
        FAIL_RETURN(lexer->Scan(decl, declParser);
        mName = declParser->mName;
        mArgs = declParser->ArgNames();
        mArity = declParser->mArity;
    }
    return NOERROR;
    /*
    if (arity == DeclarationParser.UNKNOWN_ARITY) {
        args = IMPLICIT_ARGS;
    }
    */
}

} // namespace Arity
} // namespace Javia
} // namespace Org