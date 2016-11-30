
#ifndef __ORG_ALLJOYN_BUS_TRANSLATOR_H__
#define __ORG_ALLJOYN_BUS_TRANSLATOR_H__

#include "_Org.Alljoyn.Bus.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Alljoyn {
namespace Bus {

class Translator
    : public Object
    , public ITranslator
{
public:
    CAR_INTERFACE_DECL();

    /**
     * Destroy native resources held by objects of this class.
     */
    Translator();

    /**
     * Destroy native resources held by objects of this class.
     */
    virtual ~Translator();

private:
    /**
     * Create any native resources held by objects of this class.  Specifically,
     * we allocate a C++ counterpart of this Translator object.
     */
    CARAPI_(void) Create();

    /**
     * Release any native resources held by objects of this class.
     * Specifically, we may delete a C++ counterpart of this Translator object.
     */
    CARAPI_(void) Destroy();

private:
    friend class InterfaceDescription;
    /**
     * The opaque pointer to the underlying C++ object which is actually tied
     * to the AllJoyn code.
     */
    Int64 mHandle;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_TRANSLATOR_H__
