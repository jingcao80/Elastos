
#ifndef __ORG_ALLJOYN_BUS_NATIVEABOUTOBJECT_H__
#define __ORG_ALLJOYN_BUS_NATIVEABOUTOBJECT_H__

#include "_Org.Alljoyn.Bus.h"
#include <alljoyn/AboutObj.h>
#include <alljoyn/AboutDataListener.h>

namespace Org {
namespace Alljoyn {
namespace Bus {

class NativeAboutObject : public ajn::AboutObj, public ajn::AboutDataListener
{

};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVEABOUTOBJECT_H__
