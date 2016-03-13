#include "CMonkeySourceNetworkViews.h"
#include "MonkeySourceNetwork.h"
#include <elastos/core/StringBuffer.h>
#include <elastos/core/StringBuilder.h>
#include "CMonkeyCommandReturn.h"
#include <elastos/core/StringUtils.h>
#include "CMonkeySourceNetworkViews.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuffer;
using Elastos::Core::StringBuilder;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Core::StringUtils;

using Elastos::Droid::AccessibilityService::CUiTestAutomationBridge;
using Elastos::Droid::AccessibilityService::IUiTestAutomationBridge;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::Accessibility::CAccessibilityNodeInfoHelper;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfoHelper;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

AutoPtr<IUiTestAutomationBridge> CMonkeySourceNetworkViews::sUiTestAutomationBridge;
AutoPtr<IIPackageManager> CMonkeySourceNetworkViews::sPm = CMonkeySourceNetworkViews::InitStatics();
const String CMonkeySourceNetworkViews::CLASS_NOT_FOUND = String("Error retrieving class information");
const String CMonkeySourceNetworkViews::NO_ACCESSIBILITY_EVENT = String("No accessibility event has occured yet");
const String CMonkeySourceNetworkViews::NO_NODE = String("Node with given ID does not exist");
const String CMonkeySourceNetworkViews::NO_CONNECTION = String("Failed to connect to AccessibilityService, " \
                                                "try restarting Monkey");
AutoPtr<HashMap<String, void*> > CMonkeySourceNetworkViews::sClassMap = new HashMap<String, void*>();

AutoPtr<HashMap<String, AutoPtr<CMonkeySourceNetworkViews::ViewIntrospectionCommand> > > CMonkeySourceNetworkViews::COMMAND_MAP;

 AutoPtr<IIPackageManager> CMonkeySourceNetworkViews::InitStatics()
 {
    AutoPtr<IServiceManager> sm;
    ASSERT_SUCCEEDED(CServiceManager::AcquireSingleton((IServiceManager**)&sm));
    AutoPtr<IInterface> obj;
    sm->GetService(String("package"), (IInterface**)&obj);
    AutoPtr<IIPackageManager> pm = IIPackageManager::Probe(obj);

    COMMAND_MAP = new HashMap<String, AutoPtr<ViewIntrospectionCommand> >();
    (*COMMAND_MAP)[String("getlocation")] = new GetLocation();
    (*COMMAND_MAP)[String("gettext")] = new GetText();
    (*COMMAND_MAP)[String("getclass")] = new GetClass();
    (*COMMAND_MAP)[String("getchecked")] = new GetChecked();
    (*COMMAND_MAP)[String("getenabled")] = new GetEnabled();
    (*COMMAND_MAP)[String("getselected")] = new GetSelected();
    (*COMMAND_MAP)[String("setselected")] = new SetSelected();
    (*COMMAND_MAP)[String("getfocused")] = new GetFocused();
    (*COMMAND_MAP)[String("setfocused")] = new SetFocused();
    (*COMMAND_MAP)[String("getparent")] = new GetParent();
    (*COMMAND_MAP)[String("getchildren")] = new GetChildren();
    (*COMMAND_MAP)[String("getaccessibilityids")] = new GetAccessibilityIds();
    return pm;
}

ECode CMonkeySourceNetworkViews::Setup()
{
    CUiTestAutomationBridge::New((IUiTestAutomationBridge**)&sUiTestAutomationBridge);
    sUiTestAutomationBridge->Connect();
    return NOERROR;
}


//Todo
/*private static Class<?> getIdClass(String packageName, String sourceDir)
            throws ClassNotFoundException {
        // This kind of reflection is expensive, so let's only do it
        // if we need to
        Class<?> klass = sClassMap.get(packageName);
        if (klass == null) {
            DexClassLoader classLoader = new DexClassLoader(
                    sourceDir, "/data/local/tmp",
                    null, ClassLoader.getSystemClassLoader());
            klass = classLoader.loadClass(packageName + ".R$id");
            sClassMap.put(packageName, klass);
        }
        return klass;
    }*/

CAR_INTERFACE_IMPL_2(CMonkeySourceNetworkViews::ListViewsCommand, IMonkeyCommand,
    IListViewsCommand);

ECode CMonkeySourceNetworkViews::ListViewsCommand::TranslateCommand(
            /* [in] */ ArrayOf<String>* command,
            /* [in] */ ICommandQueue* queue,
            /* [out] */ IMonkeyCommandReturn** ret)
{
    VALIDATE_NOT_NULL(ret)
    AutoPtr<IAccessibilityEvent> lastEvent = NULL;
    sUiTestAutomationBridge->GetLastAccessibilityEvent((IAccessibilityEvent**)&lastEvent);
    if(!lastEvent.Get())
    {
        CMonkeyCommandReturn::New(FALSE, NO_ACCESSIBILITY_EVENT, ret);
        return NOERROR;
    }
    AutoPtr<IAccessibilityNodeInfo> node = NULL;
    lastEvent->GetSource((IAccessibilityNodeInfo**)&node);
    /* Occasionally the API will generate an event with no source, which is essentially the
             * same as it generating no event at all */
    if(!node.Get())
    {
        CMonkeyCommandReturn::New(FALSE, NO_ACCESSIBILITY_EVENT, ret);
        return NOERROR;
    }

    AutoPtr<ICharSequence> pn = NULL;
    String packageName;
    node->GetPackageName((ICharSequence**)&pn);
    pn->ToString(&packageName);
    /*try{
            Class<?> klass;
            ApplicationInfo appInfo = sPm.getApplicationInfo(packageName, 0, UserHandle.myUserId());
            klass = getIdClass(packageName, appInfo.sourceDir);
            StringBuilder fieldBuilder = new StringBuilder();
            Field[] fields = klass.getFields();
            for (Field field : fields) {
                fieldBuilder.append(field.getName() + " ");
            }
            return new MonkeyCommandReturn(true, fieldBuilder.toString());
        } catch (RemoteException e){
            return new MonkeyCommandReturn(false, REMOTE_ERROR);
        } catch (ClassNotFoundException e){
            return new MonkeyCommandReturn(false, CLASS_NOT_FOUND);
        }*/
    AutoPtr<IApplicationInfo> appInfo;
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetMyUserId(&userId);
    sPm->GetApplicationInfo(packageName, 0, userId, (IApplicationInfo**)&appInfo);
    String clsName;
    appInfo->GetClassName(&clsName);
    AutoPtr<IModuleInfo> moduleInfo;
    AutoPtr<IClassInfo> classInfo;
    AutoPtr<IMethodInfo> methodInfo;
    //AutoPtr<IArgumentList> argumentList;
    _CReflector_AcquireModuleInfo(
            packageName, (IModuleInfo**)&moduleInfo);
    moduleInfo->GetClassInfo(
            clsName, (IClassInfo**)&classInfo);
    //Todo

    return NOERROR;
}

CAR_INTERFACE_IMPL_2(CMonkeySourceNetworkViews::QueryViewCommand, IMonkeyCommand,
    IQueryViewCommand);

ECode CMonkeySourceNetworkViews::QueryViewCommand::TranslateCommand(
            /* [in] */ ArrayOf<String>* command,
            /* [in] */ ICommandQueue* queue,
            /* [out] */ IMonkeyCommandReturn** ret)
{
    VALIDATE_NOT_NULL(ret)
    if(command->GetLength() > 2)
    {
        Boolean isConnected = FALSE;
        sUiTestAutomationBridge->IsConnected(&isConnected);
        if(!isConnected)
        {
            CMonkeyCommandReturn::New(FALSE, NO_CONNECTION, ret);
            return NOERROR;
        }

        AutoPtr<IAccessibilityEvent> lastEvent = NULL;
        sUiTestAutomationBridge->GetLastAccessibilityEvent((IAccessibilityEvent**)&lastEvent);
        if(!lastEvent.Get())
        {
            CMonkeyCommandReturn::New(FALSE, NO_ACCESSIBILITY_EVENT, ret);
            return NOERROR;
        }

        String idType = (*command)[1];
        AutoPtr<IAccessibilityNodeInfo> node = NULL;
        String viewQuery;
        AutoPtr<List<String> > args = new List<String>();
        Int32 length = command->GetLength();
        if(idType.Equals("viewid"))
        {
            node = GetNodeByViewId((*command)[2], lastEvent.Get());
            viewQuery = (*command)[3];
            for(Int32 i = 4; i < length; ++i)
            {
                 args->PushBack((*command)[i]);
            }
        }
        else if(idType.Equals("accessibilityids"))
        {
            node = GetNodeByAccessibilityIds((*command)[2], (*command)[3]);
            viewQuery = (*command)[4];
            for(Int32 i = 5; i < length; ++i)
            {
                 args->PushBack((*command)[i]);
            }
        }
        else
        {
            *ret = MonkeySourceNetwork::EARG;
            REFCOUNT_ADD(*ret);
            return NOERROR;
        }

        if(!node.Get())
        {
            CMonkeyCommandReturn::New(FALSE, NO_NODE, ret);
            return NOERROR;
        }

        AutoPtr<ViewIntrospectionCommand> getter= (*COMMAND_MAP)[viewQuery];
        if(getter)
        {
            getter->Query(node, args, ret);
        }
        else
        {
            *ret = MonkeySourceNetwork::EARG;
            REFCOUNT_ADD(*ret);
            return NOERROR;
        }
    }
    *ret = MonkeySourceNetwork::EARG;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(CMonkeySourceNetworkViews::GetRootViewCommand, IMonkeyCommand,
    IGetRootViewCommand);

ECode CMonkeySourceNetworkViews::GetRootViewCommand::TranslateCommand(
            /* [in] */ ArrayOf<String>* command,
            /* [in] */ ICommandQueue* queue,
            /* [out] */ IMonkeyCommandReturn** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<IAccessibilityEvent> lastEvent = NULL;
    sUiTestAutomationBridge->GetLastAccessibilityEvent((IAccessibilityEvent**)&lastEvent);
    if(!lastEvent.Get())
    {
        CMonkeyCommandReturn::New(FALSE, NO_ACCESSIBILITY_EVENT, ret);
        return NOERROR;
    }
    AutoPtr<IAccessibilityNodeInfo> node = NULL;
    lastEvent->GetSource((IAccessibilityNodeInfo**)&node);
    AutoPtr<GetAccessibilityIds> accessibilityids = new GetAccessibilityIds();

    AutoPtr<List<String> > arrayList = new List<String>();
    accessibilityids->Query(node, arrayList, ret);
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(CMonkeySourceNetworkViews::GetViewsWithTextCommand, IMonkeyCommand,
    IGetViewsWithTextCommand);

ECode CMonkeySourceNetworkViews::GetViewsWithTextCommand::TranslateCommand(
            /* [in] */ ArrayOf<String>* command,
            /* [in] */ ICommandQueue* queue,
            /* [out] */ IMonkeyCommandReturn** ret)
{
    VALIDATE_NOT_NULL(ret);
    Boolean isConnected = FALSE;
    sUiTestAutomationBridge->IsConnected(&isConnected);
    if(!isConnected)
    {
        CMonkeyCommandReturn::New(FALSE, NO_CONNECTION, ret);
        return NOERROR;
    }

    if(2 == command->GetLength())
    {
        String text = (*command)[1];
        AutoPtr<IObjectContainer> nodes = NULL;
        sUiTestAutomationBridge->FindAccessibilityNodeInfosByText(
            IUiTestAutomationBridge::ACTIVE_WINDOW_ID,
            IUiTestAutomationBridge::ROOT_NODE_ID,
            text,
            (IObjectContainer**)&nodes);
        AutoPtr<ViewIntrospectionCommand> idGetter = new GetAccessibilityIds();
        AutoPtr<List<String> > emptyArgs = new List<String>();
        StringBuilder ids;
        AutoPtr<IObjectEnumerator> enumerator;
        nodes->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
        Boolean hasNext = FALSE;
        while (enumerator->MoveNext(&hasNext), hasNext) {
            AutoPtr<IInterface> node;
            enumerator->Current((IInterface**)&node);
            AutoPtr<IMonkeyCommandReturn> result = NULL;
            idGetter->Query(IAccessibilityNodeInfo::Probe(node), emptyArgs, (IMonkeyCommandReturn**)&result);
            Boolean suc = FALSE;
            result->WasSuccessful(&suc);
            if(!suc)
            {
                *ret = result;
                REFCOUNT_ADD(*ret);
                return NOERROR;
            }
            String tmp;
            result->GetMessage(&tmp);
            ids.AppendString(tmp);
            ids.AppendString(String(" "));
        }
        String tmp;
        ids.ToString(&tmp);
        CMonkeyCommandReturn::New(TRUE, tmp, ret);
        return NOERROR;
    }

    *ret = MonkeySourceNetwork::EARG;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CMonkeySourceNetworkViews::GetLocation::Query(
            /* [in] */ IAccessibilityNodeInfo* node,
            /* [in] */ List<String>* args,
            /* [out] */IMonkeyCommandReturn** ret)
{
    VALIDATE_NOT_NULL(ret);
    if(0 == args->GetSize())
    {
        AutoPtr<IRect> nodePosition;
        CRect::New((IRect**)&nodePosition);
        node->GetBoundsInScreen(nodePosition);
        StringBuilder positions;
        Int32 left = 0;
        Int32 right = 0;
        Int32 top = 0;
        Int32 bottom = 0;
        nodePosition->Get(&left, &top, &right, &bottom);
        positions.AppendInt32(left);
        positions.AppendString(String(" "));
        positions.AppendInt32(top);
        positions.AppendString(String(" "));
        positions.AppendInt32(right - left);
        positions.AppendString(String(" "));
        positions.AppendInt32(bottom - top);
        String tmp;
        positions.ToString(&tmp);
        CMonkeyCommandReturn::New(TRUE, tmp, ret);
        return NOERROR;
    }
    *ret = MonkeySourceNetwork::EARG;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CMonkeySourceNetworkViews::GetText::Query(
            /* [in] */ IAccessibilityNodeInfo* node,
            /* [in] */ List<String>* args,
            /* [out] */IMonkeyCommandReturn** ret)
{
    VALIDATE_NOT_NULL(ret);
    if(0 == args->GetSize())
    {
        Boolean isPassword = FALSE;
        node->IsPassword(&isPassword);
        if(isPassword)
        {
            CMonkeyCommandReturn::New(FALSE, String("Node contains a password"), ret);
            return NOERROR;
        }
        /* Occasionally we get a null from the accessibility API, rather than an empty
                 * string */
        AutoPtr<ICharSequence> text;
        node->GetText((ICharSequence**)&text);
        if(!text)
        {
            CMonkeyCommandReturn::New(TRUE, String(""), ret);
            return NOERROR;
        }
        String textString;
        text->ToString(&textString);
        CMonkeyCommandReturn::New(TRUE, textString, ret);
        return NOERROR;
    }

    *ret = MonkeySourceNetwork::EARG;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CMonkeySourceNetworkViews::GetClass::Query(
            /* [in] */ IAccessibilityNodeInfo* node,
            /* [in] */ List<String>* args,
            /* [out] */IMonkeyCommandReturn** ret)
{
    VALIDATE_NOT_NULL(ret);
    if(0 == args->GetSize())
    {
        AutoPtr<ICharSequence> className;
        node->GetClassName((ICharSequence**)&className);
        String textString;
        className->ToString(&textString);
        CMonkeyCommandReturn::New(TRUE, textString, ret);
        return NOERROR;
    }

    *ret = MonkeySourceNetwork::EARG;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CMonkeySourceNetworkViews::GetChecked::Query(
            /* [in] */ IAccessibilityNodeInfo* node,
            /* [in] */ List<String>* args,
            /* [out] */IMonkeyCommandReturn** ret)
{
    VALIDATE_NOT_NULL(ret);
    if(0 == args->GetSize())
    {
        Boolean isChecked = FALSE;
        node->IsChecked(&isChecked);
        CMonkeyCommandReturn::New(TRUE, StringUtils::BooleanToString(isChecked), ret);
        return NOERROR;
    }

    *ret = MonkeySourceNetwork::EARG;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CMonkeySourceNetworkViews::GetEnabled::Query(
            /* [in] */ IAccessibilityNodeInfo* node,
            /* [in] */ List<String>* args,
            /* [out] */IMonkeyCommandReturn** ret)
{
    VALIDATE_NOT_NULL(ret);
    if(0 == args->GetSize())
    {
        Boolean isEnabled = FALSE;
        node->IsEnabled(&isEnabled);
        CMonkeyCommandReturn::New(TRUE, StringUtils::BooleanToString(isEnabled), ret);
        return NOERROR;
    }

    *ret = MonkeySourceNetwork::EARG;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CMonkeySourceNetworkViews::GetSelected::Query(
            /* [in] */ IAccessibilityNodeInfo* node,
            /* [in] */ List<String>* args,
            /* [out] */IMonkeyCommandReturn** ret)
{
    VALIDATE_NOT_NULL(ret);
    if(0 == args->GetSize())
    {
        Boolean isSelected = FALSE;
        node->IsSelected(&isSelected);
        CMonkeyCommandReturn::New(TRUE, StringUtils::BooleanToString(isSelected), ret);
        return NOERROR;
    }

    *ret = MonkeySourceNetwork::EARG;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CMonkeySourceNetworkViews::SetSelected::Query(
            /* [in] */ IAccessibilityNodeInfo* node,
            /* [in] */ List<String>* args,
            /* [out] */IMonkeyCommandReturn** ret)
{
    VALIDATE_NOT_NULL(ret);
    if(1 == args->GetSize())
    {
        Boolean actionPerformed;
        if((*args)[0].EqualsIgnoreCase("true"))
        {
            node->PerformAction(IAccessibilityNodeInfo::ACTION_SELECT, &actionPerformed);
        }
        else
        {
            node->PerformAction(IAccessibilityNodeInfo::ACTION_CLEAR_SELECTION, &actionPerformed);
        }
        CMonkeyCommandReturn::New(actionPerformed, ret);
        return NOERROR;
    }
    *ret = MonkeySourceNetwork::EARG;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CMonkeySourceNetworkViews::GetFocused::Query(
            /* [in] */ IAccessibilityNodeInfo* node,
            /* [in] */ List<String>* args,
            /* [out] */IMonkeyCommandReturn** ret)
{
    VALIDATE_NOT_NULL(ret);
    if(0 == args->GetSize())
    {
        Boolean isFocused = FALSE;
        node->IsFocused(&isFocused);
        CMonkeyCommandReturn::New(TRUE, StringUtils::BooleanToString(isFocused), ret);
        return NOERROR;
    }

    *ret = MonkeySourceNetwork::EARG;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CMonkeySourceNetworkViews::SetFocused::Query(
            /* [in] */ IAccessibilityNodeInfo* node,
            /* [in] */ List<String>* args,
            /* [out] */IMonkeyCommandReturn** ret)
{
    VALIDATE_NOT_NULL(ret);
    if(1 == args->GetSize())
    {
        Boolean actionPerformed;
        if((*args)[0].EqualsIgnoreCase("true"))
        {
            node->PerformAction(IAccessibilityNodeInfo::ACTION_FOCUS, &actionPerformed);
        }
        else
        {
            node->PerformAction(IAccessibilityNodeInfo::ACTION_CLEAR_FOCUS, &actionPerformed);
        }
        CMonkeyCommandReturn::New(actionPerformed, ret);
        return NOERROR;
    }
    *ret = MonkeySourceNetwork::EARG;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CMonkeySourceNetworkViews::GetAccessibilityIds::Query(
            /* [in] */ IAccessibilityNodeInfo* node,
            /* [in] */ List<String>* args,
            /* [out] */IMonkeyCommandReturn** ret)
{
    VALIDATE_NOT_NULL(ret);
    if(0 == args->GetSize())
    {
        Int32 viewId;
        Int64 sourceNodeId = 0;
        node->GetSourceNodeId(&sourceNodeId);
        AutoPtr<IAccessibilityNodeInfoHelper> helper;
        ASSERT_SUCCEEDED(CAccessibilityNodeInfoHelper::AcquireSingleton((IAccessibilityNodeInfoHelper**)&helper));
        helper->GetAccessibilityViewId(sourceNodeId, &viewId);
        String ids;
        Int32 windowId = 0;
        node->GetWindowId(&windowId);
        ids.AppendFormat("%d %d", windowId, viewId);
        CMonkeyCommandReturn::New(TRUE, ids, ret);
        return NOERROR;
    }
    *ret = MonkeySourceNetwork::EARG;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CMonkeySourceNetworkViews::GetParent::Query(
            /* [in] */ IAccessibilityNodeInfo* node,
            /* [in] */ List<String>* args,
            /* [out] */IMonkeyCommandReturn** ret)
{
    VALIDATE_NOT_NULL(ret);
    if(0 == args->GetSize())
    {
        AutoPtr<IAccessibilityNodeInfo> parent;
        node->GetParent((IAccessibilityNodeInfo**)&parent);
        if(!parent)
        {
            CMonkeyCommandReturn::New(FALSE, String("Given node has no parent"), ret);
            return NOERROR;
        }
        ViewIntrospectionCommand* idGetter = new GetAccessibilityIds();
        AutoPtr<List<String> > emptyArgs = new List<String>();
        idGetter->Query(parent, emptyArgs, ret);
        if(idGetter)
            delete idGetter;
        return NOERROR;
    }
    *ret = MonkeySourceNetwork::EARG;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CMonkeySourceNetworkViews::GetChildren::Query(
    /* [in] */ IAccessibilityNodeInfo* node,
    /* [in] */ List<String>* args,
    /* [out] */IMonkeyCommandReturn** ret)
{
    VALIDATE_NOT_NULL(ret);
    if(args->IsEmpty())
    {
        AutoPtr<ViewIntrospectionCommand> idGetter = new GetAccessibilityIds();
        AutoPtr<List<String> > emptyArgs = new List<String>();
        StringBuilder ids;
        Int32 totalChildren = 0;
        node->GetChildCount(&totalChildren);
        for(Int32 i = 0; i < totalChildren; ++i)
        {
            AutoPtr<IAccessibilityNodeInfo> childNode;
            node->GetChild(i, (IAccessibilityNodeInfo**)&childNode);
            AutoPtr<IMonkeyCommandReturn> result;
            idGetter->Query(childNode, emptyArgs, (IMonkeyCommandReturn**)&result);
            Boolean wasSuccessful = FALSE;
            result->WasSuccessful(&wasSuccessful);
            if(!wasSuccessful)
            {
                *ret = result;
                REFCOUNT_ADD(*ret);
                return NOERROR;
            }
            else
            {
                String msg;
                result->GetMessage(&msg);
                msg.AppendFormat(" ");
                ids.AppendString(msg);
            }
        }
        String str;
        ids.ToString(&str);
        CMonkeyCommandReturn::New(TRUE, str, ret);
        return NOERROR;
    }
    *ret = MonkeySourceNetwork::EARG;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

String CMonkeySourceNetworkViews::GetPositionFromNode(IAccessibilityNodeInfo* node)
{
    AutoPtr<IRect> nodePosition;
    CRect::New((IRect**)&nodePosition);
    node->GetBoundsInScreen(nodePosition);
    StringBuilder positions;
    Int32 left = 0;
    Int32 right = 0;
    Int32 top = 0;
    Int32 bottom = 0;
    nodePosition->Get(&left, &top, &right, &bottom);
    positions.AppendInt32(left);
    positions.Append(" ");
    positions.AppendInt32(top);
    positions.Append(" ");
    positions.AppendInt32(right - left);
    positions.Append(" ");
    positions.AppendInt32(bottom - top);
    String ret;
    positions.ToString(&ret);
    return ret;
}

Int32 CMonkeySourceNetworkViews::GetId(
        /* [in] */ const String& stringId,
        /* [in] */ IAccessibilityEvent *event)
{
        /*try {
        AccessibilityNodeInfo node = event.getSource();
        String packageName = node.getPackageName().toString();
        ApplicationInfo appInfo = sPm.getApplicationInfo(packageName, 0, UserHandle.myUserId());
        Class<?> klass;
        klass = getIdClass(packageName, appInfo.sourceDir);
        return klass.getField(stringId).getInt(null);
        } catch (RemoteException e) {
            throw new MonkeyViewException(REMOTE_ERROR);
        } catch (ClassNotFoundException e){
            throw new MonkeyViewException(e.getMessage());
        } catch (NoSuchFieldException e){
            throw new MonkeyViewException("No such node with given id");
        } catch (IllegalAccessException e){
            throw new MonkeyViewException("Private identifier");
        } catch (NullPointerException e) {
            // AccessibilityServiceConnection throws a NullPointerException if you hand it
            // an ID that doesn't exist onscreen
            throw new MonkeyViewException("No node with given id exists onscreen");
        }*/
    //Todo
    AutoPtr<IAccessibilityNodeInfo> node;
    event->GetSource((IAccessibilityNodeInfo**)&node);
    AutoPtr<ICharSequence> pkg;
    node->GetPackageName((ICharSequence**)&pkg);
    String packageName;
    pkg->ToString(&packageName);
    AutoPtr<IApplicationInfo> appInfo;
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetMyUserId(&userId);
    sPm->GetApplicationInfo(packageName, 0, userId, (IApplicationInfo**)&appInfo);
    String clsName;
    appInfo->GetClassName(&clsName);
    AutoPtr<IModuleInfo> moduleInfo;
    AutoPtr<IClassInfo> classInfo;
    AutoPtr<IMethodInfo> methodInfo;
    AutoPtr<IArgumentList> argumentList;
    _CReflector_AcquireModuleInfo(
            packageName, (IModuleInfo**)&moduleInfo);
    moduleInfo->GetClassInfo(
            clsName, (IClassInfo**)&classInfo);
    //Todo
    //ASSERT(0 && "get value of static field named stringId##");
    return 0;
}

IAccessibilityNodeInfo* CMonkeySourceNetworkViews::GetNodeByAccessibilityIds(
        /* [in] */ const String& windowString,
        /* [in] */ const String& viewString)
{
    Int32 windowId = 0;
    Int32 viewId = 0;
    StringUtils::ParseInt32(windowString, &windowId);
    StringUtils::ParseInt32(viewString, &viewId);
    AutoPtr<IAccessibilityNodeInfo> info;
    sUiTestAutomationBridge->FindAccessibilityNodeInfoByAccessibilityId(
        windowId,
        viewId,
        (IAccessibilityNodeInfo**)&info);
    return info;
}

IAccessibilityNodeInfo* CMonkeySourceNetworkViews::GetNodeByViewId(
        /* [in] */ const String& viewId,
        /* [in] */ IAccessibilityEvent *event)
{
    AutoPtr<IAccessibilityNodeInfo> info;
    Int32 id = GetId(viewId, event);
    sUiTestAutomationBridge->FindAccessibilityNodeInfoByViewId(
        IUiTestAutomationBridge::ACTIVE_WINDOW_ID,
        IUiTestAutomationBridge::ROOT_NODE_ID,
        id,
        (IAccessibilityNodeInfo**)&info);
    return info;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
