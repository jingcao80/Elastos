
#ifndef __Elastos_DevSamples_DFObserver_MyAboutData_H__
#define __Elastos_DevSamples_DFObserver_MyAboutData_H__

#include <_Org.Alljoyn.Bus.h>
#include <elastos/core/Object.h>

using Org::Alljoyn::Bus::IAboutDataListener;
using Elastos::Utility::IMap;

namespace Elastos {
namespace DevSamples {
namespace DFObserver {

class MyAboutData
    : public Object
    , public IAboutDataListener
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetAboutData(
        /* [in] */ const String& language,
        /* [out] */ IMap** data);

    CARAPI GetAnnouncedAboutData(
        /* [out] */ IMap** data);
};

} // namespace DFObserver
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_DFObserver_MyAboutData_H__
