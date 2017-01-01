//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
        /* [in] */ const String& ifaceName,
        /* [in] */ const String& propertyName,
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
