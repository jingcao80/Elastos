// Code generated from InspectorInstrumentation.idl

#ifndef InstrumentingAgentsInl_h
#define InstrumentingAgentsInl_h

#include "wtf/FastAllocBase.h"
#include "wtf/Noncopyable.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"

namespace WebCore {

class InspectorApplicationCacheAgent;
class InspectorCSSAgent;
class InspectorCanvasAgent;
class InspectorConsoleAgent;
class InspectorDOMAgent;
class InspectorDOMDebuggerAgent;
class InspectorDOMStorageAgent;
class InspectorDebuggerAgent;
class InspectorInspectorAgent;
class InspectorLayerTreeAgent;
class InspectorPageAgent;
class InspectorProfilerAgent;
class InspectorResourceAgent;
class InspectorTimelineAgent;
class InspectorWorkerAgent;
class PageDebuggerAgent;
class PageRuntimeAgent;
class WorkerRuntimeAgent;

class InstrumentingAgents : public RefCounted<InstrumentingAgents> {
    WTF_MAKE_NONCOPYABLE(InstrumentingAgents);
    WTF_MAKE_FAST_ALLOCATED;
public:
    static PassRefPtr<InstrumentingAgents> create()
    {
        return adoptRef(new InstrumentingAgents());
    }
    ~InstrumentingAgents() { }
    void reset();


    InspectorApplicationCacheAgent* inspectorApplicationCacheAgent() const { return m_inspectorApplicationCacheAgent; }
    void setInspectorApplicationCacheAgent(InspectorApplicationCacheAgent* agent) { m_inspectorApplicationCacheAgent = agent; }

    InspectorCSSAgent* inspectorCSSAgent() const { return m_inspectorCSSAgent; }
    void setInspectorCSSAgent(InspectorCSSAgent* agent) { m_inspectorCSSAgent = agent; }

    InspectorCanvasAgent* inspectorCanvasAgent() const { return m_inspectorCanvasAgent; }
    void setInspectorCanvasAgent(InspectorCanvasAgent* agent) { m_inspectorCanvasAgent = agent; }

    InspectorConsoleAgent* inspectorConsoleAgent() const { return m_inspectorConsoleAgent; }
    void setInspectorConsoleAgent(InspectorConsoleAgent* agent) { m_inspectorConsoleAgent = agent; }

    InspectorDOMAgent* inspectorDOMAgent() const { return m_inspectorDOMAgent; }
    void setInspectorDOMAgent(InspectorDOMAgent* agent) { m_inspectorDOMAgent = agent; }

    InspectorDOMDebuggerAgent* inspectorDOMDebuggerAgent() const { return m_inspectorDOMDebuggerAgent; }
    void setInspectorDOMDebuggerAgent(InspectorDOMDebuggerAgent* agent) { m_inspectorDOMDebuggerAgent = agent; }

    InspectorDOMStorageAgent* inspectorDOMStorageAgent() const { return m_inspectorDOMStorageAgent; }
    void setInspectorDOMStorageAgent(InspectorDOMStorageAgent* agent) { m_inspectorDOMStorageAgent = agent; }

    InspectorDebuggerAgent* inspectorDebuggerAgent() const { return m_inspectorDebuggerAgent; }
    void setInspectorDebuggerAgent(InspectorDebuggerAgent* agent) { m_inspectorDebuggerAgent = agent; }

    InspectorInspectorAgent* inspectorInspectorAgent() const { return m_inspectorInspectorAgent; }
    void setInspectorInspectorAgent(InspectorInspectorAgent* agent) { m_inspectorInspectorAgent = agent; }

    InspectorLayerTreeAgent* inspectorLayerTreeAgent() const { return m_inspectorLayerTreeAgent; }
    void setInspectorLayerTreeAgent(InspectorLayerTreeAgent* agent) { m_inspectorLayerTreeAgent = agent; }

    InspectorPageAgent* inspectorPageAgent() const { return m_inspectorPageAgent; }
    void setInspectorPageAgent(InspectorPageAgent* agent) { m_inspectorPageAgent = agent; }

    InspectorProfilerAgent* inspectorProfilerAgent() const { return m_inspectorProfilerAgent; }
    void setInspectorProfilerAgent(InspectorProfilerAgent* agent) { m_inspectorProfilerAgent = agent; }

    InspectorResourceAgent* inspectorResourceAgent() const { return m_inspectorResourceAgent; }
    void setInspectorResourceAgent(InspectorResourceAgent* agent) { m_inspectorResourceAgent = agent; }

    InspectorTimelineAgent* inspectorTimelineAgent() const { return m_inspectorTimelineAgent; }
    void setInspectorTimelineAgent(InspectorTimelineAgent* agent) { m_inspectorTimelineAgent = agent; }

    InspectorWorkerAgent* inspectorWorkerAgent() const { return m_inspectorWorkerAgent; }
    void setInspectorWorkerAgent(InspectorWorkerAgent* agent) { m_inspectorWorkerAgent = agent; }

    PageDebuggerAgent* pageDebuggerAgent() const { return m_pageDebuggerAgent; }
    void setPageDebuggerAgent(PageDebuggerAgent* agent) { m_pageDebuggerAgent = agent; }

    PageRuntimeAgent* pageRuntimeAgent() const { return m_pageRuntimeAgent; }
    void setPageRuntimeAgent(PageRuntimeAgent* agent) { m_pageRuntimeAgent = agent; }

    WorkerRuntimeAgent* workerRuntimeAgent() const { return m_workerRuntimeAgent; }
    void setWorkerRuntimeAgent(WorkerRuntimeAgent* agent) { m_workerRuntimeAgent = agent; }

private:
    InstrumentingAgents();

    InspectorApplicationCacheAgent* m_inspectorApplicationCacheAgent;
    InspectorCSSAgent* m_inspectorCSSAgent;
    InspectorCanvasAgent* m_inspectorCanvasAgent;
    InspectorConsoleAgent* m_inspectorConsoleAgent;
    InspectorDOMAgent* m_inspectorDOMAgent;
    InspectorDOMDebuggerAgent* m_inspectorDOMDebuggerAgent;
    InspectorDOMStorageAgent* m_inspectorDOMStorageAgent;
    InspectorDebuggerAgent* m_inspectorDebuggerAgent;
    InspectorInspectorAgent* m_inspectorInspectorAgent;
    InspectorLayerTreeAgent* m_inspectorLayerTreeAgent;
    InspectorPageAgent* m_inspectorPageAgent;
    InspectorProfilerAgent* m_inspectorProfilerAgent;
    InspectorResourceAgent* m_inspectorResourceAgent;
    InspectorTimelineAgent* m_inspectorTimelineAgent;
    InspectorWorkerAgent* m_inspectorWorkerAgent;
    PageDebuggerAgent* m_pageDebuggerAgent;
    PageRuntimeAgent* m_pageRuntimeAgent;
    WorkerRuntimeAgent* m_workerRuntimeAgent;
};

}

#endif // !defined(InstrumentingAgentsInl_h)
