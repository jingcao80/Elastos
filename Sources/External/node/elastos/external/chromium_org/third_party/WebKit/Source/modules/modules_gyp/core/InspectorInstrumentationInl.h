// Code generated from InspectorInstrumentation.idl

#ifndef InspectorInstrumentationInl_h
#define InspectorInstrumentationInl_h

#include "core/dom/PseudoElement.h"
#include "core/inspector/InspectorInstrumentation.h"

namespace WebCore {

namespace InspectorInstrumentation {

void didClearDocumentOfWindowObjectImpl(InstrumentingAgents*, LocalFrame*);

inline void didClearDocumentOfWindowObject(LocalFrame* paramLocalFrame)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        didClearDocumentOfWindowObjectImpl(agents, paramLocalFrame);
}

void willInsertDOMNodeImpl(InstrumentingAgents*, Node*);

inline void willInsertDOMNode(Node* parent)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(parent))
        willInsertDOMNodeImpl(agents, parent);
}

void didInsertDOMNodeImpl(InstrumentingAgents*, Node*);

inline void didInsertDOMNode(Node* paramNode)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramNode))
        didInsertDOMNodeImpl(agents, paramNode);
}

void willRemoveDOMNodeImpl(InstrumentingAgents*, Node*);

inline void willRemoveDOMNode(Node* paramNode)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramNode))
        willRemoveDOMNodeImpl(agents, paramNode);
}

void willModifyDOMAttrImpl(InstrumentingAgents*, Element*, const AtomicString&, const AtomicString&);

inline void willModifyDOMAttr(Element* paramElement, const AtomicString& oldValue, const AtomicString& newValue)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramElement))
        willModifyDOMAttrImpl(agents, paramElement, oldValue, newValue);
}

void didModifyDOMAttrImpl(InstrumentingAgents*, Element*, const AtomicString&, const AtomicString&);

inline void didModifyDOMAttr(Element* paramElement, const AtomicString& name, const AtomicString& value)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramElement))
        didModifyDOMAttrImpl(agents, paramElement, name, value);
}

void didRemoveDOMAttrImpl(InstrumentingAgents*, Element*, const AtomicString&);

inline void didRemoveDOMAttr(Element* paramElement, const AtomicString& name)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramElement))
        didRemoveDOMAttrImpl(agents, paramElement, name);
}

void characterDataModifiedImpl(InstrumentingAgents*, CharacterData*);

inline void characterDataModified(CharacterData* paramCharacterData)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramCharacterData))
        characterDataModifiedImpl(agents, paramCharacterData);
}

void didInvalidateStyleAttrImpl(InstrumentingAgents*, Node*);

inline void didInvalidateStyleAttr(Node* paramNode)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramNode))
        didInvalidateStyleAttrImpl(agents, paramNode);
}

void documentDetachedImpl(InstrumentingAgents*, Document*);

inline void documentDetached(Document* paramDocument)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        documentDetachedImpl(agents, paramDocument);
}

void willMutateRulesImpl(InstrumentingAgents*);

inline void willMutateRules(CSSStyleSheet* paramCSSStyleSheet)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramCSSStyleSheet))
        willMutateRulesImpl(agents);
}

void didMutateRulesImpl(InstrumentingAgents*, CSSStyleSheet*);

inline void didMutateRules(CSSStyleSheet* paramCSSStyleSheet)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramCSSStyleSheet))
        didMutateRulesImpl(agents, paramCSSStyleSheet);
}

void willMutateStyleImpl(InstrumentingAgents*);

inline void willMutateStyle(CSSStyleDeclaration* paramCSSStyleDeclaration)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramCSSStyleDeclaration))
        willMutateStyleImpl(agents);
}

void didMutateStyleImpl(InstrumentingAgents*, CSSStyleDeclaration*, bool);

inline void didMutateStyle(CSSStyleDeclaration* paramCSSStyleDeclaration, bool parambool)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramCSSStyleDeclaration))
        didMutateStyleImpl(agents, paramCSSStyleDeclaration, parambool);
}

void activeStyleSheetsUpdatedImpl(InstrumentingAgents*, Document*);

inline void activeStyleSheetsUpdated(Document* paramDocument)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        activeStyleSheetsUpdatedImpl(agents, paramDocument);
}

void frameWindowDiscardedImpl(InstrumentingAgents*, LocalDOMWindow*);

inline void frameWindowDiscarded(LocalFrame* paramLocalFrame, LocalDOMWindow* domWindow)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        frameWindowDiscardedImpl(agents, domWindow);
}

void mediaQueryResultChangedImpl(InstrumentingAgents*);

inline void mediaQueryResultChanged(Document* paramDocument)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        mediaQueryResultChangedImpl(agents);
}

void didPushShadowRootImpl(InstrumentingAgents*, Element*, ShadowRoot*);

inline void didPushShadowRoot(Element* host, ShadowRoot* paramShadowRoot)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(host))
        didPushShadowRootImpl(agents, host, paramShadowRoot);
}

void willPopShadowRootImpl(InstrumentingAgents*, Element*, ShadowRoot*);

inline void willPopShadowRoot(Element* host, ShadowRoot* paramShadowRoot)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(host))
        willPopShadowRootImpl(agents, host, paramShadowRoot);
}

void willSendXMLHttpRequestImpl(InstrumentingAgents*, const String&);

inline void willSendXMLHttpRequest(ExecutionContext* paramExecutionContext, const String& url)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        willSendXMLHttpRequestImpl(agents, url);
}

void didFireWebGLErrorImpl(InstrumentingAgents*, const String&);

inline void didFireWebGLError(Element* paramElement, const String& errorName)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramElement))
        didFireWebGLErrorImpl(agents, errorName);
}

void didFireWebGLWarningImpl(InstrumentingAgents*);

inline void didFireWebGLWarning(Element* paramElement)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramElement))
        didFireWebGLWarningImpl(agents);
}

void didFireWebGLErrorOrWarningImpl(InstrumentingAgents*, const String&);

inline void didFireWebGLErrorOrWarning(Element* paramElement, const String& message)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramElement))
        didFireWebGLErrorOrWarningImpl(agents, message);
}

void didInstallTimerImpl(InstrumentingAgents*, ExecutionContext*, int, int, bool);

inline void didInstallTimer(ExecutionContext* paramExecutionContext, int timerId, int timeout, bool singleShot)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        didInstallTimerImpl(agents, paramExecutionContext, timerId, timeout, singleShot);
}

void didRemoveTimerImpl(InstrumentingAgents*, ExecutionContext*, int);

inline void didRemoveTimer(ExecutionContext* paramExecutionContext, int timerId)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        didRemoveTimerImpl(agents, paramExecutionContext, timerId);
}

InspectorInstrumentationCookie willCallFunctionImpl(InstrumentingAgents*, ExecutionContext*, int, const String&, int);

inline InspectorInstrumentationCookie willCallFunction(ExecutionContext* paramExecutionContext, int scriptId, const String& scriptName, int scriptLine)
{   
    FAST_RETURN_IF_NO_FRONTENDS(InspectorInstrumentationCookie());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        return willCallFunctionImpl(agents, paramExecutionContext, scriptId, scriptName, scriptLine);
    return InspectorInstrumentationCookie();
}

void didCallFunctionImpl(const InspectorInstrumentationCookie&);

inline void didCallFunction(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (paramInspectorInstrumentationCookie.isValid())
        didCallFunctionImpl(paramInspectorInstrumentationCookie);
}

InspectorInstrumentationCookie willDispatchXHRReadyStateChangeEventImpl(InstrumentingAgents*, ExecutionContext*, XMLHttpRequest*);

inline InspectorInstrumentationCookie willDispatchXHRReadyStateChangeEvent(ExecutionContext* paramExecutionContext, XMLHttpRequest* paramXMLHttpRequest)
{   
    FAST_RETURN_IF_NO_FRONTENDS(InspectorInstrumentationCookie());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        return willDispatchXHRReadyStateChangeEventImpl(agents, paramExecutionContext, paramXMLHttpRequest);
    return InspectorInstrumentationCookie();
}

void didDispatchXHRReadyStateChangeEventImpl(const InspectorInstrumentationCookie&);

inline void didDispatchXHRReadyStateChangeEvent(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (paramInspectorInstrumentationCookie.isValid())
        didDispatchXHRReadyStateChangeEventImpl(paramInspectorInstrumentationCookie);
}

InspectorInstrumentationCookie willDispatchEventImpl(InstrumentingAgents*, Document*, const Event&, LocalDOMWindow*, Node*, const EventPath&);

inline InspectorInstrumentationCookie willDispatchEvent(Document* paramDocument, const Event& paramEvent, LocalDOMWindow* paramLocalDOMWindow, Node* paramNode, const EventPath& paramEventPath)
{   
    FAST_RETURN_IF_NO_FRONTENDS(InspectorInstrumentationCookie());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        return willDispatchEventImpl(agents, paramDocument, paramEvent, paramLocalDOMWindow, paramNode, paramEventPath);
    return InspectorInstrumentationCookie();
}

void didDispatchEventImpl(const InspectorInstrumentationCookie&);

inline void didDispatchEvent(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (paramInspectorInstrumentationCookie.isValid())
        didDispatchEventImpl(paramInspectorInstrumentationCookie);
}

void didEnqueueEventImpl(InstrumentingAgents*, EventTarget*, Event*);

inline void didEnqueueEvent(EventTarget* paramEventTarget, Event* paramEvent)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramEventTarget))
        didEnqueueEventImpl(agents, paramEventTarget, paramEvent);
}

void didRemoveEventImpl(InstrumentingAgents*, EventTarget*, Event*);

inline void didRemoveEvent(EventTarget* paramEventTarget, Event* paramEvent)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramEventTarget))
        didRemoveEventImpl(agents, paramEventTarget, paramEvent);
}

InspectorInstrumentationCookie willHandleEventImpl(InstrumentingAgents*, EventTarget*, Event*, EventListener*, bool);

inline InspectorInstrumentationCookie willHandleEvent(EventTarget* paramEventTarget, Event* paramEvent, EventListener* listener, bool useCapture)
{   
    FAST_RETURN_IF_NO_FRONTENDS(InspectorInstrumentationCookie());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramEventTarget))
        return willHandleEventImpl(agents, paramEventTarget, paramEvent, listener, useCapture);
    return InspectorInstrumentationCookie();
}

void didHandleEventImpl(const InspectorInstrumentationCookie&);

inline void didHandleEvent(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (paramInspectorInstrumentationCookie.isValid())
        didHandleEventImpl(paramInspectorInstrumentationCookie);
}

InspectorInstrumentationCookie willDispatchEventOnWindowImpl(InstrumentingAgents*, const Event&, LocalDOMWindow*);

inline InspectorInstrumentationCookie willDispatchEventOnWindow(LocalFrame* paramLocalFrame, const Event& paramEvent, LocalDOMWindow* paramLocalDOMWindow)
{   
    FAST_RETURN_IF_NO_FRONTENDS(InspectorInstrumentationCookie());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        return willDispatchEventOnWindowImpl(agents, paramEvent, paramLocalDOMWindow);
    return InspectorInstrumentationCookie();
}

void didDispatchEventOnWindowImpl(const InspectorInstrumentationCookie&);

inline void didDispatchEventOnWindow(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (paramInspectorInstrumentationCookie.isValid())
        didDispatchEventOnWindowImpl(paramInspectorInstrumentationCookie);
}

void didEnqueueMutationRecordImpl(InstrumentingAgents*, ExecutionContext*, MutationObserver*);

inline void didEnqueueMutationRecord(ExecutionContext* paramExecutionContext, MutationObserver* paramMutationObserver)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        didEnqueueMutationRecordImpl(agents, paramExecutionContext, paramMutationObserver);
}

void didClearAllMutationRecordsImpl(InstrumentingAgents*, ExecutionContext*, MutationObserver*);

inline void didClearAllMutationRecords(ExecutionContext* paramExecutionContext, MutationObserver* paramMutationObserver)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        didClearAllMutationRecordsImpl(agents, paramExecutionContext, paramMutationObserver);
}

void willDeliverMutationRecordsImpl(InstrumentingAgents*, ExecutionContext*, MutationObserver*);

inline void willDeliverMutationRecords(ExecutionContext* paramExecutionContext, MutationObserver* paramMutationObserver)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        willDeliverMutationRecordsImpl(agents, paramExecutionContext, paramMutationObserver);
}

void didDeliverMutationRecordsImpl(InstrumentingAgents*);

inline void didDeliverMutationRecords(ExecutionContext* paramExecutionContext)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        didDeliverMutationRecordsImpl(agents);
}

InspectorInstrumentationCookie willEvaluateScriptImpl(InstrumentingAgents*, LocalFrame*, const String&, int);

inline InspectorInstrumentationCookie willEvaluateScript(LocalFrame* paramLocalFrame, const String& url, int lineNumber)
{   
    FAST_RETURN_IF_NO_FRONTENDS(InspectorInstrumentationCookie());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        return willEvaluateScriptImpl(agents, paramLocalFrame, url, lineNumber);
    return InspectorInstrumentationCookie();
}

void didEvaluateScriptImpl(const InspectorInstrumentationCookie&);

inline void didEvaluateScript(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (paramInspectorInstrumentationCookie.isValid())
        didEvaluateScriptImpl(paramInspectorInstrumentationCookie);
}

void didCreateIsolatedContextImpl(InstrumentingAgents*, LocalFrame*, ScriptState*, SecurityOrigin*);

inline void didCreateIsolatedContext(LocalFrame* paramLocalFrame, ScriptState* paramScriptState, SecurityOrigin* paramSecurityOrigin)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        didCreateIsolatedContextImpl(agents, paramLocalFrame, paramScriptState, paramSecurityOrigin);
}

InspectorInstrumentationCookie willFireTimerImpl(InstrumentingAgents*, ExecutionContext*, int);

inline InspectorInstrumentationCookie willFireTimer(ExecutionContext* paramExecutionContext, int timerId)
{   
    FAST_RETURN_IF_NO_FRONTENDS(InspectorInstrumentationCookie());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        return willFireTimerImpl(agents, paramExecutionContext, timerId);
    return InspectorInstrumentationCookie();
}

void didFireTimerImpl(const InspectorInstrumentationCookie&);

inline void didFireTimer(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (paramInspectorInstrumentationCookie.isValid())
        didFireTimerImpl(paramInspectorInstrumentationCookie);
}

void didInvalidateLayoutImpl(InstrumentingAgents*, LocalFrame*);

inline void didInvalidateLayout(LocalFrame* paramLocalFrame)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        didInvalidateLayoutImpl(agents, paramLocalFrame);
}

InspectorInstrumentationCookie willLayoutImpl(InstrumentingAgents*, LocalFrame*);

inline InspectorInstrumentationCookie willLayout(LocalFrame* paramLocalFrame)
{   
    FAST_RETURN_IF_NO_FRONTENDS(InspectorInstrumentationCookie());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        return willLayoutImpl(agents, paramLocalFrame);
    return InspectorInstrumentationCookie();
}

void didLayoutImpl(const InspectorInstrumentationCookie&, RenderObject*);

inline void didLayout(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie, RenderObject* root)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (paramInspectorInstrumentationCookie.isValid())
        didLayoutImpl(paramInspectorInstrumentationCookie, root);
}

void didScrollImpl(InstrumentingAgents*);

inline void didScroll(Page* paramPage)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramPage))
        didScrollImpl(agents);
}

void didResizeMainFrameImpl(InstrumentingAgents*);

inline void didResizeMainFrame(Page* paramPage)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramPage))
        didResizeMainFrameImpl(agents);
}

InspectorInstrumentationCookie willDispatchXHRLoadEventImpl(InstrumentingAgents*, ExecutionContext*, XMLHttpRequest*);

inline InspectorInstrumentationCookie willDispatchXHRLoadEvent(ExecutionContext* paramExecutionContext, XMLHttpRequest* paramXMLHttpRequest)
{   
    FAST_RETURN_IF_NO_FRONTENDS(InspectorInstrumentationCookie());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        return willDispatchXHRLoadEventImpl(agents, paramExecutionContext, paramXMLHttpRequest);
    return InspectorInstrumentationCookie();
}

void didDispatchXHRLoadEventImpl(const InspectorInstrumentationCookie&);

inline void didDispatchXHRLoadEvent(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (paramInspectorInstrumentationCookie.isValid())
        didDispatchXHRLoadEventImpl(paramInspectorInstrumentationCookie);
}

void willScrollLayerImpl(InstrumentingAgents*, RenderObject*);

inline void willScrollLayer(RenderObject* paramRenderObject)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramRenderObject))
        willScrollLayerImpl(agents, paramRenderObject);
}

void didScrollLayerImpl(InstrumentingAgents*);

inline void didScrollLayer(RenderObject* paramRenderObject)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramRenderObject))
        didScrollLayerImpl(agents);
}

void willPaintImpl(InstrumentingAgents*, RenderObject*, const GraphicsLayer*);

inline void willPaint(RenderObject* paramRenderObject, const GraphicsLayer* paramGraphicsLayer)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramRenderObject))
        willPaintImpl(agents, paramRenderObject, paramGraphicsLayer);
}

void didPaintImpl(InstrumentingAgents*, RenderObject*, const GraphicsLayer*, GraphicsContext*, const LayoutRect&);

inline void didPaint(RenderObject* paramRenderObject, const GraphicsLayer* paramGraphicsLayer, GraphicsContext* paramGraphicsContext, const LayoutRect& paramLayoutRect)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramRenderObject))
        didPaintImpl(agents, paramRenderObject, paramGraphicsLayer, paramGraphicsContext, paramLayoutRect);
}

void willPaintImageImpl(InstrumentingAgents*, RenderImage*);

inline void willPaintImage(RenderImage* paramRenderImage)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramRenderImage))
        willPaintImageImpl(agents, paramRenderImage);
}

void didPaintImageImpl(InstrumentingAgents*);

inline void didPaintImage(RenderImage* paramRenderImage)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramRenderImage))
        didPaintImageImpl(agents);
}

InspectorInstrumentationCookie willRecalculateStyleImpl(InstrumentingAgents*, Document*);

inline InspectorInstrumentationCookie willRecalculateStyle(Document* paramDocument)
{   
    FAST_RETURN_IF_NO_FRONTENDS(InspectorInstrumentationCookie());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        return willRecalculateStyleImpl(agents, paramDocument);
    return InspectorInstrumentationCookie();
}

void didRecalculateStyleImpl(const InspectorInstrumentationCookie&, int);

inline void didRecalculateStyle(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie, int elementCount)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (paramInspectorInstrumentationCookie.isValid())
        didRecalculateStyleImpl(paramInspectorInstrumentationCookie, elementCount);
}

void didScheduleStyleRecalculationImpl(InstrumentingAgents*, Document*);

inline void didScheduleStyleRecalculation(Document* paramDocument)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        didScheduleStyleRecalculationImpl(agents, paramDocument);
}

void applyUserAgentOverrideImpl(InstrumentingAgents*, String*);

inline void applyUserAgentOverride(LocalFrame* paramLocalFrame, String* userAgent)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        applyUserAgentOverrideImpl(agents, userAgent);
}

bool applyViewportStyleOverrideImpl(InstrumentingAgents*, StyleResolver*);

inline bool applyViewportStyleOverride(Document* paramDocument, StyleResolver* paramStyleResolver)
{   
    FAST_RETURN_IF_NO_FRONTENDS(false);
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        return applyViewportStyleOverrideImpl(agents, paramStyleResolver);
    return false;
}

void applyEmulatedMediaImpl(InstrumentingAgents*, String*);

inline void applyEmulatedMedia(LocalFrame* paramLocalFrame, String* media)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        applyEmulatedMediaImpl(agents, media);
}

void willSendRequestImpl(InstrumentingAgents*, unsigned long, DocumentLoader*, ResourceRequest&, const ResourceResponse&, const FetchInitiatorInfo&);

inline void willSendRequest(LocalFrame* paramLocalFrame, unsigned long identifier, DocumentLoader* paramDocumentLoader, ResourceRequest& paramResourceRequest, const ResourceResponse& redirectResponse, const FetchInitiatorInfo& paramFetchInitiatorInfo)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        willSendRequestImpl(agents, identifier, paramDocumentLoader, paramResourceRequest, redirectResponse, paramFetchInitiatorInfo);
}

void markResourceAsCachedImpl(InstrumentingAgents*, unsigned long);

inline void markResourceAsCached(Page* paramPage, unsigned long identifier)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramPage))
        markResourceAsCachedImpl(agents, identifier);
}

void didReceiveResourceResponseImpl(InstrumentingAgents*, LocalFrame*, unsigned long, DocumentLoader*, const ResourceResponse&, ResourceLoader*);

inline void didReceiveResourceResponse(LocalFrame* paramLocalFrame, unsigned long identifier, DocumentLoader* paramDocumentLoader, const ResourceResponse& paramResourceResponse, ResourceLoader* paramResourceLoader)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        didReceiveResourceResponseImpl(agents, paramLocalFrame, identifier, paramDocumentLoader, paramResourceResponse, paramResourceLoader);
}

void continueAfterXFrameOptionsDeniedImpl(LocalFrame*, DocumentLoader*, unsigned long, const ResourceResponse&);

inline void continueAfterXFrameOptionsDenied(LocalFrame* frame, DocumentLoader* loader, unsigned long identifier, const ResourceResponse& r)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    continueAfterXFrameOptionsDeniedImpl(frame, loader, identifier, r);
}

void continueWithPolicyDownloadImpl(LocalFrame*, DocumentLoader*, unsigned long, const ResourceResponse&);

inline void continueWithPolicyDownload(LocalFrame* frame, DocumentLoader* loader, unsigned long identifier, const ResourceResponse& r)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    continueWithPolicyDownloadImpl(frame, loader, identifier, r);
}

void continueWithPolicyIgnoreImpl(LocalFrame*, DocumentLoader*, unsigned long, const ResourceResponse&);

inline void continueWithPolicyIgnore(LocalFrame* frame, DocumentLoader* loader, unsigned long identifier, const ResourceResponse& r)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    continueWithPolicyIgnoreImpl(frame, loader, identifier, r);
}

void didReceiveDataImpl(InstrumentingAgents*, LocalFrame*, unsigned long, const char*, int, int);

inline void didReceiveData(LocalFrame* paramLocalFrame, unsigned long identifier, const char* data, int dataLength, int encodedDataLength)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        didReceiveDataImpl(agents, paramLocalFrame, identifier, data, dataLength, encodedDataLength);
}

void didFinishLoadingImpl(InstrumentingAgents*, unsigned long, DocumentLoader*, double, int64_t);

inline void didFinishLoading(LocalFrame* frame, unsigned long identifier, DocumentLoader* paramDocumentLoader, double finishTime, int64_t encodedDataLength)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(frame))
        didFinishLoadingImpl(agents, identifier, paramDocumentLoader, finishTime, encodedDataLength);
}

void didReceiveCORSRedirectResponseImpl(InstrumentingAgents*, LocalFrame*, unsigned long, DocumentLoader*, const ResourceResponse&, ResourceLoader*);

inline void didReceiveCORSRedirectResponse(LocalFrame* paramLocalFrame, unsigned long identifier, DocumentLoader* paramDocumentLoader, const ResourceResponse& paramResourceResponse, ResourceLoader* paramResourceLoader)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        didReceiveCORSRedirectResponseImpl(agents, paramLocalFrame, identifier, paramDocumentLoader, paramResourceResponse, paramResourceLoader);
}

void didFailLoadingImpl(InstrumentingAgents*, unsigned long, const ResourceError&);

inline void didFailLoading(LocalFrame* frame, unsigned long identifier, const ResourceError& paramResourceError)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(frame))
        didFailLoadingImpl(agents, identifier, paramResourceError);
}

void documentThreadableLoaderStartedLoadingForClientImpl(InstrumentingAgents*, unsigned long, ThreadableLoaderClient*);

inline void documentThreadableLoaderStartedLoadingForClient(ExecutionContext* paramExecutionContext, unsigned long identifier, ThreadableLoaderClient* client)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        documentThreadableLoaderStartedLoadingForClientImpl(agents, identifier, client);
}

void willLoadXHRImpl(InstrumentingAgents*, XMLHttpRequest*, ThreadableLoaderClient*, const AtomicString&, const KURL&, bool, PassRefPtr<FormData>, const HTTPHeaderMap&, bool);

inline void willLoadXHR(ExecutionContext* paramExecutionContext, XMLHttpRequest* xhr, ThreadableLoaderClient* client, const AtomicString& method, const KURL& url, bool async, PassRefPtr<FormData> paramFormData, const HTTPHeaderMap& headers, bool includeCredentials)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        willLoadXHRImpl(agents, xhr, client, method, url, async, paramFormData, headers, includeCredentials);
}

void didFailXHRLoadingImpl(InstrumentingAgents*, XMLHttpRequest*, ThreadableLoaderClient*);

inline void didFailXHRLoading(ExecutionContext* paramExecutionContext, XMLHttpRequest* xhr, ThreadableLoaderClient* client)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        didFailXHRLoadingImpl(agents, xhr, client);
}

void didFinishXHRLoadingImpl(InstrumentingAgents*, XMLHttpRequest*, ThreadableLoaderClient*, unsigned long, ScriptString, const AtomicString&, const String&, const String&, unsigned);

inline void didFinishXHRLoading(ExecutionContext* paramExecutionContext, XMLHttpRequest* xhr, ThreadableLoaderClient* client, unsigned long identifier, ScriptString sourceString, const AtomicString& method, const String& url, const String& sendURL, unsigned sendLineNumber)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        didFinishXHRLoadingImpl(agents, xhr, client, identifier, sourceString, method, url, sendURL, sendLineNumber);
}

void scriptImportedImpl(InstrumentingAgents*, unsigned long, const String&);

inline void scriptImported(ExecutionContext* paramExecutionContext, unsigned long identifier, const String& sourceString)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        scriptImportedImpl(agents, identifier, sourceString);
}

void scriptExecutionBlockedByCSPImpl(InstrumentingAgents*, const String&);

inline void scriptExecutionBlockedByCSP(ExecutionContext* paramExecutionContext, const String& directiveText)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        scriptExecutionBlockedByCSPImpl(agents, directiveText);
}

void didReceiveScriptResponseImpl(InstrumentingAgents*, unsigned long);

inline void didReceiveScriptResponse(ExecutionContext* paramExecutionContext, unsigned long identifier)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        didReceiveScriptResponseImpl(agents, identifier);
}

void domContentLoadedEventFiredImpl(InstrumentingAgents*, LocalFrame*);

inline void domContentLoadedEventFired(LocalFrame* paramLocalFrame)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        domContentLoadedEventFiredImpl(agents, paramLocalFrame);
}

void loadEventFiredImpl(InstrumentingAgents*, LocalFrame*);

inline void loadEventFired(LocalFrame* paramLocalFrame)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        loadEventFiredImpl(agents, paramLocalFrame);
}

void frameAttachedToParentImpl(InstrumentingAgents*, LocalFrame*);

inline void frameAttachedToParent(LocalFrame* paramLocalFrame)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        frameAttachedToParentImpl(agents, paramLocalFrame);
}

void frameDetachedFromParentImpl(InstrumentingAgents*, LocalFrame*);

inline void frameDetachedFromParent(LocalFrame* paramLocalFrame)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        frameDetachedFromParentImpl(agents, paramLocalFrame);
}

void didCommitLoadImpl(InstrumentingAgents*, LocalFrame*, DocumentLoader*);

inline void didCommitLoad(LocalFrame* paramLocalFrame, DocumentLoader* paramDocumentLoader)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        didCommitLoadImpl(agents, paramLocalFrame, paramDocumentLoader);
}

void frameDocumentUpdatedImpl(InstrumentingAgents*, LocalFrame*);

inline void frameDocumentUpdated(LocalFrame* paramLocalFrame)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        frameDocumentUpdatedImpl(agents, paramLocalFrame);
}

void loaderDetachedFromFrameImpl(InstrumentingAgents*, DocumentLoader*);

inline void loaderDetachedFromFrame(LocalFrame* paramLocalFrame, DocumentLoader* paramDocumentLoader)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        loaderDetachedFromFrameImpl(agents, paramDocumentLoader);
}

void frameStartedLoadingImpl(InstrumentingAgents*, LocalFrame*);

inline void frameStartedLoading(LocalFrame* paramLocalFrame)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        frameStartedLoadingImpl(agents, paramLocalFrame);
}

void frameStoppedLoadingImpl(InstrumentingAgents*, LocalFrame*);

inline void frameStoppedLoading(LocalFrame* paramLocalFrame)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        frameStoppedLoadingImpl(agents, paramLocalFrame);
}

void frameScheduledNavigationImpl(InstrumentingAgents*, LocalFrame*, double);

inline void frameScheduledNavigation(LocalFrame* paramLocalFrame, double delay)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        frameScheduledNavigationImpl(agents, paramLocalFrame, delay);
}

void frameClearedScheduledNavigationImpl(InstrumentingAgents*, LocalFrame*);

inline void frameClearedScheduledNavigation(LocalFrame* paramLocalFrame)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        frameClearedScheduledNavigationImpl(agents, paramLocalFrame);
}

InspectorInstrumentationCookie willRunJavaScriptDialogImpl(InstrumentingAgents*, const String&);

inline InspectorInstrumentationCookie willRunJavaScriptDialog(Page* paramPage, const String& message)
{   
    FAST_RETURN_IF_NO_FRONTENDS(InspectorInstrumentationCookie());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramPage))
        return willRunJavaScriptDialogImpl(agents, message);
    return InspectorInstrumentationCookie();
}

void didRunJavaScriptDialogImpl(const InspectorInstrumentationCookie&);

inline void didRunJavaScriptDialog(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (paramInspectorInstrumentationCookie.isValid())
        didRunJavaScriptDialogImpl(paramInspectorInstrumentationCookie);
}

void willDestroyResourceImpl(Resource*);

inline void willDestroyResource(Resource* cachedResource)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    willDestroyResourceImpl(cachedResource);
}

InspectorInstrumentationCookie willWriteHTMLImpl(InstrumentingAgents*, Document*, unsigned);

inline InspectorInstrumentationCookie willWriteHTML(Document* paramDocument, unsigned startLine)
{   
    FAST_RETURN_IF_NO_FRONTENDS(InspectorInstrumentationCookie());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        return willWriteHTMLImpl(agents, paramDocument, startLine);
    return InspectorInstrumentationCookie();
}

void didWriteHTMLImpl(const InspectorInstrumentationCookie&, unsigned);

inline void didWriteHTML(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie, unsigned endLine)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (paramInspectorInstrumentationCookie.isValid())
        didWriteHTMLImpl(paramInspectorInstrumentationCookie, endLine);
}

void didRequestAnimationFrameImpl(InstrumentingAgents*, Document*, int);

inline void didRequestAnimationFrame(Document* paramDocument, int callbackId)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        didRequestAnimationFrameImpl(agents, paramDocument, callbackId);
}

void didCancelAnimationFrameImpl(InstrumentingAgents*, Document*, int);

inline void didCancelAnimationFrame(Document* paramDocument, int callbackId)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        didCancelAnimationFrameImpl(agents, paramDocument, callbackId);
}

InspectorInstrumentationCookie willFireAnimationFrameImpl(InstrumentingAgents*, Document*, int);

inline InspectorInstrumentationCookie willFireAnimationFrame(Document* paramDocument, int callbackId)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        return willFireAnimationFrameImpl(agents, paramDocument, callbackId);
    return InspectorInstrumentationCookie();
}

void didFireAnimationFrameImpl(const InspectorInstrumentationCookie&);

inline void didFireAnimationFrame(const InspectorInstrumentationCookie& paramInspectorInstrumentationCookie)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (paramInspectorInstrumentationCookie.isValid())
        didFireAnimationFrameImpl(paramInspectorInstrumentationCookie);
}

void didDispatchDOMStorageEventImpl(InstrumentingAgents*, const String&, const String&, const String&, StorageType, SecurityOrigin*);

inline void didDispatchDOMStorageEvent(Page* page, const String& key, const String& oldValue, const String& newValue, StorageType storageType, SecurityOrigin* securityOrigin)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(page))
        didDispatchDOMStorageEventImpl(agents, key, oldValue, newValue, storageType, securityOrigin);
}

void didStartWorkerGlobalScopeImpl(InstrumentingAgents*, WorkerGlobalScopeProxy*, const KURL&);

inline void didStartWorkerGlobalScope(ExecutionContext* paramExecutionContext, WorkerGlobalScopeProxy* proxy, const KURL& url)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        didStartWorkerGlobalScopeImpl(agents, proxy, url);
}

void willEvaluateWorkerScriptImpl(InstrumentingAgents*, WorkerGlobalScope*, int);

inline void willEvaluateWorkerScript(WorkerGlobalScope* context, int workerThreadStartMode)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(context))
        willEvaluateWorkerScriptImpl(agents, context, workerThreadStartMode);
}

void workerGlobalScopeTerminatedImpl(InstrumentingAgents*, WorkerGlobalScopeProxy*);

inline void workerGlobalScopeTerminated(ExecutionContext* paramExecutionContext, WorkerGlobalScopeProxy* proxy)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramExecutionContext))
        workerGlobalScopeTerminatedImpl(agents, proxy);
}

void willProcessTaskImpl(InstrumentingAgents*);

inline void willProcessTask(WorkerGlobalScope* context)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(context))
        willProcessTaskImpl(agents);
}

void didProcessTaskImpl(InstrumentingAgents*);

inline void didProcessTask(WorkerGlobalScope* context)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(context))
        didProcessTaskImpl(agents);
}

void willEnterNestedRunLoopImpl(InstrumentingAgents*);

inline void willEnterNestedRunLoop(WorkerGlobalScope* context)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(context))
        willEnterNestedRunLoopImpl(agents);
}

void didLeaveNestedRunLoopImpl(InstrumentingAgents*);

inline void didLeaveNestedRunLoop(WorkerGlobalScope* context)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(context))
        didLeaveNestedRunLoopImpl(agents);
}

void didCreateWebSocketImpl(InstrumentingAgents*, Document*, unsigned long, const KURL&, const String&);

inline void didCreateWebSocket(Document* paramDocument, unsigned long identifier, const KURL& requestURL, const String& protocol)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        didCreateWebSocketImpl(agents, paramDocument, identifier, requestURL, protocol);
}

void willSendWebSocketHandshakeRequestImpl(InstrumentingAgents*, Document*, unsigned long, const WebSocketHandshakeRequest*);

inline void willSendWebSocketHandshakeRequest(Document* paramDocument, unsigned long identifier, const WebSocketHandshakeRequest* request)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        willSendWebSocketHandshakeRequestImpl(agents, paramDocument, identifier, request);
}

void didReceiveWebSocketHandshakeResponseImpl(InstrumentingAgents*, Document*, unsigned long, const WebSocketHandshakeRequest*, const WebSocketHandshakeResponse*);

inline void didReceiveWebSocketHandshakeResponse(Document* paramDocument, unsigned long identifier, const WebSocketHandshakeRequest* request, const WebSocketHandshakeResponse* response)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        didReceiveWebSocketHandshakeResponseImpl(agents, paramDocument, identifier, request, response);
}

void didCloseWebSocketImpl(InstrumentingAgents*, Document*, unsigned long);

inline void didCloseWebSocket(Document* paramDocument, unsigned long identifier)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        didCloseWebSocketImpl(agents, paramDocument, identifier);
}

void didReceiveWebSocketFrameImpl(InstrumentingAgents*, unsigned long, int, bool, const char*, size_t);

inline void didReceiveWebSocketFrame(Document* paramDocument, unsigned long identifier, int opCode, bool masked, const char* payload, size_t payloadLength)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        didReceiveWebSocketFrameImpl(agents, identifier, opCode, masked, payload, payloadLength);
}

void didSendWebSocketFrameImpl(InstrumentingAgents*, unsigned long, int, bool, const char*, size_t);

inline void didSendWebSocketFrame(Document* paramDocument, unsigned long identifier, int opCode, bool masked, const char* payload, size_t payloadLength)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        didSendWebSocketFrameImpl(agents, identifier, opCode, masked, payload, payloadLength);
}

void didReceiveWebSocketFrameErrorImpl(InstrumentingAgents*, unsigned long, const String&);

inline void didReceiveWebSocketFrameError(Document* paramDocument, unsigned long identifier, const String& errorMessage)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramDocument))
        didReceiveWebSocketFrameErrorImpl(agents, identifier, errorMessage);
}

void networkStateChangedImpl(InstrumentingAgents*, bool);

inline void networkStateChanged(Page* paramPage, bool online)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramPage))
        networkStateChangedImpl(agents, online);
}

void updateApplicationCacheStatusImpl(InstrumentingAgents*, LocalFrame*);

inline void updateApplicationCacheStatus(LocalFrame* paramLocalFrame)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        updateApplicationCacheStatusImpl(agents, paramLocalFrame);
}

void willUpdateLayerTreeImpl(InstrumentingAgents*);

inline void willUpdateLayerTree(LocalFrame* paramLocalFrame)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        willUpdateLayerTreeImpl(agents);
}

void layerTreeDidChangeImpl(InstrumentingAgents*);

inline void layerTreeDidChange(LocalFrame* paramLocalFrame)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        layerTreeDidChangeImpl(agents);
}

void didUpdateLayerTreeImpl(InstrumentingAgents*);

inline void didUpdateLayerTree(LocalFrame* paramLocalFrame)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramLocalFrame))
        didUpdateLayerTreeImpl(agents);
}

void pseudoElementCreatedImpl(InstrumentingAgents*, PseudoElement*);

inline void pseudoElementCreated(PseudoElement* paramPseudoElement)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramPseudoElement))
        pseudoElementCreatedImpl(agents, paramPseudoElement);
}

void pseudoElementDestroyedImpl(InstrumentingAgents*, PseudoElement*);

inline void pseudoElementDestroyed(PseudoElement* paramPseudoElement)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramPseudoElement))
        pseudoElementDestroyedImpl(agents, paramPseudoElement);
}

void willExecuteCustomElementCallbackImpl(InstrumentingAgents*, Element*);

inline void willExecuteCustomElementCallback(Element* paramElement)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(paramElement))
        willExecuteCustomElementCallbackImpl(agents, paramElement);
}

} // namespace InspectorInstrumentation

} // namespace WebCore

#endif // !defined(InspectorInstrumentationInl_h)
