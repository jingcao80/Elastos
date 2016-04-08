
#include "SkbTemplate.h"
#include "SoftKey.h"

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

SkbTemplate::SkbTemplate(
    /* [in] */ Int32 skbTemplateId)
    : mSkbTemplateId(skbTemplateId)
    , mXMargin(0.f)
    , mYMargin(0.f)
{
}

Int32 SkbTemplate::GetSkbTemplateId()
{
    return mSkbTemplateId;
}

void SkbTemplate::SetBackgrounds(
    /* [in] */ IDrawable* skbBg,
    /* [in] */ IDrawable* balloonBg,
    /* [in] */ IDrawable* popupBg)
{
    mSkbBg = skbBg;
    mBalloonBg = balloonBg;
    mPopupBg = popupBg;
}

AutoPtr<IDrawable> SkbTemplate::GetSkbBackground()
{
    return mSkbBg;
}

AutoPtr<IDrawable> SkbTemplate::GetBalloonBackground()
{
    return mBalloonBg;
}

AutoPtr<IDrawable> SkbTemplate::GetPopupBackground()
{
    return mPopupBg;
}

void SkbTemplate::SetMargins(
    /* [in] */ Float xMargin,
    /* [in] */ Float yMargin)
{
    mXMargin = xMargin;
    mYMargin = yMargin;
}

Float SkbTemplate::GetXMargin()
{
    return mXMargin;
}

Float SkbTemplate::GetYMargin()
{
    return mYMargin;
}

AutoPtr<SoftKeyType> SkbTemplate::CreateKeyType(
    /* [in] */ Int32 id,
    /* [in] */ IDrawable* bg,
    /* [in] */ IDrawable* hlBg)
{
    return new SoftKeyType(id, bg, hlBg);
}

Boolean SkbTemplate::AddKeyType(
    /* [in] */ SoftKeyType* keyType)
{
    assert(keyType != NULL);
    // The newly added item should have the right id.
    if ((Int32)mKeyTypeList.GetSize() != keyType->mKeyTypeId) return FALSE;
    mKeyTypeList.PushBack(keyType);
    return TRUE;
}

AutoPtr<SoftKeyType> SkbTemplate::GetKeyType(
    /* [in] */ Int32 typeId)
{
    if (typeId < 0 || typeId > (Int32)mKeyTypeList.GetSize()) return NULL;
    return mKeyTypeList[typeId];
}

void SkbTemplate::AddDefaultKeyIcons(
    /* [in] */ Int32 keyCode,
    /* [in] */ IDrawable* icon,
    /* [in] */ IDrawable* iconPopup)
{
    if (NULL == icon || NULL == iconPopup) return;

    AutoPtr<KeyIconRecord> iconRecord = new KeyIconRecord();
    iconRecord->mIcon = icon;
    iconRecord->mIconPopup = iconPopup;
    iconRecord->mKeyCode = keyCode;

    List<AutoPtr<KeyIconRecord> >::Iterator ator = mKeyIconRecords.Begin();
    for (; ator != mKeyIconRecords.End(); ++ator) {
        AutoPtr<KeyIconRecord> record = *ator;
        assert(record != NULL);
        if (record->mKeyCode >= keyCode) break;
    }

    mKeyIconRecords.Insert(ator, iconRecord);
}

AutoPtr<IDrawable> SkbTemplate::GetDefaultKeyIcon(
    /* [in] */ Int32 keyCode)
{
    List<AutoPtr<KeyIconRecord> >::Iterator ator = mKeyIconRecords.Begin();
    while(ator != mKeyIconRecords.End()) {
        AutoPtr<KeyIconRecord> iconRecord = *ator;
        assert(iconRecord != NULL);
        if (iconRecord->mKeyCode < keyCode) {
            ++ator;
            continue;
        }
        if (iconRecord->mKeyCode == keyCode) {
            return iconRecord->mIcon;
        }
        return NULL;
    }

    return NULL;
}

AutoPtr<IDrawable> SkbTemplate::GetDefaultKeyIconPopup(
    /* [in] */ Int32 keyCode)
{
    List<AutoPtr<KeyIconRecord> >::Iterator ator = mKeyIconRecords.Begin();
    while(ator != mKeyIconRecords.End()) {
        AutoPtr<KeyIconRecord> iconRecord = *ator;
        assert(iconRecord != NULL);
        if (iconRecord->mKeyCode < keyCode) {
            ++ator;
            continue;
        }
        if (iconRecord->mKeyCode == keyCode) {
            return iconRecord->mIconPopup;
        }
        return NULL;
    }

    return NULL;
}

void SkbTemplate::AddDefaultKey(
    /* [in] */ Int32 keyId,
    /* [in] */ SoftKey* softKey)
{
    if (NULL == softKey) return;

    AutoPtr<KeyRecord> keyRecord = new KeyRecord();
    keyRecord->mKeyId = keyId;
    keyRecord->mSoftKey = softKey;

    List<AutoPtr<KeyRecord> >::Iterator ator = mKeyRecords.Begin();
    while(ator != mKeyRecords.End()) {
        AutoPtr<KeyRecord> record = *ator;
        assert(record != NULL);
        if (record->mKeyId >= keyId) break;
        ++ator;
    }

    mKeyRecords.Insert(ator, keyRecord);
}

AutoPtr<SoftKey> SkbTemplate::GetDefaultKey(
    /* [in] */ Int32 keyId)
{
    List<AutoPtr<KeyRecord> >::Iterator ator = mKeyRecords.Begin();
    while(ator != mKeyRecords.End()) {
        AutoPtr<KeyRecord> keyRecord = *ator;
        if (keyRecord->mKeyId < keyId) {
            ++ator;
            continue;
        }
        if (keyRecord->mKeyId == keyId) {
            return keyRecord->mSoftKey;
        }
        return NULL;
    }

    return NULL;
}

SoftKeyType::SoftKeyType(
    /* [in] */ Int32 id,
    /* [in] */ IDrawable* bg,
    /* [in] */ IDrawable* hlBg)
    : mKeyTypeId(id)
    , mKeyBg(bg)
    , mKeyHlBg(hlBg)
    , mColor(0)
    , mColorHl(0)
    , mColorBalloon(0)
{
}

void SoftKeyType::SetColors(
    /* [in] */ Int32 color,
    /* [in] */ Int32 colorHl,
    /* [in] */ Int32 colorBalloon)
{
    mColor = color;
    mColorHl = colorHl;
    mColorBalloon = colorBalloon;
}

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos
