
#ifndef __XPLAY_H__
#define __XPLAY_H__

#include <sndfile.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "file.h"
#include "wrfile.h"
#include "inputchan.h"
#include "outputchan.h"
#include "logging.h"
#include "plugin.h"
#include "library.h"

/* Note, this gets multiplied up by channel count */
/* TODO Ideally should be based on latency (frames per buffer) */ 
#define BUFFER_LENGTH (1024*4)

#define XPLAY_VERSION_MAJOR 1
#define XPLAY_VERSION_MINOR 3

typedef enum playmode{PLAYMODE_TONE, PLAYMODE_FILE, PLAYMODE_SILENCE} playmode_t;

typedef enum recmode{RECMODE_NONE, RECMODE_FILE, RECMODE_SILENCE} recmode_t;

class XPlay 
{
	public:
  		XPlay(unsigned sampleRate, OutputChan *oc, InputChan *ic, plugin_ *pi);
  		~XPlay();
  		int run(unsigned delay_ms, int device);
  		unsigned GetSampleRate();
        unsigned GetNumChansOut(){return devChanCountOut;};
        unsigned GetNumChansIn(){return devChanCountIn;};
  		OutputChan* outChans;   // TODO Should be private
  		InputChan* inChans;     // TODO Should be private
        plugin_* plugin;            // TODO Should be private;

	private:
  		unsigned sampleRate;
  		unsigned devChanCountIn;
  		unsigned devChanCountOut;
        unsigned playmode;
  		//bool useWDM;          //TODO future option
};

#endif




