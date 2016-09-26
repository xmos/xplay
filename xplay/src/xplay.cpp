
#include "xplay.h"
#include "logging.h"
#include "string.h"
#include "stdio.h"
#include "logging.h"

#include "portaudio.h"

#if OS_DARWIN 
#include "pa_mac_core.h"
#endif

unsigned XPlay::GetSampleRate()
{
    return this->sampleRate;
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
 
    if(xplay->outChans != NULL)
    { 
        for(int i = 0; i < framesPerBuffer; i++)
        {
            for (int j = 0; j < xplay->outChans->getChanCount(); j++) 
            {
                int sample = xplay->outChans->getNextSample();
                *out++ = sample;
            }
        }
    }

    if(xplay->inChans != NULL)
    {
        in = (int *) inputBuffer;

        for(int i = 0; i < framesPerBuffer; i++)
        {
            for (int j = 0; j < xplay->inChans->getChanCount(); j++) 
            {
                xplay->inChans->consumeSample(*in++);
            }
        }
    }

    return 0;
}




int XPlay::run(unsigned delay)
{
    int useWDM = 0;

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
            char *cmp = devChanCountIn == 0 ? wdmIn : wdmOut; 
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

    inputParameters.channelCount = devChanCountIn; // Should this be the chan count of device or file? 
    inputParameters.device = device;
    inputParameters.sampleFormat = paInt32;
    inputParameters.suggestedLatency = 
    Pa_GetDeviceInfo(inputParameters.device)->defaultHighInputLatency;
#if OS_DARWIN 
    PaMacCoreStreamInfo hostInfoIn;
    PaMacCore_SetupStreamInfo(&hostInfoIn, paMacCoreChangeDeviceParameters|paMacCoreFailIfConversionRequired);
    inputParameters.hostApiSpecificStreamInfo = &hostInfoIn; 
#endif
    outputParameters.channelCount = devChanCountOut;
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
                       devChanCountIn != 0 ? &inputParameters : NULL,
                       devChanCountOut != 0 ? &outputParameters : NULL,
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



XPlay::XPlay(unsigned sampleRate, OutputChan *oc, InputChan *ic)
{
    this->sampleRate = sampleRate;
    this->devChanCountOut = 2;  // TODO
    this->devChanCountIn = 2;   // TODO
    this->outChans = oc;
    this->inChans = ic;

    log("Configured for %d in, %d out @ %d Hz\n", this->devChanCountIn, this->devChanCountOut, this->sampleRate);

}

XPlay::~XPlay()
{

}
