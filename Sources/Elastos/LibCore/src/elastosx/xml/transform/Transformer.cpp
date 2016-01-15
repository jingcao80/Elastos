
#include "Transformer.h"

namespace Elastosx {
namespace Xml {
namespace Transform {

CAR_INTERFACE_IMPL(Transformer, Object, ITransformer);
Transformer::Transformer()
{}

Transformer::~Transformer()
{}

ECode Transformer::Reset()
{
    assert(0);
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
    // implementors should override this method
    // throw new UnsupportedOperationException(
    //     "This Transformer, \"" + this.getClass().getName() + "\", does not support the reset functionality."
    //     + "  Specification \"" + this.getClass().getPackage().getSpecificationTitle() + "\""
    //     + " version \"" + this.getClass().getPackage().getSpecificationVersion() + "\""
    //     );
}

} // namespace Transform
} // namespace Xml
} // namespace Elastosx
