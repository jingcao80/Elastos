//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_INTERNAL_HTTP_MULTIPART_STRINGPART_H__
#define __ELASTOS_DROID_INTERNAL_HTTP_MULTIPART_STRINGPART_H__

#include "elastos/droid/internal/http/multipart/PartBase.h"

// using Org::Apache::Commons::Logging::ILog;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Http {
namespace Multipart {

/**
 * Simple string parameter for a multipart post
 *
 * @author <a href="mailto:mattalbright@yahoo.com">Matthew Albright</a>
 * @author <a href="mailto:jsdever@apache.org">Jeff Dever</a>
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 * @author <a href="mailto:oleg@ural.ru">Oleg Kalnichevski</a>
 *
 * @since 2.0
 */
class StringPart
    : public PartBase
    , public IStringPart
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Constructor.
     *
     * @param name The name of the part
     * @param value the string to post
     * @param charset the charset to be used to encode the string, if <code>null</code>
     * the {@link #DEFAULT_CHARSET default} is used
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& value,
        /* [in] */ const String& charset);

    /**
     * Constructor.
     *
     * @param name The name of the part
     * @param value the string to post
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    /* (non-Javadoc)
     * @see org.apache.commons.httpclient.methods.multipart.BasePart#setCharSet(java.lang.String)
     */
    CARAPI SetCharSet(
        /* [in] */ const String& charSet);

protected:
    /**
     * Writes the data to the given OutputStream.
     * @param out the OutputStream to write to
     * @throws IOException if there is a write error
     */
    CARAPI SendData(
        /* [in] */ IOutputStream* outStream);

    /**
     * Return the length of the data.
     * @return The length of the data.
     * @see Part#lengthOfData()
     */
    CARAPI LengthOfData(
        /* [out] */ Int64* length);

private:
    /**
     * Gets the content in bytes.  Bytes are lazily created to allow the charset to be changed
     * after the part is created.
     *
     * @return the content in bytes
     */
    CARAPI_(AutoPtr<ArrayOf<Byte> >) GetContent();

private:
    /** Log object for this class. */
    // static const AutoPtr<ILog> LOG;

    /** Contents of this StringPart. */
    AutoPtr<ArrayOf<Byte> > mContent;

    /** The String value of this part. */
    String mValue;

};

} // namespace Multipart
} // namespace Http
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_HTTP_MULTIPART_STRINGPART_H__
