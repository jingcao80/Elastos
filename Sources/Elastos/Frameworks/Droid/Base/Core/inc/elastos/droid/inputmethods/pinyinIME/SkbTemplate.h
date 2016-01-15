
#ifndef  __ELASTOS_DROID_INPUTMETHODS_PINYINIME_SKBTEMPLATE_H__
#define  __ELASTOS_DROID_INPUTMETHODS_PINYINIME_SKBTEMPLATE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Droid::Graphics::Drawable::IDrawable;

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

class SoftKey;

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


class SoftKeyType;

/**
 * Soft keyboard template used by soft keyboards to share common resources. In
 * this way, memory cost is reduced.
 */
class SkbTemplate : public Object
{
public:
    SkbTemplate(
        /* [in] */ Int32 skbTemplateId);

    Int32 GetSkbTemplateId();

    void SetBackgrounds(
        /* [in] */ IDrawable* skbBg,
        /* [in] */ IDrawable* balloonBg,
        /* [in] */ IDrawable* popupBg);

    AutoPtr<IDrawable> GetSkbBackground();

    AutoPtr<IDrawable> GetBalloonBackground();

    AutoPtr<IDrawable> GetPopupBackground();

    void SetMargins(
        /* [in] */ Float xMargin,
        /* [in] */ Float yMargin);

    Float GetXMargin();

    Float GetYMargin();

    AutoPtr<SoftKeyType> CreateKeyType(
        /* [in] */ Int32 id,
        /* [in] */ IDrawable* bg,
        /* [in] */ IDrawable* hlBg);

    Boolean AddKeyType(
        /* [in] */ SoftKeyType* keyType);

    AutoPtr<SoftKeyType> GetKeyType(
        /* [in] */ Int32 typeId);

    void AddDefaultKeyIcons(
        /* [in] */ Int32 keyCode,
        /* [in] */ IDrawable* icon,
        /* [in] */ IDrawable* iconPopup);

    AutoPtr<IDrawable> GetDefaultKeyIcon(
        /* [in] */ Int32 keyCode);

    AutoPtr<IDrawable> GetDefaultKeyIconPopup(
        /* [in] */ Int32 keyCode);

    void AddDefaultKey(
        /* [in] */ Int32 keyId,
        /* [in] */ SoftKey* softKey);

    AutoPtr<SoftKey> GetDefaultKey(
        /* [in] */ Int32 keyId);

private:
    Int32 mSkbTemplateId;
    AutoPtr<IDrawable> mSkbBg;
    AutoPtr<IDrawable> mBalloonBg;
    AutoPtr<IDrawable> mPopupBg;
    Float mXMargin;
    Float mYMargin;
    /** Key type list. */
    List<AutoPtr<SoftKeyType> > mKeyTypeList;// = new Vector<SoftKeyType>();

    /**
     * Default key icon list. It is only for keys which do not have popup icons.
     */
    List<AutoPtr<KeyIconRecord> > mKeyIconRecords;// = new Vector<KeyIconRecord>();

    /**
     * Default key list.
     */
    List<AutoPtr<KeyRecord> > mKeyRecords;// = new Vector<KeyRecord>();
};


class SoftKeyType : public Object
{
public:
    SoftKeyType(
        /* [in] */ Int32 id,
        /* [in] */ IDrawable* bg,
        /* [in] */ IDrawable* hlBg);

    void SetColors(
        /* [in] */ Int32 color,
        /* [in] */ Int32 colorHl,
        /* [in] */ Int32 colorBalloon);

public:
    static const Int32 KEYTYPE_ID_NORMAL_KEY = 0;

    Int32 mKeyTypeId;
    AutoPtr<IDrawable> mKeyBg;
    AutoPtr<IDrawable> mKeyHlBg;
    Int32 mColor;
    Int32 mColorHl;
    Int32 mColorBalloon;
};


} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos


#endif  //__ELASTOS_DROID_INPUTMETHODS_PINYINIME_SKBTEMPLATE_H__
