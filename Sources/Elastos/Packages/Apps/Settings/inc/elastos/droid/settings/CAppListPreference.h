#ifndef __ELASTOS_DROID_SETTINGS_CAPPLISTPREFERENCE_H__
#define __ELASTOS_DROID_SETTINGS_CAPPLISTPREFERENCE_H__

#include "_Elastos_Droid_Settings_CAppListPreference.h"
#include "elastos/droid/preference/ListPreference.h"
#include "elastos/droid/widget/ArrayAdapter.h"

using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Preference::ListPreference;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::ArrayAdapter;

namespace Elastos {
namespace Droid {
namespace Settings {

/**
 * Extends ListPreference to allow us to show the icons for a given list of applications. We do this
 * because the names of applications are very similar and the user may not be able to determine what
 * app they are selecting without an icon.
 */
CarClass(CAppListPreference)
    , public ListPreference
{
public:
    class AppArrayAdapter
        : public ArrayAdapter
    {
    public:
        TO_STRING_IMPL("CAppListPreference::AppArrayAdapter")

        AppArrayAdapter();

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ Int32 textViewResourceId,
            /* [in] */ ArrayOf<ICharSequence*>* objects,
            /* [in] */ ArrayOf<IDrawable*>* imageDrawables,
            /* [in] */ Int32 selectedIndex);

        //@Override
        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** result);

    private:
        AutoPtr< ArrayOf<IDrawable*> > mImageDrawables;
        Int32 mSelectedIndex;
    };

public:
    CAR_OBJECT_DECL()

    TO_STRING_IMPL("CAppListPreference")

    CAppListPreference();

    ~CAppListPreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetPackageNames(
        /* [in] */ ArrayOf<String>* packageNames,
        /* [in] */ const String& defaultPackageName);

protected:
    //@Override
    CARAPI OnPrepareDialogBuilder(
        /* [in] */ IAlertDialogBuilder* builder);

private:
    AutoPtr< ArrayOf<IDrawable*> > mEntryDrawables;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CAPPLISTPREFERENCE_H__