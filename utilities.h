#pragma once
#include "StdAfx.h" 
//Declaration of createLayer():
Acad::ErrorStatus CreateLayer(const TCHAR* aszLayerName, AcDbObjectId& arLayerId);

//Declaration of createBlockRecord():
Acad::ErrorStatus CreateBlockRecord(const TCHAR* aszBlockTableRecordName);

Acad::ErrorStatus SelectAcDbObject(const ACHAR* aszPrompt, AcDbObjectId& arObjectId);