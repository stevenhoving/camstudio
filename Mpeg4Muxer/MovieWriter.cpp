// MovieWriter.cpp: implementation of basic file structure classes.
//
//
// Geraint Davies, May 2004
//
// Copyright (c) GDCL 2004-6. All Rights Reserved. 
// You are free to re-use this as the basis for your own filter development,
// provided you retain this copyright notice in the source.
// http://www.gdcl.co.uk
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MovieWriter.h"
#include "TypeHandler.h"
    
Atom::Atom(AtomWriter* pContainer, LONGLONG llOffset, DWORD type)
: m_pContainer(pContainer),
  m_cBytes(0),
  m_llOffset(llOffset),
  m_bClosed(false)
{
    // write the initial length and type dwords
    BYTE b[8];
    WriteLong(8, b);
    WriteLong(type, b+4);
    Append(b, 8);
}

HRESULT 
Atom::Close()
{
    m_bClosed = true;
    // we only support 32-bit lengths for atoms
    // (otherwise you would have to either decide in the constructor
    // or shift the whole atom down).
    if (m_cBytes > 0xffffffff)
    {
        return E_INVALIDARG;
    }

    BYTE b[4];
    WriteLong(long(m_cBytes), b);
    return Replace(0, b, 4);
}

Atom* 
Atom::CreateAtom(DWORD type)
{
    return new Atom(this, Length(), type);
}

// --------------------------------------------------------------------

MovieWriter::MovieWriter(AtomWriter* pContainer)
: m_pContainer(pContainer),
  m_bStopped(false),
  m_bFTYPInserted(false),
  m_tInterleave(UNITS)
{
}

TrackWriter* 
MovieWriter::MakeTrack(const CMediaType* pmt)
{
	TypeHandler* ph = TypeHandler::Make(pmt);
	if (!ph)
	{
		return NULL;
	}
    TrackWriter* pTrack = new TrackWriter(this, (long)m_Tracks.size(), ph);
    m_Tracks.push_back(pTrack);
    return pTrack;
}

HRESULT 
MovieWriter::Close(REFERENCE_TIME* pDuration)
{
    // get longest duration of all tracks
    // also get earliest sample
    vector<TrackWriterPtr>::iterator it;
    REFERENCE_TIME tEarliest = -1;
	REFERENCE_TIME tThis;
    for (it = m_Tracks.begin(); it != m_Tracks.end(); it++)
    {
        TrackWriter* pTrack = *it;
        tThis = pTrack->Earliest();
        if (tThis != -1)
        {
          if ((tEarliest == -1) || (tThis < tEarliest))
          {
              tEarliest = tThis;
          }
        }
    }

    // adjust track start times so that the earliest track starts at 0
    REFERENCE_TIME tDur = 0;
    REFERENCE_TIME tAdj = -tEarliest;
    for (it = m_Tracks.begin(); it != m_Tracks.end(); it++)
    {
        TrackWriter* pTrack = *it;
        pTrack->AdjustStart(tAdj);
        tThis = pTrack->Duration();
        if (tThis > tDur)
        {
            tDur = tThis;
        }
    }
    *pDuration = tDur;
    LONGLONG tScaledDur = tDur * MovieScale() / UNITS;

    // finish writing mdat
    if (m_patmMDAT)
    {
        m_patmMDAT->Close();
        m_patmMDAT = NULL;
    }

    // create moov atom
    HRESULT hr = S_OK;
    smart_ptr<Atom> pmoov = new Atom(m_pContainer, m_pContainer->Length(), DWORD('moov'));

    // movie header
    // we are using 90khz as the movie timescale, so
    // we may need 64-bits.
    smart_ptr<Atom> pmvhd = pmoov->CreateAtom('mvhd');
    BYTE b[28*4];
    ZeroMemory(b, (28*4));
    int cHdr;
    if (tScaledDur > 0x7fffffff)
    {
        b[0] = 1;               // version 1 
        // create time 64bit
        // modify time 64bit
        // timescale 32-bit
        // duration 64-bit
        WriteLong(MovieScale(), b + (5*4));
        WriteI64(tScaledDur, b + (6 * 4));
        cHdr = 8 * 4;
    }
    else
    {
        long lDur = long(tScaledDur);
        WriteLong(MovieScale(), b + (3 * 4));
        WriteLong(lDur, b + (4 * 4));
        cHdr = 5 * 4;
    }
    b[cHdr + 1] = 0x01;
    b[cHdr + 4] = 0x01;
    b[cHdr + 17] = 0x01;
    b[cHdr + 33] = 0x01;
    b[cHdr + 48] = 0x40;
    WriteLong((long)m_Tracks.size() + 1, b + cHdr + 76); // one-based next-track-id
    pmvhd->Append(b, cHdr + 80);
    pmvhd->Close();

    MakeIODS(pmoov);

    for (it = m_Tracks.begin(); it != m_Tracks.end(); it++)
    {
        TrackWriter* pTrack = *it;
        hr = pTrack->Close(pmoov);
        if (FAILED(hr))
        {
            break;
        }
    }
	
	pmoov->Close();

    return hr;
}

void 
MovieWriter::Stop()
{
    CAutoLock lock(&m_csWrite);
    m_bStopped = true;
}
    
void 
MovieWriter::InsertFTYP(AtomWriter* pFile)
{
    if (!m_bFTYPInserted)
    {
		bool bHasOld = false;
	    vector<TrackWriterPtr>::iterator it;
		for (it = m_Tracks.begin(); it != m_Tracks.end(); it++)
		{
			TrackWriter* pTrack = *it;
			if (pTrack->IsNonMP4())
			{
				bHasOld = true;
				break;
			}
		}
        smart_ptr<Atom> pFTYP = new Atom(pFile, pFile->Length(), DWORD('ftyp'));
        // file type
        BYTE b[8];
		if (bHasOld)
		{
			WriteLong(DWORD('qt  '), b);
			// minor version
			b[4] = 0x20;
			b[5] = 0x04;
			b[6] = 0x06;
			b[7] = 0x00;
		}
		else
		{
			WriteLong(DWORD('mp42'), b);
			// minor version
			WriteLong(0, b+4);
		}
		pFTYP->Append(b, 8);
        // additional compatible specs
        WriteLong(DWORD('mp42'), b);
        pFTYP->Append(b, 4);
        WriteLong(DWORD('isom'), b);
        pFTYP->Append(b, 4);
        pFTYP->Close();
        m_bFTYPInserted = true;
    }
}

bool 
MovieWriter::CheckQueues()
{
    CAutoLock lock(&m_csWrite);
    if (m_bStopped)
    {
        return false;
    }

    // threading notes: we don't lock the
    // individual track queues except during the
    // actual access functions. The tracks are free to
    // add data to the end of the queue. The head of the queue 
    // will not be removed except during Stop and by us. The
    // m_bStopped flag ensures that we are not running when the
    // tracks enter Stop.

    // we need to return true if the whole set is at EOS
    // and all queues emptied
    bool bAllFinished;
    for(;;)
    {
        bAllFinished = true; // ... until proven otherwise

        // scan tracks to find which if any should write a chunk
        bool bSomeNotReady = false;
        bool bSomeAtEOS = false;
        LONGLONG tEarliestNotReady = -1;
        LONGLONG tEarliestReady = -1;
        int indexReady = -1;
        for (UINT i = 0; i < m_Tracks.size(); i++)
        {
            LONGLONG tHead;
            if (!m_Tracks[i]->GetHeadTime(&tHead))
            {
                // no chunk ready -- ok if finished
                if (!m_Tracks[i]->IsAtEOS())
                {
                    bAllFinished = false;

                    // note last write time
                    bSomeNotReady = true;
                    LONGLONG tWritten = m_Tracks[i]->LastWrite();
                    if ((tEarliestNotReady == -1) || (tWritten < tEarliestNotReady))
                    {
                        // remember the time of the track that is furthest
                        // behind
                        tEarliestNotReady = tWritten;
                    }
                } else {
                    bSomeAtEOS = true;
                }
            } else {

                bAllFinished = false;  // queue not empty -> not finished

                // remember the earliest of the ready blocks
                if ((tEarliestReady == -1) || (tHead < tEarliestReady))
                {
                    tEarliestReady = tHead;
                    indexReady = i;
                }
            }
        }

        // is there anything to write
        if (indexReady < 0)
        {
            break;
        }
        
        // mustn't get too far ahead of any blocked tracks (unless we have reached EOS)
		if (!bSomeAtEOS && bSomeNotReady)
        {
            // wait for more data on earliest-not-ready track
            break;
        }

		WriteTrack(indexReady);
    }

    return bAllFinished;
}

void
MovieWriter::WriteTrack(int indexReady)
{
    // make sure we have space in an mdat atom
    // -- make a new atom every 1Gb
    if ((m_patmMDAT) && (m_patmMDAT->Length() >= 1024*1024*1024))
    {
        m_patmMDAT->Close();
        m_patmMDAT = NULL;
    }
    if (m_patmMDAT == NULL)
    {
        if (!m_bFTYPInserted)
        {
            InsertFTYP(m_pContainer);
        }
        m_patmMDAT = new Atom(m_pContainer, m_pContainer->Length(), DWORD('mdat'));
    }

	// write earliest block
    m_Tracks[indexReady]->WriteHead(m_patmMDAT);
}

void
MovieWriter::WriteOnStop()
{
    CAutoLock lock(&m_csWrite);
	ASSERT(m_bStopped);

	// loop writing as long as there are blocks queued at the pins
	for (;;)
	{
		LONGLONG tReady = 0;
		int idxReady = -1;
		// find the earliest
        for (UINT i = 0; i < m_Tracks.size(); i++)
        {
            LONGLONG tHead;
            if (m_Tracks[i]->GetHeadTime(&tHead))
            {
				if ((idxReady == -1) ||
					(tHead < tReady))
				{
					idxReady = i;
					tReady = tHead;
				}
			}
		}
	
		if (idxReady == -1)
		{
			// all done
			return;
		}

		WriteTrack(idxReady);
	}
}

REFERENCE_TIME 
MovieWriter::CurrentPosition()
{
    CAutoLock lock(&m_csWrite);
    LONGLONG tEarliest = -1;
    for (UINT i = 0; i < m_Tracks.size(); i++)
    {
        LONGLONG tWritten = m_Tracks[i]->LastWrite();
        if ((tEarliest < 0) || (tWritten < tEarliest))
        {
            tEarliest = tWritten;
        }
    }
    return tEarliest;
}

void 
MovieWriter::MakeIODS(Atom* pmoov)
{
    smart_ptr<Atom> piods = pmoov->CreateAtom('iods');

    Descriptor iod(Descriptor::MP4_IOD);
    BYTE b[16];
    WriteShort(0x004f, b);      // object id 1, no url, no inline profile + reserved bits
    b[2] = 0xff;        // no od capability required
    b[3] = 0xff;        // no scene graph capability required
    b[4] = 0x0f;        // audio profile
    b[5] = 0x03;        // video profile
    b[6] = 0xff;        // no graphics capability required
    iod.Append(b, 7);

    // append the id of each media track
    for (UINT i = 0; i < m_Tracks.size(); i++)
    {
        if (m_Tracks[i]->IsVideo() || m_Tracks[i]->IsAudio())
        {
            // use 32-bit track id in IODS
            Descriptor es(Descriptor::ES_ID_Inc);
            WriteLong(m_Tracks[i]->ID(), b);
            es.Append(b, 4);
            iod.Append(&es);
        }
    }
    WriteLong(0, b);
    piods->Append(b, 4);       // ver/flags
    iod.Write(piods);
    piods->Close();
}

void MovieWriter::RecordBitrate(size_t index, long bitrate)
{
	CAutoLock lock(&m_csBitrate);

	if (m_Bitrates.size() <= index)
	{
		m_Bitrates.resize(index+1, 0);
	}
	if (m_Bitrates[index] < bitrate)
	{
		DbgLog((LOG_TRACE, 0, TEXT("Bitrate %d : %d kb/s"), index, bitrate/1024));

		m_Bitrates[index] = bitrate;
		long totalbits = 0;
		for (size_t i = 0; i < m_Bitrates.size(); i++)
		{
			totalbits += m_Bitrates[i];
		}
		REFERENCE_TIME tNew  = (UNITS * 8 * MAX_INTERLEAVE_SIZE) / totalbits;
		if (tNew < m_tInterleave)
		{
			m_tInterleave = tNew;
			DbgLog((LOG_TRACE, 0, TEXT("Interleave: %d ms"), long(m_tInterleave / 10000)));
		}
	}
}

// -------- Track -------------------------------------------------------

TrackWriter::TrackWriter(MovieWriter* pMovie, int index, TypeHandler* pType)
: m_bEOS(false),
  m_bStopped(false),
  m_index(index),
  m_pType(pType),
  m_tLast(0),
  m_StartAt(0),
  m_pMovie(pMovie),
  m_Durations(90000),     // scale: 90KHz
  m_SC(1)                // dataref 1
{
    // adjust scale to media type (mostly because audio scales must be 16 bits);
    m_Durations.SetScale(pType->Scale());
	m_Durations.SetFrameDuration(m_pType->FrameDuration());
}

HRESULT 
TrackWriter::Add(IMediaSample* pSample)
{
    HRESULT hr = S_OK;
    { 
        // restrict scope of cs so we don't hold it
        // during the CheckQueues call
        CAutoLock lock(&m_csQueue);

        if (m_bEOS || m_bStopped)
        {
            hr = VFW_E_WRONG_STATE;
        } else {
            if (m_pCurrent == NULL)
            {
                m_pCurrent = new MediaChunk(this);
				if (m_pType->IsOldIndexFormat())
				{
					m_pCurrent->SetOldIndexFormat();
				}
            }
            m_pCurrent->AddSample(pSample);
			// write out this block if larger than permitted interleave size
			if (m_pCurrent->IsFull(m_pMovie->MaxInterleaveDuration()))
			{
				REFERENCE_TIME tDur = m_pCurrent->GetDuration();
				if (tDur > 0)
				{
					long bitrate = long(m_pCurrent->Length() * UNITS / tDur) * 8;
					m_pMovie->RecordBitrate(m_index, bitrate);
				}
                m_Queue.push_back(m_pCurrent);
                m_pCurrent = NULL;
            }
        }
    }

    m_pMovie->CheckQueues();

    return hr;
}

// returns true if all tracks now at end
bool 
TrackWriter::OnEOS()
{
    {
        CAutoLock lock(&m_csQueue);
        m_bEOS = true;
        // queue final partial chunk
        if (m_pCurrent && (m_pCurrent->Samples() > 0))
        {
            m_Queue.push_back(m_pCurrent);
            m_pCurrent = NULL;
        }
    }
    return m_pMovie->CheckQueues();
}

// no more writes accepted -- partial/queued writes abandoned
void 
TrackWriter::Stop(bool bFlush)
{
    CAutoLock lock(&m_csQueue);

    // prevent further writes
    m_bStopped = true;

	if (bFlush)
	{
		// discard queued but unwritten samples
		m_pCurrent = NULL;
		m_Queue.clear();
	}
	else
	{
		// queue current partial block 
        if (m_pCurrent && (m_pCurrent->Samples() > 0))
        {
            m_Queue.push_back(m_pCurrent);
            m_pCurrent = NULL;
        }

	}
}

bool 
TrackWriter::GetHeadTime(LONGLONG* ptHead)
{
    CAutoLock lock(&m_csQueue);
    if (m_Queue.size() == 0)
    {
        return false;
    }
    MediaChunkPtr pChunk = *m_Queue.begin();
    REFERENCE_TIME tLast;
    pChunk->GetTime(ptHead, &tLast);
    return true;
}

HRESULT 
TrackWriter::WriteHead(Atom* patm)
{
    CAutoLock lock(&m_csQueue);
    if (m_Queue.size() == 0)
    {
        return E_FAIL;
    }
    MediaChunkPtr pChunk = *m_Queue.begin();
    m_Queue.pop_front();

    REFERENCE_TIME tStart, tEnd;
    pChunk->GetTime(&tStart, &tEnd);

    // the chunk will call back to us to index
    // the samples during this call, once
    // the media data is successfully written
    HRESULT hr = pChunk->Write(patm);
    if (SUCCEEDED(hr))
    {
        m_tLast = tEnd;
    }
    return hr;
}

REFERENCE_TIME 
TrackWriter::LastWrite()
{
    CAutoLock lock(&m_csQueue);
    return m_tLast;
}

void 
TrackWriter::IndexChunk(LONGLONG posChunk, long nSamples)
{
    m_SC.Add(nSamples);
    m_CO.Add(posChunk);
}

void 
TrackWriter::IndexSample(bool bSync, REFERENCE_TIME tStart, REFERENCE_TIME tStop, long cBytes)
{
    // CTS offset means ES type-specific content parser?
	// -- this is done now by calculation from the frames start time (heuristically!)

    m_Sizes.Add(cBytes);
    m_Durations.Add(tStart, tStop);
    m_Syncs.Add(bSync);
}

void 
TrackWriter::OldIndex(LONGLONG posChunk, long cBytes)
{
	// check if it works before optimising it!
	int nSamples = cBytes / m_pType->BlockAlign();
	m_Sizes.AddMultiple(1, nSamples);
	m_Durations.AddOldFormat(nSamples);

	m_SC.Add(nSamples);
	m_CO.Add(posChunk);
}

HRESULT 
TrackWriter::Close(Atom* patm)
{
    smart_ptr<Atom> ptrak = patm->CreateAtom('trak');

    // track header tkhd
    smart_ptr<Atom> ptkhd = ptrak->CreateAtom('tkhd');
    BYTE b[24*4];
    ZeroMemory(b, (24*4));

    // duration in movie timescale
    LONGLONG scaledur = long(Duration() * m_pMovie->MovieScale() / UNITS);
    int cHdr = 6 * 4;
    if (scaledur > 0x7fffffff)
    {
        // use 64-bit version (64-bit create/modify and duration
        cHdr = 9*4;
        b[0] = 1;
        WriteLong(ID(), b+(5*4));
        WriteI64(scaledur, b+(7*4));
    }
    else
    {
        WriteLong(ID(), b+(3*4));     // 1-base track id
        WriteLong(long(scaledur), b+(5*4));
        cHdr = 6*4;
    }
    b[3] = 7;   // enabled, in movie and in preview

    if (IsAudio())
    {
        b[cHdr + 12] = 0x01;
    }
    b[cHdr + 17] = 1;
    b[cHdr + 33] = 1;
    b[cHdr + 48] = 0x40;
    if (IsVideo())
    {
		WriteShort(m_pType->Width(), &b[cHdr + 52]);
		WriteShort(m_pType->Height(), &b[cHdr + 56]);
    }

    ptkhd->Append(b, cHdr + 60);
    ptkhd->Close();

    // track ref tref
    Handler()->WriteTREF(ptrak);

    // edts -- used for first-sample offet
    // -- note, this is in movie timescale, not track
    m_Durations.WriteEDTS(ptrak, m_pMovie->MovieScale());

    smart_ptr<Atom> pmdia = ptrak->CreateAtom('mdia');

    // Media Header mdhd
    smart_ptr<Atom> pmdhd = pmdia->CreateAtom('mdhd');
    ZeroMemory(b, 9*4);
    
    // duration now in track timescale
    scaledur = m_Durations.Duration() * m_Durations.Scale() / UNITS;
    if (scaledur > 0x7fffffff)
    {
        b[0] = 1;       // 64-bit
        WriteLong(m_Durations.Scale(), b+20);
        WriteI64(scaledur, b+24);         
        cHdr = 8*4;
    }
    else
    {
        WriteLong(m_Durations.Scale(), b+12);
        WriteLong(long(scaledur), b+16);         
        cHdr = 5*4;
    }
    // 'eng' as offset from 0x60 in 0 pad bit plus 3x5-bit (05 0xe 07)
    b[cHdr] = 0x15;
    b[cHdr+1] = 0xc7;
    pmdhd->Append(b, cHdr + 4);
    pmdhd->Close();

    // handler id hdlr
    smart_ptr<Atom> phdlr = pmdia->CreateAtom('hdlr');
    ZeroMemory(b, 25);
	WriteLong(Handler()->DataType(), b+4);
    WriteLong(Handler()->Handler(), b+8);
    phdlr->Append(b, 25);
    phdlr->Close();
    
    smart_ptr<Atom> pminf = pmdia->CreateAtom('minf');

    // media information header vmhd/smhd
    ZeroMemory(b, sizeof(b));
    if (IsVideo())
    {
        smart_ptr<Atom> pvmhd = pminf->CreateAtom('vmhd');
        b[3] = 1;
        pvmhd->Append(b, 12);
        pvmhd->Close();
    } else if (IsAudio())
    {
        smart_ptr<Atom> psmhd = pminf->CreateAtom('smhd');
        psmhd->Append(b, 8);
        psmhd->Close();
    } else {
        smart_ptr<Atom> pnmhd = pminf->CreateAtom('nmhd');
        pnmhd->Append(b, 4);
        pnmhd->Close();
    }

    // dinf/dref -- data reference
    smart_ptr<Atom> pdinf = pminf->CreateAtom('dinf');
    smart_ptr<Atom> pdref = pdinf->CreateAtom('dref');
    WriteLong(0, b);        // ver/flags
    WriteLong(1, b+4);      // entries
    pdref->Append(b, 8);
    smart_ptr<Atom> purl = pdref->CreateAtom('url ');
    // self-contained flag set, and no string required
    // -- all data is in this file
    b[3] = 1;
    purl->Append(b, 4);
    purl->Close();
    pdref->Close();
    pdinf->Close();

    smart_ptr<Atom> pstbl = pminf->CreateAtom('stbl');

    // Sample description
    // -- contains one descriptor atom mp4v/mp4a/... for each data reference.
    smart_ptr<Atom> pstsd = pstbl->CreateAtom('stsd');
    WriteLong(0, b);    // ver/flags
    WriteLong(1, b+4);    // count of entries
    pstsd->Append(b, 8);
    Handler()->WriteDescriptor(pstsd, ID(), 1, m_Durations.Scale());   // dataref = 1
    pstsd->Close();

    HRESULT hr = m_Durations.WriteTable(pstbl);
    if (SUCCEEDED(hr))
    {
        hr = m_Syncs.Write(pstbl);
    }
    if (SUCCEEDED(hr))
    {
        hr = m_SC.Write(pstbl);
    }
    if (SUCCEEDED(hr))
    {
        hr = m_Sizes.Write(pstbl);
    }
    if (SUCCEEDED(hr))
    {
        hr = m_CO.Write(pstbl);
    }
    pstbl->Close();
    pminf->Close();
    pmdia->Close();
    ptrak->Close();

    return hr;
}

// -- Media Chunk ----------------------

MediaChunk::MediaChunk(TrackWriter* pTrack)
: m_cBytes(0),
  m_pTrack(pTrack),
  m_tStart(0),
  m_tEnd(0),
  m_bOldIndexFormat(false)
{
}
MediaChunk::~MediaChunk()
{
    // I wanted to use list<IMediaSamplePtr> but the
    // compiler could not handle the deep nesting of templates
    while(m_Samples.size() > 0)
    {
        IMediaSample* pSample = m_Samples.front();
        pSample->Release();
        m_Samples.pop_front();
    }
}

HRESULT 
MediaChunk::AddSample(IMediaSample* pSample)
{
    REFERENCE_TIME tStart, tEnd;
    HRESULT hr = pSample->GetTime(&tStart, &tEnd);
    if (SUCCEEDED(hr)) // watch out for VFW_S_NO_STOP_TIME
    {
        // H264 samples from large frames
        // may be broken across several buffers, with the
        // time set on the last sample

        // record overall time
        if (m_cBytes == 0)
        {
            // first sample
            m_tStart = tStart;
            m_tEnd = tEnd;
        } else {
            if (tStart < m_tStart)
            {
                m_tStart = tStart;
            }
            if (tEnd > m_tEnd)
            {
                m_tEnd = tEnd;
            }
        }
    }

    m_cBytes += pSample->GetActualDataLength();
    pSample->AddRef();
    m_Samples.push_back(pSample);
    return S_OK;
}

HRESULT 
MediaChunk::Write(Atom* patm)
{
    // record chunk start position
    LONGLONG posChunk = patm->Position() + patm->Length();

	if (m_bOldIndexFormat)
	{
		long cBytes = 0;

		// ensure that we don't break in the middle of a sample (Maxim Kartavenkov)
		const int MAX_PCM_SIZE = 22050;
		int max_bytes = MAX_PCM_SIZE - (MAX_PCM_SIZE % m_pTrack->Handler()->BlockAlign());

		list<IMediaSample*>::iterator it = m_Samples.begin();
		long cAvail = 0;
		BYTE* pBuffer = NULL;

		for (;;)
		{
			if (!cAvail)
			{
				if (it == m_Samples.end())
				{
					break;
				}
				IMediaSample* pSample = *it++;
				pSample->GetPointer(&pBuffer);
				cAvail = pSample->GetActualDataLength();
				REFERENCE_TIME tStart, tStop;
				if (SUCCEEDED(pSample->GetTime(&tStart, &tStop)))
				{
					m_pTrack->SetOldIndexStart(tStart);
				}
			}
			long cThis = max_bytes - cBytes;
			if (cThis > cAvail)
			{
				cThis = cAvail;
			}
			
			int cActual = 0;
			m_pTrack->Handler()->WriteData(patm, pBuffer, cThis, &cActual);
			cBytes += cActual;
			cAvail -= cActual;
			pBuffer += cActual;

			if (cBytes >= max_bytes)
			{
				m_pTrack->OldIndex(posChunk, cBytes);
				posChunk = patm->Position() + patm->Length();				
				cBytes = 0;
			}
		}
		if (cBytes)
		{
			m_pTrack->OldIndex(posChunk, cBytes);
		}
		return S_OK;
	}

    // Remember that large H264 samples may be broken 
    // across several buffers, with Sync flag at start and
    // time on last buffer.
    bool bSync = false;
    long cBytes = 0;
	long nSamples = 0;

    // loop once through the samples writing the data
    list<IMediaSample*>::iterator it;
    for (it = m_Samples.begin(); it != m_Samples.end(); it++)
    {
        IMediaSample* pSample = *it;

        // record positive sync flag, but for
        // multiple-buffer samples, only one sync flag will be present
        // so don't overwrite with later negatives.
        if (pSample->IsSyncPoint() == S_OK)
        {
            bSync = true;
        }

		// write payload, including any transformation (eg BSF to length-prepended)
        BYTE* pBuffer;
        pSample->GetPointer(&pBuffer);
		int cActual = 0;
		m_pTrack->Handler()->WriteData(patm, pBuffer, pSample->GetActualDataLength(), &cActual);
		cBytes += cActual;
        REFERENCE_TIME tStart, tEnd;
        HRESULT hr = pSample->GetTime(&tStart, &tEnd);
        if (SUCCEEDED(hr))
        {
			// this is the last buffer in the sample
			m_pTrack->IndexSample(bSync, tStart, tEnd, cBytes);
            // reset for new sample
            bSync = false;
			cBytes = 0;
			nSamples++;
        }
    }

    // add chunk position to index
	m_pTrack->IndexChunk(posChunk, nSamples);

    return S_OK;
}

bool 
MediaChunk::IsFull(REFERENCE_TIME tMaxDur)
{
	if (Length() > MAX_INTERLEAVE_SIZE)
	{
		return true;
	}

	if (GetDuration() > tMaxDur)
	{
		return true;
	}
	return false;
}

REFERENCE_TIME MediaChunk::GetDuration()
{
	if (m_pTrack->IsAudio())
	{
		return (m_tEnd - m_tStart);
	}
	else
	{
		return Samples() * m_pTrack->SampleDuration();
	}
}


// ---- index classes --------------------

ListOfLongs::ListOfLongs()
: m_nEntriesInLast(0)
{
    m_Blocks.push_back(new BYTE[EntriesPerBlock * 4]);
}

void 
ListOfLongs::Append(long l)
{
    if (m_nEntriesInLast >= EntriesPerBlock)
    {
        m_Blocks.push_back(new BYTE[EntriesPerBlock * 4]);
		m_nEntriesInLast = 0;
    }
    BytePtr p = m_Blocks[m_Blocks.size() - 1];
    WriteLong(l, p + m_nEntriesInLast*4);
    m_nEntriesInLast++;
}

HRESULT 
ListOfLongs::Write(Atom* patm)
{
    // write all the full blocks
    for (UINT i = 0; i < m_Blocks.size() -1; i++)
    {
        BytePtr p = m_Blocks[i];
        HRESULT hr = patm->Append(p, EntriesPerBlock*4);
        if (FAILED(hr))
        {
            return hr;
        }
    }

    // partial last block
    if (m_nEntriesInLast > 0)
    {
        BytePtr p = m_Blocks[m_Blocks.size() - 1];
        HRESULT hr = patm->Append(p, m_nEntriesInLast * 4);
        if (FAILED(hr))
        {
            return hr;
        }
    }
    return S_OK;
}

long 
ListOfLongs::Entry(long nEntry)
{
    // read back a value (for 32 to 64 conversion)
    long nValue = 0;
    if (nEntry < Entries())
    {
        BytePtr p = m_Blocks[nEntry/EntriesPerBlock];
        nValue = ReadLong(p + (nEntry % EntriesPerBlock)*4);
    }
    return nValue;
}


ListOfI64::ListOfI64()
: m_nEntriesInLast(0)
{
    m_Blocks.push_back(new BYTE[EntriesPerBlock * 8]);
}

void 
ListOfI64::Append(LONGLONG ll)
{
    if (m_nEntriesInLast >= EntriesPerBlock)
    {
        m_Blocks.push_back(new BYTE[EntriesPerBlock * 8]);
		m_nEntriesInLast = 0;
    }
    BytePtr p = m_Blocks[m_Blocks.size() - 1];
    WriteI64(ll, p + m_nEntriesInLast*8);
    m_nEntriesInLast++;
}

HRESULT 
ListOfI64::Write(Atom* patm)
{
    // write all the full blocks
    for (UINT i = 0; i < m_Blocks.size() -1; i++)
    {
        BytePtr p = m_Blocks[i];
        HRESULT hr = patm->Append(p, EntriesPerBlock*8);
        if (FAILED(hr))
        {
            return hr;
        }
    }

    // partial last block
    if (m_nEntriesInLast > 0)
    {
        BytePtr p = m_Blocks[m_Blocks.size() - 1];
        HRESULT hr = patm->Append(p, m_nEntriesInLast * 8);
        if (FAILED(hr))
        {
            return hr;
        }
    }
    return S_OK;
}

ListOfPairs::ListOfPairs()
: m_cEntries(0),
  m_lCount(0)
{
}

void 
ListOfPairs::Append(long val)
{
	if (m_lCount == 0)
	{
		m_lCount = 1;
		m_lValue = val;
	}
	else if (val == m_lValue)
    {
        m_lCount++;
    } else {
        m_Table.Append(m_lCount);
        m_Table.Append(m_lValue);

		m_lCount = 1;
        m_lValue = val;
    }
	m_cEntries++;
}

HRESULT 
ListOfPairs::Write(Atom* patm)
{
	if (m_lCount > 0)
	{
		m_Table.Append(m_lCount);
		m_Table.Append(m_lValue);
	}
    // ver/flags == 0
    // nEntries
    // pairs of <count, value>

    BYTE b[8];
    ZeroMemory(b, 8);
    // entry count is count of pairs
    WriteLong(m_Table.Entries() / 2, b+4);

    HRESULT hr = patm->Append(b, 8);

	if (SUCCEEDED(hr))
    {
        hr = m_Table.Write(patm);
    }
	return hr;
}

// -----

SizeIndex::SizeIndex()
: m_cBytesCurrent(0),
  m_nCurrent(0),
  m_nSamples(0)
{
}

void 
SizeIndex::AddMultiple(long cBytes, long count)
{
	if (m_nSamples == 0)
	{
		// first entry
		m_cBytesCurrent = cBytes;
		m_nCurrent = count;
		m_nSamples = count;
	}
	else if ((m_nCurrent > 0) && (cBytes == m_cBytesCurrent))
	{
		// normal case
		m_nCurrent += count;
		m_nSamples += count;
	}
	else
	{
		// not worth trying to optimise this, but make sure
		// it works if we ever get here.
		for (int i = 0; i < count; i++)
		{
			Add(cBytes);
		}
	}
}
void 
SizeIndex::Add(long cBytes)
{
    // if all sizes are the same, we only need a single count/size entry.
    // Otherwise we need one size for each entry

    if (m_nSamples == 0)
    {
        // first sample
        m_cBytesCurrent = cBytes;
        m_nCurrent = 1;
    } else if (m_nCurrent > 0)
    {
        // still accumulating identical sizes
        if (cBytes == m_cBytesCurrent)
        {
            // another of the same size
            m_nCurrent++;
        } else {
            // different -- need to create an entry for every one so far
            for (long n = 0; n < m_nCurrent; n++)
            {
                m_Table.Append(m_cBytesCurrent);
            }
            m_nCurrent = 0;

            // add this sample
            m_Table.Append(cBytes);
        }
    } else 
    {
        // we are creating a separate entry for each sample
        m_Table.Append(cBytes);
    }
    m_nSamples++;
}

HRESULT 
SizeIndex::Write(Atom* patm)
{
    smart_ptr<Atom> psz = patm->CreateAtom('stsz');
    BYTE b[12];
    ZeroMemory(b, 12);

    // ver/flags = 0
    // size
    // count
    // if size == 0, list of <count> sizes

    if (m_Table.Entries() == 0)
    {
        // this size field is left 0 if we are
        // creating a size entry for each sample
        WriteLong(m_cBytesCurrent, b+4);
    }
    WriteLong(m_nSamples, b+8);
    psz->Append(b, 12);
    if (m_Table.Entries() > 0)
    {
        m_Table.Write(psz);
    }
    psz->Close();
    return S_OK;
}

DurationIndex::DurationIndex(long scale)
: m_scale(scale),
  m_TotalDuration(0),
  m_refDuration(0),
  m_tStartFirst(-1),
  m_tStartLast(0),
  m_tStopLast(0),
  m_nSamples(0),
  m_bCTTS(false),
  m_tFrame(0)
{
}

void 
DurationIndex::Add(REFERENCE_TIME tStart, REFERENCE_TIME tEnd)
{
	// In general it is safer to just use the start time of each sample
	// since the stop time will be either wrong, or will just be deduced from
	// the next sample start time.
	// However, when frame re-ordering is happening, the composition time (== PTS) will 
	// not be the same as the decode time (== DTS) and we need to use both start and
	// stop time to build the CTTS table. 
	// We save the first few timestamps and then decide which mode to be in.
	if (m_nSamples < mode_decide_count)
	{
		if (m_nSamples == 0)
		{
			m_SumDurations = 0;
		}
		m_SumDurations += (tEnd - tStart);
		
		m_SampleStarts[m_nSamples] = tStart;
		m_SampleStops[m_nSamples] = tEnd;
		m_nSamples++;
		return;
	}
	else if (m_nSamples == mode_decide_count)
	{
		// this decides on a mode and then processes 
		// all the samples in the table
		ModeDecide();
	}

	if (m_bCTTS)
	{
		AppendCTTSMode(tStart, tEnd);
	}
	else
	{
        AddDuration(long(ToScale(tStart) - m_TotalDuration));
    }
	m_nSamples++;
    m_tStartLast = tStart;
    m_tStopLast = tEnd;
    return;
}

void 
DurationIndex::AddOldFormat(int count)
{
	for (int i = 0; i < count; i++)
	{
		AddDuration(1);
	}
	m_nSamples += count;
	m_tStopLast = ToReftime(m_nSamples);
}

void 
DurationIndex::SetOldIndexStart(REFERENCE_TIME tStart)
{
	if (m_nSamples == 0)
	{
		m_tStartFirst = tStart;
	}
}

void
DurationIndex::AddDuration(long cThis)
{
	if (cThis < 0)
	{
		// sometimes AdjustTimes causes the final duration to be negative.
		// I think I've fixed that, but just in case:
		cThis = 1;
	}

	m_STTS.Append(cThis);

    m_TotalDuration += cThis;
}

void 
DurationIndex::AppendCTTSMode(REFERENCE_TIME tStart, REFERENCE_TIME tEnd)
{
	// if the frames are out of order and the end time is invalid,
	// we must use the frame rate to work out the difference
	REFERENCE_TIME dur;
	if (m_bUseFrameRate)
	{
		dur = m_tFrame;
	}
	else
	{
		 dur = (tEnd - tStart);
	}
	// ToScale will round down, and this truncation causes the 
	// diff between decode and presentation time to get larger and larger
	// so we sum both reference time and scaled totals and use the difference. 
	// That way the rounding error does not build up.
	// the simpler version of these two lines is: cThis = long(ToScale(tEnd - tStart));
	m_refDuration += dur;
	long cThis = long(ToScale(m_refDuration) - m_TotalDuration);

	// difference between sum of durations to here and actual CTS
	// -- note: do this before adding current sample to total duration
	long cDiff =  long(ToScale(tStart) - m_TotalDuration);

	AddDuration(cThis);

	m_CTTS.Append(cDiff);
}

void
DurationIndex::ModeDecide()
{
	if (m_nSamples > 0)
	{
		bool bReverse = false;
		bool bDurOk = true;
		LONGLONG ave = m_SumDurations / m_nSamples;

		// 70fps is the maximum reasonable frame rate, so anything less than this is
		// likely to be an error
		const REFERENCE_TIME min_frame_dur = (UNITS / 70);
		if (ave < min_frame_dur)
		{
			bDurOk = false;
		}

		// in the most common case, when converting from TS output, we don't
		// get either accurate end times or an accuration rate in the media type.
		// The smallest positive interval between frames should be the frame duration.
		REFERENCE_TIME tInterval = 0;

		for (int i = 0; i < m_nSamples; i++)
		{
			if (i > 0)
			{
				if (m_SampleStarts[i] < m_SampleStarts[i-1])
				{
					bReverse = true;
				}
				else 
				{
					REFERENCE_TIME tThis = m_SampleStarts[i] - m_SampleStarts[i-1];
					if (tThis > min_frame_dur)
					{
						if ((tInterval == 0) || (tThis < tInterval))
						{
							tInterval = tThis;
						}
					}
				}
			}
		}
		m_tStartFirst = m_SampleStarts[0];

		// this interval is a better guess than the media type frame rate
		if (tInterval > min_frame_dur)
		{
			m_tFrame = tInterval;
		}

		if (bReverse)
		{
			m_bCTTS = true;

			if (!bDurOk)
			{
				m_bUseFrameRate = true;
			}
			else
			{
				m_bUseFrameRate = false;
			}

			// remember that the first frame might not be zero
			m_TotalDuration = ToScale(m_tStartFirst);
			m_refDuration = m_tStartFirst;
			for (int i = 0; i < m_nSamples; i++)
			{
				AppendCTTSMode(m_SampleStarts[i], m_SampleStops[i]);
			}
		}
		else
		{
			m_bCTTS = false;
			m_TotalDuration = ToScale(m_tStartFirst);
			for (int i = 1; i < m_nSamples; i++)
			{
				AddDuration(long(ToScale(m_SampleStarts[i]) - m_TotalDuration));
			}
		}
	}
}

HRESULT 
DurationIndex::WriteEDTS(Atom* patm, long scale)
{
    if (m_tStartFirst > 0)
    {
        // structure is 8 x 32-bit values
        //  flags/ver
        //  nr of entries
        //     duration : offset of first sample
        //     -1 : media time -- no media
        //     media rate: 1 (16 bit + 16-bit 0)
        //     duration : duration of whole track
        //     0 : start of media
        //     media rate 1

        smart_ptr<Atom> pedts = patm->CreateAtom('edts');
        smart_ptr<Atom> pelst = pedts->CreateAtom('elst');

        BYTE b[48];
        ZeroMemory(b, sizeof(b));

        // values are in movie scale
        LONGLONG offset = long(m_tStartFirst * scale / UNITS);
        LONGLONG dur  = long((m_tStopLast - m_tStartFirst) * scale / UNITS);

        int cSz;
        if ((offset > 0x7fffffff) || (dur > 0x7fffffff))
        {
            b[0] = 1;   // version 1 = 64-bit entries

            // create an offset for the first sample
            // using an "empty" edit
            WriteLong(2, b+4);
            WriteI64(offset, b+8);
            WriteI64(-1, b+16);        // no media used
            b[25] = 1;

            // whole track as next edit
            WriteI64(dur, b+28);
            WriteI64(0, b+36);
            b[45] = 1;
            cSz = 48;
        }
        else
        {
            // create an offset for the first sample
            // using an "empty" edit
            WriteLong(2, b+4);
            WriteLong(long(offset), b+8);
            WriteLong(-1, b+12);        // no media used
            b[17] = 1;

            // whole track as next edit
            WriteLong(long(dur), b+20);
            WriteLong(0, b+24);
            b[29] = 1;
            cSz = 32;
        }
        pelst->Append(b, cSz);

        pelst->Close();
        pedts->Close();
    }
    return S_OK;
}

HRESULT 
DurationIndex::WriteTable(Atom* patm)
{
    // do nothing if no samples at all
    HRESULT hr = S_OK;
	if (m_nSamples <= mode_decide_count)
	{
		ModeDecide();
	}
	if (m_nSamples > 0)
    {
		if (!m_bCTTS)
		{
			// the final sample duration has not been recorded -- use the
			// stop time
			if (ToScale(m_tStopLast) > m_TotalDuration)
			{
				AddDuration(long(ToScale(m_tStopLast) - m_TotalDuration));
			}
		}

        // create atom and write table
        smart_ptr<Atom> pstts = patm->CreateAtom('stts');
		m_STTS.Write(pstts);
        pstts->Close();

		if (m_bCTTS)
		{
			// write CTTS table
			smart_ptr<Atom> pctts = patm->CreateAtom('ctts');
			m_CTTS.Write(pctts);
			pctts->Close();
		}
    }
    return hr;
}

SamplesPerChunkIndex::SamplesPerChunkIndex(long dataref)
: m_dataref(dataref),
  m_nTotalChunks(0),
  m_nSamples(0)
{
}

void 
SamplesPerChunkIndex::Add(long nSamples)
{
    // make a new entry if the old one does not match
    if (m_nSamples != nSamples)
    {
        // the entry is <chunk nr, samples per chunk, data ref>
        // The Chunk Nr is one-based
        m_Table.Append(m_nTotalChunks+1);
        m_Table.Append(nSamples);
        m_Table.Append(m_dataref);

        m_nSamples = nSamples;
    }
    m_nTotalChunks++;
}

HRESULT 
SamplesPerChunkIndex::Write(Atom* patm)
{
    smart_ptr<Atom> pSC = patm->CreateAtom('stsc');
    //
    // ver/flags = 0
    // count of entries
    //    triple <first chunk, samples per chunk, dataref>

    BYTE b[8];
    WriteLong(0, b);
    WriteLong(m_Table.Entries() / 3, b+4);
    HRESULT hr = pSC->Append(b, 8);
    if (SUCCEEDED(hr))
    {
        hr = m_Table.Write(pSC);
    }
    return hr;
}

void 
ChunkOffsetIndex::Add(LONGLONG posChunk)
{
    // use the 32-bit table until we see a
    // value > 2Gb, then always use the 
    // 64-bit table for the remainder.
    if ((posChunk >= 0x80000000) || (m_Table64.Entries() > 0))
    {
        m_Table64.Append(posChunk);
    } else {
        m_Table32.Append(long(posChunk & 0xffffffff));
    }
}

HRESULT 
ChunkOffsetIndex::Write(Atom* patm)
{
    HRESULT hr = S_OK;
    // did we need 64-bit offsets?
    if (m_Table64.Entries() > 0)
    {
        // convert 32-bit entries to 64-bit
        ListOfI64 converted;
        for (long idx = 0; idx < m_Table32.Entries(); idx++)
        {
            converted.Append(m_Table32.Entry(idx));
        }

        // create 64-bit atom co64
        smart_ptr<Atom> pCO = patm->CreateAtom('co64');
        BYTE b[8];
        WriteLong(0, b);        // ver/flags
        long nEntries = converted.Entries() + m_Table64.Entries();
        WriteLong(nEntries, b+4);
        hr = pCO->Append(b, 8);
        if (SUCCEEDED(hr))
        {
            hr = converted.Write(pCO);
        }
        if (SUCCEEDED(hr))
        {
            hr = m_Table64.Write(pCO);
        }

        pCO->Close();
    } else if (m_Table32.Entries() > 0) {
        // 32-bit atom
        smart_ptr<Atom> pCO = patm->CreateAtom('stco');
        BYTE b[8];
        WriteLong(0, b);        // ver/flags
        WriteLong(m_Table32.Entries(), b+4);
        hr = pCO->Append(b, 8);
        if (SUCCEEDED(hr))
        {
            hr = m_Table32.Write(pCO);
        }
        pCO->Close();
    }
    return hr;
}

SyncIndex::SyncIndex()
: m_bAllSync(true),
  m_nSamples(0)
{
}

void 
SyncIndex::Add(bool bSync)
{
    if (m_bAllSync)
    {
        if (!bSync)
        {
            // no longer all syncs - 
            m_bAllSync = false;

            // must create table entries for all syncs so far
            for (long i = 0; i < m_nSamples; i++)
            {
                // 1-based sample index
                m_Syncs.Append(i+1);
            }
            // but we don't need to record this one as it is not sync
        }
    } else {
        if (bSync)
        {
            m_Syncs.Append(m_nSamples+1);
        }
    }
    m_nSamples++;
}

HRESULT 
SyncIndex::Write(Atom* patm)
{
    HRESULT hr = S_OK;

    // if all syncs, create no table
    if (!m_bAllSync)
    {
        smart_ptr<Atom> pss = patm->CreateAtom('stss');

        BYTE b[8];
        WriteLong(0, b);    // ver/flags
        WriteLong(m_Syncs.Entries(), b+4);
        hr = pss->Append(b, 8);
        if (SUCCEEDED(hr))
        {
            hr = m_Syncs.Write(pss);
        }
        pss->Close();
    }
    return hr;
}



