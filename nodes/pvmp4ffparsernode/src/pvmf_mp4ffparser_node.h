/* ------------------------------------------------------------------
 * Copyright (C) 1998-2009 PacketVideo
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 * -------------------------------------------------------------------
 */
#ifndef PVMF_MP4FFPARSER_NODE_H_INCLUDED
#define PVMF_MP4FFPARSER_NODE_H_INCLUDED

#ifndef PVLOGGER_FILE_APPENDER_H_INCLUDED
#include "pvlogger_file_appender.h"
#endif

#ifndef OSCL_BIN_STREAM_H_INCLUDED
#include "oscl_bin_stream.h"
#endif

#ifndef PVMF_NODE_INTERFACE_IMPL_H_INCLUDED
#include "pvmf_node_interface_impl.h"
#endif

#ifndef PVMF_MP4FFPARSER_DEFS_H_INCLUDED
#include "pvmf_mp4ffparser_defs.h"
#endif

#ifndef PVMF_DATA_SOURCE_PLAYBACK_CONTROL_H_INCLUDED
#include "pvmf_data_source_playback_control.h"
#endif

#ifndef PVMF_META_DATA_EXTENSION_H_INCLUDED
#include "pvmf_meta_data_extension.h"
#endif

#ifndef PVMF_DATA_SOURCE_INIT_EXTENSION_H_INCLUDED
#include "pvmf_data_source_init_extension.h"
#endif

#ifndef PVMF_TRACK_SELECTION_EXTENSION_H_INCLUDED
#include "pvmf_track_selection_extension.h"
#endif

#ifndef PVMF_TRACK_LEVEL_INFO_EXTENSION_H_INCLUDED
#include "pvmf_track_level_info_extension.h"
#endif

#ifndef PVMF_MP4_PROGDOWNLOAD_SUPPORT_EXTENSION_H_INCLUDED
#include "pvmf_mp4_progdownload_support_extension.h"
#endif

#ifndef CPM_H_INCLUDED
#include "cpm.h"
#endif

#ifndef PVMF_LOCAL_DATA_SOURCE_H_INCLUDED
#include "pvmf_local_data_source.h"
#endif

#ifndef PV_GAU_H_
#include "pv_gau.h"
#endif

#ifndef PVMF_FORMAT_PROGDOWNLOAD_SUPPORT_EXTENSION_H_INCLUDED
#include "pvmf_format_progdownload_support_extension.h"
#endif

#ifndef PVMF_DOWNLOAD_PROGRESS_EXTENSION_H
#include "pvmf_download_progress_interface.h"
#endif

#ifndef PVMI_DATASTREAMUSER_INTERFACE_H_INCLUDED
#include "pvmi_datastreamuser_interface.h"
#endif

#ifndef PVMF_GAPLESS_METADATA_H_INCLUDED
#include "pvmf_gapless_metadata.h"
#endif

#ifndef PVMF_COMMON_AUDIO_DECNODE_H_INCLUDE
#include "pvmf_common_audio_decnode.h"
#endif

#ifndef PVMF_MP4FFPARSER_OUTPORT_H_INCLUDED
#include "pvmf_mp4ffparser_outport.h"
#endif

#ifndef PVMF_SOURCE_CONTEXT_DATA_H_INCLUDED
#include "pvmf_source_context_data.h"
#endif

#ifndef PVMF_CPMPLUGIN_LICENSE_INTERFACE_H_INCLUDED
#include "pvmf_cpmplugin_license_interface.h"
#endif

#ifndef PVMI_KVP_UTIL_H_INCLUDED
#include "pvmi_kvp_util.h"
#endif

#ifndef PVMF_DATA_SOURCE_DIRECTION_CONTROL_H_INCLUDED
#include "pvmf_data_source_direction_control.h"
#endif

/**
* Node command handling
*/

class PVMFMP4ParserNodeLoggerDestructDealloc : public OsclDestructDealloc
{
    public:
        void destruct_and_dealloc(OsclAny* ptr)
        {
            PVLoggerAppender* p = OSCL_REINTERPRET_CAST(PVLoggerAppender*, ptr);
            BinaryFileAppender* binPtr = OSCL_REINTERPRET_CAST(BinaryFileAppender*, p);
            if (!binPtr)
                return;
            OSCL_DELETE(binPtr);
        }
};

#define PVMF_MP4FFPARSERNODE_UNDERFLOW_STATUS_TIMER_ID 1

#define PVMF_MP4FFPARSERNODE_MAX_CPM_METADATA_KEYS 256

#define NORMAL_PLAYRATE 100000


/**
* The Node class
*/
class IMpeg4File;
class PVMFMP4FFParserOutPort;
class PVMFMP4FFPortIter;
class PVLogger;
class PVMFMediaClock;

enum BaseKeys_SelectionType
{
    INVALID = 0,
    NET,
    FILE_IO
};

#define PVMFFFPARSERNODE_MAX_NUM_TRACKS 6

class PVMFMP4FFParserNode
        : public PVMFNodeInterfaceImpl
        , public PVMFDataSourceInitializationExtensionInterface
        , public PVMFTrackSelectionExtensionInterface
        , public PvmfDataSourcePlaybackControlInterface
        , public PVMFMetadataExtensionInterface
        , public PVMFTrackLevelInfoExtensionInterface
        , public PVMFCPMStatusObserver
        , public PvmiDataStreamObserver
        , public PVMIDatastreamuserInterface
        , public PVMFFormatProgDownloadSupportInterface
        , public OsclTimerObserver
        , public PvmiCapabilityAndConfigBase
        , public PVMFMediaClockStateObserver // For observing the playback clock states
        , public PvmfDataSourceDirectionControlInterface
{
    public:
        PVMFMP4FFParserNode(int32 aPriority = OsclActiveObject::EPriorityNominal);
        virtual ~PVMFMP4FFParserNode();

        // From PVMFNodeInterface
        PVMFStatus QueryInterfaceSync(PVMFSessionId aSession,
                                      const PVUuid& aUuid,
                                      PVInterface*& aInterfacePtr);

        /* cap config interface */
        virtual void setObserver(PvmiConfigAndCapabilityCmdObserver* aObserver)
        {
            ciObserver = aObserver;
        }

        virtual PVMFStatus getParametersSync(PvmiMIOSession aSession,
                                             PvmiKeyType aIdentifier,
                                             PvmiKvp*& aParameters,
                                             int& aNumParamElements,
                                             PvmiCapabilityContext aContext);
        virtual PVMFStatus releaseParameters(PvmiMIOSession aSession,
                                             PvmiKvp* aParameters,
                                             int num_elements);
        virtual void setParametersSync(PvmiMIOSession aSession, PvmiKvp* aParameters,
                                       int num_elements, PvmiKvp * & aRet_kvp);
        virtual PVMFStatus verifyParametersSync(PvmiMIOSession aSession,
                                                PvmiKvp* aParameters,
                                                int num_elements);

        PVMFStatus GetConfigParameter(PvmiKvp*& aParameters, int& aNumParamElements,
                                      int32 aIndex, PvmiKvpAttr reqattr);
        PVMFStatus VerifyAndSetConfigParameter(int index, PvmiKvp& aParameter, bool set);


        // From PVInterface
        void addRef();
        void removeRef();
        bool queryInterface(const PVUuid& uuid, PVInterface*& iface);
        PVMFPortIter* GetPorts(const PVMFPortFilter* aFilter = NULL);

        // From PVMFDataSourceInitializationExtensionInterface
        void AudioSinkEvent(PVMFStatus aEvent, uint32 aStreamId)
        {
            OSCL_UNUSED_ARG(aEvent);
            OSCL_UNUSED_ARG(aStreamId);
            //ignore
        }
        PVMFStatus SetSourceInitializationData(OSCL_wString& aSourceURL, PVMFFormatType& aSourceFormat, OsclAny* aSourceData,
                                               uint32 aClipIndex = 0, PVMFFormatTypeDRMInfo aType = PVMF_FORMAT_TYPE_CONNECT_DRM_INFO_UNKNOWN);
        PVMFStatus SetClientPlayBackClock(PVMFMediaClock* aClientClock);
        PVMFStatus SetEstimatedServerClock(PVMFMediaClock* aClientClock);

        // From PVMFTrackSelectionExtensionInterface
        PVMFStatus GetMediaPresentationInfo(PVMFMediaPresentationInfo& aInfo);
        PVMFStatus SelectTracks(PVMFMediaPresentationInfo& aInfo);

        // From PVMFMetadataExtensionInterface
        PVMFStatus SetMetadataClipIndex(uint32 aClipIndex)
        {
            return (aClipIndex == 0) ? PVMFSuccess : PVMFErrArgument;
        }
        uint32 GetNumMetadataKeys(char* aQueryKeyString = NULL);
        uint32 GetNumMetadataValues(PVMFMetadataList& aKeyList);

        PVMFCommandId GetNodeMetadataKeys(PVMFSessionId aSessionId, PVMFMetadataList& aKeyList, uint32 aStartingKeyIndex, int32 aMaxKeyEntries,
                                          char* aQueryKeyString = NULL, const OsclAny* aContextData = NULL);
        PVMFCommandId GetNodeMetadataValues(PVMFSessionId aSessionId, PVMFMetadataList& aKeyList,
                                            Oscl_Vector<PvmiKvp, OsclMemAllocator>& aValueList, uint32 aStartingValueIndex, int32 aMaxValueEntries, const OsclAny* aContextData = NULL);
        PVMFStatus ReleaseNodeMetadataKeys(PVMFMetadataList& aKeyList, uint32 aStartingKeyIndex, uint32 aEndKeyIndex);
        PVMFStatus ReleaseNodeMetadataValues(Oscl_Vector<PvmiKvp, OsclMemAllocator>& aValueList, uint32 aStartingValueIndex, uint32 aEndValueIndex);

        // From PvmfDataSourcePlaybackControlInterface
        PVMFCommandId SetDataSourcePosition(PVMFSessionId aSessionId, PVMFTimestamp aTargetNPT, PVMFTimestamp& aActualNPT,
                                            PVMFTimestamp& aActualMediaDataTS, bool aSeekToSyncPoint = true, uint32 aStreamID = 0, OsclAny* aContext = NULL);
        PVMFCommandId QueryDataSourcePosition(PVMFSessionId aSessionId, PVMFTimestamp aTargetNPT, PVMFTimestamp& aActualNPT,
                                              bool aSeekToSyncPoint = true, OsclAny* aContext = NULL);
        PVMFCommandId QueryDataSourcePosition(PVMFSessionId aSessionId, PVMFTimestamp aTargetNPT,
                                              PVMFTimestamp& aSeekPointBeforeTargetNPT, PVMFTimestamp& aSeekPointAfterTargetNPT,  OsclAny* aContext = NULL, bool aSeekToSyncPoint = true);

        PVMFCommandId SetDataSourceRate(PVMFSessionId aSession, int32 aRate, PVMFTimebase* aTimebase = NULL, OsclAny* aContext = NULL);
        PVMFCommandId SetDataSourceDirection(PVMFSessionId aSessionId, int32 aDirection, PVMFTimestamp& aActualNPT,
                                             PVMFTimestamp& aActualMediaDataTS, PVMFTimebase* aTimebase, OsclAny* aContext);

        // From PVMFTrackLevelInfoExtensionInterface
        PVMFStatus GetAvailableTracks(Oscl_Vector<PVMFTrackInfo, OsclMemAllocator>& aTracks);
        PVMFStatus GetTimestampForSampleNumber(PVMFTrackInfo& aTrackInfo, uint32 aSampleNum, PVMFTimestamp& aTimestamp);
        PVMFStatus GetSampleNumberForTimestamp(PVMFTrackInfo& aTrackInfo, PVMFTimestamp aTimestamp, uint32& aSampleNum);
        PVMFStatus GetNumberOfSyncSamples(PVMFTrackInfo& aTrackInfo, int32& aNumSyncSamples);
        PVMFStatus GetSyncSampleInfo(PVMFTrackInfo& aTrackInfo, PVMFSampleNumTSList& aList, uint32 aStartIndex = 0, int32 aMaxEntries = -1);
        PVMFStatus GetSyncSampleInfo(PVMFSampleNumTSList& aList, PVMFTrackInfo& aTrackInfo, int32 aTargetTimeInMS, uint32 aHowManySamples = 1);
        PVMFStatus GetTimestampForDataPosition(PVMFTrackInfo& aTrackInfo, uint32 aDataPosition, PVMFTimestamp& aTimestamp);
        PVMFStatus GetDataPositionForTimestamp(PVMFTrackInfo& aTrackInfo, PVMFTimestamp aTimestamp, uint32& aDataPosition);


        // From PVMFPortActivityHandler
        void HandlePortActivity(const PVMFPortActivity& aActivity);

        // From PVMFCPMStatusObserver
        void CPMCommandCompleted(const PVMFCmdResp& aResponse);

        /* From PVMIDatastreamuserInterface */
        void PassDatastreamFactory(PVMFDataStreamFactory& aFactory,
                                   int32 aFactoryTag,
                                   const PvmfMimeString* aFactoryConfig = NULL);
        void PassDatastreamReadCapacityObserver(PVMFDataStreamReadCapacityObserver* aObserver);


        /* From PVMFFormatProgDownloadSupportInterface */
        int32 convertSizeToTime(uint32 fileSize, uint32& aNPTInMS);
        void setFileSize(const uint32 aFileSize);
        void setDownloadProgressInterface(PVMFDownloadProgressInterface*);
        void playResumeNotification(bool aDownloadComplete);
        void notifyDownloadComplete();

        // From OsclTimer
        void TimeoutOccurred(int32 timerID, int32 timeoutInfo);

        //from PVMFMediaClockStateObserver
        void ClockStateUpdated();
        void NotificationsInterfaceDestroyed();

    private:
        // from OsclTimerObject
        void Run();

        // Event reporting
        void ReportMP4FFParserErrorEvent(PVMFEventType aEventType, OsclAny* aEventData = NULL, PVUuid* aEventUUID = NULL, int32* aEventCode = NULL);
        void ReportMP4FFParserInfoEvent(PVMFEventType aEventType, OsclAny* aEventData = NULL, PVUuid* aEventUUID = NULL, int32* aEventCode = NULL);
        void ChangeNodeState(TPVMFNodeInterfaceState aNewState);

        PVMFStatus HandleExtensionAPICommands();

        // Node command handlers
        PVMFStatus DoQueryUuid();
        PVMFStatus DoQueryInterface();
        PVMFStatus DoRequestPort(PVMFPortInterface*&);
        void GetTrackMaxParameters(PVMFFormatType aFormatType, uint32& aMaxDataSize, uint32& aMaxQueueDepth);
        PVMFStatus DoReleasePort();

        PVMFStatus DoInit();
        bool ParseMP4File(PVMFNodeCommand& aCmd, PVMFStatus& aStatus);
        PVMFStatus InitMetaData();
        PVMFStatus InitImotionMetaData();
        uint32 CountImotionMetaDataKeys();
        int32 CountMetaDataKeys();
        PVMFStatus CompleteInit(PVMFNodeCommand& aCmd);
        void CompleteCancelAfterInit();

        PVMFStatus DoPrepare();
        PVMFStatus DoStop();
        PVMFStatus DoPause();

        PVMFStatus DoReset();
        PVMFStatus CompleteReset();

        PVMFStatus CancelCurrentCommand();
        // For metadata extention interface
        PVMFStatus DoGetNodeMetadataKeys();
        PVMFStatus CompleteGetMetadataKeys();

        void PushToAvailableMetadataKeysList(const char* aKeystr, char* aOptionalParam = NULL);
        PVMFStatus GetIndexParamValues(const char* aString, uint32& aStartIndex, uint32& aEndIndex);
        PVMFStatus CreateNewArray(uint32** aTrackidList, uint32 aNumTracks);

        PVMFStatus DoGetNodeMetadataValues();
        void CompleteGetMetaDataValues();
        int32 AddToValueList(Oscl_Vector<PvmiKvp, OsclMemAllocator>& aValueList, PvmiKvp& aNewValue);

        PVMFStatus GetVideoFrameWidth(uint32 aId, int32& aWidth, int32& aDisplayWidth);
        PVMFStatus GetVideoFrameHeight(uint32 aId, int32& aHeight, int32& aDisplayHeight);
        int32 FindVideoWidth(uint32 aId);
        int32 FindVideoHeight(uint32 aId);
        int32 FindVideoDisplayWidth(uint32 aId);
        int32 FindVideoDisplayHeight(uint32 aId);
        PVMFStatus PopulateVideoDimensions(uint32 aId);
        PVMFStatus FindBestThumbnailKeyFrame(uint32 aId, uint32& aKeyFrameNum);

        // For data source position extension interface
        PVMFStatus DoSetDataSourcePosition();
        PVMFStatus DoQueryDataSourcePosition();
        PVMFStatus DoSetDataSourceRate();

        PVMFStatus DoSetDataSourceDirection();

        void HandleTrackState();
        bool RetrieveTrackConfigInfo(uint32 aTrackId,
                                     PVMFFormatType aFormatType,
                                     OsclRefCounterMemFrag &aConfig);
        bool RetrieveTrackConfigInfoAndFirstSample(uint32 aTrackId,
                PVMFFormatType aFormatType,
                OsclRefCounterMemFrag &aConfig);
        bool RetrieveTrackData(PVMP4FFNodeTrackPortInfo& aTrackPortInfo);
        bool SendTrackData(PVMP4FFNodeTrackPortInfo& aTrackPortInfo);
        bool GenerateAVCNALGroup(PVMP4FFNodeTrackPortInfo& aTrackPortInfo, OsclSharedPtr<PVMFMediaDataImpl>& aMediaFragGroup);
        bool GenerateAACFrameFrags(PVMP4FFNodeTrackPortInfo& aTrackPortInfo, OsclSharedPtr<PVMFMediaDataImpl>& aMediaFragGroup);
        bool GetAVCNALLength(OsclBinIStreamBigEndian& stream, uint32& lengthSize, int32& len);
        bool UpdateTextSampleEntry(PVMP4FFNodeTrackPortInfo& aTrackPortInfo, uint32 aEntryIndex, PVMFTimedTextMediaData& aTextMediaData);
        uint32 GetGaplessDuration(PVMP4FFNodeTrackPortInfo& aTrackPortInfo);

        // Port processing
        void ProcessPortActivity();
        void QueuePortActivity(const PVMFPortActivity& aActivity);
        PVMFStatus ProcessIncomingMsg(PVMFPortInterface* aPort);
        PVMFStatus ProcessOutgoingMsg(PVMFPortInterface* aPort);
        Oscl_Vector<PVMFPortActivity, OsclMemAllocator> iPortActivityQueue;

        friend class PVMFMP4FFParserOutPort;

        PVMFFormatType GetFormatTypeFromMIMEType(PvmfMimeString* aMIMEString);

        void ResetAllTracks();
        bool ReleaseAllPorts();
        void RemoveAllCommands();
        void CleanupFileSource();

        // For comparison with download progress clock
        bool checkTrackPosition(PVMP4FFNodeTrackPortInfo& aTrackPortInfo, uint32 numsamples);

        bool GetTrackPortInfoForTrackID(PVMP4FFNodeTrackPortInfo*& aInfo,
                                        uint32 aTrackID);

        bool GetTrackPortInfoForPort(PVMP4FFNodeTrackPortInfo*& aInfo,
                                     PVMFPortInterface* aPort);

        uint32 GetNumAudioChannels(uint32 aId);
        uint32 GetAudioSampleRate(uint32 aId);
        uint32 GetAudioBitsPerSample(uint32 aId);


        OSCL_wHeapString<OsclMemAllocator> iFilename;
        PVMFFormatType iSourceFormat;
        PVMFMediaClock* iClientPlayBackClock;
        PVMFMediaClockNotificationsInterface *iClockNotificationsInf;
        bool iUseCPMPluginRegistry;
        PVMFLocalDataSource iCPMSourceData;
        PVMFSourceContextData iSourceContextData;
        OsclFileHandle* iFileHandle;
        Oscl_FileServer iFileServer;
        uint32 iParsingMode;
        bool iProtectedFile;
        IMpeg4File* iMP4FileHandle;
        uint32 iMP4ParserNodeMetadataValueCount;
        Oscl_Vector<OSCL_HeapString<OsclMemAllocator>, OsclMemAllocator> iCPMMetadataKeys;
        Oscl_Vector<PVMP4FFNodeTrackPortInfo, OsclMemAllocator> iNodeTrackPortList;
        Oscl_Vector<PVMFTrackInfo, OsclMemAllocator> iSelectedTrackInfoList;
        Oscl_Vector<VideoTrackDimensionInfo, OsclMemAllocator> iVideoDimensionInfoVec;

        PVMFMP4FFPortIter* iPortIter;

        // stream id
        uint32 iStreamID;


        PVLogger* iLogger;
        PVLogger* iDataPathLogger;
        PVLogger* iAVCDataPathLogger;
        PVLogger* iClockLogger;
        PVLogger* iDiagnosticsLogger;
        PVLogger* iGaplessLogger;
        // Reference counter for extension


        // variables to support download autopause
        OsclSharedPtr<PVMFMediaClock> download_progress_clock;
        PVMFDownloadProgressInterface* download_progress_interface;
        uint32 iDownloadFileSize;
        bool autopaused;

        void DataStreamCommandCompleted(const PVMFCmdResp& aResponse);
        void DataStreamInformationalEvent(const PVMFAsyncEvent& aEvent);
        void DataStreamErrorEvent(const PVMFAsyncEvent& aEvent);

        bool MapMP4ErrorCodeToEventCode(int32 aMP4ErrCode, PVUuid& aEventUUID, int32& aEventCode);

        GAU iGau;

        bool iThumbNailMode;

        // Content Policy Manager related
        bool iSourceContextDataValid;
        bool iPreviewMode;
        PVMFCPM* iCPM;
        PVMFSessionId iCPMSessionID;
        PVMFCPMContentType iCPMContentType;
        PVMFCPMPluginAccessInterfaceFactory* iCPMContentAccessFactory;
        PVMFMetadataExtensionInterface* iCPMMetaDataExtensionInterface;
        PVMFCPMPluginLicenseInterface* iCPMLicenseInterface;
        PVInterface* iCPMLicenseInterfacePVI;
        PVMFCPMPluginAccessUnitDecryptionInterface* iDecryptionInterface;
        PvmiKvp iRequestedUsage;
        PvmiKvp iApprovedUsage;
        PvmiKvp iAuthorizationDataKvp;
        PVMFCPMUsageID iUsageID;
        bool oWaitingOnLicense;
        bool iPoorlyInterleavedContentEventSent;

        PVMFCommandId iCPMInitCmdId;
        PVMFCommandId iCPMOpenSessionCmdId;
        PVMFCommandId iCPMRegisterContentCmdId;
        PVMFCommandId iCPMRequestUsageId;
        PVMFCommandId iCPMUsageCompleteCmdId;
        PVMFCommandId iCPMCloseSessionCmdId;
        PVMFCommandId iCPMResetCmdId;
        PVMFCommandId iCPMGetMetaDataKeysCmdId;
        PVMFCommandId iCPMGetMetaDataValuesCmdId;
        PVMFCommandId iCPMGetLicenseInterfaceCmdId;
        void InitCPM();
        void OpenCPMSession();
        void CPMRegisterContent();
        bool GetCPMContentAccessFactory();
        void GetCPMContentType();
        bool GetCPMMetaDataExtensionInterface();
        void GetCPMLicenseInterface();
        void RequestUsage(PVMP4FFNodeTrackOMA2DRMInfo* aInfo);
        void SendUsageComplete();
        void CloseCPMSession();
        void ResetCPM();
        void GetCPMMetaDataKeys();
        PVMFStatus CheckCPMCommandCompleteStatus(PVMFCommandId, PVMFStatus);
        PVMFStatus iCPMRequestUsageCommandStatus;

        PVMFStatus DoGetLicense(bool aWideCharVersion = false);
        PVMFStatus DoCancelGetLicense();
        void CompleteGetLicense();

        void PopulateOMA1DRMInfo();
        /*
         * OMA2 DRM Related Methods
         */
        Oscl_Vector<PVMP4FFNodeTrackOMA2DRMInfo, OsclMemAllocator> iOMA2DRMInfoVec;
        PVMP4FFNodeTrackOMA2DRMInfo* LookUpOMA2TrackInfoForTrack(uint32 aTrackID);
        PVMFStatus InitOMA2DRMInfo();
        void PopulateOMA2DRMInfo(PVMP4FFNodeTrackOMA2DRMInfo* aInfo);
        PVMFStatus CheckForOMA2AuthorizationComplete(PVMP4FFNodeTrackOMA2DRMInfo*& aInfo);
        void OMA2TrackAuthorizationComplete();
        bool CheckForOMA2UsageApproval();
        void ResetOMA2Flags();
        uint8* iOMA2DecryptionBuffer;

        PVMFStatus GetFileOffsetForAutoResume(uint32& aOffset, bool aPortsAvailable = true);
        PVMFStatus GetFileOffsetForAutoResume(uint32& aOffset, PVMP4FFNodeTrackPortInfo* aInfo);

        PVMFStatus CheckForUnderFlow(PVMP4FFNodeTrackPortInfo* aInfo);

        PVMFStatus CheckForMP4HeaderAvailability();
        int32 CreateErrorInfoMsg(PVMFBasicErrorInfoMessage** aErrorMsg, PVUuid aEventUUID, int32 aEventCode);
        void CreateDurationInfoMsg(uint32 adurationms);
        PVMFStatus PushValueToList(Oscl_Vector<OSCL_HeapString<OsclMemAllocator>, OsclMemAllocator> &aRefMetadataKeys,
                                   PVMFMetadataList *&aKeyListPtr, uint32 aLcv);
        void GetGaplessMetadata(PVMP4FFNodeTrackPortInfo& aInfo);
        bool SendBeginOfClipCommand(PVMP4FFNodeTrackPortInfo& aTrackPortInfo);
        bool SendEndOfClipCommand(PVMP4FFNodeTrackPortInfo& aTrackPortInfo);

        PVMIDataStreamSyncInterface* iDataStreamInterface;
        PVMFDataStreamFactory* iDataStreamFactory;
        PVMFDataStreamReadCapacityObserver* iDataStreamReadCapacityObserver;
        PvmiDataStreamSession iDataStreamSessionID;
        PvmiDataStreamCommandId iRequestReadCapacityNotificationID;
        uint32 iMP4HeaderSize;
        bool iDownloadComplete;
        bool iProgressivelyPlayable;
        uint32 iLastNPTCalcInConvertSizeToTime;
        uint32 iFileSizeLastConvertedToTime;
        bool iFastTrackSession;
        bool iSmoothStreamingSession;

        /* External PseudoStreaming related */
        bool iExternalDownload;

        bool iUnderFlowEventReported;
        PVMFStatus ReportUnderFlow();
        OsclTimer<OsclMemAllocator> *iUnderFlowCheckTimer;

        /* bitstream logging */
        void LogMediaData(PVMFSharedMediaDataPtr data,
                          PVMFPortInterface* aPort);
        bool iPortDataLog;
        char iLogFileIndex;
        OSCL_HeapString<OsclMemAllocator> portLogPath;

        uint32 minTime;
        uint32 avgTime;
        uint32 maxTime;
        uint32 sumTime;
        bool iDiagnosticsLogged;
        void LogDiagnostics();
        uint32 iTimeTakenInReadMP4File;
        bool iBackwardReposFlag; /* To avoid backwardlooping :: A flag to remember backward repositioning */
        bool iForwardReposFlag;
        uint32 iCurPos;
        bool iEOTForTextSentToMIO;

        bool iSetTextSampleDurationZero;

        /* To take into account if we get negative TS for text track after repositionings*/
        bool iTextInvalidTSAfterReposition;

        uint32 iDelayAddToNextTextSample;

        uint32 iCacheSize;
        uint32 iAsyncReadBuffSize;
        bool iPVLoggerEnableFlag;
        bool iPVLoggerStateEnableFlag;
        uint32 iNativeAccessMode;

        BaseKeys_SelectionType iBaseKey;
        uint32 iJitterBufferDurationInMs;
        bool iDataStreamRequestPending;
        bool iCPMSequenceInProgress;
        bool oIsAACFramesFragmented;

        int32 iPlayBackDirection;
        int32 iStartForNextTSSearch;
        int32 iPrevSampleTS;
        bool iParseAudioDuringFF;
        bool iParseAudioDuringREW;
        bool iParseVideoOnly;
        bool iOpenFileOncePerTrack;
        int32 iDataRate;
        int32 minFileOffsetTrackID;
        PVMFMetadataList iAvailableMetadataKeys;
        //This will hold the total number of ID3 specific values present in the value list
        uint32 iTotalID3MetaDataTagInValueList;

};


class PVMFMP4FFPortIter : public PVMFPortIter
{
    public:
        PVMFMP4FFPortIter(Oscl_Vector<PVMP4FFNodeTrackPortInfo, OsclMemAllocator>& aTrackList)
        {
            iIndex = 0;
            iTrackList = &aTrackList;
        };

        virtual ~PVMFMP4FFPortIter() {};

        uint16 NumPorts()
        {
            if (iTrackList->size() < 0xFFFF)
            {
                return (uint16)(iTrackList->size());
            }
            else
            {
                return 0xFFFF;
            }
        };

        PVMFPortInterface* GetNext()
        {
            if (iIndex < iTrackList->size())
            {
                PVMFPortInterface* portiface = (*iTrackList)[iIndex].iPortInterface;
                ++iIndex;
                return portiface;
            }
            else
            {
                return NULL;
            }
        };

        void Reset()
        {
            iIndex = 0;
        };

    private:
        Oscl_Vector<PVMP4FFNodeTrackPortInfo, OsclMemAllocator>* iTrackList;
        uint32 iIndex;
};

#endif // PVMF_MP4FFPARSER_NODE_H_INCLUDED

