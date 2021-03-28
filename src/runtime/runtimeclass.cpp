
#include "runtimeclass.h"

RuntimeClass::RuntimeClass(ClassID classID, ICorProfilerInfo10 *pCorProfilerInfo) :
    m_classID(classID),
    m_pCorProfilerInfo(pCorProfilerInfo)
{

}

String RuntimeClass::Name()
{
    ModuleID modId;
    mdTypeDef classToken;
    ClassID parentClassID;
    ULONG32 nTypeArgs;
    ClassID typeArgs[SHORT_LENGTH];
    HRESULT hr = S_OK;

    if (m_classID == NULL)
    {
        printf("FAIL: Null ClassID passed in\n");
        return WCHAR("");
    }

    hr = m_pCorProfilerInfo->GetClassIDInfo2(m_classID,
                                           &modId,
                                           &classToken,
                                           &parentClassID,
                                           SHORT_LENGTH,
                                           &nTypeArgs,
                                           typeArgs);
    if (CORPROF_E_CLASSID_IS_ARRAY == hr)
    {
        // We have a ClassID of an array.
        return WCHAR("ArrayClass");
    }
    else if (CORPROF_E_CLASSID_IS_COMPOSITE == hr)
    {
        // We have a composite class
        return WCHAR("CompositeClass");
    }
    else if (CORPROF_E_DATAINCOMPLETE == hr)
    {
        // type-loading is not yet complete. Cannot do anything about it.
        return WCHAR("DataIncomplete");
    }
    else if (FAILED(hr))
    {
        printf("FAIL: GetClassIDInfo returned 0x%x for ClassID %x\n", hr, (unsigned int)m_classID);
        return WCHAR("GetClassIDNameFailed");
    }

    COMPtrHolder<IMetaDataImport> pMDImport;
    hr = m_pCorProfilerInfo->GetModuleMetaData(modId,
                                             (ofRead | ofWrite),
                                             IID_IMetaDataImport,
                                             (IUnknown **)&pMDImport );
    if (FAILED(hr))
    {
        printf("FAIL: GetModuleMetaData call failed with hr=0x%x\n", hr);
        return WCHAR("ClassIDLookupFailed");
    }

    WCHAR wName[LONG_LENGTH];
    DWORD dwTypeDefFlags = 0;
    hr = pMDImport->GetTypeDefProps(classToken,
                                         wName,
                                         LONG_LENGTH,
                                         NULL,
                                         &dwTypeDefFlags,
                                         NULL);
    if (FAILED(hr))
    {
        printf("FAIL: GetModuleMetaData call failed with hr=0x%x\n", hr);
        return WCHAR("ClassIDLookupFailed");
    }

    String name = wName;
    if (nTypeArgs > 0)
        name += WCHAR("<");

    for(ULONG32 i = 0; i < nTypeArgs; i++)
    {
        RuntimeClass genericType(typeArgs[i], m_pCorProfilerInfo);
        name += genericType.Name();

        if ((i + 1) != nTypeArgs)
            name += WCHAR(", ");
    }

    if (nTypeArgs > 0)
        name += WCHAR(">");

    return name;
}