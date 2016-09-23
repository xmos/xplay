
#ifndef __XPLAY_H__
#define __XPLAY_H__

#include <sndfile.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "file.h"
#include "inputchan.h"
#include "outputchan.h"

#define OUT_BLOCK_SIZE (1024*8)

typedef enum playmode{PLAYMODE_TONE, PLAYMODE_FILE, PLAYMODE_SILENCE} playmode_t;

typedef enum recmode{RECMODE_NONE, RECMODE_FILE, RECMODE_SILENCE} recmode_t;

class XPlay 
{
	public:
  		XPlay(unsigned sampleRate, OutputChan *oc);
  		~XPlay();
  		int run(unsigned delay_ms);
  		unsigned GetSampleRate();
        unsigned GetNumChansOut(){return devChanCountOut;};
        unsigned GetNumChansIn(){return devChanCountIn;};
  		OutputChan* outChans;   // TODO Should be private
  		InputChan* inChans;

	private:
  		unsigned sampleRate;
  		unsigned devChanCountIn;
  		unsigned devChanCountOut;
        unsigned playmode;
  		//bool loopback;        //TODO future option
  		//bool useWDM;          //TODO future option
};

#endif




