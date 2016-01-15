
#ifndef __ELASTOS_DROID_CONTENT_RES_STRINGBLOCK_H__
#define __ELASTOS_DROID_CONTENT_RES_STRINGBLOCK_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Graphics::IPaintFontMetricsInt;
using Elastos::Droid::Text::ITextPaint;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::Style::ILineHeightSpanWithDensity;
using Elastos::Droid::Text::Style::ICharacterStyle;

using Elastos::Core::ICharSequence;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

/**
 * Conveniences for retrieving data out of a compiled string resource.
 *
 * {@hide}
 */
class StringBlock : public Object
{
public:
    class StyleIDs : public Object
    {
    public:
        StyleIDs();

    public:
        Int32 boldId;
        Int32 italicId;
        Int32 underlineId;
        Int32 ttId;
        Int32 bigId;
        Int32 smallId;
        Int32 subId;
        Int32 supId;
        Int32 strikeId;
        Int32 listItemId;
        Int32 marqueeId;
    };

    class Height
        : public Object
        , public ILineHeightSpanWithDensity
    {
    public:
        CAR_INTERFACE_DECL()

        Height(
            /* [in] */ Int32 size);

        CARAPI ChooseHeight(
            /* [in] */ ICharSequence* text,
            /* [in] */ Int32 start,
            /* [in] */ Int32 end,
            /* [in] */ Int32 spanstartv,
            /* [in] */ Int32 v,
            /* [in] */ IPaintFontMetricsInt* fm);

        CARAPI ChooseHeight(
            /* [in] */ ICharSequence* text,
            /* [in] */ Int32 start,
            /* [in] */ Int32 end,
            /* [in] */ Int32 spanstartv,
            /* [in] */ Int32 v,
            /* [in] */ IPaintFontMetricsInt* fm,
            /* [in] */ ITextPaint* paint);
    private:
        Int32 mSize;
        static Float sProportion;
    };

public:
    StringBlock(
        /* [in] */ const ArrayOf<Byte> & data,
        /* [in] */ Boolean useSparse);

    StringBlock(
        /* [in] */ const ArrayOf<Byte> & data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 size,
        /* [in] */ Boolean useSparse);

    /**
     * Create from an existing string block native object.  This is
     * -extremely- dangerous -- only use it if you absolutely know what you
     *  are doing!  The given native object must exist for the entire lifetime
     *  of this newly creating StringBlock.
     */
    StringBlock(
        /* [in] */ Handle64 obj,
        /* [in] */ Boolean useSparse);

    virtual ~StringBlock();

    CARAPI_(AutoPtr<ICharSequence>) Get(
        /* [in] */ Int32 idx);

private:
    CARAPI_(AutoPtr<ICharSequence>) ApplyStyles(
            /* [in] */ const String& str,
            /* [in] */ ArrayOf<Int32>* style,
            /* [in] */ StyleIDs* ids);

    /**
     * If a translator has messed up the edges of paragraph-level markup,
     * fix it to actually cover the entire paragraph that it is attached to
     * instead of just whatever range they put it on.
     */
    static CARAPI_(void) AddParagraphSpan(
            /* [in] */ ISpannable* buffer,
            /* [in] */ IInterface* what,
            /* [in] */ Int32 start,
            /* [in] */ Int32 end);

    static CARAPI_(String) Subtag(
            /* [in] */ const String& full,
            /* [in] */ const String& attribute);

    static CARAPI_(Handle64) NativeCreate(
        /* [in] */ const ArrayOf<Byte>& data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 size);

    static CARAPI_(Int32) NativeGetSize(
        /* [in] */ Handle64 obj);

    static CARAPI_(String) NativeGetString(
        /* [in] */ Handle64 obj,
        /* [in] */ Int32 idx);

    static CARAPI_(AutoPtr< ArrayOf<Int32> >) NativeGetStyle(
        /* [in] */ Handle64 obj,
        /* [in] */ Int32 idx);

    static CARAPI_(void) NativeDestroy(
        /* [in] */ Handle64 obj);

    /**
     * Returns a span for the specified color string representation.
     * If the specified string does not represent a color (null, empty, etc.)
     * the color black is returned instead.
     *
     * @param color The color as a string. Can be a resource reference,
     *              HTML hexadecimal, octal or a name
     * @param foreground True if the color will be used as the foreground color,
     *                   false otherwise
     *
     * @return A CharacterStyle
     *
     * @see Color#getHtmlColor(String)
     */
    static CARAPI_(AutoPtr<ICharacterStyle>) GetColor(
        /* [in] */ const String& color,
        /* [in] */ Boolean foreground);

private:
    static const String TAG;
    static const Boolean sLocalLOGV;

    Handle64 mNative;
    AutoPtr< ArrayOf<ICharSequence*> > mStrings;
    AutoPtr< HashMap<Int32, AutoPtr<ICharSequence> > > mSparseStrings;
    AutoPtr<StyleIDs> mStyleIDs;
    Object mSyncLock;
    Boolean mUseSparse;
    Boolean mOwnsNative;
};

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Content::Res::StringBlock, IInterface)

#endif // __ELASTOS_DROID_CONTENT_RES_STRINGBLOCK_H__
