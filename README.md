# BBB3
Repo of my BeagleBone Black experiments

Contains:
- requirements and setup for remote execution and debug from a Linux host
- Eclipse setup
- Some libraries:
  - Posix utilities to simplify threads, mutexes, etc
  - Single Linked List (co I cant make sense of the docs for the existing Posix one)
  - gpiof: A factory pattern library using sysfs (OBSOLETE! replaced by libgpiod)
 - Some simple test apps:
   - Ye olde hello world
   - A threading example
   - A simple example using libgpiod to query the GPIO character devices 

All of the notes are kept in Jupyter notebooks in the notebooks directory
