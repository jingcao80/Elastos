
#ifndef __ELASTOS_DROID_VIEW_LAYOUTINFLATER_H__
#define __ELASTOS_DROID_VIEW_LAYOUTINFLATER_H__

#include <elastos/droid/ext/frameworkext.h>
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Graphics.h"
#include <elastos/droid/widget/FrameLayout.h>
#include <elastos/core/Object.h>

using Elastos::Utility::IHashMap;
using Org::Xmlpull::V1::IXmlPullParser;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerCallback;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace View {

class ECO_PUBLIC LayoutInflater
    : public Object
    , public ILayoutInflater
{
public:
    class BlinkLayout
        : public FrameLayout
        , public IBlinkLayout
    {
    private:
        class HandlerCallback
            : public Object
            , public IHandlerCallback
        {
        public:
            CAR_INTERFACE_DECL()

            HandlerCallback(
                /* [in] */ BlinkLayout* host);

            CARAPI HandleMessage(
                /* [in] */ IMessage* msg,
                /* [in] */ Boolean* result);

        private:
            BlinkLayout* mHost;
        };

    public:
        CAR_INTERFACE_DECL()

        BlinkLayout();

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IAttributeSet* attrs);

    protected:
        CARAPI OnAttachedToWindow();

        CARAPI OnDetachedFromWindow();

        CARAPI DispatchDraw(
                /* [in] */ ICanvas* canvas);

    private:
        CARAPI_(void) MakeBlink();

    private:
        ECO_LOCAL static const Int32 MESSAGE_BLINK = 0x42;
        ECO_LOCAL static const Int32 BLINK_DELAY = 500;

        Boolean mBlink;
        Boolean mBlinkState;
        AutoPtr<IHandler> mHandler;
    };

private:
    class ECO_LOCAL FactoryMerger
        : public Object
        , public ILayoutInflaterFactory
        , public ILayoutInflaterFactory2
    {
    public:
        CAR_INTERFACE_DECL()

        FactoryMerger(
            /* [in] */ ILayoutInflaterFactory* f1,
            /* [in] */ ILayoutInflaterFactory2* f12,
            /* [in] */ ILayoutInflaterFactory* f2,
            /* [in] */ ILayoutInflaterFactory2* f22);

        CARAPI OnCreateView(
            /* [in] */ const String& name,
            /* [in] */ IContext* ctx,
            /* [in] */ IAttributeSet* attrs,
            /* [out] */ IView** view);

        CARAPI OnCreateView(
            /* [in] */ IView* parent,
            /* [in] */ const String& name,
            /* [in] */ IContext* ctx,
            /* [in] */ IAttributeSet* attrs,
            /* [out] */ IView** view);
    private:
        AutoPtr<ILayoutInflaterFactory> mF1, mF2;
        AutoPtr<ILayoutInflaterFactory2> mF12, mF22;
    };

public:
    LayoutInflater();

    ~LayoutInflater();

    CAR_INTERFACE_DECL()

    /**
     * Obtains the LayoutInflater from the given context.
     */
    static CARAPI From(
        /* [in] */ IContext* context,
        /* [out] */ ILayoutInflater** inflater);

    /**
     * Return the context we are running in, for access to resources, class
     * loader, etc.
     */
    CARAPI GetContext(
        /* [out] */ IContext** inflater);

    CARAPI GetFactory(
        /* [out] */ ILayoutInflaterFactory** factory);

    CARAPI GetFactory2(
        /* [out] */ ILayoutInflaterFactory2** factory);

    CARAPI SetFactory(
        /* [in] */ ILayoutInflaterFactory* factory);

    CARAPI SetFactory2(
        /* [in] */ ILayoutInflaterFactory2* factory);

    CARAPI SetPrivateFactory(
        /* [in] */ ILayoutInflaterFactory2* factory);

    CARAPI GetFilter(
        /* [out] */ ILayoutInflaterFilter** filter);

    CARAPI SetFilter(
        /* [in] */ ILayoutInflaterFilter* filter);

    CARAPI Inflate(
        /* [in] */ Int32 resource,
        /* [in] */ IViewGroup* root,
        /* [out] */ IView** view);

    CARAPI Inflate(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IViewGroup* root,
        /* [out] */ IView** view);

    CARAPI Inflate(
        /* [in] */ Int32 resource,
        /* [in] */ IViewGroup* root,
        /* [in] */ Boolean attachToRoot,
        /* [out] */ IView** view);

    CARAPI Inflate(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IViewGroup* root,
        /* [in] */ Boolean attachToRoot,
        /* [out] */ IView** view);

    /**
     * Low-level function for instantiating a view by name. This attempts to
     * instantiate a view class of the given <var>name</var> found in this
     * LayoutInflater's ClassLoader.
     *
     * <p>
     * There are two things that can happen in an error case: either the
     * exception describing the error will be thrown, or a null will be
     * returned. You must deal with both possibilities -- the former will happen
     * the first time createView() is called for a class of a particular name,
     * the latter every time there-after for that class name.
     *
     * @param name The full name of the class to be instantiated.
     * @param attrs The XML attributes supplied for this instance.
     *
     * @return View The newly instantied view, or null.
     */
    CARAPI CreateView(
        /* [in] */ const String& name,
        /* [in] */ const String& prefix,
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IView** view);

protected:
    /**
     * This routine is responsible for creating the correct subclass of View
     * given the xml element name. Override it to handle custom view objects. If
     * you override this in your subclass be sure to call through to
     * super.onCreateView(name) for names you do not recognize.
     *
     * @param name The fully qualified class name of the View to be create.
     * @param attrs An AttributeSet of attributes to apply to the View.
     *
     * @return View The View created.
     */
    virtual CARAPI OnCreateView(
        /* [in] */ const String& name,
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IView** view);

    /**
     * Version of {@link #onCreateView(String, AttributeSet)} that also
     * takes the future parent of the view being constructure.  The default
     * implementation simply calls {@link #onCreateView(String, AttributeSet)}.
     *
     * @param parent The future parent of the returned view.  <em>Note that
     * this may be null.</em>
     * @param name The fully qualified class name of the View to be create.
     * @param attrs An AttributeSet of attributes to apply to the View.
     *
     * @return View The View created.
     */
    virtual CARAPI OnCreateView(
        /* [in] */ IView* parent,
        /* [in] */ const String& name,
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IView** view);

    /*
     * default visibility so the BridgeInflater can override it.
     */
    virtual CARAPI CreateViewFromTag(
        /* [in] */ IView* parent,
        /* [in] */ const String& name,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Boolean inheritContext,
        /* [out] */ IView** view);

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Create a new LayoutInflater instance that is a copy of an existing
     * LayoutInflater, optionally with its Context changed.  For use in
     * implementing {@link #cloneInContext}.
     *
     * @param original The original LayoutInflater to copy.
     * @param newContext The new Context to use.
     */
    CARAPI constructor(
        /* [in] */ LayoutInflater* original,
        /* [in] */ IContext* newContext);

    /**
     * Recursive method used to descend down the xml hierarchy and instantiate
     * views, instantiate their children, and then call onFinishInflate().
     */
    CARAPI RInflate(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IView* parent,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Boolean finishInflate,
        /* [in] */ Boolean inheritContext);

private:
    /**
     * Throw an excpetion because the specified class is not allowed to be inflated.
     */
    CARAPI FailNotAllowed(
        /* [in] */ const String& name,
        /* [in] */ const String& prefix,
        /* [in] */ IAttributeSet* attrs);

    CARAPI ParseInclude(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IView* parent,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Boolean inheritContext);

    CARAPI ParseViewTag(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IView* view,
        /* [in] */ IAttributeSet* attrs);

    static CARAPI_(String) ConvertViewName(
        /* [in] */ const String& name);

    static CARAPI_(String) GetReflectionClassName(
        /* [in] */ const String& prefix,
        /* [in] */ const String& name);

    /**
     * Parses a <code>&lt;request-focus&gt;</code> element and requests focus on
     * the containing View.
     */
    CARAPI ParseRequestFocus(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IView* view);

public:
    AutoPtr<ArrayOf<IInterface*> > mConstructorArgs;

    static String mConstructorSignature;

private:
    Boolean DEBUG;

    IContext* mContext;

    // these are optional, set by the caller
    Boolean mFactorySet;
    AutoPtr<ILayoutInflaterFactory> mFactory;
    AutoPtr<ILayoutInflaterFactory2> mFactory2;
    ILayoutInflaterFactory2* mPrivateFactory;
    AutoPtr<ILayoutInflaterFilter> mFilter;

    Object mConstructorArgsLock;

    // static AutoPtr< HashMap<String, AutoPtr<IConstructorInfo> > > sConstructorMap;
    ECO_LOCAL static AutoPtr<IHashMap> sConstructorMap;

    AutoPtr<IHashMap> mFilterMap;

    ECO_LOCAL static const String TAG;

    ECO_LOCAL static const String TAG_TAG;
    ECO_LOCAL static const String TAG_MERGE;
    ECO_LOCAL static const String TAG_INCLUDE;
    ECO_LOCAL static const String TAG_1995;
    ECO_LOCAL static const String TAG_REQUEST_FOCUS;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_VIEW_LAYOUTINFLATER_H__
