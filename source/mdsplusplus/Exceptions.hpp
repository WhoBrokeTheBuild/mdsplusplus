#ifndef MDSPLUS_EXCEPTIONS_HPP
#define MDSPLUS_EXCEPTIONS_HPP

#include <stdexcept>

#include <mdsshr.h>
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

} // namespace mdsplus

#endif // MDSPLUS_EXCEPTIONS_HPP
