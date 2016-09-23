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


enum  optionIndex { UNKNOWN, HELP, PLAYFILE, SAMPLERATE, PLAYTONE, RECFILE};

const option::Descriptor usage[] =
{
 {UNKNOWN, 0, "", "",option::Arg::None, "USAGE: example [options]\n\n"
                                        "Options:" },
 {HELP, 0,"", "help",option::Arg::None, "  --help  \tPrint usage and exit." },
 {PLAYFILE, 0,"p","playfile",Arg::Required, "  --playfile, -o  \tPlay File." },
 {RECFILE, 0,"r","recordfile",Arg::Required, "  --recordfile, -o  \tRecord File." },
 {SAMPLERATE, 0,"s","samplerate",Arg::Numeric, "  -r <arg>, \t--required=<arg>  \tMust have an argument."  },
 {PLAYTONE, 0,"t","playtone",Arg::Numeric, "  -r <arg>, \t--required=<arg>  \tMust have an argument."  },
 {UNKNOWN, 0, "", "",option::Arg::None, "\nExamples:\n"
                               "  example --unknown -- --this_is_no_option\n"
                               "  example -unk --plus -ppp file1 file2\n" },
 {0,0,0,0,0,0}
};

int main(int argc, char *argv[]) 
{
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
    OutputChan *oc;
    //InputChan *ic;
    const char * filename;
    const char * filename_rec;

    if(options[SAMPLERATE])
    {
        sampleRate = atoi(options[SAMPLERATE].arg);
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
   
    /* TODO we might want multple output modes for different channels, current file play is only one */
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
        //    ic = new FileInputChan((char*) filename_rec, numChansIn);
            break;
        default:
            break;
    }
 

    XPlay xplay(sampleRate, oc /*, ic */);

    /* TODO duration should be while(1) (i.e. delay 0) by default or cmd line opt */
    int duration = 100000;  
    return xplay.run(duration);
}
