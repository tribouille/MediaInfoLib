// File_Mpeg4 - Info for MPEG-4 files
// Copyright (C) 2005-2011 MediaArea.net SARL, Info@MediaArea.net
//
// This library is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library. If not, see <http://www.gnu.org/licenses/>.
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Main part
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
// Compilation conditions
#include "MediaInfo/Setup.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifdef MEDIAINFO_MPEG4_YES
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Multiple/File_Mpeg4.h"
#include "MediaInfo/Multiple/File_Mpeg4_Descriptors.h"
#if defined(MEDIAINFO_MPEGPS_YES)
    #include "MediaInfo/Multiple/File_MpegPs.h"
#endif
#include "ZenLib/FileName.h"
#include "MediaInfo/MediaInfo_Internal.h"
#if MEDIAINFO_EVENTS
    #include "MediaInfo/MediaInfo_Events.h"
#endif //MEDIAINFO_EVENTS
#include "MediaInfo/Multiple/File__ReferenceFilesHelper.h"
#include "ZenLib/Format/Http/Http_Utils.h"
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Const
//***************************************************************************

namespace Elements
{
    const int64u free=0x66726565;
    const int64u mdat=0x6D646174;
    const int64u moov_meta______=0x2D2D2D2D;
    const int64u moov_meta___ART=0xA9415254;
    const int64u moov_meta___alb=0xA9616C62;
    const int64u moov_meta___aut=0xA9617574;
    const int64u moov_meta___cmt=0xA9636D74;
    const int64u moov_meta___cpy=0xA9637079;
    const int64u moov_meta___day=0xA9646179;
    const int64u moov_meta___des=0xA9646573;
    const int64u moov_meta___dir=0xA9646972;
    const int64u moov_meta___dis=0xA9646973;
    const int64u moov_meta___edl=0xA965646C;
    const int64u moov_meta___enc=0xA9656E63;
    const int64u moov_meta___fmt=0xA9666D74;
    const int64u moov_meta___gen=0xA967656E;
    const int64u moov_meta___grp=0xA9677270;
    const int64u moov_meta___hos=0xA9686F73;
    const int64u moov_meta___inf=0xA9696E66;
    const int64u moov_meta___key=0xA96B6579;
    const int64u moov_meta___mak=0xA96D616B;
    const int64u moov_meta___mod=0xA96D6F64;
    const int64u moov_meta___nam=0xA96E616D;
    const int64u moov_meta___prd=0xA9707264;
    const int64u moov_meta___PRD=0xA9505244;
    const int64u moov_meta___prf=0xA9707266;
    const int64u moov_meta___req=0xA9726571;
    const int64u moov_meta___src=0xA9737263;
    const int64u moov_meta___swr=0xA9737772;
    const int64u moov_meta___too=0xA9746F6F;
    const int64u moov_meta___wrn=0xA977726E;
    const int64u moov_meta___wrt=0xA9777274;
    const int64u moov_meta__aART=0x61415254;
    const int64u moov_meta__albm=0x616C626D;
    const int64u moov_meta__auth=0x61757468;
    const int64u moov_meta__cpil=0x6370696C;
    const int64u moov_meta__cprt=0x63707274;
    const int64u moov_meta__covr=0x636F7672;
    const int64u moov_meta__disk=0x6469736B;
    const int64u moov_meta__dscp=0x64736370;
    const int64u moov_meta__gnre=0x676E7265;
    const int64u moov_meta__name=0x6E616D65;
    const int64u moov_meta__perf=0x70657266;
    const int64u moov_meta__pgap=0x70676170;
    const int64u moov_meta__titl=0x7469746C;
    const int64u moov_meta__tool=0x746F6F6C;
    const int64u moov_meta__trkn=0x74726B6E;
    const int64u moov_meta__tmpo=0x746D706F;
    const int64u moov_meta__year=0x79656172;
    const int64u skip=0x736B6970;
    const int64u wide=0x77696465;
}

//---------------------------------------------------------------------------
Ztring Mpeg4_Encoded_Library(int32u Vendor)
{
    switch (Vendor)
    {
        case 0x33495658 : return _T("3ivX");                //3IVX
        case 0x6170706C : return _T("Apple QuickTime");     //appl
        case 0x6E696B6F : return _T("Nikon");               //niko
        case 0x6F6C796D : return _T("Olympus");             //olym
        case 0x6F6D6E65 : return _T("Omneon");              //omne
        default: return Ztring().From_CC4(Vendor);
    }
}

//---------------------------------------------------------------------------
Ztring Mpeg4_Language_Apple(int16u Language)
{
    switch (Language)
    {
        case  0 : return _T("en");
        case  1 : return _T("fr");
        case  2 : return _T("de");
        case  6 : return _T("es");
        default: return Ztring::ToZtring(Language);
    }
}

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
File_Mpeg4::File_Mpeg4()
:File__Analyze()
{
    //Configuration
    #if MEDIAINFO_EVENTS
        ParserIDs[0]=MediaInfo_Parser_Mpeg4;
        StreamIDs_Width[0]=8;
    #endif //MEDIAINFO_EVENTS
    #if MEDIAINFO_DEMUX
        Demux_Level=2; //Container
    #endif //MEDIAINFO_DEMUX
    #if MEDIAINFO_TRACE
        Trace_Layers_Update(0); //Container1
    #endif //MEDIAINFO_TRACE
    #if MEDIAINFO_DEMUX
        Demux_EventWasSent_Accept_Specific=true;
    #endif //MEDIAINFO_DEMUX

    DataMustAlwaysBeComplete=false;

    //Temp
    mdat_MustParse=false;
    TimeScale=1;
    Vendor=0x00000000;
    FirstMdatPos=(int64u)-1;
    FirstMoovPos=(int64u)-1;
    IsSecondPass=false;
    IsParsing_mdat=false;
    moov_trak_tkhd_TrackID=(int32u)-1;
    ReferenceFiles=NULL;
    #if MEDIAINFO_NEXTPACKET
        ReferenceFiles_IsParsing=false;
    #endif MEDIAINFO_NEXTPACKET
}

//***************************************************************************
// Streams management
//***************************************************************************

//---------------------------------------------------------------------------
void File_Mpeg4::Streams_Finish()
{
    #if MEDIAINFO_NEXTPACKET
        //Locators only
        if (ReferenceFiles_IsParsing)
        {
            ReferenceFiles->ParseReferences();
            #if MEDIAINFO_DEMUX
                if (Config->Demux_EventWasSent)
                    return;
            #endif //MEDIAINFO_DEMUX

            Streams_Finish_CommercialNames();
            return;
        }
    #endif MEDIAINFO_NEXTPACKET

    //Final Cut EIA-608 format
    if (Retrieve(Stream_General, 0, General_Format)==_T("Final Cut EIA-608"))
    {
        for (streams::iterator Stream=Streams.begin(); Stream!=Streams.end(); Stream++)
        {
            if (Stream->second.Parser->Count_Get(Stream_Text))
            {
                Stream_Prepare(Stream_Text);
                Fill(Stream_Text, StreamPos_Last, Text_ID, Stream->first==1?"608-1":"608-2");
                Fill(Stream_Text, StreamPos_Last, "MuxingMode", _T("Final Cut"), Unlimited);
                Merge(*Stream->second.Parser, Stream_Text, 0, StreamPos_Last);
            }
        }

        return;
    }

    Fill_Flush();
    int64u File_Size_Total=File_Size;

    //For each stream
    streams::iterator Temp=Streams.begin();
    while (Temp!=Streams.end())
    {
        //Preparing
        StreamKind_Last=Temp->second.StreamKind;
        StreamPos_Last=Temp->second.StreamPos;

        if (StreamKind_Last==Stream_Video && !Temp->second.IsTimeCode)
        {
            if (Temp->second.mdhd_TimeScale && Temp->second.stts_Min && Temp->second.stts_Max)
            {
                if (Temp->second.stts_Min!=Temp->second.stts_Max && ((float)Temp->second.mdhd_TimeScale)/Temp->second.stts_Min-((float)Temp->second.mdhd_TimeScale)/Temp->second.stts_Max>=0.001)
                {
                    Fill(Stream_Video, StreamPos_Last, Video_FrameRate_Minimum, ((float)Temp->second.mdhd_TimeScale)/Temp->second.stts_Max, 3, true);
                    Fill(Stream_Video, StreamPos_Last, Video_FrameRate_Maximum, ((float)Temp->second.mdhd_TimeScale)/Temp->second.stts_Min, 3, true);
                    Fill(Stream_Video, StreamPos_Last, Video_FrameRate,         ((float)Temp->second.stts_FrameCount)/Temp->second.mdhd_Duration*Temp->second.mdhd_TimeScale, 3, true);
                    Fill(Stream_Video, StreamPos_Last, Video_FrameRate_Mode,    "VFR", Unlimited, true, true);
                }
                else
                {
                    Fill(Stream_Video, StreamPos_Last, Video_FrameRate,         ((float)Temp->second.mdhd_TimeScale)/Temp->second.stts_Max, 3, true);
                    Fill(Stream_Video, StreamPos_Last, Video_FrameRate_Mode,    "CFR", Unlimited, true, true);
                }
            }
        }

        //Coherency testing
        if (TimeScale && !Temp->second.IsTimeCode && TimeScale && Temp->second.mdhd_TimeScale)
        {
            float32 Duration_tkhd_H=((float32)(Temp->second.tkhd_Duration+1))/TimeScale;
            float32 Duration_tkhd_L=((float32)(Temp->second.tkhd_Duration-1))/TimeScale;
            float32 Duration_stts=((float32)Temp->second.stts_Duration)/Temp->second.mdhd_TimeScale;
            float32 Duration_mdhd=((float32)Temp->second.mdhd_Duration)/Temp->second.mdhd_TimeScale;
            if (!(Duration_stts>Duration_tkhd_L && Duration_stts<Duration_tkhd_H))
            {
                //There is a difference between media/stts atom and track atom
                Fill(StreamKind_Last, StreamPos_Last, "Material_Duration", Duration_stts*1000, 0);
                if (Temp->second.stts_Min==Temp->second.stts_Max)
                {
                    Ztring Material_Duration_FirstFrame=Retrieve(StreamKind_Last, StreamPos_Last, "Duration_FirstFrame");
                    if (!Material_Duration_FirstFrame.empty())
                    {
                        Fill(StreamKind_Last, StreamPos_Last, "Material_Duration_FirstFrame", Material_Duration_FirstFrame);
                        Clear(StreamKind_Last, StreamPos_Last, "Duration_FirstFrame");
                    }
                    Ztring Material_Duration_LastFrame=Retrieve(StreamKind_Last, StreamPos_Last, "Duration_LastFrame");
                    if (!Material_Duration_LastFrame.empty())
                    {
                        Fill(StreamKind_Last, StreamPos_Last, "Material_Duration_LastFrame", Material_Duration_LastFrame);
                        Clear(StreamKind_Last, StreamPos_Last, "Duration_LastFrame");
                    }
                }
                Ztring Material_StreamSize=Retrieve(StreamKind_Last, StreamPos_Last, "StreamSize");
                if (!Material_StreamSize.empty())
                {
                    Fill(StreamKind_Last, StreamPos_Last, "Material_StreamSize", Material_StreamSize);
                    Clear(StreamKind_Last, StreamPos_Last, "StreamSize");
                }
                Ztring Material_FrameCount=Retrieve(StreamKind_Last, StreamPos_Last, "FrameCount");
                bool FillFrameCount=false;
                if (!Material_FrameCount.empty())
                {
                    FillFrameCount=true;
                    Fill(StreamKind_Last, StreamPos_Last, "Material_FrameCount", Material_FrameCount);
                    Clear(StreamKind_Last, StreamPos_Last, "FrameCount");
                }

                //Calculating new stream size
                int64u FrameCount;
                if (Temp->second.stts_Min && Temp->second.stts_Min==Temp->second.stts_Max)
                {
                    FrameCount=float64_int64s(((float64)Temp->second.tkhd_Duration)/TimeScale*Temp->second.mdhd_TimeScale/Temp->second.stts_Min);
                }
                else
                {
                    FrameCount=0;
                    int64u Ticks_Max=float64_int64s(((float64)Temp->second.tkhd_Duration)/TimeScale*Temp->second.mdhd_TimeScale);
                    int64u Ticks=0;
                    for (size_t stts_Pos=0; stts_Pos<Temp->second.stts.size(); stts_Pos++)
                    {
                        int64u Ticks_Complete=Temp->second.stts[stts_Pos].SampleCount*Temp->second.stts[stts_Pos].SampleDuration;
                        if (Ticks+Ticks_Complete>=Ticks_Max)
                        {
                            if (Temp->second.stts[stts_Pos].SampleDuration)
                                FrameCount+=float64_int64s(((float64)(Ticks_Max-Ticks))/Temp->second.stts[stts_Pos].SampleDuration);
                            break;
                        }
                        Ticks+=Ticks_Complete;
                        FrameCount+=Temp->second.stts[stts_Pos].SampleCount;
                    }
                }
                if (Temp->second.stsz_Total.empty())
                {
                    Fill(StreamKind_Last, StreamPos_Last, "StreamSize", FrameCount*Temp->second.stsz_Sample_Size*Temp->second.stsz_Sample_Multiplier);
                    if (FillFrameCount)
                        Fill(StreamKind_Last, StreamPos_Last, "FrameCount", FrameCount);
                }
                else if (FrameCount<=Temp->second.stsz_Total.size())
                {
                    int64u StreamSize=0;
                    for (size_t stsz_Pos=0; stsz_Pos<FrameCount; stsz_Pos++)
                        StreamSize+=Temp->second.stsz_Total[stsz_Pos];
                    Fill(StreamKind_Last, StreamPos_Last, "StreamSize", StreamSize);
                    if (FillFrameCount)
                        Fill(StreamKind_Last, StreamPos_Last, "FrameCount", FrameCount);
                }
            }
            else if (!(Duration_mdhd>Duration_tkhd_L && Duration_mdhd<Duration_tkhd_H))
            {
                //There is a difference between media/mdhd atom and track atom
                Fill(StreamKind_Last, StreamPos_Last, "mdhd_Duration", Duration_mdhd*1000, 0); 
            }
        }

        //Parser specific
        if (Temp->second.Parser)
        {
            if (Config_ParseSpeed<=1.0)
            {
                Fill(Temp->second.Parser);
                Temp->second.Parser->Open_Buffer_Unsynch();
            }

            //Finalizing and Merging
            Finish(Temp->second.Parser);
            if (StreamKind_Last==Stream_General)
            {
                //Special case for TimeCode without link
                for (std::map<int32u, stream>::iterator Target=Streams.begin(); Target!=Streams.end(); Target++)
                    if (Target->second.StreamKind!=Stream_General)
                        Merge(*Temp->second.Parser, Target->second.StreamKind, 0, Target->second.StreamPos);
            }
            else
            {
                //Hacks - Before
                Ztring FrameRate_Temp, FrameRate_Mode_Temp, Duration_Temp, Delay_Temp;
                if (StreamKind_Last==Stream_Video)
                {
                    if (Temp->second.Parser && Retrieve(Stream_Video, 0, Video_CodecID_Hint)==_T("DVCPRO HD"))
                    {
                        Temp->second.Parser->Clear(Stream_Video, 0, Video_FrameRate);
                        Temp->second.Parser->Clear(Stream_Video, 0, Video_Width);
                        Temp->second.Parser->Clear(Stream_Video, 0, Video_Height);
                        Temp->second.Parser->Clear(Stream_Video, 0, Video_DisplayAspectRatio);
                        Temp->second.Parser->Clear(Stream_Video, 0, Video_PixelAspectRatio);
                    }

                    FrameRate_Temp=Retrieve(Stream_Video, StreamPos_Last, Video_FrameRate);
                    FrameRate_Mode_Temp=Retrieve(Stream_Video, StreamPos_Last, Video_FrameRate_Mode);
                    Duration_Temp=Retrieve(Stream_Video, StreamPos_Last, Video_Duration);
                    Delay_Temp=Retrieve(Stream_Video, StreamPos_Last, Video_Delay);

                    //Special case: DV 1080i and MPEG-4 header is lying (saying this is 1920 pixel wide, but this is 1440 pixel wide)
                    if (Temp->second.Parser->Get(Stream_Video, 0, Video_Format)==_T("DV") && Retrieve(Stream_Video, StreamKind_Last, Video_Width)==_T("1080"))
                        Clear(Stream_Video, StreamKind_Last, Video_Width);
                }

                //Special case - Multiple sub-streams in a stream
                if ((Temp->second.Parser->Retrieve(Stream_General, 0, General_Format)==_T("ChannelGrouping") && Temp->second.Parser->Count_Get(Stream_Audio))
                 ||  Temp->second.Parser->Retrieve(Stream_General, 0, General_Format)==_T("Final Cut EIA-608")
                 ||  Temp->second.Parser->Retrieve(Stream_General, 0, General_Format)==_T("Final Cut CDP"))
                {
                    //Before
                    Clear(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_StreamSize));
                    if (StreamKind_Last==Stream_Audio)
                    {
                        Clear(Stream_Audio, StreamPos_Last, Audio_Format_Settings_Sign);
                    }
                    ZtringList StreamSave; StreamSave.Write((*File__Analyze::Stream)[StreamKind_Last][StreamPos_Last].Read());
                    ZtringListList StreamMoreSave; StreamMoreSave.Write((*Stream_More)[StreamKind_Last][StreamPos_Last].Read());

                    //Erasing former streams data
                    stream_t NewKind=StreamKind_Last;
                    size_t NewPos1;
                    Ztring ID;
                    if (Temp->second.Parser->Retrieve(Stream_General, 0, General_Format)==_T("ChannelGrouping"))
                    {
                        //Channel coupling, removing the 2 corresponding streams
                        NewPos1=(StreamPos_Last/2)*2;
                        size_t NewPos2=NewPos1+1;
                        ID=Retrieve(StreamKind_Last, NewPos1, General_ID)+_T(" / ")+Retrieve(StreamKind_Last, NewPos2, General_ID);

                        Stream_Erase(NewKind, NewPos2);
                        Stream_Erase(NewKind, NewPos1);

                        streams::iterator NextStream=Temp;
                        NextStream++;
                        size_t NewAudio_Count=Temp->second.Parser->Count_Get(Stream_Audio);
                        while (NextStream!=Streams.end())
                        {
                            if (NextStream->second.StreamKind==Stream_Audio)
                            {
                                NextStream->second.StreamPos-=2;
                                NextStream->second.StreamPos+=NewAudio_Count;
                            }
                            NextStream++;
                        }
                    }
                    else
                    {
                        //One channel
                        NewPos1=StreamPos_Last;
                        ID=Retrieve(StreamKind_Last, NewPos1, General_ID);
                        Stream_Erase(StreamKind_Last, StreamPos_Last);
                    }

                    //After
                    size_t New_Count=Temp->second.Parser->Count_Get(NewKind);
                    for (size_t StreamPos=0; StreamPos<New_Count; StreamPos++)
                    {
                        Stream_Prepare(NewKind, NewPos1+StreamPos);
                        Merge(*Temp->second.Parser, StreamKind_Last, StreamPos, StreamPos_Last);
                        Ztring Parser_ID=Retrieve(StreamKind_Last, StreamPos_Last, General_ID);
                        Fill(StreamKind_Last, StreamPos_Last, General_ID, ID+_T("-")+Parser_ID, true);
                        for (size_t Pos=0; Pos<StreamSave.size(); Pos++)
                            if (Retrieve(StreamKind_Last, StreamPos_Last, Pos).empty())
                                Fill(StreamKind_Last, StreamPos_Last, Pos, StreamSave[Pos]);
                        for (size_t Pos=0; Pos<StreamMoreSave.size(); Pos++)
                            Fill(StreamKind_Last, StreamPos_Last, StreamMoreSave(Pos, 0).To_Local().c_str(), StreamMoreSave(Pos, 1));
                    }
                }
                else
                {
                    //Temp->second.Parser->Clear(StreamKind_Last, StreamPos_Last, "Delay"); //DV TimeCode is removed
                    Merge(*Temp->second.Parser, StreamKind_Last, 0, StreamPos_Last);
                }

                //Hacks - After
                if (StreamKind_Last==Stream_Video)
                {
                    Fill(Stream_Video, StreamPos_Last, Video_Duration, Duration_Temp, true);
                    if (!FrameRate_Temp.empty() && FrameRate_Temp!=Retrieve(Stream_Video, StreamPos_Last, Video_FrameRate))
                        Fill(Stream_Video, StreamPos_Last, Video_FrameRate, FrameRate_Temp, true);
                    if (!FrameRate_Mode_Temp.empty() && FrameRate_Mode_Temp!=Retrieve(Stream_Video, StreamPos_Last, Video_FrameRate_Mode))
                        Fill(Stream_Video, StreamPos_Last, Video_FrameRate_Mode, FrameRate_Mode_Temp, true);

                    //Special case for TimeCode and DV multiple audio
                    if (!Delay_Temp.empty() && Delay_Temp!=Retrieve(Stream_Video, StreamPos_Last, Video_Delay))
                    {
                        for (size_t Pos=0; Pos<Count_Get(Stream_Audio); Pos++)
                            if (Retrieve(Stream_Audio, Pos, "MuxingMode_MoreInfo")==_T("Muxed in Video #1"))
                            {
                                //Fill(Stream_Audio, Pos, Audio_Delay_Original, Retrieve(Stream_Audio, Pos, Audio_Delay));
                                Fill(Stream_Audio, Pos, Audio_Delay, Retrieve(Stream_Video, StreamPos_Last, Video_Delay), true);
                                Fill(Stream_Audio, Pos, Audio_Delay_Settings, Retrieve(Stream_Video, StreamPos_Last, Video_Delay_Settings), true);
                                Fill(Stream_Audio, Pos, Audio_Delay_Source, Retrieve(Stream_Video, StreamPos_Last, Video_Delay_Source), true);
                                Fill(Stream_Audio, Pos, Audio_Delay_Original, Retrieve(Stream_Video, StreamPos_Last, Video_Delay_Original), true);
                                Fill(Stream_Audio, Pos, Audio_Delay_Original_Settings, Retrieve(Stream_Video, StreamPos_Last, Video_Delay_Original_Settings), true);
                                Fill(Stream_Audio, Pos, Audio_Delay_Original_Source, Retrieve(Stream_Video, StreamPos_Last, Video_Delay_Original_Source), true);
                            }
                        for (size_t Pos=0; Pos<Count_Get(Stream_Text); Pos++)
                            if (Retrieve(Stream_Text, Pos, "MuxingMode_MoreInfo")==_T("Muxed in Video #1"))
                            {
                                //Fill(Stream_Text, Pos, Text_Delay_Original, Retrieve(Stream_Text, Pos, Text_Delay));
                                Fill(Stream_Text, Pos, Text_Delay, Retrieve(Stream_Video, StreamPos_Last, Video_Delay), true);
                                Fill(Stream_Text, Pos, Text_Delay_Settings, Retrieve(Stream_Video, StreamPos_Last, Video_Delay_Settings), true);
                                Fill(Stream_Text, Pos, Text_Delay_Source, Retrieve(Stream_Video, StreamPos_Last, Video_Delay_Source), true);
                                Fill(Stream_Text, Pos, Text_Delay_Original, Retrieve(Stream_Video, StreamPos_Last, Video_Delay_Original), true);
                                Fill(Stream_Text, Pos, Text_Delay_Original_Settings, Retrieve(Stream_Video, StreamPos_Last, Video_Delay_Original_Settings), true);
                                Fill(Stream_Text, Pos, Text_Delay_Original_Source, Retrieve(Stream_Video, StreamPos_Last, Video_Delay_Original_Source), true);
                            }
                    }
                }

                //TimeCode specific
                if (StreamKind_Last==Stream_Video && Retrieve(Stream_Menu, StreamPos_Last, Menu_Format)==_T("TimeCode"))
                {
                    Clear(Stream_Menu, StreamPos_Last, "Duration");
                    Clear(Stream_Menu, StreamPos_Last, "StreamSize");
                }

                //Special case: AAC
                if (StreamKind_Last==Stream_Audio
                 && (Retrieve(Stream_Audio, StreamPos_Last, Audio_Format)==_T("AAC")
                  || Retrieve(Stream_Audio, StreamPos_Last, Audio_Format)==_T("MPEG Audio")
                  || Retrieve(Stream_Audio, StreamPos_Last, Audio_Format)==_T("Vorbis")))
                    Clear(Stream_Audio, StreamPos_Last, Audio_BitDepth); //Resolution is not valid for AAC / MPEG Audio / Vorbis

                //Special case: DV with Audio or/and Text in the video stream
                if (StreamKind_Last==Stream_Video && Temp->second.Parser && (Temp->second.Parser->Count_Get(Stream_Audio) || Temp->second.Parser->Count_Get(Stream_Text)))
                {
                    //Video and Audio are together
                    size_t Audio_Count=Temp->second.Parser->Count_Get(Stream_Audio);
                    for (size_t Audio_Pos=0; Audio_Pos<Audio_Count; Audio_Pos++)
                    {
                        Fill_Flush();
                        Stream_Prepare(Stream_Audio);
                        size_t Pos=Count_Get(Stream_Audio)-1;
                        Merge(*Temp->second.Parser, Stream_Audio, Audio_Pos, StreamPos_Last);
                        if (Retrieve(Stream_Audio, Pos, Audio_MuxingMode).empty())
                            Fill(Stream_Audio, Pos, Audio_MuxingMode, Retrieve(Stream_Video, Temp->second.StreamPos, Video_Format), true);
                        else
                            Fill(Stream_Audio, Pos, Audio_MuxingMode, Retrieve(Stream_Video, Temp->second.StreamPos, Video_Format)+_T(" / ")+Retrieve(Stream_Audio, Pos, Audio_MuxingMode), true);
                        Fill(Stream_Audio, Pos, Audio_MuxingMode_MoreInfo, _T("Muxed in Video #")+Ztring().From_Number(Temp->second.StreamPos+1));
                        Fill(Stream_Audio, Pos, Audio_Duration, Retrieve(Stream_Video, Temp->second.StreamPos, Video_Duration));
                        Fill(Stream_Audio, Pos, Audio_StreamSize, 0, 10, true); //Included in the DV stream size
                        Ztring ID=Retrieve(Stream_Audio, Pos, Audio_ID);
                        Fill(Stream_Audio, Pos, Audio_ID, Retrieve(Stream_Video, Temp->second.StreamPos, Video_ID)+_T("-")+ID, true);
                        Fill(Stream_Audio, Pos, "Source", Retrieve(Stream_Video, Temp->second.StreamPos, "Source"));
                        Fill(Stream_Audio, Pos, "Source_Info", Retrieve(Stream_Video, Temp->second.StreamPos, "Source_Info"));
                    }

                    //Video and Text are together
                    size_t Text_Count=Temp->second.Parser->Count_Get(Stream_Text);
                    for (size_t Text_Pos=0; Text_Pos<Text_Count; Text_Pos++)
                    {
                        Fill_Flush();
                        Stream_Prepare(Stream_Text);
                        size_t Pos=Count_Get(Stream_Text)-1;
                        Merge(*Temp->second.Parser, Stream_Text, Text_Pos, StreamPos_Last);
                        if (Retrieve(Stream_Text, Pos, Text_MuxingMode).empty())
                            Fill(Stream_Text, Pos, Text_MuxingMode, Retrieve(Stream_Video, Temp->second.StreamPos, Video_Format), true);
                        else
                            Fill(Stream_Text, Pos, Text_MuxingMode, Retrieve(Stream_Video, Temp->second.StreamPos, Video_Format)+_T(" / ")+Retrieve(Stream_Text, Pos, Text_MuxingMode), true);
                        Fill(Stream_Text, Pos, Text_MuxingMode_MoreInfo, _T("Muxed in Video #")+Ztring().From_Number(Temp->second.StreamPos+1));
                        Fill(Stream_Text, Pos, Text_Duration, Retrieve(Stream_Video, Temp->second.StreamPos, Video_Duration));
                        Fill(Stream_Text, Pos, Text_StreamSize, 0, 10, true); //Included in the DV stream size
                        Ztring ID=Retrieve(Stream_Text, Pos, Text_ID);
                        Fill(Stream_Text, Pos, Text_ID, Retrieve(Stream_Video, Temp->second.StreamPos, Video_ID)+_T("-")+ID, true);
                        Fill(Stream_Text, Pos, "Source", Retrieve(Stream_Video, Temp->second.StreamPos, "Source"));
                        Fill(Stream_Text, Pos, "Source_Info", Retrieve(Stream_Video, Temp->second.StreamPos, "Source_Info"));
                    }

                    StreamKind_Last=Temp->second.StreamKind;
                    StreamPos_Last=Temp->second.StreamPos;
                }
            }
        }

        //External file name specific
        if (Temp->second.MI && Temp->second.MI->Info)
        {
            //Preparing
            StreamKind_Last=Temp->second.StreamKind;
            StreamPos_Last=Temp->second.StreamPos;

            //Hacks - Before
            Ztring CodecID=Retrieve(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_CodecID));
            Ztring Source=Retrieve(StreamKind_Last, StreamPos_Last, "Source");
            Ztring Source_Info=Retrieve(StreamKind_Last, StreamPos_Last, "Source_Info");

            Merge(*Temp->second.MI->Info, Temp->second.StreamKind, 0, Temp->second.StreamPos);
            File_Size_Total+=Ztring(Temp->second.MI->Get(Stream_General, 0, General_FileSize)).To_int64u();

            //Hacks - After
            if (CodecID!=Retrieve(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_CodecID)))
            {
                if (!CodecID.empty())
                    CodecID+=_T(" / ");
                CodecID+=Retrieve(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_CodecID));
                Fill(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_CodecID), CodecID, true);
            }
            if (Source!=Retrieve(StreamKind_Last, StreamPos_Last, "Source"))
            {
                Ztring Source_Original=Retrieve(StreamKind_Last, StreamPos_Last, "Source");
                Ztring Source_Original_Info=Retrieve(StreamKind_Last, StreamPos_Last, "Source_Info");
                Fill(StreamKind_Last, StreamPos_Last, "Source", Source, true);
                Fill(StreamKind_Last, StreamPos_Last, "Source_Info", Source_Info, true);
                Fill(StreamKind_Last, StreamPos_Last, "Source_Original", Source_Original, true);
                Fill(StreamKind_Last, StreamPos_Last, "Source_Original_Info", Source_Original_Info, true);
            }

            //Muxing Mode
            Fill(StreamKind_Last, StreamPos_Last, "MuxingMode", Temp->second.MI->Get(Stream_General, 0, General_Format));

            //Special case: DV with Audio or/and Text in the video stream
            if (StreamKind_Last==Stream_Video && Temp->second.MI->Info && (Temp->second.MI->Info->Count_Get(Stream_Audio) || Temp->second.MI->Info->Count_Get(Stream_Text)))
            {
                //Video and Audio are together
                size_t Audio_Count=Temp->second.MI->Info->Count_Get(Stream_Audio);
                for (size_t Audio_Pos=0; Audio_Pos<Audio_Count; Audio_Pos++)
                {
                    Fill_Flush();
                    Stream_Prepare(Stream_Audio);
                    size_t Pos=Count_Get(Stream_Audio)-1;
                    Merge(*Temp->second.MI->Info, Stream_Audio, Audio_Pos, StreamPos_Last);
                    if (Retrieve(Stream_Audio, Pos, Audio_MuxingMode).empty())
                        Fill(Stream_Audio, Pos, Audio_MuxingMode, Retrieve(Stream_Video, Temp->second.StreamPos, Video_Format), true);
                    else
                        Fill(Stream_Audio, Pos, Audio_MuxingMode, Retrieve(Stream_Video, Temp->second.StreamPos, Video_Format)+_T(" / ")+Retrieve(Stream_Audio, Pos, Audio_MuxingMode), true);
                    Fill(Stream_Audio, Pos, Audio_MuxingMode_MoreInfo, _T("Muxed in Video #")+Ztring().From_Number(Temp->second.StreamPos+1));
                    Fill(Stream_Audio, Pos, Audio_Duration, Retrieve(Stream_Video, Temp->second.StreamPos, Video_Duration), true);
                    Fill(Stream_Audio, Pos, Audio_StreamSize, 0, 10, true); //Included in the DV stream size
                    Ztring ID=Retrieve(Stream_Audio, Pos, Audio_ID);
                    Fill(Stream_Audio, Pos, Audio_ID, Retrieve(Stream_Video, Temp->second.StreamPos, Video_ID)+_T("-")+ID, true);
                    Fill(Stream_Audio, Pos, "Source", Retrieve(Stream_Video, Temp->second.StreamPos, "Source"));
                    Fill(Stream_Audio, Pos, "Source_Info", Retrieve(Stream_Video, Temp->second.StreamPos, "Source_Info"));
                }

                //Video and Text are together
                size_t Text_Count=Temp->second.MI->Info->Count_Get(Stream_Text);
                for (size_t Text_Pos=0; Text_Pos<Text_Count; Text_Pos++)
                {
                    Fill_Flush();
                    Stream_Prepare(Stream_Text);
                    size_t Pos=Count_Get(Stream_Text)-1;
                    Merge(*Temp->second.MI->Info, Stream_Text, Text_Pos, StreamPos_Last);
                    if (Retrieve(Stream_Text, Pos, Text_MuxingMode).empty())
                        Fill(Stream_Text, Pos, Text_MuxingMode, Retrieve(Stream_Video, Temp->second.StreamPos, Video_Format), true);
                    else
                        Fill(Stream_Text, Pos, Text_MuxingMode, Retrieve(Stream_Video, Temp->second.StreamPos, Video_Format)+_T(" / ")+Retrieve(Stream_Text, Pos, Text_MuxingMode), true);
                    Fill(Stream_Text, Pos, Text_MuxingMode_MoreInfo, _T("Muxed in Video #")+Ztring().From_Number(Temp->second.StreamPos+1));
                    Fill(Stream_Text, Pos, Text_Duration, Retrieve(Stream_Video, Temp->second.StreamPos, Video_Duration));
                    Fill(Stream_Text, Pos, Text_StreamSize, 0, 10, true); //Included in the DV stream size
                    Ztring ID=Retrieve(Stream_Text, Pos, Text_ID);
                    Fill(Stream_Text, Pos, Text_ID, Retrieve(Stream_Video, Temp->second.StreamPos, Video_ID)+_T("-")+ID, true);
                    Fill(Stream_Text, Pos, "Source", Retrieve(Stream_Video, Temp->second.StreamPos, "Source"));
                    Fill(Stream_Text, Pos, "Source_Info", Retrieve(Stream_Video, Temp->second.StreamPos, "Source_Info"));
                }
            }
        }

        //Aperture size
        if (Temp->second.CleanAperture_Width)
        {
            Ztring CleanAperture_Width=Ztring().From_Number(Temp->second.CleanAperture_Width, 0);
            Ztring CleanAperture_Height=Ztring().From_Number(Temp->second.CleanAperture_Height, 0);
            if (CleanAperture_Width!=Retrieve(Stream_Video, StreamPos_Last, Video_Width))
            {
                Fill(Stream_Video, StreamPos_Last, Video_Width_Original, Retrieve(Stream_Video, StreamPos_Last, Video_Width), true);
                Fill(Stream_Video, StreamPos_Last, Video_Width, Temp->second.CleanAperture_Width, 0, true);
            }
            if (CleanAperture_Height!=Retrieve(Stream_Video, StreamPos_Last, Video_Height))
            {
                Fill(Stream_Video, StreamPos_Last, Video_Height_Original, Retrieve(Stream_Video, StreamPos_Last, Video_Height), true);
                Fill(Stream_Video, StreamPos_Last, Video_Height, Temp->second.CleanAperture_Height, 0, true);
            }
            if (Temp->second.CleanAperture_PixelAspectRatio)
            {
                Clear(Stream_Video, StreamPos_Last, Video_DisplayAspectRatio);
                Clear(Stream_Video, StreamPos_Last, Video_PixelAspectRatio);
                Fill(Stream_Video, StreamPos_Last, Video_PixelAspectRatio, Temp->second.CleanAperture_PixelAspectRatio, 3, true);
                if (Retrieve(Stream_Video, StreamPos_Last, Video_PixelAspectRatio)==Retrieve(Stream_Video, StreamPos_Last, Video_PixelAspectRatio_Original))
                    Clear(Stream_Video, StreamPos_Last, Video_PixelAspectRatio_Original);
                if (Retrieve(Stream_Video, StreamPos_Last, Video_DisplayAspectRatio)==Retrieve(Stream_Video, StreamPos_Last, Video_DisplayAspectRatio_Original))
                {
                    Clear(Stream_Video, StreamPos_Last, Video_DisplayAspectRatio_Original);
                    Clear(Stream_Video, StreamPos_Last, Video_DisplayAspectRatio_Original_String);
                }
            }
        }

        Temp++;
    }
    if (Vendor!=0x00000000 && Vendor!=0xFFFFFFFF)
    {
        Ztring VendorS=Mpeg4_Encoded_Library(Vendor);
        if (!Vendor_Version.empty())
        {
            VendorS+=_T(' ');
            VendorS+=Vendor_Version;
        }
        Fill(Stream_General, 0, General_Encoded_Library, VendorS);
        Fill(Stream_General, 0, General_Encoded_Library_Name, Mpeg4_Encoded_Library(Vendor));
        Fill(Stream_General, 0, General_Encoded_Library_Version, Vendor_Version);
    }

    if (File_Size_Total!=File_Size)
        Fill(Stream_General, 0, General_FileSize, File_Size_Total, 10, true);
    if (Count_Get(Stream_Video)==0 && Count_Get(Stream_Image)==0 && Count_Get(Stream_Audio)>0)
        Fill(Stream_General, 0, General_InternetMediaType, "audio/mp4", Unlimited, true, true);

    //Parsing reference files
    for (streams::iterator Stream=Streams.begin(); Stream!=Streams.end(); Stream++)
        if (!Stream->second.File_Name.empty())
        {
            if (ReferenceFiles==NULL)
                ReferenceFiles=new File__ReferenceFilesHelper(this, Config);

            File__ReferenceFilesHelper::reference Reference;
            Reference.FileNames.push_back(Stream->second.File_Name);
            Reference.StreamKind=Stream->second.StreamKind;
            Reference.StreamPos=Stream->second.StreamPos;
            Reference.StreamID=Retrieve(Stream->second.StreamKind, Stream->second.StreamPos, General_ID);
            if (Stream->second.StreamKind==Stream_Video)
                Reference.FrameRate=Retrieve(Stream_Video, Stream->second.StreamPos, Video_FrameRate).To_float64();
            ReferenceFiles->References.push_back(Reference);
        }

    if (ReferenceFiles)
    {
        ReferenceFiles->ParseReferences();
        #if MEDIAINFO_NEXTPACKET
            if (Config->NextPacket_Get() && ReferenceFiles && !ReferenceFiles->References.empty())
            {
                ReferenceFiles_IsParsing=true;
                return;
            }
        #endif //MEDIAINFO_NEXTPACKET
    }

    //Commercial names
    Streams_Finish_CommercialNames();
}

//---------------------------------------------------------------------------
void File_Mpeg4::Streams_Finish_CommercialNames()
{
    if (Count_Get(Stream_Video)==1)
    {
        Streams_Finish_StreamOnly();
        if (Retrieve(Stream_Video, 0, Video_Format)==_T("DV") && Retrieve(Stream_Video, 0, Video_Format_Commercial)==_T("DVCPRO HD"))
        {
            int32u BitRate=Retrieve(Stream_Video, 0, Video_BitRate).To_int32u();
            int32u BitRate_Max=Retrieve(Stream_Video, 0, Video_BitRate_Maximum).To_int32u();

            if (BitRate_Max && BitRate>=BitRate_Max)
            {
                Clear(Stream_Video, 0, Video_BitRate_Maximum);
                Fill(Stream_Video, 0, Video_BitRate, BitRate_Max, 10, true);
                Fill(Stream_Video, 0, Video_BitRate_Mode, "CBR", Unlimited, true, true);
            }
        }
             if (!Retrieve(Stream_Video, 0, Video_Format_Commercial_IfAny).empty())
        {
            Fill(Stream_General, 0, General_Format_Commercial_IfAny, Retrieve(Stream_Video, 0, Video_Format_Commercial_IfAny));
            Fill(Stream_General, 0, General_Format_Commercial, Retrieve(Stream_General, 0, General_Format)+_T(' ')+Retrieve(Stream_Video, 0, Video_Format_Commercial_IfAny));
        }
        else if (Retrieve(Stream_Video, 0, Video_Format)==_T("MPEG Video") && Retrieve(Stream_Video, 0, Video_Format_Settings_GOP)!=_T("N=1") && Retrieve(Stream_Video, 0, Video_Colorimetry)==_T("4:2:0") && Retrieve(Stream_Video, 0, Video_BitRate)==_T("18000000") || Retrieve(Stream_Video, 0, Video_BitRate_Nominal)==_T("18000000") || Retrieve(Stream_Video, 0, Video_BitRate_Maximum)==_T("18000000"))
        {
            Fill(Stream_General, 0, General_Format_Commercial_IfAny, "XDCAM EX 18");
            Fill(Stream_Video, 0, Video_Format_Commercial_IfAny, "XDCAM EX 18");
        }
        else if (Retrieve(Stream_Video, 0, Video_Format)==_T("MPEG Video") && Retrieve(Stream_Video, 0, Video_Format_Settings_GOP)!=_T("N=1") && Retrieve(Stream_Video, 0, Video_Colorimetry)==_T("4:2:0") && Retrieve(Stream_Video, 0, Video_BitRate)==_T("25000000") || Retrieve(Stream_Video, 0, Video_BitRate_Nominal)==_T("25000000") || Retrieve(Stream_Video, 0, Video_BitRate_Maximum)==_T("25000000"))
        {
            Fill(Stream_General, 0, General_Format_Commercial_IfAny, "XDCAM EX 25");
            Fill(Stream_Video, 0, Video_Format_Commercial_IfAny, "XDCAM EX 25");
        }
        else if (Retrieve(Stream_Video, 0, Video_Format)==_T("MPEG Video") && Retrieve(Stream_Video, 0, Video_Format_Settings_GOP)!=_T("N=1") && Retrieve(Stream_Video, 0, Video_Colorimetry)==_T("4:2:0") && Retrieve(Stream_Video, 0, Video_BitRate)==_T("35000000") || Retrieve(Stream_Video, 0, Video_BitRate_Nominal)==_T("35000000") || Retrieve(Stream_Video, 0, Video_BitRate_Maximum)==_T("35000000"))
        {
            Fill(Stream_General, 0, General_Format_Commercial_IfAny, "XDCAM EX 35");
            Fill(Stream_Video, 0, Video_Format_Commercial_IfAny, "XDCAM EX 35");
        }
        else if (Retrieve(Stream_Video, 0, Video_Format)==_T("MPEG Video") && Retrieve(Stream_Video, 0, Video_Format_Settings_GOP)!=_T("N=1") && Retrieve(Stream_Video, 0, Video_Colorimetry)==_T("4:2:2") && (Retrieve(Stream_Video, 0, Video_BitRate)==_T("50000000") || Retrieve(Stream_Video, 0, Video_BitRate_Nominal)==_T("50000000") || Retrieve(Stream_Video, 0, Video_BitRate_Maximum)==_T("50000000")))
        {
            Fill(Stream_General, 0, General_Format_Commercial_IfAny, "XDCAM EX422");
            Fill(Stream_Video, 0, Video_Format_Commercial_IfAny, "XDCAM EX422");
        }
    }
}

//***************************************************************************
// Buffer - Global
//***************************************************************************

//---------------------------------------------------------------------------
void File_Mpeg4::Read_Buffer_Unsynched()
{
    mdat_Pos_Temp=mdat_Pos.begin();
    while (mdat_Pos_Temp!=mdat_Pos.end() && mdat_Pos_Temp->first<File_GoTo)
        mdat_Pos_Temp++;
    if (mdat_Pos_Temp!=mdat_Pos.end() && mdat_Pos_Temp->first>File_GoTo)
        mdat_Pos_Temp--; //Previous frame
    if (mdat_Pos_Temp!=mdat_Pos.end())
        IsParsing_mdat=true;

    for (std::map<int32u, stream>::iterator Stream=Streams.begin(); Stream!=Streams.end(); Stream++)
    {
        if (Stream->second.Parser)
            Stream->second.Parser->Open_Buffer_Unsynch();
        #if MEDIAINFO_SEEK
            for (size_t stco_Pos=0; stco_Pos<Stream->second.stco.size(); stco_Pos++)
                if (Stream->second.stco[stco_Pos]>=File_GoTo)
                {
                    //Searching the corresponding frame position
                    std::vector<stream::stsc_struct>::iterator Stsc=Stream->second.stsc.begin();
                    int64u SamplePos=0;
                    for (; Stsc!=Stream->second.stsc.end();  Stsc++)
                    {
                        std::vector<stream::stsc_struct>::iterator Stsc_Next=Stsc; Stsc_Next++;
                        if (Stsc_Next!=Stream->second.stsc.end() && stco_Pos+1>=Stsc_Next->FirstChunk)
                        {
                            int64u CountOfSamples=(Stsc_Next->FirstChunk-Stsc->FirstChunk)*Stsc->SamplesPerChunk;
                            SamplePos+=CountOfSamples;
                        }
                        else
                        {
                            int64u CountOfSamples=(stco_Pos+1-Stsc->FirstChunk)*Stsc->SamplesPerChunk;
                            SamplePos+=CountOfSamples;

                            Stream->second.stts_FramePos=SamplePos;

                            //Searching the corresponding duration block position
                            for (stream::stts_durations::iterator Stts_Duration=Stream->second.stts_Durations.begin(); Stts_Duration!=Stream->second.stts_Durations.end(); Stts_Duration++)
                                if (SamplePos>=Stts_Duration->Pos_Begin && SamplePos<Stts_Duration->Pos_End)
                                {
                                    Stream->second.stts_Durations_Pos=Stts_Duration-Stream->second.stts_Durations.begin();
                                    break;
                                }

                            break;
                        }
                    }

                    break;
                }
        #endif //MEDIAINFO_SEEK
    }
}

//---------------------------------------------------------------------------
#if MEDIAINFO_SEEK
size_t File_Mpeg4::Read_Buffer_Seek (size_t Method, int64u Value, int64u ID)
{
    if (ReferenceFiles)
        return ReferenceFiles->Read_Buffer_Seek(Method, Value, ID);
                        
    //Parsing
    switch (Method)
    {
        case 0  :   
                    if (Value==0)
                        return Read_Buffer_Seek(3, 0, ID);
                    return (size_t)-1; //Not supported
        case 1  :   if (Value==0)
                        return Read_Buffer_Seek(3, 0, ID);
                    return (size_t)-1; //Not supported
        case 2  :   //Timestamp
                    {
                        //Looking for video stream
                        std::map<int32u, stream>::iterator Stream;
                        for (Stream=Streams.begin(); Stream!=Streams.end(); Stream++)
                            if (Stream->second.StreamKind==Stream_Video)
                                break;
                        if (Stream==Streams.end())
                            for (Stream=Streams.begin(); Stream!=Streams.end(); Stream++)
                                if (Stream->second.StreamKind==Stream_Video)
                                    break;
                        if (Stream==Streams.end())
                            return 0; //Not supported

                        //Searching the corresponding frame
                        Value=Value*Stream->second.mdhd_TimeScale/1000000000; //Transformed in mpeg4 ticks
                        stream::stts_durations::iterator stts_Duration=Stream->second.stts_Durations.begin();
                        for (; stts_Duration!=Stream->second.stts_Durations.end(); stts_Duration++)
                        {
                            if (Value>=stts_Duration->DTS_Begin && Value<stts_Duration->DTS_End)
                            {
                                Value=stts_Duration->DTS_Begin+(Value-stts_Duration->Pos_Begin)/stts_Duration->SampleDuration;
                                break;
                            }
                        }
                        if (stts_Duration==Stream->second.stts_Durations.end())
                            return 2; //Invalid value
                    }
                    //No break;
        case 3  :   //FrameNumber
                    {
                        //Looking for video stream
                        std::map<int32u, stream>::iterator Stream;
                        for (Stream=Streams.begin(); Stream!=Streams.end(); Stream++)
                            if (Stream->second.StreamKind==Stream_Video)
                                break;
                        if (Stream==Streams.end())
                            for (Stream=Streams.begin(); Stream!=Streams.end(); Stream++)
                                if (Stream->second.StreamKind==Stream_Video)
                                    break;
                        if (Stream==Streams.end())
                            return 0; //Not supported

                        //Searching the I-Frame
                        if (!Stream->second.stss.empty())
                        {
                            for (size_t Pos=0; Pos<Stream->second.stss.size(); Pos++)
                                if (Value<=Stream->second.stss[Pos])
                                {
                                    if (Pos && Value<Stream->second.stss[Pos])
                                        Value=Stream->second.stss[Pos-1];
                                    break;
                                }
                        }

                        //Searching the corresponding stco
                        std::vector<stream::stsc_struct>::iterator Stsc=Stream->second.stsc.begin();
                        int64u SamplePos=0;
                        for (; Stsc!=Stream->second.stsc.end();  Stsc++)
                        {
                            std::vector<stream::stsc_struct>::iterator Stsc_Next=Stsc; Stsc_Next++;
                            int64u CountOfSamples=((Stsc_Next==Stream->second.stsc.end()?Stream->second.stco.size():Stsc_Next->FirstChunk)-Stsc->FirstChunk)*Stsc->SamplesPerChunk;
                            if (Stsc_Next!=Stream->second.stsc.end() && Value>=SamplePos+CountOfSamples)
                                SamplePos+=CountOfSamples;
                            else
                            {
                                int64u CountOfChunks=(Value-SamplePos)/Stsc->SamplesPerChunk;
                                size_t stco_Pos=(size_t)(Stsc->FirstChunk-1+CountOfChunks); //-1 because first chunk is number 1
                                int64u Offset=Stream->second.stco[stco_Pos];

                                GoTo(Offset);
                                Open_Buffer_Unsynch();
                                return 1;
                                break;
                            }
                        }

                        return 2; //Invalid value
                    }
        default :   return 0;
    }
}
#endif //MEDIAINFO_SEEK

//***************************************************************************
// Buffer
//***************************************************************************

//---------------------------------------------------------------------------
bool File_Mpeg4::Header_Begin()
{
    #if MEDIAINFO_DEMUX
        //Handling of multiple frames in one block
        if (IsParsing_mdat && Config->Demux_Unpacketize_Get())
        {
            Open_Buffer_Continue(Streams[(int32u)Element_Code].Parser, Buffer+Buffer_Offset, 0);
            if (Config->Demux_EventWasSent)
                return false;
        }
    #endif //MEDIAINFO_DEMUX

    if (IsParsing_mdat && Element_Level==0)
        Element_Begin();

    return true;
}

//---------------------------------------------------------------------------
void File_Mpeg4::Header_Parse()
{
    //mdat
    if (IsParsing_mdat)
    {
        //Positionning
        if (File_Offset+Buffer_Offset<mdat_Pos_Temp->first)
        {
            if (File_Offset+Buffer_Size>mdat_Pos_Temp->first)
            {
                Buffer_Offset=(size_t)(File_Offset+Buffer_Size-mdat_Pos_Temp->first);
            }
            else
            {
                Buffer_Offset=Buffer_Size;
                return;
            }
        }

        //Filling
        Header_Fill_Code(mdat_Pos_Temp->second.StreamID, Ztring::ToZtring(mdat_Pos_Temp->second.StreamID));
        Header_Fill_Size(mdat_Pos_Temp->second.Size);
        if (Buffer_Offset+mdat_Pos_Temp->second.Size<=Buffer_Size)
            mdat_Pos_Temp++; //Only if we will not need it later (in case of partial data, this function will be called again for the same chunk)
        else
            Element_WaitForMoreData();
        return;
    }

    //Parsing
    int64u Size;
    int32u Size_32, Name;
    Get_B4 (Size_32,                                            "Size");
    if (Size_32==0 && (Element_Size==4 || Element_Size==8))
    {
        //Filling
        Header_Fill_Code(0, "Junk");
        Header_Fill_Size(4);
        return;
    }
    Size=Size_32;
    Get_C4 (Name,                                               "Name");
    if (Size<8)
    {
        //Special case: until the end of the atom
            if (Size==0)
        {
            Size=File_Size-(File_Offset+Buffer_Offset);
            if (Status[IsAccepted] && Element_Level==2 && Name==0x00000000) //First real level (Level 1 is atom, level 2 is header block)
            {
                Element_Offset=0;
                Name=Elements::mdat;
            }
        }
        //Special case: Big files, size is 64-bit
        else if (Size==1)
        {
            //Reading Extended size
            Get_B8 (Size,                                       "Size (Extended)");
        }
        //Not in specs!
        else
        {
            Size=File_Size-(File_Offset+Buffer_Offset);
        }
    }

    //Specific case: file begin with "free" atom
    if (!Status[IsAccepted]
     && (Name==Elements::free
      || Name==Elements::skip
      || Name==Elements::wide))
    {
        Accept("MPEG-4");
        Fill(Stream_General, 0, General_Format, "QuickTime");
    }

    //Filling
    Header_Fill_Code(Name, Ztring().From_CC4(Name));
    Header_Fill_Size(Size);
}

//---------------------------------------------------------------------------
bool File_Mpeg4::BookMark_Needed()
{
    if (IsSecondPass || !mdat_MustParse || !mdat_Pos.empty())
        return false;

    IsSecondPass=true;
    return true;
}

//---------------------------------------------------------------------------
//Get language string from 2CC
Ztring File_Mpeg4::Language_Get(int16u Language)
{
    if (Language==0x7FFF || Language==0xFFFF)
        return Ztring();

    if (Language<0x100)
        return Mpeg4_Language_Apple(Language);

    Ztring ToReturn;
    ToReturn.append(1, (Char)((Language>>10&0x1F)+0x60));
    ToReturn.append(1, (Char)((Language>> 5&0x1F)+0x60));
    ToReturn.append(1, (Char)((Language>> 0&0x1F)+0x60));
    return ToReturn;
}

//---------------------------------------------------------------------------
//Get Metadata definition from 4CC
File_Mpeg4::method File_Mpeg4::Metadata_Get(std::string &Parameter, int64u Meta)
{
    File_Mpeg4::method Method;
    switch (Meta)
    {
        //http://atomicparsley.sourceforge.net/mpeg-4files.html
        case Elements::moov_meta___ART : Parameter="Performer"; Method=Method_String; break;
        case Elements::moov_meta___alb : Parameter="Album"; Method=Method_String; break;
        case Elements::moov_meta___aut : Parameter="Performer"; Method=Method_String; break;
        case Elements::moov_meta___cmt : Parameter="Comment"; Method=Method_String; break;
        case Elements::moov_meta___cpy : Parameter="Copyright"; Method=Method_String; break;
        case Elements::moov_meta___day : Parameter="Recorded_Date"; Method=Method_String; break;
        case Elements::moov_meta___des : Parameter="Title/More"; Method=Method_String; break;
        case Elements::moov_meta___dir : Parameter="Director"; Method=Method_String; break;
        case Elements::moov_meta___dis : Parameter="TermsOfUse"; Method=Method_String; break;
        case Elements::moov_meta___edl : Parameter="Tagged_Date"; Method=Method_String; break;
        case Elements::moov_meta___enc : Parameter="Encoded_Application"; Method=Method_String; break;
        case Elements::moov_meta___fmt : Parameter="Origin"; Method=Method_String; break;
        case Elements::moov_meta___gen : Parameter="Genre"; Method=Method_String; break;
        case Elements::moov_meta___grp : Parameter="Grouping"; Method=Method_String; break;
        case Elements::moov_meta___hos : Parameter="HostComputer"; Method=Method_String; break;
        case Elements::moov_meta___inf : Parameter="Title/More"; Method=Method_String; break;
        case Elements::moov_meta___key : Parameter="Keywords"; Method=Method_String; break;
        case Elements::moov_meta___mak : Parameter="Make"; Method=Method_String; break;
        case Elements::moov_meta___mod : Parameter="Model"; Method=Method_String; break;
        case Elements::moov_meta___nam : Parameter="Title"; Method=Method_String3; break;
        case Elements::moov_meta___prd : Parameter="Producer"; Method=Method_String; break;
        case Elements::moov_meta___PRD : Parameter="Product"; Method=Method_String; break;
        case Elements::moov_meta___prf : Parameter="Performer"; Method=Method_String; break;
        case Elements::moov_meta___req : Parameter="Comment"; Method=Method_String; break;
        case Elements::moov_meta___src : Parameter="DistribtedBy"; Method=Method_String; break;
        case Elements::moov_meta___swr : Parameter="Encoded_Application"; Method=Method_String; break;
        case Elements::moov_meta___too : Parameter="Encoded_Application"; Method=Method_String; break;
        case Elements::moov_meta___wrn : Parameter="Warning"; Method=Method_String; break;
        case Elements::moov_meta___wrt : Parameter="Composer"; Method=Method_String; break;
        case Elements::moov_meta__auth : Parameter="Performer"; Method=Method_String2; break;
        case Elements::moov_meta__albm : Parameter="Album"; Method=Method_String2; break; //Has a optional track number after the NULL byte
        case Elements::moov_meta__aART : Parameter="Album/Performer"; Method=Method_String2; break;
        case Elements::moov_meta__cpil : Parameter="Compilation"; Method=Method_Binary; break;
        case Elements::moov_meta__cprt : Parameter="Copyright"; Method=Method_String2; break;
        case Elements::moov_meta__disk : Parameter="Part"; Method=Method_Binary; break;
        case Elements::moov_meta__dscp : Parameter="Title/More"; Method=Method_String2; break;
        case Elements::moov_meta__gnre : Parameter="Genre"; Method=Method_String2; break;
        case Elements::moov_meta__name : Parameter="Title"; Method=Method_String; break;
        case Elements::moov_meta__perf : Parameter="Performer"; Method=Method_String2; break;
        case Elements::moov_meta__pgap : Parameter.clear(); Method=Method_None; break;
        case Elements::moov_meta__titl : Parameter="Title"; Method=Method_String2; break;
        case Elements::moov_meta__tool : Parameter="Encoded_Application"; Method=Method_String3; break;
        case Elements::moov_meta__trkn : Parameter="Track"; Method=Method_Binary; break;
        case Elements::moov_meta__year : Parameter="Recorded_Date"; Method=Method_String2; break;
        case Elements::moov_meta__tmpo : Parameter="BPM"; Method=Method_Binary; break;
        default :
            {
                Parameter.clear();
                Parameter.append(1, (char)((Meta&0xFF000000)>>24));
                Parameter.append(1, (char)((Meta&0x00FF0000)>>16));
                Parameter.append(1, (char)((Meta&0x0000FF00)>> 8));
                Parameter.append(1, (char)((Meta&0x000000FF)>> 0));
                Method=Method_String;
            }
    }

    Ztring Value=Ztring().From_CC4((int32u)Meta);
    if (MediaInfoLib::Config.CustomMapping_IsPresent(_T("MP4"), Value))
        Parameter=MediaInfoLib::Config.CustomMapping_Get(_T("MP4"), Value).To_Local();

    return Method;
}

//---------------------------------------------------------------------------
//Get Metadata definition from string
File_Mpeg4::method File_Mpeg4::Metadata_Get(std::string &Parameter, const std::string &Meta)
{
         if (Meta=="com.apple.quicktime.copyright") Parameter="Copyright";
    else if (Meta=="com.apple.quicktime.displayname") Parameter="Title";
    else if (Meta=="DATE") Parameter="Encoded_Date";
    else if (Meta=="iTunNORM") Parameter="";
    else if (Meta=="iTunes_CDDB_IDs") Parameter="";
    else if (Meta=="iTunSMPB") Parameter="";
    else if (Meta=="PERFORMER") Parameter="Performer";
    else if (Meta=="PUBLISHER") Parameter="Publisher";
    else Parameter=Meta;
    return Method_String;
}

//---------------------------------------------------------------------------
void File_Mpeg4::Descriptors()
{
    //Preparing
    File_Mpeg4_Descriptors MI;
    MI.KindOfStream=StreamKind_Last;
    MI.Parser_DoNotFreeIt=true;
    Open_Buffer_Init(&MI);

    //Parsing
    Open_Buffer_Continue(&MI);

    //Filling
    Finish(&MI);
    Merge(MI, StreamKind_Last, 0, StreamPos_Last);

    //Special case: AAC
    if (StreamKind_Last==Stream_Audio
     && (Retrieve(Stream_Audio, StreamPos_Last, Audio_Format)==_T("AAC")
      || Retrieve(Stream_Audio, StreamPos_Last, Audio_Format)==_T("MPEG Audio")
      || Retrieve(Stream_Audio, StreamPos_Last, Audio_Format)==_T("Vorbis")))
        Clear(Stream_Audio, StreamPos_Last, Audio_BitDepth); //Resolution is not valid for AAC / MPEG Audio / Vorbis

    //Parser from Descriptor
    if (MI.Parser)
    {
        if (Streams[moov_trak_tkhd_TrackID].Parser)
            delete Streams[moov_trak_tkhd_TrackID].Parser; //Streams[moov_trak_tkhd_TrackID].Parser=NULL
        Streams[moov_trak_tkhd_TrackID].Parser=MI.Parser;
        mdat_MustParse=true;
    }
}

//***************************************************************************
// C++
//***************************************************************************

} //NameSpace

#endif //MEDIAINFO_MPEG4_YES

