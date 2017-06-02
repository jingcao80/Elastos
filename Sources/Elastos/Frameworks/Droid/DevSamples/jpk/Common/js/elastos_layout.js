elog("====elastos_layout.js====begin====");

module.exports = function(aoElastos, aoActivity){
//--------common definition----begin----
    var CObject = aoElastos.CObject;

    var CString = aoElastos.Core.CString;
    var Droid_New = aoElastos.Droid.New;
    var Core_New = aoElastos.Core.New;

    var R = aoElastos.Application.R;

    var oActivity = aoActivity.ActivityInstance;
    var oHandler = aoActivity.ActivityHandler;

    var IView__VISIBLE = 0x00000000;
    var IView__INVISIBLE = 0x00000004;
    var IView__GONE = 0x00000008;
    var IView__NO_ID = -1;
    var IMotionEvent__ACTION_DOWN = 0;
    var IMotionEvent__ACTION_UP = 1;

//--------common definition----end----

//=====LayoutInflater::const====
var TAG = "LayoutInflater";
var TAG_TAG = "tag";
var TAG_MERGE = "merge";
var TAG_INCLUDE = "include";
var TAG_1995 = "blink";
var TAG_REQUEST_FOCUS = "requestFocus";

//=====IXmlPullParser::const====
var START_TAG = 2;
var END_TAG = 3;
var END_DOCUMENT = 1;

class STC {

// ECode Activity::SetContentView(
//     /* [in] */ Int32 layoutResID)
// {
//     FAIL_RETURN(GetWindow()->SetContentView(layoutResID));
//     return InitWindowDecorActionBar();
// }
SetContentView(ao_Activity, layoutResID)
{
    elog("====STC::SetContentView====begin====");
    var lo_view = this.Inflate(ao_Activity, layoutResID);
    elog("====STC::SetContentView====0====");
    ao_Activity.SetContentView(lo_view);
    elog("====STC::SetContentView====end====");
}

// ECode LayoutInflater::Inflate(
//     /* [in] */ Int32 resource,
//     /* [in] */ IViewGroup* root,
//     /* [in] */ Boolean attachToRoot,
//     /* [out] */ IView** view)
// {
//     VALIDATE_NOT_NULL(view)
//     *view = NULL;

//     if (DEBUG) {
//         Slogger::D(TAG, "Inflate from resource: %08x", resource);
//     }

//     AutoPtr<IContext> ctx;
//     GetContext((IContext**)&ctx);
//     AutoPtr<IResources> res;
//     ASSERT_SUCCEEDED(ctx->GetResources((IResources**)&res));
//     AutoPtr<IXmlResourceParser> parser;
//     ASSERT_SUCCEEDED(res->GetLayout(resource, (IXmlResourceParser**)&parser));
//     ECode ec = Inflate(IXmlPullParser::Probe(parser), root, attachToRoot, view);
//     parser->Close();
//     return ec;
// }
Inflate(ctx, resource)
{
//     /* [in] */ Int32 resource,
//     /* [in] */ IViewGroup* root,
//     /* [in] */ Boolean attachToRoot,
//     /* [out] */ IView** view)
// {
    // VALIDATE_NOT_NULL(view)
    // *view = NULL;
    var view;

    elog("Inflate from resource: " + resource);

    // AutoPtr<IContext> ctx;
    // GetContext((IContext**)&ctx);
    // AutoPtr<IResources> res;
    // ASSERT_SUCCEEDED(ctx->GetResources((IResources**)&res));

    var res = ctx.GetResources();


    // AutoPtr<IXmlResourceParser> parser;
    // ASSERT_SUCCEEDED(res->GetLayout(resource, (IXmlResourceParser**)&parser));

    var parser = res.GetLayout(resource);

    //ECode ec = Inflate(IXmlPullParser::Probe(parser), root, attachToRoot, view);

    var pullParser = parser.Probe("Org.Xmlpull.V1.IXmlPullParser");

    view = this._Inflate(pullParser, null, false);

    parser.Close();

    return view;
}   //Inflate

_Inflate(parser, root, attachToRoot) {
// ECode LayoutInflater::Inflate(
//     /* [in] */ IXmlPullParser* parser,
//     /* [in] */ IViewGroup* root,
//     /* [in] */ Boolean attachToRoot,
//     /* [out] */ IView** view)
// {
//     VALIDATE_NOT_NULL(view)
//     *view = NULL;

    var view;

//     AutoLock lock(mConstructorArgsLock);
//     AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(parser);


// AutoPtr<IAttributeSet> Xml::AsAttributeSet(
//     /* [in] */ IXmlPullParser* parser)
// {
//     AutoPtr<IAttributeSet> set = IAttributeSet::Probe(parser);
//     if (set == NULL) {
//         CXmlPullAttributes::New(parser, (IAttributeSet**)&set);
//     }

//     return set;
// }


    //var attrs = Xml::AsAttributeSet(parser);
    //var attrs = parser;
    var attrs = parser.Probe("Elastos.Droid.Utility.IAttributeSet");

//     AutoPtr<IContext> lastContext = IContext::Probe((*mConstructorArgs)[0]);
//     mConstructorArgs->Set(0, mContext);
//     AutoPtr<IView> result = root != NULL? IView::Probe(root) : NULL;
    var result;

//     // Look for the root node.
//     String name;
//     Int32 type;
//     FAIL_GOTO(parser->Next(&type), _EXIT_)
//     while ((type != IXmlPullParser::START_TAG) &&
//             type != IXmlPullParser::END_DOCUMENT) {
//         // Empty
//         FAIL_GOTO(parser->Next(&type), _EXIT_)
//     }
    var type = parser.Next();
    while(type != START_TAG && type != END_DOCUMENT) {
        type = parser.Next();
    }

//     if (type != IXmlPullParser::START_TAG) {
//         Slogger::E(TAG, "InflateException No start tag found!");
//         mConstructorArgs->Set(0, lastContext);
//         mConstructorArgs->Set(1, NULL);
//         return E_INFLATE_EXCEPTION;
//     }
    if (type != START_TAG) {
        elog("====STC::_Inflate====InflateException No start tag found!");
        Assert(0);
    }

//     parser->GetName(&name);
    var name = parser.GetName();

//     if (DEBUG) {
//         Slogger::D(TAG, "**************************");
//         Slogger::D(TAG, "Creating root view: %s", name.string());
//         Slogger::D(TAG, "**************************");
//     }
    elog("====STC::_Inflate====Creating root view: "+name);

    if (name == TAG_MERGE) {
//         if (root == NULL || !attachToRoot) {
//             Slogger::E(TAG, "<merge /> can be used only with a valid ViewGroup root and attachToRoot=true");
//             mConstructorArgs->Set(0, lastContext);
//             mConstructorArgs->Set(1, NULL);
//             return E_INFLATE_EXCEPTION;
//         }

//         LAYOUT_INFLATOR_CATCH_EXCEPTION2(RInflate(parser, IView::Probe(root), attrs.Get(), FALSE, FALSE))
    }
    else {
//         // Temp is the root view that was found in the xml
//         AutoPtr<IView> temp;
//         LAYOUT_INFLATOR_CATCH_EXCEPTION2(CreateViewFromTag(IView::Probe(root), name, attrs, FALSE, (IView**)&temp))
        var temp = this.CreateViewFromTag(root, name, attrs, false);

//         AutoPtr<IViewGroupLayoutParams> params;

//         if (root != NULL) {
//             if (DEBUG) {
//                 Slogger::D(TAG, "Creating params from root: %p", root);
//             }
//             // Create layout params that match root, if supplied
//             root->GenerateLayoutParams(attrs, (IViewGroupLayoutParams**)&params);
//             assert(params.Get());
//             if (!attachToRoot) {
//                 // Set the layout params for temp if we are not
//                 // attaching. (If we are, we use addView, below)
//                 temp->SetLayoutParams(params.Get());
//             }
//         }

//         if (DEBUG) {
//             Slogger::D(TAG, "-----> start inflating children");
//         }
//         // Inflate all children under temp
//         LAYOUT_INFLATOR_CATCH_EXCEPTION2(RInflate(parser, temp, attrs, TRUE, TRUE));
        this.RInflate(parser, temp, attrs, true, true)
//         if (DEBUG) {
//             Slogger::D(TAG, "-----> done inflating children");
//         }
        elog("====STC::_Inflate====-----> done inflating children");

//         // We are supposed to attach all the views we found (int temp)
//         // to root. Do that now.
//         if (root != NULL && attachToRoot) {
//             root->AddView(temp, params);
//         }

//         // Decide whether to return the root that was passed in or the
//         // top view found in xml.
//         if (root == NULL || !attachToRoot) {
//             result = temp;
//         }
        result = temp;
    }

// _EXIT_:
//     *view = result.Get();
//     REFCOUNT_ADD(*view);
        view = result;

//     mConstructorArgs->Set(0, lastContext);
//     mConstructorArgs->Set(1, NULL);
//     return NOERROR;
// }

    return view;

}   //_Inflate


// ECode LayoutInflater::CreateViewFromTag(
//     /* [in] */ IView* parent,
//     /* [in] */ const String& inName,
//     /* [in] */ IAttributeSet* attrs,
//     /* [in] */ Boolean inheritContext,
//     /* [out] */ IView** view)
// {
CreateViewFromTag(parent, inName, attrs, inheritContext) {
    elog("====STC::CreateViewFromTag====begin===="+inName);

    var view;

    // VALIDATE_NOT_NULL(view)
    // *view = NULL;

    // String name;
    // if (inName.Equals("view")) {
    //     attrs->GetAttributeValue(String(NULL), String("class"), &name);
    // }
    // else {
    //     name = inName;
    // }
    var name = inName;

    // AutoPtr<IContext> viewContext;
    var viewContext;
    // if (parent != NULL && inheritContext) {
    //     parent->GetContext((IContext**)&viewContext);
    // }
    // else {
    //     viewContext = mContext;
        viewContext = oActivity;
    // }

    // String packageName;
    // viewContext->GetPackageName(&packageName);
    var packageName = viewContext.GetPackageName();

    // name = LoadedPkg::GetElastosClassName(packageName, name);

    // if (DEBUG) {
    //     Slogger::D(TAG, "******** Creating view: %s, mFactory2: %p, mFactory: %p, mPrivateFactory: %p",
    //         name.string(),  mFactory2.Get(), mFactory.Get(), mPrivateFactory);
    // }

    // // Apply a theme wrapper, if requested.
    // AutoPtr<ITypedArray> ta;
    // AutoPtr<ArrayOf<Int32> > ATTRS_THEME = ArrayOf<Int32>::Alloc(1);
    // (*ATTRS_THEME)[0] = R::attr::theme;
    // viewContext->ObtainStyledAttributes(attrs, ATTRS_THEME, (ITypedArray**)&ta);
    // Int32 themeResId;
    // ta->GetResourceId(0, 0, &themeResId);
    // if (themeResId != 0) {
    //     // mContext of class View is naked pointer
    //     AutoPtr<ContextThemeWrapperHolder> themeContex = new ContextThemeWrapperHolder();

    //     String name = TO_STR(viewContext);
    //     if (name.Contains("BasePackageName=android OpPackageName=android")) {
    //         themeContex->mMemoryLeakTarget = TRUE;
    //         Slogger::I(TAG, " >>================================================");
    //         Slogger::I(TAG, " >> %p Create ContextThemeWrapperHolder %p: for %s",
    //             this, themeContex.Get(), name.string());
    //         Slogger::I(TAG, " >>================================================");
    //     }

    //     themeContex->constructor(viewContext, themeResId, FALSE/* do not hold */);
    //     viewContext = themeContex;
    // }
    // ta->Recycle();

    // if (name.Equals(TAG_1995)) {
    //     // Let's party like it's 1995!
    //     return CBlinkLayout::New(viewContext, attrs, view);
    // }

//    try {
    // if (mFactory2 != NULL) {
    //     mFactory2->OnCreateView(parent, name, viewContext, attrs, view);
    // }
    // else if (mFactory != NULL) {
    //     mFactory->OnCreateView(name, viewContext, attrs, view);
    // }
    // else {
    //     *view = NULL;
    // }

    // if (*view == NULL && mPrivateFactory != NULL) {
    //     mPrivateFactory->OnCreateView(parent, name, viewContext, attrs, view);
    // }

    // if (*view == NULL) {
    //     AutoPtr<IInterface> lastContext = (*mConstructorArgs)[0];
    //     mConstructorArgs->Set(0, viewContext);
    //     if (-1 == name.IndexOf('.')) {
    //         LAYOUT_INFLATOR_CATCH_EXCEPTION3_WITH_FINALLY(OnCreateView(parent, name, attrs, view), mConstructorArgs->Set(0, lastContext))
    //     } else {
    //         LAYOUT_INFLATOR_CATCH_EXCEPTION3_WITH_FINALLY(CreateView(name, String(NULL), attrs, view), mConstructorArgs->Set(0, lastContext))
    //     }
    // }

    //view = this.OnCreateView(name, viewContext, attrs);
    view = this.OnCreateView(name, attrs);

    // if (DEBUG) {
    //     Slogger::D(TAG, "Created view is: %p", *view);
    // }
    elog("====STC::CreateViewFromTag===="+"Created view is: %p");


    // return NOERROR;

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

    elog("====STC::CreateViewFromTag====end====");

    return view;
}   //CreateViewFromTag




// ECode PhoneLayoutInflater::OnCreateView(
//     /* [in] */ const String& name,
//     /* [in] */ IAttributeSet* attrs,
//     /* [out] */ IView** view)
// {
OnCreateView(name, attrs) {

    // VALIDATE_NOT_NULL(view);

    // for (Int32 i = 0; i < sClassPrefixList->GetLength(); ++i) {
    //     String prefix = (*sClassPrefixList)[i];
    //     ECode ec = CreateView(name, prefix, attrs, view);
    //     if (SUCCEEDED(ec) && (*view != NULL)) return ec;
    // }
    var prefix;
    var view = this.CreateView(name, prefix, attrs);
    return view;

    // return LayoutInflater::OnCreateView(name, attrs, view);
}


//String LayoutInflater::GetReflectionClassName(
//    /* [in] */ const String& prefix,
//    /* [in] */ const String& name)
//{
GetReflectionClassName(prefix, name) {
    // StringBuilder sb;
    var sb = "";
    // if (!prefix.IsNull()) {
    //     sb += prefix;
    // }
    if(prefix)sb += prefix;
    // String tmp = name;
    // Int32 index = name.LastIndexOf('$');
    // if (index != -1) {
    //     tmp = name.Substring(0, index);
    //     tmp += name.Substring(index + 1);
    // }
    var tmp = name.replace('$','');

    // index = tmp.LastIndexOf('.');
    var index = tmp.lastIndexOf('.');
    // if (index == -1) {
    //     sb += "C";
    // }
    if(index<0)sb+="C";
    // sb += tmp;
    sb+=tmp;
    // return sb.ToString();
    return sb;
}



// ECode LayoutInflater::CreateView(
//     /* [in] */ const String& name,
//     /* [in] */ const String& inPrefix,
//     /* [in] */ IAttributeSet* attrs,
//     /* [out] */ IView** view)
// {
CreateView(name, inPrefix, attrs) {
    elog("====STC::CreateView====begin===="+name);

    var view;

    // String prefix = inPrefix;
    // if (!inPrefix.IsNull()
    //     && (name.Equals("ViewStub") || name.Equals("View"))) {
    //     prefix = "Elastos.Droid.View."; // speed up for View or ViewStub
    // }
    // String reflectionClassName = GetReflectionClassName(prefix, name);
    var prefix = inPrefix;
    if(!prefix)prefix = "Elastos.Droid.View.";

    if(name=="LinearLayout")prefix = "Elastos.Droid.Widget.";
    // if(name=="LinearLayout") {
    //     name = "Elastos.DevSamples.Node.JSCalculator.CNodeLinearLayout";
    //     prefix = "";
    // }
    if(name=="RelativeLayout")prefix = "Elastos.Droid.Widget.";
    if(name=="Button")prefix = "Elastos.Droid.Widget.";

    if(name=="TextView")prefix = "Elastos.Droid.Widget.";
    if(name=="EditText")prefix = "Elastos.Droid.Widget.";
    if(name=="CheckBox")prefix = "Elastos.Droid.Widget.";
    if(name=="RadioGroup")prefix = "Elastos.Droid.Widget.";
    if(name=="RadioButton")prefix = "Elastos.Droid.Widget.";
    if(name=="ToggleButton")prefix = "Elastos.Droid.Widget.";
    if(name=="ListView")prefix = "Elastos.Droid.Widget.";

    if(name.indexOf("Elastos.")==0)prefix = "";

    var reflectionClassName = this.GetReflectionClassName(prefix, name);

    // // Slogger::I(TAG, " >> CreateView: name:%s, prefix:%s, fullname:%s",
    // //     name.string(), prefix.string(), reflectionClassName.string());
    elog("====STC::CreateView====reflectionClassName:"+reflectionClassName);

    // AutoPtr<IConstructorInfo> constructor;
    // AutoPtr<IClassInfo> clazz;

//    try {
    // AutoPtr<ICharSequence> csq;
    // CString::New(name, (ICharSequence**)&csq);
    // AutoPtr<IInterface> value;
    // sConstructorMap->Get(csq, (IInterface**)&value);

    // if (value == NULL) {
    //     // Class not found in the cache, see if it's real, and try to add it
    //     AutoPtr<IClassLoader> cl;
    //     LAYOUT_INFLATOR_CATCH_EXCEPTION1(mContext->GetClassLoader((IClassLoader**)&cl), "GetClassLoader")
    //     assert(cl != NULL);

    //     StringBuilder sb("LoadClass ");
    //     sb += reflectionClassName;
    //     LAYOUT_INFLATOR_CATCH_EXCEPTION1(cl->LoadClass(reflectionClassName, (IClassInfo**)&clazz), sb.ToString().string());

    //     if (mFilter != NULL && clazz != NULL) {
    //         Boolean allowed;
    //         mFilter->OnLoadClass(clazz, &allowed);
    //         if (!allowed) {
    //             LAYOUT_INFLATOR_CATCH_EXCEPTION1(FailNotAllowed(name, prefix, attrs), "FailNotAllowed");
    //         }
    //     }

    //     LAYOUT_INFLATOR_CATCH_EXCEPTION1(clazz->GetConstructorInfoByParamNames(
    //         mConstructorSignature, (IConstructorInfo**)&constructor), "GetConstructorInfoByParamNames");
    //     sConstructorMap->Put(csq, constructor.Get());
    // }
    // else {
    //     constructor = IConstructorInfo::Probe(value);
    //     // If we have a filter, apply it to cached constructor
    //     if (mFilter != NULL) {
    //         // Have we seen this name before?
    //         Boolean allowedState;
    //         AutoPtr<IInterface> stateTmp;
    //         mFilterMap->Get(csq, (IInterface**)&stateTmp);
    //         if (stateTmp == NULL) {
    //             // New class -- remember whether it is allowed
    //             AutoPtr<IClassLoader> cl;
    //             LAYOUT_INFLATOR_CATCH_EXCEPTION1(mContext->GetClassLoader((IClassLoader**)&cl), "GetClassLoader")
    //             assert(cl != NULL);
    //             LAYOUT_INFLATOR_CATCH_EXCEPTION1(cl->LoadClass(reflectionClassName, (IClassInfo**)&clazz), "LoadClass");

    //             Boolean allowed;
    //             mFilter->OnLoadClass(clazz, &allowed);
    //             allowed = allowed && (clazz != NULL);
    //             AutoPtr<IBoolean> stateInterface;
    //             CBoolean::New(allowed, (IBoolean**)&stateInterface);
    //             AutoPtr<IInterface> resultTmp;
    //             mFilterMap->Put(csq, stateInterface, (IInterface**)&resultTmp);
    //             if (!allowed) {
    //                 LAYOUT_INFLATOR_CATCH_EXCEPTION1(FailNotAllowed(name, prefix, attrs), "FailNotAllowed");
    //             }
    //         }
    //         else {
    //             AutoPtr<IBoolean> state = IBoolean::Probe(stateTmp);
    //             state->GetValue(&allowedState);
    //             if (!allowedState) {
    //                 LAYOUT_INFLATOR_CATCH_EXCEPTION1(FailNotAllowed(name, prefix, attrs), "FailNotAllowed");
    //             }
    //         }
    //     }
    // }

    // mConstructorArgs->Set(1, attrs);

    // AutoPtr<IArgumentList> args;
    // LAYOUT_INFLATOR_CATCH_EXCEPTION1(constructor->CreateArgumentList((IArgumentList**)&args), "CreateArgumentList");
    // LAYOUT_INFLATOR_CATCH_EXCEPTION1(args->SetInputArgumentOfObjectPtr(0, (*mConstructorArgs)[0]), "SetInputArgumentOfObjectPtr");
    // LAYOUT_INFLATOR_CATCH_EXCEPTION1(args->SetInputArgumentOfObjectPtr(1, (*mConstructorArgs)[1]), "SetInputArgumentOfObjectPtr");

    // AutoPtr<IInterface> obj;
    // LAYOUT_INFLATOR_CATCH_EXCEPTION1(constructor->CreateObject(args, (IInterface**)&obj), "CreateObject");
    // *view = IView::Probe(obj);
    // assert(*view != NULL);
    // REFCOUNT_ADD(*view);

    elog("====STC::CreateView====attrs====className:"+attrs.getClass().GetName());

    var bCustomControl = false;
    // bCustomControl = bCustomControl || reflectionClassName.indexOf("CalculatorEditText") > -1;
    // bCustomControl = bCustomControl || reflectionClassName.indexOf("CalculatorPadViewPager") > -1;
    // bCustomControl = bCustomControl || reflectionClassName.indexOf("CalculatorPadLayout") > -1;
    // bCustomControl = bCustomControl || reflectionClassName.indexOf("NodeLinearLayout") > -1;
    if (bCustomControl) {
        view = CObject.New("/data/elastos/Elastos.DevSamples.Node.JSCalculator.eco", reflectionClassName, oActivity, attrs);
    }
    else {
        view = Droid_New(reflectionClassName, oActivity, attrs);
    }

    // if (IViewStub::Probe(*view) != NULL) {
    //     // always use ourselves when inflating ViewStub later
    //     AutoPtr<ILayoutInflater> paraInflater;
    //     CloneInContext(IContext::Probe((*mConstructorArgs)[0]), (ILayoutInflater**)&paraInflater);
    //     IViewStub::Probe(*view)->SetLayoutInflater(paraInflater);
    // }

// #if defined(_DEBUG) || defined(_ELASTOS_DEBUG)
//     Int32 viewId;
//     (*view)->GetId(&viewId);
//     Slogger::D(TAG, " > CreateView: %s, id: 0x%08x, type: %s", TO_CSTR(*view), viewId, reflectionClassName.string());
// #endif

    elog("====STC::CreateView====end===="+view.getClass().GetName());

    // return NOERROR;
    return view;

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




// ECode LayoutInflater::RInflate(
//     /* [in] */ IXmlPullParser* parser,
//     /* [in] */ IView* parent,
//     /* [in] */ IAttributeSet* attrs,
//     /* [in] */ Boolean finishInflate,
//     /* [in] */ Boolean inheritContext)
// {
RInflate(parser, parent, attrs, finishInflate, inheritContext) {

    elog("====STC::RInflate====parent className:===RInflate===1==="+parent.getClass().GetName());


//     Int32 orgDepth, depth;
//     Int32 type;
//     String name;
    var name;
//     FAIL_RETURN(parser->GetDepth(&orgDepth));
    var orgDepth = parser.GetDepth();
//     FAIL_RETURN(parser->Next(&type))
    var type = parser.Next();
//     FAIL_RETURN(parser->GetDepth(&depth));
    var depth = parser.GetDepth();
//     while ((type != IXmlPullParser::END_TAG ||
//             depth > orgDepth) && type != IXmlPullParser::END_DOCUMENT) {
    while ( (type != END_TAG || depth > orgDepth) && type != END_DOCUMENT) {

//         if (type != IXmlPullParser::START_TAG) {
//             FAIL_RETURN(parser->Next(&type))
//             FAIL_RETURN(parser->GetDepth(&depth));
//             continue;
//         }
        if (type != START_TAG) {
            type = parser.Next();
            depth = parser.GetDepth();
            continue;
        }

//         FAIL_RETURN(parser->GetName(&name));
        name = parser.GetName();
//         if (DEBUG) {
//             Slogger::I(TAG, " parent: %s rInflate : %s  === ", TO_CSTR(parent), name.string());
//         }
        elog("====STC::RInflate===="+parent+": "+parent+" rInflate : "+name+"  === ");

//         if (name.Equals(TAG_REQUEST_FOCUS)) {
        if (name == TAG_REQUEST_FOCUS) {
//             FAIL_RETURN(ParseRequestFocus(parser, parent));
            this.ParseRequestFocus(parser, parent);
        }
//         else if (TAG_TAG.Equals(name)) {
        else if (name == TAG_TAG) {
//             FAIL_RETURN(ParseViewTag(parser, parent, attrs));
            this.ParseViewTag(parser, parent, attrs);
        }
//         else if (name.Equals(TAG_INCLUDE)) {
        else if (name == TAG_INCLUDE) {
//             Int32 d;
//             FAIL_RETURN(parser->GetDepth(&d));
            var d = parser.GetDepth();
//             if (d == 0) {
//                 Slogger::E(TAG, "<include /> cannot be the root element");
//                 return E_INFLATE_EXCEPTION;
//             }
            if (d == 0) {
                elog("====STC::RInflate====<include /> cannot be the root element");
                Assert(0);
            }

//             FAIL_RETURN(ParseInclude(parser, parent, attrs, inheritContext));
            this.ParseInclude(parser, parent, attrs, inheritContext);
        }
//         else if (name.Equals(TAG_MERGE)) {
        else if (name == TAG_MERGE) {
//             Slogger::E(TAG, "<merge /> must be the root element");
//             return E_INFLATE_EXCEPTION;
                elog("====STC::RInflate====<merge /> must be the root element");
                Assert(0);
        }
        else {
//             AutoPtr<IView> view;
//             FAIL_RETURN(CreateViewFromTag(parent, name, attrs, inheritContext, (IView**)&view));
elog("====STC::RInflate====6666===="+name+"===="+parent.GetChildCount());
                var view = this.CreateViewFromTag(parent, name, attrs, inheritContext);
elog("====STC::RInflate====7777====");
//             AutoPtr<IViewGroup> viewGroup = IViewGroup::Probe(parent);
                var viewGroup = parent;
//             AutoPtr<IViewGroupLayoutParams> params;
//             FAIL_RETURN(viewGroup->GenerateLayoutParams(attrs, (IViewGroupLayoutParams**)&params));
                var params = viewGroup.GenerateLayoutParams(attrs);
//             FAIL_RETURN(RInflate(parser, view, attrs, TRUE, TRUE));

var pName = parent.getClass().GetName();
var cName = view.getClass().GetName();
var aName = params.getClass().GetName();
elog(`====STC::RInflate====8888====${pName}->${aName}->${cName}`);
//elog("========parent className:===RInflate===1==="+parent.getClass().GetName());

CObject.showMethods(params);
if(params._obj){
    elog("====STC::RInflate====999====");
    CObject.showMethods(params._obj);
}


                this.RInflate(parser, view, attrs, true, true);

//             viewGroup->AddView(view, params);
                viewGroup.AddView(view, params);
        }

//         FAIL_RETURN(parser->Next(&type))
//         FAIL_RETURN(parser->GetDepth(&depth));
        type = parser.Next();
        depth = parser.GetDepth();
    }

//     if (finishInflate) {
//         ((View*)parent)->OnFinishInflate();
//     }
    if (finishInflate) {
        elog("====STC::RInflate====finishInflate====name:"+parent.getClass().GetName()+"====type:"+(typeof parent.OnFinishInflate));
        // CObject.showMethods(parent,"Inflate");
        // var pp = parent.Probe("Elastos.Droid.View.IView");
        // elog("====aaaa===="+pp.getClass().GetName());
        // CObject.showMethods(pp,"Inflate");

        // if(parent&&parent.OnFinishInflate) {
        //     parent.OnFinishInflate();
        // }
        // else {
        //     elog("====STC::RInflate====finishInflate====error: can not call protected method!");
        //     ss.tt();Assert(0);
        // }
        this.OnFinishInflate(parent);
    }

//     return NOERROR;
}    //RInflate


OnFinishInflate(parent) {
    var name = parent.getClass().GetName();
    elog("====STC::OnFinishInflate====name:"+name+"====type:"+(typeof parent.OnFinishInflate));

// CRelativeLayout        RelativeLayout::OnFinishInflate
// CButton                 Button::OnFinishInflate
// CLinearLayout           LinearLayout::OnFinishInflate
// CCalculatorEditText
// CCalculatorPadLayout
// CCalculatorPadViewPager

    if(parent&&parent.OnFinishInflate) {
        //parent.OnFinishInflate();
        parent.OnFinishInflate();
    }
    else {
        elog("====STC::RInflate====finishInflate====error: can not call protected method!");
        //ss.tt();Assert(0);
    }

    return;
}


// ECode LayoutInflater::ParseInclude(
//     /* [in] */ IXmlPullParser* parser,
//     /* [in] */ IView* parent,
//     /* [in] */ IAttributeSet* attrs,
//     /* [in] */ Boolean inheritContext)
// {
ParseInclude(parser, parent, attrs, inheritContext) {

elog("========parent className:===ParseInclude===1==="+parent.getClass().GetName());

    var mContext = oActivity;

//     Int32 type;
    var type;

//     AutoPtr<IViewGroup> group = IViewGroup::Probe(parent);
    var group = parent;
//     if (group != NULL) {
    if (group) {
//         Int32 layout;
//         attrs->GetAttributeResourceValue(String(NULL), String("layout"), 0, &layout);
        var layout = attrs.GetAttributeResourceValue("", "layout", 0);

        if (layout == 0) {
//             String value;
//             attrs->GetAttributeValue(String(NULL), String("layout"), &value);
            var value = attrs.GetAttributeValue("", layout);
            if (!value) {
                loge("====STC::ParseInclude====You must specifiy a layout in the include tag: <include layout=\"@layout/layoutID\" />");
                Assert(0);
            }
            else {
                loge("====STC::ParseInclude====You must specifiy a valid layout reference. The layout ID "+value+" is not valid.");
                Assert(0);
            }
        }
        else {
//             AutoPtr<IResources> res;
//             ASSERT_SUCCEEDED(mContext->GetResources((IResources**)&res));
            //var res = mContext.GetResources();
            var res = mContext.GetResources();

//             AutoPtr<IXmlResourceParser> childParser;
//             ASSERT_SUCCEEDED(res->GetLayout(layout, (IXmlResourceParser**)&childParser));
            //var childParser = res.GetLayout(layout);
            var childParser = res.GetLayout(layout).Probe("Org.Xmlpull.V1.IXmlPullParser");

// //            try {
//             AutoPtr<IAttributeSet> childAttrs = Xml::AsAttributeSet(IXmlPullParser::Probe(childParser));
            var childAttrs = childParser.Probe("Elastos.Droid.Utility.IAttributeSet");

//             FAIL_RETURN_WITH_CLOSE(IXmlPullParser::Probe(childParser)->Next(&type), childParser)
            var type = childParser.Next();
//             while (type != IXmlPullParser::START_TAG && type != IXmlPullParser::END_DOCUMENT) {
//                 // Empty.
//                 FAIL_RETURN_WITH_CLOSE(IXmlPullParser::Probe(childParser)->Next(&type), childParser)
//             }
            while (type != START_TAG && type != END_DOCUMENT) {
                // Empty.
                type = childParser.Next();
            }

//             if (type != IXmlPullParser::START_TAG) {
//                 String des;
//                 IXmlPullParser::Probe(childParser)->GetPositionDescription(&des);
//                 Slogger::E(TAG, "%s: No start tag found!", des.string());
//                 childParser->Close();
//                 return E_INFLATE_EXCEPTION;
//             }
            if (type != START_TAG) {
                var des = childParser.GetPositionDescription();
                elog("====STC::ParseInclude===="+des+": No start tag found!");
                childParser.Close();
                ss.tt();Assert(0);
            }

//             String childName;
//             FAIL_RETURN_WITH_CLOSE(IXmlPullParser::Probe(childParser)->GetName(&childName), childParser)
            var childName = childParser.GetName();
//             if (DEBUG) {
//                 Slogger::I(TAG, " layout:%08x > childName: %s", layout, childName.string());
//             }
            elog("====STC::ParseInclude==== layout:"+layout+" > childName: "+childName);
//             if (childName.Equals(TAG_MERGE)) {
            if (childName == TAG_MERGE) {
//                 // Inflate all children.
//                 FAIL_RETURN_WITH_CLOSE(RInflate(IXmlPullParser::Probe(childParser), parent, childAttrs, FALSE, inheritContext), childParser)
                this.RInflate(childParser, parent, childAttrs, false, inheritContext)
            }
            else {
elog("====STC::ParseInclude====00====");
//                 AutoPtr<IView> view;
//                 FAIL_RETURN_WITH_CLOSE(CreateViewFromTag(parent, childName, childAttrs, inheritContext, (IView**)&view), childParser)
                var view = this.CreateViewFromTag(parent, childName, childAttrs, inheritContext);
elog("====STC::ParseInclude====01====");

//                 // We try to load the layout params set in the <include /> tag. If
//                 // they don't exist, we will rely on the layout params set in the
//                 // included XML file.
//                 // During a layoutparams generation, a runtime exception is thrown
//                 // if either layout_width or layout_height is missing. We catch
//                 // this exception and set localParams accordingly: true means we
//                 // successfully loaded layout params from the <include /> tag,
//                 // false means we need to rely on the included layout params.
//                 AutoPtr<IViewGroupLayoutParams> params;
// //                try {
//                 ECode ec = group->GenerateLayoutParams(attrs, (IViewGroupLayoutParams**)&params);
//                 if (ec == (Int32)E_RUNTIME_EXCEPTION) {
//                     params = NULL;
//                     FAIL_RETURN_WITH_CLOSE(group->GenerateLayoutParams(childAttrs, (IViewGroupLayoutParams**)&params), childParser)
//                 }
// //                } catch (RuntimeException e) {
// //                    params = group.generateLayoutParams(childAttrs);
// //                } finally {
//                 if (params != NULL) {
//                     view->SetLayoutParams(params.Get());
//                 }
                var params = group.GenerateLayoutParams(childAttrs);

var pName = parent.getClass().GetName();
var cName = view.getClass().GetName();
var aName = params.getClass().GetName();
elog(`====STC::ParseInclude====8888====${pName}->${aName}->${cName}`);

CObject.showMethods(params);
if(params._obj){
    elog("====STC::ParseInclude====03====");
    CObject.showMethods(params._obj);
}

                if (params) {
elog("====STC::ParseInclude====04====");
                    view.SetLayoutParams(params);
                }
// //                }

//                 // Inflate all children.
//                 FAIL_RETURN_WITH_CLOSE(RInflate(IXmlPullParser::Probe(childParser), view, childAttrs, TRUE, TRUE), childParser)

elog("====STC::ParseInclude====view className:"+view.getClass().GetName());

                this.RInflate(childParser, view, childAttrs, true, true);

//elog("====STC::ParseInclude====params className:"+params.getClass().GetName());
//sss.ttt();Assert();

//                 // Attempt to override the included layout's android:id with the
//                 // one set on the <include /> tag itself.
//                 AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::View);
                var attrIds = R.Droid.styleable.View;
//                 AutoPtr<ITypedArray> a;
//                 mContext->ObtainStyledAttributes(attrs, attrIds, 0, 0, (ITypedArray**)&a);
                var a = mContext.ObtainStyledAttributes(attrs, attrIds, 0, 0);
//                 Int32 id;
//                 a->GetResourceId(R::styleable::View_id, IView::NO_ID, &id);
                var id = a.GetResourceId(R.styleable.View_id, IView__NO_ID);
//                 // While we're at it, let's try to override android:visibility.
//                 Int32 visibility;
//                 a->GetInt32(R::styleable::View_visibility, -1, &visibility);
                var visibility = a.GetInt32(R.styleable.View_visibility, -1);
//                 a->Recycle();
                a.Recycle();

//                 if (id != IView::NO_ID) {
//                     view->SetId(id);
//                 }
                if (id != IView__NO_ID) {
                    view.SetId(id);
                }

//                 switch (visibility) {
//                     case 0:
//                         view->SetVisibility(IView::VISIBLE);
//                         break;
//                     case 1:
//                         view->SetVisibility(IView::INVISIBLE);
//                         break;
//                     case 2:
//                         view->SetVisibility(IView::GONE);
//                         break;
//                 }
                switch (visibility) {
                    case 0:
                        view.SetVisibility(IView__VISIBLE);
                        break;
                    case 1:
                        view.SetVisibility(IView__INVISIBLE);
                        break;
                    case 2:
                        view.SetVisibility(IView__GONE);
                        break;
                }

//                 group->AddView(view);
                group.AddView(view);
            }

            //childParser.Close();
            childParser.Probe("Elastos.IO.ICloseable").Close();
        }
    } else {
//         Slogger::E(TAG, "<include /> can only be used inside of a ViewGroup");
//         return E_INFLATE_EXCEPTION;
        elog("====STC::ParseInclude====<include /> can only be used inside of a ViewGroup");
        Assert(0);
    }

//     Int32 currentDepth, depth;
//     FAIL_RETURN(parser->GetDepth(&currentDepth));
    var currentDepth = parser.GetDepth();
//     FAIL_RETURN(parser->Next(&type))
    type = parser.Next();
//     FAIL_RETURN(parser->GetDepth(&depth));
    var depth = parser.GetDepth();
//     while ((type != IXmlPullParser::END_TAG ||
//             depth > currentDepth) &&
//             type != IXmlPullParser::END_DOCUMENT) {
//         // Empty
//         FAIL_RETURN(parser->Next(&type))
//         FAIL_RETURN(parser->GetDepth(&depth));
//     }
    while ( (type != END_TAG || depth > currentDepth) && type != END_DOCUMENT) {
        // Empty
        type = parser.Next();
        depth = parser.GetDepth();
    }

//     return NOERROR;
    return;
}   //ParseInclude

}   //class STC

return STC;

};  //module.exports

elog("====elastos_layout.js====end====");