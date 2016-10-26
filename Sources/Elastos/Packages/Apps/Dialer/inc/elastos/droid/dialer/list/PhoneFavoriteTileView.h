#ifndef __ELASTOS_DROID_DIALER_LIST_PHONEFAVORITETITLEVIEW_H__
#define __ELASTOS_DROID_DIALER_LIST_PHONEFAVORITETITLEVIEW_H__

#include "elastos/droid/contacts/common/list/ContactTileView.h"

using Elastos::Droid::Contacts::Common::IContactPhotoManagerDefaultImageProvider;
using Elastos::Droid::Contacts::Common::List::ContactTileView;
using Elastos::Droid::Content::IClipData;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnLongClickListener;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

class PhoneFavoriteListView;

/**
 * A light version of the {@link com.android.contacts.common.list.ContactTileView} that is used in
 * Dialtacts for frequently called contacts. Slightly different behavior from superclass when you
 * tap it, you want to call the frequently-called number for the contact, even if that is not the
 * default number for that contact. This abstract class is the super class to both the row and tile
 * view.
 */
class PhoneFavoriteTileView
    : public ContactTileView
    , public IPhoneFavoriteTileView
{
private:
    class MyLongClickListener
        : public Object
        , public IViewOnLongClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyLongClickListener(
            /* [in] */ PhoneFavoriteTileView* host);

        // @Override
        CARAPI OnLongClick(
            /* [in] */ IView* v,
            /* [out] */ Boolean* result);

    private:
        PhoneFavoriteTileView* mHost;
    };

    class MyClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyClickListener(
            /* [in] */ PhoneFavoriteTileView* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        PhoneFavoriteTileView* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    // @Override
    CARAPI LoadFromContact(
        /* [in] */ IInterface* entry);

protected:
    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI_(Boolean) IsDarkTheme();

    // @Override
    CARAPI_(AutoPtr<IViewOnClickListener>) CreateClickListener();

    // @Override
    CARAPI_(AutoPtr<IContactPhotoManagerDefaultImageProvider>)  GetDefaultImageRequest(
        /* [in] */ const String& displayName,
        /* [in] */ const String& lookupKey);

    // @Override
    CARAPI_(void) ConfigureViewForImage(
        /* [in] */ Boolean isDefaultImage);

    // @Override
    CARAPI_(Boolean) IsContactPhotoCircular();

protected:
    // Constant to pass to the drag event so that the drag action only happens when a phone favorite
    // tile is long pressed.
    static const String DRAG_PHONE_FAVORITE_TILE;

private:
    static const String TAG;
    static const Boolean DEBUG = FALSE;

    // These parameters instruct the photo manager to display the default image/letter at 70% of
    // its normal size, and vertically offset upwards 12% towards the top of the letter tile, to
    // make room for the contact name and number label at the bottom of the image.
    static const Float DEFAULT_IMAGE_LETTER_OFFSET = -0.12f;
    static const Float DEFAULT_IMAGE_LETTER_SCALE = 0.70f;

    /** View that contains the transparent shadow that is overlaid on top of the contact image. */
    AutoPtr<IView> mShadowOverlay;

    /** Users' most frequent phone number. */
    String mPhoneNumberString;

    // Dummy clip data object that is attached to drag shadows so that text views
    // don't crash with an NPE if the drag shadow is released in their bounds
    static const AutoPtr<IClipData> EMPTY_CLIP_DATA;

    friend class MyClickListener;
    friend class PhoneFavoriteListView;
};

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_PHONEFAVORITETITLEVIEW_H__
