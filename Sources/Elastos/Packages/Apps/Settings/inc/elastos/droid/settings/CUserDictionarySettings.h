#ifndef __ELASTOS_DROID_SETTINGS_CUSERDICTIONARYSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_CUSERDICTIONARYSETTINGS_H__

#include "_Elastos_Droid_Settings_CUserDictionarySettings.h"
#include "elastos/droid/app/ListFragment.h"
#include "elastos/droid/widget/SimpleCursorAdapter.h"

using Elastos::Droid::App::ListFragment;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::SimpleCursorAdapter;
using Elastos::Droid::Widget::IAlphabetIndexer;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ISectionIndexer;
using Elastos::Droid::Widget::ISimpleCursorAdapterViewBinder;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CUserDictionarySettings)
    , public ListFragment
{
private:
    class MyAdapter
        : public SimpleCursorAdapter
        , public ISectionIndexer
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("CUserDictionarySettings::MyAdapter")

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ Int32 layout,
            /* [in] */ ICursor* c,
            /* [in] */ ArrayOf<String>* from,
            /* [in] */ ArrayOf<Int32>* to,
            /* [in] */ CUserDictionarySettings* host);

        ~MyAdapter();

        //@Override
        CARAPI GetPositionForSection(
            /* [in] */ Int32 section,
            /* [out] */ Int32* result);

        //@Override
        CARAPI GetSectionForPosition(
            /* [in] */ Int32 position,
            /* [out] */ Int32* result);

        //@Override
        CARAPI GetSections(
            /* [out, callee] */ ArrayOf<IInterface*>** result);

    private:
        AutoPtr<IAlphabetIndexer> mIndexer;

        AutoPtr<ISimpleCursorAdapterViewBinder> mViewBinder;

        CUserDictionarySettings* mHost;
    };

    class ViewBinder
        : public Object
        , public ISimpleCursorAdapterViewBinder
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("CUserDictionarySettings::ViewBinder")

        ViewBinder();

        ~ViewBinder();

        //@Override
        CARAPI SetViewValue(
            /* [in] */ IView* v,
            /* [in] */ ICursor* c,
            /* [in] */ Int32 columnIndex,
            /* [out] */ Boolean* res);
    };

public:
    CAR_OBJECT_DECL()

    CUserDictionarySettings();

    ~CUserDictionarySettings();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** result);

    //@Override
    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnListItemClick(
        /* [in] */ IListView* l,
        /* [in] */ IView* v,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    //@Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [in] */ IMenuInflater* inflater);

    //@Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    static CARAPI_(void) DeleteWord(
        /* [in] */ const String& word,
        /* [in] */ const String& shortcut,
        /* [in] */ IContentResolver* resolver);

private:
    CARAPI_(AutoPtr<ICursor>) CreateCursor(
        /* [in] */ const String& locale);

    CARAPI_(AutoPtr<IListAdapter>) CreateAdapter();

    /**
     * Add or edit a word. If editingWord is NULL, it's an add; otherwise, it's an edit.
     * @param editingWord the word to edit, or NULL if it's an add.
     * @param editingShortcut the shortcut for this entry, or NULL if none.
     */
    CARAPI_(void) ShowAddOrEditDialog(
        /* [in] */ const String& editingWord,
        /* [in] */ const String& editingShortcut);

    CARAPI_(String) GetWord(
        /* [in] */ Int32 position);

    CARAPI_(String) GetShortcut(
        /* [in] */ Int32 position);

protected:
    String mLocale;

private:
    static const String TAG;

    static const AutoPtr< ArrayOf<String> > QUERY_PROJECTION;

    // The index of the shortcut in the above array.

    static const Int32 INDEX_SHORTCUT;

    // Either the locale is empty (means the word is applicable to all locales)
    // or the word equals our current locale

    static const String QUERY_SELECTION;

    static const String QUERY_SELECTION_ALL_LOCALES;

    static const String DELETE_SELECTION_WITH_SHORTCUT;

    static const String DELETE_SELECTION_WITHOUT_SHORTCUT;

    static const Int32 OPTIONS_MENU_ADD;

    AutoPtr<ICursor> mCursor;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CUSERDICTIONARYSETTINGS_H__