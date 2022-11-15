#pragma once
#include "Common.h"
#include "Action.h"

namespace OL
{
class BatchAction : public Action
{
public:
    virtual int Run();
   
};
}
