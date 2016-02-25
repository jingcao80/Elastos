
#ifndef __ELASTOS_DROID_PREFERENCE_GENERICINFLATER_H__
#define __ELASTOS_DROID_PREFERENCE_GENERICINFLATER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Preference.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Org::Xmlpull::V1::IXmlPullParser;


namespace Elastos {
namespace Droid {
namespace Preference {

class GenericInflater
    : public Object
    , public IGenericInflater
{
private:
    class FactoryMerger
        : public Object
        , public IGenericInflaterFactory
    {
    public:
        CAR_INTERFACE_DECL()

        FactoryMerger(
            /* [in] */ IGenericInflaterFactory* f1,
            /* [in] */ IGenericInflaterFactory* f2);

        CARAPI OnCreateItem(
            /* [in] */ const String& name,
            /* [in] */ IContext* context,
            /* [in] */ IAttributeSet* attrs,
            /* [out] */ IInterface** item);

    private:
        AutoPtr<IGenericInflaterFactory> mF1;
        AutoPtr<IGenericInflaterFactory> mF2;
    };

public:
    CAR_INTERFACE_DECL()

    GenericInflater(){}

    /**
     * Create a copy of the existing inflater object, with the copy
     * pointing to a different Context than the original.  This is used by
     * {@link ContextThemeWrapper} to create a new inflater to go along
     * with the new Context theme.
     *
     * @param newContext The new Context to associate with the new inflater.
     * May be the same as the original Context if desired.
     *
     * @return Returns a brand spanking new inflater object associated with
     * the given Context.
     */
    virtual CARAPI CloneInContext(
        /* [in] */ IContext* newContext,
        /* [out] */ IGenericInflater** ret) = 0;

    /**
     * Sets the default package that will be searched for classes to construct
     * for tag names that have no explicit package.
     *
     * @param defaultPackage The default package. This will be prepended to the
     *            tag name, so it should end with a period.
     */
    virtual CARAPI SetDefaultPackage(
        /* [in] */ const String& defaultPackage);

    /**
     * Returns the default package, or null if it is not set.
     *
     * @see #setDefaultPackage(String)
     * @return The default package.
     */
    virtual CARAPI GetDefaultPackage(
        /* [out] */ String* defaultPackage);

    /**
     * Return the context we are running in, for access to resources, class
     * loader, etc.
     */
    virtual CARAPI GetContext(
        /* [out] */ IContext** context);

    /**
     * Return the current factory (or null). This is called on each element
     * name. If the factory returns an item, add that to the hierarchy. If it
     * returns null, proceed to call onCreateItem(name).
     */
    virtual CARAPI GetFactory(
        /* [out] */ IGenericInflaterFactory** factory);

    /**
     * Attach a custom Factory interface for creating items while using this
     * inflater. This must not be null, and can only be set
     * once; after setting, you can not change the factory. This is called on
     * each element name as the XML is parsed. If the factory returns an item,
     * that is added to the hierarchy. If it returns null, the next factory
     * default {@link #onCreateItem} method is called.
     * <p>
     * If you have an existing inflater and want to add your
     * own factory to it, use {@link #cloneInContext} to clone the existing
     * instance and then you can use this function (once) on the returned new
     * instance. This will merge your own factory with whatever factory the
     * original instance is using.
     */
    virtual CARAPI SetFactory(
        /* [in] */ IGenericInflaterFactory* factory);


    /**
     * Inflate a new item hierarchy from the specified xml resource. Throws
     * InflaterException if there is an error.
     *
     * @param resource ID for an XML resource to load (e.g.,
     *        <code>R.layout.main_page</code>)
     * @param root Optional parent of the generated hierarchy.
     * @return The root of the inflated hierarchy. If root was supplied,
     *         this is the root item; otherwise it is the root of the inflated
     *         XML file.
     */
    virtual CARAPI Inflate(
        /* [in] */ Int32 resource,
        /* [in] */ IInterface* root,
        /* [out] */ IInterface** result);

    /**
     * Inflate a new hierarchy from the specified xml node. Throws
     * InflaterException if there is an error. *
     * <p>
     * <em><strong>Important</strong></em>&nbsp;&nbsp;&nbsp;For performance
     * reasons, inflation relies heavily on pre-processing of XML files
     * that is done at build time. Therefore, it is not currently possible to
     * use inflater with an XmlPullParser over a plain XML file at runtime.
     *
     * @param parser XML dom node containing the description of the
     *        hierarchy.
     * @param root Optional parent of the generated hierarchy.
     * @return The root of the inflated hierarchy. If root was supplied,
     *         this is the that; otherwise it is the root of the inflated
     *         XML file.
     */
    virtual CARAPI Inflate(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IInterface* root,
        /* [out] */ IInterface** result);

    /**
     * Inflate a new hierarchy from the specified xml resource. Throws
     * InflaterException if there is an error.
     *
     * @param resource ID for an XML resource to load (e.g.,
     *        <code>R.layout.main_page</code>)
     * @param root Optional root to be the parent of the generated hierarchy (if
     *        <em>attachToRoot</em> is true), or else simply an object that
     *        provides a set of values for root of the returned
     *        hierarchy (if <em>attachToRoot</em> is false.)
     * @param attachToRoot Whether the inflated hierarchy should be attached to
     *        the root parameter?
     * @return The root of the inflated hierarchy. If root was supplied and
     *         attachToRoot is true, this is root; otherwise it is the root of
     *         the inflated XML file.
     */
    virtual CARAPI Inflate(
        /* [in] */ Int32 resource,
        /* [in] */ IInterface* root,
        /* [in] */ Boolean attachToRoot,
        /* [out] */ IInterface** result);

    /**
     * Inflate a new hierarchy from the specified XML node. Throws
     * InflaterException if there is an error.
     * <p>
     * <em><strong>Important</strong></em>&nbsp;&nbsp;&nbsp;For performance
     * reasons, inflation relies heavily on pre-processing of XML files
     * that is done at build time. Therefore, it is not currently possible to
     * use inflater with an XmlPullParser over a plain XML file at runtime.
     *
     * @param parser XML dom node containing the description of the
     *        hierarchy.
     * @param root Optional to be the parent of the generated hierarchy (if
     *        <em>attachToRoot</em> is true), or else simply an object that
     *        provides a set of values for root of the returned
     *        hierarchy (if <em>attachToRoot</em> is false.)
     * @param attachToRoot Whether the inflated hierarchy should be attached to
     *        the root parameter?
     * @return The root of the inflated hierarchy. If root was supplied and
     *         attachToRoot is true, this is root; otherwise it is the root of
     *         the inflated XML file.
     */
    virtual CARAPI Inflate(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IInterface* root,
        /* [in] */ Boolean attachToRoot,
        /* [out] */ IInterface** result);

    /**
     * Low-level function for instantiating by name. This attempts to
     * instantiate class of the given <var>name</var> found in this
     * inflater's ClassLoader.
     *
     * <p>
     * There are two things that can happen in an error case: either the
     * exception describing the error will be thrown, or a null will be
     * returned. You must deal with both possibilities -- the former will happen
     * the first time createItem() is called for a class of a particular name,
     * the latter every time there-after for that class name.
     *
     * @param name The full name of the class to be instantiated.
     * @param attrs The XML attributes supplied for this instance.
     *
     * @return The newly instantied item, or null.
     */
    virtual CARAPI CreateItem(
        /* [in] */ const String& name,
        /* [in] */ const String& prefix,
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IInterface** t);

    CARAPI GetFactorySet(
        /* [out] */ Boolean* fs);

    CARAPI SetFactorySet(
        /* [in] */ Boolean fs);

   CARAPI GetConstructorArgs(
        /* [out, callee] */ ArrayOf<IInterface*>** ca);

   CARAPI SetConstructorArgs(
        /* [in] */ ArrayOf<IInterface*>* ca);

protected:
    /**
     * Create a new inflater instance associated with a
     * particular Context.
     *
     * @param context The Context in which this inflater will
     *            create its items; most importantly, this supplies the theme
     *            from which the default values for their attributes are
     *            retrieved.
     */
    GenericInflater(
        /* [in] */ IContext* context);

    /**
     * Create a new inflater instance that is a copy of an
     * existing inflater, optionally with its Context
     * changed. For use in implementing {@link #cloneInContext}.
     *
     * @param original The original inflater to copy.
     * @param newContext The new Context to use.
     */
    GenericInflater(
        /* [in] */ IGenericInflater* original,
        /* [in] */ IContext* newContext);

    /**
     * This routine is responsible for creating the correct subclass of item
     * given the xml element name. Override it to handle custom item objects. If
     * you override this in your subclass be sure to call through to
     * super.onCreateItem(name) for names you do not recognize.
     *
     * @param name The fully qualified class name of the item to be create.
     * @param attrs An AttributeSet of attributes to apply to the item.
     * @return The item created.
     */
    virtual CARAPI OnCreateItem(
        /* [in] */ const String& name,
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IInterface** t);

    /**
     * Before this inflater tries to create an item from the tag, this method
     * will be called. The parser will be pointing to the start of a tag, you
     * must stop parsing and return when you reach the end of this element!
     *
     * @param parser XML dom node containing the description of the hierarchy.
     * @param parent The item that should be the parent of whatever you create.
     * @param attrs An AttributeSet of attributes to apply to the item.
     * @return Whether you created a custom object (true), or whether this
     *         inflater should proceed to create an item.
     */
    virtual CARAPI OnCreateCustomFromTag(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IInterface* parent,
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ Boolean* res);

    virtual CARAPI OnMergeRoots(
        /* [in] */ IInterface* givenRoot,
        /* [in] */ Boolean attachToGivenRoot,
        /* [in] */ IInterface* xmlRoot,
        /* [out] */ IInterface** p);

private:
    CARAPI CreateItemFromTag(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& name,
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IInterface** t);

    /**
     * Recursive method used to descend down the xml hierarchy and instantiate
     * items, instantiate their children, and then call onFinishInflate().
     */
    CARAPI RInflate(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IInterface* parent,
        /* [in] */ IAttributeSet* attrs);

protected:
    AutoPtr<IContext> mContext;

private:
    static const Boolean DEBUG;

    // these are optional, set by the caller
    Boolean mFactorySet;
    AutoPtr<IGenericInflaterFactory> mFactory;

    AutoPtr< ArrayOf<IInterface*> > mConstructorArgs;
    Object mConstructorArgsLock;

    static const String sConstructorSignature;

    // static final HashMap sConstructorMap = new HashMap();
    static HashMap<String, AutoPtr<IConstructorInfo> > sConstructorMap;

    String mDefaultPackage;
};

} // Preference
} // Droid
} // Elastos

#endif  // __ELASTOS_DROID_PREFERENCE_GENERICINFLATER_H__
