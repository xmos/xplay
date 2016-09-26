xPlay
=====

Simple audio playing/recording example application.

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
 
   * Install xcode command line tools
   * In xplay/xplay run "make all"
   
Windows
~~~~~~~

The xplay source code uses C11 threads (uses features:  thread creation and management, mutex and conditional variables). One way of supporting this on Windows is as follows:
  
   * Download and install MSYS2 (msys2.github.io)
   * Open an MSYS2 command prompt (or the 32-bit or 64-bit command prompts if you plan on building 32-bit or 64-bit things) from the start menu entries
   * Install make (pacman -S make)
   * Install a toolchain e.g:
      - pacman -S mingw-w64-i686-gcc
   * Move into xplay/xplay and use the command
      - make all
   
Note, if GCC isn't found, you probably are not running the  correct command prompt  for the tools you installed


