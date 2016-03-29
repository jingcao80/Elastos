
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include <Elastos.CoreLibrary.External.h>
#include "elastos/droid/preference/GenericInflater.h"
#include "elastos/droid/utility/Xml.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::IClassLoader;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Utility::Xml;
using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Preference {

const Boolean GenericInflater::DEBUG = FALSE;
const String GenericInflater::sConstructorSignature("CtxAttrs");
HashMap<String, AutoPtr<IConstructorInfo> > GenericInflater::sConstructorMap;

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


//====================================================
// GenericInflater::FactoryMerger
//====================================================

CAR_INTERFACE_IMPL(GenericInflater::FactoryMerger, Object, IGenericInflaterFactory);

GenericInflater::FactoryMerger::FactoryMerger(
    /* [in] */ IGenericInflaterFactory* f1,
    /* [in] */ IGenericInflaterFactory* f2)
    : mF1(f1)
    , mF2(f2)
{}

ECode GenericInflater::FactoryMerger::OnCreateItem(
    /* [in] */ const String& name,
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item)
    AutoPtr<IInterface> temp;
    mF1->OnCreateItem(name, context, attrs, (IInterface**)&temp);
    if (temp != NULL) {
        *item = temp;
        REFCOUNT_ADD(*item)
        return NOERROR;
    }
    return mF2->OnCreateItem(name, context, attrs, item);
}


//====================================================
// GenericInflater
//====================================================

CAR_INTERFACE_IMPL(GenericInflater, Object, IGenericInflater)

GenericInflater::GenericInflater(
    /* [in] */ IContext* context)
    : mContext(context)
    , mFactorySet(FALSE)
{
    mConstructorArgs = ArrayOf<IInterface*>::Alloc(2);
}

GenericInflater::GenericInflater(
    /* [in] */ IGenericInflater* original,
    /* [in] */ IContext* newContext)
    : mContext(newContext)
    , mFactorySet(FALSE)
{
    mConstructorArgs = ArrayOf<IInterface*>::Alloc(2);
    AutoPtr<IGenericInflaterFactory> factory;
    original->GetFactory((IGenericInflaterFactory**)&factory);
    original->SetFactory(factory);
}

ECode GenericInflater::SetDefaultPackage(
    /* [in] */ const String& defaultPackage)
{
    mDefaultPackage = defaultPackage;
    return NOERROR;
}

ECode GenericInflater::GetDefaultPackage(
    /* [out] */ String* defaultPackage)
{
    VALIDATE_NOT_NULL(defaultPackage)
    *defaultPackage = mDefaultPackage;
    return NOERROR;
}

ECode GenericInflater::GetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context)
    *context = mContext;
    REFCOUNT_ADD(*context)
    return NOERROR;
}

ECode GenericInflater::GetFactory(
    /* [out] */ IGenericInflaterFactory** factory)
{
    VALIDATE_NOT_NULL(factory)
    *factory = mFactory;
    REFCOUNT_ADD(*factory)
    return NOERROR;
}

ECode GenericInflater::SetFactory(
    /* [in] */ IGenericInflaterFactory* factory)
{
    if (mFactorySet) {
        Slogger::D("GenericInflater", "A factory has already been set on this inflater");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (factory == NULL) {
        Slogger::D("GenericInflater", "Given factory can not be null");
        return E_NULL_POINTER_EXCEPTION;
    }
    mFactorySet = TRUE;
    if (mFactory == NULL) {
        mFactory = factory;
    }
    else {
        mFactory = new FactoryMerger(factory, mFactory);
    }
    return NOERROR;
}

ECode GenericInflater::Inflate(
    /* [in] */ Int32 resource,
    /* [in] */ IInterface* root,
    /* [out] */ IInterface** t)
{
    return Inflate(resource, root, root != NULL, t);
}

ECode GenericInflater::Inflate(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IInterface* root,
    /* [out] */ IInterface** t)
{
    return Inflate(parser, root, root != NULL, t);
}

ECode GenericInflater::Inflate(
    /* [in] */ Int32 resource,
    /* [in] */ IInterface* root,
    /* [in] */ Boolean attachToRoot,
    /* [out] */ IInterface** t)
{
    VALIDATE_NOT_NULL(t)
    if (DEBUG) Slogger::W("GenericInflater", "INFLATING from resource: %d", resource);
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IResources> res;
    ctx->GetResources((IResources**)&res);
    AutoPtr<IXmlResourceParser> parser;
    res->GetXml(resource, (IXmlResourceParser**)&parser);
    AutoPtr<IXmlPullParser> pp = IXmlPullParser::Probe(parser);
    ECode ec = Inflate(pp, root, attachToRoot, t);
    parser->Close();
    return ec;
}

ECode GenericInflater::Inflate(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IInterface* root,
    /* [in] */ Boolean attachToRoot,
    /* [out] */ IInterface** t)
{
    VALIDATE_NOT_NULL(t)
    *t = NULL;

    AutoLock lock(mConstructorArgsLock);
    AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(parser);
    mConstructorArgs->Set(0, mContext);
    AutoPtr<IInterface> result = root;
    // Look for the root node.
    Int32 type;
    while (parser->Next(&type), (type != IXmlPullParser::START_TAG
            && type != IXmlPullParser::END_DOCUMENT)) {
        ;
    }

    if (type != IXmlPullParser::START_TAG) {
        String str;
        parser->GetPositionDescription(&str);
        Slogger::D("GenericInflater", str + ": No start tag found!");
        return E_INFLATE_EXCEPTION;
    }

    // if (DEBUG) {
    //     System.out.println("**************************");
    //     System.out.println("Creating root: "
    //             + parser.getName());
    //     System.out.println("**************************");
    // }
    // Temp is the root that was found in the xml
    AutoPtr<IInterface> xmlRoot;
    String name;
    ECode ec = CreateItemFromTag(parser, (parser->GetName(&name), name), attrs, (IInterface**)&xmlRoot);
    FAIL_GOTO(ec, fail);

    result = NULL;
    ec = OnMergeRoots(root, attachToRoot, xmlRoot, (IInterface**)&result);
    FAIL_GOTO(ec, fail);

    // if (DEBUG) {
    //     System.out.println("-----> start inflating children");
    // }
    // Inflate all children under temp
    FAIL_RETURN(RInflate(parser, result, attrs))
    // if (DEBUG) {
    //     System.out.println("-----> done inflating children");
    // }

fail:
    if (ec == (ECode)E_XML_PULL_PARSER_EXCEPTION) {
        return E_INFLATE_EXCEPTION;
    }
    else if (ec == (ECode)E_IO_EXCEPTION) {
        return E_INFLATE_EXCEPTION;
    }
    else if (FAILED(ec)){
        return ec;
    }

    *t = result;
    REFCOUNT_ADD(*t)
    return NOERROR;
}

ECode GenericInflater::CreateItem(
    /* [in] */ const String& name,
    /* [in] */ const String& prefix,
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IInterface** t)
{
    VALIDATE_NOT_NULL(t)
    *t = NULL;

    ECode ec = NOERROR;
    AutoPtr<IConstructorInfo> constructor;
    String prefixName(name);
    AutoPtr<IArgumentList> args;
    HashMap<String, AutoPtr<IConstructorInfo> >::Iterator it = sConstructorMap.Find(name);
    if (it == sConstructorMap.End()) {
        AutoPtr<IClassLoader> cl;
        AutoPtr<IClassInfo> clazz;
        // Class not found in the cache, see if it's real,
        // and try to add it
        ec = mContext->GetClassLoader((IClassLoader**)&cl);
        FAIL_GOTO(ec, fail)
        if (!prefix.IsNull()) {
            prefixName = prefix + name;
        }
        // ec = cl->LoadClass(prefixName, (IClassInfo**)&clazz);
        FAIL_GOTO(ec, fail)
        ec = clazz->GetConstructorInfoByParamNames(sConstructorSignature, (IConstructorInfo**)&constructor);
        FAIL_GOTO(ec, fail)
        sConstructorMap[name] = constructor;
    }
    else {
        constructor = it->mSecond;
    }

    mConstructorArgs->Set(1, attrs);
    ec = constructor->CreateArgumentList((IArgumentList**)&args);
    FAIL_GOTO(ec, fail)
    ec = args->SetInputArgumentOfObjectPtr(0, (*mConstructorArgs)[0]);
    FAIL_GOTO(ec, fail)
    ec = args->SetInputArgumentOfObjectPtr(1, (*mConstructorArgs)[1]);
    FAIL_GOTO(ec, fail)
    ec = constructor->CreateObject(args, t);
fail:
    if (ec == (ECode)E_NO_SUCH_METHOD_EXCEPTION) {
        // InflateException ie = new InflateException(attrs
        //         .getPositionDescription()
        //         + ": Error inflating class "
        //         + (prefix != null ? (prefix + name) : name));
        // ie.initCause(e);
        // throw ie;
        String desp;
        attrs->GetPositionDescription(&desp);
        if (!prefix.IsNull()) {
            prefixName = prefix + name;
        }
        Slogger::E("GenericInflater", "%s: Error inflating class %s", desp.string(), prefixName.string());
        return E_INFLATE_EXCEPTION;
    }
    else if (ec == (ECode)E_CLASS_NOT_FOUND_EXCEPTION) {
        return ec;
    }
    else if (FAILED(ec)){
        // InflateException ie = new InflateException(attrs
        //         .getPositionDescription()
        //         + ": Error inflating class "
        //         + constructor.getClass().getName());
        // ie.initCause(e);
        // throw ie;
        String desp;
        attrs->GetPositionDescription(&desp);
        // StringBuf* pStringBuf = StringBuf::Alloc(256);
        // if (constructor != NULL) {
        //     constructor->GetName(pStringBuf);
        // }
        // Slogger::E("GenericInflater", "%s: Error inflating class %s", desp.string(), pStringBuf->GetPayload());
        // StringBuf::Free(pStringBuf);
        return E_INFLATE_EXCEPTION;
    }

    return NOERROR;
}

ECode GenericInflater::GetFactorySet(
    /* [out] */ Boolean* fs)
{
    VALIDATE_NOT_NULL(fs)
    *fs = mFactorySet;
    return NOERROR;
}

ECode GenericInflater::SetFactorySet(
    /* [in] */ Boolean fs)
{
    mFactorySet = fs;
    return NOERROR;
}

ECode GenericInflater::GetConstructorArgs(
    /* [out, callee] */ ArrayOf<IInterface*>** ca)
{
    VALIDATE_NOT_NULL(ca)
    *ca = mConstructorArgs;
    REFCOUNT_ADD(*ca)
    return NOERROR;
}

ECode GenericInflater::SetConstructorArgs(
    /* [in] */ ArrayOf<IInterface*>* ca)
{
    mConstructorArgs = ca;
    return NOERROR;
}

ECode GenericInflater::OnCreateItem(
    /* [in] */ const String& name,
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IInterface** t)
{
    VALIDATE_NOT_NULL(t)
    return CreateItem(name, mDefaultPackage, attrs, t);
}

ECode GenericInflater::CreateItemFromTag(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& name,
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IInterface** t)
{
    // if (DEBUG) System.out.println("******** Creating item: " + name);
    VALIDATE_NOT_NULL(t)
    *t = NULL;

    AutoPtr<IInterface> item;
    ECode ec = NOERROR;
    if (mFactory != NULL) {
        ec = mFactory->OnCreateItem(name, mContext, attrs, (IInterface**)&item);
        FAIL_GOTO(ec, fail)
    }

    if (item == NULL) {
        if (-1 == name.IndexOf('.')) {
            ec = OnCreateItem(name, attrs, (IInterface**)&item);
            FAIL_GOTO(ec, fail)
        }
        else {
            ec = CreateItem(name, String(NULL), attrs, (IInterface**)&item);
            FAIL_GOTO(ec, fail)
        }
    }

fail:
    if (ec == (ECode)E_INFLATE_EXCEPTION) {
        return ec;
    }
    else if (FAILED(ec)){
        String desp;
        attrs->GetPositionDescription(&desp);
        Slogger::E("GenericInflater", "%s: Error inflating class %s", desp.string(), name.string());
        return E_INFLATE_EXCEPTION;
    }
    // if (DEBUG) System.out.println("Created item is: " + item);
    *t = item;
    REFCOUNT_ADD(*t)
    return NOERROR;
}

ECode GenericInflater::RInflate(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IInterface* parent,
    /* [in] */ IAttributeSet* attrs)
{
    Int32 depth;
    parser->GetDepth(&depth);
    Int32 type, d2;
    while (((parser->Next(&type), type != IXmlPullParser::END_TAG) ||
            (parser->GetDepth(&d2), d2 > depth)) && type != IXmlPullParser::END_DOCUMENT) {
        if (type != IXmlPullParser::START_TAG) {
            continue;
        }

        Boolean result;
        if (OnCreateCustomFromTag(parser, parent, attrs, &result), result) {
            continue;
        }

        // if (DEBUG) {
        //     System.out.println("Now inflating tag: " + parser.getName());
        // }
        String name;
        parser->GetName(&name);

        AutoPtr<IInterface> item;
        FAIL_RETURN(CreateItemFromTag(parser, name, attrs, (IInterface**)&item))

        // if (DEBUG) {
        //     System.out
        //             .println("Creating params from parent: " + parent);
        // }

        assert(IGenericInflaterParent::Probe(parent) != NULL);
        FAIL_RETURN(IGenericInflaterParent::Probe(parent)->AddItemFromInflater(item))

        // if (DEBUG) {
        //     System.out.println("-----> start inflating children");
        // }
        FAIL_RETURN(RInflate(parser, item, attrs))
        // if (DEBUG) {
        //     System.out.println("-----> done inflating children");
        // }
    }

    return NOERROR;
}

ECode GenericInflater::OnCreateCustomFromTag(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IInterface* parent,
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

ECode GenericInflater::OnMergeRoots(
    /* [in] */ IInterface* givenRoot,
    /* [in] */ Boolean attachToGivenRoot,
    /* [in] */ IInterface* xmlRoot,
    /* [out] */ IInterface** p)
{
    VALIDATE_NOT_NULL(p)
    *p = xmlRoot;
    REFCOUNT_ADD(*p)
    return NOERROR;
}


} // Preference
} // Droid
} // Elastos
