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

#ifndef __ELASTOS_APPS_CONTACTS_COMMON_COLLAPSER_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_COLLAPSER_H__

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {

/**
 * Class used for collapsing data items into groups of similar items. The data items that should be
 * collapsible should implement the Collapsible interface. The class also contains a utility
 * function that takes an ArrayList of items and returns a list of the same items collapsed into
 * groups.
 */
class Collapser
    : public Object
{
public:
    /**
     * Collapses a list of Collapsible items into a list of collapsed items. Items are collapsed
     * if {@link Collapsible#shouldCollapseWith(Object)} returns true, and are collapsed
     * through the {@Link Collapsible#collapseWith(Object)} function implemented by the data item.
     *
     * @param list List of Objects of type <T extends Collapsible<T>> to be collapsed.
     */
    static CARAPI_(void) CollapseList(
        /* [in] */ IList* list,
        /* [in] */ IContext* context);

private:
    Collapser();

private:
    /*
     * The Collapser uses an n^2 algorithm so we don't want it to run on
     * lists beyond a certain size. This specifies the maximum size to collapse.
     */
    static const Int32 MAX_LISTSIZE_TO_COLLAPSE; // = 20;

};

} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_COLLAPSER_H__
