# BBB3
Repo of my BeagleBone Black experiments

Contains:
- requirements and setup for remote execution and debug from a Linux host
- Eclipse setup
- Some libraries:
  - Posix utilities to simplify threads, mutexes, etc
  - Single Linked List (cos I cant make sense of the complex docs for the existing Posix one)
 - Some simple test apps:
   - Ye olde hello world
   - A threading example
   - A simple example using libgpiod C interface to query the GPIO character devices
   - Example using the libgpiod .CPP bindings 

All of the notes are kept in Jupyter notebooks in the notebooks directory
