xPlay
=====

Simple audio playing/recording example application

Dependancies
------------

 * libsndfile (http://www.mega-nerd.com/libsndfile/#Download)
 * portaudio (http://www.portaudio.com)
 * ASIO SDK (http://www.steinberg.net/en/company/developers.html)
 
 Building
 --------
 
 A *simple* makefile is provided. It expects a sandbox with the following directory structure:
 
 - xplay (this repo)
 - portaudio (port audio source)
 - asiosdk (ASIO SDK files)
 - libsndfile (libsndfile source)
 
Mac
~~~
 
   * Install xcode command like tools
   * In xplay/xplay run "make all"
   
Windows
~~~~~~~

The xplay source code uses C11 threads (uses features:  thread creation and management, mutex and conditional variables). One way of supporting this on Windows is as follows:
  
   * Download and install MSYS2 (https://sourceforge.net/projects/msys2/)
   * Open an MSYS2 command prompt (or the 32-bit or 64-bit command prompts if you plan on building 32-bit or 64-bit things) from the start menu entries
   * Install make (pacman -S make)
   * Install MinGW-w64 GCC e.g. :
      - pacman -S mingw-w64-i686-gcc
   



