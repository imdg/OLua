/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Common.h"
#include "VisitorBase.h"


namespace OL
{

class CompleteAst : public RecursiveVisitor
{
public:
    CompleteAst();
    virtual EVisitStatus Visit(SPtr<ABase> Node) override;
    virtual EVisitStatus BeginVisit(SPtr<ABase> Node) override;
    virtual EVisitStatus EndVisit(SPtr<ABase> Node) override;
    OLList<SPtr<ABase>> ParentStack;
};



}