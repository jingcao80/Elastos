
#ifndef __ELASTOS_DROID_VIEW_MENUINFLATER_H__
#define __ELASTOS_DROID_VIEW_MENUINFLATER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;
using Org::Xmlpull::V1::IXmlPullParser;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;

namespace Elastos {
namespace Droid {
namespace View {

/**
 * This class is used to instantiate menu XML files into Menu objects.
 * <p>
 * For performance reasons, menu inflation relies heavily on pre-processing of
 * XML files that is done at build time. Therefore, it is not currently possible
 * to use MenuInflater with an XmlPullParser over a plain XML file at runtime;
 * it only works with an XmlPullParser returned from a compiled resource (R.
 * <em>something</em> file.)
 */
class MenuInflater
    : public Object
    , public IMenuInflater
{
private:
    class InflatedOnMenuItemClickListener
        : public Object
        , public IOnMenuItemClickListener
    {
    public:
        InflatedOnMenuItemClickListener(
            /* [in] */ IObject* realOwner,
            /* [in] */ const String& methodName);

        CAR_INTERFACE_DECL()

        CARAPI OnMenuItemClick(
            /* [in] */ IMenuItem* item,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<IObject> mRealOwner;
        AutoPtr<IMethodInfo> mMethod;
    };

    /**
     * State for the current menu.
     * <p>
     * Groups can not be nested unless there is another menu (which will have
     * its state class).
     */
    class MenuState
        : public Object
    {
    public:
        MenuState(
            /* [in] */ IMenu* menu,
            /* [in] */ MenuInflater* owner);

        CARAPI_(void) ResetGroup();

        /**
         * Called when the parser is pointing to a group tag.
         */
        CARAPI_(void) ReadGroup(
            /* [in] */ IAttributeSet* attrs);

        /**
         * Called when the parser is pointing to an item tag.
         */
        CARAPI_(void) ReadItem(
            /* [in] */ IAttributeSet* attrs);

        CARAPI AddItem(
            /* [out] */ IMenuItem** item);

        CARAPI AddSubMenuItem(
            /* [out] */ ISubMenu** subMenu);

        CARAPI_(Boolean) HasAddedItem();

    private:
        CARAPI_(Char32) GetShortcut(
            /* [in] */ const String& shortcutString);

        CARAPI SetItem(
            /* [in, out] */ IMenuItem* item);

        CARAPI NewInstance(
            /* [in] */ const String& className,
            /* [in] */ const String& constructorSignature,
            /* [in] */ ArrayOf<IInterface*>* arguments,
            /* [out] */ IInterface** object);

    private:
        friend class MenuInflater;

        AutoPtr<IMenu> mMenu;

        /*
         * Group state is set on items as they are added, allowing an item to
         * override its group state. (As opposed to set on items at the group end tag.)
         */
        Int32 mGroupId;
        Int32 mGroupCategory;
        Int32 mGroupOrder;
        Int32 mGroupCheckable;
        Boolean mGroupVisible;
        Boolean mGroupEnabled;

        Boolean mItemAdded;
        Int32 mItemId;
        Int32 mItemCategoryOrder;
        AutoPtr<ICharSequence> mItemTitle;
        AutoPtr<ICharSequence> mItemTitleCondensed;
        Int32 mItemIconResId;
        Char32 mItemAlphabeticShortcut;
        Char32 mItemNumericShortcut;
        /**
         * Sync to attrs.xml enum:
         * - 0: none
         * - 1: all
         * - 2: exclusive
         */
        Int32 mItemCheckable;
        Boolean mItemChecked;
        Boolean mItemVisible;
        Boolean mItemEnabled;

        /**
         * Sync to attrs.xml enum, values in MenuItem:
         * - 0: never
         * - 1: ifRoom
         * - 2: always
         * - -1: Safe sentinel for "no value".
         */
        Int32 mItemShowAsAction;

        Int32 mItemActionViewLayout;
        String mItemActionViewClassName;
        String mItemActionProviderClassName;

        String mItemListenerMethodName;

        AutoPtr<IActionProvider> mItemActionProvider;

        static const Int32 sDefaultGroupId;
        static const Int32 sDefaultItemId;
        static const Int32 sDefaultItemCategory;
        static const Int32 sDefaultItemOrder;
        static const Int32 sDefaultItemCheckable;
        static const Boolean sDefaultItemChecked;
        static const Boolean sDefaultItemVisible;
        static const Boolean sDefaultItemEnabled;

        MenuInflater* mOwner;
    };

public:
    MenuInflater();

    CAR_INTERFACE_DECL()

    /**
     * Constructs a menu inflater.
     *
     * @see Activity#getMenuInflater()
     */
    CARAPI constructor (
        /* [in] */ IContext* context);

    /**
     * Constructs a menu inflater.
     *
     * @see Activity#getMenuInflater()
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IInterface* realOwner);

    /**
     * Inflate a menu hierarchy from the specified XML resource. Throws
     * {@link InflateException} if there is an error.
     *
     * @param menuRes Resource ID for an XML layout resource to load (e.g.,
     *            <code>R.menu.main_activity</code>)
     * @param menu The Menu to inflate into. The items and submenus will be
     *            added to this Menu.
     */
    CARAPI Inflate(
        /* [in] */ Int32 menuRes,
        /* [in, out] */ IMenu* menu);

private:
    /**
     * Called internally to fill the given menu. If a sub menu is seen, it will
     * call this recursively.
     */
    CARAPI ParseMenu(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [in, out] */ IMenu* menu);

    CARAPI_(void) RegisterMenu(
        /* [in] */ IMenuItem* item,
        /* [in] */ IAttributeSet* set);

    CARAPI_(void) RegisterMenu(
        /* [in] */ ISubMenu* subMenu,
        /* [in] */ IAttributeSet* set);

    CARAPI GetContext(
        /* [out] */ IContext** ctx);

    CARAPI_(AutoPtr<IInterface>) GetRealOwner();

    CARAPI_(AutoPtr<IInterface>) FindRealOwner(
        /* [in] */ IInterface* owner);

private:
    static const String TAG;

    /** Menu tag name in XML. */
    static const String XML_MENU;

    /** Group tag name in XML. */
    static const String XML_GROUP;

    /** Item tag name in XML. */
    static const String XML_ITEM;

    static const Int32 NO_ID;

    static const String ACTION_VIEW_CONSTRUCTOR_SIGNATURE;

    static const String ACTION_PROVIDER_CONSTRUCTOR_SIGNATURE;

    AutoPtr< ArrayOf<IInterface*> > mActionViewConstructorArguments;

    AutoPtr< ArrayOf<IInterface*> > mActionProviderConstructorArguments;

    AutoPtr<IContext> mContext;
    AutoPtr<IInterface> mRealOwner;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_VIEW_MENUINFLATER_H__
