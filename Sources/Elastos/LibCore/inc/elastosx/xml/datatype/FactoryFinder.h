#ifndef __ELASTOSX_XML_DATATYPE_FACTORYFINDER_H__
#define __ELASTOSX_XML_DATATYPE_FACTORYFINDER_H__

#include "Object.h"

using Elastos::Core::Object;
using Elastos::Core::IClassLoader;

namespace Elastosx {
namespace Xml {
namespace Datatype {

class FactoryFinder
{
public:
    static CARAPI NewInstance(
        /* [in] */ String clsName,
        /* [in] */ IClassLoader* loader,
        /* [out] */ IInterface** obj);

    static CARAPI Find(
        /* [in] */ String factoryId,
        /* [in] */ String fallbackClassName,
        /* [out] */ IInterface** obj);

private:
    FactoryFinder();

    static CARAPI FindClassLoader(
        /* [in] */ String clsName,
        /* [out] */ IClassLoader** pLoader);

private:
    /** <p>Cache properties for performance.</p> */
    // static AutoPtr<IProperties> sCacheProps;

    /** <p>First time requires initialization overhead.</p> */
    static Boolean sFirstTime;

    /** Default columns per line. */
    static const Int32 DEFAULT_LINE_LENGTH;
};

} // namespace Datatype
} // namespace Xml
} // namespace Elastosx

#endif
