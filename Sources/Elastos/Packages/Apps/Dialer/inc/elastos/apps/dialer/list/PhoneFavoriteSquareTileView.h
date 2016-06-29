#ifndef __ELASTOS_APPS_DIALER_LIST_PHONEFAVORITESQUARETITLEVIEW_H__
#define __ELASTOS_APPS_DIALER_LIST_PHONEFAVORITESQUARETITLEVIEW_H__

#include "_Elastos.Apps.Dialer.h"
#include "elastos/apps/dialer/list/PhoneFavoriteTileView.h"
#include <elastos/core/Object.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IView;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::IImageButton;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace List {

/**
 * Displays the contact's picture overlaid with their name and number type in a tile.
 */
class PhoneFavoriteSquareTileView
    : public PhoneFavoriteTileView
    , public IPhoneFavoriteSquareTileView
{
private:
    class SecondaryButtonOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        SecondaryButtonOnClickListener(
            /* [in] */ PhoneFavoriteSquareTileView* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        PhoneFavoriteSquareTileView* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    // TODO:
    // @Override
    // CARAPI LoadFromContact(
    //     /* [in] */ IContactEntry* entry);

    // CARAPI GetContactEntry(
    //     /* [out] */ IContactEntry** entry);

protected:
    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI GetApproximateImageSize(
        /* [out] */ Int32* size);

    // @Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);
private:
    CARAPI_(void) LaunchQuickContact();

private:
    static const String TAG; // = PhoneFavoriteSquareTileView.class.getSimpleName();

    Float mHeightToWidthRatio;

    AutoPtr<IImageButton> mSecondaryButton;

    // AutoPtr<IContactEntry> mContactEntry;
};

} // List
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_LIST_PHONEFAVORITESQUARETITLEVIEW_H__
