
#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CINDENTINGPRINTWRITER_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CINDENTINGPRINTWRITER_H__

#include "_Elastos_Droid_Internal_Utility_CIndentingPrintWriter.h"
#include <elastos/io/PrintWriter.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::IO::IWriter;
using Elastos::IO::PrintWriter;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CarClass(CIndentingPrintWriter)
    , public PrintWriter
    , public IIndentingPrintWriter
{
public:
    CIndentingPrintWriter();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IWriter* writer,
        /* [in] */ const String& indent);

    CARAPI constructor(
        /* [in] */ IWriter* writer,
        /* [in] */ const String& indent,
        /* [in] */ Int32 wrapLength);

    CARAPI IncreaseIndent();

    CARAPI DecreaseIndent();

    CARAPI PrintPair(
        /* [in] */ const String& key,
        /* [in] */ IInterface* value);

    CARAPI PrintHexPair(
        /* [in] */ const String& key,
        /* [in] */ Int32 value);

    CARAPI Write(
        /* [in] */ ArrayOf<Char32>* buf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

private:
    CARAPI MaybeWriteIndent();

private:
    String mSingleIndent;
    Int32 mWrapLength;

    /** Mutable version of current indent */
    StringBuilder mIndentBuilder;
    /** Cache of current {@link #mIndentBuilder} value */
    AutoPtr<ArrayOf<Char32> > mCurrentIndent;
    /** Length of current line being built, excluding any indent */
    Int32 mCurrentLength;
    /**
     * Flag indicating if we're currently sitting on an empty line, and that
     * next write should be prefixed with the current indent.
     */
    Boolean mEmptyLine;
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CINDENTINGPRINTWRITER_H__
