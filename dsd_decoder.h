///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 Edouard Griffiths, F4EXB.                                  //
//                                                                               //
// This program is free software; you can redistribute it and/or modify          //
// it under the terms of the GNU General Public License as published by          //
// the Free Software Foundation as version 3 of the License, or                  //
//                                                                               //
// This program is distributed in the hope that it will be useful,               //
// but WITHOUT ANY WARRANTY; without even the implied warranty of                //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  //
// GNU General Public License V3 for more details.                               //
//                                                                               //
// You should have received a copy of the GNU General Public License             //
// along with this program. If not, see <http://www.gnu.org/licenses/>.          //
///////////////////////////////////////////////////////////////////////////////////

#ifndef DSDCC_DSD_DECODER_H_
#define DSDCC_DSD_DECODER_H_

#include "dsd_opts.h"
#include "dsd_state.h"
#include "dsd_logger.h"
#include "dsd_symbol.h"
#include "dsd_mbe.h"
#include "dmr_voice.h"
#include "dmr_data.h"
#include "dstar.h"

/*
 * Frame sync patterns
 */
#define INV_P25P1_SYNC "333331331133111131311111"
#define P25P1_SYNC     "111113113311333313133333"

#define X2TDMA_BS_VOICE_SYNC "113131333331313331113311"
#define X2TDMA_BS_DATA_SYNC  "331313111113131113331133"
#define X2TDMA_MS_DATA_SYNC  "313113333111111133333313"
#define X2TDMA_MS_VOICE_SYNC "131331111333333311111131"

#define DSTAR_HD       "131313131333133113131111"
#define INV_DSTAR_HD   "313131313111311331313333"
#define DSTAR_SYNC     "313131313133131113313111"
#define INV_DSTAR_SYNC "131313131311313331131333"

#define NXDN_MS_DATA_SYNC      "313133113131111333"
#define INV_NXDN_MS_DATA_SYNC  "131311331313333111"
#define NXDN_MS_VOICE_SYNC     "313133113131113133"
#define INV_NXDN_MS_VOICE_SYNC "131311331313331311"
#define INV_NXDN_BS_DATA_SYNC  "131311331313333131"
#define NXDN_BS_DATA_SYNC      "313133113131111313"
#define INV_NXDN_BS_VOICE_SYNC "131311331313331331"
#define NXDN_BS_VOICE_SYNC     "313133113131113113"

#define DMR_BS_DATA_SYNC  "313333111331131131331131"
#define DMR_BS_VOICE_SYNC "131111333113313313113313"
#define DMR_MS_DATA_SYNC  "311131133313133331131113"
#define DMR_MS_VOICE_SYNC "133313311131311113313331"

#define INV_PROVOICE_SYNC    "31313111333133133311331133113311"
#define PROVOICE_SYNC        "13131333111311311133113311331133"
#define INV_PROVOICE_EA_SYNC "13313133113113333311313133133311"
#define PROVOICE_EA_SYNC     "31131311331331111133131311311133"

namespace DSDcc
{

class DSDDecoder
{
    friend class DSDSymbol;
    friend class DSDMBEDecoder;
    friend class DSDDMRVoice;
    friend class DSDDMRData;
    friend class DSDDstar;
public:
    typedef enum
    {
        DSDDecodeAuto,
        DSDDecodeNone,
        DSDDecodeP25P1,
        DSDDecodeDStar,
        DSDDecodeNXDN48,
        DSDDecodeNXDN96,
        DSDDecodeProVoice,
        DSDDecodeDMR,
        DSDDecodeX2TDMA
    } DSDDecodeMode;

    typedef enum
    {
        DSDModulationOptimAuto,
        DSDModulationOptimGFSK,
        DSDModulationOptimQPSK,
        DSDModulationOptimC4FM,
    } DSDModulationOptim;

    typedef enum
    {
        DSDShowP25EncryptionSyncBits,
        DSDShowP25LinkControlBits,
        DSDShowP25StatusBitsAndLowSpeedData,
        DSDShowP25TalkGroupInfo
    } DSDShowP25;

    typedef enum
    {
        DSDLookForSync,
        DSDSyncFound,
        DSDprocessFrame,
        DSDprocessNXDNVoice,
        DSDprocessNXDNData,
        DSDprocessDSTAR,
        DSDprocessDSTAR_HD,
        DSDprocessDMRvoice,
        DSDprocessDMRdata,
        DSDprocessX2TDMAvoice,
        DSDprocessX2TDMAdata,
        DSDprocessProVoice,
        DSDprocessUnknown
    } DSDFSMState;

    typedef enum
    {
        DSDSyncP25p1P,
        DSDSyncP25p1N,
        DSDSyncX2TDMADataP,
        DSDSyncX2TDMAVoiceN,
        DSDSyncX2TDMAVoiceP,
        DSDSyncX2TDMADataN,
        DSDSyncDStarP,
        DSDSyncDStarN,
        DSDSyncNXDNVoiceP,
        DSDSyncNXDNVoiceN,
        DSDSyncDMRDataP,
        DSDSyncDMRVoiceN,
        DSDSyncDMRVoiceP,
        DSDSyncDMRDataN,
        DSDSyncProVoiceP,
        DSDSyncProVoiceN,
        DSDSyncNXDNDataP,
        DSDSyncNXDNDataN,
        DSDSyncDStarHeaderP,
        DSDSyncDStarHeaderN,
        DSDSyncNone
    } DSDSyncType;

    typedef enum
    {
        DSDStationTypeNotApplicable,
        DSDBaseStation,
        DSDMobileStation
    } DSDStationType;

    DSDDecoder();
    ~DSDDecoder();

    void run(short sample);

    const char *getMbe() const {
        return &ambe_fr[0][0];
    }

    bool mbeReady() const {
        return m_mbeReady;
    }

    void resetMbe() {
        m_mbeReady = false;
    }

    short *getAudio(int& nbSamples)
    {
        nbSamples = m_state.audio_out_nb_samples;
        return m_state.audio_out_buf;
    }

    void resetAudio()
    {
        m_state.audio_out_nb_samples = 0;
        m_state.audio_out_buf_p = m_state.audio_out_buf;
    }

    //DSDOpts *getOpts() { return &m_opts; }
    //DSDState *getState() { return &m_state; }

    void setLogVerbosity(int verbosity) { m_dsdLogger.setVerbosity(verbosity); }
    void setLogFile(const char *filename) { m_dsdLogger.setFile(filename); }
    const DSDLogger& getLogger() const { return m_dsdLogger; }

    DSDSyncType getSyncType() const
    {
        if (m_state.lastsynctype < 0) {
            return DSDSyncNone;
        } else {
            return (DSDSyncType) m_state.lastsynctype;
        }
    }

    const DSDStationType getStationType() const { return m_stationType; }
    const char *getFrameTypeText() const { return m_state.ftype; }
    const char *getFrameSubtypeText() const { return m_state.fsubtype; }
    const char *getModulationText() const { return m_modulation; }
    const char *getSlot0Text() const { return m_state.slot0light; }
    const char *getSlot1Text() const { return m_state.slot1light; }
    int getInLevel() const { return (int) m_state.max / 164; }
    int getSamplesPerSymbol() const { return m_state.samplesPerSymbol; }
    const DSDDstar& getDStarDecoder() const { return m_dsdDstar; }

    // Initializations:
    void setQuiet();
    void setVerbosity(int verbosity);
    void showDatascope();
    void setDatascopeFrameRate(int frameRate);
    void showErrorBars();
    void showSymbolTiming();
    void setP25DisplayOptions(DSDShowP25 mode, bool on);
    void muteEncryptedP25(bool on);
    void setDecodeMode(DSDDecodeMode mode, bool on);
    void setModulationOptimizations(DSDModulationOptim mode);
    void setAudioGain(float gain);
    void setUvQuality(int uvquality);
    void setUpsampling(int upsampling);
    void setStereo(bool on);
    void setInvertedXTDMA(bool on);
    void setInvertedDMR(bool on);
    void setAutoDetectionThreshold(int threshold);
    void setQPSKSymbolBufferSize(int size);
    void setQPSKMinMaxBufferSize(int size);
    void enableCosineFiltering(bool on);
    void enableAudioOut(bool on);
    void enableScanResumeAfterTDULCFrames(int nbFrames);

    // parameter getters:
    bool upsampling() const { return m_opts.upsample != 0; }

private:
    int getFrameSync();
    void resetFrameSync();
    void printFrameSync(const char *frametype, int offset, char *modulation);
    void noCarrier();
    void printFrameInfo();
    void processFrameInit();
    static int comp(const void *a, const void *b);

    DSDOpts m_opts;
    DSDState m_state;
    DSDLogger m_dsdLogger;
    DSDFSMState m_fsmState;
    DSDStationType m_stationType;
    // sync engine:
    int m_sync; //!< The current internal sync type
    int m_dibit, m_synctest_pos, m_lastt;
    char m_synctest[25];
    char m_synctest18[19];
    char m_synctest32[33];
    char m_modulation[8];
    char *m_synctest_p;
    char m_synctest_buf[10240];
    int m_lmin, m_lmax, m_lidx;
    int m_lbuf[24], m_lbuf2[24];
    int m_lsum;
    char m_spectrum[64];
    int m_t;
    int m_hasSync; //!< tells whether we are in synced phase
    // Symbol extraction and operations
    DSDSymbol m_dsdSymbol;
    // MBE decoder
    char ambe_fr[4][24];
    bool m_mbeReady;
    DSDMBEDecoder m_mbeDecoder;
    // Frame decoders
    DSDDMRVoice m_dsdDMRVoice;
    DSDDMRData m_dsdDMRData;
    DSDDstar m_dsdDstar;
};

} // namespace dsdcc

#endif /* DSDCC_DSD_DECODER_H_ */
