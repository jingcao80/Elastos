
#ifndef _ORG_JAVIA_ARITY_DECLARATIONPARSER_H__
#define _ORG_JAVIA_ARITY_DECLARATIONPARSER_H__

#include "org/javia/arity/TokenConsumer.h"

using Elastos::Utility::IVector;

namespace Org {
namespace Javia {
namespace Arity {

class DeclarationParser : public TokenConsumer
{
public:
    DeclarationParser();

    CARAPI_(void) Start();

    CARAPI Push(
        /* [in] */ Token* token);

    CARAPI_(AutoPtr<ArrayOf<String> >) ArgNames();

public:
    static const AutoPtr<ArrayOf<String> > NO_ARGS;
    static const Int32 UNKNOWN_ARITY = -2;
    static const Int32 MAX_ARITY = 5;

    String mName;
    Int32 mArity;
    AutoPtr<IVector> mArgs;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_DECLARATIONPARSER_H__