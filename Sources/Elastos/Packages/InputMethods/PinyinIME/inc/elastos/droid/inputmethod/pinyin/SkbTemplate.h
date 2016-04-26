
#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_SKBTEMPLATE_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_SKBTEMPLATE_H__

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/inputmethod/pinyin/SoftKey.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/Vector.h>

using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Core::Object;
using Elastos::Utility::Etl::Vector;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

/**
 * Key icon definition. It is defined in soft keyboard template. A soft keyboard
 * can refer to such an icon in its xml file directly to improve performance.
 */
class KeyIconRecord: public Object
{
public:
    KeyIconRecord()
        : mKeyCode(0)
    {}

public:
    Int32 mKeyCode;
    AutoPtr<IDrawable> mIcon;
    AutoPtr<IDrawable> mIconPopup;
};


/**
 * Default definition for a certain key. It is defined in soft keyboard
 * template. A soft keyboard can refer to a default key in its xml file. Nothing
 * of the key can be overwritten, including the size.
 */
class KeyRecord: public Object
{
public:
    KeyRecord()
        : mKeyId(0)
    {}

public:
    Int32 mKeyId;
    AutoPtr<SoftKey> mSoftKey;
};

/**
 * Soft keyboard template used by soft keyboards to share common resources. In
 * this way, memory cost is reduced.
 */
class SkbTemplate : public Object
{
public:
    SkbTemplate(
        /* [in] */ Int32 skbTemplateId);

    CARAPI_(Int32) GetSkbTemplateId();

    CARAPI_(void) SetBackgrounds(
        /* [in] */ IDrawable* skbBg,
        /* [in] */ IDrawable* balloonBg,
        /* [in] */ IDrawable* popupBg);

    CARAPI_(AutoPtr<IDrawable>) GetSkbBackground();

    CARAPI_(AutoPtr<IDrawable>) GetBalloonBackground();

    CARAPI_(AutoPtr<IDrawable>) GetPopupBackground();

    CARAPI_(void) SetMargins(
        /* [in] */ Float xMargin,
        /* [in] */ Float yMargin);

    CARAPI_(Float) GetXMargin();

    CARAPI_(Float) GetYMargin();

    CARAPI_(AutoPtr<SoftKeyType>) CreateKeyType(
        /* [in] */ Int32 id,
        /* [in] */ IDrawable* bg,
        /* [in] */ IDrawable* hlBg);

    CARAPI_(Boolean) AddKeyType(
        /* [in] */ SoftKeyType* keyType);

    CARAPI_(AutoPtr<SoftKeyType>) GetKeyType(
        /* [in] */ Int32 typeId);

    CARAPI_(void) AddDefaultKeyIcons(
        /* [in] */ Int32 keyCode,
        /* [in] */ IDrawable* icon,
        /* [in] */ IDrawable* iconPopup);

    CARAPI_(AutoPtr<IDrawable>) GetDefaultKeyIcon(
        /* [in] */ Int32 keyCode);

    CARAPI_(AutoPtr<IDrawable>) GetDefaultKeyIconPopup(
        /* [in] */ Int32 keyCode);

    CARAPI_(void) AddDefaultKey(
        /* [in] */ Int32 keyId,
        /* [in] */ SoftKey* softKey);

    CARAPI_(AutoPtr<SoftKey>) GetDefaultKey(
        /* [in] */ Int32 keyId);

private:
    Int32 mSkbTemplateId;
    AutoPtr<IDrawable> mSkbBg;
    AutoPtr<IDrawable> mBalloonBg;
    AutoPtr<IDrawable> mPopupBg;
    Float mXMargin;
    Float mYMargin;
    /** Key type list. */
    Vector< AutoPtr<SoftKeyType> > mKeyTypeList;

    /**
     * Default key icon list. It is only for keys which do not have popup icons.
     */
    Vector< AutoPtr<KeyIconRecord> > mKeyIconRecords;

    /**
     * Default key list.
     */
    Vector< AutoPtr<KeyRecord> > mKeyRecords;
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos


#endif  //__ELASTOS_DROID_INPUTMETHOD_PINYIN_SKBTEMPLATE_H__
