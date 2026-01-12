
/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "CompileCommon.h"

namespace OL
{

STRUCT_RTTI_BEGIN(CodePos)
    RTTI_MEMBER(Pos)
    RTTI_MEMBER(Line)
    RTTI_MEMBER(Col)
STRUCT_RTTI_END(CodePos)

CodePos CodePos::Zero = {0, 0, 0};

STRUCT_RTTI_BEGIN(SourceRange)
    RTTI_STRUCT_MEMBER(Start, CodePos)
    RTTI_STRUCT_MEMBER(End, CodePos)
    RTTI_MEMBER(Flag)
STRUCT_RTTI_END(SourceRange)

SourceRange SourceRange::Zero = {CodePos::Zero, CodePos::Zero};

STRUCT_RTTI_BEGIN(CodeLineInfo)
    RTTI_MEMBER(Line)
    RTTI_MEMBER(Col)
STRUCT_RTTI_END(CodeLineInfo)

CodeLineInfo CodeLineInfo::Zero = {0, 0, 0};

STRUCT_RTTI_BEGIN(UnresolvedBaseTypeInfo)
    RTTI_MEMBER(Name)
    RTTI_MEMBER(Resolved)
STRUCT_RTTI_END(UnresolvedBaseTypeInfo)



}