//
// Copyright (c) 2025, Massachusetts Institute of Technology All rights reserved.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the “Software”), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#ifndef MDSPLUS_HPP
#define MDSPLUS_HPP

#include <algorithm>
#include <cassert>
#include <complex>
#include <cstdint>
#include <cstring>
#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <vector>

#if __has_include(<optional>)
    #include <optional>
#endif

#if __has_include(<span>)
    #include <span>
#endif

extern "C" {

    #include <camshr_messages.h>
    #include <dbidef.h>
    #include <mdsdcl_messages.h>
    #include <mdsdescrip.h>
    #include <mdsshr.h>
    #include <mdsshr_messages.h>
    #include <mitdevices_messages.h>
    #include <ncidef.h>
    #include <servershr_messages.h>
    #include <tcl_messages.h>
    #include <tdishr_messages.h>
    #include <treeshr.h>
    #include <treeshr_messages.h>

    int TdiConvert(mdsdsc_a_t * dsc, mdsdsc_a_t * convert);
    int TdiCall(dtype_t rtype, int narg, mdsdsc_t *list[], mdsdsc_xd_t *out_ptr);
    int _TdiIntrinsic(void **ctx, opcode_t opcode, int narg, mdsdsc_t *list[], mdsdsc_xd_t *out_ptr);
    int TdiIntrinsic(opcode_t opcode, int narg, mdsdsc_t *list[], mdsdsc_xd_t *out_ptr);

    // Needed for mdsdsc*_t, <dtypedef.h>, <classdef.h>

    int ConnectToMds(char *host);
    void DisconnectFromMds(int sockId);
    int SetCompressionLevel(int level);
    void FreeMessage(void *m);

    int SendArg(
        int id,
        unsigned char idx,
        char dtype,
        unsigned char nargs,
        unsigned short length,
        char ndims,
        int *dims,
        char *bytes
    );

    int GetAnswerInfoTO(
        int id,
        char *dtype,
        short *length,
        char *ndims,
        int *dims,
        int *numbytes,
        void **dptr,
        void **m,
        int timeout
    );

} // extern "C"

namespace mdsplus {

class MDSplusException : public std::runtime_error
{
public:

    inline MDSplusException()
        : std::runtime_error("Unknown Status Code")
    { }

    inline MDSplusException(int status)
        : std::runtime_error(MdsGetMsg(status))
        , _status(status)
    { }

    inline int getStatus() const {
        return _status;
    }

private:

    int _status;

}; // class MdsException

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

}; // LibInsufficientVirtualMemory

class LibInvalidArgument : public LibException
{
public:

    inline LibInvalidArgument()
        : LibException(LibINVARG)
    { }

}; // LibInvalidArgument

class LibInvalidStringDescriptor : public LibException
{
public:

    inline LibInvalidStringDescriptor()
        : LibException(LibINVSTRDES)
    { }

}; // LibInvalidStringDescriptor

class LibKeyNotFound : public LibException
{
public:

    inline LibKeyNotFound()
        : LibException(LibKEYNOTFOU)
    { }

}; // LibKeyNotFound

class LibNotFound : public LibException
{
public:

    inline LibNotFound()
        : LibException(LibNOTFOU)
    { }

}; // LibNotFound

class LibQueueEmpty : public LibException
{
public:

    inline LibQueueEmpty()
        : LibException(LibQUEWASEMP)
    { }

}; // LibQueueEmpty

class LibStringTruncated : public LibException
{
public:

    inline LibStringTruncated()
        : LibException(LibSTRTRU)
    { }

}; // LibStringTruncated

class StrException : public MDSplusException
{
public:

    inline StrException(int status)
        : MDSplusException(status)
    { }

}; // StrException

class StrMatch : public StrException
{
public:

    inline StrMatch()
        : StrException(StrMATCH)
    { }

}; // StrMatch

class StrNoMatch : public StrException
{
public:

    inline StrNoMatch()
        : StrException(StrNOMATCH)
    { }

}; // StrNoMatch

class StrNoElement : public StrException
{
public:

    inline StrNoElement()
        : StrException(StrNOELEM)
    { }

}; // StrNoElement

class StrInvalidDelimiter : public StrException
{
public:

    inline StrInvalidDelimiter()
        : StrException(StrINVDELIM)
    { }

}; // StrInvalidDelimiter

class StrStringTooLong : public StrException
{
public:

    inline StrStringTooLong()
        : StrException(StrSTRTOOLON)
    { }

}; // StrStringTooLong

class MDSplusWarning : public MDSplusException
{
public:

    inline MDSplusWarning()
        : MDSplusException(MDSplusWARNING)
    { }

}; // MDSplusWarning

class MDSplusSuccess : public MDSplusException
{
public:

    inline MDSplusSuccess()
        : MDSplusException(MDSplusSUCCESS)
    { }

}; // MDSplusSuccess

class MDSplusError : public MDSplusException
{
public:

    inline MDSplusError()
        : MDSplusException(MDSplusERROR)
    { }

}; // MDSplusError

class MDSplusFatal : public MDSplusException
{
public:

    inline MDSplusFatal()
        : MDSplusException(MDSplusFATAL)
    { }

}; // MDSplusFatal

class MDSplusSandbox : public MDSplusException
{
public:

    inline MDSplusSandbox()
        : MDSplusException(MDSplusSANDBOX)
    { }

}; // MDSplusSandbox

class SsException : public MDSplusException
{
public:

    inline SsException(int status)
        : MDSplusException(status)
    { }

}; // SsException

class SsSuccess : public SsException
{
public:

    inline SsSuccess()
        : SsException(SsSUCCESS)
    { }

}; // SsSuccess

class SsIntegerOverflow : public SsException
{
public:

    inline SsIntegerOverflow()
        : SsException(SsINTOVF)
    { }

}; // SsIntegerOverflow

class SsInternal : public SsException
{
public:

    inline SsInternal()
        : SsException(SsINTERNAL)
    { }

}; // SsInternal

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

}; // TdiMissingBreak

class TdiMissingCase : public TdiException
{
public:

    inline TdiMissingCase()
        : TdiException(TdiCASE)
    { }

}; // TdiMissingCase

class TdiMissingContinue : public TdiException
{
public:

    inline TdiMissingContinue()
        : TdiException(TdiCONTINUE)
    { }

}; // TdiMissingContinue

class TdiExtraneousCharacters : public TdiException
{
public:

    inline TdiExtraneousCharacters()
        : TdiException(TdiEXTRANEOUS)
    { }

}; // TdiExtraneousCharacters

class TdiExtraneousReturn : public TdiException
{
public:

    inline TdiExtraneousReturn()
        : TdiException(TdiRETURN)
    { }

}; // TdiExtraneousReturn

class TdiAbort : public TdiException
{
public:

    inline TdiAbort()
        : TdiException(TdiABORT)
    { }

}; // TdiAbort

class TdiBadIndex : public TdiException
{
public:

    inline TdiBadIndex()
        : TdiException(TdiBAD_INDEX)
    { }

}; // TdiBadIndex

class TdiBadPunctuation : public TdiException
{
public:

    inline TdiBadPunctuation()
        : TdiException(TdiBOMB)
    { }

}; // TdiBadPunctuation

class TdiTooManyArguments : public TdiException
{
public:

    inline TdiTooManyArguments()
        : TdiException(TdiEXTRA_ARG)
    { }

}; // TdiTooManyArguments

class TdiMissingGotoLabel : public TdiException
{
public:

    inline TdiMissingGotoLabel()
        : TdiException(TdiGOTO)
    { }

}; // TdiMissingGotoLabel

class TdiInvalidClass : public TdiException
{
public:

    inline TdiInvalidClass()
        : TdiException(TdiINVCLADSC)
    { }

}; // TdiInvalidClass

class TdiInvalidClassForDataType : public TdiException
{
public:

    inline TdiInvalidClassForDataType()
        : TdiException(TdiINVCLADTY)
    { }

}; // TdiInvalidClassForDataType

class TdiInvalidDataType : public TdiException
{
public:

    inline TdiInvalidDataType()
        : TdiException(TdiINVDTYDSC)
    { }

}; // TdiInvalidDataType

class TdiInvalidOpcode : public TdiException
{
public:

    inline TdiInvalidOpcode()
        : TdiException(TdiINV_OPC)
    { }

}; // TdiInvalidOpcode

class TdiInvalidSize : public TdiException
{
public:

    inline TdiInvalidSize()
        : TdiException(TdiINV_SIZE)
    { }

}; // TdiInvalidSize

class TdiArgumentMismatch : public TdiException
{
public:

    inline TdiArgumentMismatch()
        : TdiException(TdiMISMATCH)
    { }

}; // TdiArgumentMismatch

class TdiMissingArgument : public TdiException
{
public:

    inline TdiMissingArgument()
        : TdiException(TdiMISS_ARG)
    { }

}; // TdiMissingArgument

class TdiTooManyDimensions : public TdiException
{
public:

    inline TdiTooManyDimensions()
        : TdiException(TdiNDIM_OVER)
    { }

}; // TdiTooManyDimensions

class TdiComplexUnsupported : public TdiException
{
public:

    inline TdiComplexUnsupported()
        : TdiException(TdiNO_CMPLX)
    { }

}; // TdiComplexUnsupported

class TdiOpcodeUnsupported : public TdiException
{
public:

    inline TdiOpcodeUnsupported()
        : TdiException(TdiNO_OPC)
    { }

}; // TdiOpcodeUnsupported

class TdiNoOutputPointer : public TdiException
{
public:

    inline TdiNoOutputPointer()
        : TdiException(TdiNO_OUTPTR)
    { }

}; // TdiNoOutputPointer

class TdiNoSelfPointer : public TdiException
{
public:

    inline TdiNoSelfPointer()
        : TdiException(TdiNO_SELF_PTR)
    { }

}; // TdiNoSelfPointer

class TdiNotANumber : public TdiException
{
public:

    inline TdiNotANumber()
        : TdiException(TdiNOT_NUMBER)
    { }

}; // TdiNotANumber

class TdiNullPointer : public TdiException
{
public:

    inline TdiNullPointer()
        : TdiException(TdiNULL_PTR)
    { }

}; // TdiNullPointer

class TdiRecursionLimitReached : public TdiException
{
public:

    inline TdiRecursionLimitReached()
        : TdiException(TdiRECURSIVE)
    { }

}; // TdiRecursionLimitReached

class TdiSignalDimensionMismatch : public TdiException
{
public:

    inline TdiSignalDimensionMismatch()
        : TdiException(TdiSIG_DIM)
    { }

}; // TdiSignalDimensionMismatch

class TdiSyntaxError : public TdiException
{
public:

    inline TdiSyntaxError()
        : TdiException(TdiSYNTAX)
    { }

}; // TdiSyntaxError

class TdiPrecisionLoss : public TdiException
{
public:

    inline TdiPrecisionLoss()
        : TdiException(TdiTOO_BIG)
    { }

}; // TdiPrecisionLoss

class TdiUnbalanced : public TdiException
{
public:

    inline TdiUnbalanced()
        : TdiException(TdiUNBALANCE)
    { }

}; // TdiUnbalanced

class TdiUnknownVariable : public TdiException
{
public:

    inline TdiUnknownVariable()
        : TdiException(TdiUNKNOWN_VAR)
    { }

}; // TdiUnknownVariable

class TdiStringTooLong : public TdiException
{
public:

    inline TdiStringTooLong()
        : TdiException(TdiSTRTOOLON)
    { }

}; // TdiStringTooLong

class TdiTimeout : public TdiException
{
public:

    inline TdiTimeout()
        : TdiException(TdiTIMEOUT)
    { }

}; // TdiTimeout

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

}; // ApdAppendError

class ApdInvalidKeyValuePair : public ApdException
{
public:

    inline ApdInvalidKeyValuePair()
        : ApdException(ApdDICT_KEYVALPAIR)
    { }

}; // ApdInvalidKeyValuePair

class ApdKeyNotScalar : public ApdException
{
public:

    inline ApdKeyNotScalar()
        : ApdException(ApdDICT_KEYCLS)
    { }

}; // ApdKeyNotScalar

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

}; // DevBadEndIndex

class DevBadFilter : public DevException
{
public:

    inline DevBadFilter()
        : DevException(DevBAD_FILTER)
    { }

}; // DevBadFilter

class DevBadFrequency : public DevException
{
public:

    inline DevBadFrequency()
        : DevException(DevBAD_FREQ)
    { }

}; // DevBadFrequency

class DevBadGain : public DevException
{
public:

    inline DevBadGain()
        : DevException(DevBAD_GAIN)
    { }

}; // DevBadGain

class DevBadHeader : public DevException
{
public:

    inline DevBadHeader()
        : DevException(DevBAD_HEADER)
    { }

}; // DevBadHeader

class DevBadHeaderIndex : public DevException
{
public:

    inline DevBadHeaderIndex()
        : DevException(DevBAD_HEADER_IDX)
    { }

}; // DevBadHeaderIndex

class DevBadMemories : public DevException
{
public:

    inline DevBadMemories()
        : DevException(DevBAD_MEMORIES)
    { }

}; // DevBadMemories

class DevBadMode : public DevException
{
public:

    inline DevBadMode()
        : DevException(DevBAD_MODE)
    { }

}; // DevBadMode

class DevBadName : public DevException
{
public:

    inline DevBadName()
        : DevException(DevBAD_NAME)
    { }

}; // DevBadName

class DevBadOffset : public DevException
{
public:

    inline DevBadOffset()
        : DevException(DevBAD_OFFSET)
    { }

}; // DevBadOffset

class DevBadStartIndex : public DevException
{
public:

    inline DevBadStartIndex()
        : DevException(DevBAD_STARTIDX)
    { }

}; // DevBadStartIndex

class DevNotTriggered : public DevException
{
public:

    inline DevNotTriggered()
        : DevException(DevNOT_TRIGGERED)
    { }

}; // DevNotTriggered

class DevFrequencyTooHigh : public DevException
{
public:

    inline DevFrequencyTooHigh()
        : DevException(DevFREQ_TOO_HIGH)
    { }

}; // DevFrequencyTooHigh

class DevInvalidNumberOfChannels : public DevException
{
public:

    inline DevInvalidNumberOfChannels()
        : DevException(DevINVALID_NOC)
    { }

}; // DevInvalidNumberOfChannels

class DevRangeMismatch : public DevException
{
public:

    inline DevRangeMismatch()
        : DevException(DevRANGE_MISMATCH)
    { }

}; // DevRangeMismatch

class DevBadVerbs : public DevException
{
public:

    inline DevBadVerbs()
        : DevException(DevBAD_VERBS)
    { }

}; // DevBadVerbs

class DevBadCommands : public DevException
{
public:

    inline DevBadCommands()
        : DevException(DevBAD_COMMANDS)
    { }

}; // DevBadCommands

class DevInvalidSetup : public DevException
{
public:

    inline DevInvalidSetup()
        : DevException(DevINV_SETUP)
    { }

}; // DevInvalidSetup

class DevPyDeviceNotFound : public DevException
{
public:

    inline DevPyDeviceNotFound()
        : DevException(DevPYDEVICE_NOT_FOUND)
    { }

}; // DevPyDeviceNotFound

class DevPyInterfaceLibraryNotFound : public DevException
{
public:

    inline DevPyInterfaceLibraryNotFound()
        : DevException(DevPY_INTERFACE_LIBRARY_NOT_FOUND)
    { }

}; // DevPyInterfaceLibraryNotFound

class DevIOStuck : public DevException
{
public:

    inline DevIOStuck()
        : DevException(DevIO_STUCK)
    { }

}; // DevIOStuck

class DevUnknownState : public DevException
{
public:

    inline DevUnknownState()
        : DevException(DevUNKNOWN_STATE)
    { }

}; // DevUnknownState

class DevWrongTree : public DevException
{
public:

    inline DevWrongTree()
        : DevException(DevWRONG_TREE)
    { }

}; // DevWrongTree

class DevWrongPath : public DevException
{
public:

    inline DevWrongPath()
        : DevException(DevWRONG_PATH)
    { }

}; // DevWrongPath

class DevWrongShot : public DevException
{
public:

    inline DevWrongShot()
        : DevException(DevWRONG_SHOT)
    { }

}; // DevWrongShot

class DevOffline : public DevException
{
public:

    inline DevOffline()
        : DevException(DevOFFLINE)
    { }

}; // DevOffline

class DevTriggeredNotStored : public DevException
{
public:

    inline DevTriggeredNotStored()
        : DevException(DevTRIGGERED_NOT_STORED)
    { }

}; // DevTriggeredNotStored

class DevNoNameSpecified : public DevException
{
public:

    inline DevNoNameSpecified()
        : DevException(DevNO_NAME_SPECIFIED)
    { }

}; // DevNoNameSpecified

class DevBadActiveChannel : public DevException
{
public:

    inline DevBadActiveChannel()
        : DevException(DevBAD_ACTIVE_CHAN)
    { }

}; // DevBadActiveChannel

class DevBadTriggerSource : public DevException
{
public:

    inline DevBadTriggerSource()
        : DevException(DevBAD_TRIG_SRC)
    { }

}; // DevBadTriggerSource

class DevBadClockSource : public DevException
{
public:

    inline DevBadClockSource()
        : DevException(DevBAD_CLOCK_SRC)
    { }

}; // DevBadClockSource

class DevBadPreTrigger : public DevException
{
public:

    inline DevBadPreTrigger()
        : DevException(DevBAD_PRE_TRIG)
    { }

}; // DevBadPreTrigger

class DevBadPostTrigger : public DevException
{
public:

    inline DevBadPostTrigger()
        : DevException(DevBAD_POST_TRIG)
    { }

}; // DevBadPostTrigger

class DevBadClockFrequency : public DevException
{
public:

    inline DevBadClockFrequency()
        : DevException(DevBAD_CLOCK_FREQ)
    { }

}; // DevBadClockFrequency

class DevTriggerFailed : public DevException
{
public:

    inline DevTriggerFailed()
        : DevException(DevTRIGGER_FAILED)
    { }

}; // DevTriggerFailed

class DevErrorReadingChannel : public DevException
{
public:

    inline DevErrorReadingChannel()
        : DevException(DevERROR_READING_CHANNEL)
    { }

}; // DevErrorReadingChannel

class DevErrorDoingInit : public DevException
{
public:

    inline DevErrorDoingInit()
        : DevException(DevERROR_DOING_INIT)
    { }

}; // DevErrorDoingInit

class DevCannotLoadSettings : public DevException
{
public:

    inline DevCannotLoadSettings()
        : DevException(DevCANNOT_LOAD_SETTINGS)
    { }

}; // DevCannotLoadSettings

class DevCannotGetBoardState : public DevException
{
public:

    inline DevCannotGetBoardState()
        : DevException(DevCANNOT_GET_BOARD_STATE)
    { }

}; // DevCannotGetBoardState

class DevBadParameter : public DevException
{
public:

    inline DevBadParameter()
        : DevException(DevBAD_PARAMETER)
    { }

}; // DevBadParameter

class DevCommunicationError : public DevException
{
public:

    inline DevCommunicationError()
        : DevException(DevCOMM_ERROR)
    { }

}; // DevCommunicationError

class DevCameraNotFound : public DevException
{
public:

    inline DevCameraNotFound()
        : DevException(DevCAMERA_NOT_FOUND)
    { }

}; // DevCameraNotFound

class DevNotAPyDevice : public DevException
{
public:

    inline DevNotAPyDevice()
        : DevException(DevNOT_A_PYDEVICE)
    { }

}; // DevNotAPyDevice

class CamException : public MDSplusException
{
public:

    inline CamException(int status)
        : MDSplusException(status)
    { }

}; // CamException

class CamDoneQ : public CamException
{
public:

    inline CamDoneQ()
        : CamException(CamDONE_Q)
    { }

}; // CamDoneQ

class CamDoneNoQ : public CamException
{
public:

    inline CamDoneNoQ()
        : CamException(CamDONE_NOQ)
    { }

}; // CamDoneNoQ

class CamDoneNoX : public CamException
{
public:

    inline CamDoneNoX()
        : CamException(CamDONE_NOX)
    { }

}; // CamDoneNoX

class CamSerialTransmissionError : public CamException
{
public:

    inline CamSerialTransmissionError()
        : CamException(CamSERTRAERR)
    { }

}; // CamSerialTransmissionError

class CamSerialCrateControllerFailure : public CamException
{
public:

    inline CamSerialCrateControllerFailure()
        : CamException(CamSCCFAIL)
    { }

}; // CamSerialCrateControllerFailure

class CamOffline : public CamException
{
public:

    inline CamOffline()
        : CamException(CamOFFLINE)
    { }

}; // CamOffline

class MdsdclException : public MDSplusException
{
public:

    inline MdsdclException(int status)
        : MDSplusException(status)
    { }

}; // MdsdclException

class MdsdclSuccess : public MdsdclException
{
public:

    inline MdsdclSuccess()
        : MdsdclException(MdsdclSUCCESS)
    { }

}; // MdsdclSuccess

class MdsdclExit : public MdsdclException
{
public:

    inline MdsdclExit()
        : MdsdclException(MdsdclEXIT)
    { }

}; // MdsdclExit

class MdsdclError : public MdsdclException
{
public:

    inline MdsdclError()
        : MdsdclException(MdsdclERROR)
    { }

}; // MdsdclError

class MdsdclNormal : public MdsdclException
{
public:

    inline MdsdclNormal()
        : MdsdclException(MdsdclNORMAL)
    { }

}; // MdsdclNormal

class MdsdclPresent : public MdsdclException
{
public:

    inline MdsdclPresent()
        : MdsdclException(MdsdclPRESENT)
    { }

}; // MdsdclPresent

class MdsdclInvalidVerb : public MdsdclException
{
public:

    inline MdsdclInvalidVerb()
        : MdsdclException(MdsdclIVVERB)
    { }

}; // MdsdclInvalidVerb

class MdsdclAbsent : public MdsdclException
{
public:

    inline MdsdclAbsent()
        : MdsdclException(MdsdclABSENT)
    { }

}; // MdsdclAbsent

class MdsdclNegated : public MdsdclException
{
public:

    inline MdsdclNegated()
        : MdsdclException(MdsdclNEGATED)
    { }

}; // MdsdclNegated

class MdsdclNotNegatable : public MdsdclException
{
public:

    inline MdsdclNotNegatable()
        : MdsdclException(MdsdclNOTNEGATABLE)
    { }

}; // MdsdclNotNegatable

class MdsdclInvalidQualifier : public MdsdclException
{
public:

    inline MdsdclInvalidQualifier()
        : MdsdclException(MdsdclIVQUAL)
    { }

}; // MdsdclInvalidQualifier

class MdsdclPromptMore : public MdsdclException
{
public:

    inline MdsdclPromptMore()
        : MdsdclException(MdsdclPROMPT_MORE)
    { }

}; // MdsdclPromptMore

class MdsdclTooManyParameters : public MdsdclException
{
public:

    inline MdsdclTooManyParameters()
        : MdsdclException(MdsdclTOO_MANY_PRMS)
    { }

}; // MdsdclTooManyParameters

class MdsdclTooManyValues : public MdsdclException
{
public:

    inline MdsdclTooManyValues()
        : MdsdclException(MdsdclTOO_MANY_VALS)
    { }

}; // MdsdclTooManyValues

class MdsdclMissingValue : public MdsdclException
{
public:

    inline MdsdclMissingValue()
        : MdsdclException(MdsdclMISSING_VALUE)
    { }

}; // MdsdclMissingValue

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

}; // ServerNotDispatched

class ServerInvalidDependency : public ServerException
{
public:

    inline ServerInvalidDependency()
        : ServerException(ServerINVALID_DEPENDENCY)
    { }

}; // ServerInvalidDependency

class ServerCantHappen : public ServerException
{
public:

    inline ServerCantHappen()
        : ServerException(ServerCANT_HAPPEN)
    { }

}; // ServerCantHappen

class ServerInvalidShot : public ServerException
{
public:

    inline ServerInvalidShot()
        : ServerException(ServerINVSHOT)
    { }

}; // ServerInvalidShot

class ServerAborted : public ServerException
{
public:

    inline ServerAborted()
        : ServerException(ServerABORT)
    { }

}; // ServerAborted

class ServerPathDown : public ServerException
{
public:

    inline ServerPathDown()
        : ServerException(ServerPATH_DOWN)
    { }

}; // ServerPathDown

class ServerSocketAddressError : public ServerException
{
public:

    inline ServerSocketAddressError()
        : ServerException(ServerSOCKET_ADDR_ERROR)
    { }

}; // ServerSocketAddressError

class ServerInvalidActionOperation : public ServerException
{
public:

    inline ServerInvalidActionOperation()
        : ServerException(ServerINVALID_ACTION_OPERATION)
    { }

}; // ServerInvalidActionOperation

class TclException : public MDSplusException
{
public:

    inline TclException(int status)
        : MDSplusException(status)
    { }

}; // TclException

class TclNormal : public TclException
{
public:

    inline TclNormal()
        : TclException(TclNORMAL)
    { }

}; // TclNormal

class TclFailedEssential : public TclException
{
public:

    inline TclFailedEssential()
        : TclException(TclFAILED_ESSENTIAL)
    { }

}; // TclFailedEssential

class TclNoDispatchTable : public TclException
{
public:

    inline TclNoDispatchTable()
        : TclException(TclNO_DISPATCH_TABLE)
    { }

}; // TclNoDispatchTable

class TreeException : public MDSplusException
{
public:

    inline TreeException(int status)
        : MDSplusException(status)
    { }

}; // TreeException

class TreeNodeAlreadyOff : public TreeException
{
public:

    inline TreeNodeAlreadyOff()
        : TreeException(TreeALREADY_OFF)
    { }

}; // TreeNodeAlreadyOff

class TreeNodeAlreadyOn : public TreeException
{
public:

    inline TreeNodeAlreadyOn()
        : TreeException(TreeALREADY_ON)
    { }

}; // TreeNodeAlreadyOn

class TreeNodeAlreadyOpen : public TreeException
{
public:

    inline TreeNodeAlreadyOpen()
        : TreeException(TreeALREADY_OPEN)
    { }

}; // TreeNodeAlreadyOpen

class TreeNodeAlreadyThere : public TreeException
{
public:

    inline TreeNodeAlreadyThere()
        : TreeException(TreeALREADY_THERE)
    { }

}; // TreeNodeAlreadyThere

class TreeBadRecord : public TreeException
{
public:

    inline TreeBadRecord()
        : TreeException(TreeBADRECORD)
    { }

}; // TreeBadRecord

class TreeNodeAndParentOff : public TreeException
{
public:

    inline TreeNodeAndParentOff()
        : TreeException(TreeBOTH_OFF)
    { }

}; // TreeNodeAndParentOff

class TreeBufferOverflow : public TreeException
{
public:

    inline TreeBufferOverflow()
        : TreeException(TreeBUFFEROVF)
    { }

}; // TreeBufferOverflow

class TreeConglomFull : public TreeException
{
public:

    inline TreeConglomFull()
        : TreeException(TreeCONGLOMFULL)
    { }

}; // TreeConglomFull

class TreeConglomNotFull : public TreeException
{
public:

    inline TreeConglomNotFull()
        : TreeException(TreeCONGLOM_NOT_FULL)
    { }

}; // TreeConglomNotFull

class TreeContinuing : public TreeException
{
public:

    inline TreeContinuing()
        : TreeException(TreeCONTINUING)
    { }

}; // TreeContinuing

class TreeDuplicateTag : public TreeException
{
public:

    inline TreeDuplicateTag()
        : TreeException(TreeDUPTAG)
    { }

}; // TreeDuplicateTag

class TreeNotPermittedWhileEditing : public TreeException
{
public:

    inline TreeNotPermittedWhileEditing()
        : TreeException(TreeEDITING)
    { }

}; // TreeNotPermittedWhileEditing

class TreeIllegalItem : public TreeException
{
public:

    inline TreeIllegalItem()
        : TreeException(TreeILLEGAL_ITEM)
    { }

}; // TreeIllegalItem

class TreeIllegalPageCount : public TreeException
{
public:

    inline TreeIllegalPageCount()
        : TreeException(TreeILLPAGCNT)
    { }

}; // TreeIllegalPageCount

class TreeInvalidDataFormatClass : public TreeException
{
public:

    inline TreeInvalidDataFormatClass()
        : TreeException(TreeINVDFFCLASS)
    { }

}; // TreeInvalidDataFormatClass

class TreeInvalidPutUsage : public TreeException
{
public:

    inline TreeInvalidPutUsage()
        : TreeException(TreeINVDTPUSG)
    { }

}; // TreeInvalidPutUsage

class TreeInvalidPath : public TreeException
{
public:

    inline TreeInvalidPath()
        : TreeException(TreeINVPATH)
    { }

}; // TreeInvalidPath

class TreeInvalidRecordType : public TreeException
{
public:

    inline TreeInvalidRecordType()
        : TreeException(TreeINVRECTYP)
    { }

}; // TreeInvalidRecordType

class TreeInvalidTreeStructure : public TreeException
{
public:

    inline TreeInvalidTreeStructure()
        : TreeException(TreeINVTREE)
    { }

}; // TreeInvalidTreeStructure

class TreeMaxOpenForEdit : public TreeException
{
public:

    inline TreeMaxOpenForEdit()
        : TreeException(TreeMAXOPENEDIT)
    { }

}; // TreeMaxOpenForEdit

class TreeNew : public TreeException
{
public:

    inline TreeNew()
        : TreeException(TreeNEW)
    { }

}; // TreeNew

class TreeNoMoreNodes : public TreeException
{
public:

    inline TreeNoMoreNodes()
        : TreeException(TreeNMN)
    { }

}; // TreeNoMoreNodes

class TreeNoMoreTags : public TreeException
{
public:

    inline TreeNoMoreTags()
        : TreeException(TreeNMT)
    { }

}; // TreeNoMoreTags

class TreeNodeNotFound : public TreeException
{
public:

    inline TreeNodeNotFound()
        : TreeException(TreeNNF)
    { }

}; // TreeNodeNotFound

class TreeNoData : public TreeException
{
public:

    inline TreeNoData()
        : TreeException(TreeNODATA)
    { }

}; // TreeNoData

class TreeNodeNameTooLong : public TreeException
{
public:

    inline TreeNodeNameTooLong()
        : TreeException(TreeNODNAMLEN)
    { }

}; // TreeNodeNameTooLong

class TreeNotOpenForEdit : public TreeException
{
public:

    inline TreeNotOpenForEdit()
        : TreeException(TreeNOEDIT)
    { }

}; // TreeNotOpenForEdit

class TreeNoPathDefined : public TreeException
{
public:

    inline TreeNoPathDefined()
        : TreeException(TreeNOLOG)
    { }

}; // TreeNoPathDefined

class TreeNoMethod : public TreeException
{
public:

    inline TreeNoMethod()
        : TreeException(TreeNOMETHOD)
    { }

}; // TreeNoMethod

class TreeNodeWriteOnce : public TreeException
{
public:

    inline TreeNodeWriteOnce()
        : TreeException(TreeNOOVERWRITE)
    { }

}; // TreeNodeWriteOnce

class TreeSubtreesNotOpen : public TreeException
{
public:

    inline TreeSubtreesNotOpen()
        : TreeException(TreeNOTALLSUBS)
    { }

}; // TreeSubtreesNotOpen

class TreeNodeHasChildren : public TreeException
{
public:

    inline TreeNodeHasChildren()
        : TreeException(TreeNOTCHILDLESS)
    { }

}; // TreeNodeHasChildren

class TreeNotInList : public TreeException
{
public:

    inline TreeNotInList()
        : TreeException(TreeNOT_IN_LIST)
    { }

}; // TreeNotInList

class TreeNodeHasMembers : public TreeException
{
public:

    inline TreeNodeHasMembers()
        : TreeException(TreeNOTMEMBERLESS)
    { }

}; // TreeNodeHasMembers

class TreeInvalidSubtreeNode : public TreeException
{
public:

    inline TreeInvalidSubtreeNode()
        : TreeException(TreeNOTSON)
    { }

}; // TreeInvalidSubtreeNode

class TreeNotConglom : public TreeException
{
public:

    inline TreeNotConglom()
        : TreeException(TreeNOT_CONGLOM)
    { }

}; // TreeNotConglom

class TreeNotOpen : public TreeException
{
public:

    inline TreeNotOpen()
        : TreeException(TreeNOT_OPEN)
    { }

}; // TreeNotOpen

class TreeNoWriteModel : public TreeException
{
public:

    inline TreeNoWriteModel()
        : TreeException(TreeNOWRITEMODEL)
    { }

}; // TreeNoWriteModel

class TreeNoWriteShot : public TreeException
{
public:

    inline TreeNoWriteShot()
        : TreeException(TreeNOWRITESHOT)
    { }

}; // TreeNoWriteShot

class TreeNoContext : public TreeException
{
public:

    inline TreeNoContext()
        : TreeException(TreeNO_CONTEXT)
    { }

}; // TreeNoContext

class TreeNodeIsOff : public TreeException
{
public:

    inline TreeNodeIsOff()
        : TreeException(TreeOFF)
    { }

}; // TreeNodeIsOff

class TreeNodeIsOn : public TreeException
{
public:

    inline TreeNodeIsOn()
        : TreeException(TreeON)
    { }

}; // TreeNodeIsOn

class TreeOpen : public TreeException
{
public:

    inline TreeOpen()
        : TreeException(TreeOPEN)
    { }

}; // TreeOpen

class TreeOpenEdit : public TreeException
{
public:

    inline TreeOpenEdit()
        : TreeException(TreeOPEN_EDIT)
    { }

}; // TreeOpenEdit

class TreeParentOff : public TreeException
{
public:

    inline TreeParentOff()
        : TreeException(TreePARENT_OFF)
    { }

}; // TreeParentOff

class TreeRecordReadError : public TreeException
{
public:

    inline TreeRecordReadError()
        : TreeException(TreeREADERR)
    { }

}; // TreeRecordReadError

class TreeReadOnly : public TreeException
{
public:

    inline TreeReadOnly()
        : TreeException(TreeREADONLY)
    { }

}; // TreeReadOnly

class TreeResolved : public TreeException
{
public:

    inline TreeResolved()
        : TreeException(TreeRESOLVED)
    { }

}; // TreeResolved

class TreeSuccess : public TreeException
{
public:

    inline TreeSuccess()
        : TreeException(TreeSUCCESS)
    { }

}; // TreeSuccess

class TreeTagNameTooLong : public TreeException
{
public:

    inline TreeTagNameTooLong()
        : TreeException(TreeTAGNAMLEN)
    { }

}; // TreeTagNameTooLong

class TreeTagNotFound : public TreeException
{
public:

    inline TreeTagNotFound()
        : TreeException(TreeTNF)
    { }

}; // TreeTagNotFound

class TreeNotFound : public TreeException
{
public:

    inline TreeNotFound()
        : TreeException(TreeTREENF)
    { }

}; // TreeNotFound

class TreeUnresolved : public TreeException
{
public:

    inline TreeUnresolved()
        : TreeException(TreeUNRESOLVED)
    { }

}; // TreeUnresolved

class TreeUnsupportedClass : public TreeException
{
public:

    inline TreeUnsupportedClass()
        : TreeException(TreeUNSPRTCLASS)
    { }

}; // TreeUnsupportedClass

class TreeUnsupportedArrayType : public TreeException
{
public:

    inline TreeUnsupportedArrayType()
        : TreeException(TreeUNSUPARRDTYPE)
    { }

}; // TreeUnsupportedArrayType

class TreeNeedWriteFirst : public TreeException
{
public:

    inline TreeNeedWriteFirst()
        : TreeException(TreeWRITEFIRST)
    { }

}; // TreeNeedWriteFirst

class TreeFailure : public TreeException
{
public:

    inline TreeFailure()
        : TreeException(TreeFAILURE)
    { }

}; // TreeFailure

class TreeLockFailure : public TreeException
{
public:

    inline TreeLockFailure()
        : TreeException(TreeLOCK_FAILURE)
    { }

}; // TreeLockFailure

class TreeFileNotFound : public TreeException
{
public:

    inline TreeFileNotFound()
        : TreeException(TreeFILE_NOT_FOUND)
    { }

}; // TreeFileNotFound

class TreeCancel : public TreeException
{
public:

    inline TreeCancel()
        : TreeException(TreeCANCEL)
    { }

}; // TreeCancel

class TreeUnsupportedThickOperation : public TreeException
{
public:

    inline TreeUnsupportedThickOperation()
        : TreeException(TreeUNSUPTHICKOP)
    { }

}; // TreeUnsupportedThickOperation

class TreeNoSegments : public TreeException
{
public:

    inline TreeNoSegments()
        : TreeException(TreeNOSEGMENTS)
    { }

}; // TreeNoSegments

class TreeInvalidSegmentDataType : public TreeException
{
public:

    inline TreeInvalidSegmentDataType()
        : TreeException(TreeINVDTYPE)
    { }

}; // TreeInvalidSegmentDataType

class TreeInvalidSegmentShape : public TreeException
{
public:

    inline TreeInvalidSegmentShape()
        : TreeException(TreeINVSHAPE)
    { }

}; // TreeInvalidSegmentShape

class TreeInvalidShot : public TreeException
{
public:

    inline TreeInvalidShot()
        : TreeException(TreeINVSHOT)
    { }

}; // TreeInvalidShot

class TreeInvalidTag : public TreeException
{
public:

    inline TreeInvalidTag()
        : TreeException(TreeINVTAG)
    { }

}; // TreeInvalidTag

class TreeNoPath : public TreeException
{
public:

    inline TreeNoPath()
        : TreeException(TreeNOPATH)
    { }

}; // TreeNoPath

class TreeTreeFileReadError : public TreeException
{
public:

    inline TreeTreeFileReadError()
        : TreeException(TreeTREEFILEREADERR)
    { }

}; // TreeTreeFileReadError

class TreeMemoryAllocateError : public TreeException
{
public:

    inline TreeMemoryAllocateError()
        : TreeException(TreeMEMERR)
    { }

}; // TreeMemoryAllocateError

class TreeNoCurrentShot : public TreeException
{
public:

    inline TreeNoCurrentShot()
        : TreeException(TreeNOCURRENT)
    { }

}; // TreeNoCurrentShot

class TreeOpenReadWriteFailed : public TreeException
{
public:

    inline TreeOpenReadWriteFailed()
        : TreeException(TreeFOPENW)
    { }

}; // TreeOpenReadWriteFailed

class TreeOpenReadOnlyFailed : public TreeException
{
public:

    inline TreeOpenReadOnlyFailed()
        : TreeException(TreeFOPENR)
    { }

}; // TreeOpenReadOnlyFailed

class TreeCreateFailed : public TreeException
{
public:

    inline TreeCreateFailed()
        : TreeException(TreeFCREATE)
    { }

}; // TreeCreateFailed

class TreeConnectFailed : public TreeException
{
public:

    inline TreeConnectFailed()
        : TreeException(TreeCONNECTFAIL)
    { }

}; // TreeConnectFailed

class TreeNCIWriteError : public TreeException
{
public:

    inline TreeNCIWriteError()
        : TreeException(TreeNCIWRITE)
    { }

}; // TreeNCIWriteError

class TreeDeleteFailed : public TreeException
{
public:

    inline TreeDeleteFailed()
        : TreeException(TreeDELFAIL)
    { }

}; // TreeDeleteFailed

class TreeRenameFailed : public TreeException
{
public:

    inline TreeRenameFailed()
        : TreeException(TreeRENFAIL)
    { }

}; // TreeRenameFailed

class TreeEmpty : public TreeException
{
public:

    inline TreeEmpty()
        : TreeException(TreeEMPTY)
    { }

}; // TreeEmpty

class TreeParseError : public TreeException
{
public:

    inline TreeParseError()
        : TreeException(TreePARSEERR)
    { }

}; // TreeParseError

class TreeNCIReadError : public TreeException
{
public:

    inline TreeNCIReadError()
        : TreeException(TreeNCIREAD)
    { }

}; // TreeNCIReadError

class TreeNoVersion : public TreeException
{
public:

    inline TreeNoVersion()
        : TreeException(TreeNOVERSION)
    { }

}; // TreeNoVersion

class TreeDatafileReadError : public TreeException
{
public:

    inline TreeDatafileReadError()
        : TreeException(TreeDFREAD)
    { }

}; // TreeDatafileReadError

class TreeFileCloseError : public TreeException
{
public:

    inline TreeFileCloseError()
        : TreeException(TreeCLOSEERR)
    { }

}; // TreeFileCloseError

class TreeFileOverwriteError : public TreeException
{
public:

    inline TreeFileOverwriteError()
        : TreeException(TreeMOVEERROR)
    { }

}; // TreeFileOverwriteError

class TreeOpenFileForEditError : public TreeException
{
public:

    inline TreeOpenFileForEditError()
        : TreeException(TreeOPENEDITERR)
    { }

}; // TreeOpenFileForEditError

class TreeOpenReadOnly : public TreeException
{
public:

    inline TreeOpenReadOnly()
        : TreeException(TreeREADONLY_TREE)
    { }

}; // TreeOpenReadOnly

class TreeWriteTreeFileError : public TreeException
{
public:

    inline TreeWriteTreeFileError()
        : TreeException(TreeWRITETREEERR)
    { }

}; // TreeWriteTreeFileError

class TreeWildcardInNodeSpecifier : public TreeException
{
public:

    inline TreeWildcardInNodeSpecifier()
        : TreeException(TreeNOWILD)
    { }

}; // TreeWildcardInNodeSpecifier

inline void throwException(int status)
{
    switch (status)
    {
    case LibINSVIRMEM:
        throw LibInsufficientVirtualMemory();

    case LibINVARG:
        throw LibInvalidArgument();

    case LibINVSTRDES:
        throw LibInvalidStringDescriptor();

    case LibKEYNOTFOU:
        throw LibKeyNotFound();

    case LibNOTFOU:
        throw LibNotFound();

    case LibQUEWASEMP:
        throw LibQueueEmpty();

    case LibSTRTRU:
        throw LibStringTruncated();

    case StrMATCH:
        throw StrMatch();

    case StrNOMATCH:
        throw StrNoMatch();

    case StrNOELEM:
        throw StrNoElement();

    case StrINVDELIM:
        throw StrInvalidDelimiter();

    case StrSTRTOOLON:
        throw StrStringTooLong();

    case MDSplusWARNING:
        throw MDSplusWarning();

    case MDSplusSUCCESS:
        throw MDSplusSuccess();

    case MDSplusERROR:
        throw MDSplusError();

    case MDSplusFATAL:
        throw MDSplusFatal();

    case MDSplusSANDBOX:
        throw MDSplusSandbox();

    case SsSUCCESS:
        throw SsSuccess();

    case SsINTOVF:
        throw SsIntegerOverflow();

    case SsINTERNAL:
        throw SsInternal();

    case TdiBREAK:
        throw TdiMissingBreak();

    case TdiCASE:
        throw TdiMissingCase();

    case TdiCONTINUE:
        throw TdiMissingContinue();

    case TdiEXTRANEOUS:
        throw TdiExtraneousCharacters();

    case TdiRETURN:
        throw TdiExtraneousReturn();

    case TdiABORT:
        throw TdiAbort();

    case TdiBAD_INDEX:
        throw TdiBadIndex();

    case TdiBOMB:
        throw TdiBadPunctuation();

    case TdiEXTRA_ARG:
        throw TdiTooManyArguments();

    case TdiGOTO:
        throw TdiMissingGotoLabel();

    case TdiINVCLADSC:
        throw TdiInvalidClass();

    case TdiINVCLADTY:
        throw TdiInvalidClassForDataType();

    case TdiINVDTYDSC:
        throw TdiInvalidDataType();

    case TdiINV_OPC:
        throw TdiInvalidOpcode();

    case TdiINV_SIZE:
        throw TdiInvalidSize();

    case TdiMISMATCH:
        throw TdiArgumentMismatch();

    case TdiMISS_ARG:
        throw TdiMissingArgument();

    case TdiNDIM_OVER:
        throw TdiTooManyDimensions();

    case TdiNO_CMPLX:
        throw TdiComplexUnsupported();

    case TdiNO_OPC:
        throw TdiOpcodeUnsupported();

    case TdiNO_OUTPTR:
        throw TdiNoOutputPointer();

    case TdiNO_SELF_PTR:
        throw TdiNoSelfPointer();

    case TdiNOT_NUMBER:
        throw TdiNotANumber();

    case TdiNULL_PTR:
        throw TdiNullPointer();

    case TdiRECURSIVE:
        throw TdiRecursionLimitReached();

    case TdiSIG_DIM:
        throw TdiSignalDimensionMismatch();

    case TdiSYNTAX:
        throw TdiSyntaxError();

    case TdiTOO_BIG:
        throw TdiPrecisionLoss();

    case TdiUNBALANCE:
        throw TdiUnbalanced();

    case TdiUNKNOWN_VAR:
        throw TdiUnknownVariable();

    case TdiSTRTOOLON:
        throw TdiStringTooLong();

    case TdiTIMEOUT:
        throw TdiTimeout();

    case ApdAPD_APPEND:
        throw ApdAppendError();

    case ApdDICT_KEYVALPAIR:
        throw ApdInvalidKeyValuePair();

    case ApdDICT_KEYCLS:
        throw ApdKeyNotScalar();

    case DevBAD_ENDIDX:
        throw DevBadEndIndex();

    case DevBAD_FILTER:
        throw DevBadFilter();

    case DevBAD_FREQ:
        throw DevBadFrequency();

    case DevBAD_GAIN:
        throw DevBadGain();

    case DevBAD_HEADER:
        throw DevBadHeader();

    case DevBAD_HEADER_IDX:
        throw DevBadHeaderIndex();

    case DevBAD_MEMORIES:
        throw DevBadMemories();

    case DevBAD_MODE:
        throw DevBadMode();

    case DevBAD_NAME:
        throw DevBadName();

    case DevBAD_OFFSET:
        throw DevBadOffset();

    case DevBAD_STARTIDX:
        throw DevBadStartIndex();

    case DevNOT_TRIGGERED:
        throw DevNotTriggered();

    case DevFREQ_TOO_HIGH:
        throw DevFrequencyTooHigh();

    case DevINVALID_NOC:
        throw DevInvalidNumberOfChannels();

    case DevRANGE_MISMATCH:
        throw DevRangeMismatch();

    case DevBAD_VERBS:
        throw DevBadVerbs();

    case DevBAD_COMMANDS:
        throw DevBadCommands();

    case DevINV_SETUP:
        throw DevInvalidSetup();

    case DevPYDEVICE_NOT_FOUND:
        throw DevPyDeviceNotFound();

    case DevPY_INTERFACE_LIBRARY_NOT_FOUND:
        throw DevPyInterfaceLibraryNotFound();

    case DevIO_STUCK:
        throw DevIOStuck();

    case DevUNKNOWN_STATE:
        throw DevUnknownState();

    case DevWRONG_TREE:
        throw DevWrongTree();

    case DevWRONG_PATH:
        throw DevWrongPath();

    case DevWRONG_SHOT:
        throw DevWrongShot();

    case DevOFFLINE:
        throw DevOffline();

    case DevTRIGGERED_NOT_STORED:
        throw DevTriggeredNotStored();

    case DevNO_NAME_SPECIFIED:
        throw DevNoNameSpecified();

    case DevBAD_ACTIVE_CHAN:
        throw DevBadActiveChannel();

    case DevBAD_TRIG_SRC:
        throw DevBadTriggerSource();

    case DevBAD_CLOCK_SRC:
        throw DevBadClockSource();

    case DevBAD_PRE_TRIG:
        throw DevBadPreTrigger();

    case DevBAD_POST_TRIG:
        throw DevBadPostTrigger();

    case DevBAD_CLOCK_FREQ:
        throw DevBadClockFrequency();

    case DevTRIGGER_FAILED:
        throw DevTriggerFailed();

    case DevERROR_READING_CHANNEL:
        throw DevErrorReadingChannel();

    case DevERROR_DOING_INIT:
        throw DevErrorDoingInit();

    case DevCANNOT_LOAD_SETTINGS:
        throw DevCannotLoadSettings();

    case DevCANNOT_GET_BOARD_STATE:
        throw DevCannotGetBoardState();

    case DevBAD_PARAMETER:
        throw DevBadParameter();

    case DevCOMM_ERROR:
        throw DevCommunicationError();

    case DevCAMERA_NOT_FOUND:
        throw DevCameraNotFound();

    case DevNOT_A_PYDEVICE:
        throw DevNotAPyDevice();

    case CamDONE_Q:
        throw CamDoneQ();

    case CamDONE_NOQ:
        throw CamDoneNoQ();

    case CamDONE_NOX:
        throw CamDoneNoX();

    case CamSERTRAERR:
        throw CamSerialTransmissionError();

    case CamSCCFAIL:
        throw CamSerialCrateControllerFailure();

    case CamOFFLINE:
        throw CamOffline();

    case MdsdclSUCCESS:
        throw MdsdclSuccess();

    case MdsdclEXIT:
        throw MdsdclExit();

    case MdsdclERROR:
        throw MdsdclError();

    case MdsdclNORMAL:
        throw MdsdclNormal();

    case MdsdclPRESENT:
        throw MdsdclPresent();

    case MdsdclIVVERB:
        throw MdsdclInvalidVerb();

    case MdsdclABSENT:
        throw MdsdclAbsent();

    case MdsdclNEGATED:
        throw MdsdclNegated();

    case MdsdclNOTNEGATABLE:
        throw MdsdclNotNegatable();

    case MdsdclIVQUAL:
        throw MdsdclInvalidQualifier();

    case MdsdclPROMPT_MORE:
        throw MdsdclPromptMore();

    case MdsdclTOO_MANY_PRMS:
        throw MdsdclTooManyParameters();

    case MdsdclTOO_MANY_VALS:
        throw MdsdclTooManyValues();

    case MdsdclMISSING_VALUE:
        throw MdsdclMissingValue();

    case ServerNOT_DISPATCHED:
        throw ServerNotDispatched();

    case ServerINVALID_DEPENDENCY:
        throw ServerInvalidDependency();

    case ServerCANT_HAPPEN:
        throw ServerCantHappen();

    case ServerINVSHOT:
        throw ServerInvalidShot();

    case ServerABORT:
        throw ServerAborted();

    case ServerPATH_DOWN:
        throw ServerPathDown();

    case ServerSOCKET_ADDR_ERROR:
        throw ServerSocketAddressError();

    case ServerINVALID_ACTION_OPERATION:
        throw ServerInvalidActionOperation();

    case TclNORMAL:
        throw TclNormal();

    case TclFAILED_ESSENTIAL:
        throw TclFailedEssential();

    case TclNO_DISPATCH_TABLE:
        throw TclNoDispatchTable();

    case TreeALREADY_OFF:
        throw TreeNodeAlreadyOff();

    case TreeALREADY_ON:
        throw TreeNodeAlreadyOn();

    case TreeALREADY_OPEN:
        throw TreeNodeAlreadyOpen();

    case TreeALREADY_THERE:
        throw TreeNodeAlreadyThere();

    case TreeBADRECORD:
        throw TreeBadRecord();

    case TreeBOTH_OFF:
        throw TreeNodeAndParentOff();

    case TreeBUFFEROVF:
        throw TreeBufferOverflow();

    case TreeCONGLOMFULL:
        throw TreeConglomFull();

    case TreeCONGLOM_NOT_FULL:
        throw TreeConglomNotFull();

    case TreeCONTINUING:
        throw TreeContinuing();

    case TreeDUPTAG:
        throw TreeDuplicateTag();

    case TreeEDITING:
        throw TreeNotPermittedWhileEditing();

    case TreeILLEGAL_ITEM:
        throw TreeIllegalItem();

    case TreeILLPAGCNT:
        throw TreeIllegalPageCount();

    case TreeINVDFFCLASS:
        throw TreeInvalidDataFormatClass();

    case TreeINVDTPUSG:
        throw TreeInvalidPutUsage();

    case TreeINVPATH:
        throw TreeInvalidPath();

    case TreeINVRECTYP:
        throw TreeInvalidRecordType();

    case TreeINVTREE:
        throw TreeInvalidTreeStructure();

    case TreeMAXOPENEDIT:
        throw TreeMaxOpenForEdit();

    case TreeNEW:
        throw TreeNew();

    case TreeNMN:
        throw TreeNoMoreNodes();

    case TreeNMT:
        throw TreeNoMoreTags();

    case TreeNNF:
        throw TreeNodeNotFound();

    case TreeNODATA:
        throw TreeNoData();

    case TreeNODNAMLEN:
        throw TreeNodeNameTooLong();

    case TreeNOEDIT:
        throw TreeNotOpenForEdit();

    case TreeNOLOG:
        throw TreeNoPathDefined();

    case TreeNOMETHOD:
        throw TreeNoMethod();

    case TreeNOOVERWRITE:
        throw TreeNodeWriteOnce();

    case TreeNOTALLSUBS:
        throw TreeSubtreesNotOpen();

    case TreeNOTCHILDLESS:
        throw TreeNodeHasChildren();

    case TreeNOT_IN_LIST:
        throw TreeNotInList();

    case TreeNOTMEMBERLESS:
        throw TreeNodeHasMembers();

    case TreeNOTSON:
        throw TreeInvalidSubtreeNode();

    case TreeNOT_CONGLOM:
        throw TreeNotConglom();

    case TreeNOT_OPEN:
        throw TreeNotOpen();

    case TreeNOWRITEMODEL:
        throw TreeNoWriteModel();

    case TreeNOWRITESHOT:
        throw TreeNoWriteShot();

    case TreeNO_CONTEXT:
        throw TreeNoContext();

    case TreeOFF:
        throw TreeNodeIsOff();

    case TreeON:
        throw TreeNodeIsOn();

    case TreeOPEN:
        throw TreeOpen();

    case TreeOPEN_EDIT:
        throw TreeOpenEdit();

    case TreePARENT_OFF:
        throw TreeParentOff();

    case TreeREADERR:
        throw TreeRecordReadError();

    case TreeREADONLY:
        throw TreeReadOnly();

    case TreeRESOLVED:
        throw TreeResolved();

    case TreeSUCCESS:
        throw TreeSuccess();

    case TreeTAGNAMLEN:
        throw TreeTagNameTooLong();

    case TreeTNF:
        throw TreeTagNotFound();

    case TreeTREENF:
        throw TreeNotFound();

    case TreeUNRESOLVED:
        throw TreeUnresolved();

    case TreeUNSPRTCLASS:
        throw TreeUnsupportedClass();

    case TreeUNSUPARRDTYPE:
        throw TreeUnsupportedArrayType();

    case TreeWRITEFIRST:
        throw TreeNeedWriteFirst();

    case TreeFAILURE:
        throw TreeFailure();

    case TreeLOCK_FAILURE:
        throw TreeLockFailure();

    case TreeFILE_NOT_FOUND:
        throw TreeFileNotFound();

    case TreeCANCEL:
        throw TreeCancel();

    case TreeUNSUPTHICKOP:
        throw TreeUnsupportedThickOperation();

    case TreeNOSEGMENTS:
        throw TreeNoSegments();

    case TreeINVDTYPE:
        throw TreeInvalidSegmentDataType();

    case TreeINVSHAPE:
        throw TreeInvalidSegmentShape();

    case TreeINVSHOT:
        throw TreeInvalidShot();

    case TreeINVTAG:
        throw TreeInvalidTag();

    case TreeNOPATH:
        throw TreeNoPath();

    case TreeTREEFILEREADERR:
        throw TreeTreeFileReadError();

    case TreeMEMERR:
        throw TreeMemoryAllocateError();

    case TreeNOCURRENT:
        throw TreeNoCurrentShot();

    case TreeFOPENW:
        throw TreeOpenReadWriteFailed();

    case TreeFOPENR:
        throw TreeOpenReadOnlyFailed();

    case TreeFCREATE:
        throw TreeCreateFailed();

    case TreeCONNECTFAIL:
        throw TreeConnectFailed();

    case TreeNCIWRITE:
        throw TreeNCIWriteError();

    case TreeDELFAIL:
        throw TreeDeleteFailed();

    case TreeRENFAIL:
        throw TreeRenameFailed();

    case TreeEMPTY:
        throw TreeEmpty();

    case TreePARSEERR:
        throw TreeParseError();

    case TreeNCIREAD:
        throw TreeNCIReadError();

    case TreeNOVERSION:
        throw TreeNoVersion();

    case TreeDFREAD:
        throw TreeDatafileReadError();

    case TreeCLOSEERR:
        throw TreeFileCloseError();

    case TreeMOVEERROR:
        throw TreeFileOverwriteError();

    case TreeOPENEDITERR:
        throw TreeOpenFileForEditError();

    case TreeREADONLY_TREE:
        throw TreeOpenReadOnly();

    case TreeWRITETREEERR:
        throw TreeWriteTreeFileError();

    case TreeNOWILD:
        throw TreeWildcardInNodeSpecifier();

    }
}

enum class Class : uint8_t
{
    Missing = CLASS_MISSING,

    S = CLASS_S,
    D = CLASS_D,
    A = CLASS_A,
    XD = CLASS_XD,
    XS = CLASS_XS,
    R = CLASS_R,
    CA = CLASS_CA,
    APD = CLASS_APD,

}; // enum class Class

std::string to_string(const Class& class_);

inline constexpr bool operator==(const Class lhs, const class_t rhs) {
    return lhs == static_cast<Class>(rhs);
}

enum class DType : uint8_t
{
    Missing = DTYPE_MISSING,

    BU = DTYPE_BU,
    WU = DTYPE_WU,
    LU = DTYPE_LU,
    QU = DTYPE_QU,
    B = DTYPE_B,
    W = DTYPE_W,
    L = DTYPE_L,
    Q = DTYPE_Q,
    F = DTYPE_F,
    D = DTYPE_D,
    FC = DTYPE_FC,
    DC = DTYPE_DC,
    T = DTYPE_T,
    DSC = DTYPE_DSC,
    OU = DTYPE_OU,
    O = DTYPE_O,
    G = DTYPE_G,
    H = DTYPE_H,
    GC = DTYPE_GC,
    HC = DTYPE_HC,
    FS = DTYPE_FS,
    FT = DTYPE_FT,
    FSC = DTYPE_FSC,
    FTC = DTYPE_FTC,
    C = DTYPE_C,

    Float = DTYPE_FLOAT,
    Double = DTYPE_DOUBLE,

    Pointer = DTYPE_POINTER,
    Ident = DTYPE_IDENT,
    NID = DTYPE_NID,
    Path = DTYPE_PATH,
    Param = DTYPE_PARAM,
    Signal = DTYPE_SIGNAL,
    Dimension = DTYPE_DIMENSION,
    Window = DTYPE_WINDOW,
    Slope = DTYPE_SLOPE,
    Function = DTYPE_FUNCTION,
    Conglom = DTYPE_CONGLOM,
    Range = DTYPE_RANGE,
    Action = DTYPE_ACTION,
    Dispatch = DTYPE_DISPATCH,
    Program = DTYPE_PROGRAM,
    Routine = DTYPE_ROUTINE,
    Procedure = DTYPE_PROCEDURE,
    Method = DTYPE_METHOD,
    Dependency = DTYPE_DEPENDENCY,
    Condition = DTYPE_CONDITION,
    Event = DTYPE_EVENT,
    WithUnits = DTYPE_WITH_UNITS,
    Call = DTYPE_CALL,
    WithError = DTYPE_WITH_ERROR,
    List = DTYPE_LIST,
    Tuple = DTYPE_TUPLE,
    Dictionary = DTYPE_DICTIONARY,
    Opaque = DTYPE_OPAQUE,

}; // enum class DType

std::string to_string(const DType& dtype);

inline constexpr bool operator==(const DType lhs, const dtype_t rhs) {
    return lhs == static_cast<DType>(rhs);
}

class Tree;

class Data
{
public:

    typedef std::nullptr_t __ctype;
    static constexpr Class __class = Class::Missing;
    static constexpr DType __dtype = DType::Missing;

    template <typename ResultType = Data, typename ...ArgTypes>
    static ResultType Compile(const std::string& expression, const ArgTypes& ...args);

    template <typename ResultType = Data, typename ...ArgTypes>
    static ResultType Execute(const std::string& expression, const ArgTypes& ...args);

    template <typename ValueType>
    static Data FromScalar(ValueType value) = delete;

    #ifdef __cpp_lib_span

        template <typename ValueType>
        static Data FromArray(
            std::span<const ValueType> values,
            const std::vector<uint32_t>& dims = {}
        );

    #endif

    template <typename ValueType>
    static Data FromArray(
        const std::vector<ValueType>& values,
        const std::vector<uint32_t>& dims = {}
    );

    template <typename ResultType = Data, typename ...ArgTypes>
    static inline ResultType FromExpression(const std::string& expression, ArgTypes... args) {
        return Execute<ResultType>(expression, args...);
    }

    inline Data() = default;

    inline Data(mdsdsc_xd_t && xd, Tree * tree = nullptr)
        : _xd(xd)
        , _tree(tree)
    {
        xd = MDSDSC_XD_INITIALIZER;
    }

    inline virtual ~Data() {
        MdsFree1Dx(&_xd, nullptr);
    }

    // Disallow copy and assign
    Data(const Data&) = delete;
    Data& operator=(Data&) = delete;

    // Enable move operators
    inline Data(Data&& other) {
        _xd = other._xd;
        other._xd = MDSDSC_XD_INITIALIZER;
        _tree = other._tree;
        other._tree = nullptr;
    }

    Data& operator=(Data&& other) {
        _xd = other._xd;
        other._xd = MDSDSC_XD_INITIALIZER;
        _tree = other._tree;
        other._tree = nullptr;
        return *this;
    }

    virtual const char * getClassName() const {
        return "Data";
    }

    inline void setTree(Tree * tree) {
        _tree = tree;
    }

    [[nodiscard]]
    inline Tree * getTree() const {
        return _tree;
    }

    inline bool hasTree() const {
        return (_tree != nullptr);
    }

    [[nodiscard]]
    inline mdsdsc_t * getDescriptor() const {
        return _xd.pointer;
    }

    [[nodiscard]]
    inline length_t getLength() const {
        mdsdsc_t * dsc = getDescriptor();
        return (dsc ? dsc->length : 0);
    }

    [[nodiscard]]
    inline Class getClass() const {
        mdsdsc_t * dsc = getDescriptor();
        return (dsc ? Class(dsc->class_) : Class::Missing);
    }

    [[nodiscard]]
    inline DType getDType() const {
        mdsdsc_t * dsc = getDescriptor();
        return (dsc ? DType(dsc->dtype) : DType::Missing);
    }

    [[nodiscard]]
    inline void * getPointer() const {
        mdsdsc_t * dsc = getDescriptor();
        return (dsc ? dsc->pointer : nullptr);
    }

    [[nodiscard]]
    inline mdsdsc_xd_t release() {
        mdsdsc_xd_t tmp = std::move(_xd);
        _xd = MDSDSC_XD_INITIALIZER;
        return tmp;
    }

    [[nodiscard]]
    inline Data clone() const {
        return _clone<Data>();
    }

    template <typename ResultType>
    [[nodiscard]]
    inline ResultType convert() const {
        return clone().releaseAndConvert<ResultType>();
    }

    template <typename ResultType>
    [[nodiscard]]
    ResultType releaseAndConvert();

    std::string decompile() const;

    // TODO: Return Int8Array?
    Data serialize() const;

    template <typename DataType = Data>
    inline DataType getData() const {
        return convert<DataType>();
    }

    template <typename UnitsType = Data>
    UnitsType getUnits() const;

    // TODO: Default UnitsType to String
    template <typename DataType = Data, typename UnitsType = Data>
    std::tuple<DataType, UnitsType> getDataWithUnits() const;

    template <typename ErrorType = Data>
    ErrorType getError() const;

    template <typename HelpType = Data>
    HelpType getHelp() const;

    template <typename ValidationType = Data>
    ValidationType getValidation() const;

    // TODO: Explain decltype() conditionally enabling functions

    template <typename ScalarType>
    inline auto operator+(const ScalarType& value) -> decltype(Data::FromScalar(value)) {
        return *this + Data::FromScalar(value);
    }

    template <typename ScalarType>
    inline auto operator-(const ScalarType& value) -> decltype(Data::FromScalar(value)) {
        return *this - Data::FromScalar(value);
    }

    template <typename ScalarType>
    inline auto operator*(const ScalarType& value) -> decltype(Data::FromScalar(value)) {
        return *this * Data::FromScalar(value);
    }

    template <typename ScalarType>
    inline auto operator/(const ScalarType& value) -> decltype(Data::FromScalar(value)) {
        return *this / Data::FromScalar(value);
    }

    template <typename ScalarType>
    inline auto operator%(const ScalarType& value) -> decltype(Data::FromScalar(value)) {
        return *this % Data::FromScalar(value);
    }

    template <typename ResultType, typename ScalarType>
    inline auto add(const ScalarType& value) -> decltype(Data::FromScalar(value), ResultType()) {
        return (*this + Data::FromScalar(value)).template releaseAndConvert<ResultType>();
    }

    template <typename ResultType, typename ScalarType>
    inline auto subtract(const ScalarType& value) -> decltype(Data::FromScalar(value), ResultType()) {
        return (*this - Data::FromScalar(value)).template releaseAndConvert<ResultType>();
    }

    template <typename ResultType, typename ScalarType>
    inline auto multiply(const ScalarType& value) -> decltype(Data::FromScalar(value), ResultType()) {
        return (*this * Data::FromScalar(value)).template releaseAndConvert<ResultType>();
    }

    template <typename ResultType, typename ScalarType>
    inline auto divide(const ScalarType& value) -> decltype(Data::FromScalar(value), ResultType()) {
        return (*this / Data::FromScalar(value)).template releaseAndConvert<ResultType>();
    }

    template <typename ResultType, typename ScalarType>
    inline auto modulo(const ScalarType& value) -> decltype(Data::FromScalar(value), ResultType()) {
        return (*this % Data::FromScalar(value)).template releaseAndConvert<ResultType>();
    }

    template <typename ArrayType>
    inline auto operator+(const ArrayType& values) -> decltype(Data::FromArray(values)) {
        return *this + Data::FromArray(values);
    }

    template <typename ArrayType>
    inline auto operator-(const ArrayType& values) -> decltype(Data::FromArray(values)) {
        return *this - Data::FromArray(values);
    }

    template <typename ArrayType>
    inline auto operator*(const ArrayType& values) -> decltype(Data::FromArray(values)) {
        return *this * Data::FromArray(values);
    }

    template <typename ArrayType>
    inline auto operator/(const ArrayType& values) -> decltype(Data::FromArray(values)) {
        return *this / Data::FromArray(values);
    }

    template <typename ArrayType>
    inline auto operator%(const ArrayType& values) -> decltype(Data::FromArray(values)) {
        return *this % Data::FromArray(values);
    }

    template <typename ResultType, typename ArrayType>
    inline auto add(const ArrayType& value) -> decltype(Data::FromArray(value), ResultType()) {
        return (*this + Data::FromArray(value)).template releaseAndConvert<ResultType>();
    }

    template <typename ResultType, typename ArrayType>
    inline auto subtract(const ArrayType& value) -> decltype(Data::FromArray(value), ResultType()) {
        return (*this - Data::FromArray(value)).template releaseAndConvert<ResultType>();
    }

    template <typename ResultType, typename ArrayType>
    inline auto multiply(const ArrayType& value) -> decltype(Data::FromArray(value), ResultType()) {
        return (*this * Data::FromArray(value)).template releaseAndConvert<ResultType>();
    }

    template <typename ResultType, typename ArrayType>
    inline auto divide(const ArrayType& value) -> decltype(Data::FromArray(value), ResultType()) {
        return (*this / Data::FromArray(value)).template releaseAndConvert<ResultType>();
    }

    template <typename ResultType, typename ArrayType>
    inline auto modulo(const ArrayType& value) -> decltype(Data::FromArray(value), ResultType()) {
        return (*this % Data::FromArray(value)).template releaseAndConvert<ResultType>();
    }

    template <typename DataType,
        typename std::enable_if<std::is_base_of<Data, DataType>::value, bool>::type = true>
    inline Data operator+(const DataType& other)
    {
        mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
        mdsdsc_t * args[] = { getDescriptor(), other.getDescriptor() };
        int status = _intrinsic(OPC_ADD, 2, args, &out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Data(std::move(out), getTree());
    }

    template <typename DataType,
        typename std::enable_if<std::is_base_of<Data, DataType>::value, bool>::type = true>
    inline Data operator-(const DataType& other)
    {
        mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
        mdsdsc_t * args[] = { getDescriptor(), other.getDescriptor() };
        int status = _intrinsic(OPC_SUBTRACT, 2, args, &out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Data(std::move(out), getTree());
    }

    template <typename DataType,
        typename std::enable_if<std::is_base_of<Data, DataType>::value, bool>::type = true>
    inline Data operator/(const DataType& other)
    {
        mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
        mdsdsc_t * args[] = { getDescriptor(), other.getDescriptor() };
        int status = _intrinsic(OPC_DIVIDE, 2, args, &out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Data(std::move(out), getTree());
    }

    template <typename DataType,
        typename std::enable_if<std::is_base_of<Data, DataType>::value, bool>::type = true>
    inline Data operator*(const DataType& other)
    {
        mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
        mdsdsc_t * args[] = { getDescriptor(), other.getDescriptor() };
        int status = _intrinsic(OPC_MULTIPLY, 2, args, &out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Data(std::move(out), getTree());
    }

    template <typename DataType,
        typename std::enable_if<std::is_base_of<Data, DataType>::value, bool>::type = true>
    inline Data operator%(const DataType& other)
    {
        mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
        mdsdsc_t * args[] = { getDescriptor(), other.getDescriptor() };
        int status = _intrinsic(OPC_MOD, 2, args, &out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Data(std::move(out), getTree());
    }

    template <typename DataType,
        typename std::enable_if<std::is_base_of<Data, DataType>::value, bool>::type = true>
    inline bool operator==(const DataType& other) const
    {
        // TODO: Should this always be the case?
        if (getTree() != other.getTree()) {
            return false;
        }

        bool equal = false;
        mdsdsc_s_t out = {
            .length = 1,
            .dtype = DTYPE_BU,
            .class_ = CLASS_S,
            .pointer = (char *)&equal
        };

        mdsdsc_t * args[] = { getDescriptor(), other.getDescriptor() };
        int status = _intrinsic(OPC_EQ, 2, args, (mdsdsc_xd_t *)&out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return equal;
    }

    template <typename DataType,
        typename std::enable_if<std::is_base_of<Data, DataType>::value, bool>::type = true>
    inline bool operator!=(const DataType& other) const {
        return !(*this == other);
    }

    template <typename DataType>
    inline bool equals(const DataType& other) const {
        return (*this == other);
    }

    template <typename DataType>
    inline bool notEquals(const DataType& other) const {
        return (*this != other);
    }

protected:

    mdsdsc_xd_t _xd = MDSDSC_XD_INITIALIZER;

    Tree * _tree = nullptr;

    int _intrinsic(opcode_t opcode, int narg, mdsdsc_t *list[], mdsdsc_xd_t * out) const;

    template <typename ResultType>
    inline ResultType _clone() const {
        mdsdsc_xd_t xd = MDSDSC_XD_INITIALIZER;
        int status = MdsCopyDxXd(getDescriptor(), &xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return ResultType(std::move(xd), getTree());
    }

    template <typename ResultType>
    ResultType _convertToScalar();

    template <typename ResultType>
    ResultType _convertToArray();

}; // class Data

static const Data EmptyData;

std::string to_string(const Data * data);

inline std::string to_string(const Data& data) {
    return to_string(&data);
}

template <>
inline Data Data::releaseAndConvert() {
    return Data(release(), getTree());
}

template <>
inline Data Data::FromScalar(std::nullptr_t) {
    return Data();
}

template <typename T>
struct is_std_vector : std::false_type { };

template <typename T, typename A>
struct is_std_vector<std::vector<T, A>> : std::true_type { };

template <typename T>
struct is_valid_ctype : std::false_type { };

// TODO: Rename? DataWrapper DataView
class DataView
{
public:

    DataView() = delete;

    DataView(const DataView&) = default;
    DataView& operator=(const DataView&) = default;

    inline mdsdsc_t * getDescriptor() const {
        return (mdsdsc_t *)&_dsc;
    }

    inline DataView(std::nullptr_t)
        : _dsc(mdsdsc_a_t{
            .length = 0,
            .dtype = DTYPE_MISSING,
            .class_ = CLASS_MISSING,
            .pointer = nullptr,
        })
    { }

    // If the argument is already a MDSplus Data type, we only want to store a reference
    // to it, not take ownership, so we store a CLASS_S descriptor instead of a CLASS_XD.
    template <typename DataType,
        typename std::enable_if<std::is_base_of<Data, DataType>::value, bool>::type = true>
    inline DataView(const DataType &value)
        : _dsc(mdsdsc_a_t{
            .length = 0,
            .dtype = DTYPE_DSC,
            .class_ = CLASS_S,
            .pointer = (char *)value.getDescriptor(),
        })
    { }

    template <typename CType,
        typename std::enable_if<is_valid_ctype<CType>::value, bool>::type = true>
    inline DataView(const std::vector<CType> &value)
        : _dsc(mdsdsc_a_t{
            .length = sizeof(CType),
            .dtype = _getDTypeForCType<CType>(),
            .class_ = CLASS_S,
            .pointer = value.data(),
            .scale = 0,
            .digits = 0,
            .aflags = aflags_t{
                .binscale = false,
                .redim = true,
                .column = true,
                .coeff = false,
                .bounds = false,
            },
            .dimct = 0,
            .arsize = value.size() * sizeof(CType),
        })
    { }

    #ifdef __cpp_lib_span

        template <typename CType,
            typename std::enable_if<is_valid_ctype<CType>::value, bool>::type = true>
        inline DataView(std::span<const CType> value)
            : _dsc(mdsdsc_a_t{
                .length = sizeof(CType),
                .dtype = _getDTypeForCType<CType>(),
                .class_ = CLASS_S,
                .pointer = value.data(),
                .scale = 0,
                .digits = 0,
                .aflags = aflags_t{
                    .binscale = false,
                    .redim = true,
                    .column = true,
                    .coeff = false,
                    .bounds = false,
                },
                .dimct = 0,
                .arsize = value.size() * sizeof(CType),
            })
        { }

    #endif

    template <typename CType,
        typename std::enable_if<is_valid_ctype<CType>::value, bool>::type = true>
    inline DataView(const CType &value)
        : _dsc(mdsdsc_a_t{
            .length = sizeof(CType),
            .dtype = _getDTypeForCType<CType>(),
            .class_ = CLASS_S,
            .pointer = (char *)&value,
        })
    { }

    // TODO: Improve?
    #ifdef __cpp_lib_string_view

        inline DataView(std::string_view value)
            : _dsc(mdsdsc_a_t{
                .length = length_t(value.size()),
                .dtype = DTYPE_T,
                .class_ = CLASS_S,
                .pointer = const_cast<char *>(value.data()),
            })
        { }

    #else

        inline DataView(const char * value)
            : _dsc(mdsdsc_a_t{
                .length = length_t(strlen(value)),
                .dtype = DTYPE_T,
                .class_ = CLASS_S,
                .pointer = const_cast<char *>(value),
            })
        { }

        inline DataView(const std::string& value)
            : _dsc(mdsdsc_a_t{
                .length = length_t(value.size()),
                .dtype = DTYPE_T,
                .class_ = CLASS_S,
                .pointer = const_cast<char *>(value.data()),
            })
        { }

    #endif // __cpp_lib_string_view

private:

    mdsdsc_a_t _dsc = MDSDSC_XD_INITIALIZER;

    template <typename CType>
    inline dtype_t _getDTypeForCType()
    {
        std::type_index id = typeid(CType);
        if (id == typeid(int8_t)) {
            return DTYPE_B;
        }
        else if (id == typeid(int16_t)) {
            return DTYPE_W;
        }
        else if (id == typeid(int32_t)) {
            return DTYPE_L;
        }
        else if (id == typeid(int64_t)) {
            return DTYPE_Q;
        }
        else if (id == typeid(uint8_t)) {
            return DTYPE_BU;
        }
        else if (id == typeid(uint16_t)) {
            return DTYPE_WU;
        }
        else if (id == typeid(uint32_t)) {
            return DTYPE_LU;
        }
        else if (id == typeid(uint64_t)) {
            return DTYPE_QU;
        }
        else if (id == typeid(float)) {
            return DTYPE_FS;
        }
        else if (id == typeid(double)) {
            return DTYPE_FT;
        }
        else if (id == typeid(std::complex<float>)) {
            return DTYPE_FSC;
        }
        else if (id == typeid(std::complex<double>)) {
            return DTYPE_FTC;
        }

        return DTYPE_MISSING;
    };

};

class String : public Data
{
public:

    static constexpr Class __class = Class::S;
    static constexpr DType __dtype = DType::T;

    String() = default;

    String(mdsdsc_xd_t && xd, Tree * tree = nullptr)
        : Data(std::move(xd), tree)
    { }

    #ifdef __cpp_lib_string_view

        inline String(std::string_view value) {
            setValue(value);
        }

    #else

        inline String(const std::string& value) {
            setValue(value);
        }

    #endif // __cpp_lib_string_view

    inline String(const char *value, length_t length) {
        setValue(value, length);
    }

    String(String&&) = default;
    String& operator=(String&&) = default;

    [[nodiscard]]
    inline String clone() const {
        return _clone<String>();
    }

    inline std::string getValue() const {
        return getString();
    }

    inline std::string getString() const {
        mdsdsc_t * dsc = getDescriptor();
        return std::string(dsc->pointer, dsc->length);
    }

    #ifdef __cpp_lib_string_view

        // Warning: this will not be null-terminated
        inline std::string_view getStringView() const {
            mdsdsc_t * dsc = getDescriptor();
            return std::string_view(dsc->pointer, dsc->length);
        }

    #endif // __cpp_lib_string_view

    #ifdef __cpp_lib_string_view

        void setValue(std::string_view value) {
            setValue(value.data(), value.size());
        }

    #else

        void setValue(const std::string& value) {
            setValue(value.data(), value.size());
        }

    #endif // __cpp_lib_string_view

    void setValue(const char * value, length_t length);

}; // class String

// TODO: Decide if to_string(String) should be different
inline std::string to_string(const String * data) {
    return data->getString();
}

inline std::string to_string(const String& data) {
    return data.getString();
}

template <>
inline String Data::releaseAndConvert()
{
    mdsdsc_xd_t xd = release();
    mdsdsc_t * dsc = xd.pointer;

    if (dsc->class_ == CLASS_S && dsc->dtype == DTYPE_T) {
        return String(std::move(xd), getTree());
    }

    // TODO: Call DATA()?

    mdsdsc_xd_t xdDeco = MDSDSC_XD_INITIALIZER;
    int status = _intrinsic(OPC_DECOMPILE, 1, &dsc, &xdDeco);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    MdsFree1Dx(&xd, nullptr);
    return String(std::move(xdDeco), getTree());
}

template <>
inline std::string Data::getData() const {
    return getData<String>().getString();
}

#ifdef __cpp_lib_string_view

    template <>
    inline Data Data::FromScalar(std::string_view value) {
        return String(value);
    }

#else

    template <>
    inline Data Data::FromScalar(const char * value) {
        return String(value);
    }

    template <>
    inline Data Data::FromScalar(const std::string& value) {
        return String(value);
    }

#endif // __cpp_lib_string_view

class StringArray : public Data
{
public:

    static constexpr Class __class = Class::A;
    static constexpr DType __dtype = DType::T;

    StringArray() = default;

    inline StringArray(mdsdsc_xd_t && xd, Tree * tree = nullptr)
        : Data(std::move(xd), tree)
    { }

    #ifdef __cpp_lib_span

        inline StringArray(std::span<const std::string> values, const std::vector<uint32_t>& dims = {}) {
            setValues(values, dims);
        }

    #else

        inline StringArray(const std::vector<std::string>& values, const std::vector<uint32_t>& dims = {}) {
            setValues(values, dims);
        }

    #endif // __cpp_lib_span

    inline StringArray(std::initializer_list<std::string> values, const std::vector<uint32_t>& dims = {}) {
        std::vector<std::string> tmp(values);
        setValues(tmp, dims);
    }

    #ifdef __cpp_lib_string_view

        inline StringArray(std::initializer_list<std::string_view> values, const std::vector<uint32_t>& dims = {}) {
            std::vector<std::string_view> tmp(values);
            setValues(tmp, dims);
        }

    #endif // __cpp_lib_string_view

    StringArray(StringArray&&) = default;
    StringArray& operator=(StringArray&&) = default;

    [[nodiscard]]
    inline mdsdsc_a_t * getArrayDescriptor() const {
        return reinterpret_cast<mdsdsc_a_t *>(_xd.pointer);
    }

    [[nodiscard]]
    inline StringArray clone() const {
        return _clone<StringArray>();
    }

    [[nodiscard]]
    std::vector<std::string> getValues() const;

    [[nodiscard]]
    std::string getValueAt(size_t index) const;

    [[nodiscard]]
    size_t getSize() const;

    void setValues(
        const std::vector<std::string>& values,
        const std::vector<uint32_t>& dims = {}
    );

    #ifdef __cpp_lib_string_view

        void setValues(
            const std::vector<std::string_view>& values,
            const std::vector<uint32_t>& dims = {}
        );

    #endif // __cpp_lib_string_view

#ifdef __cpp_lib_span

        inline void setValues(
            std::span<const std::string> values,
            const std::vector<uint32_t>& dims = {}
        ) {
            setValues(values, dims);
        }

    #endif

    // STL Compatability

    [[nodiscard]]
    inline size_t size() const {
        return getSize();
    }

    [[nodiscard]]
    inline size_t empty() const {
        return getSize() == 0;
    }

    [[nodiscard]]
    inline std::string at(size_t index) const {
        return getValueAt(index);
    }

    struct Iterator
    {
    public:

        Iterator(size_t index, const StringArray * array)
            : _index(index)
            , _array(array)
        { }

        inline std::string operator*() const {
            return _array->getValueAt(_index);
        }

        inline Iterator& operator++() {
            _index++;
            return *this;
        }

        inline Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        inline friend bool operator==(const Iterator& a, const Iterator& b) {
            return (a._index == b._index && a._array == b._array);
        };

        inline friend bool operator!=(const Iterator& a, const Iterator& b) {
            return (a._index != b._index || a._array != b._array);
        };

    private:

        size_t _index;

        const StringArray * _array;

    };

    inline Iterator begin() const {
        return Iterator(0, this);
    }

    inline Iterator end() const {
        return Iterator(getSize(), this);
    }

protected:

    void _setValues(
        const std::string& buffer,
        length_t length,
        const std::vector<uint32_t>& dims
    );

}; // class StringArray

template <>
inline StringArray Data::releaseAndConvert()
{
    mdsdsc_xd_t xd = release();
    mdsdsc_t * dsc = xd.pointer;

    if (dsc->class_ == CLASS_A && dsc->dtype == DTYPE_T) {
        return StringArray(std::move(xd), getTree());
    }
    else if (dsc->class_ == CLASS_S && dsc->dtype == DTYPE_T) {
        #ifdef __cpp_lib_string_view
            return StringArray({ std::string_view(dsc->pointer, dsc->length) });
        #else
            return StringArray({ std::string(dsc->pointer, dsc->length) });
        #endif
    }

    MdsFree1Dx(&xd, nullptr);
    throw TdiInvalidDataType();
}

class Scalar : public Data
{
public:

    static constexpr Class __class = Class::S;

    Scalar() = default;

    Scalar(mdsdsc_xd_t && xd, Tree * tree = nullptr)
        : Data(std::move(xd), tree)
    { }

protected:

    template <typename CType>
    Scalar(DType dtype, CType value) {
        _setValue<CType>(dtype, value);
    }

    template <typename CType>
    inline void _setValue(DType dtype, CType value)
    {
        int status;

        mdsdsc_s_t dsc = {
            .length = sizeof(CType),
            .dtype = dtype_t(dtype),
            .class_ = class_t(__class),
            .pointer = (char *)&value,
        };

        status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }
    }

    template <typename CType>
    inline CType _getValue() const {
        mdsdsc_t * dsc = getDescriptor();
        if (dsc && dsc->pointer) {
            return *(CType *)dsc->pointer;
        }

        // TODO: throw something?
        return {};
    }

};

#define _MDSPLUS_SCALAR_BOOTSTRAP(ScalarType, CType, DataType)  \
                                                                \
    typedef CType __ctype;                                      \
    static constexpr DType __dtype = DataType;                  \
                                                                \
    ScalarType() = default;                                     \
                                                                \
    ScalarType(ScalarType&&) = default;                         \
    ScalarType& operator=(ScalarType&&) = default;              \
                                                                \
    inline ScalarType(mdsdsc_xd_t && xd, Tree * tree = nullptr) \
        : Scalar(std::move(xd), tree)                           \
    { }                                                         \
                                                                \
    /** Construct a Scalar from a value **/                     \
    inline ScalarType(__ctype value)                            \
        : Scalar(__dtype, value)                                \
    { }                                                         \
                                                                \
    inline const char * getClassName() const override {         \
        return #ScalarType;                                     \
    }                                                           \
                                                                \
    [[nodiscard]]                                               \
    inline ScalarType clone() const {                           \
        return _clone<ScalarType>();                            \
    }                                                           \
                                                                \
    [[nodiscard]]                                               \
    inline __ctype getValue() const {                           \
        return _getValue<__ctype>();                            \
    }                                                           \
                                                                \
    inline void setValue(__ctype value) {                       \
        _setValue<__ctype>(__dtype, value);                     \
    }

class Int8 : public Scalar
{
public:

    _MDSPLUS_SCALAR_BOOTSTRAP(Int8, int8_t, DType::B)

}; // class Int8

class UInt8 : public Scalar
{
public:

    _MDSPLUS_SCALAR_BOOTSTRAP(UInt8, uint8_t, DType::BU)

}; // class UInt8

class Int16 : public Scalar
{
public:

    _MDSPLUS_SCALAR_BOOTSTRAP(Int16, int16_t, DType::W)

}; // class Int16

class UInt16 : public Scalar
{
public:

    _MDSPLUS_SCALAR_BOOTSTRAP(UInt16, uint16_t, DType::WU)

}; // class UInt16

class Int32 : public Scalar
{
public:

    _MDSPLUS_SCALAR_BOOTSTRAP(Int32, int32_t, DType::L)

}; // class Int32

class UInt32 : public Scalar
{
public:

    _MDSPLUS_SCALAR_BOOTSTRAP(UInt32, uint32_t, DType::LU)

}; // class UInt32

class Int64 : public Scalar
{
public:

    _MDSPLUS_SCALAR_BOOTSTRAP(Int64, int64_t, DType::Q)

}; // class Int32

class UInt64 : public Scalar
{
public:

    _MDSPLUS_SCALAR_BOOTSTRAP(UInt64, uint64_t, DType::QU)

}; // class UInt64

// TODO: Int128 / UInt128 ?

class Float32 : public Scalar
{
public:

    _MDSPLUS_SCALAR_BOOTSTRAP(Float32, float, DType::FS)

}; // class Float32

class Float64 : public Scalar
{
public:

    _MDSPLUS_SCALAR_BOOTSTRAP(Float64, double, DType::FT)

}; // class Float64

class Complex32 : public Scalar
{
public:

    _MDSPLUS_SCALAR_BOOTSTRAP(Complex32, std::complex<float>, DType::FSC)

}; // class Complex32

class Complex64 : public Scalar
{
public:

    _MDSPLUS_SCALAR_BOOTSTRAP(Complex64, std::complex<double>, DType::FTC)

}; // class Complex64

#define _MDSPLUS_SCALAR_CUSTOMIZATION(ScalarType)                    \
                                                                     \
    template <>                                                      \
    struct is_valid_ctype<ScalarType::__ctype> : std::true_type { }; \
                                                                     \
    template <>                                                      \
    inline ScalarType Data::releaseAndConvert() {                    \
        return _convertToScalar<ScalarType>();                       \
    }                                                                \
                                                                     \
    template <>                                                      \
    inline ScalarType::__ctype Data::getData() const {               \
        return getData<ScalarType>().getValue();                     \
    }                                                                \
                                                                     \
    template <>                                                      \
    inline Data Data::FromScalar(ScalarType::__ctype value) {        \
        return ScalarType(value);                                    \
    }

_MDSPLUS_SCALAR_CUSTOMIZATION(Int8)
_MDSPLUS_SCALAR_CUSTOMIZATION(Int16)
_MDSPLUS_SCALAR_CUSTOMIZATION(Int32)
_MDSPLUS_SCALAR_CUSTOMIZATION(Int64)
_MDSPLUS_SCALAR_CUSTOMIZATION(UInt8)
_MDSPLUS_SCALAR_CUSTOMIZATION(UInt16)
_MDSPLUS_SCALAR_CUSTOMIZATION(UInt32)
_MDSPLUS_SCALAR_CUSTOMIZATION(UInt64)
_MDSPLUS_SCALAR_CUSTOMIZATION(Float32)
_MDSPLUS_SCALAR_CUSTOMIZATION(Float64)
_MDSPLUS_SCALAR_CUSTOMIZATION(Complex32)
_MDSPLUS_SCALAR_CUSTOMIZATION(Complex64)

class Array : public Data
{
public:

    static constexpr Class __class = Class::A;

    Array() = default;

    Array(mdsdsc_xd_t && xd, Tree * tree = nullptr)
        : Data(std::move(xd), tree)
    { }

    [[nodiscard]]
    inline mdsdsc_a_t * getArrayDescriptor() const {
        return reinterpret_cast<mdsdsc_a_t *>(_xd.pointer);
    }

    [[nodiscard]]
    std::vector<uint32_t> getDimensions() const;

    [[nodiscard]]
    inline size_t getSize() const {
        mdsdsc_a_t * dsc = getArrayDescriptor();
        if (dsc) {
            return dsc->arsize / dsc->length;
        }
        return 0;
    }

    // STL Compatability

    inline size_t size() const {
        return getSize();
    }

    inline bool empty() const {
        return (getSize() == 0);
    }

protected:

    template <typename CType>
    inline Array(DType dtype, std::initializer_list<CType> values, const std::vector<uint32_t>& dims = {}) {
        std::vector<CType> tmp(values);
        _setValues(dtype, tmp, dims);
    }

    #ifdef __cpp_lib_span

        template <typename CType>
        inline Array(DType dtype, std::span<const CType> values, const std::vector<uint32_t>& dims = {}) {
            _setValues<CType>(dtype, values, dims);
        }

        template <typename CType>
        inline Array(DType dtype, const std::vector<CType>& values, const std::vector<uint32_t>& dims = {}) {
            _setValues<CType>(dtype, std::span(values), dims);
        }

        template <typename CType>
        inline void _setValues(DType dtype, std::span<const CType> values, const std::vector<uint32_t>& dims = {}) {
            if (dims.empty()) {
                _setValues(dtype, values.data(), values.size());
            }
            else {
                _setValues(dtype, values.data(), dims.data(), dims.size());
            }
        }

        template <typename CType>
        std::span<CType> _getSpan() const;

    #else

        template <typename CType>
        inline Array(DType dtype, const std::vector<CType>& values, const std::vector<uint32_t>& dims = {}) {
            _setValues<CType>(dtype, values, dims);
        }

        template <typename CType>
        inline void _setValues(DType dtype, const std::vector<CType>& values, const std::vector<uint32_t>& dims = {}) {
            if (dims.empty()) {
                _setValues(dtype, values.data(), values.size());
            }
            else {
                _setValues(dtype, values.data(), dims.data(), dims.size());
            }
        }

    #endif

    template <typename CType>
    inline void _setValues(DType dtype, const CType * values, uint32_t count) {
        _setValues(dtype, values, &count, 1);
    }

    template <typename CType>
    void _setValues(DType dtype, const CType * values, const uint32_t * dims, dimct_t dimCount);

    template <typename CType>
    std::vector<CType> _getVector() const;

    template <typename CType>
    const CType& _getValueAt(size_t index) const;

    // STL Compatability

    template <typename CType>
    inline const CType& _at(size_t index) const {
        return _getValueAt<CType>(index);
    }

    template <typename CType>
    CType * _begin() const;

    template <typename CType>
    CType * _end() const;

}; // class Array

#ifdef __cpp_lib_span

    #define _MDSPLUS_ARRAY_BOOTSTRAP_STD(ArrayType)                \
                                                                   \
        inline ArrayType(                                          \
            std::span<const __ctype> values,                       \
            const std::vector<uint32_t>& dims = {}                 \
        )                                                          \
            : Array(__dtype, values, dims)                         \
        { }                                                        \
                                                                   \
        inline void setValues(                                     \
            std::span<const __ctype> values,                       \
            const std::vector<uint32_t>& dims = {}                 \
        ) {                                                        \
            _setValues<__ctype>(__dtype, values, dims);            \
        }                                                          \
                                                                   \
        inline ArrayType(                                          \
            const std::vector<__ctype> &values,                    \
            const std::vector<uint32_t> &dims = {}                 \
        )                                                          \
            : Array(__dtype, std::span(values), dims)              \
        { }                                                        \
                                                                   \
        inline void setValues(                                     \
            const std::vector<__ctype> &values,                    \
            const std::vector<uint32_t> &dims = {}                 \
        ) {                                                        \
            _setValues<__ctype>(__dtype, std::span(values), dims); \
        }                                                          \
                                                                   \
        inline std::span<__ctype> getSpan() const {                \
            return _getSpan<__ctype>();                            \
        }

#else

    #define _MDSPLUS_ARRAY_BOOTSTRAP_STD(ArrayType) \
                                                    \
        inline ArrayType(                           \
            const std::vector<__ctype> &values,     \
            const std::vector<uint32_t> &dims = {}  \
        )                                           \
            : Array(__dtype, values, dims)          \
        { }                                         \
                                                    \
        inline void setValues(                      \
            const std::vector<__ctype> &values,     \
            const std::vector<uint32_t> &dims = {}  \
        ) {                                         \
            _setValues(__dtype, values, dims);      \
        }

#endif // __cpp_lib_span

#define _MDSPLUS_ARRAY_BOOTSTRAP(ArrayType, CType, DataType)                       \
                                                                                   \
    typedef CType __ctype;                                                         \
    static constexpr DType __dtype = DataType;                                     \
                                                                                   \
    ArrayType() = default;                                                         \
                                                                                   \
    ArrayType(ArrayType &&) = default;                                             \
    ArrayType &operator=(ArrayType &&) = default;                                  \
                                                                                   \
    inline ArrayType(mdsdsc_xd_t &&xd, Tree *tree = nullptr)                       \
        : Array(std::move(xd), tree)                                               \
    { }                                                                            \
                                                                                   \
    inline ArrayType(                                                              \
        std::initializer_list<__ctype> values,                                     \
        const std::vector<uint32_t> &dims = {}                                     \
    )                                                                              \
        : Array(__dtype, std::vector<__ctype>(values.begin(), values.end()), dims) \
    { }                                                                            \
                                                                                   \
    inline const char * getClassName() const override {                            \
        return #ArrayType;                                                         \
    }                                                                              \
                                                                                   \
    [[nodiscard]]                                                                  \
    inline ArrayType clone() const {                                               \
        return _clone<ArrayType>();                                                \
    }                                                                              \
                                                                                   \
    [[nodiscard]]                                                                  \
    inline std::vector<__ctype> getVector() const {                                \
        return _getVector<__ctype>();                                              \
    }                                                                              \
                                                                                   \
    [[nodiscard]]                                                                  \
    inline std::vector<__ctype> getValues() const {                                \
        return getVector();                                                        \
    }                                                                              \
                                                                                   \
    [[nodiscard]]                                                                  \
    inline const __ctype &getValueAt(size_t index) const {                         \
        return _getValueAt<__ctype>(index);                                        \
    }                                                                              \
                                                                                   \
    [[nodiscard]]                                                                  \
    inline const __ctype &operator[](size_t index) const {                         \
        return _getValueAt<__ctype>(index);                                        \
    }                                                                              \
                                                                                   \
    [[nodiscard]]                                                                  \
    inline const __ctype& at(size_t index) const {                                 \
        return _at<__ctype>(index);                                                \
    }                                                                              \
                                                                                   \
    inline void setValues(const __ctype *values, uint32_t count) {                 \
        _setValues(__dtype, values, count);                                        \
    }                                                                              \
                                                                                   \
    void setValues(                                                                \
        const __ctype *values,                                                     \
        const uint32_t *dims,                                                      \
        dimct_t dimCount                                                           \
    ) {                                                                            \
        _setValues(__dtype, values, dims, dimCount);                               \
    }                                                                              \
                                                                                   \
    [[nodiscard]]                                                                  \
    inline __ctype * begin() const {                                               \
        return _begin<__ctype>();                                                  \
    }                                                                              \
                                                                                   \
    [[nodiscard]]                                                                  \
    inline __ctype * end() const {                                                 \
        return _end<__ctype>();                                                    \
    }                                                                              \
                                                                                   \
    _MDSPLUS_ARRAY_BOOTSTRAP_STD(ArrayType)

class Int8Array : public Array
{
public:

    _MDSPLUS_ARRAY_BOOTSTRAP(Int8Array, int8_t, DType::B)

    template <typename ResultType = Data>
    [[nodiscard]]
    ResultType deserialize();

}; // class Int8Array

class UInt8Array : public Array
{
public:

    _MDSPLUS_ARRAY_BOOTSTRAP(UInt8Array, uint8_t, DType::BU)

    template <typename ResultType = Data>
    [[nodiscard]]
    ResultType deserialize();

}; // class Int8Array

class Int16Array : public Array
{
public:

    _MDSPLUS_ARRAY_BOOTSTRAP(Int16Array, int16_t, DType::W)

}; // class Int16Array

class UInt16Array : public Array
{
public:

    _MDSPLUS_ARRAY_BOOTSTRAP(UInt16Array, uint16_t, DType::WU)

}; // class UInt16Array

class Int32Array : public Array
{
public:

    _MDSPLUS_ARRAY_BOOTSTRAP(Int32Array, int32_t, DType::L)

}; // class Int32Array

class UInt32Array : public Array
{
public:

    _MDSPLUS_ARRAY_BOOTSTRAP(UInt32Array, uint32_t, DType::LU)

}; // class UInt32Array

class Int64Array : public Array
{
public:

    _MDSPLUS_ARRAY_BOOTSTRAP(Int64Array, int64_t, DType::Q)

}; // class Int64Array

class UInt64Array : public Array
{
public:

    _MDSPLUS_ARRAY_BOOTSTRAP(UInt64Array, uint64_t, DType::QU)

}; // class UInt64Array

class Float32Array : public Array
{
public:

    _MDSPLUS_ARRAY_BOOTSTRAP(Float32Array, float, DType::FS)

}; // class Float32Array

class Float64Array : public Array
{
public:

    _MDSPLUS_ARRAY_BOOTSTRAP(Float64Array, double, DType::FT)

}; // class Float64Array

class Complex32Array : public Array
{
public:

    _MDSPLUS_ARRAY_BOOTSTRAP(Complex32Array, std::complex<float>, DType::FSC)

}; // class Complex32Array

class Complex64Array : public Array
{
public:

    _MDSPLUS_ARRAY_BOOTSTRAP(Complex64Array, std::complex<double>, DType::FTC)

}; // class Complex64Array

#ifdef __cpp_lib_span

    #define MDSPLUS_ARRAY_CUSTOMIZATION_SPAN(ArrayType)    \
        template <>                                        \
        inline Data Data::FromArray(                       \
            std::span<const ArrayType::__ctype> values,    \
            const std::vector<uint32_t>& dims /*= {}*/     \
        ) {                                                \
            return ArrayType(values, dims);                \
        }                                                  \
                                                           \
        template <>                                        \
        inline Data Data::FromArray(                       \
            const std::vector<ArrayType::__ctype>& values, \
            const std::vector<uint32_t>& dims /*= {}*/     \
        ) {                                                \
            return ArrayType(std::span(values), dims);     \
        }

#else

    #define MDSPLUS_ARRAY_CUSTOMIZATION_SPAN(ArrayType)    \
                                                           \
        template <>                                        \
        inline Data Data::FromArray(                       \
            const std::vector<ArrayType::__ctype>& values, \
            const std::vector<uint32_t>& dims /*= {}*/     \
        ) {                                                \
            return ArrayType(values, dims);                \
        }

#endif

#define MDSPLUS_ARRAY_CUSTOMIZATION(ArrayType)                     \
                                                                   \
    template <>                                                    \
    inline ArrayType Data::releaseAndConvert() {                   \
        return _convertToArray<ArrayType>();                       \
    }                                                              \
                                                                   \
    template <>                                                    \
    inline std::vector<ArrayType::__ctype> Data::getData() const { \
        return getData<ArrayType>().getValues();                   \
    }                                                              \
                                                                   \
    MDSPLUS_ARRAY_CUSTOMIZATION_SPAN(ArrayType)

MDSPLUS_ARRAY_CUSTOMIZATION(Int8Array)
MDSPLUS_ARRAY_CUSTOMIZATION(Int16Array)
MDSPLUS_ARRAY_CUSTOMIZATION(Int32Array)
MDSPLUS_ARRAY_CUSTOMIZATION(Int64Array)
MDSPLUS_ARRAY_CUSTOMIZATION(UInt8Array)
MDSPLUS_ARRAY_CUSTOMIZATION(UInt16Array)
MDSPLUS_ARRAY_CUSTOMIZATION(UInt32Array)
MDSPLUS_ARRAY_CUSTOMIZATION(UInt64Array)
MDSPLUS_ARRAY_CUSTOMIZATION(Float32Array)
MDSPLUS_ARRAY_CUSTOMIZATION(Float64Array)
MDSPLUS_ARRAY_CUSTOMIZATION(Complex32Array)
MDSPLUS_ARRAY_CUSTOMIZATION(Complex64Array)

class APD : public Array
{
public:

    static constexpr Class __class = Class::APD;

    APD() = default;

    inline APD(mdsdsc_xd_t && dsc, Tree * tree = nullptr)
        : Array(std::move(dsc), tree)
    { }

    APD(APD&&) = default;
    APD& operator=(APD&&) = default;

    template <typename ...ArgTypes>
    inline APD(DType dtype, const ArgTypes& ...args) {
        _append(dtype, args...);
    }

protected:

    inline mdsdsc_t ** _getDescriptorArray() const {
        return reinterpret_cast<mdsdsc_t **>(getPointer());
    }

    template <typename ...ArgTypes>
    inline void _append(DType dtype, const ArgTypes& ...args)
    {
        std::vector<DataView> argList = { DataView(args)... };

        std::vector<mdsdsc_t *> dscList;
        for (size_t i = 0; i < argList.size(); ++i) {
            dscList.push_back(argList[i].getDescriptor());
        }

        _append(dtype, dscList);
    }

    inline void _append(DType dtype, std::vector<mdsdsc_t *> values)
    {
        std::vector<mdsdsc_t *> dscList = _getVector<mdsdsc_t *>();
        dscList.reserve(dscList.size() + values.size());
        dscList.insert(dscList.end(), values.begin(), values.end());

        DESCRIPTOR_APD(dsc, dtype_t(dtype), dscList.data(), 0);

        // TODO: Replace the 0 above with dscList.size() once a cast to arsize_t is added
        dsc.arsize = arsize_t(dscList.size() * sizeof(mdsdsc_t *));

        mdsdsc_xd_t tmpXD = std::move(_xd);

        int status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        MdsFree1Dx(&tmpXD, nullptr);
    }

}; // class APD

class List : public APD
{
public:

    static constexpr DType __dtype = DType::List;

    inline List(mdsdsc_xd_t && dsc, Tree * tree = nullptr)
        : APD(std::move(dsc), tree)
    { }

    template <typename ...ArgTypes>
    inline List(const ArgTypes& ...args)
        : APD(__dtype, args...)
    { }

    template <typename DataType>
    inline List(const std::vector<DataType>& values)
    {
        std::vector<mdsdsc_t *> dscList;
        for (size_t i = 0; i < values.size(); ++i) {
            dscList.push_back(values[i].getDescriptor());
        }

        _append(__dtype, dscList);
    }

    inline const char * getClassName() const override {
        return "List";
    }

    template <typename ...ArgTypes>
    inline void append(ArgTypes ...args) {
        _append(__dtype, args...);
    }

    inline std::vector<Data> getValues() const
    {
        int status;

        #ifdef __cpp_lib_span
            std::span<mdsdsc_t *> dscList = _getSpan<mdsdsc_t *>();
        #else
            std::vector<mdsdsc_t *> dscList = _getVector<mdsdsc_t *>();
        #endif

        std::vector<Data> values;
        for (mdsdsc_t * dsc : dscList) {
            mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
            status = MdsCopyDxXd(dsc, &out);
            if (IS_NOT_OK(status)) {
                throwException(status);
            }

            values.emplace_back(Data(std::move(out), getTree()));
        }

        return std::move(values);
    }

    // STL Compatibility

    struct Iterator
    {
    public:

        Iterator(size_t index, const List * list)
            : _index(index)
            , _list(list)
        { }

        inline Data operator*() const
        {
            mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
            mdsdsc_a_t * dsc = _list->getArrayDescriptor();

            mdsdsc_t * tmp = _list->_getDescriptorArray()[_index];
            int status = MdsCopyDxXd(tmp, &out);
            if (IS_NOT_OK(status)) {
                throwException(status);
            }

            return Data(std::move(out), _list->getTree());
        }

        inline mdsdsc_t * operator->() const {
            return _list->_getDescriptorArray()[_index];
        }

        inline Iterator& operator++() {
            _index++;
            return *this;
        }

        inline Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        inline friend bool operator==(const Iterator& a, const Iterator& b) {
            return (a._index == b._index && a._list == b._list);
        };

        inline friend bool operator!=(const Iterator& a, const Iterator& b) {
            return (a._index != b._index || a._list != b._list);
        };

    private:

        size_t _index;

        const List * _list;

    };

    inline Iterator begin() const {
        return Iterator(0, this);
    }

    inline Iterator end() const {
        return Iterator(getSize(), this);
    }

}; // class List

class Dictionary : public APD
{
public:

    static constexpr DType __dtype = DType::Dictionary;

    inline Dictionary(mdsdsc_xd_t && dsc, Tree * tree = nullptr)
        : APD(std::move(dsc), tree)
    { }

    template <typename KeyType, typename ValueType,
        typename std::enable_if<is_valid_ctype<KeyType>::value, bool>::type = true,
        typename std::enable_if<is_valid_ctype<ValueType>::value, bool>::type = true>
    inline Dictionary(const std::map<KeyType, ValueType>& map)
    {
        std::vector<Data> values;
        for (const auto& it : map) {
            values.emplace_back(Data::FromScalar(it->first));
            values.emplace_back(Data::FromScalar(it->second));
        }
    }

    inline Dictionary(const std::vector<Data>& keys, const std::vector<Data>& values)
    {
        assert(keys.size() == values.size());

        std::vector<mdsdsc_t *> dscList = _getVector<mdsdsc_t *>();
        for (size_t i = 0; i < keys.size(); ++i) {
            dscList.push_back(keys[i].getDescriptor());
            dscList.push_back(values[i].getDescriptor());
        }

        DESCRIPTOR_APD(dsc, dtype_t(__dtype), dscList.data(), 0);

        // TODO: Replace the 0 above with dscList.size() once a cast to arsize_t is added
        dsc.arsize = arsize_t(dscList.size() * sizeof(mdsdsc_t *));

        int status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }
    }

    template <typename ...ArgTypes,
        typename std::enable_if<(sizeof...(ArgTypes) % 2) == 0, bool>::type = true>
    inline Dictionary(const ArgTypes& ...args)
        : APD(__dtype, args...)
    { }

    inline const char * getClassName() const override {
        return "Dictionary";
    }

    template <typename KeyType, typename ValueType>
    void append(KeyType key, ValueType value) {
        _append(__dtype, key, value);
    }

    inline std::tuple<std::vector<Data>, std::vector<Data>> getKeysAndValues() const {
        int status;

        #ifdef __cpp_lib_span
            std::span<mdsdsc_t *> dscList = _getSpan<mdsdsc_t *>();
        #else
            std::vector<mdsdsc_t *> dscList = _getVector<mdsdsc_t *>();
        #endif

        assert((dscList.size() % 2) == 0);

        std::vector<Data> keys;
        std::vector<Data> values;
        for (size_t i = 0; i < dscList.size(); i += 2) {
            mdsdsc_xd_t outKey = MDSDSC_XD_INITIALIZER;
            mdsdsc_xd_t outValue = MDSDSC_XD_INITIALIZER;

            status = MdsCopyDxXd(dscList[i], &outKey);
            if (IS_NOT_OK(status)) {
                throwException(status);
            }

            status = MdsCopyDxXd(dscList[i + 1], &outValue);
            if (IS_NOT_OK(status)) {
                throwException(status);
            }

            // keys.emplace(std::move(outKey), getTree());
            // values.emplace(std::move(outValue), getTree());
        }

        return { std::move(keys), std::move(values) };
    }

    template <typename KeyType = Data, typename ValueType = Data,
        typename std::enable_if<std::is_base_of<Data, KeyType>::value, bool>::type = true,
        typename std::enable_if<std::is_base_of<Data, ValueType>::value, bool>::type = true>
    inline std::unordered_map<KeyType, ValueType> getUnorderedMap() const {
        int status;

        #ifdef __cpp_lib_span
            std::span<mdsdsc_t *> dscList = _getSpan<mdsdsc_t *>();
        #else
            std::vector<mdsdsc_t *> dscList = _getVector<mdsdsc_t *>();
        #endif

        assert((dscList.size() % 2) == 0);

        std::unordered_map<KeyType, ValueType> values;
        for (size_t i = 0; i < dscList.size(); i += 2) {
            mdsdsc_xd_t outKey = MDSDSC_XD_INITIALIZER;
            mdsdsc_xd_t outValue = MDSDSC_XD_INITIALIZER;

            status = MdsCopyDxXd(dscList[i], &outKey);
            if (IS_NOT_OK(status)) {
                throwException(status);
            }

            status = MdsCopyDxXd(dscList[i + 1], &outValue);
            if (IS_NOT_OK(status)) {
                throwException(status);
            }

            values.emplace(
                Data(std::move(outKey), getTree()).releaseAndConvert<KeyType>(),
                Data(std::move(outValue), getTree()).releaseAndConvert<ValueType>()
            );
        }

        return std::move(values);
    }

}; // class Dictionary

// TODO: Tuple

class Record : public Data
{
public:

    static constexpr Class __class = Class::R;

    Record() = default;

    inline Record(mdsdsc_xd_t && dsc, Tree * tree = nullptr)
        : Data(std::move(dsc), tree)
    { }

    Record(Record&&) = default;
    Record& operator=(Record&&) = default;

    virtual ~Record() = default;

    [[nodiscard]]
    inline mdsdsc_r_t * getRecordDescriptor() const {
        return reinterpret_cast<mdsdsc_r_t *>(_xd.pointer);
    }

    inline size_t getNumDescriptors() const {
        mdsdsc_r_t * dsc = getRecordDescriptor();
        return (dsc ? dsc->ndesc : 0);
    }

    inline bool hasDescriptorAt(size_t index) const {
        mdsdsc_r_t * dsc = getRecordDescriptor();
        return (dsc && index < dsc->ndesc && dsc->dscptrs[index] != nullptr);
    }

    template <typename ResultType = Data>
    inline ResultType getDescriptorAt(size_t index) const {
        mdsdsc_r_t * dsc = getRecordDescriptor();

        if (dsc && index < dsc->ndesc) {
            mdsdsc_xd_t xd = MDSDSC_XD_INITIALIZER;
            int status = MdsCopyDxXd(dsc->dscptrs[index], &xd);
            if (IS_NOT_OK(status)) {
                throwException(status);
            }

            mdsdsc_t * newDsc = xd.pointer;
            if (Class(newDsc->class_) == ResultType::__class && DType(newDsc->dtype) == ResultType::__dtype) {
                return ResultType(std::move(xd), getTree());
            }

            return Data(std::move(xd), getTree()).releaseAndConvert<ResultType>();
        }

        return ResultType();
    }

}; // class Record

#define MDSPLUS_RECORD_BOOTSTRAP(RecordType, DataType)           \
                                                                 \
    static constexpr DType __dtype = DataType;                   \
                                                                 \
    RecordType() = default;                                      \
                                                                 \
    inline RecordType(mdsdsc_xd_t && dsc, Tree * tree = nullptr) \
        : Record(std::move(dsc), tree)                           \
    { }                                                          \
                                                                 \
    RecordType(RecordType &&) = default;                         \
    RecordType &operator=(RecordType &&) = default;              \
                                                                 \
    inline const char * getClassName() const override {          \
        return #RecordType;                                      \
    }                                                            \
                                                                 \
    [[nodiscard]]                                                \
    inline RecordType clone() const {                            \
        return _clone<RecordType>();                             \
    }

#define MDSPLUS_RECORD_CUSTOMIZATION(RecordType)                 \
    template <>                                                  \
    inline RecordType Data::releaseAndConvert()                  \
    {                                                            \
        mdsdsc_xd_t xd = release();                              \
        mdsdsc_t * dsc = xd.pointer;                             \
                                                                 \
        if (dsc->class_ == class_t(RecordType::__class) &&       \
            dsc->dtype == dtype_t(RecordType::__dtype)) {        \
            return RecordType(std::move(xd), getTree());         \
        }                                                        \
                                                                 \
        MdsFree1Dx(&xd, nullptr);                                \
        throw TdiInvalidDataType();                              \
    }

class Param : public Record
{
public:

    MDSPLUS_RECORD_BOOTSTRAP(Param, DType::Param)

    template <
        typename ValueType,
        typename HelpType,
        typename ValidationType
    >
    Param(
        const ValueType& value,
        const HelpType& help,
        const ValidationType& validation
    ) {
        DataView argValue(value);
        DataView argHelp(help);
        DataView argValidation(validation);

        DESCRIPTOR_PARAM(dsc,
            argValue.getDescriptor(),
            argHelp.getDescriptor(),
            argValidation.getDescriptor()
        );

        int status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }
    }

    template <typename ValueType = Data>
    [[nodiscard]]
    inline ValueType getValue() const {
        return getDescriptorAt<ValueType>(0);
    }

    template <typename HelpType = Data>
    [[nodiscard]]
    inline HelpType getHelp() const {
        return getDescriptorAt<HelpType>(1);
    }

    template <typename ValidationType = Data>
    [[nodiscard]]
    inline ValidationType getValidation() const {
        return getDescriptorAt<ValidationType>(2);
    }

}; // class Param

MDSPLUS_RECORD_CUSTOMIZATION(Param)

class Signal : public Record
{
public:

    MDSPLUS_RECORD_BOOTSTRAP(Signal, DType::Signal)

    template <
        typename ValueType,
        typename RawType,
        typename DimensionType
    >
    Signal(
        const ValueType& value,
        const RawType& raw,
        const DimensionType& dimension = {}
    ) {
        DataView argValue(value);
        DataView argRaw(raw);
        DataView argDimension(dimension);

        DESCRIPTOR_SIGNAL_1(dsc,
            argValue.getDescriptor(),
            argRaw.getDescriptor(),
            argDimension.getDescriptor()
        );

        int status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }
    }

    template <
        typename ValueType,
        typename RawType,
        typename ...DimensionTypes
    >
    Signal(
        const ValueType& value,
        const RawType& raw,
        const DimensionTypes& ...dimensions
    ) {
        DataView argValue(value);
        DataView argRaw(raw);
        std::vector<DataView> argDimensions = { DataView(dimensions)... };

        DESCRIPTOR_SIGNAL(dsc,
            sizeof...(dimensions),
            argValue.getDescriptor(),
            argRaw.getDescriptor()
        );

        for (size_t i = 0; i < argDimensions.size(); ++i) {
            dsc.dimensions[i] = argDimensions[i].getDescriptor();
        }

        int status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }
    }

    template <typename ValueType = Data>
    [[nodiscard]]
    inline ValueType getValue() const {
        return getDescriptorAt<ValueType>(0);
    }

    template <typename RawType = Data>
    [[nodiscard]]
    inline RawType getRaw() const {
        return getDescriptorAt<RawType>(1);
    }

    template <typename DimensionType = Data>
    [[nodiscard]]
    inline DimensionType getDimensionAt(size_t index = 0) const {
        return getDescriptorAt<DimensionType>(2 + index);
    }

    template <typename DimensionType = Data>
    [[nodiscard]]
    inline std::vector<DimensionType> getDimensions() const {
        std::vector<DimensionType> dims;
        size_t count = getNumDescriptors() - 2;
        for (size_t i = 0; i < count; ++i) {
            dims.push_back(getDimensionAt(i));
        }
        return dims;
    }

}; // class Signal

MDSPLUS_RECORD_CUSTOMIZATION(Signal)

class Dimension : public Record
{
public:

    MDSPLUS_RECORD_BOOTSTRAP(Dimension, DType::Dimension)

    template <typename WindowType, typename AxisType>
    Dimension(const WindowType& window, const AxisType& axis)
    {
        DataView tmpWindow(window);
        DataView tmpAxis(axis);

        DESCRIPTOR_DIMENSION(dsc,
            tmpWindow.getDescriptor(),
            tmpAxis.getDescriptor()
        );

        int status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }
    }

    template <typename WindowType = Data>
    [[nodiscard]]
    inline WindowType getWindow() const {
        return getDescriptorAt<WindowType>(0);
    }

    template <typename AxisType = Data>
    [[nodiscard]]
    inline AxisType getAxis() const {
        return getDescriptorAt<AxisType>(1);
    }

}; // class Dimension

MDSPLUS_RECORD_CUSTOMIZATION(Dimension)

class Window : public Record
{
public:

    MDSPLUS_RECORD_BOOTSTRAP(Window, DType::Window)

    template <
        typename StartIndexType,
        typename EndIndexType,
        typename ValueType
    >
    Window(
        const StartIndexType& startIndex,
        const EndIndexType& endIndex,
        const ValueType& valueAtIndex0
    ) {
        DataView argStartIndex(startIndex);
        DataView argEndIndex(endIndex);
        DataView argValueAtIndex0(valueAtIndex0);

        DESCRIPTOR_WINDOW(dsc,
            argStartIndex.getDescriptor(),
            argEndIndex.getDescriptor(),
            argValueAtIndex0.getDescriptor()
        );

        int status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }
    }

    template <typename StartIndexType = Data>
    [[nodiscard]]
    inline StartIndexType getStartIndex() const {
        return getDescriptorAt<StartIndexType>(0);
    }

    template <typename EndIndexType = Data>
    [[nodiscard]]
    inline EndIndexType getEndIndex() const {
        return getDescriptorAt<EndIndexType>(1);
    }

    template <typename ValueType = Data>
    [[nodiscard]]
    inline ValueType getValueAtIndex0() const {
        return getDescriptorAt<ValueType>(2);
    }

}; // class Window

MDSPLUS_RECORD_CUSTOMIZATION(Window)

class Function : public Record
{
public:

    MDSPLUS_RECORD_BOOTSTRAP(Function, DType::Function)

    template <typename ...ArgTypes>
    Function(opcode_t opcode, const ArgTypes& ...args)
    {
        // TODO: #define MAX_ARGS 255 ?
        static_assert(sizeof...(args) <= 255, "Function's are limited to 254 arguments");

        std::vector<DataView> argList = { DataView(args)... };

        DESCRIPTOR_FUNCTION(dsc, &opcode, sizeof...(args));

        for (size_t i = 0; i < argList.size(); ++i) {
            dsc.arguments[i] = argList[i].getDescriptor();
        }

        int status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }
    }

    Data call() const;

}; // class Function

MDSPLUS_RECORD_CUSTOMIZATION(Function)

// TODO: Conglom?

class Range : public Record
{
public:

    MDSPLUS_RECORD_BOOTSTRAP(Range, DType::Range)

    template <
        typename BeginType = Data,
        typename EndingType = Data,
        typename DeltaType = Data
    >
    Range(
        const BeginType& begin,
        const EndingType& ending,
        const DeltaType& delta
    ) {
        DataView argBegin(begin);
        DataView argEnding(ending);
        DataView argDelta(delta);

        DESCRIPTOR_RANGE(dsc,
            argBegin.getDescriptor(),
            argEnding.getDescriptor(),
            argDelta.getDescriptor()
        );

        int status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }
    }

    template <typename BeginType = Data>
    [[nodiscard]]
    inline BeginType getBegin() const {
        return getDescriptorAt<BeginType>(0);
    }

    template <typename EndingType = Data>
    [[nodiscard]]
    inline EndingType getEnding() const {
        return getDescriptorAt<EndingType>(1);
    }

    template <typename DeltaType = Data>
    [[nodiscard]]
    inline DeltaType getDelta() const {
        return getDescriptorAt<DeltaType>(2);
    }

    // TODO: isFinite() ?
    // TODO: begin/end/size

}; // class Range

MDSPLUS_RECORD_CUSTOMIZATION(Range)

// TODO: Action
// TODO: Dispatch
// TODO: Routine?
// TODO: Method?
// TODO: Event?

class WithUnits : public Record
{
public:

    MDSPLUS_RECORD_BOOTSTRAP(WithUnits, DType::WithUnits)

    template <typename ValueType, typename UnitsType>
    WithUnits(const ValueType& value, const UnitsType& units)
    {
        DataView argValue(value);
        DataView argUnits(units);

        DESCRIPTOR_WITH_UNITS(dsc,
            argValue.getDescriptor(),
            argUnits.getDescriptor()
        );

        int status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }
    }

    template <typename ValueType = Data>
    [[nodiscard]]
    inline ValueType getValue() const {
        return getDescriptorAt<ValueType>(0);
    }

    template <typename UnitsType = Data>
    [[nodiscard]]
    inline UnitsType getUnits() const {
        return getDescriptorAt<UnitsType>(1);
    }

}; // class WithUnits

MDSPLUS_RECORD_CUSTOMIZATION(WithUnits)

// TODO: Call?

class WithError : public Record
{
public:

    MDSPLUS_RECORD_BOOTSTRAP(WithError, DType::WithError)

    template <typename ValueType, typename ErrorType>
    WithError(const ValueType& value, const ErrorType& error)
    {
        DataView argValue(value);
        DataView argError(error);

        DESCRIPTOR_WITH_ERROR(dsc,
            argValue.getDescriptor(),
            argError.getDescriptor()
        );

        int status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }
    }

    template <typename ValueType = Data>
    [[nodiscard]]
    inline ValueType getValue() const {
        return getDescriptorAt<ValueType>(0);
    }

    template <typename ErrorType = Data>
    [[nodiscard]]
    inline ErrorType getError() const {
        return getDescriptorAt<ErrorType>(1);
    }

}; // class WithError

MDSPLUS_RECORD_CUSTOMIZATION(WithError)

// TODO: Opaque?

enum class Usage : uint8_t
{
    Any = TreeUSAGE_ANY,
    Structure = TreeUSAGE_STRUCTURE,
    Action = TreeUSAGE_ACTION,
    Device = TreeUSAGE_DEVICE,
    Dispatch = TreeUSAGE_DISPATCH,
    Numeric = TreeUSAGE_NUMERIC,
    Signal = TreeUSAGE_SIGNAL,
    Task = TreeUSAGE_TASK,
    Text = TreeUSAGE_TEXT,
    Window = TreeUSAGE_WINDOW,
    Axis = TreeUSAGE_AXIS,
    Subtree = TreeUSAGE_SUBTREE,
    CompoundData = TreeUSAGE_COMPOUND_DATA,

}; // enum class Usage

std::string to_string(const Usage& usage);

struct TreeNodeFlags
{
    uint32_t State : 1;
    uint32_t ParentState : 1;
    uint32_t Essential : 1;
    uint32_t Cached : 1;
    uint32_t Versions : 1;
    uint32_t Segmented : 1;
    uint32_t SetupInformation : 1;
    uint32_t WriteOnce : 1;
    uint32_t Compressible : 1;
    uint32_t DoNotCompress : 1;
    uint32_t CompressOnPut : 1;
    uint32_t NoWriteModel : 1;
    uint32_t NoWriteShot : 1;
    uint32_t PathReference : 1;
    uint32_t NidReference : 1;
    uint32_t IncludeInPulse : 1;
    uint32_t CompressSegments : 1;
    uint32_t : 15;

}; // struct TreeNodeFlags

static_assert(sizeof(TreeNodeFlags) == sizeof(uint32_t),
    "sizeof(TreeNodeFlags) != sizeof(uint32_t)");

std::string to_string(const TreeNodeFlags& flags);

class Tree;

class TreeNode
{
public:

    TreeNode() = default;

    inline TreeNode(Tree * tree, int nid)
        : _tree(tree)
        , _nid(nid)
    { }

    virtual ~TreeNode() = default;

    [[nodiscard]]
    inline Tree * getTree() const {
        return _tree;
    }

    [[nodiscard]]
    void * getDBID() const;

    [[nodiscard]]
    void * getContext() const;

    [[nodiscard]]
    inline int getNID() const {
        return _nid;
    }

    TreeNode getNode(const std::string& path) const;

    TreeNode addNode(const std::string& path, Usage usage) const;

    TreeNode addDevice(const std::string& path, const std::string& model) const;

    [[nodiscard]]
    inline uint64_t getTimeInserted() const {
        return _getNCI<uint64_t>(NciTIME_INSERTED);
    }

    [[nodiscard]]
    inline uint32_t getOwnerID() const {
        return _getNCI<uint32_t>(NciOWNER_ID);
    }

    [[nodiscard]]
    inline class_t getClass() const {
        return _getNCI<class_t>(NciCLASS);
    }

    [[nodiscard]]
    inline dtype_t getDType() const {
        return _getNCI<dtype_t>(NciDTYPE);
    }

    [[nodiscard]]
    inline uint32_t getLength() const {
        return _getNCI<uint32_t>(NciLENGTH);
    }

    [[nodiscard]]
    inline uint32_t getStatus() const {
        return _getNCI<uint32_t>(NciSTATUS);
    }

    [[nodiscard]]
    inline uint16_t getConglomorateElements() const {
        return _getNCI<uint16_t>(NciCONGLOMERATE_ELT);
    }

    [[nodiscard]]
    inline TreeNodeFlags getFlags() const {
        return _getNCI<TreeNodeFlags>(NciGET_FLAGS);
    }

    inline void setFlagsOn(TreeNodeFlags flags) const {

    }

    inline void setFlagsOff(TreeNodeFlags flags) const {

    }

    inline void clearFlags(TreeNodeFlags flags) const {
        setFlagsOff(flags);
    }

    [[nodiscard]]
    inline uint32_t getFlagsInt() const {
        return _getNCI<uint32_t>(NciGET_FLAGS);
    }

    [[nodiscard]]
    std::string getNodeName() const;

    [[nodiscard]]
    inline std::string getPath() const {
        return _getStringNCI(NciPATH, 1024);
    }

    [[nodiscard]]
    inline uint32_t getDepth() const {
        return _getNCI<uint32_t>(NciDEPTH);
    }

    [[nodiscard]]
    inline int getParentNID() const {
        return _getNCI<uint32_t>(NciPARENT);
    }

    [[nodiscard]]
    inline TreeNode getParent() const {
        int nid = getParentNID();
        if (nid == 0) {
            throw TreeNodeNotFound();
        }
        return TreeNode(_tree, nid);
    }

    [[nodiscard]]
    inline int getBrotherNID() const {
        return _getNCI<uint32_t>(NciPARENT);
    }

    [[nodiscard]]
    inline TreeNode getBrother() const {
        int nid = getBrotherNID();
        if (nid == 0) {
            throw TreeNodeNotFound();
        }
        return TreeNode(_tree, nid);
    }

    [[nodiscard]]
    inline int getMemberID() const {
        return _getNCI<uint32_t>(NciPARENT);
    }

    [[nodiscard]]
    inline TreeNode getMember() const {
        int nid = getMemberID();
        if (nid == 0) {
            throw TreeNodeNotFound();
        }
        return TreeNode(_tree, nid);
    }

    [[nodiscard]]
    inline int getChildNID() const {
        return _getNCI<uint32_t>(NciPARENT);
    }

    [[nodiscard]]
    inline TreeNode getChild() const {
        int nid = getChildNID();
        if (nid == 0) {
            throw TreeNodeNotFound();
        }
        return TreeNode(_tree, nid);
    }

    [[nodiscard]]
    inline ncik_t getParentRelationship() const {
        return _getNCI<ncik_t>(NciPARENT_RELATIONSHIP);
    }

    [[nodiscard]]
    inline std::vector<int> getConglomerateElementNIDs() const {
        return _getNIDArrayNCI(NciCONGLOMERATE_NIDS, NciNUMBER_OF_ELTS);
    }

    [[nodiscard]]
    inline std::vector<TreeNode> getConglomerateElements() const {
        return _getTreeNodeArrayNCI(NciCONGLOMERATE_NIDS, NciNUMBER_OF_ELTS);
    }

    [[nodiscard]]
    inline std::string getOriginalPartName() const {
        return _getStringNCI(NciORIGINAL_PART_NAME, 1024);
    }

    [[nodiscard]]
    inline uint32_t getNumberOfMembers() const {
        return _getNCI<uint32_t>(NciNUMBER_OF_MEMBERS);
    }

    [[nodiscard]]
    inline uint32_t getNumberOfChildren() const {
        return _getNCI<uint32_t>(NciNUMBER_OF_CHILDREN);
    }

    [[nodiscard]]
    inline std::vector<int> getMemberNIDs() const {
        return _getNIDArrayNCI(NciMEMBER_NIDS, NciNUMBER_OF_MEMBERS);
    }

    [[nodiscard]]
    inline std::vector<TreeNode> getMembers() const {
        return _getTreeNodeArrayNCI(NciMEMBER_NIDS, NciNUMBER_OF_MEMBERS);
    }

    [[nodiscard]]
    inline std::vector<int> getChildrenNIDs() const {
        return _getNIDArrayNCI(NciCHILDREN_NIDS, NciNUMBER_OF_CHILDREN);
    }

    [[nodiscard]]
    inline std::vector<TreeNode> getChildren() const {
        return _getTreeNodeArrayNCI(NciCHILDREN_NIDS, NciNUMBER_OF_CHILDREN);
    }

    [[nodiscard]]
    std::vector<TreeNode> getMembersAndChildren(bool sortedNIDs = true) const;

    [[nodiscard]]
    inline std::string getFullPath() const {
        return _getStringNCI(NciFULLPATH, 1024);
    }

    [[nodiscard]]
    inline std::string getMinPath() const {
        return _getStringNCI(NciMINPATH, 1024);
    }

    [[nodiscard]]
    inline usage_t getUsage() const {
        return _getNCI<usage_t>(NciUSAGE);
    }

    // setUsage

    [[nodiscard]]
    inline std::string getParentTreeName() const {
        return _getStringNCI(NciPARENT_TREE, 64);
    }

    [[nodiscard]]
    inline uint32_t getRecordLength() const {
        return _getNCI<uint32_t>(NciRLENGTH);
    }

    [[nodiscard]]
    inline uint32_t getNumberOfElements() const {
        return _getNCI<uint32_t>(NciNUMBER_OF_ELTS);
    }

    [[nodiscard]]
    inline bool hasDataInNCI() const {
        return _getNCI<bool>(NciDATA_IN_NCI);
    }

    [[nodiscard]]
    inline bool hasErrorOnPut() const {
        return _getNCI<bool>(NciERROR_ON_PUT);
    }

    [[nodiscard]]
    inline uint64_t getRFA() const {
        return _getNCI<uint64_t>(NciRFA);
    }

    [[nodiscard]]
    inline uint32_t getIOStatus() const {
        return _getNCI<uint32_t>(NciIO_STATUS);
    }

    [[nodiscard]]
    inline uint32_t getIOSTV() const {
        return _getNCI<uint32_t>(NciIO_STV);
    }

    [[nodiscard]]
    inline std::string getDTypeString() const {
        return _getStringNCI(NciDTYPE_STR, 64);
    }

    [[nodiscard]]
    inline std::string getUsageString() const {
        return _getStringNCI(NciUSAGE_STR, 64);
    }

    [[nodiscard]]
    inline std::string getClassString() const {
        return _getStringNCI(NciCLASS_STR, 64);
    }

    [[nodiscard]]
    inline uint32_t getVersion() const {
        return _getNCI<uint32_t>(NciVERSION);
    }

    [[nodiscard]]
    inline uint8_t getCompressionMethod() const {
        return _getNCI<uint8_t>(NciCOMPRESSION_METHOD);
    }

    // setCompressionMethod?

    [[nodiscard]]
    inline std::string getCompressionMethodString() const {
        return _getStringNCI(NciCOMPRESSION_METHOD_STR, 64);
    }

    // setCompressionMethodString?

    // template <typename T = Data>
    // [[nodiscard]]
    // inline T getExtendedAttribute(const std::string& name) {
    //     _TreeSetXNci(void *dbid, int nid, const char *xnciname, mdsdsc_t *value)
    // }

    // void setExtendedAttribute(const std::string& name, const Data& data);

    [[nodiscard]]
    Data getRecord() const;

    void putRecord(const Data& data) const;

    template <typename DataType = Data>
    [[nodiscard]]
    DataType getData() const;

    template <typename DataType = Data, typename UnitsType = Data>
    [[nodiscard]]
    std::tuple<DataType, UnitsType> getDataWithUnits() const;

    template <typename DataType>
    inline void setData(DataType value) const {
        putRecord(Data::FromScalar(value));
    }

    template <typename DataType, typename UnitsType>
    inline void setDataWithUnits(DataType value, UnitsType units) const {
        putRecord(WithUnits(Data::FromScalar(value), Data::FromScalar(units)));
    }

#ifdef __cpp_lib_span

    template <typename DataType>
    void setArrayData(const std::span<const DataType> values, const std::vector<uint32_t>& dims = {}) const {
        if (dims.empty()) {
            return setArrayData(values.data(), { values.size() });
        }
        return setArrayData(values.data(), dims);
    }

    template <typename DataType, typename UnitsType>
    void setArrayDataWithUnits(const std::span<const DataType> values, UnitsType units, const std::vector<uint32_t>& dims = {}) const {
        if (dims.empty()) {
            return setArrayDataWithUnits(values.data(), units, { values.size() });
        }
        return setArrayDataWithUnits(values.data(), units, dims);
    }

#endif

    template <typename DataType>
    inline void setArrayData(std::vector<DataType>& values, const std::vector<uint32_t>& dims = {}) const {
        if (dims.empty()) {
            return setArrayData(values.data(), { values.size() });
        }
        return setArrayData(values.data(), dims);
    }

    template <typename DataType, typename UnitsType>
    inline void setArrayDataWithUnits(const std::vector<DataType>& values, UnitsType units, const std::vector<uint32_t>& dims = {}) const {
        if (dims.empty()) {
            return setArrayDataWithUnits(values.data(), units, { values.size() });
        }
        return setArrayDataWithUnits(values.data(), units, dims);
    }

    template <typename DataType>
    inline void setArrayData(const DataType * values, const std::vector<uint32_t>& dims) const {
        putRecord(Data::FromArray(values, dims));
    }

    template <typename DataType, typename UnitsType>
    inline void setArrayDataWithUnits(const DataType * values, UnitsType units, const std::vector<uint32_t>& dims) const {
        putRecord(WithUnits(Data::FromArray(values, dims), Data::FromScalar(units)));
    }

    template <typename ValueType>
    void putRow(int segmentLength, ValueType value, int64_t timestamp);

    template <typename ValueArrayType>
    void putSegment(ValueArrayType values, int index = -1);

    template <typename ValueArrayType>
    inline void putTimestampedSegment(std::vector<int64_t> timestamps, ValueArrayType values) {
        putTimestampedSegment(timestamps.data(), values);
    }

    template <typename ValueArrayType>
    void putTimestampedSegment(int64_t * timestamps, ValueArrayType values);

    template <
        typename StartIndexType,
        typename EndIndexType,
        typename DimensionType,
        typename ValueArrayType
    >
    void makeSegment(
        StartIndexType startIndex,
        EndIndexType endIndex,
        DimensionType dimension,
        ValueArrayType values,
        int index = -1,
        int rowsFilled = -1
    ) const;

    template <
        typename StartIndexType,
        typename EndIndexType,
        typename DimensionType,
        typename ValueArrayType
    >
    void makeSegmentResampled(
        StartIndexType startIndex,
        EndIndexType endIndex,
        DimensionType dimension,
        ValueArrayType values,
        const TreeNode& resampleNode,
        int resampleFactor,
        int index = -1,
        int rowsFilled = -1
    ) const;

    // template <typename StartIndexType, typename EndIndexType, typename DimensionType, typename ValueArrayType>
    // void makeSegmentMinMax(
    //     StartIndexType startIndex,
    //     EndIndexType endIndex,
    //     DimensionType dimension,
    //     ValueArrayType values,
    //     const TreeNode& resampleNode,
    //     int resampleFactor,
    //     int index = -1,
    //     int rowsFilled = -1
    // ) const;

    // template <typename TimestampArrayType, typename ValueArrayType>
    // inline void makeTimestampedSegment(
    //     TimestampArrayType timestamps,
    //     ValueArrayType values,
    //     int index = -1,
    //     int rowsFilled = -1
    // ) const;

    // template <typename ValueArrayType>
    // inline void makeTimestampedSegment(
    //     Int64Array timestamps,
    //     ValueArrayType values,
    //     int index = -1,
    //     int rowsFilled = -1
    // ) const;

    template <typename ValueArrayType>
    inline void makeTimestampedSegment(
        std::vector<int64_t> timestamps,
        ValueArrayType values,
        int index = -1,
        int rowsFilled = -1
    ) const
    {
        makeTimestampedSegment(timestamps.data(), values, index, rowsFilled);
    }

    template <typename ValueArrayType>
    void makeTimestampedSegment(
        int64_t * timestamps,
        ValueArrayType values,
        int index = -1,
        int rowsFilled = -1
    ) const;

    // TODO: Move
    #ifdef __cpp_lib_optional

        [[nodiscard]]
        std::optional<TreeNode> tryGetNode(const std::string& path) const
        {
            int nid = 0;

            int status = _TreeFindNodeRelative(getDBID(), path.data(), _nid, &nid);
            if (status == TreeNNF) {
                return std::nullopt;
            }
            else if (IS_NOT_OK(status)) {
                throwException(status);
            }

            return TreeNode(_tree, nid);
        }

        [[nodiscard]]
        inline std::optional<TreeNode> tryGetParent() const
        {
            int nid = getParentNID();
            if (nid == 0) {
                return std::nullopt;
            }
            return TreeNode(_tree, nid);
        }

        [[nodiscard]]
        inline std::optional<TreeNode> tryGetBrother() const
        {
            int nid = getBrotherNID();
            if (nid == 0) {
                return std::nullopt;
            }
            return TreeNode(_tree, nid);
        }

        [[nodiscard]]
        inline std::optional<TreeNode> tryGetMember() const
        {
            int nid = getMemberID();
            if (nid == 0) {
                return std::nullopt;
            }
            return TreeNode(_tree, nid);
        }

        [[nodiscard]]
        inline std::optional<TreeNode> tryGetChild() const
        {
            int nid = getChildNID();
            if (nid == 0) {
                return std::nullopt;
            }
            return TreeNode(_tree, nid);
        }

        [[nodiscard]]
        std::optional<Data> tryGetData() const
        {
            mdsdsc_xd_t xd = MDSDSC_XD_INITIALIZER;
            int status = _TreeGetRecord(getDBID(), _nid, &xd);
            if (status == TreeNODATA) {
                return std::nullopt;
            }
            else if (IS_NOT_OK(status)) {
                throwException(status);
            }

            return Data(std::move(xd));
        }

    #endif

protected:

    Tree * _tree = nullptr;

    int _nid = -1;

    template <typename ResultType>
    ResultType _getNCI(nci_t code) const;

    std::string _getStringNCI(nci_t code, int16_t size) const;

    std::vector<int> _getNIDArrayNCI(nci_t code, nci_t codeForNumberOf) const;

    std::vector<TreeNode> _getTreeNodeArrayNCI(nci_t code, nci_t codeForNumberOf) const;

}; // class TreeNode

std::string to_string(const TreeNode * node);

inline std::string to_string(const TreeNode& node) {
    return to_string(&node);
}

enum class Mode
{
    Normal,
    ReadOnly,
    Edit,
    New,

    // TODO: Rename
    View,
};

std::string to_string(const Mode& mode);

class Tree : public TreeNode
{
public:

    static Tree GetActive() {
        char name[64];
        int shot;
        int retNameLength;

        DBI_ITM itmList[] = {
            { sizeof(name), DbiNAME, name, &retNameLength, },
            { sizeof(shot), DbiSHOTID, &shot, nullptr, },
            { 0, DbiEND_OF_LIST, nullptr, nullptr, },
        };
        int status = TreeGetDbi(itmList);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Tree(name, shot, TreeDbid());
    }

    inline void setActive() {
        TreeSwitchDbid(getDBID());
    }

    Tree();

    Tree(const std::string& treename, int shot, Mode mode = Mode::Normal)
        : _treename(treename)
        , _shot(shot)
        , _mode(mode)
    {
        open();
    }

    Tree(const std::string& treename, int shot, void * dbid)
        : _treename(treename)
        , _shot(shot)
        , _mode(Mode::View)
        , _dbid(dbid)
    { }

    inline ~Tree()
    {
        if (_mode != Mode::View) {
            TreeFreeDbid(_dbid);
            _dbid = nullptr;
        }
    }

    [[nodiscard]]
    inline void * getDBID() const {
        return _dbid;
    }

    [[nodiscard]]
    inline void ** getContext() const {
        return const_cast<void **>(&_dbid);
    }

    [[nodiscard]]
    inline std::string getTreeName() const {
        return _treename;
    }

    [[nodiscard]]
    inline int getShot() const {
        return _shot;
    }

    [[nodiscard]]
    inline Mode getMode() const {
        return _mode;
    }

    inline void open(const std::string& treename, int shot, Mode mode = Mode::Normal)
    {
        _treename = treename;
        _shot = shot;
        _mode = mode;

        return open();
    }

    void open();

    inline void reopen() { open(); }

    void close();

    void write();

    inline void setDefaultNode(const std::string& path) const {
        setDefaultNode(getNode(path));
    }

    void setDefaultNode(const TreeNode& node) const;

    TreeNode getDefaultNode() const;

    template <typename ResultType = Data, typename ...ArgTypes>
    ResultType compileData(const std::string& expression, ArgTypes... args) const;

    template <typename ResultType = Data, typename ...ArgTypes>
    ResultType executeData(const std::string& expression, ArgTypes... args) const;

private:

    void * _dbid = nullptr;

    bool _open = false;

    std::string _treename;

    int _shot;

    Mode _mode = Mode::Normal;

};

std::string to_string(const Tree * tree);

inline std::string to_string(const Tree& tree) {
    return to_string(&tree);
}

class GetMany;
class PutMany;

class Connection
{
public:

    static constexpr int InvalidConnectionID = -1;

    inline Connection(const std::string& hostspec) {
        connect(hostspec);
    }

    inline virtual ~Connection() {
        disconnect();
    }

    void connect(const std::string& hostspec);

    void disconnect();

    inline void reconnect() {
        disconnect();
        connect(_hostspec);
    }

    template <typename ResultType = Data, typename ...ArgTypes>
    ResultType get(const std::string& expression, const ArgTypes& ...args) const
    {
        if (_local) {
            return Data::Execute<ResultType>(expression, args...);
        }
        else {
            std::vector<DataView> argList({ DataView(args)... });
            return _get(expression, std::move(argList)).releaseAndConvert<ResultType>();
        }
    }

    template <typename ResultType = Data, typename ...ArgTypes>
    ResultType getObject(const std::string& expression, ArgTypes ...args) const
    {
        Int8Array serializedData = get<Int8Array>("SerializeOut(`(" + expression + ";))", args...);
        return serializedData.deserialize<ResultType>();
    }

    inline void openTree(const std::string& tree, int shot) const {
        int status = get<Int32>("TreeOpen($,$)", tree, shot).getValue();
        if (IS_NOT_OK(status)) {
            throwException(status);
        }
    }

    inline void closeTree(const std::string& tree, int shot) const {
        int status = get<Int32>("TreeClose($,$)", tree, shot).getValue();
        if (IS_NOT_OK(status)) {
            throwException(status);
        }
    }

    inline int closeAllTrees() const {
        return get<Int32>("_i=0;WHILE(IAND(TreeClose(),1)) _i++;_i").getValue();
    }

    inline void setDefault(const std::string& path) {
        get("TreeSetDefault($)", path);
    }

    template <typename ...Args>
    inline void put(const std::string& node, const std::string& expression, const Args& ...args) const
    {
        std::string putExpression = "TreePut($,$";
        for (size_t i = 0; i < sizeof...(args) - 2; ++i) {
            putExpression += ",$";
        }
        putExpression += ")";

        get(putExpression, node, expression, args...);
    }

    GetMany getMany() const;

    // putMany

private:

    Data _get(const std::string& expression, std::vector<DataView>&& xdArgs) const;

    int _id = InvalidConnectionID;

    std::string _hostspec;

    bool _local = false;

}; // class Connection

class GetMany
{
public:

    GetMany(const Connection * conn)
        : _conn(conn)
    { }

    template <typename ...ArgTypes>
    void append(const std::string& name, const std::string& expression, const ArgTypes& ...args)
    {
        // _queries.emplace_back(std::unordered_map<std::string, Data>{
        //     { "name", String(name) },
        //     { "exp", String(expression), },
        //     { "args", List(args...) },
        // });
        _queries.emplace_back(Dictionary(
            "name", name,
            "exp", expression,
            "args", List(args...)
        ));
    }

    const Dictionary& execute()
    {
        printf("%s\n", to_string(List(_queries).serialize()).c_str());
        Data result = _conn->get("GetManyExecute($)", List(_queries).serialize());

        if (result.getClass() == Class::S) {
            if (result.getDType() == DType::T) {
                // TODO: Read error
                throw MDSplusException();
            }
            else if (result.getDType() == DType::L) {
                int status = result.releaseAndConvert<Int32>().getValue();
                throwException(status);
            }
        }

        _result = result.releaseAndConvert<UInt8Array>().deserialize<Dictionary>();
        return _result;
    }

private:

    const Connection * _conn;

    // std::vector<std::unordered_map<std::string, Data>> _queries;

    std::vector<Dictionary> _queries;

    Dictionary _result;

}; // class GetMany

inline GetMany Connection::getMany() const {
    return GetMany(this);
}

struct DevicePart
{
    std::string Path;
    mdsplus::Usage Usage = mdsplus::Usage::Any;
    Data * Value = {};
    std::string ValueExpression = {};
    TreeNodeFlags Flags = {};
    std::map<std::string, Data *> XNCI = {};
    std::function<void(TreeNode& node)> Callback;
};

class Device : public TreeNode
{
public:

    Device(Tree * tree, int nid)
        : TreeNode(tree, nid)
    { }

    static std::vector<DevicePart> GetParts() {
        return {};
    }

    void addParts(std::vector<DevicePart>&& parts) const;

};

#define MDSPLUS_DEVICE_BOOTSTRAP(DeviceClass) \
    DeviceClass(Tree * tree, int nid) \
        : Device(tree, nid) \
    { } \
    virtual ~DeviceClass() = default; \

#define MDSPLUS_DEVICE(LibraryName, DeviceClass, DeviceClassLower)             \
    extern "C" int DeviceClassLower##__add(                                    \
        mdsdsc_t * name_dsc,                                                   \
        mdsdsc_t * dummy_dsc,                                                  \
        int * nid_ptr)                                                         \
    {                                                                          \
        (void)dummy_dsc;                                                       \
        int head_nid = 0;                                                      \
        static DESCRIPTOR(library_dsc, LibraryName);                           \
        static DESCRIPTOR(model_dsc, #DeviceClass);                            \
        static DESCRIPTOR_CONGLOM(device_dsc, &library_dsc, &model_dsc, 0, 0); \
        std::vector<mdsplus::DevicePart> parts = DeviceClass::GetParts();      \
        TreeStartConglomerate(parts.size() + 1);                               \
        std::string name(name_dsc->pointer, name_dsc->length);                 \
        TreeAddNode(name.c_str(), &head_nid, TreeUSAGE_DEVICE);                \
        TreePutRecord(head_nid, (mdsdsc_t *)&device_dsc, 0);                   \
        Tree tree = mdsplus::Tree::GetActive();                                \
        DeviceClass device(&tree, head_nid);                                   \
        device.addParts(std::move(parts));                                     \
        TreeEndConglomerate();                                                 \
        if (nid_ptr) {                                                         \
            *nid_ptr = head_nid;                                               \
        }                                                                      \
        return TreeSUCCESS;                                                    \
    }

#define MDSPLUS_DEVICE_METHOD(DeviceClassLower, DeviceClass, MethodName) \
    extern "C" int DeviceClassLower##__##MethodName(mdsdsc_t * nid)      \
    {                                                                    \
        Tree tree = mdsplus::Tree::GetActive();                          \
        DeviceClass device(&tree, *(int *)nid->pointer);                 \
        try {                                                            \
            device.MethodName();                                         \
        } catch (mdsplus::MDSplusException& e) {                         \
            return e.getStatus();                                        \
        }                                                                \
        return TreeSUCCESS;                                              \
    }

class SineWave : public Device
{
public:

    MDSPLUS_DEVICE_BOOTSTRAP(SineWave)

    static std::vector<DevicePart> GetParts() {
        // std::vector<DevicePart> parts = {
        //     DevicePart{
        //         .Path = ":COMMENT",
        //         .Usage = Usage::Any,
        //         .Value = new String("Test 123"),
        //         .Flags = {
        //             .WriteOnce = true,
        //             .NoWriteShot = true,
        //         },
        //         .XNCI = {
        //             { "tooltip", new String("A comment") },
        //         }
        //     },
        //     DevicePart{
        //         .Path = ":FREQUENCY",
        //         .Usage = Usage::Numeric,
        //         .Value = new WithUnits(1000.0f, "Hz"),
        //         .Flags = {
        //             .NoWriteShot = true,
        //         },
        //         .Callback = [](TreeNode& node) {
        //             node.setFlagsOn(TreeNodeFlags{
        //                 .Cached = true,
        //             });
        //             // node.setExtendedAttribute("tooltip", "The Frequency in Hz");
        //         },
        //         // .XNCI = {
        //         //     { "tooltip", String("The Frequency in Hz") },
        //         // }
        //     }
        // };

        // for (int i = 0; i < 32; ++i) {
        //     std::string path = ":INPUTS:INPUT_" + std::to_string(i);
        //     // parts.push_back(DevicePart{
        //     //     .Path = path,
        //     //     // ...
        //     // });
        // }

        // return parts;
        return {};
    }

};

inline std::string to_string(const Class& class_)
{
    switch (class_) {
        case Class::Missing: return"Class::Missing";
        case Class::S: return"Class::S";
        case Class::D: return"Class::D";
        case Class::A: return"Class::A";
        case Class::XD: return"Class::XD";
        case Class::XS: return"Class::XS";
        case Class::R: return"Class::R";
        case Class::CA: return"Class::CA";
        case Class::APD: return"Class::APD";
    }

    return "?";
}

inline std::string to_string(const DType& dtype)
{
    switch (dtype) {
        case DType::Missing: return "DType::Missing";
        case DType::BU: return "DType::BU";
        case DType::WU: return "DType::WU";
        case DType::LU: return "DType::LU";
        case DType::QU: return "DType::QU";
        case DType::B: return "DType::B";
        case DType::W: return "DType::W";
        case DType::L: return "DType::L";
        case DType::Q: return "DType::Q";
        case DType::F: return "DType::F";
        case DType::D: return "DType::D";
        case DType::FC: return "DType::FC";
        case DType::DC: return "DType::DC";
        case DType::T: return "DType::T";
        case DType::DSC: return "DType::DSC";
        case DType::OU: return "DType::OU";
        case DType::O: return "DType::O";
        case DType::G: return "DType::G";
        case DType::H: return "DType::H";
        case DType::GC: return "DType::GC";
        case DType::HC: return "DType::HC";

        case DType::Pointer: return "DType::Pointer";
        case DType::FS: return "DType::FS";
        case DType::FT: return "DType::FT";
        case DType::FSC: return "DType::FSC";
        case DType::FTC: return "DType::FTC";
        case DType::C: return "DType::C";

        case DType::List: return "DType::List";
        case DType::Tuple: return "DType::Tuple";
        case DType::Dictionary: return "DType::Dictionary";

        case DType::Ident: return "DType::Ident";
        case DType::NID: return "DType::NID";
        case DType::Path: return "DType::Path";
        case DType::Param: return "DType::Param";
        case DType::Signal: return "DType::Signal";
        case DType::Dimension: return "DType::Dimension";
        case DType::Window: return "DType::Window";
        case DType::Slope: return "DType::Slope";
        case DType::Function: return "DType::Function";
        case DType::Conglom: return "DType::Conglom";
        case DType::Range: return "DType::Range";
        case DType::Action: return "DType::Action";
        case DType::Dispatch: return "DType::Dispatch";
        case DType::Program: return "DType::Program";
        case DType::Routine: return "DType::Routine";
        case DType::Procedure: return "DType::Procedure";
        case DType::Method: return "DType::Method";
        case DType::Dependency: return "DType::Dependency";
        case DType::Condition: return "DType::Condition";
        case DType::Event: return "DType::Event";
        case DType::WithUnits: return "DType::WithUnits";
        case DType::Call: return "DType::Call";
        case DType::WithError: return "DType::WithError";
        case DType::Opaque: return "DType::Opaque";
    }

    return "?";
}

inline std::string to_string(const Data * data)
{
    assert(data);

    std::string str(data->getClassName());
    str += "(";
    str += to_string(data->getClass());
    str += ", ";
    str += to_string(data->getDType());
    str += ", `";
    str += data->decompile();
    str += "`)";

    return str;
}

inline std::string Data::decompile() const
{
    int status;

    mdsdsc_t * dsc = getDescriptor();
    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    status = _intrinsic(OPC_DECOMPILE, 1, &dsc, &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    mdsdsc_t * deco = out.pointer;
    assert(deco->dtype == DTYPE_T);

    std::string decompile(deco->pointer, deco->length);

    status = MdsFree1Dx(&out, nullptr);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return decompile;
}

inline Data Data::serialize() const
{
    mdsdsc_t * dsc = getDescriptor();
    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    int status = MdsSerializeDscOut(dsc, &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out), getTree());
}

template <typename UnitsType /*= Data*/>
inline UnitsType Data::getUnits() const
{
    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    mdsdsc_t * dsc = getDescriptor();
    int status = _intrinsic(OPC_UNITS_OF, 1, &dsc, &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).releaseAndConvert<UnitsType>();
}

template <typename DataType /*= Data*/, typename UnitsType /*= Data*/>
std::tuple<DataType, UnitsType> Data::getDataWithUnits() const
{
    // We could use OPC_DATA_WITH_UNITS to upcast it to a WithUnits() type,
    // but returning a tuple is a cleaner API
    return { getData<DataType>(), getUnits<UnitsType>() };
}

template <typename ErrorType /*= Data*/>
inline ErrorType Data::getError() const
{
    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    mdsdsc_t * dsc = getDescriptor();
    int status = _intrinsic(OPC_ERROR_OF, 1, &dsc, &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).releaseAndConvert<ErrorType>();
}

template <typename HelpType /*= Data*/>
inline HelpType Data::getHelp() const
{
    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    mdsdsc_t * dsc = getDescriptor();
    int status = _intrinsic(OPC_HELP_OF, 1, &dsc, &out);
    if (status == TdiINVDTYDSC) {
        // Unlike UNITS_OF and ERROR_OF, this throws an error when there is no value
        return HelpType();
    }
    else if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).releaseAndConvert<HelpType>();
}

template <typename ValidationType /*= Data*/>
inline ValidationType Data::getValidation() const
{
    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    mdsdsc_t * dsc = getDescriptor();
    int status = _intrinsic(OPC_ERROR_OF, 1, &dsc, &out);
    if (status == TdiINVDTYDSC) {
        // Unlike UNITS_OF and ERROR_OF, this throws an error when there is no value
        return ValidationType();
    }
    else if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).releaseAndConvert<ValidationType>();
}

template <typename ResultType /*= Data*/, typename ...ArgTypes>
ResultType Data::Compile(const std::string& expression, const ArgTypes& ...args)
{
    DataView argExp(expression);
    std::vector<DataView> argList = { DataView(args)... };

    std::vector<mdsdsc_t *> dscList = { argExp.getDescriptor() };
    for (const auto& arg : argList) {
        dscList.push_back(arg.getDescriptor());
    }

    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    int status = TdiIntrinsic(OPC_COMPILE, dscList.size(), dscList.data(), &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).releaseAndConvert<ResultType>();
}

template <typename ResultType /*= Data*/, typename ...ArgTypes>
ResultType Data::Execute(const std::string& expression, const ArgTypes& ...args)
{
    DataView argExp(expression);
    std::vector<DataView> argList = { DataView(args)... };

    std::vector<mdsdsc_t *> dscList = { argExp.getDescriptor() };
    for (const auto& arg : argList) {
        dscList.push_back(arg.getDescriptor());
    }

    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    int status = TdiIntrinsic(OPC_EXECUTE, dscList.size(), dscList.data(), &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).releaseAndConvert<ResultType>();
}

inline int Data::_intrinsic(opcode_t opcode, int narg, mdsdsc_t *list[], mdsdsc_xd_t * out) const
{
    int status;

    if (hasTree()) {
        status = _TdiIntrinsic(getTree()->getContext(), opcode, narg, list, out);
    }
    else {
        status = TdiIntrinsic(opcode, narg, list, out);
    }

    return status;
}

template <typename ResultType>
inline ResultType Data::_convertToScalar()
{
    int status;

    mdsdsc_xd_t xd = release();
    mdsdsc_t * dsc = xd.pointer;

    if (dsc->class_ == class_t(ResultType::__class) &&
        dsc->dtype == dtype_t(ResultType::__dtype)) {
        return ResultType(std::move(xd), getTree());
    }

    // TODO: Check edge cases for when we need to call DATA()
    if (dsc->class_ == CLASS_R) {
        mdsdsc_xd_t dataXD = MDSDSC_XD_INITIALIZER;
        status = _intrinsic(OPC_DATA, 1, &dsc, &dataXD);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        MdsFree1Dx(&xd, nullptr);

        xd = dataXD;
        dsc = xd.pointer;

        if (dsc->class_ == class_t(ResultType::__class) &&
            dsc->dtype == dtype_t(ResultType::__dtype)) {
            return ResultType(std::move(xd), getTree());
        }
    }

    typename ResultType::__ctype value = {};

    mdsdsc_t convert = {
        .length = sizeof(typename ResultType::__ctype),
        .dtype = dtype_t(ResultType::__dtype),
        .class_ = CLASS_S,
        .pointer = (char *)&value,
    };

    status = TdiConvert((mdsdsc_a_t *)dsc, (mdsdsc_a_t *)&convert);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    MdsFree1Dx(&xd, nullptr);
    return ResultType(value);
}

template <typename ResultType>
inline ResultType Data::_convertToArray()
{
    int status;

    mdsdsc_xd_t xd = release();
    mdsdsc_t * dsc = xd.pointer;

    if (dsc->class_ == class_t(ResultType::__class) &&
        dsc->dtype == dtype_t(ResultType::__dtype)) {
        return ResultType(std::move(xd), getTree());
    }

    // TODO: Check edge cases for when we need to call DATA()
    if (dsc->class_ == CLASS_R) {
        mdsdsc_xd_t dataXD = MDSDSC_XD_INITIALIZER;
        status = _intrinsic(OPC_DATA, 1, &dsc, &dataXD);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        MdsFree1Dx(&xd, nullptr);

        xd = dataXD;
        dsc = xd.pointer;

        if (dsc->class_ == class_t(ResultType::__class) &&
            dsc->dtype == dtype_t(ResultType::__dtype)) {
            return ResultType(std::move(xd), getTree());
        }
    }

    array_coeff arrayOfOne = {
        .length = dsc->length,
        .dtype = dsc->dtype,
        .class_ = CLASS_A,
        .pointer = dsc->pointer,
        .scale = 0,
        .digits = 0,
        .aflags = {
            .binscale = false,
            .redim = true,
            .column = true,
            .coeff = false,
            .bounds = false,
        },
        .dimct = 0,
        .arsize = dsc->length,
    };

    array_coeff * dscArray = &arrayOfOne;
    if (dsc->class_ == CLASS_A) {
        dscArray = (array_coeff *)dsc;
    }

    size_t size = (dscArray->arsize / dscArray->length);
    std::vector<typename ResultType::__ctype> values(size);
    array_coeff convert = {
        .length = sizeof(typename ResultType::__ctype),
        .dtype = dtype_t(ResultType::__dtype),
        .class_ = CLASS_A,
        .pointer = (char *)values.data(),
        .scale = dscArray->scale,
        .digits = dscArray->digits,
        .aflags = dscArray->aflags,
        .dimct = dscArray->dimct,
        .arsize = arsize_t(size * sizeof(typename ResultType::__ctype)),
        .a0 = (char *)values.data(),
        .m = { 0 },
    };

    std::vector<uint32_t> dims(dscArray->dimct);
    if (dscArray->aflags.coeff) {
        for (dimct_t i = 0; i < dscArray->dimct; ++i) {
            convert.m[i] = dscArray->m[i];
            dims[i] = dscArray->m[i];
        }
    }
    else {
        // TODO: Improve
        dims.clear();
    }

    status = TdiConvert((mdsdsc_a_t *)dscArray, (mdsdsc_a_t *)&convert);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    MdsFree1Dx(&xd, nullptr);
    return ResultType(values, dims);
}

inline void String::setValue(const char * value, length_t length)
{
    mdsdsc_s_t dsc = {
        length,
        DTYPE_T,
        CLASS_S,
        (char *)value
    };

    int status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

inline std::vector<std::string> StringArray::getValues() const
{
    mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
    if (dsc) {
        std::vector<std::string> values;
        for (size_t i = 0; i < size(); ++i) {
            values.emplace_back(dsc->pointer + (i * dsc->length), dsc->length);
        }
        return values;
    }
    return {};
}

inline std::string StringArray::getValueAt(size_t index) const
{
    mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
    if (dsc && index < size()) {
        return std::string(dsc->pointer + (index * dsc->length), dsc->length);
    }

    throw TdiBadIndex();
}

inline size_t StringArray::getSize() const
{
    mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
    if (dsc) {
        return dsc->arsize / dsc->length;
    }
    return 0;
}

inline void StringArray::setValues(
    const std::vector<std::string>& values,
    const std::vector<uint32_t>& dims /*= {}*/
) {
    length_t maxSize = 0;
    for (const auto& value : values) {
        if (value.size() > maxSize) {
            maxSize = value.size();
        }
    }

    std::string buffer;
    for (const auto& value : values) {
        buffer += value;
        for (length_t i = value.size(); i < maxSize; ++i) {
            buffer.push_back(' ');
        }
    }

    if (dims.empty()) {
        _setValues(buffer, maxSize, { length_t(values.size()) });
    }
    else {
        _setValues(buffer, maxSize, dims);
    }
}

#ifdef __cpp_lib_string_view

    inline void StringArray::setValues(
        const std::vector<std::string_view>& values,
        const std::vector<uint32_t>& dims /*= {}*/
    ) {
        length_t maxSize = 0;
        for (const auto& value : values) {
            if (value.size() > maxSize) {
                maxSize = value.size();
            }
        }

        std::string buffer;
        for (const auto& value : values) {
            buffer += value;
            for (length_t i = value.size(); i < maxSize; ++i) {
                buffer.push_back(' ');
            }
        }

        if (dims.empty()) {
            _setValues(buffer, maxSize, { length_t(values.size()) });
        }
        else {
            _setValues(buffer, maxSize, dims);
        }
    }

#endif // __cpp_lib_string_view

inline void StringArray::_setValues(
    const std::string& buffer,
    length_t length,
    const std::vector<uint32_t>& dims
) {
    array_coeff dsc = {
        .length = length,
        .dtype = DTYPE_T,
        .class_ = CLASS_A,
        .pointer = const_cast<char *>(buffer.data()),
        .scale = 0,
        .digits = 0,
        .aflags = aflags_t{
            .binscale = false,
            .redim = true,
            .column = true,
            .coeff = true,
            .bounds = false,
        },
        .dimct = dimct_t(dims.size()),
        .arsize = arsize_t(buffer.size()),
        .a0 = const_cast<char *>(buffer.data()),
        .m = { 0 },
    };

    for (size_t i = 0; i < dims.size(); ++i) {
        dsc.m[i] = dims[i];
    }

    int status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

inline std::vector<uint32_t> Array::getDimensions() const
{
    mdsdsc_a_t * dsc = getArrayDescriptor();
    if (dsc) {
        if (dsc->aflags.coeff) {
            array_coeff * dscCoeff = reinterpret_cast<array_coeff *>(dsc);
            return std::vector<uint32_t>(
                (uint32_t *)dscCoeff->m,
                (uint32_t *)(dscCoeff->m + dscCoeff->dimct)
            );
        }
        else {
            return { (dsc->arsize / dsc->length) };
        }
    }

    return {};
}

template <typename CType>
inline void Array::_setValues(DType dtype, const CType * values, const uint32_t * dims, dimct_t dimCount)
{
    // TODO: Overwrite existing values if the shape/type is the same

    uint32_t count = dims[0];
    for (dimct_t i = 1; i < dimCount; ++i) {
        count *= dims[dimCount];
    }

    array_coeff dsc = {
        .length = sizeof(CType),
        .dtype = dtype_t(dtype),
        .class_ = CLASS_A,
        .pointer = (char *)values,
        .scale = 0,
        .digits = 0,
        .aflags = aflags_t{
            .binscale = false,
            .redim = true,
            .column = true,
            .coeff = false,
            .bounds = false,
        },
        .dimct = dimCount,
        .arsize = arsize_t(count * sizeof(CType)),
        .a0 = (char *)values,
        .m = { count, 0, 0, 0, 0, 0, 0, 0, },
    };

    if (dims && dimCount > 0) {
        dsc.aflags.coeff = true;
        for (size_t i = 0; i < dimCount; ++i) {
            dsc.m[i] = dims[i];
        }
    }

    int status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

#ifdef __cpp_lib_span

    template <typename CType>
    inline std::span<CType> Array::_getSpan() const {
        mdsdsc_a_t * dsc = getArrayDescriptor();
        if (dsc) {
            return std::span<CType>(
                reinterpret_cast<CType *>(dsc->pointer),
                reinterpret_cast<CType *>(dsc->pointer + dsc->arsize)
            );
        }
        return {};
    }

#endif // __cpp_lib_span

template <typename CType>
inline std::vector<CType> Array::_getVector() const {
    mdsdsc_a_t * dsc = getArrayDescriptor();
    if (dsc) {
        return std::vector<CType>(
            reinterpret_cast<CType *>(dsc->pointer),
            reinterpret_cast<CType *>(dsc->pointer + dsc->arsize)
        );
    }
    return {};
}

template <typename CType>
inline const CType& Array::_getValueAt(size_t index) const {
    mdsdsc_a_t * dsc = getArrayDescriptor();
    if (dsc && index < size()) {
        return *((CType *)dsc->pointer + index);
    }

    throw TdiBadIndex();
}

template <typename CType>
inline CType * Array::_begin() const
{
    mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
    if (dsc) {
        return reinterpret_cast<CType *>(dsc->pointer);
    }

    return nullptr;
}

template <typename CType>
inline CType * Array::_end() const
{
    mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
    if (dsc) {
        return reinterpret_cast<CType *>(dsc->pointer + dsc->arsize);
    }

    return nullptr;
}

template <typename ResultType /*= Data*/>
inline ResultType Int8Array::deserialize()
{
    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    int status = MdsSerializeDscIn(getDescriptor()->pointer, &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).releaseAndConvert<ResultType>();
}

template <typename ResultType /*= Data*/>
inline ResultType UInt8Array::deserialize()
{
    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    int status = MdsSerializeDscIn(getDescriptor()->pointer, &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).releaseAndConvert<ResultType>();
}

inline Data Function::call() const
{
    int status;

    descriptor_function * dsc = (descriptor_function *)getDescriptor();

    // TODO: Tree*
    opcode_t * opcode = (opcode_t *)dsc->pointer;
    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    _intrinsic(*opcode, dsc->ndesc, dsc->arguments, &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out));
}

inline std::string to_string(const Usage& usage)
{
    switch (usage) {
    case Usage::Any: return "Usage::Any";
    case Usage::Structure: return "Usage::Structure";
    case Usage::Action: return "Usage::Action";
    case Usage::Device: return "Usage::Device";
    case Usage::Dispatch: return "Usage::Dispatch";
    case Usage::Numeric: return "Usage::Numeric";
    case Usage::Signal: return "Usage::Signal";
    case Usage::Task: return "Usage::Task";
    case Usage::Text: return "Usage::Text";
    case Usage::Window: return "Usage::Window";
    case Usage::Axis: return "Usage::Axis";
    case Usage::Subtree: return "Usage::Subtree";
    case Usage::CompoundData: return "Usage::CompoundData";
    }

    return "?";
}

inline std::string to_string(const TreeNodeFlags& flags)
{
    std::string str;

    if (flags.CompressSegments) {
        str += "CompressSegments | ";
    }

    if (flags.IncludeInPulse) {
        str += "IncludeInPulse | ";
    }

    if (flags.NidReference) {
        str += "NidReference | ";
    }

    if (flags.PathReference) {
        str += "PathReference | ";
    }

    if (flags.NoWriteShot) {
        str += "NoWriteShot | ";
    }

    if (flags.NoWriteModel) {
        str += "NoWriteModel | ";
    }

    if (flags.CompressOnPut) {
        str += "CompressOnPut | ";
    }

    if (flags.DoNotCompress) {
        str += "DoNotCompress | ";
    }

    if (flags.Compressible) {
        str += "Compressible | ";
    }

    if (flags.WriteOnce) {
        str += "WriteOnce | ";
    }

    if (flags.SetupInformation) {
        str += "SetupInformation | ";
    }

    if (flags.Segmented) {
        str += "Segmented | ";
    }

    if (flags.Versions) {
        str += "Versions | ";
    }

    if (flags.Cached) {
        str += "Cached | ";
    }

    if (flags.Essential) {
        str += "Essential | ";
    }

    if (flags.ParentState) {
        str += "ParentState | ";
    }

    if (flags.State) {
        str += "State | ";
    }

    // Strip trailing " | "
    if (!str.empty()) {
        str.resize(str.size() - 3);
    }

    return str;
}

inline std::string to_string(const TreeNode * node)
{
    assert(node);

    std::string str = "TreeNode(";
    str += to_string(node->getTree());
    str += ", ";
    str += std::to_string(node->getNID());
    str += ")";

    return str;

    // return std::format(
    //     "TreeNode({}, {})",
    //     to_string(node->getTree()),
    //     node->getNID()
    // );
}

inline std::string to_string(const Mode& mode)
{
    switch (mode) {
    case Mode::Normal: return "Mode::Normal";
    case Mode::ReadOnly: return "Mode::ReadOnly";
    case Mode::Edit: return "Mode::Edit";
    case Mode::New: return "Mode::New";

    case Mode::View: return "Mode::View";
    }

    return "?";
}

inline std::string to_string(const Tree * tree)
{
    assert(tree);

    std::string str = "Tree(\"";
    str += tree->getTreeName();
    str += "\", ";
    str += std::to_string(tree->getShot());
    str += ", ";
    str += to_string(tree->getMode());
    str += ")";

    return str;

    // return std::format(
    //     "Tree(\"{}\", {}, {})",
    //     tree->getTreeName(),
    //     tree->getShot(),
    //     to_string(tree->getMode())
    // );
}

[[nodiscard]]
inline void * TreeNode::getDBID() const {
    return getTree()->getDBID();
}

[[nodiscard]]
inline void * TreeNode::getContext() const {
    return getTree()->getContext();
}

inline TreeNode TreeNode::getNode(const std::string& path) const
{
    int nid = 0;

    int status = _TreeFindNodeRelative(getTree()->getDBID(), path.data(), _nid, &nid);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return TreeNode(_tree, nid);
}

inline TreeNode TreeNode::addNode(const std::string& path, Usage usage) const
{
    int nid = 0;

    int status = _TreeAddNode(getTree()->getDBID(), path.data(), &nid, static_cast<unsigned char>(usage));
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return TreeNode(_tree, nid);
}

inline TreeNode TreeNode::addDevice(const std::string& path, const std::string& model) const
{
    int nid = 0;

    int status = _TreeAddConglom(getTree()->getDBID(), path.data(), model.data(), &nid);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return TreeNode(_tree, nid);
}

inline std::string TreeNode::getNodeName() const
{
    std::string name = _getStringNCI(NciNODE_NAME, 64);

    // Trim trailing spaces
    size_t it = name.find(' ');
    if (it != std::string::npos) {
        name[it] = '\0';
    }

    return name;
}

inline Data TreeNode::getRecord() const
{
    mdsdsc_xd_t xd = MDSDSC_XD_INITIALIZER;
    int status = _TreeGetRecord(getTree()->getDBID(), _nid, &xd);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
    void putSegment(const Data& data, int index = -1);

    return Data(std::move(xd), getTree());
}

inline void TreeNode::putRecord(const Data& data) const
{
    mdsdsc_t * dsc = data.getDescriptor();
    int status = _TreePutRecord(getTree()->getDBID(), _nid, dsc, 0);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

inline std::vector<TreeNode> TreeNode::getMembersAndChildren(bool sortedNIDs /*= true*/) const
{
    int status;

    uint32_t numberOfMembers = 0;
    uint32_t numberOfChildren = 0;
    nci_itm countItemList[] = {
        { sizeof(numberOfMembers), NciNUMBER_OF_MEMBERS, &numberOfMembers, nullptr },
        { sizeof(numberOfChildren), NciNUMBER_OF_CHILDREN, &numberOfChildren, nullptr },
        { 0, NciEND_OF_LIST, nullptr, nullptr },
    };

    status = _TreeGetNci(getTree()->getDBID(), _nid, countItemList);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    std::vector<int> nids(numberOfMembers + numberOfChildren);

    int16_t sizeInBytesMembers = sizeof(int) * numberOfMembers;
    int16_t sizeInBytesChildren = sizeof(int) * numberOfChildren;
    nci_itm itemList[] = {
        { sizeInBytesMembers, NciMEMBER_NIDS, (void *)&nids[0], nullptr },
        { sizeInBytesChildren, NciCHILDREN_NIDS, (void *)&nids[numberOfMembers], nullptr },
        { 0, NciEND_OF_LIST, nullptr, nullptr },
    };

    status = _TreeGetNci(getTree()->getDBID(), _nid, itemList);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    if (sortedNIDs) {
        std::sort(nids.begin(), nids.end());
    }

    std::vector<TreeNode> nodes;
    nodes.reserve(nids.size());

    for (const int& nid : nids) {
        nodes.emplace_back(_tree, nid);
    }

    return nodes;
}

template <typename ResultType>
inline ResultType TreeNode::_getNCI(nci_t code) const
{
    ResultType value = {};
    nci_itm itm_lst[] = {
        { sizeof(value), code, &value, nullptr },
        { 0, NciEND_OF_LIST, nullptr, nullptr },
    };

    int status = _TreeGetNci(_tree->getDBID(), _nid, itm_lst);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return value;
}

inline std::string TreeNode::_getStringNCI(nci_t code, int16_t size) const
{
    std::string buffer;
    buffer.resize(size);

    int retlen = 0;
    nci_itm itemList[] = {
        { size, code, (void *)buffer.data(), &retlen },
        { 0, NciEND_OF_LIST, nullptr, nullptr },
    };

    int status = _TreeGetNci(getTree()->getDBID(), _nid, itemList);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    buffer.shrink_to_fit();

    return buffer;
}

inline std::vector<int> TreeNode::_getNIDArrayNCI(nci_t code, nci_t codeForNumberOf) const
{
    uint32_t numberOfNodes = _getNCI<uint32_t>(codeForNumberOf);
    std::vector<int> nids(numberOfNodes);

    int16_t sizeInBytes = sizeof(int) * numberOfNodes;
    nci_itm itemList[] = {
        { sizeInBytes, code, (void *)nids.data(), nullptr },
        { 0, NciEND_OF_LIST, nullptr, nullptr },
    };

    int status = _TreeGetNci(getTree()->getDBID(), _nid, itemList);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return nids;
}

inline std::vector<TreeNode> TreeNode::_getTreeNodeArrayNCI(nci_t code, nci_t codeForNumberOf) const
{
    const auto& nids = _getNIDArrayNCI(code, codeForNumberOf);

    std::vector<TreeNode> nodes;
    nodes.reserve(nids.size());

    for (const int& nid : nids) {
        nodes.emplace_back(_tree, nid);
    }

    return nodes;
}

template <typename ValueType>
void TreeNode::putRow(int segmentLength, ValueType value, int64_t timestamp)
{
    DataView argValue(value);

    int status = _TreePutRow(
        getTree()->getDBID(),
        getNID(),
        segmentLength,
        &timestamp,
        (mdsdsc_a_t *)argValue.getDescriptor()
    );
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

template <typename ValueArrayType>
void TreeNode::putSegment(ValueArrayType values, int index /*= -1*/)
{
    DataView argValues(values);

    int status = _TreePutSegment(
        getTree()->getDBID(),
        getNID(),
        index,
        (mdsdsc_a_t *)argValues.getDescriptor()
    );
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

template <typename ValueArrayType>
void TreeNode::putTimestampedSegment(int64_t * timestamps, ValueArrayType values)
{
    DataView argValues(values);

    int status = _TreePutTimestampedSegment(
        getTree()->getDBID(),
        getNID(),
        timestamps,
        (mdsdsc_a_t *)argValues.getDescriptor()
    );
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

template <typename DataType /*= Data*/>
[[nodiscard]]
inline DataType TreeNode::getData() const {
    return getRecord().releaseAndConvert<DataType>();
}

template <typename DataType /*= Data*/, typename UnitsType /*= Data*/>
[[nodiscard]]
inline std::tuple<DataType, UnitsType> TreeNode::getDataWithUnits() const {
    auto data = getRecord();
    auto units = data.getUnits<UnitsType>();
    return { data.releaseAndConvert<DataType>(), std::move(units) };
}

inline int _getMaxRowsFilled(mdsdsc_a_t * dsc)
{
    // TODO: Investigate, taken from TreeNode:makeSegment() in tree.py
    if (dsc->class_ == CLASS_R) {
        return 1;
    }
    else if (dsc->class_ == CLASS_A) {
        // TODO: Verify
        if (dsc->aflags.coeff) {
            array_coeff * dscCoeff = (array_coeff *)dsc;
            return dscCoeff->m[dscCoeff->dimct - 1];
        }
        else {
            return (dsc->arsize / dsc->length);
        }
    }

    return -1;
}

template <
    typename StartIndexType,
    typename EndIndexType,
    typename DimensionType,
    typename ValueArrayType
>
void TreeNode::makeSegment(
    StartIndexType startIndex,
    EndIndexType endIndex,
    DimensionType dimension,
    ValueArrayType values,
    int index /*= -1*/,
    int rowsFilled /*= -1*/
) const
{
    DataView argStartIndex(startIndex);
    DataView argEndIndex(endIndex);
    DataView argDimension(dimension);
    DataView argValues(values);

    mdsdsc_a_t * dscValues = (mdsdsc_a_t *)argValues.getDescriptor();
    if (rowsFilled < 0) {
        rowsFilled = _getMaxRowsFilled(dscValues);
    }

    int status = _TreeMakeSegment(
        getTree()->getDBID(),
        getNID(),
        argStartIndex.getDescriptor(),
        argEndIndex.getDescriptor(),
        argDimension.getDescriptor(),
        dscValues,
        index,
        rowsFilled
    );
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

template <
    typename StartIndexType,
    typename EndIndexType,
    typename DimensionType,
    typename ValueArrayType
>
void TreeNode::makeSegmentResampled(
    StartIndexType startIndex,
    EndIndexType endIndex,
    DimensionType dimension,
    ValueArrayType values,
    const TreeNode& resampleNode,
    int resampleFactor,
    int index /*= -1*/,
    int rowsFilled /*= -1*/
) const
{
    DataView argStartIndex(startIndex);
    DataView argEndIndex(endIndex);
    DataView argDimension(dimension);
    DataView argValues(values);

    mdsdsc_a_t * dscValues = (mdsdsc_a_t *)argValues.getDescriptor();
    if (rowsFilled < 0) {
        rowsFilled = _getMaxRowsFilled(dscValues);
    }

    int status = _TreeMakeSegmentResampled(
        getTree()->getDBID(),
        getNID(),
        argStartIndex.getDescriptor(),
        argEndIndex.getDescriptor(),
        argDimension.getDescriptor(),
        dscValues,
        index,
        rowsFilled,
        resampleNode.getNID(),
        resampleFactor
    );
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

// template <
//     typename StartIndexType,
//     typename EndIndexType,
//     typename DimensionType,
//     typename ValueArrayType
// >
// void TreeNode::makeSegmentMinMax(
//     StartIndexType startIndex,
//     EndIndexType endIndex,
//     DimensionType dimension,
//     ValueArrayType values,
//     const TreeNode& resampleNode,
//     int resampleFactor,
//     int index /*= -1*/,
//     int rowsFilled /*= -1*/
// ) const
// {
//     Argument argStartIndex(startIndex);
//     Argument argEndIndex(endIndex);
//     Argument argDimension(dimension);
//     Argument argValues(values);

//     mdsdsc_a_t * dscValues = (mdsdsc_a_t *)argValues.getDescriptor();
//     if (rowsFilled < 0) {
//         rowsFilled = _getMaxRowsFilled(dscValues);
//     }

//     int status = _TreeMakeSegmentMinMax(
//         getTree()->getDBID(),
//         getNID(),
//         argStartIndex.getDescriptor(),
//         argEndIndex.getDescriptor(),
//         argDimension.getDescriptor(),
//         dscValues,
//         index,
//         rowsFilled,
//         resampleNode.getNID(),
//         resampleFactor
//     );
//     if (IS_NOT_OK(status)) {
//         throwException(status);
//     }
// }

template <typename ValueArrayType>
void TreeNode::makeTimestampedSegment(
    int64_t * timestamps,
    ValueArrayType values,
    int index /*= -1*/,
    int rowsFilled /*= -1*/
) const
{
    DataView argValues(values);

    mdsdsc_a_t * dscValues = (mdsdsc_a_t *)argValues.getDescriptor();
    if (rowsFilled < 0) {
        rowsFilled = _getMaxRowsFilled(dscValues);
    }

    int status = _TreeMakeTimestampedSegment(
        getTree()->getDBID(),
        getNID(),
        timestamps,
        dscValues,
        index,
        rowsFilled
    );
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

inline void Tree::open()
{
    int status;

    if (_open) {
        close();
    }

    switch (_mode) {
    case Mode::Normal:
        status = _TreeOpen(&_dbid, _treename.c_str(), _shot, 0);
        break;
    case Mode::ReadOnly:
        status = _TreeOpen(&_dbid, _treename.c_str(), _shot, 1);
        break;
    case Mode::Edit:
        status = _TreeOpenEdit(&_dbid, _treename.c_str(), _shot);
        break;
    case Mode::New:
        status = _TreeOpenNew(&_dbid, _treename.c_str(), _shot);
        break;
    default: ;
    }

    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    // Initialize TreeNode parent class to \\TOP
    _tree = this;
    _nid = 0;

    _open = true;
}

inline void Tree::close()
{
    int status = _TreeClose(&_dbid, _treename.c_str(), _shot);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    // Cleanup TreeNode
    _tree = nullptr;
    _nid = -1;

    _open = false;
}

inline void Tree::write()
{
    int status = _TreeWriteTree(&_dbid, _treename.c_str(), _shot);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

inline void Tree::setDefaultNode(const TreeNode& node) const
{
    int status = _TreeSetDefaultNid(getDBID(), node.getNID());
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

inline TreeNode Tree::getDefaultNode() const
{
    int nid;
    int status = _TreeGetDefaultNid(getDBID(), &nid);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return TreeNode(getTree(), nid);
}

template <typename ResultType /*= Data*/, typename ...ArgTypes>
ResultType Tree::compileData(const std::string& expression, ArgTypes... args) const
{
    DataView argExp(expression);
    std::vector<DataView> argList = { DataView(args)... };

    std::vector<mdsdsc_t *> dscList = { argExp.getDescriptor() };
    for (const auto& arg : argList) {
        dscList.push_back(arg.getDescriptor());
    }

    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    int status = TdiIntrinsic(OPC_COMPILE, dscList.size(), dscList.data(), &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out), getTree()).releaseAndConvert<ResultType>();
}

template <typename ResultType /*= Data*/, typename ...ArgTypes>
ResultType Tree::executeData(const std::string& expression, ArgTypes... args) const
{
    DataView argExp(expression);
    std::vector<DataView> argList = { DataView(args)... };

    std::vector<mdsdsc_t *> dscList = { argExp.getDescriptor() };
    for (const auto& arg : argList) {
        dscList.push_back(arg.getDescriptor());
    }

    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    int status = _TdiIntrinsic(getContext(), OPC_EXECUTE, dscList.size(), dscList.data(), &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out), getTree()).releaseAndConvert<ResultType>();
}

inline void Device::addParts(std::vector<DevicePart>&& parts) const
{
    auto defaultNode = getTree()->getDefaultNode();
    getTree()->setDefaultNode(*this);

    for (const auto& part : parts)
    {
        printf("Adding %s\n", part.Path.c_str());
        auto node = addNode(part.Path, part.Usage);

        if (!part.ValueExpression.empty()) {
            node.putRecord(getTree()->compileData(part.ValueExpression));
        }
        else {
            node.putRecord(*part.Value);
        }

        node.setFlagsOn(part.Flags);
    }

    for (auto& part : parts)
    {
        if (part.Value) {
            delete part.Value;
            part.Value = nullptr;
        }

        for (auto& it : part.XNCI) {
            if (it.second) {
                delete it.second;
                it.second = nullptr;
            }
        }
    }

    getTree()->setDefaultNode(defaultNode);
}

inline void Connection::connect(const std::string& hostspec)
{
    if (hostspec == "local") {
        _local = true;
    }
    else {
        // TODO: SetCompressionLevel
        _id = ConnectToMds(const_cast<char *>(hostspec.c_str()));
        if (_id == InvalidConnectionID) {
            // TODO:
            throw MDSplusException();
        }
    }

    _hostspec = hostspec;
}

inline void Connection::disconnect()
{
    _local = false;

    if (_id != InvalidConnectionID) {
        DisconnectFromMds(_id);
        _id = InvalidConnectionID;
    }
}

inline Data Connection::_get(const std::string& expression, std::vector<DataView>&& argList) const
{
    int status;
    int argIndex = 0;
    uint8_t numberOfArgs = argList.size() + 1;

    status = SendArg(
        _id,
        argIndex,
        DTYPE_T,
        numberOfArgs,
        expression.size(),
        0,
        nullptr,
        const_cast<char *>(expression.data())
    );
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    ++argIndex;

    for (auto& arg : argList) {
        status = MDSplusERROR;

        mdsdsc_t * dscArg = arg.getDescriptor();

        if (dscArg->class_ == CLASS_S) {
            mdsdsc_s_t * scalar = reinterpret_cast<mdsdsc_s_t *>(dscArg);
            status = SendArg(
                _id,
                argIndex,
                scalar->dtype,
                numberOfArgs,
                scalar->length,
                0,
                nullptr,
                scalar->pointer
            );
        }
        else if (dscArg->class_ == CLASS_A) {
            array_coeff * array = reinterpret_cast<array_coeff *>(dscArg);
            status = SendArg(
                _id,
                argIndex,
                array->dtype,
                numberOfArgs,
                array->length,
                array->dimct,
                reinterpret_cast<int *>(array->m),
                array->pointer
            );
        }

        // TODO: Throw exception for unsupported types

        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        ++argIndex;
    }

    // Switch to temporary variables and building a descriptor :(

    array_coeff * response = (array_coeff *)calloc(1, sizeof(array_coeff));
    mdsdsc_xd_t dscResponse = {
        .length = 0,
        .dtype = DTYPE_DSC,
        .class_ = CLASS_XD,
        .pointer = (mdsdsc_t *)response,
        .l_length = sizeof(array_coeff),
    };

    void * message = nullptr;
    status = GetAnswerInfoTO(
        _id,
        reinterpret_cast<char *>(&response->dtype),
        reinterpret_cast<short *>(&response->length),
        reinterpret_cast<char *>(&response->dimct),
        reinterpret_cast<int *>(response->m),
        reinterpret_cast<int *>(&response->length),
        reinterpret_cast<void **>(&response->pointer),
        &message,
        -1
    );
    if (IS_NOT_OK(status)) {
        if (response->dtype == DTYPE_T) {
            printf("%.*s\n", response->length, response->pointer);
            fflush(stdout);
        }

        throwException(status);
    }

    if (response->dimct == 0) {
        response->class_ = CLASS_S;
    }
    else {
        response->class_ = CLASS_A;

        // Calculate arsize
        arsize_t size = 1;
        for (int i = 0; i < response->dimct; ++i) {
            size *= response->m[i];
        }
        response->arsize = size * response->length;
    }

    switch (response->dtype) {
    case DTYPE_F:
        response->dtype = DTYPE_FLOAT;
        break;
    case DTYPE_D:
        response->dtype = DTYPE_DOUBLE;
        break;
    case DTYPE_FC:
        response->dtype = DTYPE_FLOAT_COMPLEX;
        break;
    case DTYPE_DC:
        response->dtype = DTYPE_DOUBLE_COMPLEX;
        break;
    default: ;
    }

    return Data(std::move(dscResponse));
}

} // namespace mdsplus

namespace std {

template <> struct hash<mdsplus::String>
{
    size_t operator()(const mdsplus::String& data) const
    {
        #ifdef __cpp_lib_string_view
            return std::hash<std::string_view>()(data.getStringView());
        #else
            return std::hash<std::string>()(data.getString());
        #endif
    }
};

#define _MDSPLUS_SPECIALIZE_STD_HASH(ScalarType)                      \
    template <> struct hash<ScalarType>                               \
    {                                                                 \
        size_t operator()(const ScalarType& data) const {             \
            return std::hash<ScalarType::__ctype>()(data.getValue()); \
        }                                                             \
    };

_MDSPLUS_SPECIALIZE_STD_HASH(mdsplus::Int8)
_MDSPLUS_SPECIALIZE_STD_HASH(mdsplus::Int16)
_MDSPLUS_SPECIALIZE_STD_HASH(mdsplus::Int32)
_MDSPLUS_SPECIALIZE_STD_HASH(mdsplus::Int64)
_MDSPLUS_SPECIALIZE_STD_HASH(mdsplus::UInt8)
_MDSPLUS_SPECIALIZE_STD_HASH(mdsplus::UInt16)
_MDSPLUS_SPECIALIZE_STD_HASH(mdsplus::UInt32)
_MDSPLUS_SPECIALIZE_STD_HASH(mdsplus::UInt64)
_MDSPLUS_SPECIALIZE_STD_HASH(mdsplus::Float32)
_MDSPLUS_SPECIALIZE_STD_HASH(mdsplus::Float64)

} // namespace std

#endif // MDSPLUS_HPP
