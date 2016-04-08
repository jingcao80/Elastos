
#include <Elastos.CoreLibrary.External.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/view/MenuInflater.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContextWrapper;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::View::Menu::IMenuItemImpl;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::R;
using Elastos::Core::IClassLoader;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {

//=============================================================================
// MenuInflater::InflatedOnMenuItemClickListener
//=============================================================================
MenuInflater::InflatedOnMenuItemClickListener::InflatedOnMenuItemClickListener(
    /* [in] */ IObject* realOwner,
    /* [in] */ const String& methodName)
{
    assert(0);
    /*mRealOwner = realOwner;
    ClassID clsid;
    realOwner->GetClassID(&clsid);
    AutoPtr<IModuleInfo> module;
    _CReflector_AcquireModuleInfo(clsid.pUunm, (IModuleInfo**)&module);
    Int32 count;
    module->GetClassCount(&count);
    BufferOf<IClassInfo*>* classes = BufferOf<IClassInfo*>::Alloc(count);
    module->GetAllClassInfos(classes);
    AutoPtr<IClassInfo> clazz;
    for (Int32 i = 0; i < classes->GetUsed(); ++i) {
        clazz = (*classes)[i];
        ClassID id;
        clazz->GetId(&id);
        if (clsid == id) break;
    }
    clazz->GetMethodInfo(methodName.string(), (IMethodInfo**)&mMethod);
    for (Int32 i = 0; i < classes->GetUsed(); ++i) {
        (*classes)[i]->Release();
    }
    BufferOf<IClassInfo*>::Free(classes);*/
}

CAR_INTERFACE_IMPL(MenuInflater::InflatedOnMenuItemClickListener, Object, IOnMenuItemClickListener)

ECode MenuInflater::InflatedOnMenuItemClickListener::OnMenuItemClick(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    AutoPtr<IArgumentList> argumentList;
    mMethod->CreateArgumentList((IArgumentList**)&argumentList);
    argumentList->SetInputArgumentOfObjectPtr(0, item);
    ECode ec = mMethod->Invoke(mRealOwner->Probe(EIID_IInterface), argumentList);
    if (FAILED(ec)) return E_RUNTIME_EXCEPTION;
    return NOERROR;
}


//=============================================================================
// MenuInflater::MenuState
//=============================================================================
const Int32 MenuInflater::MenuState::sDefaultGroupId = MenuInflater::NO_ID;
const Int32 MenuInflater::MenuState::sDefaultItemId = MenuInflater::NO_ID;
const Int32 MenuInflater::MenuState::sDefaultItemCategory = 0;
const Int32 MenuInflater::MenuState::sDefaultItemOrder = 0;
const Int32 MenuInflater::MenuState::sDefaultItemCheckable = 0;
const Boolean MenuInflater::MenuState::sDefaultItemChecked = FALSE;
const Boolean MenuInflater::MenuState::sDefaultItemVisible = TRUE;
const Boolean MenuInflater::MenuState::sDefaultItemEnabled = TRUE;

MenuInflater::MenuState::MenuState(
    /* [in] */ IMenu* menu,
    /* [in] */ MenuInflater* owner)
    : mMenu(menu)
    , mGroupId(0)
    , mGroupCategory(0)
    , mGroupOrder(0)
    , mGroupCheckable(0)
    , mGroupVisible(FALSE)
    , mGroupEnabled(FALSE)
    , mItemAdded(FALSE)
    , mItemId(0)
    , mItemCategoryOrder(0)
    , mItemIconResId(0)
    , mItemAlphabeticShortcut(0)
    , mItemNumericShortcut(0)
    , mItemCheckable(0)
    , mItemChecked(FALSE)
    , mItemVisible(FALSE)
    , mItemEnabled(FALSE)
    , mItemShowAsAction(0)
    , mItemActionViewLayout(0)
    , mOwner(owner)
{
    ResetGroup();
}

void MenuInflater::MenuState::ResetGroup()
{
    mGroupId = sDefaultGroupId;
    mGroupCategory = sDefaultItemCategory;
    mGroupOrder = sDefaultItemOrder;
    mGroupCheckable = sDefaultItemCheckable;
    mGroupVisible = sDefaultItemVisible;
    mGroupEnabled = sDefaultItemEnabled;
}

void MenuInflater::MenuState::ReadGroup(
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32 *>(R::styleable::MenuGroup),
        ArraySize(R::styleable::MenuGroup));
    AutoPtr<ITypedArray> a;
    mOwner->mContext->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);

    a->GetResourceId(R::styleable::MenuGroup_id, sDefaultGroupId, &mGroupId);
    a->GetInt32(R::styleable::MenuGroup_menuCategory, sDefaultItemCategory, &mGroupCategory);
    a->GetInt32(R::styleable::MenuGroup_orderInCategory, sDefaultItemOrder, &mGroupOrder);
    a->GetInt32(R::styleable::MenuGroup_checkableBehavior, sDefaultItemCheckable, &mGroupCheckable);
    a->GetBoolean(R::styleable::MenuGroup_visible, sDefaultItemVisible, &mGroupVisible);
    a->GetBoolean(R::styleable::MenuGroup_enabled, sDefaultItemEnabled, &mGroupEnabled);

    a->Recycle();
}

void MenuInflater::MenuState::ReadItem(
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32 *>(R::styleable::MenuItem),
        ArraySize(R::styleable::MenuItem));
    AutoPtr<ITypedArray> a;
    mOwner->mContext->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);

    // Inherit attributes from the group as default value
    a->GetResourceId(R::styleable::MenuItem_id, sDefaultItemId, &mItemId);
    Int32 category = 0;
    a->GetInt32(R::styleable::MenuItem_menuCategory, mGroupCategory, &category);
    Int32 order = 0;
    a->GetInt32(R::styleable::MenuItem_orderInCategory, mGroupOrder, &order);
    mItemCategoryOrder = (category & IMenu::CATEGORY_MASK) | (order & IMenu::USER_MASK);
    mItemTitle = NULL;
    a->GetText(R::styleable::MenuItem_title, (ICharSequence**)&mItemTitle);
    mItemTitleCondensed = NULL;
    a->GetText(R::styleable::MenuItem_titleCondensed, (ICharSequence**)&mItemTitleCondensed);
    a->GetResourceId(R::styleable::MenuItem_icon, 0, &mItemIconResId);
    String alphabeticShortcut, numericShortcut;
    a->GetString(R::styleable::MenuItem_alphabeticShortcut, &alphabeticShortcut);
    a->GetString(R::styleable::MenuItem_numericShortcut, &numericShortcut);
    mItemAlphabeticShortcut = GetShortcut(alphabeticShortcut);
    mItemNumericShortcut = GetShortcut(numericShortcut);

    Boolean hasValue = FALSE;
    a->HasValue(R::styleable::MenuItem_checkable, &hasValue);
    if (hasValue) {
        // Item has attribute checkable, use it
        Boolean temp = FALSE;
        a->GetBoolean(R::styleable::MenuItem_checkable, FALSE, &temp);
        mItemCheckable = temp ? 1 : 0;
    }
    else {
        // Item does not have attribute, use the group's (group can have one more state
        // for checkable that represents the exclusive checkable)
        mItemCheckable = mGroupCheckable;
    }
    a->GetBoolean(R::styleable::MenuItem_checked, sDefaultItemChecked, &mItemChecked);
    a->GetBoolean(R::styleable::MenuItem_visible, mGroupVisible, &mItemVisible);
    a->GetBoolean(R::styleable::MenuItem_enabled, mGroupEnabled, &mItemEnabled);
    a->GetInt32(R::styleable::MenuItem_showAsAction, -1, &mItemShowAsAction);
    a->GetString(R::styleable::MenuItem_onClick, &mItemListenerMethodName);
    a->GetResourceId(R::styleable::MenuItem_actionLayout, 0, &mItemActionViewLayout);
    a->GetString(R::styleable::MenuItem_actionViewClass, &mItemActionViewClassName);
    a->GetString(R::styleable::MenuItem_actionProviderClass, &mItemActionProviderClassName);

    Boolean hasActionProvider = !mItemActionProviderClassName.IsNull();
    if (hasActionProvider && mItemActionViewLayout == 0 && mItemActionViewClassName.IsNull()) {
        AutoPtr<IInterface> obj;
        NewInstance(mItemActionProviderClassName,
                MenuInflater::ACTION_PROVIDER_CONSTRUCTOR_SIGNATURE,
                mOwner->mActionProviderConstructorArguments,
                (IInterface**)&obj);
        mItemActionProvider = IActionProvider::Probe(obj);
    }
    else {
        if (hasActionProvider) {
            Logger::W(TAG, "Ignoring attribute 'actionProviderClass'. Action view already specified.");
        }
        mItemActionProvider = NULL;
    }

    a->Recycle();

    mItemAdded = FALSE;
}

Char32 MenuInflater::MenuState::GetShortcut(
    /* [in] */ const String& shortcutString)
{
    if (shortcutString.IsNull()) {
        return 0;
    }
    else {
        return shortcutString.GetChar(0);
    }
}

ECode MenuInflater::MenuState::SetItem(
    /* [in] */ IMenuItem* item)
{
    item->SetChecked(mItemChecked);
    item->SetVisible(mItemVisible);
    item->SetEnabled(mItemEnabled);
    item->SetCheckable(mItemCheckable >= 1);
    item->SetTitleCondensed(mItemTitleCondensed);
    item->SetIcon(mItemIconResId);
    item->SetAlphabeticShortcut(mItemAlphabeticShortcut);
    item->SetNumericShortcut(mItemNumericShortcut);

    if (mItemShowAsAction >= 0) {
        item->SetShowAsAction(mItemShowAsAction);
    }

    if (!mItemListenerMethodName.IsNull()) {
        Boolean result = FALSE;
        if ((mOwner->mContext->IsRestricted(&result), result)) {
            // throw new IllegalStateException("The android:onClick attribute cannot "
            //         + "be used within a restricted context");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        AutoPtr<IOnMenuItemClickListener> l = new InflatedOnMenuItemClickListener(
                IObject::Probe(mOwner->GetRealOwner()), mItemListenerMethodName);
        item->SetOnMenuItemClickListener(l);
    }

    if (IMenuItemImpl::Probe(item) != NULL) {
        if (mItemCheckable >= 2) {
            IMenuItemImpl::Probe(item)->SetExclusiveCheckable(TRUE);
        }
    }

    Boolean actionViewSpecified = FALSE;
    if (!mItemActionViewClassName.IsNull()) {
        AutoPtr<IInterface> obj;
        FAIL_RETURN(NewInstance(mItemActionViewClassName,
                ACTION_VIEW_CONSTRUCTOR_SIGNATURE, mOwner->mActionViewConstructorArguments, (IInterface**)&obj));
        AutoPtr<IView> actionView = IView::Probe(obj);
        item->SetActionView(actionView);
        actionViewSpecified = TRUE;
    }
    if (mItemActionViewLayout > 0) {
        if (!actionViewSpecified) {
            item->SetActionView(mItemActionViewLayout);
            actionViewSpecified = TRUE;
        }
        else {
            Logger::W(TAG, "Ignoring attribute 'itemActionViewLayout'. Action view already specified.");
        }
    }
    if (mItemActionProvider != NULL) {
        item->SetActionProvider(mItemActionProvider);
    }
    return NOERROR;
}

ECode MenuInflater::MenuState::AddItem(
    /* [out] */ IMenuItem** result)
{
    VALIDATE_NOT_NULL(result)
    mItemAdded = TRUE;
    AutoPtr<IMenuItem> item;
    mMenu->Add(mGroupId, mItemId, mItemCategoryOrder, mItemTitle, (IMenuItem**)&item);
    SetItem(item);
    *result = item;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode MenuInflater::MenuState::AddSubMenuItem(
    /* [out] */ ISubMenu** subMenu)
{
    VALIDATE_NOT_NULL(subMenu)
    mItemAdded = TRUE;
    FAIL_RETURN(mMenu->AddSubMenu(mGroupId, mItemId, mItemCategoryOrder, mItemTitle, subMenu));
    AutoPtr<IMenuItem> item;
    (*subMenu)->GetItem((IMenuItem**)&item);
    return SetItem(item);
}

Boolean MenuInflater::MenuState::HasAddedItem()
{
    return mItemAdded;
}

ECode MenuInflater::MenuState::NewInstance(
    /* [in] */ const String& className,
    /* [in] */ const String& constructorSignature,
    /* [in] */ ArrayOf<IInterface*>* arguments,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    *object = NULL;
    AutoPtr<IClassLoader> cl;
    FAIL_RETURN(mOwner->mContext->GetClassLoader((IClassLoader**)&cl));
    AutoPtr<IClassInfo> clazz;
    FAIL_RETURN(cl->LoadClass(className, (IClassInfo**)&clazz));
    AutoPtr<IConstructorInfo> constructor;
    FAIL_RETURN(clazz->GetConstructorInfoByParamNames(
            constructorSignature, (IConstructorInfo**)&constructor));
    AutoPtr<IArgumentList> args;
    constructor->CreateArgumentList((IArgumentList**)&args);
    if (arguments != NULL) {
        for (Int32 i = 0; i < arguments->GetLength(); ++i) {
            args->SetInputArgumentOfObjectPtr(i, (*arguments)[i]);
        }
    }
    return constructor->CreateObject(args, object);
}


//=============================================================================
// MenuInflater
//=============================================================================
CAR_INTERFACE_IMPL(MenuInflater, Object, IMenuInflater)

const String MenuInflater::TAG("MenuInflater");
const String MenuInflater::XML_MENU("menu");
const String MenuInflater::XML_GROUP("group");
const String MenuInflater::XML_ITEM("item");
const Int32 MenuInflater::NO_ID = 0;
const String MenuInflater::ACTION_VIEW_CONSTRUCTOR_SIGNATURE("ctx");
const String MenuInflater::ACTION_PROVIDER_CONSTRUCTOR_SIGNATURE("ctx");

MenuInflater::MenuInflater()
{
}

ECode MenuInflater::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    mActionViewConstructorArguments = ArrayOf<IInterface*>::Alloc(1);
    mActionViewConstructorArguments->Set(0, context);
    mActionProviderConstructorArguments = mActionViewConstructorArguments;
    return NOERROR;
}

ECode MenuInflater::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IInterface* realOwner)
{
    mContext = context;
    mRealOwner = realOwner;
    mActionViewConstructorArguments = ArrayOf<IInterface*>::Alloc(1);
    mActionViewConstructorArguments->Set(0, context);
    mActionProviderConstructorArguments = mActionViewConstructorArguments;
    return NOERROR;
}

ECode MenuInflater::Inflate(
    /* [in] */ Int32 menuRes,
    /* [in] */ IMenu* menu)
{
    AutoPtr<IXmlResourceParser> parser;
    //try {
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    resources->GetLayout(menuRes, (IXmlResourceParser**)&parser);
    assert(0);
    /*AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(parser);

    ECode ec = ParseMenu(parser, attrs, menu);
    if (FAILED(ec)) ec = E_INFLATE_EXCEPTION;*/
    //}
    // catch (XmlPullParserException e) {
    //     throw new InflateException("Error inflating menu XML", e);
    // } catch (IOException e) {
    //     throw new InflateException("Error inflating menu XML", e);
    // } finally {
    if (parser != NULL) parser->Close();
    // }
    assert(0);
    return NOERROR;//ec;
}

ECode MenuInflater::ParseMenu(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IMenu* menu)
{
    AutoPtr<MenuState> menuState = new MenuState(menu, this);

    Int32 eventType = 0;
    parser->GetEventType(&eventType);
    String tagName;
    Boolean lookingForEndOfUnknownTag = FALSE;
    String unknownTagName;

    // This loop will skip to the menu start tag
    do {
        if (eventType == IXmlPullParser::START_TAG) {
            parser->GetName(&tagName);
            if (XML_MENU.Equals(tagName)) {
                // Go to next tag
                parser->Next(&eventType);
                break;
            }

            //throw new RuntimeException("Expecting menu, got " + tagName);
            return E_RUNTIME_EXCEPTION;
        }
        parser->Next(&eventType);
    } while (eventType != IXmlPullParser::END_DOCUMENT);

    Boolean reachedEndOfMenu = FALSE;
    while (!reachedEndOfMenu) {
        switch (eventType) {
            case IXmlPullParser::START_TAG:
                if (lookingForEndOfUnknownTag) {
                    break;
                }

                parser->GetName(&tagName);
                if (XML_GROUP.Equals(tagName)) {
                    menuState->ReadGroup(attrs);
                }
                else if (XML_ITEM.Equals(tagName)) {
                    menuState->ReadItem(attrs);
                }
                else if (XML_MENU.Equals(tagName)) {
                    // A menu start tag denotes a submenu for an item
                    AutoPtr<ISubMenu> subMenu;
                    FAIL_RETURN(menuState->AddSubMenuItem((ISubMenu**)&subMenu));
                    RegisterMenu(subMenu, attrs);
                    // Parse the submenu into returned SubMenu
                    ParseMenu(parser, attrs, IMenu::Probe(subMenu));
                }
                else {
                    lookingForEndOfUnknownTag = TRUE;
                    unknownTagName = tagName;
                }
                break;

            case IXmlPullParser::END_TAG:
                parser->GetName(&tagName);
                if (lookingForEndOfUnknownTag && tagName.Equals(unknownTagName)) {
                    lookingForEndOfUnknownTag = FALSE;
                    unknownTagName = NULL;
                }
                else if (XML_GROUP.Equals(tagName)) {
                    menuState->ResetGroup();
                }
                else if (XML_ITEM.Equals(tagName)) {
                    // Add the item if it hasn't been added (if the item was
                    // a submenu, it would have been added already)
                    if (!menuState->HasAddedItem()) {
                        Boolean result = FALSE;
                        if (menuState->mItemActionProvider != NULL &&
                                (menuState->mItemActionProvider->HasSubMenu(&result), result)) {
                            AutoPtr<ISubMenu> subMenu;
                            FAIL_RETURN(menuState->AddSubMenuItem((ISubMenu**)&subMenu));
                            RegisterMenu(subMenu, attrs);
                        }
                        else {
                            AutoPtr<IMenuItem> item;
                            FAIL_RETURN(menuState->AddItem((IMenuItem**)&item));
                            RegisterMenu(item, attrs);
                        }
                    }
                }
                else if (XML_MENU.Equals(tagName)) {
                    reachedEndOfMenu = TRUE;
                }
                break;

            case IXmlPullParser::END_DOCUMENT:
                //throw new RuntimeException("Unexpected end of document");
                return E_RUNTIME_EXCEPTION;
        }

        parser->Next(&eventType);
    }
    return NOERROR;
}

/**
 * The method is a hook for layoutlib to do its magic.
 * Nothing is needed outside of LayoutLib. However, it should not be deleted because it
 * appears to do nothing.
 * @SuppressWarnings("unused")
 */
void MenuInflater::RegisterMenu(
    /* [in] */ IMenuItem* item,
    /* [in] */ IAttributeSet* set)
{
}

/**
 * The method is a hook for layoutlib to do its magic.
 * Nothing is needed outside of LayoutLib. However, it should not be deleted because it
 * appears to do nothing.
 * @SuppressWarnings("unused")
 */
void MenuInflater::RegisterMenu(
    /* [in] */ ISubMenu* subMenu,
    /* [in] */ IAttributeSet* set)
{
}

// Needed by layoutlib.
ECode MenuInflater::GetContext(
    /* [out] */ IContext** ctx)
{
    VALIDATE_NOT_NULL(ctx)
    *ctx = mContext;
    REFCOUNT_ADD(*ctx)
    return NOERROR;
}

AutoPtr<IInterface> MenuInflater::GetRealOwner()
{
    if (mRealOwner == NULL) {
        mRealOwner = FindRealOwner(mContext);
    }
    return mRealOwner;
}

AutoPtr<IInterface> MenuInflater::FindRealOwner(
    /* [in] */ IInterface* owner)
{
    if (NULL != IActivity::Probe(owner)) {
        return owner;
    }

    if (NULL != IContextWrapper::Probe(owner)) {
        AutoPtr<IContextWrapper> wrapper = IContextWrapper::Probe(owner);
        AutoPtr<IContext> ctx;
        wrapper->GetBaseContext((IContext**)&ctx);
        return FindRealOwner((IInterface*)ctx->Probe(EIID_IInterface));
    }

    return owner;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
