#pragma once
#include "StdAfx.h" 
//Declaration of createLayer():
Acad::ErrorStatus createLayer(const TCHAR* layerName, AcDbObjectId& layerId);

//Declaration of createBlockRecord():
Acad::ErrorStatus createBlockRecord(const TCHAR* name);