
#ifndef __ELASTOS_DROID_CONTENT_CCLIPDESCRIPTION_H__
#define __ELASTOS_DROID_CONTENT_CCLIPDESCRIPTION_H__

#include "_Elastos_Droid_Content_CClipDescription.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::IStringBuilder;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CClipDescription)
    , public Object
    , public IClipDescription
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CClipDescription();

    virtual ~CClipDescription();

    /**
     * Return the label for this clip.
     */
    CARAPI GetLabel(
        /* [out] */ ICharSequence** label);

    /**
     * Check whether the clip description contains the given MIME type.
     *
     * @param mimeType The desired MIME type.  May be a pattern.
     * @return Returns true if one of the MIME types in the clip description
     * matches the desired MIME type, else false.
     */
    CARAPI HasMimeType(
        /* [in] */ const String& mimeType,
        /* [out] */ Boolean* hasMimeType);

    /**
     * Filter the clip description MIME types by the given MIME type.  Returns
     * all MIME types in the clip that match the given MIME type.
     *
     * @param mimeType The desired MIME type.  May be a pattern.
     * @return Returns an array of all matching MIME types.  If there are no
     * matching MIME types, null is returned.
     */
    CARAPI FilterMimeTypes(
        /* [in] */ const String& mimeType,
        /* [out, callee] */ ArrayOf<String>** mimeTypes);

    /**
     * Return the number of MIME types the clip is available in.
     */
    CARAPI GetMimeTypeCount(
        /* [out] */ Int32* count);

    /**
     * Return one of the possible clip MIME types.
     */
    CARAPI GetMimeType(
        /* [in] */ Int32 index,
        /* [out] */ String* mimeType);

    /** @hide */
    CARAPI Validate();

    CARAPI ToString(
        /* [out] */ String* str);

    /** @hide */
    CARAPI ToShortString(
        /* [in] */ IStringBuilder* builder,
        /* [out] */ Boolean* value);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * Create a new clip.
     *
     * @param label Label to show to the user describing this clip.
     * @param mimeTypes An array of MIME types this data is available as.
     */
    CARAPI constructor(
        /* [in] */ ICharSequence* label,
        /* [in] */ ArrayOf<String>* mimeTypes);

    /**
     * Create a copy of a ClipDescription.
     */
    CARAPI constructor(
        /* [in] */ IClipDescription* o);

    CARAPI constructor();

private:
    CARAPI WriteStringArray(
        /* [in] */ IParcel* dest,
        /* [in] */ ArrayOf<String>* val);

    CARAPI CreateStringArray(
        /* [in] */ IParcel* source,
        /* [out, callee] */ ArrayOf<String>** val);

private:
    AutoPtr<ICharSequence> mLabel;
    AutoPtr<ArrayOf<String> > mMimeTypes;

};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CCLIPDESCRIPTION_H__
