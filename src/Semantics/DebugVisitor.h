/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Common.h"
#include "VisitorBase.h"
#include "OLFunc.h"
namespace OL
{



class DebugVisitor : public VisitorWrapper
{
public:
    DebugVisitor(RecursiveVisitor* InWrappedVisitor, bool InEnableBreak);

    virtual void OnVisit(SPtr<ABase> Node);
    virtual void OnBeginVisit(SPtr<ABase> Node);
    virtual void OnEndVisit(SPtr<ABase> Node);

    
    virtual void OnVisitFinish(SPtr<ABase> Node, EVisitStatus Status);
    virtual void OnBeginVisitFinish(SPtr<ABase> Node, EVisitStatus Status);
    virtual void OnEndVisitFinish(SPtr<ABase> Node, EVisitStatus Status);

    bool EnableBreak;

    OLList<OLFunc<void(SPtr<ABase>)> > OnVisitCallbacks;
    OLList<OLFunc<void(SPtr<ABase>)> > OnBeginVisitCallbacks;
    OLList<OLFunc<void(SPtr<ABase>)> > OnEndVisitCallbacks;

    OLList<OLFunc<void(SPtr<ABase>, EVisitStatus)> > OnVisitFinishCallbacks;
    OLList<OLFunc<void(SPtr<ABase>, EVisitStatus)> > OnBeginVisitFinishCallbacks;
    OLList<OLFunc<void(SPtr<ABase>, EVisitStatus)> > OnEndVisitFinishCallbacks;
    
    };

}