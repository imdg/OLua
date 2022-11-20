/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

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
