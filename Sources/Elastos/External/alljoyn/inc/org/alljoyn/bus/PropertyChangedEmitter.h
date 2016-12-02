
#ifndef __ORG_ALLJOYN_BUS_PROPERTY_CHANGED_EMITTER_H__
#define __ORG_ALLJOYN_BUS_PROPERTY_CHANGED_EMITTER_H__

#include "_Org.Alljoyn.Bus.h"
#include "org/alljoyn/bus/SignalEmitter.h"

using Org::Alljoyn::Bus::Ifaces::IProperties;

namespace Org {
namespace Alljoyn {
namespace Bus {

/**
 * A helper proxy used by BusObjects to send property change notifications.  A PropertyChangedlEmitter
 * instance can be used to send any signal from a given AllJoyn interface.
 */
class PropertyChangedEmitter
    : public SignalEmitter
    , public IPropertyChangedEmitter
{
public:
    CAR_INTERFACE_DECL()

    PropertyChangedEmitter();

    virtual ~PropertyChangedEmitter();

    CARAPI constructor(
        /* [in] */ IBusObject* source,
        /* [in] */ Int32 sessionId,
        /* [in] */ GlobalBroadcast globalBroadcast);

    CARAPI constructor(
        /* [in] */ IBusObject* source,
        /* [in] */ Int32 sessionId);

    CARAPI constructor(
        /* [in] */ IBusObject* source);

    /**
     * Sends the PropertyChanged signal.
     *
     * @param ifaceName name of the interface the property belongs to
     * @param propertyName name of the property that is changed
     * @param newValue Variant containing the new property value
     * @throws BusException indicating failure to send the PropertyChanged signal
     */
    CARAPI PropertyChanged(
        /* [in] */ String ifaceName,
        /* [in] */ String propertyName,
        /* [in] */ IVariant* newValue);

    /**
     * Sends the PropertiesChanged signal
     *
     * @param iface the BusInterface the property belongs to
     * @param properties list of properties that were changed
     * @throws BusException indicating failure to send the PropertiesChanged signal
     */
    CARAPI PropertiesChanged(
        /* [in] */ IInterfaceInfo* iface,
        /* [in] */ ArrayOf<String>* properties);

private:
    AutoPtr<IProperties> mProps;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_PROPERTY_CHANGED_EMITTER_H__
