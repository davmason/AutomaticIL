#pragma once

#include "common.h"

class RuntimeClass
{
private:
    ClassID m_classID;
    COMPtrHolder<ICorProfilerInfo10> m_pCorProfilerInfo;

public:
    RuntimeClass(ClassID classID, ICorProfilerInfo10 *pCorProfilerInfo);
    ~RuntimeClass() = default;

    String Name();
};
