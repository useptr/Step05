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
		// Add your code for command AsdkStep05._CREATE here
				// Add your code for command AsdkStep03._CREATE here
		// TODO: Implement the command

		// Create a new layer named "USER"
		// createLayer returns the object ID of the newly created layer
		AcDbObjectId layerId;
		if (createLayer(_T("USER"), layerId) != Acad::eOk) {
			acutPrintf(_T("\nERROR: Couldn't create layer record."));
			return;
		}
		// This is not always needed, but a call to 'applyCurDwgLayerTableChanges()'
		// will synchronize the newly created layer table change with the 
		// rest of the current DWG database.
		applyCurDwgLayerTableChanges();

		acutPrintf(_T("\nLayer USER successfully created."));

		// Create a new block definition named "EMPLOYEE"
		if (createBlockRecord(_T("EMPLOYEE")) != Acad::eOk)
			acutPrintf(_T("\nERROR: Couldn't create block record."));
		else
			acutPrintf(_T("\nBlock EMPLOYEE successfully created."));
	}

	static void AsdkStep05_ADDDETAIL(void)
	{
		// Add your code for command AsdkStep05._ADDDETAIL here
		// Prompt the user for the employee details
		ads_name ename;
		ads_point pt;
		// Get the data from the user
		if (acedEntSel(_T("Select employee: "), ename, pt) != RTNORM)
			return;
		// Do a quick check
		// a more comprehensive check could include 
		// whether we already have the detail object on this candidate
		AcDbObjectId idO;
		if (acdbGetObjectId(idO, ename) != Acad::eOk)
			return;
		AcDbObject* pO;
		if (acdbOpenAcDbObject(pO, idO, AcDb::kForWrite) != Acad::eOk)
			return;
		if (!pO->isKindOf(AcDbBlockReference::desc())) {
			acutPrintf(_T("\nThis is not a block reference."));
			pO->close();
			return;
		}
		// Get user input
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
		// Get the extension dictionary
		if ((idO = pO->extensionDictionary()) == AcDbObjectId::kNull) {
			if (pO->createExtensionDictionary() != Acad::eOk) {
				pO->close();
				acutPrintf(_T("\nFailed to create ext. dictionary."));
				return;
			}
			idO = pO->extensionDictionary();
		}
		// We do not need the block reference object anymore.
		pO->close();
		// Make sure you open erased extension dictionaries
		// you may need to unerase them
		AcDbDictionary* pExtDict;
		if (acdbOpenAcDbObject((AcDbObject*&)pExtDict, idO, AcDb::kForWrite, Adesk::kTrue) != Acad::eOk) {
			acutPrintf(_T("\nFailed to open ext. dictionary."));
			return;
		}
		// Unerase the ext. dictionary if it was erased
		if (pExtDict->isErased())
			pExtDict->erase(Adesk::kFalse);
		// See if our dictionary is already there
		AcDbDictionary* pEmployeeDict;
		if (pExtDict->getAt(_T("ASDK_EMPLOYEE_DICTIONARY"), idO) == Acad::eKeyNotFound) {
			// Create it if not
			pEmployeeDict = new AcDbDictionary;
			Acad::ErrorStatus es;
			if ((es = pExtDict->setAt(_T("ASDK_EMPLOYEE_DICTIONARY"), pEmployeeDict, idO)) != Acad::eOk) {
				pExtDict->close();
				acutPrintf(_T("\nFailed to create the 'Employee' dictionary."));
				return;
			}
		}
		else {
			// Open our dictionary for write if it is already there
			if (acdbOpenAcDbObject(pO, idO, AcDb::kForWrite) != Acad::eOk) {
				pExtDict->close();
				acutPrintf(_T("\nFailed to open the 'Employee' dictionary."));
				return;
			}
			// Check if someone has else has created an entry with our name
			// that is not a dictionary.
			if ((pEmployeeDict = AcDbDictionary::cast(pO)) == NULL) {
				pO->close();
				pExtDict->close();
				acutPrintf(_T("\nThe entry is not a dictionary"));
				return;
			}
		}
		// We do not need the ext. dictionary object anymore
		pExtDict->close();
		// Check if a record with this key is already there
		if (pEmployeeDict->getAt(_T("DETAILS"), idO) == Acad::eOk) {
			pEmployeeDict->close();
			acutPrintf(_T("\nDetails already assign to that 'Employee' object."));
			return;
		}
		// Create an EmployeeDetails object and set its fields
		ADSKEmployeeDetails* pEmployeeDetails = new ADSKEmployeeDetails;
		pEmployeeDetails->setID(id);
		pEmployeeDetails->setCube(cubeNumber);
		pEmployeeDetails->setFirstName(strFirstName);
		pEmployeeDetails->setLastName(strLastName);
		// Add it to the dictionary
		if (pEmployeeDict->setAt(_T("DETAILS"), pEmployeeDetails, idO) != Acad::eOk) {
			delete pEmployeeDetails;
			acutPrintf(_T("\nFailed to add details to that object."));
			pEmployeeDict->close();
			return;

		}
		// Done
		acutPrintf(_T("\nDetails successfully added!"));
		pEmployeeDict->close();
		pEmployeeDetails->close();
	}

	static void AsdkStep05_LISTDETAILS(void)
	{
		// Add your code for command AsdkStep05._LISTDETAILS here
		ads_name ename;
		ads_point pt;
		// Get the data from the user
		if (acedEntSel(_T("Select employee: "), ename, pt) != RTNORM)
			return;
		// Do a quick check
		// a more comprehensive check could include 
		// whether we already have the detail object on this candidate
		AcDbObjectId idO;
		if (acdbGetObjectId(idO, ename) != Acad::eOk)
			return;
		AcDbObject* pO;
		if (acdbOpenAcDbObject(pO, idO, AcDb::kForRead) != Acad::eOk)
			return;
		if (!pO->isKindOf(AcDbBlockReference::desc())) {
			acutPrintf(_T("\nThis is not a block reference."));
			pO->close();
			return;
		}
		// Get the Ext. Dictionary
		if ((idO = pO->extensionDictionary()) == AcDbObjectId::kNull) {
			// Nothing to do
			pO->close();
			return;
		}
		// We do not need the block reference object anymore.
		pO->close();
		// If erased, nothing to do
		AcDbDictionary* pExtDict;
		if (acdbOpenAcDbObject((AcDbObject*&)pExtDict, idO, AcDb::kForRead, Adesk::kFalse) != Acad::eOk) {
			acutPrintf(_T("\nFailed to open ext. dictionary."));
			return;
		}
		// See if our dictionary is already there
		AcDbDictionary* pEmployeeDict;
		if (pExtDict->getAt(_T("ASDK_EMPLOYEE_DICTIONARY"), idO) == Acad::eKeyNotFound) {
			// Nothing to do if not
			pExtDict->close();
			return;
		}
		else {
			// Open dictionary for write if it is already there
			if (acdbOpenAcDbObject(pO, idO, AcDb::kForRead) != Acad::eOk) {
				pExtDict->close();
				acutPrintf(_T("\nFailed to open the 'Employee' dictionary."));
				return;
			}
			// Check if someone has else has created an entry with our name
			// that is not a dictionary. 
			if ((pEmployeeDict = AcDbDictionary::cast(pO)) == NULL) {
				pO->close();
				pExtDict->close();
				acutPrintf(_T("\nThe entry is not a dictionary"));
				return;
			}
		}
		// Check if a record with this key is already there
		if (pEmployeeDict->getAt(_T("DETAILS"), idO) != Acad::eOk) {
			// Nothing to do
			pEmployeeDict->close();
			pExtDict->close();
			return;
		}
		// Open the object for write 
		if (acdbOpenAcDbObject(pO, idO, AcDb::kForRead) != Acad::eOk) {
			pEmployeeDict->close();
			pExtDict->close();
			acutPrintf(_T("\nFailed to open the object detail."));
			return;
		}
		// Check it is a AsdkEmployeeDetails object
		ADSKEmployeeDetails* pEmployeeDetails = ADSKEmployeeDetails::cast(pO);
		if (pEmployeeDetails == NULL) {
			acutPrintf(_T("\nNo details found!."));
			pO->close();
			pEmployeeDict->close();
			pExtDict->close();
			return;
		}
		// And display details
		Adesk::Int32 i;
		pEmployeeDetails->iD(i);
		acutPrintf(_T("*Employee's ID: %d\n"), i);
		pEmployeeDetails->cube(i);
		acutPrintf(_T("*Employee's cube number: %d\n"), i);
		TCHAR* st = NULL;
		pEmployeeDetails->firstName(st);
		acutPrintf(_T("*Employee's first name: %s\n"), st);
		delete[] st;
		pEmployeeDetails->lastName(st);
		acutPrintf(_T("*Employee's last name: %s\n"), st);
		delete[] st;

		pO->close();
		pEmployeeDict->close();
		pExtDict->close();
	}


	// ----- AsdkStep05._REMOVEDETAIL command (do not rename)
	static void AsdkStep05_REMOVEDETAIL(void)
	{
		ads_name ename;
		ads_point pt;
		// Get the data from the user
		if (acedEntSel(_T("Select employee: "), ename, pt) != RTNORM)
			return;
		// Do a quick check
		// a more comprehensive check could include 
		// whether we already have the detail object on this candidate
		AcDbObjectId idO;
		if (acdbGetObjectId(idO, ename) != Acad::eOk)
			return;
		AcDbObject* pO;
		if (acdbOpenAcDbObject(pO, idO, AcDb::kForRead) != Acad::eOk)
			return;
		if (!pO->isKindOf(AcDbBlockReference::desc())) {
			acutPrintf(_T("\nThis is not a block reference."));
			pO->close();
			return;
		}
		// Get the Ext. Dictionary
		if ((idO = pO->extensionDictionary()) == AcDbObjectId::kNull) {
			// Nothing to do
			pO->close();
			return;
		}
		// We do not need the block reference object anymore.
		pO->close();
		// If erased, nothing to do
		AcDbDictionary* pExtDict;
		if (acdbOpenAcDbObject((AcDbObject*&)pExtDict, idO, AcDb::kForWrite, Adesk::kFalse) != Acad::eOk) {
			acutPrintf(_T("\nFailed to open ext. dictionary."));
			return;
		}
		// See if our dictionary is already there
		AcDbDictionary* pEmployeeDict;
		if (pExtDict->getAt(_T("ASDK_EMPLOYEE_DICTIONARY"), idO) == Acad::eKeyNotFound) {
			// Nothing to do if not
			pExtDict->close();
			return;
		}
		else {
			// Open the dictionary for write if it is already there
			if (acdbOpenAcDbObject(pO, idO, AcDb::kForWrite) != Acad::eOk) {
				pExtDict->close();
				acutPrintf(_T("\nFailed to open the 'Employee' dictionary."));
				return;
			}
			// Check if someone has else has created an entry with our name
			// that is not a dictionary.
			if ((pEmployeeDict = AcDbDictionary::cast(pO)) == NULL) {
				pO->close();
				pExtDict->close();
				acutPrintf(_T("\nThe entry is not a dictionary"));
				return;
			}
		}
		// Check if a record with this key is already there
		if (pEmployeeDict->getAt(_T("DETAILS"), idO) != Acad::eOk) {
			pEmployeeDict->close();
			pExtDict->close();
			acutPrintf(_T("\nNo details assigned to that 'Employee' object."));
			return;
		}
		// Open the object for write 
		if (acdbOpenAcDbObject(pO, idO, AcDb::kForWrite) != Acad::eOk) {
			pEmployeeDict->close();
			pExtDict->close();
			acutPrintf(_T("\nFailed to open the object detail."));
			return;
		}
		// And erase it
		pO->erase();
		pO->close();
		// Erase dictionary if it has no more entries
		if (pEmployeeDict->numEntries() == 0)
			pEmployeeDict->erase();
		pEmployeeDict->close();
		// Erase ext. dictionary if it has no more entries
		if (pExtDict->numEntries() == 0)
			pExtDict->erase();
		pExtDict->close();
	}
	
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CS5App)

ACED_ARXCOMMAND_ENTRY_AUTO(CS5App, AsdkStep05, _CREATE, CREATE, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CS5App, AsdkStep05, _LISTDETAILS, LISTDETAILS, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CS5App, AsdkStep05, _REMOVEDETAIL, REMOVEDETAIL, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CS5App, AsdkStep05, _ADDDETAIL, ADDDETAIL, ACRX_CMD_TRANSPARENT, NULL)

