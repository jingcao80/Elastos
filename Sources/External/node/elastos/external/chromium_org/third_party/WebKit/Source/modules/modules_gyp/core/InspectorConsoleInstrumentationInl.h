// Code generated from InspectorInstrumentation.idl

#ifndef InspectorConsoleInstrumentationInl_h
#define InspectorConsoleInstrumentationInl_h

#include "core/inspector/InspectorInstrumentation.h"
#include "core/inspector/ScriptArguments.h"
#include "core/inspector/ScriptCallStack.h"

namespace WebCore {

namespace InspectorInstrumentation {

void addMessageToConsoleImpl(InstrumentingAgents*, MessageSource, MessageType, MessageLevel, const String&, PassRefPtrWillBeRawPtr<ScriptCallStack>, unsigned long);

inline void addMessageToConsole(ExecutionContext* context, MessageSource source, MessageType type, MessageLevel level, const String& message, PassRefPtrWillBeRawPtr<ScriptCallStack> callStack, unsigned long requestIdentifier = 0)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(context))
        addMessageToConsoleImpl(agents, source, type, level, message, callStack, requestIdentifier);
}

void addMessageToConsoleImpl(InstrumentingAgents*, MessageSource, MessageType, MessageLevel, const String&, const String&, unsigned, unsigned, ScriptState*, unsigned long);

inline void addMessageToConsole(ExecutionContext* context, MessageSource source, MessageType type, MessageLevel level, const String& message, const String& scriptId, unsigned lineNumber, unsigned columnNumber, ScriptState* state, unsigned long requestIdentifier = 0)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(context))
        addMessageToConsoleImpl(agents, source, type, level, message, scriptId, lineNumber, columnNumber, state, requestIdentifier);
}

void addMessageToConsoleImpl(InstrumentingAgents*, MessageSource, MessageType, MessageLevel, const String&, ScriptState*, PassRefPtrWillBeRawPtr<ScriptArguments>, unsigned long);

inline void addMessageToConsole(ExecutionContext* context, MessageSource source, MessageType type, MessageLevel level, const String& message, ScriptState* state, PassRefPtrWillBeRawPtr<ScriptArguments> arguments, unsigned long requestIdentifier = 0)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(context))
        addMessageToConsoleImpl(agents, source, type, level, message, state, arguments, requestIdentifier);
}

void consoleCountImpl(InstrumentingAgents*, ScriptState*, PassRefPtrWillBeRawPtr<ScriptArguments>);

inline void consoleCount(ExecutionContext* context, ScriptState* state, PassRefPtrWillBeRawPtr<ScriptArguments> arguments)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(context))
        consoleCountImpl(agents, state, arguments);
}

void consoleTimeImpl(InstrumentingAgents*, ExecutionContext*, const String&);

inline void consoleTime(ExecutionContext* context, const String& title)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(context))
        consoleTimeImpl(agents, context, title);
}

void consoleTimeEndImpl(InstrumentingAgents*, ExecutionContext*, const String&, ScriptState*);

inline void consoleTimeEnd(ExecutionContext* context, const String& title, ScriptState* state)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(context))
        consoleTimeEndImpl(agents, context, title, state);
}

void consoleTimeStampImpl(InstrumentingAgents*, ExecutionContext*, const String&);

inline void consoleTimeStamp(ExecutionContext* context, const String& title)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(context))
        consoleTimeStampImpl(agents, context, title);
}

void consoleTimelineImpl(InstrumentingAgents*, ExecutionContext*, const String&, ScriptState*);

inline void consoleTimeline(ExecutionContext* context, const String& title, ScriptState* state)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(context))
        consoleTimelineImpl(agents, context, title, state);
}

void consoleTimelineEndImpl(InstrumentingAgents*, ExecutionContext*, const String&, ScriptState*);

inline void consoleTimelineEnd(ExecutionContext* context, const String& title, ScriptState* state)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(context))
        consoleTimelineEndImpl(agents, context, title, state);
}

void consoleProfileImpl(InstrumentingAgents*, const String&, ScriptState*);

inline void consoleProfile(ExecutionContext* context, const String& title, ScriptState* state)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(context))
        consoleProfileImpl(agents, title, state);
}

void consoleProfileEndImpl(InstrumentingAgents*, const String&, ScriptState*);

inline void consoleProfileEnd(ExecutionContext* context, const String& title, ScriptState* state)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(context))
        consoleProfileEndImpl(agents, title, state);
}

} // namespace InspectorInstrumentation

} // namespace WebCore

#endif // !defined(InspectorConsoleInstrumentationInl_h)
