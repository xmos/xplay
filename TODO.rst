
TODO
====

 * Allow input/output buffers to be passed to plugin even if stream is not being used
 * Make plugin system work on windows
 * Add monitor function (this could be a simple plugin example)
 * Make buffer sizes related to portaudio latency (frames per buffer)
 * Playback channel count handling 
 * Allow record and play to/from separate devices 
 * Add option to use first xmos device found 
 * Tidy up options/arguments
 * Add command line options for latency settings
 * Channel selection (when play/rec chan count < device chan count)

Done
====
 * Sound device selection (use default if not specified) 
 * Add list sound devices options
 * Record channel count handling 
 * Add (basic) plugin system
 * Handle end of play file gracefully (i.e. exit)
