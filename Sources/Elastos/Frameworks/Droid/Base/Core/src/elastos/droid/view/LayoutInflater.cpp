
#include <Elastos.CoreLibrary.External.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include "elastos/droid/view/View.h"
#ifdef DROID_CORE
#include "elastos/droid/os/CHandler.h"
// #include "elastos/droid/widget/CBlinkLayout.h"
#include "elastos/droid/view/CContextThemeWrapper.h"
#endif

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::IO::ICloseable;
using Elastos::Core::AutoLock;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Core::StringUtils;
using Elastos::Core::IClassLoader;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IHashMap;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::EIID_IHandlerCallback;
// using Elastos::Droid::Widget::CBlinkLayout;
using Elastos::Droid::Widget::IFrameLayout;

namespace Elastos {
namespace Droid {
namespace View {

#define LAYOUT_INFLATOR_CATCH_EXCEPTION1(expr) \
    do { \
        ECode ec = expr; \
        if (ec == (Int32)E_NO_SUCH_METHOD_EXCEPTION) { \
            Slogger::D(TAG, "LAYOUT_INFLATOR_CATCH_EXCEPTION1: %s, Error: E_NO_SUCH_METHOD_EXCEPTION.", name.string()); \
            /*InflateException ie = new InflateException(attrs.getPositionDescription()*/ \
                  /*+ ": Error inflating class "*/ \
                  /*+ (prefix != null ? (prefix + name) : name));*/ \
            /*ie.initCause(e);*/ \
            return ec; \
        } \
        else if (ec == (Int32)E_CLASS_NOT_FOUND_EXCEPTION) { \
            Slogger::D(TAG, "LAYOUT_INFLATOR_CATCH_EXCEPTION1: %s, Error: E_CLASS_NOT_FOUND_EXCEPTION.", name.string()); \
            /* If loadClass fails, we should propagate the exception. */ \
            return ec; \
        } \
        else if (FAILED(ec)) { \
            Slogger::D(TAG, "LAYOUT_INFLATOR_CATCH_EXCEPTION1: %s, Error: %08x.", name.string(), ec); \
            /*InflateException ie = new InflateException(attrs.getPositionDescription()*/ \
                    /*+ ": Error inflating class "*/ \
                    /*+ (clazz == null ? "<unknown>" : clazz.getName()));*/ \
            /*ie.initCause(e);*/ \
            return ec; \
        } \
    } while(0);


#define LAYOUT_INFLATOR_CATCH_EXCEPTION2(expr) \
    do { \
        ECode ec = expr; \
        if (ec == (Int32)E_XML_PULL_PARSER_EXCEPTION) { \
            Slogger::D(TAG, "LAYOUT_INFLATOR_CATCH_EXCEPTION2: Error: E_XML_PULL_PARSER_EXCEPTION."); \
            /*InflateException ex = new InflateException(e.getMessage());*/ \
            /*ie.initCause(e);*/ \
            mConstructorArgs->Set(0, lastContext); \
            mConstructorArgs->Set(1, NULL); \
            return ec; \
        } \
        else if (ec == (Int32)E_IO_EXCEPTION) { \
            Slogger::D(TAG, "LAYOUT_INFLATOR_CATCH_EXCEPTION2: Error: E_IO_EXCEPTION."); \
            /*InflateException ie = new InflateException(*/ \
                    /*parser.getPositionDescription()*/ \
                    /*+ ": " + e.getMessage());*/ \
            /*ie.initCause(e);*/ \
            mConstructorArgs->Set(0, lastContext); \
            mConstructorArgs->Set(1, NULL); \
            return ec; \
        } \
        else if (FAILED(ec)) { \
            Slogger::D(TAG, "LAYOUT_INFLATOR_CATCH_EXCEPTION2: Error: %08x.", ec); \
            mConstructorArgs->Set(0, lastContext); \
            mConstructorArgs->Set(1, NULL); \
            return ec; \
        } \
    } while(0);


#define LAYOUT_INFLATOR_CATCH_EXCEPTION3(expr) \
    do { \
        ECode ec = expr; \
        if (ec == (Int32)E_INFLATE_EXCEPTION) { \
            Slogger::D(TAG, "LAYOUT_INFLATOR_CATCH_EXCEPTION3: %s, Error: E_INFLATE_EXCEPTION.", name.string()); \
            return ec; \
        } \
        else if (ec == (Int32)E_CLASS_NOT_FOUND_EXCEPTION) { \
            Slogger::D(TAG, "LAYOUT_INFLATOR_CATCH_EXCEPTION3: %s, Error: E_CLASS_NOT_FOUND_EXCEPTION.", name.string()); \
            /*InflateException ie = new InflateException(attrs.getPositionDescription()*/ \
                    /*+ ": Error inflating class " + name);*/ \
            /*ie.initCause(e);*/ \
            return ec;; \
        } \
        else if (FAILED(ec)) { \
            Slogger::D(TAG, "LAYOUT_INFLATOR_CATCH_EXCEPTION3: %s, Error: %08x.", name.string(), ec); \
            /*InflateException ie = new InflateException(attrs.getPositionDescription()*/ \
                    /*+ ": Error inflating class " + name);*/ \
            /*ie.initCause(e);*/ \
            return ec;; \
        } \
    } while(0);

#define LAYOUT_INFLATOR_CATCH_EXCEPTION3_WITH_FINALLY(expr, finallyCode) \
    do { \
        ECode ec = expr; \
        if (ec == (Int32)E_INFLATE_EXCEPTION) { \
            Slogger::D(TAG, "LAYOUT_INFLATOR_CATCH_EXCEPTION3: %s, Error: E_INFLATE_EXCEPTION.", name.string()); \
            finallyCode; \
            return ec; \
        } \
        else if (ec == (Int32)E_CLASS_NOT_FOUND_EXCEPTION) { \
            Slogger::D(TAG, "LAYOUT_INFLATOR_CATCH_EXCEPTION3: %s, Error: E_CLASS_NOT_FOUND_EXCEPTION.", name.string()); \
            /*InflateException ie = new InflateException(attrs.getPositionDescription()*/ \
                    /*+ ": Error inflating class " + name);*/ \
            /*ie.initCause(e);*/ \
            finallyCode; \
            return ec;; \
        } \
        else if (FAILED(ec)) { \
            Slogger::D(TAG, "LAYOUT_INFLATOR_CATCH_EXCEPTION3: %s, Error: %08x.", name.string(), ec); \
            /*InflateException ie = new InflateException(attrs.getPositionDescription()*/ \
                    /*+ ": Error inflating class " + name);*/ \
            /*ie.initCause(e);*/ \
            finallyCode; \
            return ec;; \
        } \
        finallyCode; \
    } while(0);

#define FAIL_RETURN_WITH_CLOSE(expr, parser) \
    do { \
        ECode ec = expr; \
        if (FAILED(ec)) { \
            AutoPtr<ICloseable> cls = ICloseable::Probe(parser); \
            if (cls != NULL) \
                cls->Close(); \
            return ec; \
        } \
    } while(0);

const String LayoutInflater::TAG("LayoutInflater");
const String LayoutInflater::TAG_TAG("tag");
const String LayoutInflater::TAG_MERGE("merge");
const String LayoutInflater::TAG_INCLUDE("include");
const String LayoutInflater::TAG_1995("blink");
const String LayoutInflater::TAG_REQUEST_FOCUS("requestFocus");

/*
 * to be careful: the value of mConstructorSignature should be
 * be consistent with constructor methods of all widgets declared
 * in *.car.
 */
String LayoutInflater::mConstructorSignature("CtxAttrs");

static AutoPtr<IHashMap> InitMap()
{
    AutoPtr<IHashMap> rst;
    CHashMap::New(30, (IHashMap**)&rst);
    return rst;
}

AutoPtr<IHashMap> LayoutInflater::sConstructorMap = InitMap();

CAR_INTERFACE_IMPL(LayoutInflater::BlinkLayout::HandlerCallback, Object, IHandlerCallback)

LayoutInflater::BlinkLayout::HandlerCallback::HandlerCallback(
    /* [in] */ BlinkLayout* host)
    : mHost(host)
{}

ECode LayoutInflater::BlinkLayout::HandlerCallback::HandleMessage(
    /* [in] */ IMessage* msg,
    /* [in] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    if (what == MESSAGE_BLINK) {
        if (mHost->mBlink) {
            mHost->mBlinkState = !mHost->mBlinkState;
            mHost->MakeBlink();
        }
        mHost->Invalidate();
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(LayoutInflater::BlinkLayout, FrameLayout, IBlinkLayout)

LayoutInflater::BlinkLayout::BlinkLayout()
    : mBlink(FALSE)
    , mBlinkState(FALSE)
{}

ECode LayoutInflater::BlinkLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr<IHandlerCallback> callback = new HandlerCallback(this);
    CHandler::New(callback, TRUE, FALSE, (IHandler**)&mHandler);
    return NOERROR;
}

ECode LayoutInflater::BlinkLayout::OnAttachedToWindow()
{
    FrameLayout::OnAttachedToWindow();

    mBlink = TRUE;
    mBlinkState = TRUE;

    MakeBlink();
    return NOERROR;
}

ECode LayoutInflater::BlinkLayout::OnDetachedFromWindow()
{
    FrameLayout::OnDetachedFromWindow();

    mBlink = FALSE;
    mBlinkState = TRUE;

    return mHandler->RemoveMessages(MESSAGE_BLINK);
}

void LayoutInflater::BlinkLayout::DispatchDraw(
        /* [in] */ ICanvas* canvas)
{
    if (mBlinkState) {
        FrameLayout::DispatchDraw(canvas);
    }
}

void LayoutInflater::BlinkLayout::MakeBlink()
{
        AutoPtr<IMessage> message;
        mHandler->ObtainMessage(MESSAGE_BLINK, (IMessage**)&message);
        Boolean res;
        mHandler->SendMessageDelayed(message, BLINK_DELAY, &res);
}

//=======================================================================================
// LayoutInflater::FactoryMerger
//=======================================================================================

CAR_INTERFACE_IMPL_2(LayoutInflater::FactoryMerger, Object, ILayoutInflaterFactory, ILayoutInflaterFactory2)
CAR_INTERFACE_IMPL(LayoutInflater, Object, ILayoutInflater)

LayoutInflater::FactoryMerger::FactoryMerger(
    /* [in] */ ILayoutInflaterFactory* f1,
    /* [in] */ ILayoutInflaterFactory2* f12,
    /* [in] */ ILayoutInflaterFactory* f2,
    /* [in] */ ILayoutInflaterFactory2* f22)
    : mF1(f1)
    , mF2(f2)
    , mF12(f12)
    , mF22(f22)
{
}

ECode LayoutInflater::FactoryMerger::OnCreateView(
    /* [in] */ const String& name,
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    AutoPtr<IView> v;
    ECode ec = mF1->OnCreateView(name, context, attrs, (IView**)&v);
    if (v == NULL) {
        ec = mF2->OnCreateView(name, context, attrs, (IView**)&v);
    }

    *view = v;
    REFCOUNT_ADD(*view);
    return ec;
}

ECode LayoutInflater::FactoryMerger::OnCreateView(
    /* [in] */ IView* parent,
    /* [in] */ const String& name,
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    ECode ec = NOERROR;
    AutoPtr<IView> v;
    if (mF12 != NULL) {
        ec = mF12->OnCreateView(parent, name, context, attrs, (IView**)&v);
    }
    else {
        ec = mF1->OnCreateView(name, context, attrs, (IView**)&v);
    }

    if (v == NULL) {
        if (mF22 != NULL) {
            ec = mF22->OnCreateView(parent, name, context, attrs, (IView**)&v);
        }
        else {
            ec = mF2->OnCreateView(name, context, attrs, (IView**)&v);
        }
    }

    *view = v;
    REFCOUNT_ADD(*view);
    return ec;
}

//=======================================================================================
// LayoutInflater
//=======================================================================================
LayoutInflater::LayoutInflater()
    : DEBUG(FALSE)
    , mContext(NULL)
    , mFactorySet(FALSE)
    , mPrivateFactory(NULL)
{
    mConstructorArgs = ArrayOf<IInterface* >::Alloc(2);
}

LayoutInflater::~LayoutInflater()
{
    mConstructorArgs = NULL;
    mFactory = NULL;
    mFactory2 = NULL;
    mFilter = NULL;
    mFilterMap = NULL;
}

ECode LayoutInflater::From(
    /* [in] */ IContext* context,
    /* [out] */ ILayoutInflater** inflater)
{
    VALIDATE_NOT_NULL(inflater);
    *inflater = NULL;

    AutoPtr<IInterface> tmp;
    ECode ec = context->GetSystemService(
            IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&tmp);
    if (FAILED(ec) || tmp == NULL) {
        Slogger::W(TAG, "LayoutInflater not found.");
        return E_ASSERTION_ERROR;
    }

    *inflater = ILayoutInflater::Probe(tmp);
    REFCOUNT_ADD(*inflater)
    return ec;
}

/**
 * Return the context we are running in, for access to resources, class
 * loader, etc.
 */
ECode LayoutInflater::GetContext(
    /* [in] */ IContext** context)
{
    VALIDATE_NOT_NULL(context)
    *context = mContext;
    REFCOUNT_ADD(*context)
    return NOERROR;
}

ECode LayoutInflater::GetFactory(
    /* [out] */ ILayoutInflaterFactory** factory)
{
    VALIDATE_NOT_NULL(factory)
    *factory = mFactory;
    REFCOUNT_ADD(*factory)
    return NOERROR;
}

ECode LayoutInflater::GetFactory2(
    /* [out] */ ILayoutInflaterFactory2** factory)
{
    VALIDATE_NOT_NULL(factory);
    *factory = mFactory2;
    REFCOUNT_ADD(*factory)
    return NOERROR;
}

ECode LayoutInflater::SetFactory(
    /* [in] */ ILayoutInflaterFactory* factory)
{
    if (mFactorySet) {
        Slogger::E(TAG, "A factory has already been set on this LayoutInflater");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (factory == NULL) {
        Slogger::E(TAG, "Given factory can not be null");
        return E_NULL_POINTER_EXCEPTION;
    }

    mFactorySet = TRUE;
    if (mFactory == NULL) {
        mFactory = factory;
    }
    else {
        AutoPtr<ILayoutInflaterFactory> old = mFactory;
        mFactory = new FactoryMerger(factory, NULL, old, mFactory2);
    }
    return NOERROR;
}

ECode LayoutInflater::SetFactory2(
    /* [in] */ ILayoutInflaterFactory2* factory)
{
    if (mFactorySet) {
        Slogger::E(TAG, "A factory has already been set on this LayoutInflater");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (factory == NULL) {
        Slogger::E(TAG, "Given factory can not be null");
        return E_NULL_POINTER_EXCEPTION;
    }

    mFactorySet = TRUE;
    if (mFactory == NULL) {
        mFactory = ILayoutInflaterFactory::Probe(factory);
        mFactory2 = factory;
    }
    else {
        AutoPtr<ILayoutInflaterFactory> old = mFactory;
        mFactory = new FactoryMerger(ILayoutInflaterFactory::Probe(factory), factory, old, mFactory2);
        mFactory2 = ILayoutInflaterFactory2::Probe(mFactory);
    }
    return NOERROR;
}

ECode LayoutInflater::SetPrivateFactory(
    /* [in] */ ILayoutInflaterFactory2* factory)
{
    if (mPrivateFactory == NULL) {
        mPrivateFactory = factory;
    } else {
        mPrivateFactory = new FactoryMerger(ILayoutInflaterFactory::Probe(factory),
            factory, ILayoutInflaterFactory::Probe(mPrivateFactory), mPrivateFactory);
    }
    return NOERROR;
}

ECode LayoutInflater::GetFilter(
    /* [out] */ ILayoutInflaterFilter** filter)
{
    VALIDATE_NOT_NULL(filter)
    *filter = mFilter;
    REFCOUNT_ADD(*filter);
    return NOERROR;
}

ECode LayoutInflater::SetFilter(
    /* [in] */ ILayoutInflaterFilter* filter)
{
    mFilter = filter;
    if (filter != NULL) {
        mFilterMap = NULL;
        CHashMap::New((IHashMap**)&mFilterMap);
    }
    return NOERROR;
}

/**
 * Inflate a new view hierarchy from the specified xml resource. Throws
 * {@link InflateException} if there is an error.
 *
 * @param resource ID for an XML layout resource to load (e.g.,
 *        <code>R.layout.main_page</code>)
 * @param root Optional view to be the parent of the generated hierarchy.
 * @return The root View of the inflated hierarchy. If root was supplied,
 *         this is the root View; otherwise it is the root of the inflated
 *         XML file.
 */
ECode LayoutInflater::Inflate(
    /* [in] */ Int32 resource,
    /* [in] */ IViewGroup* root,
    /* [out] */ IView** view)
{
    return Inflate(resource, root, root != NULL, view);
}

ECode LayoutInflater::Inflate(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IViewGroup* root,
    /* [out] */ IView** view)
{
    return Inflate(parser, root, root != NULL, view);
}

ECode LayoutInflater::Inflate(
    /* [in] */ Int32 resource,
    /* [in] */ IViewGroup* root,
    /* [in] */ Boolean attachToRoot,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)

    if (DEBUG) {
        Slogger::D(TAG, "Inflate from resource: %08x", resource);
    }

    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IResources> res;
    ASSERT_SUCCEEDED(ctx->GetResources((IResources**)&res));
    AutoPtr<IXmlResourceParser> parser;
    ASSERT_SUCCEEDED(res->GetLayout(resource, (IXmlResourceParser**)&parser));
    ECode ec = Inflate(IXmlPullParser::Probe(parser), root, attachToRoot, view);
    parser->Close();
    return ec;
}

ECode LayoutInflater::Inflate(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IViewGroup* root,
    /* [in] */ Boolean attachToRoot,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)

    AutoLock lock(mConstructorArgsLock);
    AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(parser);
    AutoPtr<IContext> lastContext = (IContext*)((*mConstructorArgs)[0]);
    mConstructorArgs->Set(0, (IInterface*)mContext);
    AutoPtr<IView> result = root != NULL? IView::Probe(root) : NULL;

//    try {
    // Look for the root node.
    Int32 type;
    FAIL_RETURN_WITH_CLOSE(parser->Next(&type), parser)
    while ((type != IXmlPullParser::START_TAG) &&
            type != IXmlPullParser::END_DOCUMENT) {
        // Empty
        FAIL_RETURN_WITH_CLOSE(parser->Next(&type), parser)
    }

    if (type != IXmlPullParser::START_TAG) {
        Slogger::E(TAG, "InflateException No start tag found!");
        mConstructorArgs->Set(0, lastContext);
        mConstructorArgs->Set(1, NULL);
        return E_INFLATE_EXCEPTION;
    }

    String name;
    parser->GetName(&name);
    Int32 tmpIndex = name.LastIndexOf('.');
    if (tmpIndex >= 0) {
        //Get the last word.
        Int32 tmpLen = name.GetLength() - tmpIndex + 1;

        assert(tmpLen > 0);
        name = name.Substring(tmpIndex + 1, name.GetLength());
    }

    if (DEBUG) {
        Slogger::D(TAG, "**************************");
        Slogger::D(TAG, "Creating root view: %s", name.string());
        Slogger::D(TAG, "**************************");
    }

    if (name.Equals(TAG_MERGE)) {
        if (root == NULL || !attachToRoot) {
            Slogger::E(TAG, "<merge /> can be used only with a valid ViewGroup root and attachToRoot=true");
            mConstructorArgs->Set(0, lastContext);
            mConstructorArgs->Set(1, NULL);
            return E_INFLATE_EXCEPTION;
        }

        LAYOUT_INFLATOR_CATCH_EXCEPTION2(RInflate(parser, IView::Probe(root), attrs.Get(), FALSE, FALSE))
    }
    else {
        // Temp is the root view that was found in the xml
        AutoPtr<IView> temp;
        LAYOUT_INFLATOR_CATCH_EXCEPTION2(CreateViewFromTag(IView::Probe(root), name, attrs, FALSE, (IView**)&temp))

        AutoPtr<IViewGroupLayoutParams> params;

        if (root != NULL) {
            if (DEBUG) {
                Slogger::D(TAG, "Creating params from root: %p", root);
            }
            // Create layout params that match root, if supplied
            root->GenerateLayoutParams(attrs, (IViewGroupLayoutParams**)&params);
            assert(params.Get());
            if (!attachToRoot) {
                // Set the layout params for temp if we are not
                // attaching. (If we are, we use addView, below)
                temp->SetLayoutParams(params.Get());
            }
        }

        if (DEBUG) {
            Slogger::D(TAG, "-----> start inflating children");
        }
        // Inflate all children under temp
        LAYOUT_INFLATOR_CATCH_EXCEPTION2(RInflate(parser, temp, attrs, TRUE, TRUE));
        if (DEBUG) {
            Slogger::D(TAG, "-----> done inflating children");
        }

        // We are supposed to attach all the views we found (int temp)
        // to root. Do that now.
        if (root != NULL && attachToRoot) {
            root->AddView(temp, params);
        }

        // Decide whether to return the root that was passed in or the
        // top view found in xml.
        if (root == NULL || !attachToRoot) {
            result = temp;
        }
    }

//    } catch (XmlPullParserException e) {
//        InflateException ex = new InflateException(e.getMessage());
//        ex.initCause(e);
//        throw ex;
//    } catch (IOException e) {
//        InflateException ex = new InflateException(
//                parser.getPositionDescription()
//                + ": " + e.getMessage());
//        ex.initCause(e);
//        throw ex;
//    } finally {
//        // Don't retain static reference on context.
//        mConstructorArgs[0] = lastContext;
//        mConstructorArgs[1] = null;
//    }

    *view = result.Get();
    REFCOUNT_ADD(*view);

    mConstructorArgs->Set(0, lastContext);
    mConstructorArgs->Set(1, NULL);
    return NOERROR;
}

ECode LayoutInflater::CreateView(
    /* [in] */ const String& name,
    /* [in] */ const String& prefix_,
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IView** view)
{
    String prefix = prefix_;
    if (name.Equals("TabletStatusBarView") || name.Equals("NotificationPanel")) {
        prefix = "Elastos.Droid.SystemUI.StatusBar.Tablet.";
    }
    else if (name.Equals("ActionMenuView")) {
        prefix = "Elastos.Droid.View.Menu.";
    }

    AutoPtr<IConstructorInfo> constructor;
    AutoPtr<IClassInfo> clazz;

//    try {
    AutoPtr<ICharSequence> csq;
    CString::New(name, (ICharSequence**)&csq);
    AutoPtr<IInterface> value;
    sConstructorMap->Get(csq, (IInterface**)&value);

    if (value == NULL) {
        // Class not found in the cache, see if it's real, and try to add it
        AutoPtr<IClassLoader> cl;
        // LAYOUT_INFLATOR_CATCH_EXCEPTION1(mContext->GetClassLoader((IClassLoader**)&cl))
        if (!prefix.IsNull()) {
            LAYOUT_INFLATOR_CATCH_EXCEPTION1(cl->LoadClass(prefix + name, (IClassInfo**)&clazz));
        }
        else {
            LAYOUT_INFLATOR_CATCH_EXCEPTION1(cl->LoadClass(name, (IClassInfo**)&clazz));
        }

        if (mFilter != NULL && clazz != NULL) {
            Boolean allowed;
            mFilter->OnLoadClass(clazz, &allowed);
            if (!allowed) {
                LAYOUT_INFLATOR_CATCH_EXCEPTION1(FailNotAllowed(name, prefix, attrs));
            }
        }

        LAYOUT_INFLATOR_CATCH_EXCEPTION1(clazz->GetConstructorInfoByParamNames(
                mConstructorSignature, (IConstructorInfo**)&constructor));
        sConstructorMap->Put(csq, constructor.Get());
    }
    else {
        constructor = IConstructorInfo::Probe(value);
        // If we have a filter, apply it to cached constructor
        if (mFilter != NULL) {
            // Have we seen this name before?
            Boolean allowedState;
            AutoPtr<IInterface> stateTmp;
            mFilterMap->Get(csq, (IInterface**)&stateTmp);
            if (stateTmp == NULL) {
                // New class -- remember whether it is allowed
                AutoPtr<IClassLoader> cl;
                // LAYOUT_INFLATOR_CATCH_EXCEPTION1(mContext->GetClassLoader((IClassLoader**)&cl))
                if (!prefix.IsNull()) {
                    LAYOUT_INFLATOR_CATCH_EXCEPTION1(cl->LoadClass(prefix + name, (IClassInfo**)&clazz));
                }
                else {
                    LAYOUT_INFLATOR_CATCH_EXCEPTION1(cl->LoadClass(name, (IClassInfo**)&clazz));
                }

                Boolean allowed;
                mFilter->OnLoadClass(clazz, &allowed);
                allowed = allowed && (clazz != NULL);
                AutoPtr<IBoolean> stateInterface;
                CBoolean::New(allowed, (IBoolean**)&stateInterface);
                AutoPtr<IInterface> resultTmp;
                mFilterMap->Put(csq, stateInterface, (IInterface**)&resultTmp);
                if (!allowed) {
                    LAYOUT_INFLATOR_CATCH_EXCEPTION1(FailNotAllowed(name, prefix, attrs));
                }
            }
            else {
                AutoPtr<IBoolean> state = IBoolean::Probe(stateTmp);
                state->GetValue(&allowedState);
                if (!allowedState) {
                    LAYOUT_INFLATOR_CATCH_EXCEPTION1(FailNotAllowed(name, prefix, attrs));
                }
            }
        }
    }

    mConstructorArgs->Set(1, attrs);

    AutoPtr<IArgumentList> args;
    LAYOUT_INFLATOR_CATCH_EXCEPTION1(constructor->CreateArgumentList((IArgumentList**)&args));
    LAYOUT_INFLATOR_CATCH_EXCEPTION1(args->SetInputArgumentOfObjectPtr(0, (*mConstructorArgs)[0]));
    LAYOUT_INFLATOR_CATCH_EXCEPTION1(args->SetInputArgumentOfObjectPtr(1, (*mConstructorArgs)[1]));

    AutoPtr<IInterface> obj;
    LAYOUT_INFLATOR_CATCH_EXCEPTION1(constructor->CreateObject(args, (IInterface**)&obj));
    *view = IView::Probe(obj);
    assert(*view != NULL);
    REFCOUNT_ADD(*view);

    if (IViewStub::Probe(*view) != NULL) {
        // always use ourselves when inflating ViewStub later
        AutoPtr<ILayoutInflater> paraInflater;
        CloneInContext(IContext::Probe((*mConstructorArgs)[0]), (ILayoutInflater**)&paraInflater);
        IViewStub::Probe(*view)->SetLayoutInflater(paraInflater);
    }

#if defined(_DEBUG) || defined(_ELASTOS_DEBUG)
    // Int32 viewId;
    // (*view)->GetId(&viewId);
    // Slogger::D(TAG, "Created view: %p, id: 0x%08x, type: %s%s ",
    //     *view, viewId, prefix.IsNull() ? "" : prefix.string(), name.string());
#endif
    return NOERROR;

//    } catch (NoSuchMethodException e) {
//        InflateException ie = new InflateException(attrs.getPositionDescription()
//                + ": Error inflating class "
//                + (prefix != null ? (prefix + name) : name));
//        ie.initCause(e);
//        throw ie;
// } catch (ClassCastException e) {
//     // If loaded class is not a View subclass
//     InflateException ie = new InflateException(attrs.getPositionDescription()
//             + ": Class is not a View "
//             + (prefix != null ? (prefix + name) : name));
//     ie.initCause(e);
//     throw ie;
//
//    } catch (ClassNotFoundException e) {
//        // If loadClass fails, we should propagate the exception.
//        throw e;
//    } catch (Exception e) {
//        InflateException ie = new InflateException(attrs.getPositionDescription()
//                + ": Error inflating class "
//                + (clazz == null ? "<unknown>" : clazz.getName()));
//        ie.initCause(e);
//        throw ie;
//    }
}

ECode LayoutInflater::FailNotAllowed(
    /* [in] */ const String& name,
    /* [in] */ const String& prefix,
    /* [in] */ IAttributeSet* attrs)
{
    Slogger::E(TAG, "Class not allowed to be inflated: %s%s", prefix.string(), name.string());
    return E_INFLATE_EXCEPTION;
}

ECode LayoutInflater::OnCreateView(
    /* [in] */ const String& name,
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IView** view)
{
    return CreateView(name, String("Elastos.Droid.Widget."), attrs, view);
}

ECode LayoutInflater::OnCreateView(
    /* [in] */ IView* parent,
    /* [in] */ const String& name,
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IView** view)
{
    return OnCreateView(name, attrs, view);
}

static String Replace(const String& name, const char* regix, const char* replacement)
{
    String result;
    StringUtils::ReplaceAll(name, String(regix), String(replacement), &result);
    return result;
}

String LayoutInflater::ConvertViewName(
    /* [in] */ const String& nameIn)
{
    String name(nameIn.string());
    if (name.IndexOf("$") >= 0) {
        // inner class case: systemui.statusbar.tablet.NotificationIconArea$IconLayout
        // NotificationIconArea$IconLayout to NotificationIconAreaIconLayout
        name = Replace(name, "\\$", "");
    }

    if (name.StartWith("com.android.internal.widget.")) {
        return Replace(name, "com.android.internal.widget.",
            "Elastos.Droid.Widget.Internal.");
    }
    // else if (name.StartWith("com.android.systemui.statusbar.tablet.NotificationIconArea$IconLayout")) {
    //     return Replace(name, "com.android.systemui.statusbar.tablet.NotificationIconArea$IconLayout",
    //         "Elastos.Droid.SystemUI.StatusBar.Tablet.NotificationIconAreaIconLayout");
    // }
    else if (name.StartWith("com.android.systemui.statusbar.tablet.")) {
        return Replace(name, "com.android.systemui.statusbar.tablet.",
            "Elastos.Droid.SystemUI.StatusBar.Tablet.");
    }
    else if (name.StartWith("com.android.internal.view.menu.")) {
        return Replace(name, "com.android.internal.view.menu.",
            "Elastos.Droid.View.Menu.");
    }
    else if (name.StartWith("com.android.systemui.statusbar.phone.")) {
        return Replace(name, "com.android.systemui.statusbar.phone.",
            "Elastos.Droid.SystemUI.StatusBar.Phone.");
    }
    else if (name.StartWith("com.android.systemui.statusbar.policy.")) {
        return Replace(name, "com.android.systemui.statusbar.policy.",
            "Elastos.Droid.SystemUI.StatusBar.Policy.");
    }
    else if (name.StartWith("com.android.systemui.statusbar.")) {
        return Replace(name, "com.android.systemui.statusbar.",
            "Elastos.Droid.SystemUI.StatusBar.");
    }
    else if (name.StartWith("com.android.systemui.recent.")) {
        return Replace(name, "com.android.systemui.recent.",
            "Elastos.Droid.SystemUI.Recent.");
    }
    else if (name.StartWith("com.android.systemui.")) {
        assert(0 && "TODO");
    }
    else if(name.Equals("com.android.internal.app.AlertControllerRecycleListView")) {
        return String("Elastos.Droid.Widget.RecycleListView");
    }
    else if (name.StartWith("android.preference.")) {
        return Replace(name, "android.preference.",
            "Elastos.Droid.Preference.");
    }
    else if (name.StartWith("android.opengl.")) {
        return Replace(name, "android.opengl.",
            "Elastos.Droid.Opengl.");
    }
    return name;
}

ECode LayoutInflater::CreateViewFromTag(
    /* [in] */ IView* parent,
    /* [in] */ const String& _name,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Boolean inheritContext,
    /* [out] */ IView** view)
{
    String name;
    if (!_name.Compare("view")) {
        attrs->GetAttributeValue(String(NULL), String("class"), &name);
    }
    else {
        name = _name;
    }

    name = ConvertViewName(name);

    if (DEBUG) {
        Slogger::D(TAG, "******** Creating view: %s, mFactory2: %p, mFactory: %p, mPrivateFactory: %p",
            name.string(),  mFactory2.Get(), mFactory.Get(), mPrivateFactory);
    }

    AutoPtr<IContext> viewContext;
    if (parent != NULL && inheritContext) {
        parent->GetContext((IContext**)&viewContext);
    } else {
        viewContext = mContext;
    }

    // Apply a theme wrapper, if requested.
    AutoPtr<ITypedArray> ta;
    AutoPtr<ArrayOf<Int32> > ATTRS_THEME = ArrayOf<Int32>::Alloc(1);
    (*ATTRS_THEME)[0] = R::attr::theme;
    viewContext->ObtainStyledAttributes(attrs, ATTRS_THEME, (ITypedArray**)&ta);
    Int32 themeResId;
    ta->GetResourceId(0, 0, &themeResId);
    if (themeResId != 0) {
        CContextThemeWrapper::New(viewContext, themeResId, (IContext**)&viewContext);
    }
    ta->Recycle();

    if (name.Equals(TAG_1995)) {
        // Let's party like it's 1995!
        // CBlinkLayout::New(viewContext, attrs, view);
    }

//    try {
    if (mFactory2 != NULL) {
        mFactory2->OnCreateView(parent, name, viewContext, attrs, view);
    }
    else if (mFactory != NULL) {
        mFactory->OnCreateView(name, viewContext, attrs, view);
    }
    else {
        *view = NULL;
    }

    if (*view == NULL && mPrivateFactory != NULL) {
        mPrivateFactory->OnCreateView(parent, name, viewContext, attrs, view);
    }

    if (*view == NULL) {
        AutoPtr<IInterface> lastContext = (*mConstructorArgs)[0];
        mConstructorArgs->Set(0, viewContext);
        if (-1 == name.IndexOf('.')) {
            LAYOUT_INFLATOR_CATCH_EXCEPTION3_WITH_FINALLY(OnCreateView(parent, name, attrs, view), mConstructorArgs->Set(0, lastContext))
        } else {
            LAYOUT_INFLATOR_CATCH_EXCEPTION3_WITH_FINALLY(CreateView(name, String(NULL), attrs, view), mConstructorArgs->Set(0, lastContext))
        }
    }

    if (DEBUG) {
        Slogger::D(TAG, "Created view is: %p", *view);
    }

    return NOERROR;

//    } catch (InflateException e) {
//        throw e;
//
//    } catch (ClassNotFoundException e) {
//        InflateException ie = new InflateException(attrs.getPositionDescription()
//                + ": Error inflating class " + name);
//        ie.initCause(e);
//        throw ie;
//
//    } catch (Exception e) {
//        InflateException ie = new InflateException(attrs.getPositionDescription()
//                + ": Error inflating class " + name);
//        ie.initCause(e);
//        throw ie;
//    }
}

ECode LayoutInflater::RInflate(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IView* parent,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Boolean finishInflate,
    /* [in] */ Boolean inheritContext)
{
    Int32 orgDepth, depth;
    Int32 type;
    String name;

    FAIL_RETURN(parser->GetDepth(&orgDepth));
    FAIL_RETURN_WITH_CLOSE(parser->Next(&type), parser)
    FAIL_RETURN(parser->GetDepth(&depth));
    while ((type != IXmlPullParser::END_TAG ||
            depth > orgDepth) && type != IXmlPullParser::END_DOCUMENT) {

        if (type != IXmlPullParser::START_TAG) {
            FAIL_RETURN_WITH_CLOSE(parser->Next(&type), parser)
            FAIL_RETURN(parser->GetDepth(&depth));
            continue;
        }

        FAIL_RETURN(parser->GetName(&name));
        if (DEBUG)
        {
           // System.out.println("LayoutInflater : rInflate : name  === " + name );
        }

        if (name.Equals(TAG_REQUEST_FOCUS)) {
            FAIL_RETURN(ParseRequestFocus(parser, parent));
        } else if (TAG_TAG.Equals(name)) {
            FAIL_RETURN(ParseViewTag(parser, parent, attrs));
        }
        else if (String(TAG_INCLUDE).Equals(name)) {
            Int32 d;
            FAIL_RETURN(parser->GetDepth(&d));
            if (d == 0) {
//                throw new InflateException("<include /> cannot be the root element");
                return E_INFLATE_EXCEPTION;
            }
            FAIL_RETURN(ParseInclude(parser, parent, attrs, inheritContext));
        }
        else if (name.Equals(TAG_MERGE)) {
//            throw new InflateException("<merge /> must be the root element");
            return E_INFLATE_EXCEPTION;
        }
        else {
            AutoPtr<IView> view;
            FAIL_RETURN(CreateViewFromTag(parent, name, attrs, inheritContext, (IView**)&view));
            AutoPtr<IViewGroup> viewGroup = IViewGroup::Probe(parent);
            AutoPtr<IViewGroupLayoutParams> params;
            FAIL_RETURN(viewGroup->GenerateLayoutParams(attrs, (IViewGroupLayoutParams**)&params));
            FAIL_RETURN(RInflate(parser, view, attrs, TRUE, TRUE));
            viewGroup->AddView(view, params);
        }

        FAIL_RETURN_WITH_CLOSE(parser->Next(&type), parser)
        FAIL_RETURN(parser->GetDepth(&depth));
    }

    if (finishInflate) {
        ((View*)parent)->OnFinishInflate();
    }

    return NOERROR;
}

ECode LayoutInflater::ParseRequestFocus(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IView* view)
{
    Int32 type, depth;
    Boolean resultTmp;
    view->RequestFocus(&resultTmp);
    Int32 currentDepth;
    parser->GetDepth(&currentDepth);
    FAIL_RETURN_WITH_CLOSE(parser->Next(&type), parser)
    while ((type != IXmlPullParser::END_TAG ||
            (parser->GetDepth(&depth), depth) > currentDepth) && type != IXmlPullParser::END_DOCUMENT) {
        // Empty
        FAIL_RETURN_WITH_CLOSE(parser->Next(&type), parser)
    }
    return NOERROR;
}

/**
 * Parses a <code>&lt;tag&gt;</code> element and sets a keyed tag on the
 * containing View.
 */
ECode LayoutInflater::ParseViewTag(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IView* view,
    /* [in] */ IAttributeSet* attrs)
{
    Int32 type, depth;

    AutoPtr<ITypedArray> ta;
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32 *>(R::styleable::ViewTag),
        ArraySize(R::styleable::ViewTag));
    mContext->ObtainStyledAttributes(
            attrs, attrIds, (ITypedArray**)&ta);
    Int32 key;
    ta->GetResourceId(R::styleable::ViewTag_id, 0, &key);
    AutoPtr<ICharSequence> value;
    ta->GetText(R::styleable::ViewTag_value, (ICharSequence**)&value);
    view->SetTag(key, value);
    ta->Recycle();

    Int32 currentDepth;
    parser->GetDepth(&currentDepth);
    FAIL_RETURN_WITH_CLOSE(parser->Next(&type), parser)
    while ((type != IXmlPullParser::END_TAG ||
            (parser->GetDepth(&depth), depth) > currentDepth) && type != IXmlPullParser::END_DOCUMENT) {
        // Empty
        FAIL_RETURN_WITH_CLOSE(parser->Next(&type), parser)
    }
    return NOERROR;
}

ECode LayoutInflater::ParseInclude(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IView* parent,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Boolean inheritContext)
{
    Int32 type;

    AutoPtr<IViewGroup> group = IViewGroup::Probe(parent);
    if (group != NULL) {
        Int32 layout;
        attrs->GetAttributeResourceValue(String(NULL), String("layout"), 0, &layout);
        if (layout == 0) {
            String value;
            attrs->GetAttributeValue(String(NULL), String("layout"), &value);
            if (value.IsNull()) {
//                throw new InflateException("You must specifiy a layout in the"
//                        + " include tag: <include layout=\"@layout/layoutID\" />");
                return E_INFLATE_EXCEPTION;
            } else {
//                throw new InflateException("You must specifiy a valid layout "
//                        + "reference. The layout ID " + value + " is not valid.");
                return E_INFLATE_EXCEPTION;
            }
        } else {
            AutoPtr<IResources> res;
            ASSERT_SUCCEEDED(mContext->GetResources((IResources**)&res));

            AutoPtr<IXmlResourceParser> childParser;
            ASSERT_SUCCEEDED(res->GetLayout(layout, (IXmlResourceParser**)&childParser));

//            try {
            AutoPtr<IAttributeSet> childAttrs = Xml::AsAttributeSet(IXmlPullParser::Probe(childParser));

            FAIL_RETURN_WITH_CLOSE(IXmlPullParser::Probe(childParser)->Next(&type), childParser)
            while (type != IXmlPullParser::START_TAG && type != IXmlPullParser::END_DOCUMENT) {
                // Empty.
                FAIL_RETURN_WITH_CLOSE(IXmlPullParser::Probe(childParser)->Next(&type), childParser)
            }

            if (type != IXmlPullParser::START_TAG) {
//                throw new InflateException(childParser.getPositionDescription() +
//                        ": No start tag found!");
                childParser->Close();
                return E_INFLATE_EXCEPTION;
            }

            String childName;
            FAIL_RETURN_WITH_CLOSE(IXmlPullParser::Probe(childParser)->GetName(&childName), childParser)

            if (childName.Equals(TAG_MERGE)) {
                // Inflate all children.
                FAIL_RETURN_WITH_CLOSE(RInflate(IXmlPullParser::Probe(childParser), parent, childAttrs, FALSE, inheritContext), childParser)
            } else {
                AutoPtr<IView> view;
                FAIL_RETURN_WITH_CLOSE(CreateViewFromTag(parent, childName, childAttrs, inheritContext, (IView**)&view), childParser)

                // We try to load the layout params set in the <include /> tag. If
                // they don't exist, we will rely on the layout params set in the
                // included XML file.
                // During a layoutparams generation, a runtime exception is thrown
                // if either layout_width or layout_height is missing. We catch
                // this exception and set localParams accordingly: true means we
                // successfully loaded layout params from the <include /> tag,
                // false means we need to rely on the included layout params.
                AutoPtr<IViewGroupLayoutParams> params;
//                try {
                ECode ec = group->GenerateLayoutParams(attrs, (IViewGroupLayoutParams**)&params);
                if (ec == (Int32)E_RUNTIME_EXCEPTION) {
                    params = NULL;
                    FAIL_RETURN_WITH_CLOSE(group->GenerateLayoutParams(childAttrs, (IViewGroupLayoutParams**)&params), childParser)
                }
//                } catch (RuntimeException e) {
//                    params = group.generateLayoutParams(childAttrs);
//                } finally {
                if (params != NULL) {
                    PFL_EX("LayoutInflater::ParseInclude %p", params.Get());
                    view->SetLayoutParams(params.Get());
                }
//                }

                // Inflate all children.
                FAIL_RETURN_WITH_CLOSE(RInflate(IXmlPullParser::Probe(childParser), view, childAttrs, TRUE, TRUE), childParser)

                // Attempt to override the included layout's android:id with the
                // one set on the <include /> tag itself.
                AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
                    const_cast<Int32 *>(R::styleable::View),
                    ArraySize(R::styleable::View));
                AutoPtr<ITypedArray> a;
                mContext->ObtainStyledAttributes(attrs, attrIds, 0, 0, (ITypedArray**)&a);
                Int32 id;
                a->GetResourceId(R::styleable::View_id, IView::NO_ID, &id);
                // While we're at it, let's try to override android:visibility.
                Int32 visibility;
                a->GetInt32(R::styleable::View_visibility, -1, &visibility);
                a->Recycle();

                if (id != IView::NO_ID) {
                    view->SetId(id);
                }

                switch (visibility) {
                    case 0:
                        view->SetVisibility(IView::VISIBLE);
                        break;
                    case 1:
                        view->SetVisibility(IView::INVISIBLE);
                        break;
                    case 2:
                        view->SetVisibility(IView::GONE);
                        break;
                }

                group->AddView(view);
            }

            childParser->Close();
        }
    } else {
//        throw new InflateException("<include /> can only be used inside of a ViewGroup");
        return E_INFLATE_EXCEPTION;
    }

    Int32 currentDepth, depth;
    FAIL_RETURN(parser->GetDepth(&currentDepth));
    FAIL_RETURN_WITH_CLOSE(parser->Next(&type), parser)
    FAIL_RETURN(parser->GetDepth(&depth));
    while ((type != IXmlPullParser::END_TAG ||
            depth > currentDepth) &&
            type != IXmlPullParser::END_DOCUMENT) {
        // Empty
        FAIL_RETURN_WITH_CLOSE(parser->Next(&type), parser)
        FAIL_RETURN(parser->GetDepth(&depth));
    }
    return NOERROR;
}

ECode LayoutInflater::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    return NOERROR;
}

ECode LayoutInflater::constructor(
    /* [in] */ LayoutInflater* original,
    /* [in] */ IContext* newContext)
{
    mContext = newContext;
    mFactory = original->mFactory;
    mFactory2 = original->mFactory2;
    mPrivateFactory = original->mPrivateFactory;
    mFilter = original->mFilter;
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
