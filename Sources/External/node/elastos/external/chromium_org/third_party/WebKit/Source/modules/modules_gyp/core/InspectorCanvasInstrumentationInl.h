// Code generated from InspectorInstrumentation.idl

#ifndef InspectorCanvasInstrumentationInl_h
#define InspectorCanvasInstrumentationInl_h

#include "bindings/v8/ScriptValue.h"
#include "core/inspector/InspectorInstrumentation.h"

namespace WebCore {

namespace InspectorInstrumentation {

ScriptValue wrapCanvas2DRenderingContextForInstrumentationImpl(InstrumentingAgents*, const ScriptValue&);

inline ScriptValue wrapCanvas2DRenderingContextForInstrumentation(Document* paramDocument, const ScriptValue& paramScriptValue)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        return wrapCanvas2DRenderingContextForInstrumentationImpl(agents, paramScriptValue);
    return ScriptValue();
}

ScriptValue wrapWebGLRenderingContextForInstrumentationImpl(InstrumentingAgents*, const ScriptValue&);

inline ScriptValue wrapWebGLRenderingContextForInstrumentation(Document* paramDocument, const ScriptValue& paramScriptValue)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        return wrapWebGLRenderingContextForInstrumentationImpl(agents, paramScriptValue);
    return ScriptValue();
}

} // namespace InspectorInstrumentation

} // namespace WebCore

#endif // !defined(InspectorCanvasInstrumentationInl_h)
