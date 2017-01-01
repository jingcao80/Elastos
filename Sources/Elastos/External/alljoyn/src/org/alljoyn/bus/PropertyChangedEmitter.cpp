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

#include "org/alljoyn/bus/PropertyChangedEmitter.h"
#include "org/alljoyn/bus/InterfaceDescription.h"
#include "org/alljoyn/bus/Globals.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IMap;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

static const String TAG("PropertyChangedEmitter");

CAR_INTERFACE_IMPL(PropertyChangedEmitter, SignalEmitter, IPropertyChangedEmitter)

PropertyChangedEmitter::PropertyChangedEmitter()
{}

PropertyChangedEmitter::~PropertyChangedEmitter()
{}

ECode PropertyChangedEmitter::constructor(
    /* [in] */ IBusObject* source,
    /* [in] */ Int32 sessionId,
    /* [in] */ GlobalBroadcast globalBroadcast)
{
    SignalEmitter::constructor(source, String(NULL), sessionId, globalBroadcast);

    AutoPtr<IInterface> obj;
    GetInterface(Org::Alljoyn::Bus::Ifaces::EIID_IProperties, (IInterface**)&obj);
    mProps = Org::Alljoyn::Bus::Ifaces::IProperties::Probe(obj);
    assert(mProps != NULL);
    return NOERROR;
}

ECode PropertyChangedEmitter::constructor(
    /* [in] */ IBusObject* source,
    /* [in] */ Int32 sessionId)
{
    return constructor(source, sessionId, GlobalBroadcast_Off);
}

ECode PropertyChangedEmitter::constructor(
    /* [in] */ IBusObject* source)
{
    return constructor(source, IBusAttachment::SESSION_ID_ANY);
}

ECode PropertyChangedEmitter::PropertyChanged(
    /* [in] */ const String& ifaceName,
    /* [in] */ const String& propertyName,
    /* [in] */ IVariant* newValue)
{
    AutoPtr<IMap> propsChanged; // HashMap<String, Variant>();
    CHashMap::New((IMap**)&propsChanged);

    AutoPtr<ArrayOf<String> > invalidatedProps;
    if (newValue == NULL) {
        invalidatedProps = ArrayOf<String>::Alloc(1);
        invalidatedProps->Set(0, propertyName);
    }
    else {
        invalidatedProps = ArrayOf<String>::Alloc(0);
        propsChanged->Put(CoreUtils::Convert(propertyName), newValue);
    }

    return mProps->PropertiesChanged(ifaceName, propsChanged, invalidatedProps);
}

static AutoPtr<IMethodInfo> FindMethodByName(
    /* [in] */ ArrayOf<IMethodInfo*>* methodInfos,
    /* [in] */ const String& methodName)
{
    for (Int32 i = 0; i < methodInfos->GetLength(); ++i) {
        String name;
        (*methodInfos)[i]->GetName(&name);
        if (methodName.Equals(name)) {
            return (*methodInfos)[i];
        }
    }

    return NULL;
}

ECode PropertyChangedEmitter::PropertiesChanged(
    /* [in] */ IInterfaceInfo* iface,
    /* [in] */ ArrayOf<String>* properties)
{
    String ifaceName = InterfaceDescription::GetName(iface);

    AutoPtr<IMap> changedProps; // HashMap<String, Variant>();
    CHashMap::New((IMap**)&changedProps);

    Int32 count = 0;
    iface->GetMethodCount(&count);
    AutoPtr< ArrayOf<IMethodInfo*> > methodInfos = ArrayOf<IMethodInfo*>::Alloc(count);
    iface->GetAllMethodInfos(methodInfos);

    List<String> invalidatedProps;

    for (Int32 i = 0; i < properties->GetLength(); ++i) {
        String propName = (*properties)[i];
        StringBuilder sb("Get");
        sb += propName;
        String methodName(sb.ToString());

        AutoPtr<IMethodInfo> m = FindMethodByName(methodInfos, methodName);
        if (m == NULL) {
            Logger::E(TAG, "Not property with name %s found", propName.string());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        assert(0 && "TODO");
        AutoPtr<IAnnotationInfo> busPropertyAnn;
        m->GetAnnotation(Globals::Annotation_BusProperty, (IAnnotationInfo**)&busPropertyAnn);
        if (busPropertyAnn != NULL) {
            // need to emit
            AutoPtr<IAnnotationInfo> bas;
            m->GetAnnotation(Globals::Annotation_BusAnnotations, (IAnnotationInfo**)&bas);
            if (bas != NULL) {
                String value;
                AutoPtr< ArrayOf<String> > keys;
                bas->GetKeys((ArrayOf<String>**)&keys);
                for (Int32 i = 0; i < keys->GetLength(); ++i) {
                    if ((*keys)[i].Equals("Org.Freedesktop.DBus.Property.EmitsChangedSignal")) {
                        bas->GetValue((*keys)[i], &value);
                        if (value.Equals("true")) {
                            // Object o;
                            // try {
                            //     o = m.invoke(mSource);
                            // } catch (Exception ex) {
                            //     throw new BusException("can't get value of property " + propName, ex);
                            // }
                            AutoPtr<IVariant> v;
                            // if (busPropertyAnn.signature() != null && !busPropertyAnn.signature().isEmpty()) {
                            //     v = new Variant(o, busPropertyAnn.signature());
                            // }
                            // else {
                            //     v = new Variant(o);
                            // }
                            changedProps->Put(CoreUtils::Convert(propName), v);
                        }
                        else if (value.Equals("invalidates")) {
                            invalidatedProps.PushBack(propName);
                        }
                    }
                }
            }
        }
    }

    AutoPtr<ArrayOf<String> > propArray = ArrayOf<String>::Alloc(invalidatedProps.GetSize());
    List<String>::Iterator it;
    Int32 i = 0;
    for (it == invalidatedProps.Begin(); it != invalidatedProps.End(); ++it) {
        propArray->Set(i++, *it);
    }
    mProps->PropertiesChanged(ifaceName, changedProps, propArray);
    return NOERROR;
}


} // namespace Bus
} // namespace Alljoyn
} // namespace Org