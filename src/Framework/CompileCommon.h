/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Rtti.h"

namespace OL
{

enum LineFlag
{
    LF_Break = 1
};

struct CodePos
{
    int Pos;
    int Line;
    int Col;
    static CodePos Zero;
};
DECLEAR_STRUCT_RTTI(CodePos)


struct SourceRange
{
    CodePos Start;
    CodePos End;
    uint Flag;
    static SourceRange Zero;
};
DECLEAR_STRUCT_RTTI(SourceRange)

struct CodeLineInfo
{
    int Line;
    int Col;
    uint Flag;
    static CodeLineInfo Zero;
};
DECLEAR_STRUCT_RTTI(CodeLineInfo)



enum ESymbolResolvePhase
{
    // Local build phase: Do not search external scopes, unresolved symbol is acceptable
    SRP_Local,      

    // Global build phase: Search external scopes, type must resolve, vars can be left
    SRP_GlobalType,

    // Global build phase: Search external scopes, all symbols must be resolved
    SRP_GlobalVar,

    // Standalone file build: No external scopes, but all symbols must be resolved
    SRP_Standalone
};

struct UnresolvedBaseTypeInfo
{
    OLString Name;
    bool Resolved;
};
DECLEAR_STRUCT_RTTI(UnresolvedBaseTypeInfo)


}