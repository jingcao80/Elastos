// Code generated from InspectorInstrumentation.idl

#include "config.h"

#include "InspectorCanvasInstrumentationInl.h"
#include "InspectorConsoleInstrumentationInl.h"
#include "InspectorInstrumentationInl.h"
#include "InspectorOverridesInl.h"
#include "InstrumentingAgentsInl.h"
#include "core/inspector/InspectorApplicationCacheAgent.h"
#include "core/inspector/InspectorCSSAgent.h"
#include "core/inspector/InspectorCanvasAgent.h"
#include "core/inspector/InspectorConsoleAgent.h"
#include "core/inspector/InspectorDOMAgent.h"
#include "core/inspector/InspectorDOMDebuggerAgent.h"
#include "core/inspector/InspectorDOMStorageAgent.h"
#include "core/inspector/InspectorDebuggerAgent.h"
#include "core/inspector/InspectorInspectorAgent.h"
#include "core/inspector/InspectorLayerTreeAgent.h"
#include "core/inspector/InspectorPageAgent.h"
#include "core/inspector/InspectorProfilerAgent.h"
#include "core/inspector/InspectorResourceAgent.h"
#include "core/inspector/InspectorTimelineAgent.h"
#include "core/inspector/InspectorWorkerAgent.h"
#include "core/inspector/PageDebuggerAgent.h"
#include "core/inspector/PageRuntimeAgent.h"
#include "core/inspector/WorkerRuntimeAgent.h"

namespace WebCore {

InstrumentingAgents::InstrumentingAgents()
    : m_inspectorApplicationCacheAgent(0)
    , m_inspectorCSSAgent(0)
    , m_inspectorCanvasAgent(0)
    , m_inspectorConsoleAgent(0)
    , m_inspectorDOMAgent(0)
    , m_inspectorDOMDebuggerAgent(0)
    , m_inspectorDOMStorageAgent(0)
    , m_inspectorDebuggerAgent(0)
    , m_inspectorInspectorAgent(0)
    , m_inspectorLayerTreeAgent(0)
    , m_inspectorPageAgent(0)
    , m_inspectorProfilerAgent(0)
    , m_inspectorResourceAgent(0)
    , m_inspectorTimelineAgent(0)
    , m_inspectorWorkerAgent(0)
    , m_pageDebuggerAgent(0)
    , m_pageRuntimeAgent(0)
    , m_workerRuntimeAgent(0)
{
}

void InstrumentingAgents::reset()
{
    m_inspectorApplicationCacheAgent = 0;
    m_inspectorCSSAgent = 0;
    m_inspectorCanvasAgent = 0;
    m_inspectorConsoleAgent = 0;
    m_inspectorDOMAgent = 0;
    m_inspectorDOMDebuggerAgent = 0;
    m_inspectorDOMStorageAgent = 0;
    m_inspectorDebuggerAgent = 0;
    m_inspectorInspectorAgent = 0;
    m_inspectorLayerTreeAgent = 0;
    m_inspectorPageAgent = 0;
    m_inspectorProfilerAgent = 0;
    m_inspectorResourceAgent = 0;
    m_inspectorTimelineAgent = 0;
    m_inspectorWorkerAgent = 0;
    m_pageDebuggerAgent = 0;
    m_pageRuntimeAgent = 0;
    m_workerRuntimeAgent = 0;
}

namespace InspectorInstrumentation {

void didClearDocumentOfWindowObjectImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame)
{
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->didClearDocumentOfWindowObject(paramLocalFrame);
    if (InspectorInspectorAgent* agent = agents->inspectorInspectorAgent())
        agent->didClearDocumentOfWindowObject(paramLocalFrame);
    if (PageDebuggerAgent* agent = agents->pageDebuggerAgent())
        agent->didClearDocumentOfWindowObject(paramLocalFrame);
    if (PageRuntimeAgent* agent = agents->pageRuntimeAgent())
        agent->didClearDocumentOfWindowObject(paramLocalFrame);
}

void willInsertDOMNodeImpl(InstrumentingAgents* agents, Node* parent)
{
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->willInsertDOMNode(parent);
}

void didInsertDOMNodeImpl(InstrumentingAgents* agents, Node* paramNode)
{
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->didInsertDOMNode(paramNode);
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->didInsertDOMNode(paramNode);
}

void willRemoveDOMNodeImpl(InstrumentingAgents* agents, Node* paramNode)
{
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->willRemoveDOMNode(paramNode);
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->willRemoveDOMNode(paramNode);
}

void willModifyDOMAttrImpl(InstrumentingAgents* agents, Element* paramElement, const AtomicString& oldValue, const AtomicString& newValue)
{
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->willModifyDOMAttr(paramElement, oldValue, newValue);
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->willModifyDOMAttr(paramElement, oldValue, newValue);
}

void didModifyDOMAttrImpl(InstrumentingAgents* agents, Element* paramElement, const AtomicString& name, const AtomicString& value)
{
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->didModifyDOMAttr(paramElement, name, value);
}

void didRemoveDOMAttrImpl(InstrumentingAgents* agents, Element* paramElement, const AtomicString& name)
{
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->didRemoveDOMAttr(paramElement, name);
}

void characterDataModifiedImpl(InstrumentingAgents* agents, CharacterData* paramCharacterData)
{
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->characterDataModified(paramCharacterData);
}

void didInvalidateStyleAttrImpl(InstrumentingAgents* agents, Node* paramNode)
{
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->didInvalidateStyleAttr(paramNode);
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->didInvalidateStyleAttr(paramNode);
}

void documentDetachedImpl(InstrumentingAgents* agents, Document* paramDocument)
{
    if (InspectorCSSAgent* agent = agents->inspectorCSSAgent())
        agent->documentDetached(paramDocument);
}

void willMutateRulesImpl(InstrumentingAgents* agents)
{
    if (InspectorCSSAgent* agent = agents->inspectorCSSAgent())
        agent->willMutateRules();
}

void didMutateRulesImpl(InstrumentingAgents* agents, CSSStyleSheet* paramCSSStyleSheet)
{
    if (InspectorCSSAgent* agent = agents->inspectorCSSAgent())
        agent->didMutateRules(paramCSSStyleSheet);
}

void willMutateStyleImpl(InstrumentingAgents* agents)
{
    if (InspectorCSSAgent* agent = agents->inspectorCSSAgent())
        agent->willMutateStyle();
}

void didMutateStyleImpl(InstrumentingAgents* agents, CSSStyleDeclaration* paramCSSStyleDeclaration, bool parambool)
{
    if (InspectorCSSAgent* agent = agents->inspectorCSSAgent())
        agent->didMutateStyle(paramCSSStyleDeclaration, parambool);
}

void activeStyleSheetsUpdatedImpl(InstrumentingAgents* agents, Document* paramDocument)
{
    if (InspectorCSSAgent* agent = agents->inspectorCSSAgent())
        agent->activeStyleSheetsUpdated(paramDocument);
}

void frameWindowDiscardedImpl(InstrumentingAgents* agents, LocalDOMWindow* domWindow)
{
    if (InspectorConsoleAgent* agent = agents->inspectorConsoleAgent())
        agent->frameWindowDiscarded(domWindow);
    if (PageRuntimeAgent* agent = agents->pageRuntimeAgent())
        agent->frameWindowDiscarded(domWindow);
}

void mediaQueryResultChangedImpl(InstrumentingAgents* agents)
{
    if (InspectorCSSAgent* agent = agents->inspectorCSSAgent())
        agent->mediaQueryResultChanged();
}

void didPushShadowRootImpl(InstrumentingAgents* agents, Element* host, ShadowRoot* paramShadowRoot)
{
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->didPushShadowRoot(host, paramShadowRoot);
}

void willPopShadowRootImpl(InstrumentingAgents* agents, Element* host, ShadowRoot* paramShadowRoot)
{
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->willPopShadowRoot(host, paramShadowRoot);
}

void willSendXMLHttpRequestImpl(InstrumentingAgents* agents, const String& url)
{
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->willSendXMLHttpRequest(url);
}

void didFireWebGLErrorImpl(InstrumentingAgents* agents, const String& errorName)
{
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->didFireWebGLError(errorName);
}

void didFireWebGLWarningImpl(InstrumentingAgents* agents)
{
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->didFireWebGLWarning();
}

void didFireWebGLErrorOrWarningImpl(InstrumentingAgents* agents, const String& message)
{
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->didFireWebGLErrorOrWarning(message);
}

void didInstallTimerImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, int timerId, int timeout, bool singleShot)
{
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->didInstallTimer(paramExecutionContext, timerId, timeout, singleShot);
    if (InspectorDebuggerAgent* agent = agents->inspectorDebuggerAgent())
        agent->didInstallTimer(paramExecutionContext, timerId, timeout, singleShot);
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->didInstallTimer(paramExecutionContext, timerId, timeout, singleShot);
}

void didRemoveTimerImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, int timerId)
{
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->didRemoveTimer(paramExecutionContext, timerId);
    if (InspectorDebuggerAgent* agent = agents->inspectorDebuggerAgent())
        agent->didRemoveTimer(paramExecutionContext, timerId);
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->didRemoveTimer(paramExecutionContext, timerId);
}

InspectorInstrumentationCookie willCallFunctionImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, int scriptId, const String& scriptName, int scriptLine)
{
    int timelineAgentId = 0;
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent()) {
        if (agent->willCallFunction(paramExecutionContext, scriptId, scriptName, scriptLine))
            timelineAgentId = agent->id();
    }
    return InspectorInstrumentationCookie(agents, timelineAgentId);
}

void didCallFunctionImpl(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{
    if (InspectorTimelineAgent* agent = retrieveTimelineAgent(paramInspectorInstrumentationCookie))
        agent->didCallFunction();
}

InspectorInstrumentationCookie willDispatchXHRReadyStateChangeEventImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, XMLHttpRequest* paramXMLHttpRequest)
{
    int timelineAgentId = 0;
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent()) {
        if (agent->willDispatchXHRReadyStateChangeEvent(paramExecutionContext, paramXMLHttpRequest))
            timelineAgentId = agent->id();
    }
    return InspectorInstrumentationCookie(agents, timelineAgentId);
}

void didDispatchXHRReadyStateChangeEventImpl(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{
    if (InspectorTimelineAgent* agent = retrieveTimelineAgent(paramInspectorInstrumentationCookie))
        agent->didDispatchXHRReadyStateChangeEvent();
}

InspectorInstrumentationCookie willDispatchEventImpl(InstrumentingAgents* agents, Document* paramDocument, const Event& paramEvent, LocalDOMWindow* paramLocalDOMWindow, Node* paramNode, const EventPath& paramEventPath)
{
    int timelineAgentId = 0;
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent()) {
        if (agent->willDispatchEvent(paramDocument, paramEvent, paramLocalDOMWindow, paramNode, paramEventPath))
            timelineAgentId = agent->id();
    }
    return InspectorInstrumentationCookie(agents, timelineAgentId);
}

void didDispatchEventImpl(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{
    if (InspectorTimelineAgent* agent = retrieveTimelineAgent(paramInspectorInstrumentationCookie))
        agent->didDispatchEvent();
}

void didEnqueueEventImpl(InstrumentingAgents* agents, EventTarget* paramEventTarget, Event* paramEvent)
{
    if (InspectorDebuggerAgent* agent = agents->inspectorDebuggerAgent())
        agent->didEnqueueEvent(paramEventTarget, paramEvent);
}

void didRemoveEventImpl(InstrumentingAgents* agents, EventTarget* paramEventTarget, Event* paramEvent)
{
    if (InspectorDebuggerAgent* agent = agents->inspectorDebuggerAgent())
        agent->didRemoveEvent(paramEventTarget, paramEvent);
}

InspectorInstrumentationCookie willHandleEventImpl(InstrumentingAgents* agents, EventTarget* paramEventTarget, Event* paramEvent, EventListener* listener, bool useCapture)
{
    if (InspectorDebuggerAgent* agent = agents->inspectorDebuggerAgent())
        agent->willHandleEvent(paramEventTarget, paramEvent, listener, useCapture);
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->willHandleEvent(paramEventTarget, paramEvent, listener, useCapture);
    return InspectorInstrumentationCookie(agents, 0);
}

void didHandleEventImpl(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{
    if (InspectorDebuggerAgent* agent = paramInspectorInstrumentationCookie.instrumentingAgents()->inspectorDebuggerAgent())
        agent->didHandleEvent();
}

InspectorInstrumentationCookie willDispatchEventOnWindowImpl(InstrumentingAgents* agents, const Event& paramEvent, LocalDOMWindow* paramLocalDOMWindow)
{
    int timelineAgentId = 0;
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent()) {
        if (agent->willDispatchEventOnWindow(paramEvent, paramLocalDOMWindow))
            timelineAgentId = agent->id();
    }
    return InspectorInstrumentationCookie(agents, timelineAgentId);
}

void didDispatchEventOnWindowImpl(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{
    if (InspectorTimelineAgent* agent = retrieveTimelineAgent(paramInspectorInstrumentationCookie))
        agent->didDispatchEventOnWindow();
}

void didEnqueueMutationRecordImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, MutationObserver* paramMutationObserver)
{
    if (InspectorDebuggerAgent* agent = agents->inspectorDebuggerAgent())
        agent->didEnqueueMutationRecord(paramExecutionContext, paramMutationObserver);
}

void didClearAllMutationRecordsImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, MutationObserver* paramMutationObserver)
{
    if (InspectorDebuggerAgent* agent = agents->inspectorDebuggerAgent())
        agent->didClearAllMutationRecords(paramExecutionContext, paramMutationObserver);
}

void willDeliverMutationRecordsImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, MutationObserver* paramMutationObserver)
{
    if (InspectorDebuggerAgent* agent = agents->inspectorDebuggerAgent())
        agent->willDeliverMutationRecords(paramExecutionContext, paramMutationObserver);
}

void didDeliverMutationRecordsImpl(InstrumentingAgents* agents)
{
    if (InspectorDebuggerAgent* agent = agents->inspectorDebuggerAgent())
        agent->didDeliverMutationRecords();
}

InspectorInstrumentationCookie willEvaluateScriptImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame, const String& url, int lineNumber)
{
    int timelineAgentId = 0;
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent()) {
        if (agent->willEvaluateScript(paramLocalFrame, url, lineNumber))
            timelineAgentId = agent->id();
    }
    return InspectorInstrumentationCookie(agents, timelineAgentId);
}

void didEvaluateScriptImpl(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{
    if (InspectorTimelineAgent* agent = retrieveTimelineAgent(paramInspectorInstrumentationCookie))
        agent->didEvaluateScript();
}

void didCreateIsolatedContextImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame, ScriptState* paramScriptState, SecurityOrigin* paramSecurityOrigin)
{
    if (PageRuntimeAgent* agent = agents->pageRuntimeAgent())
        agent->didCreateIsolatedContext(paramLocalFrame, paramScriptState, paramSecurityOrigin);
}

InspectorInstrumentationCookie willFireTimerImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, int timerId)
{
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->willFireTimer(paramExecutionContext, timerId);
    if (InspectorDebuggerAgent* agent = agents->inspectorDebuggerAgent())
        agent->willFireTimer(paramExecutionContext, timerId);
    int timelineAgentId = 0;
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent()) {
        if (agent->willFireTimer(paramExecutionContext, timerId))
            timelineAgentId = agent->id();
    }
    return InspectorInstrumentationCookie(agents, timelineAgentId);
}

void didFireTimerImpl(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{
    if (InspectorDebuggerAgent* agent = paramInspectorInstrumentationCookie.instrumentingAgents()->inspectorDebuggerAgent())
        agent->didFireTimer();
    if (InspectorTimelineAgent* agent = retrieveTimelineAgent(paramInspectorInstrumentationCookie))
        agent->didFireTimer();
}

void didInvalidateLayoutImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame)
{
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->didInvalidateLayout(paramLocalFrame);
}

InspectorInstrumentationCookie willLayoutImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame)
{
    int timelineAgentId = 0;
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent()) {
        if (agent->willLayout(paramLocalFrame))
            timelineAgentId = agent->id();
    }
    return InspectorInstrumentationCookie(agents, timelineAgentId);
}

void didLayoutImpl(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie, RenderObject* root)
{
    if (InspectorTimelineAgent* agent = retrieveTimelineAgent(paramInspectorInstrumentationCookie))
        agent->didLayout(root);
    if (InspectorPageAgent* agent = paramInspectorInstrumentationCookie.instrumentingAgents()->inspectorPageAgent())
        agent->didLayout(root);
}

void didScrollImpl(InstrumentingAgents* agents)
{
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->didScroll();
}

void didResizeMainFrameImpl(InstrumentingAgents* agents)
{
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->didResizeMainFrame();
}

InspectorInstrumentationCookie willDispatchXHRLoadEventImpl(InstrumentingAgents* agents, ExecutionContext* paramExecutionContext, XMLHttpRequest* paramXMLHttpRequest)
{
    int timelineAgentId = 0;
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent()) {
        if (agent->willDispatchXHRLoadEvent(paramExecutionContext, paramXMLHttpRequest))
            timelineAgentId = agent->id();
    }
    return InspectorInstrumentationCookie(agents, timelineAgentId);
}

void didDispatchXHRLoadEventImpl(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{
    if (InspectorTimelineAgent* agent = retrieveTimelineAgent(paramInspectorInstrumentationCookie))
        agent->didDispatchXHRLoadEvent();
}

void willScrollLayerImpl(InstrumentingAgents* agents, RenderObject* paramRenderObject)
{
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->willScrollLayer(paramRenderObject);
}

void didScrollLayerImpl(InstrumentingAgents* agents)
{
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->didScrollLayer();
}

void willPaintImpl(InstrumentingAgents* agents, RenderObject* paramRenderObject, const GraphicsLayer* paramGraphicsLayer)
{
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->willPaint(paramRenderObject, paramGraphicsLayer);
}

void didPaintImpl(InstrumentingAgents* agents, RenderObject* paramRenderObject, const GraphicsLayer* paramGraphicsLayer, GraphicsContext* paramGraphicsContext, const LayoutRect& paramLayoutRect)
{
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->didPaint(paramRenderObject, paramGraphicsLayer, paramGraphicsContext, paramLayoutRect);
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->didPaint(paramRenderObject, paramGraphicsLayer, paramGraphicsContext, paramLayoutRect);
    if (InspectorLayerTreeAgent* agent = agents->inspectorLayerTreeAgent())
        agent->didPaint(paramRenderObject, paramGraphicsLayer, paramGraphicsContext, paramLayoutRect);
}

void willPaintImageImpl(InstrumentingAgents* agents, RenderImage* paramRenderImage)
{
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->willPaintImage(paramRenderImage);
}

void didPaintImageImpl(InstrumentingAgents* agents)
{
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->didPaintImage();
}

InspectorInstrumentationCookie willRecalculateStyleImpl(InstrumentingAgents* agents, Document* paramDocument)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->willRecalculateStyle(paramDocument);
    int timelineAgentId = 0;
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent()) {
        if (agent->willRecalculateStyle(paramDocument))
            timelineAgentId = agent->id();
    }
    return InspectorInstrumentationCookie(agents, timelineAgentId);
}

void didRecalculateStyleImpl(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie, int elementCount)
{
    if (InspectorTimelineAgent* agent = retrieveTimelineAgent(paramInspectorInstrumentationCookie))
        agent->didRecalculateStyle(elementCount);
    if (InspectorResourceAgent* agent = paramInspectorInstrumentationCookie.instrumentingAgents()->inspectorResourceAgent())
        agent->didRecalculateStyle(elementCount);
    if (InspectorPageAgent* agent = paramInspectorInstrumentationCookie.instrumentingAgents()->inspectorPageAgent())
        agent->didRecalculateStyle(elementCount);
}

void didScheduleStyleRecalculationImpl(InstrumentingAgents* agents, Document* paramDocument)
{
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->didScheduleStyleRecalculation(paramDocument);
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didScheduleStyleRecalculation(paramDocument);
}

void applyUserAgentOverrideImpl(InstrumentingAgents* agents, String* userAgent)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->applyUserAgentOverride(userAgent);
}

bool applyViewportStyleOverrideImpl(InstrumentingAgents* agents, StyleResolver* paramStyleResolver)
{
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        return agent->applyViewportStyleOverride(paramStyleResolver);
    return false;
}

void applyEmulatedMediaImpl(InstrumentingAgents* agents, String* media)
{
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->applyEmulatedMedia(media);
}

void willSendRequestImpl(InstrumentingAgents* agents, unsigned long identifier, DocumentLoader* paramDocumentLoader, ResourceRequest& paramResourceRequest, const ResourceResponse& redirectResponse, const FetchInitiatorInfo& paramFetchInitiatorInfo)
{
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->willSendRequest(identifier, paramDocumentLoader, paramResourceRequest, redirectResponse, paramFetchInitiatorInfo);
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->willSendRequest(identifier, paramDocumentLoader, paramResourceRequest, redirectResponse, paramFetchInitiatorInfo);
}

void markResourceAsCachedImpl(InstrumentingAgents* agents, unsigned long identifier)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->markResourceAsCached(identifier);
}

void didReceiveResourceResponseImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame, unsigned long identifier, DocumentLoader* paramDocumentLoader, const ResourceResponse& paramResourceResponse, ResourceLoader* paramResourceLoader)
{
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->didReceiveResourceResponse(paramLocalFrame, identifier, paramDocumentLoader, paramResourceResponse, paramResourceLoader);
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didReceiveResourceResponse(paramLocalFrame, identifier, paramDocumentLoader, paramResourceResponse, paramResourceLoader);
    if (InspectorConsoleAgent* agent = agents->inspectorConsoleAgent())
        agent->didReceiveResourceResponse(paramLocalFrame, identifier, paramDocumentLoader, paramResourceResponse, paramResourceLoader);
}

void didReceiveDataImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame, unsigned long identifier, const char* data, int dataLength, int encodedDataLength)
{
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->didReceiveData(paramLocalFrame, identifier, data, dataLength, encodedDataLength);
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didReceiveData(paramLocalFrame, identifier, data, dataLength, encodedDataLength);
}

void didFinishLoadingImpl(InstrumentingAgents* agents, unsigned long identifier, DocumentLoader* paramDocumentLoader, double finishTime, int64_t encodedDataLength)
{
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->didFinishLoading(identifier, paramDocumentLoader, finishTime, encodedDataLength);
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didFinishLoading(identifier, paramDocumentLoader, finishTime, encodedDataLength);
}

void didReceiveCORSRedirectResponseImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame, unsigned long identifier, DocumentLoader* paramDocumentLoader, const ResourceResponse& paramResourceResponse, ResourceLoader* paramResourceLoader)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didReceiveCORSRedirectResponse(paramLocalFrame, identifier, paramDocumentLoader, paramResourceResponse, paramResourceLoader);
}

void didFailLoadingImpl(InstrumentingAgents* agents, unsigned long identifier, const ResourceError& paramResourceError)
{
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->didFailLoading(identifier, paramResourceError);
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didFailLoading(identifier, paramResourceError);
    if (InspectorConsoleAgent* agent = agents->inspectorConsoleAgent())
        agent->didFailLoading(identifier, paramResourceError);
}

void documentThreadableLoaderStartedLoadingForClientImpl(InstrumentingAgents* agents, unsigned long identifier, ThreadableLoaderClient* client)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->documentThreadableLoaderStartedLoadingForClient(identifier, client);
}

void willLoadXHRImpl(InstrumentingAgents* agents, XMLHttpRequest* xhr, ThreadableLoaderClient* client, const AtomicString& method, const KURL& url, bool async, PassRefPtr<FormData> paramFormData, const HTTPHeaderMap& headers, bool includeCredentials)
{
    if (InspectorDebuggerAgent* agent = agents->inspectorDebuggerAgent())
        agent->willLoadXHR(xhr, client, method, url, async, paramFormData.get(), headers, includeCredentials);
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->willLoadXHR(xhr, client, method, url, async, paramFormData.get(), headers, includeCredentials);
}

void didFailXHRLoadingImpl(InstrumentingAgents* agents, XMLHttpRequest* xhr, ThreadableLoaderClient* client)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didFailXHRLoading(xhr, client);
}

void didFinishXHRLoadingImpl(InstrumentingAgents* agents, XMLHttpRequest* xhr, ThreadableLoaderClient* client, unsigned long identifier, ScriptString sourceString, const AtomicString& method, const String& url, const String& sendURL, unsigned sendLineNumber)
{
    if (InspectorConsoleAgent* agent = agents->inspectorConsoleAgent())
        agent->didFinishXHRLoading(xhr, client, identifier, sourceString, method, url, sendURL, sendLineNumber);
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didFinishXHRLoading(xhr, client, identifier, sourceString, method, url, sendURL, sendLineNumber);
}

void scriptImportedImpl(InstrumentingAgents* agents, unsigned long identifier, const String& sourceString)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->scriptImported(identifier, sourceString);
}

void scriptExecutionBlockedByCSPImpl(InstrumentingAgents* agents, const String& directiveText)
{
    if (InspectorDebuggerAgent* agent = agents->inspectorDebuggerAgent())
        agent->scriptExecutionBlockedByCSP(directiveText);
}

void didReceiveScriptResponseImpl(InstrumentingAgents* agents, unsigned long identifier)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didReceiveScriptResponse(identifier);
}

void domContentLoadedEventFiredImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame)
{
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->domContentLoadedEventFired(paramLocalFrame);
    if (InspectorInspectorAgent* agent = agents->inspectorInspectorAgent())
        agent->domContentLoadedEventFired(paramLocalFrame);
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->domContentLoadedEventFired(paramLocalFrame);
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->domContentLoadedEventFired(paramLocalFrame);
}

void loadEventFiredImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame)
{
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->loadEventFired(paramLocalFrame);
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->loadEventFired(paramLocalFrame);
}

void frameAttachedToParentImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame)
{
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->frameAttachedToParent(paramLocalFrame);
}

void frameDetachedFromParentImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame)
{
    if (InspectorCanvasAgent* agent = agents->inspectorCanvasAgent())
        agent->frameDetachedFromParent(paramLocalFrame);
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->frameDetachedFromParent(paramLocalFrame);
}

void didCommitLoadImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame, DocumentLoader* paramDocumentLoader)
{
    if (InspectorConsoleAgent* agent = agents->inspectorConsoleAgent())
        agent->didCommitLoad(paramLocalFrame, paramDocumentLoader);
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didCommitLoad(paramLocalFrame, paramDocumentLoader);
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->didCommitLoad(paramLocalFrame, paramDocumentLoader);
    if (InspectorCanvasAgent* agent = agents->inspectorCanvasAgent())
        agent->didCommitLoad(paramLocalFrame, paramDocumentLoader);
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->didCommitLoad(paramLocalFrame, paramDocumentLoader);
    if (PageDebuggerAgent* agent = agents->pageDebuggerAgent())
        agent->didCommitLoad(paramLocalFrame, paramDocumentLoader);
}

void frameDocumentUpdatedImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame)
{
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->frameDocumentUpdated(paramLocalFrame);
}

void loaderDetachedFromFrameImpl(InstrumentingAgents* agents, DocumentLoader* paramDocumentLoader)
{
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->loaderDetachedFromFrame(paramDocumentLoader);
}

void frameStartedLoadingImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame)
{
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->frameStartedLoading(paramLocalFrame);
}

void frameStoppedLoadingImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame)
{
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->frameStoppedLoading(paramLocalFrame);
}

void frameScheduledNavigationImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame, double delay)
{
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->frameScheduledNavigation(paramLocalFrame, delay);
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->frameScheduledNavigation(paramLocalFrame, delay);
}

void frameClearedScheduledNavigationImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame)
{
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->frameClearedScheduledNavigation(paramLocalFrame);
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->frameClearedScheduledNavigation(paramLocalFrame);
}

InspectorInstrumentationCookie willRunJavaScriptDialogImpl(InstrumentingAgents* agents, const String& message)
{
    if (InspectorPageAgent* agent = agents->inspectorPageAgent())
        agent->willRunJavaScriptDialog(message);
    return InspectorInstrumentationCookie(agents, 0);
}

void didRunJavaScriptDialogImpl(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{
    if (InspectorPageAgent* agent = paramInspectorInstrumentationCookie.instrumentingAgents()->inspectorPageAgent())
        agent->didRunJavaScriptDialog();
}

InspectorInstrumentationCookie willWriteHTMLImpl(InstrumentingAgents* agents, Document* paramDocument, unsigned startLine)
{
    int timelineAgentId = 0;
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent()) {
        if (agent->willWriteHTML(paramDocument, startLine))
            timelineAgentId = agent->id();
    }
    return InspectorInstrumentationCookie(agents, timelineAgentId);
}

void didWriteHTMLImpl(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie, unsigned endLine)
{
    if (InspectorTimelineAgent* agent = retrieveTimelineAgent(paramInspectorInstrumentationCookie))
        agent->didWriteHTML(endLine);
}

void didRequestAnimationFrameImpl(InstrumentingAgents* agents, Document* paramDocument, int callbackId)
{
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->didRequestAnimationFrame(paramDocument, callbackId);
    if (InspectorDebuggerAgent* agent = agents->inspectorDebuggerAgent())
        agent->didRequestAnimationFrame(paramDocument, callbackId);
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->didRequestAnimationFrame(paramDocument, callbackId);
}

void didCancelAnimationFrameImpl(InstrumentingAgents* agents, Document* paramDocument, int callbackId)
{
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->didCancelAnimationFrame(paramDocument, callbackId);
    if (InspectorDebuggerAgent* agent = agents->inspectorDebuggerAgent())
        agent->didCancelAnimationFrame(paramDocument, callbackId);
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->didCancelAnimationFrame(paramDocument, callbackId);
}

InspectorInstrumentationCookie willFireAnimationFrameImpl(InstrumentingAgents* agents, Document* paramDocument, int callbackId)
{
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->willFireAnimationFrame(paramDocument, callbackId);
    if (InspectorDebuggerAgent* agent = agents->inspectorDebuggerAgent())
        agent->willFireAnimationFrame(paramDocument, callbackId);
    int timelineAgentId = 0;
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent()) {
        if (agent->willFireAnimationFrame(paramDocument, callbackId))
            timelineAgentId = agent->id();
    }
    return InspectorInstrumentationCookie(agents, timelineAgentId);
}

void didFireAnimationFrameImpl(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{
    if (InspectorTimelineAgent* agent = retrieveTimelineAgent(paramInspectorInstrumentationCookie))
        agent->didFireAnimationFrame();
    if (InspectorDebuggerAgent* agent = paramInspectorInstrumentationCookie.instrumentingAgents()->inspectorDebuggerAgent())
        agent->didFireAnimationFrame();
}

void didDispatchDOMStorageEventImpl(InstrumentingAgents* agents, const String& key, const String& oldValue, const String& newValue, StorageType storageType, SecurityOrigin* securityOrigin)
{
    if (InspectorDOMStorageAgent* agent = agents->inspectorDOMStorageAgent())
        agent->didDispatchDOMStorageEvent(key, oldValue, newValue, storageType, securityOrigin);
}

void didStartWorkerGlobalScopeImpl(InstrumentingAgents* agents, WorkerGlobalScopeProxy* proxy, const KURL& url)
{
    if (InspectorWorkerAgent* agent = agents->inspectorWorkerAgent())
        agent->didStartWorkerGlobalScope(proxy, url);
}

void willEvaluateWorkerScriptImpl(InstrumentingAgents* agents, WorkerGlobalScope* context, int workerThreadStartMode)
{
    if (WorkerRuntimeAgent* agent = agents->workerRuntimeAgent())
        agent->willEvaluateWorkerScript(context, workerThreadStartMode);
}

void workerGlobalScopeTerminatedImpl(InstrumentingAgents* agents, WorkerGlobalScopeProxy* proxy)
{
    if (InspectorWorkerAgent* agent = agents->inspectorWorkerAgent())
        agent->workerGlobalScopeTerminated(proxy);
}

void willProcessTaskImpl(InstrumentingAgents* agents)
{
    if (InspectorProfilerAgent* agent = agents->inspectorProfilerAgent())
        agent->willProcessTask();
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->willProcessTask();
}

void didProcessTaskImpl(InstrumentingAgents* agents)
{
    if (InspectorProfilerAgent* agent = agents->inspectorProfilerAgent())
        agent->didProcessTask();
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->didProcessTask();
}

void willEnterNestedRunLoopImpl(InstrumentingAgents* agents)
{
    if (InspectorProfilerAgent* agent = agents->inspectorProfilerAgent())
        agent->willEnterNestedRunLoop();
}

void didLeaveNestedRunLoopImpl(InstrumentingAgents* agents)
{
    if (InspectorProfilerAgent* agent = agents->inspectorProfilerAgent())
        agent->didLeaveNestedRunLoop();
}

void didCreateWebSocketImpl(InstrumentingAgents* agents, Document* paramDocument, unsigned long identifier, const KURL& requestURL, const String& protocol)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didCreateWebSocket(paramDocument, identifier, requestURL, protocol);
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->didCreateWebSocket(paramDocument, identifier, requestURL, protocol);
}

void willSendWebSocketHandshakeRequestImpl(InstrumentingAgents* agents, Document* paramDocument, unsigned long identifier, const WebSocketHandshakeRequest* request)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->willSendWebSocketHandshakeRequest(paramDocument, identifier, request);
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->willSendWebSocketHandshakeRequest(paramDocument, identifier, request);
}

void didReceiveWebSocketHandshakeResponseImpl(InstrumentingAgents* agents, Document* paramDocument, unsigned long identifier, const WebSocketHandshakeRequest* request, const WebSocketHandshakeResponse* response)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didReceiveWebSocketHandshakeResponse(paramDocument, identifier, request, response);
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->didReceiveWebSocketHandshakeResponse(paramDocument, identifier, request, response);
}

void didCloseWebSocketImpl(InstrumentingAgents* agents, Document* paramDocument, unsigned long identifier)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didCloseWebSocket(paramDocument, identifier);
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->didCloseWebSocket(paramDocument, identifier);
}

void didReceiveWebSocketFrameImpl(InstrumentingAgents* agents, unsigned long identifier, int opCode, bool masked, const char* payload, size_t payloadLength)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didReceiveWebSocketFrame(identifier, opCode, masked, payload, payloadLength);
}

void didSendWebSocketFrameImpl(InstrumentingAgents* agents, unsigned long identifier, int opCode, bool masked, const char* payload, size_t payloadLength)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didSendWebSocketFrame(identifier, opCode, masked, payload, payloadLength);
}

void didReceiveWebSocketFrameErrorImpl(InstrumentingAgents* agents, unsigned long identifier, const String& errorMessage)
{
    if (InspectorResourceAgent* agent = agents->inspectorResourceAgent())
        agent->didReceiveWebSocketFrameError(identifier, errorMessage);
}

void networkStateChangedImpl(InstrumentingAgents* agents, bool online)
{
    if (InspectorApplicationCacheAgent* agent = agents->inspectorApplicationCacheAgent())
        agent->networkStateChanged(online);
}

void updateApplicationCacheStatusImpl(InstrumentingAgents* agents, LocalFrame* paramLocalFrame)
{
    if (InspectorApplicationCacheAgent* agent = agents->inspectorApplicationCacheAgent())
        agent->updateApplicationCacheStatus(paramLocalFrame);
}

void willUpdateLayerTreeImpl(InstrumentingAgents* agents)
{
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->willUpdateLayerTree();
}

void layerTreeDidChangeImpl(InstrumentingAgents* agents)
{
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->layerTreeDidChange();
    if (InspectorLayerTreeAgent* agent = agents->inspectorLayerTreeAgent())
        agent->layerTreeDidChange();
}

void didUpdateLayerTreeImpl(InstrumentingAgents* agents)
{
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->didUpdateLayerTree();
}

void pseudoElementCreatedImpl(InstrumentingAgents* agents, PseudoElement* paramPseudoElement)
{
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->pseudoElementCreated(paramPseudoElement);
}

void pseudoElementDestroyedImpl(InstrumentingAgents* agents, PseudoElement* paramPseudoElement)
{
    if (InspectorDOMAgent* agent = agents->inspectorDOMAgent())
        agent->pseudoElementDestroyed(paramPseudoElement);
}

void willExecuteCustomElementCallbackImpl(InstrumentingAgents* agents, Element* paramElement)
{
    if (InspectorDOMDebuggerAgent* agent = agents->inspectorDOMDebuggerAgent())
        agent->willExecuteCustomElementCallback(paramElement);
}

void addMessageToConsoleImpl(InstrumentingAgents* agents, MessageSource source, MessageType type, MessageLevel level, const String& message, PassRefPtrWillBeRawPtr<ScriptCallStack> callStack, unsigned long requestIdentifier)
{
    if (InspectorConsoleAgent* agent = agents->inspectorConsoleAgent())
        agent->addMessageToConsole(source, type, level, message, callStack, requestIdentifier);
}

void addMessageToConsoleImpl(InstrumentingAgents* agents, MessageSource source, MessageType type, MessageLevel level, const String& message, const String& scriptId, unsigned lineNumber, unsigned columnNumber, ScriptState* state, unsigned long requestIdentifier)
{
    if (InspectorConsoleAgent* agent = agents->inspectorConsoleAgent())
        agent->addMessageToConsole(source, type, level, message, scriptId, lineNumber, columnNumber, state, requestIdentifier);
}

void addMessageToConsoleImpl(InstrumentingAgents* agents, MessageSource source, MessageType type, MessageLevel level, const String& message, ScriptState* state, PassRefPtrWillBeRawPtr<ScriptArguments> arguments, unsigned long requestIdentifier)
{
    if (InspectorConsoleAgent* agent = agents->inspectorConsoleAgent())
        agent->addMessageToConsole(source, type, level, message, state, arguments, requestIdentifier);
    if (InspectorDebuggerAgent* agent = agents->inspectorDebuggerAgent())
        agent->addMessageToConsole(source, type, level, message, state, arguments, requestIdentifier);
}

void consoleCountImpl(InstrumentingAgents* agents, ScriptState* state, PassRefPtrWillBeRawPtr<ScriptArguments> arguments)
{
    if (InspectorConsoleAgent* agent = agents->inspectorConsoleAgent())
        agent->consoleCount(state, arguments);
}

void consoleTimeImpl(InstrumentingAgents* agents, ExecutionContext* context, const String& title)
{
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->consoleTime(context, title);
    if (InspectorConsoleAgent* agent = agents->inspectorConsoleAgent())
        agent->consoleTime(context, title);
}

void consoleTimeEndImpl(InstrumentingAgents* agents, ExecutionContext* context, const String& title, ScriptState* state)
{
    if (InspectorConsoleAgent* agent = agents->inspectorConsoleAgent())
        agent->consoleTimeEnd(context, title, state);
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->consoleTimeEnd(context, title, state);
}

void consoleTimeStampImpl(InstrumentingAgents* agents, ExecutionContext* context, const String& title)
{
    if (InspectorTimelineAgent* agent = agents->inspectorTimelineAgent())
        agent->consoleTimeStamp(context, title);
}

void consoleTimelineImpl(InstrumentingAgents* agents, ExecutionContext* context, const String& title, ScriptState* state)
{
    if (InspectorConsoleAgent* agent = agents->inspectorConsoleAgent())
        agent->consoleTimeline(context, title, state);
}

void consoleTimelineEndImpl(InstrumentingAgents* agents, ExecutionContext* context, const String& title, ScriptState* state)
{
    if (InspectorConsoleAgent* agent = agents->inspectorConsoleAgent())
        agent->consoleTimelineEnd(context, title, state);
}

void consoleProfileImpl(InstrumentingAgents* agents, const String& title, ScriptState* state)
{
    if (InspectorProfilerAgent* agent = agents->inspectorProfilerAgent())
        agent->consoleProfile(title, state);
}

void consoleProfileEndImpl(InstrumentingAgents* agents, const String& title, ScriptState* state)
{
    if (InspectorProfilerAgent* agent = agents->inspectorProfilerAgent())
        agent->consoleProfileEnd(title, state);
}

bool forcePseudoStateImpl(InstrumentingAgents* agents, Element* element, CSSSelector::PseudoType pseudoState)
{
    if (InspectorCSSAgent* agent = agents->inspectorCSSAgent())
        return agent->forcePseudoState(element, pseudoState);
    return false;
}

bool shouldPauseDedicatedWorkerOnStartImpl(InstrumentingAgents* agents)
{
    if (InspectorWorkerAgent* agent = agents->inspectorWorkerAgent())
        return agent->shouldPauseDedicatedWorkerOnStart();
    return false;
}

ScriptValue wrapCanvas2DRenderingContextForInstrumentationImpl(InstrumentingAgents* agents, const ScriptValue& paramScriptValue)
{
    if (InspectorCanvasAgent* agent = agents->inspectorCanvasAgent())
        return agent->wrapCanvas2DRenderingContextForInstrumentation(paramScriptValue);
    return ScriptValue();
}

ScriptValue wrapWebGLRenderingContextForInstrumentationImpl(InstrumentingAgents* agents, const ScriptValue& paramScriptValue)
{
    if (InspectorCanvasAgent* agent = agents->inspectorCanvasAgent())
        return agent->wrapWebGLRenderingContextForInstrumentation(paramScriptValue);
    return ScriptValue();
}

} // namespace InspectorInstrumentation

} // namespace WebCore
