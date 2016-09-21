
#include "xplay.h"
#include "logging.h"
#include "string.h"
#include "stdio.h"
#include "math.h"
#include "logging.h"

#include "portaudio.h"
#include "pa_mac_core.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

unsigned XPlay::GetSampleRate()
{
	return this->sampleRate;
}

unsigned XPlay::GetNumChansOut()
{
	return this->numOut;
}

unsigned XPlay::GetNumChansIn()
{
	return this->numIn;
}

static int xplayCallback( const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData )
{
    /* Cast data passed through stream to our structure. */
    XPlay * xplay = (XPlay *) userData;

    int *out = (int *) outputBuffer;
    int *in  = (int *) inputBuffer;
  
    unsigned int i;
    
    if (statusFlags & paInputOverflow) 
        report_error("Portaudio input overflow");

    if (statusFlags & paOutputUnderflow)
        report_error("Portaudio output underflow");

    if (statusFlags & paInputUnderflow) 
        report_error("Portaudio input underflow");

    if (statusFlags & paOutputOverflow)
        report_error("Portaudio output overflow");

    //log("%d.",framesPerBuffer);
  
    for(int i = 0; i < framesPerBuffer; i++)
    {
        for (int j = 0; j < xplay->GetNumChansOut(); j++) 
        {
            int sample = xplay->outChans->getNextSample();
            *out++ = sample;
        }
    }

#if 0
    in = (int *) inputBuffer;

    for(int i = 0; i < framesPerBuffer; i++)
    {
        for (int j = 0; j < numIn; j++) 
        {
            xplay->inChans[j]->consumeSample(*in++);
        }
    }
#endif

    return 0;
}




int XPlay::run(unsigned delay)
{
	PaError err;
  	err = Pa_Initialize();
  	if( err != paNoError ) 
	{
    	report_error(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
    	return 1;
  	}

	/* Try and find an XMOS soundcard */
  	for (PaHostApiIndex i = 0; i < Pa_GetHostApiCount(); i++) 
	{
    	const PaHostApiInfo *info = Pa_GetHostApiInfo(i);
    	log("Found Host API: %s\n", info->name);
  	}

  	int device = -1;
  	const PaDeviceInfo *DeviceInfo;
  	for (int i = 0; i < Pa_GetDeviceCount(); i++) 
	{
    	const char * name = Pa_GetDeviceInfo(i)->name;
    	log("Found Device %d: %s\n", i, name);

	 	if (useWDM) 
		{
			char wdmIn[] = "Line (XMOS";
			char wdmOut[] = "Speakers (XMOS";
			char *cmp = numIn > 0 ? wdmIn : wdmOut;
			if (strstr(name, cmp) != NULL) 
			{
				log("Using Device %d: %s\n", i, name);
				device = i;
				break;
			}
		}
		else if (strncmp(name, "TUSBAudio", 9) == 0 || strncmp(name, "XMOS", 4) == 0 || strncmp(name, "xCORE", 5) == 0) 
		{
			log("Using Device %d: %s\n", i, name);
			device = i;
			break;
		}
  	}
  
	if( device == -1) 
	{
    	log(  "Warning: Cannot find XMOS or Thesycon ASIO driver\n");
   		device = Pa_GetDefaultOutputDevice();
		
		const char * name = Pa_GetDeviceInfo(device)->name;
    	log("Using Default Device (%d: %s)\n", device, name);
 	}

	PaStream *stream;
	PaStreamParameters outputParameters;
	PaStreamParameters inputParameters;
	memset(&inputParameters, 0, sizeof(inputParameters));
	memset(&outputParameters, 0, sizeof(outputParameters));

	inputParameters.channelCount = numIn;
	inputParameters.device = device;
	inputParameters.sampleFormat = paInt32;
	inputParameters.suggestedLatency = 
	Pa_GetDeviceInfo(inputParameters.device)->defaultHighInputLatency;
#if OS_DARWIN 
	//TODO
	//PaMacCoreStreamInfo hostInfoIn;
	//PaMacCore_SetupStreamInfo(&hostInfoIn, paMacCoreChangeDeviceParameters|paMacCoreFailIfConversionRequired);
	//inputParameters.hostApiSpecificStreamInfo = &hostInfoIn; 
#endif
	outputParameters.channelCount = numOut;
	outputParameters.device = device;
	outputParameters.sampleFormat = paInt32;
	outputParameters.suggestedLatency = 
	Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
#if OS_DARWIN 
	PaMacCoreStreamInfo hostInfoOut;
	PaMacCore_SetupStreamInfo(&hostInfoOut, paMacCoreChangeDeviceParameters|paMacCoreFailIfConversionRequired);
	outputParameters.hostApiSpecificStreamInfo = &hostInfoOut; 
#endif

   /* Open an audio I/O stream. */
   err = Pa_OpenStream( &stream,
                       numIn != 0 ? &inputParameters : NULL,
                       numOut != 0 ? &outputParameters : NULL,
                       sampleRate,
                       paFramesPerBufferUnspecified,
                       paDitherOff,
                       xplayCallback,
                       (void *) this);
  
	if( err != paNoError ) 
	{
    	report_error(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
    	return 1;
  	}

  	err = Pa_StartStream( stream );
  	if( err != paNoError ) 
	{
   		report_error(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
   	 	return 1;
  	}

  	if (delay == 0)
    	while (1);
  	else
   	 	Pa_Sleep(delay);

  	err = Pa_StopStream( stream );
  	if( err != paNoError ) 
	{
    	report_error(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
    	return 1;
  	}

  	err = Pa_CloseStream( stream );
  	if( err != paNoError ) 
	{
    	report_error(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
    	return 1;
  	}

  	err = Pa_Terminate();
  	if( err != paNoError ) 
	{
   		report_error(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
   		return 1;
  	}
  
	log("Complete\n");
  
	return 0;
	
}

OutputChan::OutputChan() { }

void FileReader(FileBuffer &fileBuffer)
{
    int *buf = fileBuffer.writeBuffer;

    size_t bufSize = fileBuffer.getBufferSize();

    printf("FileReader:: using buffer size %d\n", bufSize);

    /* Fill up our first buffer */
    /* TODO handle readcount < bufSize */
    int readcount = sf_read_int(fileBuffer.infile, buf, bufSize);

    /* Signal that we are ready to go! */
    fileBuffer.signalFileReaderInitialized();

    while (1)     
    {
        buf = fileBuffer.getWriteBuffer();
        
        /* Note, libsnd file will do the converestion for use e.g. from wav 16 */
        int readcount = sf_read_int(fileBuffer.infile, buf, bufSize);

        if(readcount == 0)
        {
            /* TODO handle EOF case */
            printf("EOF");
            while(1);
        }
    }
}

#define OUT_BLOCK_SIZE (1024*8)

FileOutputChan::FileOutputChan(char *filename) : OutputChan() 
{
    fileBuffer = new FileBuffer(OUT_BLOCK_SIZE, filename);
    fileThread = new std::thread(FileReader, std::ref(*this->fileBuffer) /* sampleRate, freq, chanId*/);

    /* Note, this will wait until FileReader thread is ready to go.. */
    buf = fileBuffer->getInitialReadBuffer();

    this->bufSize = OUT_BLOCK_SIZE;
    this->count= 0;
    
}

/****** FileOutputChan ********/

FileOutputChan::~FileOutputChan()
{
}

int FileOutputChan::getNextSample(void) 
{
    if(this->count == 0)
    {
        int *old = buf;
        buf = fileBuffer->swapFillBuffers();
        count = this->bufSize;
    }
    
    int sample = buf[bufSize-count];
    count--;

    return sample;
}






static unsigned gcd(unsigned u, unsigned v) 
{
    while ( v != 0) 
    {
        unsigned r = u % v;
        u = v;
        v = r;
    }
    return u;
}

SineOutputChan::SineOutputChan(unsigned sampleRate, unsigned freq) : OutputChan() 
{
    unsigned d = gcd(sampleRate, freq);
    period = sampleRate/d * freq/d;
    table = new int[period];
    for (int i = 0; i < period; i++) 
    {
        float ratio = (double) sampleRate / (double) freq;
        float ii = i;
        float x = sinf(ii * 2 * M_PI / ratio);
        x = x * ldexp(2,27);
        table[i] = (int) x;
    }
    count = 0;
    this->sampleRate = sampleRate;
    this->freq = freq;
    initialDelayCount = 48000;
}

int SineOutputChan::getNextSample(void) 
{
    if (initialDelayCount) 
    {
        initialDelayCount--;
        return 0;
    }
  
    int sample = table[count];
    count++;
    if (count >= period)
    {
        count = 0;
    }
  
    return sample;
}

XPlay::XPlay(unsigned sampleRate, OutputChan *oc)
{
	this->sampleRate = sampleRate;
	this->numOut = 2;	// TODO
	this->numIn = 0; 	// TODO
    this->outChans = oc;

	log("Configured for %d in, %d out @ %d Hz\n", this->numIn, this->numOut, this->sampleRate);

}

XPlay::~XPlay()
{

}
