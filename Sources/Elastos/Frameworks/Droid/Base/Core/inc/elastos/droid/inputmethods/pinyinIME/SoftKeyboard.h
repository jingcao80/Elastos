
#ifndef  __ELASTOS_DROID_INPUTMETHODS_PINYINIME_SOFTKEYBOARD_H__
#define  __ELASTOS_DROID_INPUTMETHODS_PINYINIME_SOFTKEYBOARD_H__

#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/utility/etl/List.h>
#include "SoftKey.h"
#include "SkbTemplate.h"

using Elastos::Utility::Etl::List;
using Elastos::Droid::Graphics::IRect;

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

/**
 * Class used to represent a soft keyboard definition, including the height, the
 * background image, the image for high light, the keys, etc.
 */
class SoftKeyboard
    : public Object
    , public ISoftKeyboard
{
public:
    class KeyRow : public ElRefBase
    {
    public:
        KeyRow()
            : mRowId(0)
            , mTopF(0.f)
            , mBottomF(0.f)
            , mTop(0)
            , mBottom(0)
        {}

    public:
        static const Int32 ALWAYS_SHOW_ROW_ID;
        static const Int32 DEFAULT_ROW_ID;

        List<AutoPtr<SoftKey> > mSoftKeys;
        /**
         * If the row id is {@link #ALWAYS_SHOW_ROW_ID}, this row will always be
         * enabled.
         */
        Int32 mRowId;
        Float mTopF;
        Float mBottomF;
        Int32 mTop;
        Int32 mBottom;
    };

public:
    CAR_INTERFACE_DECL();

    SoftKeyboard(
        /* [in] */ Int32 skbXmlId,
        /* [in] */ SkbTemplate* skbTemplate,
        /* [in] */ Int32 skbWidth,
        /* [in] */ Int32 skbHeight);

    void SetFlags(
        /* [in] */ Boolean cacheFlag,
        /* [in] */ Boolean stickyFlag,
        /* [in] */ Boolean isQwerty,
        /* [in] */ Boolean isQwertyUpperCase);

    Boolean GetCacheFlag();

    void SetCacheId(
        /* [in] */ Int32 cacheId);

    Boolean GetStickyFlag();

    void SetSkbBackground(
        /* [in] */ IDrawable* skbBg);

    void SetPopupBackground(
        /* [in] */ IDrawable* popupBg);

    void SetKeyBalloonBackground(
        /* [in] */ IDrawable* balloonBg);

    void SetKeyMargins(
        /* [in] */ Float xMargin,
        /* [in] */ Float yMargin);

    Int32 GetCacheId();

    void Reset();

    void SetNewlyLoadedFlag(
        /* [in] */ Boolean newlyLoadedFlag);

    Boolean GetNewlyLoadedFlag();

    void BeginNewRow(
        /* [in] */ Int32 rowId,
        /* [in] */ Float yStartingPos);

    Boolean AddSoftKey(
        /* [in] */ SoftKey* softKey);

    Int32 GetSkbXmlId();

    // Set the size of the soft keyboard core. In other words, the background's
    // padding are not counted.
    void SetSkbCoreSize(
        /* [in] */ Int32 skbCoreWidth,
        /* [in] */ Int32 skbCoreHeight);

    Int32 GetSkbCoreWidth();

    Int32 GetSkbCoreHeight();

    Int32 GetSkbTotalWidth();

    Int32 GetSkbTotalHeight();

    Int32 GetKeyXMargin();

    Int32 GetKeyYMargin();

    AutoPtr<IDrawable> GetSkbBackground();

    AutoPtr<IDrawable>  GetBalloonBackground();

    AutoPtr<IDrawable> GetPopupBackground();

    Int32 GetRowNum();

    AutoPtr<KeyRow> GetKeyRowForDisplay(
        /* [in] */ Int32 row);

    AutoPtr<SoftKey> GetKey(
        /* [in] */ Int32 row,
        /* [in] */ Int32 location);

    AutoPtr<SoftKey> MapToKey(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    void SwitchQwertyMode(
        /* [in] */ Int32 toggle_state_id,
        /* [in] */ Boolean upperCase);

    void EnableToggleState(
        /* [in] */ Int32 toggleStateId,
        /* [in] */ Boolean resetIfNotFound);

    void DisableToggleState(
        /* [in] */ Int32 toggleStateId,
        /* [in] */ Boolean resetIfNotFound);

    void EnableToggleStates(
        /* [in] */ IToggleStates* toggleStates);

private:
    AutoPtr<IRect> GetPadding();

    /**
     * Enable a row with the give toggle Id. Rows with other toggle ids (except
     * the id {@link KeyRow#ALWAYS_SHOW_ROW_ID}) will be disabled.
     *
     * @param rowId The row id to enable.
     * @return True if the soft keyboard requires redrawing.
     */
    Boolean EnableRow(
        /* [in] */ Int32 rowId);

    // @Override
    // public String toString() {
    //     String str = "------------------SkbInfo----------------------\n";
    //     String endStr = "-----------------------------------------------\n";
    //     str += "Width: " + String.valueOf(mSkbCoreWidth) + "\n";
    //     str += "Height: " + String.valueOf(mSkbCoreHeight) + "\n";
    //     str += "KeyRowNum: " + mKeyRows == NULL ? "0" : String.valueOf(mKeyRows
    //             .size())
    //             + "\n";
    //     if (NULL == mKeyRows) return str + endStr;
    //     Int32 rowNum = mKeyRows.size();
    //     for (Int32 row = 0; row < rowNum; row++) {
    //         KeyRow keyRow = mKeyRows.get(row);
    //         List<SoftKey> softKeys = keyRow.mSoftKeys;
    //         Int32 keyNum = softKeys.size();
    //         for (Int32 i = 0; i < softKeys.size(); i++) {
    //             str += "-key " + String.valueOf(i) + ":"
    //                     + softKeys.get(i).toString();
    //         }
    //     }
    //     return str + endStr;
    // }

    // public String toShortString() {
    //     return super.toString();
    // }

public:
    /**
     * Background of the soft keyboard. If it is NULL, the one in the soft
     * keyboard template will be used.
     **/
    AutoPtr<IDrawable> mSkbBg;

private:
    /** The XML resource id for this soft keyboard. */
    Int32 mSkbXmlId;

    /** Do we need to cache this soft keyboard? */
    Boolean mCacheFlag;

    /**
     * After user switches to this soft keyboard, if this flag is TRUE, this
     * soft keyboard will be kept unless explicit switching operation is
     * performed, otherwise IME will switch back to the previous keyboard layout
     * whenever user clicks on any none-function key.
     **/
    Boolean mStickyFlag;

    /**
     * The cache id for this soft keyboard. It is used to identify it in the
     * soft keyboard pool.
     */
    Int32 mCacheId;

    /**
     * Used to indicate whether this soft keyboard is newly loaded from an XML
     * file or is just gotten from the soft keyboard pool.
     */
    Boolean mNewlyLoadedFlag;

    /** The width of the soft keyboard. */
    Int32 mSkbCoreWidth;

    /** The height of the soft keyboard. */
    Int32 mSkbCoreHeight;

    /** The soft keyboard template for this soft keyboard. */
    AutoPtr<SkbTemplate> mSkbTemplate;

    /** Used to indicate whether this soft keyboard is a QWERTY keyboard. */
    Boolean mIsQwerty;

    /**
     * When {@link #mIsQwerty} is TRUE, this member is Used to indicate that the
     * soft keyboard should be displayed in uppercase.
     */
    Boolean mIsQwertyUpperCase;

    /**
     * The id of the rows which are enabled. Rows with id
     * {@link KeyRow#ALWAYS_SHOW_ROW_ID} are always enabled.
     */
    Int32 mEnabledRowId;

    /**
     * Rows in this soft keyboard. Each row has a id. Only matched rows will be
     * enabled.
     */
    List<AutoPtr<KeyRow> > mKeyRows;

    /**
     * Background for key balloon. If it is NULL, the one in the soft keyboard
     * template will be used.
     **/
    AutoPtr<IDrawable> mBalloonBg;

    /**
     * Background for popup mini soft keyboard. If it is NULL, the one in the
     * soft keyboard template will be used.
     **/
    AutoPtr<IDrawable> mPopupBg;

    /** The left and right margin of a key. */
    Float mKeyXMargin;

    /** The top and bottom margin of a key. */
    Float mKeyYMargin;

    AutoPtr<IRect> mTmpRect;
};

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INPUTMETHODS_PINYINIME_SOFTKEYBOARD_H__
