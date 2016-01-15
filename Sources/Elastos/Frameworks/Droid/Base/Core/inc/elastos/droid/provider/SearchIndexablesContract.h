#ifndef __ELASTOS_DROID_PROVIDER_SEARCHINDEXABLESCONTRACT_H__
#define __ELASTOS_DROID_PROVIDER_SEARCHINDEXABLESCONTRACT_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Provider {

class SearchIndexablesContract
{
public:
    class BaseColumns // private
    {
    private:
        BaseColumns() {}

    public:
        /**
         * Rank of the data. This is an integer used for ranking the search results. This is
         * application specific.
         */
        static const String COLUMN_RANK;

        /**
         * Class name associated with the data (usually a Fragment class name).
         */
        static const String COLUMN_CLASS_NAME;

        /**
         * Icon resource ID for the data.
         */
        static const String COLUMN_ICON_RESID;

        /**
         * Intent action associated with the data.
         */
        static const String COLUMN_INTENT_ACTION;

        /**
         * Intent target package associated with the data.
         */
        static const String COLUMN_INTENT_TARGET_PACKAGE;

        /**
         * Intent target class associated with the data.
         */
        static const String COLUMN_INTENT_TARGET_CLASS;
    };

public:
    class XmlResource
    {
    private:
        XmlResource() {}

    public:
        static const String MIME_TYPE;
    };

    class RawData
    {
    private:
        RawData() {}

    public:
        static const String MIME_TYPE;
    };

    class NonIndexableKey
    {
    private:
        NonIndexableKey() {}

    public:
        static const String MIME_TYPE;
    };

public:
    static const String INDEXABLES_XML_RES_PATH;
    static const String INDEXABLES_RAW_PATH;
    static const String NON_INDEXABLES_KEYS_PATH;
    static const AutoPtr<ArrayOf<String> > INDEXABLES_XML_RES_COLUMNS;
    static const AutoPtr<ArrayOf<String> > INDEXABLES_RAW_COLUMNS;
    static const AutoPtr<ArrayOf<String> > NON_INDEXABLES_KEYS_COLUMNS;
    static const String SETTINGS; // private
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_SEARCHINDEXABLESCONTRACT_H__