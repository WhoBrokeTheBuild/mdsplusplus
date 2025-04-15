/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*******************************************************
 This module was generated, do not modify it
 To add new status messages modify 'one of the "*_messages.xml" files' and then do:
     python3 deploy/gen-messages-exceptions.py
*******************************************************/

#ifndef MDSPLUS_EXCEPTIONS_HPP
#define MDSPLUS_EXCEPTIONS_HPP

#include <stdexcept>

#include <camshr_messages.h>
#include <mdsdcl_messages.h>
#include <mdsshr_messages.h>
#include <mitdevices_messages.h>
#include <servershr_messages.h>
#include <tcl_messages.h>
#include <tdishr_messages.h>
#include <treeshr_messages.h>

namespace mdsplus {

class MDSplusException : public std::runtime_error
{
public:

    MDSplusException();

    MDSplusException(int status);

    inline int getStatus() const {
        return _status;
    }

private:

    int _status;

}; // class MdsException

void throwException(int status);

class MDSplusWarning : public MDSplusException
{
public:

    inline MDSplusWarning()
        : MDSplusException(MDSplusWARNING)
    { }

}; // MDSplusWARNING

class MDSplusError : public MDSplusException
{
public:

    inline MDSplusError()
        : MDSplusException(MDSplusERROR)
    { }

}; // MDSplusERROR

class MDSplusFatal : public MDSplusException
{
public:

    inline MDSplusFatal()
        : MDSplusException(MDSplusFATAL)
    { }

}; // MDSplusFATAL

class MDSplusSandbox : public MDSplusException
{
public:

    inline MDSplusSandbox()
        : MDSplusException(MDSplusSANDBOX)
    { }

}; // MDSplusSANDBOX

class LibException : public MDSplusException
{
public:

    inline LibException(int status)
        : MDSplusException(status)
    { }

}; // LibException

class LibInsufficientVirtualMemory : public LibException
{
public:

    inline LibInsufficientVirtualMemory()
        : LibException(LibINSVIRMEM)
    { }

}; // LibINSVIRMEM

class LibInvalidStringDescriptor : public LibException
{
public:

    inline LibInvalidStringDescriptor()
        : LibException(LibINVSTRDES)
    { }

}; // LibINVSTRDES

class LibInvalidArgument : public LibException
{
public:

    inline LibInvalidArgument()
        : LibException(LibINVARG)
    { }

}; // LibINVARG

class LibNotFound : public LibException
{
public:

    inline LibNotFound()
        : LibException(LibNOTFOU)
    { }

}; // LibNOTFOU

class LibQueueEmpty : public LibException
{
public:

    inline LibQueueEmpty()
        : LibException(LibQUEWASEMP)
    { }

}; // LibQUEWASEMP

class LibKeyNotFound : public LibException
{
public:

    inline LibKeyNotFound()
        : LibException(LibKEYNOTFOU)
    { }

}; // LibKEYNOTFOU

class StrException : public MDSplusException
{
public:

    inline StrException(int status)
        : MDSplusException(status)
    { }

}; // StrException

class StrStringTooLong : public StrException
{
public:

    inline StrStringTooLong()
        : StrException(StrSTRTOOLON)
    { }

}; // StrSTRTOOLON

class StrNoMatch : public StrException
{
public:

    inline StrNoMatch()
        : StrException(StrNOMATCH)
    { }

}; // StrNOMATCH

class StrInvalidDelimiter : public StrException
{
public:

    inline StrInvalidDelimiter()
        : StrException(StrINVDELIM)
    { }

}; // StrINVDELIM

class StrNoElement : public StrException
{
public:

    inline StrNoElement()
        : StrException(StrNOELEM)
    { }

}; // StrNOELEM

class TclException : public MDSplusException
{
public:

    inline TclException(int status)
        : MDSplusException(status)
    { }

}; // TclException

class TclFailedEssential : public TclException
{
public:

    inline TclFailedEssential()
        : TclException(TclFAILED_ESSENTIAL)
    { }

}; // TclFAILED_ESSENTIAL

class TclNoDispatchTable : public TclException
{
public:

    inline TclNoDispatchTable()
        : TclException(TclNO_DISPATCH_TABLE)
    { }

}; // TclNO_DISPATCH_TABLE

class CamException : public MDSplusException
{
public:

    inline CamException(int status)
        : MDSplusException(status)
    { }

}; // CamException

class CamDoneNoX : public CamException
{
public:

    inline CamDoneNoX()
        : CamException(CamDONE_NOX)
    { }

}; // CamDONE_NOX

class CamSerialTransmissionError : public CamException
{
public:

    inline CamSerialTransmissionError()
        : CamException(CamSERTRAERR)
    { }

}; // CamSERTRAERR

class CamSerialCrateControllerFailure : public CamException
{
public:

    inline CamSerialCrateControllerFailure()
        : CamException(CamSCCFAIL)
    { }

}; // CamSCCFAIL

class CamOffline : public CamException
{
public:

    inline CamOffline()
        : CamException(CamOFFLINE)
    { }

}; // CamOFFLINE

class MdsdclException : public MDSplusException
{
public:

    inline MdsdclException(int status)
        : MDSplusException(status)
    { }

}; // MdsdclException

class MdsdclError : public MdsdclException
{
public:

    inline MdsdclError()
        : MdsdclException(MdsdclERROR)
    { }

}; // MdsdclERROR

class MdsdclInvalidVerb : public MdsdclException
{
public:

    inline MdsdclInvalidVerb()
        : MdsdclException(MdsdclIVVERB)
    { }

}; // MdsdclIVVERB

class MdsdclAbsent : public MdsdclException
{
public:

    inline MdsdclAbsent()
        : MdsdclException(MdsdclABSENT)
    { }

}; // MdsdclABSENT

class MdsdclNegated : public MdsdclException
{
public:

    inline MdsdclNegated()
        : MdsdclException(MdsdclNEGATED)
    { }

}; // MdsdclNEGATED

class MdsdclNotNegatable : public MdsdclException
{
public:

    inline MdsdclNotNegatable()
        : MdsdclException(MdsdclNOTNEGATABLE)
    { }

}; // MdsdclNOTNEGATABLE

class MdsdclInvalidQualifier : public MdsdclException
{
public:

    inline MdsdclInvalidQualifier()
        : MdsdclException(MdsdclIVQUAL)
    { }

}; // MdsdclIVQUAL

class MdsdclPromptMore : public MdsdclException
{
public:

    inline MdsdclPromptMore()
        : MdsdclException(MdsdclPROMPT_MORE)
    { }

}; // MdsdclPROMPT_MORE

class MdsdclTooManyParameters : public MdsdclException
{
public:

    inline MdsdclTooManyParameters()
        : MdsdclException(MdsdclTOO_MANY_PRMS)
    { }

}; // MdsdclTOO_MANY_PRMS

class MdsdclTooManyValues : public MdsdclException
{
public:

    inline MdsdclTooManyValues()
        : MdsdclException(MdsdclTOO_MANY_VALS)
    { }

}; // MdsdclTOO_MANY_VALS

class MdsdclMissingValue : public MdsdclException
{
public:

    inline MdsdclMissingValue()
        : MdsdclException(MdsdclMISSING_VALUE)
    { }

}; // MdsdclMISSING_VALUE

class TreeException : public MDSplusException
{
public:

    inline TreeException(int status)
        : MDSplusException(status)
    { }

}; // TreeException

class TreeNoMoreNodes : public TreeException
{
public:

    inline TreeNoMoreNodes()
        : TreeException(TreeNMN)
    { }

}; // TreeNMN

class TreeNoMoreTags : public TreeException
{
public:

    inline TreeNoMoreTags()
        : TreeException(TreeNMT)
    { }

}; // TreeNMT

class TreeNodeNotFound : public TreeException
{
public:

    inline TreeNodeNotFound()
        : TreeException(TreeNNF)
    { }

}; // TreeNNF

class TreeTagNotFound : public TreeException
{
public:

    inline TreeTagNotFound()
        : TreeException(TreeTNF)
    { }

}; // TreeTNF

class TreeNotFound : public TreeException
{
public:

    inline TreeNotFound()
        : TreeException(TreeTREENF)
    { }

}; // TreeTREENF

class TreeNodeAlreadyThere : public TreeException
{
public:

    inline TreeNodeAlreadyThere()
        : TreeException(TreeALREADY_THERE)
    { }

}; // TreeALREADY_THERE

class TreeParentOff : public TreeException
{
public:

    inline TreeParentOff()
        : TreeException(TreePARENT_OFF)
    { }

}; // TreePARENT_OFF

class TreeNodeAndParentOff : public TreeException
{
public:

    inline TreeNodeAndParentOff()
        : TreeException(TreeBOTH_OFF)
    { }

}; // TreeBOTH_OFF

class TreeNodeIsOff : public TreeException
{
public:

    inline TreeNodeIsOff()
        : TreeException(TreeOFF)
    { }

}; // TreeOFF

class TreeNotOpen : public TreeException
{
public:

    inline TreeNotOpen()
        : TreeException(TreeNOT_OPEN)
    { }

}; // TreeNOT_OPEN

class TreeNoMethod : public TreeException
{
public:

    inline TreeNoMethod()
        : TreeException(TreeNOMETHOD)
    { }

}; // TreeNOMETHOD

class TreeBadRecord : public TreeException
{
public:

    inline TreeBadRecord()
        : TreeException(TreeBADRECORD)
    { }

}; // TreeBADRECORD

class TreeInvalidTreeStructure : public TreeException
{
public:

    inline TreeInvalidTreeStructure()
        : TreeException(TreeINVTREE)
    { }

}; // TreeINVTREE

class TreeDuplicateTag : public TreeException
{
public:

    inline TreeDuplicateTag()
        : TreeException(TreeDUPTAG)
    { }

}; // TreeDUPTAG

class TreeIllegalPageCount : public TreeException
{
public:

    inline TreeIllegalPageCount()
        : TreeException(TreeILLPAGCNT)
    { }

}; // TreeILLPAGCNT

class TreeMaxOpenForEdit : public TreeException
{
public:

    inline TreeMaxOpenForEdit()
        : TreeException(TreeMAXOPENEDIT)
    { }

}; // TreeMAXOPENEDIT

class TreeNoData : public TreeException
{
public:

    inline TreeNoData()
        : TreeException(TreeNODATA)
    { }

}; // TreeNODATA

class TreeNotOpenForEdit : public TreeException
{
public:

    inline TreeNotOpenForEdit()
        : TreeException(TreeNOEDIT)
    { }

}; // TreeNOEDIT

class TreeNodeHasChildren : public TreeException
{
public:

    inline TreeNodeHasChildren()
        : TreeException(TreeNOTCHILDLESS)
    { }

}; // TreeNOTCHILDLESS

class TreeInvalidPath : public TreeException
{
public:

    inline TreeInvalidPath()
        : TreeException(TreeINVPATH)
    { }

}; // TreeINVPATH

class TreeIllegalItem : public TreeException
{
public:

    inline TreeIllegalItem()
        : TreeException(TreeILLEGAL_ITEM)
    { }

}; // TreeILLEGAL_ITEM

class TreeBufferOverflow : public TreeException
{
public:

    inline TreeBufferOverflow()
        : TreeException(TreeBUFFEROVF)
    { }

}; // TreeBUFFEROVF

class TreeUnsupportedClass : public TreeException
{
public:

    inline TreeUnsupportedClass()
        : TreeException(TreeUNSPRTCLASS)
    { }

}; // TreeUNSPRTCLASS

class TreeConglomFull : public TreeException
{
public:

    inline TreeConglomFull()
        : TreeException(TreeCONGLOMFULL)
    { }

}; // TreeCONGLOMFULL

class TreeConglomNotFull : public TreeException
{
public:

    inline TreeConglomNotFull()
        : TreeException(TreeCONGLOM_NOT_FULL)
    { }

}; // TreeCONGLOM_NOT_FULL

class TreeUnresolved : public TreeException
{
public:

    inline TreeUnresolved()
        : TreeException(TreeUNRESOLVED)
    { }

}; // TreeUNRESOLVED

class TreeInvalidDataFormatClass : public TreeException
{
public:

    inline TreeInvalidDataFormatClass()
        : TreeException(TreeINVDFFCLASS)
    { }

}; // TreeINVDFFCLASS

class TreeInvalidRecordType : public TreeException
{
public:

    inline TreeInvalidRecordType()
        : TreeException(TreeINVRECTYP)
    { }

}; // TreeINVRECTYP

class TreeNodeNameTooLong : public TreeException
{
public:

    inline TreeNodeNameTooLong()
        : TreeException(TreeNODNAMLEN)
    { }

}; // TreeNODNAMLEN

class TreeTagNameTooLong : public TreeException
{
public:

    inline TreeTagNameTooLong()
        : TreeException(TreeTAGNAMLEN)
    { }

}; // TreeTAGNAMLEN

class TreeNeedWriteFirst : public TreeException
{
public:

    inline TreeNeedWriteFirst()
        : TreeException(TreeWRITEFIRST)
    { }

}; // TreeWRITEFIRST

class TreeNotConglom : public TreeException
{
public:

    inline TreeNotConglom()
        : TreeException(TreeNOT_CONGLOM)
    { }

}; // TreeNOT_CONGLOM

class TreeUnsupportedArrayType : public TreeException
{
public:

    inline TreeUnsupportedArrayType()
        : TreeException(TreeUNSUPARRDTYPE)
    { }

}; // TreeUNSUPARRDTYPE

class TreeNodeHasMembers : public TreeException
{
public:

    inline TreeNodeHasMembers()
        : TreeException(TreeNOTMEMBERLESS)
    { }

}; // TreeNOTMEMBERLESS

class TreeInvalidSubtreeNode : public TreeException
{
public:

    inline TreeInvalidSubtreeNode()
        : TreeException(TreeNOTSON)
    { }

}; // TreeNOTSON

class TreeNodeWriteOnce : public TreeException
{
public:

    inline TreeNodeWriteOnce()
        : TreeException(TreeNOOVERWRITE)
    { }

}; // TreeNOOVERWRITE

class TreeInvalidPutUsage : public TreeException
{
public:

    inline TreeInvalidPutUsage()
        : TreeException(TreeINVDTPUSG)
    { }

}; // TreeINVDTPUSG

class TreeNotPermittedWhileEditing : public TreeException
{
public:

    inline TreeNotPermittedWhileEditing()
        : TreeException(TreeEDITING)
    { }

}; // TreeEDITING

class TreeNoWriteModel : public TreeException
{
public:

    inline TreeNoWriteModel()
        : TreeException(TreeNOWRITEMODEL)
    { }

}; // TreeNOWRITEMODEL

class TreeNoWriteShot : public TreeException
{
public:

    inline TreeNoWriteShot()
        : TreeException(TreeNOWRITESHOT)
    { }

}; // TreeNOWRITESHOT

class TreeNoPathDefined : public TreeException
{
public:

    inline TreeNoPathDefined()
        : TreeException(TreeNOLOG)
    { }

}; // TreeNOLOG

class TreeReadOnly : public TreeException
{
public:

    inline TreeReadOnly()
        : TreeException(TreeREADONLY)
    { }

}; // TreeREADONLY

class TreeRecordReadError : public TreeException
{
public:

    inline TreeRecordReadError()
        : TreeException(TreeREADERR)
    { }

}; // TreeREADERR

class TreeNotInList : public TreeException
{
public:

    inline TreeNotInList()
        : TreeException(TreeNOT_IN_LIST)
    { }

}; // TreeNOT_IN_LIST

class TreeCancel : public TreeException
{
public:

    inline TreeCancel()
        : TreeException(TreeCANCEL)
    { }

}; // TreeCANCEL

class TreeUnsupportedThickOperation : public TreeException
{
public:

    inline TreeUnsupportedThickOperation()
        : TreeException(TreeUNSUPTHICKOP)
    { }

}; // TreeUNSUPTHICKOP

class TreeFailure : public TreeException
{
public:

    inline TreeFailure()
        : TreeException(TreeFAILURE)
    { }

}; // TreeFAILURE

class TreeFileNotFound : public TreeException
{
public:

    inline TreeFileNotFound()
        : TreeException(TreeFILE_NOT_FOUND)
    { }

}; // TreeFILE_NOT_FOUND

class TreeLockFailure : public TreeException
{
public:

    inline TreeLockFailure()
        : TreeException(TreeLOCK_FAILURE)
    { }

}; // TreeLOCK_FAILURE

class TreeNoSegments : public TreeException
{
public:

    inline TreeNoSegments()
        : TreeException(TreeNOSEGMENTS)
    { }

}; // TreeNOSEGMENTS

class TreeInvalidSegmentDataType : public TreeException
{
public:

    inline TreeInvalidSegmentDataType()
        : TreeException(TreeINVDTYPE)
    { }

}; // TreeINVDTYPE

class TreeInvalidSegmentShape : public TreeException
{
public:

    inline TreeInvalidSegmentShape()
        : TreeException(TreeINVSHAPE)
    { }

}; // TreeINVSHAPE

class TreeInvalidShot : public TreeException
{
public:

    inline TreeInvalidShot()
        : TreeException(TreeINVSHOT)
    { }

}; // TreeINVSHOT

class TreeInvalidTag : public TreeException
{
public:

    inline TreeInvalidTag()
        : TreeException(TreeINVTAG)
    { }

}; // TreeINVTAG

class TreeNoPath : public TreeException
{
public:

    inline TreeNoPath()
        : TreeException(TreeNOPATH)
    { }

}; // TreeNOPATH

class TreeTreeFileReadError : public TreeException
{
public:

    inline TreeTreeFileReadError()
        : TreeException(TreeTREEFILEREADERR)
    { }

}; // TreeTREEFILEREADERR

class TreeMemoryAllocateError : public TreeException
{
public:

    inline TreeMemoryAllocateError()
        : TreeException(TreeMEMERR)
    { }

}; // TreeMEMERR

class TreeNoCurrentShot : public TreeException
{
public:

    inline TreeNoCurrentShot()
        : TreeException(TreeNOCURRENT)
    { }

}; // TreeNOCURRENT

class TreeOpenReadWriteFailed : public TreeException
{
public:

    inline TreeOpenReadWriteFailed()
        : TreeException(TreeFOPENW)
    { }

}; // TreeFOPENW

class TreeOpenReadOnlyFailed : public TreeException
{
public:

    inline TreeOpenReadOnlyFailed()
        : TreeException(TreeFOPENR)
    { }

}; // TreeFOPENR

class TreeCreateFailed : public TreeException
{
public:

    inline TreeCreateFailed()
        : TreeException(TreeFCREATE)
    { }

}; // TreeFCREATE

class TreeConnectFailed : public TreeException
{
public:

    inline TreeConnectFailed()
        : TreeException(TreeCONNECTFAIL)
    { }

}; // TreeCONNECTFAIL

class TreeNCIWriteError : public TreeException
{
public:

    inline TreeNCIWriteError()
        : TreeException(TreeNCIWRITE)
    { }

}; // TreeNCIWRITE

class TreeDeleteFailed : public TreeException
{
public:

    inline TreeDeleteFailed()
        : TreeException(TreeDELFAIL)
    { }

}; // TreeDELFAIL

class TreeRenameFailed : public TreeException
{
public:

    inline TreeRenameFailed()
        : TreeException(TreeRENFAIL)
    { }

}; // TreeRENFAIL

class TreeEmpty : public TreeException
{
public:

    inline TreeEmpty()
        : TreeException(TreeEMPTY)
    { }

}; // TreeEMPTY

class TreeParseError : public TreeException
{
public:

    inline TreeParseError()
        : TreeException(TreePARSEERR)
    { }

}; // TreePARSEERR

class TreeNCIReadError : public TreeException
{
public:

    inline TreeNCIReadError()
        : TreeException(TreeNCIREAD)
    { }

}; // TreeNCIREAD

class TreeNoVersion : public TreeException
{
public:

    inline TreeNoVersion()
        : TreeException(TreeNOVERSION)
    { }

}; // TreeNOVERSION

class TreeDatafileReadError : public TreeException
{
public:

    inline TreeDatafileReadError()
        : TreeException(TreeDFREAD)
    { }

}; // TreeDFREAD

class TreeFileCloseError : public TreeException
{
public:

    inline TreeFileCloseError()
        : TreeException(TreeCLOSEERR)
    { }

}; // TreeCLOSEERR

class TreeFileOverwriteError : public TreeException
{
public:

    inline TreeFileOverwriteError()
        : TreeException(TreeMOVEERROR)
    { }

}; // TreeMOVEERROR

class TreeOpenFileForEditError : public TreeException
{
public:

    inline TreeOpenFileForEditError()
        : TreeException(TreeOPENEDITERR)
    { }

}; // TreeOPENEDITERR

class TreeOpenReadOnly : public TreeException
{
public:

    inline TreeOpenReadOnly()
        : TreeException(TreeREADONLY_TREE)
    { }

}; // TreeREADONLY_TREE

class TreeWriteTreeFileError : public TreeException
{
public:

    inline TreeWriteTreeFileError()
        : TreeException(TreeWRITETREEERR)
    { }

}; // TreeWRITETREEERR

class TreeWildcardInNodeSpecifier : public TreeException
{
public:

    inline TreeWildcardInNodeSpecifier()
        : TreeException(TreeNOWILD)
    { }

}; // TreeNOWILD

class TdiException : public MDSplusException
{
public:

    inline TdiException(int status)
        : MDSplusException(status)
    { }

}; // TdiException

class TdiMissingBreak : public TdiException
{
public:

    inline TdiMissingBreak()
        : TdiException(TdiBREAK)
    { }

}; // TdiBREAK

class TdiMissingCase : public TdiException
{
public:

    inline TdiMissingCase()
        : TdiException(TdiCASE)
    { }

}; // TdiCASE

class TdiMissingContinue : public TdiException
{
public:

    inline TdiMissingContinue()
        : TdiException(TdiCONTINUE)
    { }

}; // TdiCONTINUE

class TdiExtraneousCharacters : public TdiException
{
public:

    inline TdiExtraneousCharacters()
        : TdiException(TdiEXTRANEOUS)
    { }

}; // TdiEXTRANEOUS

class TdiExtraneousReturn : public TdiException
{
public:

    inline TdiExtraneousReturn()
        : TdiException(TdiRETURN)
    { }

}; // TdiRETURN

class TdiAbort : public TdiException
{
public:

    inline TdiAbort()
        : TdiException(TdiABORT)
    { }

}; // TdiABORT

class TdiBadIndex : public TdiException
{
public:

    inline TdiBadIndex()
        : TdiException(TdiBAD_INDEX)
    { }

}; // TdiBAD_INDEX

class TdiBadPunctuation : public TdiException
{
public:

    inline TdiBadPunctuation()
        : TdiException(TdiBOMB)
    { }

}; // TdiBOMB

class TdiTooManyArguments : public TdiException
{
public:

    inline TdiTooManyArguments()
        : TdiException(TdiEXTRA_ARG)
    { }

}; // TdiEXTRA_ARG

class TdiMissingGotoLabel : public TdiException
{
public:

    inline TdiMissingGotoLabel()
        : TdiException(TdiGOTO)
    { }

}; // TdiGOTO

class TdiInvalidClass : public TdiException
{
public:

    inline TdiInvalidClass()
        : TdiException(TdiINVCLADSC)
    { }

}; // TdiINVCLADSC

class TdiInvalidClassForDataType : public TdiException
{
public:

    inline TdiInvalidClassForDataType()
        : TdiException(TdiINVCLADTY)
    { }

}; // TdiINVCLADTY

class TdiInvalidDataType : public TdiException
{
public:

    inline TdiInvalidDataType()
        : TdiException(TdiINVDTYDSC)
    { }

}; // TdiINVDTYDSC

class TdiInvalidOpcode : public TdiException
{
public:

    inline TdiInvalidOpcode()
        : TdiException(TdiINV_OPC)
    { }

}; // TdiINV_OPC

class TdiInvalidSize : public TdiException
{
public:

    inline TdiInvalidSize()
        : TdiException(TdiINV_SIZE)
    { }

}; // TdiINV_SIZE

class TdiArgumentMismatch : public TdiException
{
public:

    inline TdiArgumentMismatch()
        : TdiException(TdiMISMATCH)
    { }

}; // TdiMISMATCH

class TdiMissingArgument : public TdiException
{
public:

    inline TdiMissingArgument()
        : TdiException(TdiMISS_ARG)
    { }

}; // TdiMISS_ARG

class TdiTooManyDimensions : public TdiException
{
public:

    inline TdiTooManyDimensions()
        : TdiException(TdiNDIM_OVER)
    { }

}; // TdiNDIM_OVER

class TdiComplexUnsupported : public TdiException
{
public:

    inline TdiComplexUnsupported()
        : TdiException(TdiNO_CMPLX)
    { }

}; // TdiNO_CMPLX

class TdiOpcodeUnsupported : public TdiException
{
public:

    inline TdiOpcodeUnsupported()
        : TdiException(TdiNO_OPC)
    { }

}; // TdiNO_OPC

class TdiNoOutputPointer : public TdiException
{
public:

    inline TdiNoOutputPointer()
        : TdiException(TdiNO_OUTPTR)
    { }

}; // TdiNO_OUTPTR

class TdiNoSelfPointer : public TdiException
{
public:

    inline TdiNoSelfPointer()
        : TdiException(TdiNO_SELF_PTR)
    { }

}; // TdiNO_SELF_PTR

class TdiNotANumber : public TdiException
{
public:

    inline TdiNotANumber()
        : TdiException(TdiNOT_NUMBER)
    { }

}; // TdiNOT_NUMBER

class TdiNullPointer : public TdiException
{
public:

    inline TdiNullPointer()
        : TdiException(TdiNULL_PTR)
    { }

}; // TdiNULL_PTR

class TdiRecursionLimitReached : public TdiException
{
public:

    inline TdiRecursionLimitReached()
        : TdiException(TdiRECURSIVE)
    { }

}; // TdiRECURSIVE

class TdiSignalDimensionMismatch : public TdiException
{
public:

    inline TdiSignalDimensionMismatch()
        : TdiException(TdiSIG_DIM)
    { }

}; // TdiSIG_DIM

class TdiSyntaxError : public TdiException
{
public:

    inline TdiSyntaxError()
        : TdiException(TdiSYNTAX)
    { }

}; // TdiSYNTAX

class TdiPrecisionLoss : public TdiException
{
public:

    inline TdiPrecisionLoss()
        : TdiException(TdiTOO_BIG)
    { }

}; // TdiTOO_BIG

class TdiUnbalanced : public TdiException
{
public:

    inline TdiUnbalanced()
        : TdiException(TdiUNBALANCE)
    { }

}; // TdiUNBALANCE

class TdiUnknownVariable : public TdiException
{
public:

    inline TdiUnknownVariable()
        : TdiException(TdiUNKNOWN_VAR)
    { }

}; // TdiUNKNOWN_VAR

class TdiStringTooLong : public TdiException
{
public:

    inline TdiStringTooLong()
        : TdiException(TdiSTRTOOLON)
    { }

}; // TdiSTRTOOLON

class TdiTimeout : public TdiException
{
public:

    inline TdiTimeout()
        : TdiException(TdiTIMEOUT)
    { }

}; // TdiTIMEOUT

class ApdException : public MDSplusException
{
public:

    inline ApdException(int status)
        : MDSplusException(status)
    { }

}; // ApdException

class ApdAppendError : public ApdException
{
public:

    inline ApdAppendError()
        : ApdException(ApdAPD_APPEND)
    { }

}; // ApdAPD_APPEND

class ApdInvalidKeyValuePair : public ApdException
{
public:

    inline ApdInvalidKeyValuePair()
        : ApdException(ApdDICT_KEYVALPAIR)
    { }

}; // ApdDICT_KEYVALPAIR

class ApdKeyNotScalar : public ApdException
{
public:

    inline ApdKeyNotScalar()
        : ApdException(ApdDICT_KEYCLS)
    { }

}; // ApdDICT_KEYCLS

class ServerException : public MDSplusException
{
public:

    inline ServerException(int status)
        : MDSplusException(status)
    { }

}; // ServerException

class ServerNotDispatched : public ServerException
{
public:

    inline ServerNotDispatched()
        : ServerException(ServerNOT_DISPATCHED)
    { }

}; // ServerNOT_DISPATCHED

class ServerInvalidDependency : public ServerException
{
public:

    inline ServerInvalidDependency()
        : ServerException(ServerINVALID_DEPENDENCY)
    { }

}; // ServerINVALID_DEPENDENCY

class ServerCantHappen : public ServerException
{
public:

    inline ServerCantHappen()
        : ServerException(ServerCANT_HAPPEN)
    { }

}; // ServerCANT_HAPPEN

class ServerInvalidShot : public ServerException
{
public:

    inline ServerInvalidShot()
        : ServerException(ServerINVSHOT)
    { }

}; // ServerINVSHOT

class ServerAborted : public ServerException
{
public:

    inline ServerAborted()
        : ServerException(ServerABORT)
    { }

}; // ServerABORT

class ServerPathDown : public ServerException
{
public:

    inline ServerPathDown()
        : ServerException(ServerPATH_DOWN)
    { }

}; // ServerPATH_DOWN

class ServerSocketAddressError : public ServerException
{
public:

    inline ServerSocketAddressError()
        : ServerException(ServerSOCKET_ADDR_ERROR)
    { }

}; // ServerSOCKET_ADDR_ERROR

class ServerInvalidActionOperation : public ServerException
{
public:

    inline ServerInvalidActionOperation()
        : ServerException(ServerINVALID_ACTION_OPERATION)
    { }

}; // ServerINVALID_ACTION_OPERATION

class DevException : public MDSplusException
{
public:

    inline DevException(int status)
        : MDSplusException(status)
    { }

}; // DevException

class DevBadEndIndex : public DevException
{
public:

    inline DevBadEndIndex()
        : DevException(DevBAD_ENDIDX)
    { }

}; // DevBAD_ENDIDX

class DevBadFilter : public DevException
{
public:

    inline DevBadFilter()
        : DevException(DevBAD_FILTER)
    { }

}; // DevBAD_FILTER

class DevBadFrequency : public DevException
{
public:

    inline DevBadFrequency()
        : DevException(DevBAD_FREQ)
    { }

}; // DevBAD_FREQ

class DevBadGain : public DevException
{
public:

    inline DevBadGain()
        : DevException(DevBAD_GAIN)
    { }

}; // DevBAD_GAIN

class DevBadHeader : public DevException
{
public:

    inline DevBadHeader()
        : DevException(DevBAD_HEADER)
    { }

}; // DevBAD_HEADER

class DevBadHeaderIndex : public DevException
{
public:

    inline DevBadHeaderIndex()
        : DevException(DevBAD_HEADER_IDX)
    { }

}; // DevBAD_HEADER_IDX

class DevBadMemories : public DevException
{
public:

    inline DevBadMemories()
        : DevException(DevBAD_MEMORIES)
    { }

}; // DevBAD_MEMORIES

class DevBadMode : public DevException
{
public:

    inline DevBadMode()
        : DevException(DevBAD_MODE)
    { }

}; // DevBAD_MODE

class DevBadName : public DevException
{
public:

    inline DevBadName()
        : DevException(DevBAD_NAME)
    { }

}; // DevBAD_NAME

class DevBadOffset : public DevException
{
public:

    inline DevBadOffset()
        : DevException(DevBAD_OFFSET)
    { }

}; // DevBAD_OFFSET

class DevBadStartIndex : public DevException
{
public:

    inline DevBadStartIndex()
        : DevException(DevBAD_STARTIDX)
    { }

}; // DevBAD_STARTIDX

class DevNotTriggered : public DevException
{
public:

    inline DevNotTriggered()
        : DevException(DevNOT_TRIGGERED)
    { }

}; // DevNOT_TRIGGERED

class DevFrequencyTooHigh : public DevException
{
public:

    inline DevFrequencyTooHigh()
        : DevException(DevFREQ_TOO_HIGH)
    { }

}; // DevFREQ_TOO_HIGH

class DevInvalidNumberOfChannels : public DevException
{
public:

    inline DevInvalidNumberOfChannels()
        : DevException(DevINVALID_NOC)
    { }

}; // DevINVALID_NOC

class DevRangeMismatch : public DevException
{
public:

    inline DevRangeMismatch()
        : DevException(DevRANGE_MISMATCH)
    { }

}; // DevRANGE_MISMATCH

class DevCAMACIOError : public DevException
{
public:

    inline DevCAMACIOError()
        : DevException(DevCAMACERR)
    { }

}; // DevCAMACERR

class DevBadVerbs : public DevException
{
public:

    inline DevBadVerbs()
        : DevException(DevBAD_VERBS)
    { }

}; // DevBAD_VERBS

class DevBadCommands : public DevException
{
public:

    inline DevBadCommands()
        : DevException(DevBAD_COMMANDS)
    { }

}; // DevBAD_COMMANDS

class DevCAMACAdressNotRecognized : public DevException
{
public:

    inline DevCAMACAdressNotRecognized()
        : DevException(DevCAM_ADNR)
    { }

}; // DevCAM_ADNR

class DevCAMACError : public DevException
{
public:

    inline DevCAMACError()
        : DevException(DevCAM_ERR)
    { }

}; // DevCAM_ERR

class DevCAMACLostSyncError : public DevException
{
public:

    inline DevCAMACLostSyncError()
        : DevException(DevCAM_LOSYNC)
    { }

}; // DevCAM_LOSYNC

class DevCAMACLongitudinalParityError : public DevException
{
public:

    inline DevCAMACLongitudinalParityError()
        : DevException(DevCAM_LPE)
    { }

}; // DevCAM_LPE

class DevCAMACHighwayTimeoutError : public DevException
{
public:

    inline DevCAMACHighwayTimeoutError()
        : DevException(DevCAM_TMO)
    { }

}; // DevCAM_TMO

class DevCAMACTransverseParityError : public DevException
{
public:

    inline DevCAMACTransverseParityError()
        : DevException(DevCAM_TPE)
    { }

}; // DevCAM_TPE

class DevCAMACSerialTransmissionError : public DevException
{
public:

    inline DevCAMACSerialTransmissionError()
        : DevException(DevCAM_STE)
    { }

}; // DevCAM_STE

class DevCAMACDerr : public DevException
{
public:

    inline DevCAMACDerr()
        : DevException(DevCAM_DERR)
    { }

}; // DevCAM_DERR

class DevCAMACDoneQ : public DevException
{
public:

    inline DevCAMACDoneQ()
        : DevException(DevCAM_SQ)
    { }

}; // DevCAM_SQ

class DevCAMACDoneNoQ : public DevException
{
public:

    inline DevCAMACDoneNoQ()
        : DevException(DevCAM_NOSQ)
    { }

}; // DevCAM_NOSQ

class DevCAMACDoneX : public DevException
{
public:

    inline DevCAMACDoneX()
        : DevException(DevCAM_SX)
    { }

}; // DevCAM_SX

class DevCAMACDoneNoX : public DevException
{
public:

    inline DevCAMACDoneNoX()
        : DevException(DevCAM_NOSX)
    { }

}; // DevCAM_NOSX

class DevInvalidSetup : public DevException
{
public:

    inline DevInvalidSetup()
        : DevException(DevINV_SETUP)
    { }

}; // DevINV_SETUP

class DevPyDeviceNotFound : public DevException
{
public:

    inline DevPyDeviceNotFound()
        : DevException(DevPYDEVICE_NOT_FOUND)
    { }

}; // DevPYDEVICE_NOT_FOUND

class DevPyInterfaceLibraryNotFound : public DevException
{
public:

    inline DevPyInterfaceLibraryNotFound()
        : DevException(DevPY_INTERFACE_LIBRARY_NOT_FOUND)
    { }

}; // DevPY_INTERFACE_LIBRARY_NOT_FOUND

class DevIOStuck : public DevException
{
public:

    inline DevIOStuck()
        : DevException(DevIO_STUCK)
    { }

}; // DevIO_STUCK

class DevUnknownState : public DevException
{
public:

    inline DevUnknownState()
        : DevException(DevUNKNOWN_STATE)
    { }

}; // DevUNKNOWN_STATE

class DevWrongTree : public DevException
{
public:

    inline DevWrongTree()
        : DevException(DevWRONG_TREE)
    { }

}; // DevWRONG_TREE

class DevWrongPath : public DevException
{
public:

    inline DevWrongPath()
        : DevException(DevWRONG_PATH)
    { }

}; // DevWRONG_PATH

class DevWrongShot : public DevException
{
public:

    inline DevWrongShot()
        : DevException(DevWRONG_SHOT)
    { }

}; // DevWRONG_SHOT

class DevOffline : public DevException
{
public:

    inline DevOffline()
        : DevException(DevOFFLINE)
    { }

}; // DevOFFLINE

class DevTriggeredNotStored : public DevException
{
public:

    inline DevTriggeredNotStored()
        : DevException(DevTRIGGERED_NOT_STORED)
    { }

}; // DevTRIGGERED_NOT_STORED

class DevNoNameSpecified : public DevException
{
public:

    inline DevNoNameSpecified()
        : DevException(DevNO_NAME_SPECIFIED)
    { }

}; // DevNO_NAME_SPECIFIED

class DevBadActiveChannel : public DevException
{
public:

    inline DevBadActiveChannel()
        : DevException(DevBAD_ACTIVE_CHAN)
    { }

}; // DevBAD_ACTIVE_CHAN

class DevBadTriggerSource : public DevException
{
public:

    inline DevBadTriggerSource()
        : DevException(DevBAD_TRIG_SRC)
    { }

}; // DevBAD_TRIG_SRC

class DevBadClockSource : public DevException
{
public:

    inline DevBadClockSource()
        : DevException(DevBAD_CLOCK_SRC)
    { }

}; // DevBAD_CLOCK_SRC

class DevBadPreTrigger : public DevException
{
public:

    inline DevBadPreTrigger()
        : DevException(DevBAD_PRE_TRIG)
    { }

}; // DevBAD_PRE_TRIG

class DevBadPostTrigger : public DevException
{
public:

    inline DevBadPostTrigger()
        : DevException(DevBAD_POST_TRIG)
    { }

}; // DevBAD_POST_TRIG

class DevBadClockFrequency : public DevException
{
public:

    inline DevBadClockFrequency()
        : DevException(DevBAD_CLOCK_FREQ)
    { }

}; // DevBAD_CLOCK_FREQ

class DevTriggerFailed : public DevException
{
public:

    inline DevTriggerFailed()
        : DevException(DevTRIGGER_FAILED)
    { }

}; // DevTRIGGER_FAILED

class DevErrorReadingChannel : public DevException
{
public:

    inline DevErrorReadingChannel()
        : DevException(DevERROR_READING_CHANNEL)
    { }

}; // DevERROR_READING_CHANNEL

class DevErrorDoingInit : public DevException
{
public:

    inline DevErrorDoingInit()
        : DevException(DevERROR_DOING_INIT)
    { }

}; // DevERROR_DOING_INIT

class DevCannotLoadSettings : public DevException
{
public:

    inline DevCannotLoadSettings()
        : DevException(DevCANNOT_LOAD_SETTINGS)
    { }

}; // DevCANNOT_LOAD_SETTINGS

class DevCannotGetBoardState : public DevException
{
public:

    inline DevCannotGetBoardState()
        : DevException(DevCANNOT_GET_BOARD_STATE)
    { }

}; // DevCANNOT_GET_BOARD_STATE

class DevBadParameter : public DevException
{
public:

    inline DevBadParameter()
        : DevException(DevBAD_PARAMETER)
    { }

}; // DevBAD_PARAMETER

class DevCommunicationError : public DevException
{
public:

    inline DevCommunicationError()
        : DevException(DevCOMM_ERROR)
    { }

}; // DevCOMM_ERROR

class DevCameraNotFound : public DevException
{
public:

    inline DevCameraNotFound()
        : DevException(DevCAMERA_NOT_FOUND)
    { }

}; // DevCAMERA_NOT_FOUND

class DevNotAPyDevice : public DevException
{
public:

    inline DevNotAPyDevice()
        : DevException(DevNOT_A_PYDEVICE)
    { }

}; // DevNOT_A_PYDEVICE

} // namespace mdsplus

#endif // MDSPLUS_EXCEPTIONS_HPP
