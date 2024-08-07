// (C) Copyright 2002-2007 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- ADSKEmployeeDetails.cpp : Implementation of ADSKEmployeeDetails
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ADSKEmployeeDetails.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 ADSKEmployeeDetails::kCurrentVersionNumber = 1;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS(
	ADSKEmployeeDetails, AcDbObject,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kNoOperation, ADSKEMPLOYEEDETAILS,
	ADSKEMPLOYEEDETAILSAPP
	| Product Desc : A description for your object
	| Company : Your company name
	| WEB Address : Your company WEB site address
)

//-----------------------------------------------------------------------------
ADSKEmployeeDetails::ADSKEmployeeDetails() : AcDbObject() {
	m_szfirstName = nullptr;
	m_szlastName = nullptr;
}

ADSKEmployeeDetails::~ADSKEmployeeDetails() {
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus ADSKEmployeeDetails::dwgOutFields(AcDbDwgFiler * pFiler) const {
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbObject::dwgOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(ADSKEmployeeDetails::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	pFiler->writeItem(m_szlastName);
	pFiler->writeItem(m_szfirstName);
	pFiler->writeItem(m_nCube);
	pFiler->writeItem(m_nID);

	return (pFiler->filerStatus());
}

Acad::ErrorStatus ADSKEmployeeDetails::dwgInFields(AcDbDwgFiler * pFiler) {
	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbObject::dwgInFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be read first
	Adesk::UInt32 version = 0;
	if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
		return (es);
	if (version > ADSKEmployeeDetails::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < ADSKEmployeeDetails::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	switch (version)
	{
	case (1):
		pFiler->readItem(&m_szlastName);
		pFiler->readItem(&m_szfirstName);
		pFiler->readItem(&m_nCube);
		pFiler->readItem(&m_nID);
		break;
	}

	return (pFiler->filerStatus());
}

//- Dxf Filing protocol
Acad::ErrorStatus ADSKEmployeeDetails::dxfOutFields(AcDbDxfFiler * pFiler) const {
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbObject::dxfOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	es = pFiler->writeItem(AcDb::kDxfSubclass, _RXST("ADSKEmployeeDetails"));
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(kDxfInt32, ADSKEmployeeDetails::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	pFiler->writeItem(AcDb::kDxfXTextString, m_szlastName);
	pFiler->writeItem(AcDb::kDxfXTextString + 1, m_szfirstName);
	pFiler->writeItem(AcDb::kDxfInt32, m_nCube);
	pFiler->writeItem(AcDb::kDxfInt32 + 1, m_nID);

	return (pFiler->filerStatus());
}

Acad::ErrorStatus ADSKEmployeeDetails::dxfInFields(AcDbDxfFiler * pFiler) {
	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbObject::dxfInFields(pFiler);
	if (es != Acad::eOk || !pFiler->atSubclassData(_RXST("ADSKEmployeeDetails")))
		return (pFiler->filerStatus());
	//----- Object version number needs to be read first
	struct resbuf rb;
	pFiler->readItem(&rb);
	if (rb.restype != AcDb::kDxfInt32) {
		pFiler->pushBackItem();
		pFiler->setError(Acad::eInvalidDxfCode, _RXST("\nError: expected group code %d (version #)"), AcDb::kDxfInt32);
		return (pFiler->filerStatus());
	}
	Adesk::UInt32 version = (Adesk::UInt32)rb.resval.rlong;
	if (version > ADSKEmployeeDetails::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < ADSKEmployeeDetails::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params in non order dependant manner
	while (es == Acad::eOk && (es = pFiler->readResBuf(&rb)) == Acad::eOk) {
		switch (rb.restype) {
			//----- Read params by looking at their DXF code (example below)
		case AcDb::kDxfXTextString:
			if (m_szlastName != NULL)
				free(m_szlastName);
			m_szlastName = _tcsdup(rb.resval.rstring);
			break;
		case  AcDb::kDxfXTextString + 1:
			if (m_szfirstName != NULL)
				free(m_szfirstName);
			m_szfirstName = _tcsdup(rb.resval.rstring);
			break;
		case  AcDb::kDxfInt32:
			m_nCube = rb.resval.rlong;
			break;
		case AcDb::kDxfInt32 + 1:
			m_nID = rb.resval.rlong;
			break;

		default:
			//----- An unrecognized group. Push it back so that the subclass can read it again.
			pFiler->pushBackItem();
			es = Acad::eEndOfFile;
			break;
		}
	}
	//----- At this point the es variable must contain eEndOfFile
	//----- - either from readResBuf() or from pushback. If not,
	//----- it indicates that an error happened and we should
	//----- return immediately.
	if (es != Acad::eEndOfFile)
		return (Acad::eInvalidResBuf);

	return (pFiler->filerStatus());
}

Acad::ErrorStatus ADSKEmployeeDetails::setID(const Adesk::Int32 anID)
{
	assertWriteEnabled();
	m_nID = anID;
	return Acad::eOk;
}

Acad::ErrorStatus ADSKEmployeeDetails::ID(Adesk::Int32 & anID)
{
	assertReadEnabled();
	anID = m_nID;
	return Acad::eOk;
}

Acad::ErrorStatus ADSKEmployeeDetails::setCube(const Adesk::Int32 anCube)
{
	assertWriteEnabled();
	m_nCube = anCube;
	return Acad::eOk;
}

Acad::ErrorStatus ADSKEmployeeDetails::cube(Adesk::Int32 & anCube)
{
	assertReadEnabled();
	anCube = m_nCube;
	return Acad::eOk;
}

Acad::ErrorStatus ADSKEmployeeDetails::setFirstName(const TCHAR * aszFirstName)
{
	assertWriteEnabled();
	if (m_szfirstName)
		free(m_szfirstName);
	m_szfirstName = _tcsdup(aszFirstName);
	return Acad::eOk;
}

Acad::ErrorStatus ADSKEmployeeDetails::firstName(TCHAR * &aszFirstName)
{
	assertReadEnabled();
	aszFirstName = _tcsdup(m_szfirstName);
	return Acad::eOk;
}

Acad::ErrorStatus ADSKEmployeeDetails::setLastName(const TCHAR * aszLastName)
{
	assertWriteEnabled();
	if (m_szlastName)
		free(m_szlastName);
	m_szlastName = _tcsdup(aszLastName);
	return Acad::eOk;
}

Acad::ErrorStatus ADSKEmployeeDetails::lastName(TCHAR * & aszLastName)
{
	assertReadEnabled();
	m_szlastName = _tcsdup(m_szlastName);
	return Acad::eOk;
}