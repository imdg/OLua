#pragma once
#include "Rtti.h"

namespace OL
{

enum LineFlag
{
    LF_Break = 1
};
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