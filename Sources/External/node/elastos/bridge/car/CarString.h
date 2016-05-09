
#ifndef __CARSTRING_H__
#define __CARSTRING_H__

#if ENABLE(CAR_BRIDGE)

#include <elastos.h>
#include <wtf/text/WTFString.h>

#include "CarStringV8.h"

namespace JSC {
namespace Bindings {

class CarString
{
public:
    CarString()
    {
        m_impl.init();
    }

    CarString(const Elastos::String& s)
    {
        m_impl.init(s);
    }

    const char* utf8() const { return m_impl.utf8(); }
    int length() const { return m_impl.length(); }
    StringImpl* impl() const { return m_impl.impl(); }

private:
    CarStringImpl m_impl;
};

} // namespace Bindings
} // namespace JSC

#endif // ENABLE(CAR_BRIDGE)

#endif //__CARSTRING_H__
