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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/text/CSelection.h"
#include "elastos/droid/text/Selection.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL(CSelection, Singleton, ISelection)

CAR_SINGLETON_IMPL(CSelection)

ECode CSelection::GetSelectionStart(
    /* [in] */ ICharSequence* text,
    /* [out] */ Int32* start)
{
    VALIDATE_NOT_NULL(start);

    *start = Selection::GetSelectionStart(text);

    return NOERROR;
}

ECode CSelection::GetSelectionEnd(
    /* [in] */ ICharSequence* text,
    /* [out] */ Int32* end)
{
    VALIDATE_NOT_NULL(text);

    *end = Selection::GetSelectionEnd(text);

    return NOERROR;
}

ECode CSelection::SetSelection(
    /* [in] */ ISpannable* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 stop)
{
    Selection::SetSelection(text, start, stop);

    return NOERROR;
}

ECode CSelection::SetSelection(
    /* [in] */ ISpannable* text,
    /* [in] */ Int32 index)
{
    Selection::SetSelection(text, index);

    return NOERROR;
}

ECode CSelection::SelectAll(
    /* [in] */ ISpannable* text)
{
    Selection::SelectAll(text);

    return NOERROR;
}

ECode CSelection::ExtendSelection(
    /* [in] */ ISpannable* text,
    /* [in] */ Int32 index)
{
    Selection::ExtendSelection(text, index);

    return NOERROR;
}

ECode CSelection::RemoveSelection(
    /* [in] */ ISpannable* text)
{
    Selection::RemoveSelection(text);

    return NOERROR;
}

ECode CSelection::MoveUp(
    /* [in] */ ISpannable* text,
    /* [in] */ ILayout* layout,
    /* [out] */ Boolean* result)
{
    Boolean ret = Selection::MoveUp(text, layout);
    if (result) *result = ret;

    return NOERROR;
}

ECode CSelection::MoveDown(
    /* [in] */ ISpannable* text,
    /* [in] */ ILayout* layout,
    /* [out] */ Boolean* result)
{
    Boolean ret = Selection::MoveDown(text, layout);
    if (result) *result = ret;

    return NOERROR;
}

ECode CSelection::MoveLeft(
    /* [in] */ ISpannable* text,
    /* [in] */ ILayout* layout,
    /* [out] */ Boolean* result)
{
    Boolean ret = Selection::MoveLeft(text, layout);
    if (result) *result = ret;

    return NOERROR;
}

ECode CSelection::MoveRight(
    /* [in] */ ISpannable* text,
    /* [in] */ ILayout* layout,
    /* [out] */ Boolean* result)
{
    Boolean ret = Selection::MoveRight(text, layout);
    if (result) *result = ret;

    return NOERROR;
}

ECode CSelection::ExtendUp(
    /* [in] */ ISpannable* text,
    /* [in] */ ILayout* layout,
    /* [out] */ Boolean* result)
{
    Boolean ret = Selection::ExtendUp(text, layout);
    if (result) *result = ret;

    return NOERROR;
}

ECode CSelection::ExtendDown(
    /* [in] */ ISpannable* text,
    /* [in] */ ILayout* layout,
    /* [out] */ Boolean* result)
{
    Boolean ret = Selection::ExtendDown(text, layout);
    if (result) *result = ret;

    return NOERROR;
}

ECode CSelection::ExtendLeft(
    /* [in] */ ISpannable* text,
    /* [in] */ ILayout* layout,
    /* [out] */ Boolean* result)
{
    Boolean ret = Selection::ExtendLeft(text, layout);
    if (result) *result = ret;

    return NOERROR;
}

ECode CSelection::ExtendRight(
    /* [in] */ ISpannable* text,
    /* [in] */ ILayout* layout,
    /* [out] */ Boolean* result)
{
    Boolean ret = Selection::ExtendRight(text, layout);
    if (result) *result = ret;

    return NOERROR;
}

ECode CSelection::ExtendToLeftEdge(
    /* [in] */ ISpannable* text,
    /* [in] */ ILayout* layout,
    /* [out] */ Boolean* result)
{
    Boolean ret = Selection::ExtendToLeftEdge(text, layout);
    if (result) *result = ret;

    return NOERROR;
}

ECode CSelection::ExtendToRightEdge(
    /* [in] */ ISpannable* text,
    /* [in] */ ILayout* layout,
    /* [out] */ Boolean* result)
{
    Boolean ret = Selection::ExtendToRightEdge(text, layout);
    if (result) *result = ret;

    return NOERROR;
}

ECode CSelection::MoveToLeftEdge(
    /* [in] */ ISpannable* text,
    /* [in] */ ILayout* layout,
    /* [out] */ Boolean* result)
{
    Boolean ret = Selection::MoveToLeftEdge(text, layout);
    if (result) *result = ret;

    return NOERROR;
}

ECode CSelection::MoveToRightEdge(
    /* [in] */ ISpannable* text,
    /* [in] */ ILayout* layout,
    /* [out] */ Boolean* result)
{
    Boolean ret = Selection::MoveToRightEdge(text, layout);
    if (result) *result = ret;

    return NOERROR;
}

/** {@hide} */
ECode CSelection::MoveToPreceding(
    /* [in] */ ISpannable* text,
    /* [in] */ ISelectionPositionIterator* iter,
    /* [in] */ Boolean extendSelection,
    /* [out] */ Boolean* result)
{
    Boolean ret = Selection::MoveToPreceding(text, iter, extendSelection);
    if (result) *result = ret;

    return NOERROR;
}

/** {@hide} */
ECode CSelection::MoveToFollowing(
    /* [in] */ ISpannable* text,
    /* [in] */ ISelectionPositionIterator* iter,
    /* [in] */ Boolean extendSelection,
    /* [out] */ Boolean* result)
{
    Boolean ret = Selection::MoveToFollowing(text, iter, extendSelection);
    if (result) *result = ret;

    return NOERROR;
}

ECode CSelection::GetSelectionStartObject(
    /* [out] */ IInterface** start)
{
    VALIDATE_NOT_NULL(start);

    *start = Selection::SELECTION_START;
    REFCOUNT_ADD(*start);

    return NOERROR;
}

ECode CSelection::GetSelectionEndObject(
    /* [out] */ IInterface** end)
{
    VALIDATE_NOT_NULL(end);

    *end = Selection::SELECTION_END;
    REFCOUNT_ADD(*end);

    return NOERROR;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos