// Code generated from InspectorInstrumentation.idl

#ifndef InspectorOverridesInl_h
#define InspectorOverridesInl_h

#include "core/inspector/InspectorInstrumentation.h"

namespace WebCore {

namespace InspectorInstrumentation {

bool forcePseudoStateImpl(InstrumentingAgents*, Element*, CSSSelector::PseudoType);

inline bool forcePseudoState(Element* element, CSSSelector::PseudoType pseudoState)
{   
    FAST_RETURN_IF_NO_FRONTENDS(false);
    if (InstrumentingAgents* agents = instrumentingAgentsFor(element))
        return forcePseudoStateImpl(agents, element, pseudoState);
    return false;
}

bool shouldPauseDedicatedWorkerOnStartImpl(InstrumentingAgents*);

inline bool shouldPauseDedicatedWorkerOnStart(ExecutionContext* context)
{   
    FAST_RETURN_IF_NO_FRONTENDS(false);
    if (InstrumentingAgents* agents = instrumentingAgentsFor(context))
        return shouldPauseDedicatedWorkerOnStartImpl(agents);
    return false;
}

} // namespace InspectorInstrumentation

} // namespace WebCore

#endif // !defined(InspectorOverridesInl_h)
