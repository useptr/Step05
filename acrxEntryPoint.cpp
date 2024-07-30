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
		if (CreateBlockRecord(_T("EMPLOYEE")) != Acad::eOk) {
			acutPrintf(_T("\nERROR: Couldn't create block record"));
			return;
		}
		acutPrintf(_T("\nBlock record EMPLOYEE successfully created"));
	}

	static void AsdkStep05_ADDDETAIL(void)
	{

		// Let the user select a block reference
		AcDbObjectId objectId;
		if (SelectAcDbObject(_T("Select employee: "), objectId) != Acad::eOk)
			return;
		// Check with the isKindOf() function that the user has selected a block reference 
		AcDbObjectPointer<AcDbObject> pObject(objectId, AcDb::kForWrite);
		if (!pObject->isKindOf(AcDbBlockReference::desc())) {
			acutPrintf(_T("\nThis is not a block reference."));
			return;
		}
		// Input employee data for employee ID; employee Cube; employee first name; employee last name 
		int nId, nCubeNumber;
		TCHAR szFirstName[133];
		TCHAR szLastName[133];
		if (acedGetInt(_T("Enter employee ID: "), &nId) != RTNORM
			|| acedGetInt(_T("Enter cube number: "), &nCubeNumber) != RTNORM
			|| acedGetString(0, _T("Enter employee first name: "), szFirstName) != RTNORM
			|| acedGetString(0, _T("Enter employee last name: "), szLastName) != RTNORM
			) {
			return;
		}
		// Get the extension dictionary of the EMPLOYEE block reference 
		AcDbObjectId extDictId = pObject->extensionDictionary();
		if (AcDbObjectId::kNull == extDictId) { // If the EMPLOYEE block reference does not have an extension dictionary create one 
			if (pObject->createExtensionDictionary() != Acad::eOk) {
				acutPrintf(_T("\nFailed to create ext. dictionary."));
				return;
			}
			extDictId = pObject->extensionDictionary();
		}
		// Open the extension dictionary.
		AcDbDictionaryPointer pExtDict(extDictId, AcDb::kForWrite);
		if (pExtDict.openStatus() != Acad::eOk) {
			acutPrintf(_T("\nFailed to open ext. dictionary."));
			return;
		}
		// If the extension dictionary was erased unerase it 
		if (pExtDict->isErased())
			pExtDict->erase(Adesk::kFalse);
		// Retrieve the "ASDK_EMPLOYEE_DICTIONARY"
		AcDbObjectId employeeDictId{ AcDbObjectId::kNull };
		AcDbDictionaryPointer pEmployeeDict;
		if (pExtDict->getAt(_T("ASDK_EMPLOYEE_DICTIONARY"), employeeDictId) == Acad::eKeyNotFound) {
			// If the "ASDK_EMLOYEE_DICTIONARY" AcDbDictionary does not exist, create an "ASDK_EMPLOYEE_DICTIONARY" AcDbDictionary and add it to the EMPLOYEE block reference extension dictionary. 
			pEmployeeDict.create();
			if (pExtDict->setAt(_T("ASDK_EMPLOYEE_DICTIONARY"), pEmployeeDict, employeeDictId) != Acad::eOk) {
				acutPrintf(_T("\nFailed to create the 'Employee' dictionary."));
				return;
			}
		}
		else {
			pEmployeeDict.open(employeeDictId, AcDb::kForWrite);
		}
		if (pEmployeeDict.openStatus() != Acad::eOk) {
			acutPrintf(_T("\nFailed to create the 'Employee' dictionary."));
			return;
		}
		// Check to see if an AsdkEmployeeDetails object is already present in the "ASDK_EMPLOYEE_DICTIONARY"
		AcDbObjectId employeeDetailstId;
		if (pEmployeeDict->getAt(_T("DETAILS"), employeeDetailstId) == Acad::eOk) {
			acutPrintf(_T("\nDetails already assign to that 'Employee' object."));
			return;
		}
		AcDbObjectPointer<ADSKEmployeeDetails> pEmployeeDetails;
		// If an AsdkEmployeeDetails object does not exist, create a new AsdkEmployeeDetails object and set its data. 
		pEmployeeDetails.create();
		pEmployeeDetails->setID(nId);
		pEmployeeDetails->setCube(nCubeNumber);
		pEmployeeDetails->setFirstName(szFirstName);
		pEmployeeDetails->setLastName(szLastName);
		// Add a new AsdkEmployeeDetails object under the key "DETAILS" in the "ASDK_EMPLOYEE_DICTIONARY" 
		if (pEmployeeDict->setAt(_T("DETAILS"), pEmployeeDetails, employeeDetailstId) != Acad::eOk) {
			acutPrintf(_T("\nFailed to add details to that object."));
			return;

		}
		acutPrintf(_T("\nDetails successfully added!"));
	}
	static void AsdkStep05_LISTDETAILS(void)
	{
		// Let the user select a block reference
		AcDbObjectId objectId;
		if (SelectAcDbObject(_T("Select employee: "), objectId) != Acad::eOk)
			return;
		AcDbObjectPointer<AcDbObject> pObject(objectId);
		// Check with the isKindOf() function that the user has selected a block reference 
		if (!pObject->isKindOf(AcDbBlockReference::desc())) { // Return if it is not a block reference
			acutPrintf(_T("\nThis is not a block reference"));
			return;
		}
		// Retrieve the extension dictionary of the block reference. 
		AcDbDictionaryPointer pExtDict(pObject->extensionDictionary());
		if (pExtDict.openStatus() != Acad::eOk) { // If there is none return
			acutPrintf(_T("\nFailed to open ext. dictionary"));
			return;
		}
		// Retrieve the "ASDK_EMPLOYEE_DICTIONARY"
		AcDbObjectId employeeDictId{ AcDbObjectId::kNull };
		if (pExtDict->getAt(_T("ASDK_EMPLOYEE_DICTIONARY"), employeeDictId) == Acad::eKeyNotFound) { // If there is none return.
			acutPrintf(_T("\nFailed to open ASDK_EMPLOYEE_DICTIONARY"));
			return;
		}
		AcDbDictionaryPointer pEmployeeDict(employeeDictId);
		// Retrieve the AsdkEmployeeDetails object from the "ASDK_EMPLOYEE_DICTIONARY" under the "DETAILS" key.
		AcDbObjectId employeeDetailstId;
		if (pEmployeeDict->getAt(_T("DETAILS"), employeeDetailstId) != Acad::eOk) {
			acutPrintf(_T("\nFailed to open DETAILS"));
			return;
		}
		AcDbObjectPointer<ADSKEmployeeDetails> pEmployeeDetails(employeeDetailstId);
		if (pEmployeeDetails.openStatus() != Acad::eOk) {
			acutPrintf(_T("\nFailed to open DETAILS"));
			return;
		}
		// Retrieve the EmployeeDetails object data and print the details. 
		Adesk::Int32 nField;
		pEmployeeDetails->ID(nField);
		acutPrintf(_T("Employee's ID: %d\n"), nField);
		pEmployeeDetails->cube(nField);
		acutPrintf(_T("Employee's cube number: %d\n"), nField);
		TCHAR* szField{ nullptr }; // TODO improve
		{
			pEmployeeDetails->firstName(szField);
			std::unique_ptr<TCHAR[]> pField(szField);
			acutPrintf(_T("Employee's first name: %s\n"), szField);
		}
		
		{
			pEmployeeDetails->lastName(szField);
			std::unique_ptr<TCHAR[]> pField(szField);
			acutPrintf(_T("Employee's last name: %s\n"), szField);
		}
	}

	static void AsdkStep05_REMOVEDETAIL(void)
	{
		// Let the user select a block reference
		AcDbObjectId objectId;
		if (SelectAcDbObject(_T("Select employee: "), objectId) != Acad::eOk)
			return;
		AcDbObjectPointer<AcDbObject> pObject(objectId);
		if (!pObject->isKindOf(AcDbBlockReference::desc())) {
			acutPrintf(_T("\nThis is not a block reference."));
			return;
		}
		// Retrieve the extension dictionary of the block reference. 
		AcDbDictionaryPointer pExtDict(pObject->extensionDictionary());
		if (pExtDict.openStatus() != Acad::eOk) { // If there is none return
			acutPrintf(_T("\nFailed to open ext. dictionary"));
			return;
		}
		// Retrieve the "ASDK_EMPLOYEE_DICTIONARY"
		AcDbObjectId employeeDictId{ AcDbObjectId::kNull };
		if (pExtDict->getAt(_T("ASDK_EMPLOYEE_DICTIONARY"), employeeDictId) == Acad::eKeyNotFound) { // If there is none return.
			acutPrintf(_T("\nFailed to open ASDK_EMPLOYEE_DICTIONARY"));
			return;
		}
		AcDbDictionaryPointer pEmployeeDict(employeeDictId);
		// Retrieve the AsdkEmployeeDetails object from the "ASDK_EMPLOYEE_DICTIONARY" under the "DETAILS" key
		AcDbObjectId employeeDetailstId;
		if (pEmployeeDict->getAt(_T("DETAILS"), employeeDetailstId) != Acad::eOk) {
			acutPrintf(_T("\nFailed to open DETAILS"));
			return;
		}
		AcDbObjectPointer<ADSKEmployeeDetails> pEmployeeDetails(employeeDetailstId, AcDb::kForWrite);
		if (pEmployeeDetails.openStatus() != Acad::eOk) {
			acutPrintf(_T("\nFailed to open DETAILS for write"));
			return;
		}
		// Erase the AsdkEmployeeDetails object
		pEmployeeDetails->erase();
		acutPrintf(_T("\nDetails successfully removed!"));
		// Erase the dictionaries if they contains no more entries
		if (pEmployeeDict->numEntries() == 0) {
			if (!pEmployeeDict->isWriteEnabled() && pEmployeeDict->upgradeOpen() != Acad::eOk) {
				return;
			}
			pEmployeeDict->erase();
		}
		// Erase ext. dictionary if it has no more entries
		if (pExtDict->numEntries() == 0) {
			if (!pExtDict->isWriteEnabled() && pExtDict->upgradeOpen() != Acad::eOk) {
				return;
			}
			pExtDict->erase();
		}
	}
	
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CS5App)

ACED_ARXCOMMAND_ENTRY_AUTO(CS5App, AsdkStep05, _CREATE, CREATE, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CS5App, AsdkStep05, _LISTDETAILS, LISTDETAILS, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CS5App, AsdkStep05, _REMOVEDETAIL, REMOVEDETAIL, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CS5App, AsdkStep05, _ADDDETAIL, ADDDETAIL, ACRX_CMD_TRANSPARENT, NULL)

