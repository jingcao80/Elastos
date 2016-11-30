
#ifndef __ORG_ALLJOYN_BUS_SIGNATURE_H__
#define __ORG_ALLJOYN_BUS_SIGNATURE_H__

#include <elastos/coredef.h>

namespace Org {
namespace Alljoyn {
namespace Bus {

/**
 * Signature provides static methods for converting between Java and DBus type signatures.
 * This class is used internally.
 */
class Signature
{
public:
    static AutoPtr<ArrayOf<String> > Split(
        /* [in] */ const String& signature);

    /**
     * Compute the DBus type signature of the type.
     *
     * @param type the Java type
     * @param signature the annotated signature for the type
     * @return the DBus type signature
     * @throws AnnotationBusException if the signature cannot be computed
     */
    static CARAPI_(String) TypeSig(
        /* [in] */ IDataTypeInfo* type,
        /* [in] */ const String& signature);

    /**
     * Compute the DBus type signature of the types.
     * @param types The Java types.
     * @param signature The annotated signature for the types.
     */
    static CARAPI_(String) TypeSig(
        /* [in] */ ArrayOf<IParamInfo*>* types,
        /* [in] */ const String& signature);

private:
    Signature();
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_SIGNATURE_H__
