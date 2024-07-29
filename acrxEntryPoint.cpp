// (C) Copyright 2002-2012 by Autodesk, Inc. 
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
//----- acrxEntryPoint.cpp
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include "Tchar.h"
#include "utilities.h"
#include "ADSKEmployeeDetails.h"
//-----------------------------------------------------------------------------
#define szRDS _RXST("ADSK")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CS5App : public AcRxArxApp {

public:
	CS5App () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		
		// TODO: Add your initialization code here
		AcRxObject* pSvc;
		if (!(pSvc = acrxServiceDictionary->at(ASDKEMPLOYEEDETAILS_DBXSERVICE)))
		{
			// Try to load the module, if it is not yet present 
			if (!acrxDynamicLinker->loadModule(_T("ADSKEmployeeDetails.dbx"), 0))
			{

				acutPrintf(_T("Unable to load ADSKEmployeeDetails.dbx. Unloading this application...\n"));
				return (AcRx::kRetError);

			}

		}
		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}
	
	static void AsdkStep05_CREATE(void)
	{
		// Create a new block record named "EMPLOYEE"
		if (createBlockRecord(_T("EMPLOYEE")) != Acad::eOk) {
			acutPrintf(_T("\nERROR: Couldn't create block record"));
			return;
		}
		acutPrintf(_T("\nBlock record EMPLOYEE successfully created"));
	}

	static void AsdkStep05_ADDDETAIL(void)
	{
		// Let the user select a block reference
		AcDbObject* pO;
		Acad::ErrorStatus es = openSelectedAcDbObject(_T("Select employee: "), pO, AcDb::kForWrite); // need kForWrite?
		if (es != Acad::eOk)
			return;
		// Check with the isKindOf() function that the user has selected a block reference 
		if (!pO->isKindOf(AcDbBlockReference::desc())) {
			acutPrintf(_T("\nThis is not a block reference."));
			pO->close();
			return;
		}
		// Input employee data for employee ID; employee Cube; employee first name; employee last name 
		int id, cubeNumber;
		TCHAR strFirstName[133];
		TCHAR strLastName[133];
		if (acedGetInt(_T("Enter employee ID: "), &id) != RTNORM
			|| acedGetInt(_T("Enter cube number: "), &cubeNumber) != RTNORM
			|| acedGetString(0, _T("Enter employee first name: "), strFirstName) != RTNORM
			|| acedGetString(0, _T("Enter employee last name: "), strLastName) != RTNORM
			) {
			pO->close();
			return;
		}
		// Get the extension dictionary of the EMPLOYEE block reference 
		AcDbObjectId extDictId = pO->extensionDictionary();
		if (AcDbObjectId::kNull == extDictId) { // If the EMPLOYEE block reference does not have an extension dictionary create one 
			if (pO->createExtensionDictionary() != Acad::eOk) {
				pO->close();
				acutPrintf(_T("\nFailed to create ext. dictionary."));
				return;
			}
			extDictId = pO->extensionDictionary();
		}
		pO->close();
		// Open the extension dictionary.
		AcDbDictionary* pExtDict;
		if (acdbOpenObject(pExtDict, extDictId, AcDb::kForWrite, Adesk::kTrue) != Acad::eOk) {
			acutPrintf(_T("\nFailed to open ext. dictionary."));
			return;
		}
		// If the extension dictionary was erased unerase it 
		if (pExtDict->isErased())
			pExtDict->erase(Adesk::kFalse);
		// Retrieve the "ASDK_EMPLOYEE_DICTIONARY"
		AcDbDictionary* pEmployeeDict{nullptr};
		if (pExtDict->getAt(_T("ASDK_EMPLOYEE_DICTIONARY"), pEmployeeDict) == Acad::eKeyNotFound) {
			// If the "ASDK_EMLOYEE_DICTIONARY" AcDbDictionary does not exist, create an "ASDK_EMPLOYEE_DICTIONARY" AcDbDictionary and add it to the EMPLOYEE block reference extension dictionary. 
			pEmployeeDict = new AcDbDictionary;
			AcDbObjectId employeeDictId;
			if (pExtDict->setAt(_T("ASDK_EMPLOYEE_DICTIONARY"), pEmployeeDict, employeeDictId) != Acad::eOk) {
				delete pEmployeeDict;
				pExtDict->close();
				acutPrintf(_T("\nFailed to create the 'Employee' dictionary."));
				return;
			}
		}
		pExtDict->close();
		if (nullptr == pEmployeeDict) {
			acutPrintf(_T("\nFailed to create the 'Employee' dictionary."));
			return;
		}
		// Check to see if an AsdkEmployeeDetails object is already present in the "ASDK_EMPLOYEE_DICTIONARY"
		ADSKEmployeeDetails* pEmployeeDetails{nullptr};
		if (pEmployeeDict->getAt(_T("DETAILS"), pEmployeeDetails) == Acad::eOk) {
			pEmployeeDict->close();
			acutPrintf(_T("\nDetails already assign to that 'Employee' object."));
			return;
		}
		// If an AsdkEmployeeDetails object does not exist, create a new AsdkEmployeeDetails object and set its data. 
		pEmployeeDetails = new ADSKEmployeeDetails;
		pEmployeeDetails->setID(id);
		pEmployeeDetails->setCube(cubeNumber);
		pEmployeeDetails->setFirstName(strFirstName);
		pEmployeeDetails->setLastName(strLastName);
		AcDbObjectId employeeDetailstId;
		// Add a new AsdkEmployeeDetails object under the key "DETAILS" in the "ASDK_EMPLOYEE_DICTIONARY" 
		if (pEmployeeDict->setAt(_T("DETAILS"), pEmployeeDetails, employeeDetailstId) != Acad::eOk) {
			acutPrintf(_T("\nFailed to add details to that object."));
			delete pEmployeeDetails;
			pEmployeeDict->close();
			return;

		}
		acutPrintf(_T("\nDetails successfully added!"));
		pEmployeeDict->close();
		pEmployeeDetails->close();
	}
	static void AsdkStep05_LISTDETAILS(void)
	{
		// Let the user select a block reference
		AcDbObject* pO;
		Acad::ErrorStatus es = openSelectedAcDbObject(_T("Select employee: "), pO, AcDb::kForRead);
		if (es != Acad::eOk)
			return;
		// Check with the isKindOf() function that the user has selected a block reference 
		if (!pO->isKindOf(AcDbBlockReference::desc())) { // Return if it is not a block reference
			acutPrintf(_T("\nThis is not a block reference."));
			pO->close();
			return;
		}
		// Retrieve the extension dictionary of the block reference. 
		AcDbObjectId extDictId = pO->extensionDictionary();
		pO->close();
		if (extDictId == AcDbObjectId::kNull) { // If there is none return
			return;
		}
		AcDbDictionary* pExtDict;
		if (acdbOpenObject(pExtDict, extDictId, AcDb::kForRead, Adesk::kFalse) != Acad::eOk) {
			acutPrintf(_T("\nFailed to open ext. dictionary."));
			return;
		}
		// Retrieve the "ASDK_EMPLOYEE_DICTIONARY"
		AcDbDictionary* pEmployeeDict{ nullptr };
		if (pExtDict->getAt(_T("ASDK_EMPLOYEE_DICTIONARY"), pEmployeeDict) == Acad::eKeyNotFound) { // If there is none return.
			pExtDict->close();
			return;
		}
		pExtDict->close();
		// Retrieve the AsdkEmployeeDetails object from the "ASDK_EMPLOYEE_DICTIONARY" under the "DETAILS" key.
		ADSKEmployeeDetails* pEmployeeDetails{ nullptr };
		if (pEmployeeDict->getAt(_T("DETAILS"), pEmployeeDetails) != Acad::eOk) {
			// Nothing to do
			pEmployeeDict->close();
			return;
		}
		pEmployeeDict->close();
		// Retrieve the AsdkEmployeeDetails object data and print the details. 
		Adesk::Int32 i;
		pEmployeeDetails->iD(i);
		acutPrintf(_T("Employee's ID: %d\n"), i);
		pEmployeeDetails->cube(i);
		acutPrintf(_T("Employee's cube number: %d\n"), i);
		TCHAR* st = NULL;
		pEmployeeDetails->firstName(st);
		acutPrintf(_T("Employee's first name: %s\n"), st);
		delete[] st;
		pEmployeeDetails->lastName(st);
		acutPrintf(_T("Employee's last name: %s\n"), st);
		delete[] st;
		pEmployeeDetails->close();
	}

	static void AsdkStep05_REMOVEDETAIL(void)
	{
		// Let the user select a block reference
		AcDbObject* pO;
		Acad::ErrorStatus es = openSelectedAcDbObject(_T("Select employee: "), pO, AcDb::kForRead);
		if (es != Acad::eOk)
			return;
		if (!pO->isKindOf(AcDbBlockReference::desc())) {
			acutPrintf(_T("\nThis is not a block reference."));
			pO->close();
			return;
		}
		// Retrieve the extension dictionary of the block reference. 
		AcDbObjectId extDictId = pO->extensionDictionary();
		pO->close();
		if (extDictId == AcDbObjectId::kNull) { // If there is none return
			return;
		}
		AcDbDictionary* pExtDict;
		if (acdbOpenObject(pExtDict, extDictId, AcDb::kForWrite, Adesk::kFalse) != Acad::eOk) {
			acutPrintf(_T("\nFailed to open ext. dictionary."));
			return;
		}
		// See if our dictionary is already there
		AcDbDictionary* pEmployeeDict;
		if (pExtDict->getAt(_T("ASDK_EMPLOYEE_DICTIONARY"), pEmployeeDict) == Acad::eKeyNotFound) {
			// Nothing to do if not
			pExtDict->close();
			return;
		}
		
		ADSKEmployeeDetails* pEmployeeDetails{ nullptr };
		// Retrieve the AsdkEmployeeDetails object from the "ASDK_EMPLOYEE_DICTIONARY" under the "DETAILS" key
		if (pEmployeeDict->getAt(_T("DETAILS"), pEmployeeDetails, AcDb::kForWrite) != Acad::eOk) {
			pEmployeeDict->close();
			pExtDict->close();
			acutPrintf(_T("\nNo details assigned to that 'Employee' object."));
			return;
		}

		// Erase the AsdkEmployeeDetails object
		pEmployeeDetails->erase();
		pEmployeeDetails->close();
		// Erase the dictionaries if they contains no more entries
		if (pEmployeeDict->numEntries() == 0) {
			if (!pEmployeeDict->isWriteEnabled() && pEmployeeDict->upgradeOpen() != Acad::eOk) {
				pEmployeeDict->close();
				pExtDict->close();
				return;
			}
			pEmployeeDict->erase();
		}
		pEmployeeDict->close();
		// Erase ext. dictionary if it has no more entries
		if (pExtDict->numEntries() == 0) {
			if (!pExtDict->isWriteEnabled() && pExtDict->upgradeOpen() != Acad::eOk) {
				pExtDict->close();
				return;
			}
			pExtDict->erase();
		}
		pExtDict->close();
	}
	
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CS5App)

ACED_ARXCOMMAND_ENTRY_AUTO(CS5App, AsdkStep05, _CREATE, CREATE, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CS5App, AsdkStep05, _LISTDETAILS, LISTDETAILS, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CS5App, AsdkStep05, _REMOVEDETAIL, REMOVEDETAIL, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CS5App, AsdkStep05, _ADDDETAIL, ADDDETAIL, ACRX_CMD_TRANSPARENT, NULL)

