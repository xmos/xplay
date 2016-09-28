#include <iostream>
#include <thread>
#include <vector>
#include "optionparser.h"
#include "xplay.h"

struct Arg: public option::Arg
{
  static void printError(const char* msg1, const option::Option& opt, const char* msg2)
  {
    fprintf(stderr, "%s", msg1);
    fwrite(opt.name, opt.namelen, 1, stderr);
    fprintf(stderr, "%s", msg2);
  }

  static option::ArgStatus Unknown(const option::Option& option, bool msg)
  {
    if (msg) printError("Unknown option '", option, "'\n");
    return option::ARG_ILLEGAL;
  }

  static option::ArgStatus Required(const option::Option& option, bool msg)
  {
    if (option.arg != 0)
      return option::ARG_OK;

    if (msg) printError("Option '", option, "' requires an argument\n");
    return option::ARG_ILLEGAL;
  }

  static option::ArgStatus NonEmpty(const option::Option& option, bool msg)
  {
    if (option.arg != 0 && option.arg[0] != 0)
      return option::ARG_OK;

    if (msg) printError("Option '", option, "' requires a non-empty argument\n");
    return option::ARG_ILLEGAL;
  }

  static option::ArgStatus Numeric(const option::Option& option, bool msg)
  {
    char* endptr = 0;
    if (option.arg != 0 && strtol(option.arg, &endptr, 10)){};
    if (endptr != option.arg && *endptr == 0)
      return option::ARG_OK;

    if (msg) printError("Option '", option, "' requires a numeric argument\n");
    return option::ARG_ILLEGAL;
  }
};


enum  optionIndex { UNKNOWN, HELP, PLAYFILE, SAMPLERATE, PLAYTONE, RECFILE, LISTDEVICES, DEVICE};

const option::Descriptor usage[] =
{
 {UNKNOWN, 0, "", "",option::Arg::None, "USAGE: example [options]\n\n"
                                        "Options:" },
 {HELP, 0,"", "help",option::Arg::None, "  --help  \tPrint usage and exit" },
 {LISTDEVICES, 0,"l", "listdevices",option::Arg::None, "  --listDevicesi, -l  \tPrint available audio devices and exit" },
 {PLAYFILE, 0,"p","playfile",Arg::Required, "  --playfile, -p <arg> \tPlay audio from file <arg>" },
 {RECFILE, 0,"r","recordfile",Arg::Required, "  --recordfile, -r <arg> \tRecord audio to file <arg>" },
 {SAMPLERATE, 0,"s","samplerate",Arg::Numeric, "  --samplerate, -s  <arg> \tSet Sample Rate to <arg>"  },
 {PLAYTONE, 0,"t","playtone",Arg::Numeric, "  --playtone, -t <arg> \tPlay tone of freq <arg>"  },
 {DEVICE, 0,"d","device",Arg::Numeric, "  --device, -d <arg> \tUse device number <arg>"  },
 {UNKNOWN, 0, "", "",option::Arg::None, "\nExamples:\n"
                               "  xplay --playtone 1000\n"
                               "  xplay --playfile file.wav\n"
                               "  xplay --playfile play.wav --recordfile record.wav\n"},
 {0,0,0,0,0,0}
};

int main(int argc, char *argv[]) 
{
    int useWDM = 0;
    PaError err;

    argc-=(argc>0); argv+=(argc>0); // skip program name argv[0] if present
    option::Stats  stats(usage, argc, argv);
    std::vector<option::Option> options(stats.options_max);
    std::vector<option::Option> buffer(stats.buffer_max);
    option::Parser parse(usage, argc, argv, &options[0], &buffer[0]);

    if (parse.error())
        return 1;

    if (options[HELP] || argc == 0) 
    {
        option::printUsage(std::cout, usage);
        return 0;
    }

    for (option::Option* opt = options[UNKNOWN]; opt; opt = opt->next())
    {    
        std::cout << "Unknown option: " << std::string(opt->name,opt->namelen) << "\n";
    }

    for (int i = 0; i < parse.nonOptionsCount(); ++i)
    {
        std::cout << "Non-option #" << i << ": " << parse.nonOption(i) << "\n";
    }

    if(parse.nonOptionsCount() || options[UNKNOWN])
    {
        exit(1);
    }

    unsigned sampleRate= 44100;         /* Default freq */
    unsigned numChansOut = 2;           /* TODO, options */
    unsigned numChansIn = 2;            /* TODO, options */
    unsigned toneFreq = 0;              /* Freq of output tone */
    playmode_t playmode = PLAYMODE_SILENCE;
    recmode_t recmode = RECMODE_NONE;
    OutputChan *oc = NULL;
    InputChan *ic = NULL;
    const char * filename;
    const char * filename_rec;
    bool listDevices = false;
    int targetDevice = -1;

    if(options[SAMPLERATE])
    {
        sampleRate = std::atoi(options[SAMPLERATE].arg);
    }
    
    if(options[PLAYTONE])
    {
        playmode = PLAYMODE_TONE;
        toneFreq = atoi(options[PLAYTONE].arg);
    }
    
    if(options[PLAYFILE])
    {
        playmode = PLAYMODE_FILE;
        filename = options[PLAYFILE].arg;
    }
    
    if(options[RECFILE])
    {
        recmode = RECMODE_FILE;
        filename_rec = options[RECFILE].arg;
    }
 
    if(options[LISTDEVICES])
    {
        listDevices = true;
    }

    if(options[DEVICE])
    {
        targetDevice = atoi(options[DEVICE].arg);
    }

    err = Pa_Initialize();
    
    if( err != paNoError ) 
    {
        report_error("PortAudio error: %s\n", Pa_GetErrorText( err ) );
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
    
    if(targetDevice < 0)
    {
        /* Look for a device to use */
        for (int i = 0; i < Pa_GetDeviceCount(); i++) 
        {
            int curDevChanCountIn = Pa_GetDeviceInfo(device)->maxInputChannels;

            const char * name = Pa_GetDeviceInfo(i)->name;
            log("Found Device %d: %s\n", i, name);

            if(listDevices == false)
            {
                if (useWDM) 
                {
                    char wdmIn[] = "Line (XMOS";
                    char wdmOut[] = "Speakers (XMOS";
                    char *cmp = curDevChanCountIn == 0 ? wdmIn : wdmOut; 
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
        }
    }
    else
    {
        /* Use specified device */
        if(targetDevice < Pa_GetDeviceCount())
        {
            device = targetDevice;   
            const char * name = Pa_GetDeviceInfo(device)->name;
            log("Using Device %d: %s\n", device, name);
        }
        else
        {
            report_error("Specified device out of range\n");
            return 1;
        }
    }
      
    if(listDevices)
        return 0;

    if(device == -1) 
    {
        log("Warning: Cannot find XMOS or Thesycon ASIO driver\n");
        device = Pa_GetDefaultOutputDevice();
        
        const char * name = Pa_GetDeviceInfo(device)->name;
        log("Using Default Device (%d: %s)\n", device, name);
    }

    /* By default set number of channels to the devices channel count */
    numChansIn =  Pa_GetDeviceInfo(device)->maxInputChannels;
    numChansOut =  Pa_GetDeviceInfo(device)->maxOutputChannels;

    log("Device info: %d inputs, %d outputs @ %d Hz\n", numChansIn, numChansOut, sampleRate);

    switch(playmode)
    {
        case PLAYMODE_TONE:
            oc = new SineOutputChan(sampleRate, toneFreq, numChansOut);
            break;
        
        case PLAYMODE_FILE:
            oc = new FileOutputChan((char*) filename, numChansOut);
            break;
        
        default:
            break;

    }

    switch(recmode)
    {
        case RECMODE_FILE:
            if(numChansIn == 0)
            {
                report_error("Cannot record from selected device (has no input channels?)");
                return 1;
            }
            ic = new FileInputChan((char*) filename_rec, numChansIn, sampleRate);
            
            break;

        default:
            break;
    }

    XPlay xplay(sampleRate, oc, ic);

    /* TODO duration should be while(1) (i.e. delay 0) by default or cmd line opt */
    int duration = 100000;  
    return xplay.run(duration, device);
}
