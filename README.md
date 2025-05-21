# Z8E Debugger


Z8E is a full-screen symbolic debugger for the Z80 CPU, known from the
former CP/M User Groups.

Main features:

* Screen-oriented.
* Animated debugging.
* Z80 and Z180 versions.
* Auxiliary terminal support.


**Version 4.3.3**

1. Configure auxiliary console for use with z80pack1.38 with
   modifications to iosim.c source code.

2. Wrote/stole code for program to function as remote auxiliary
   console on Mac Terminal. See cpmaux.c for shell aliases to
   get terminal window into raw mode (and back to SANE when done).

3. Fixed FIFO hang/block when opening (in iosim.c). You no longer
   need to run cpmaux first before starting z80pack cpmsim. This is
   not an issue with Z8E itself, but I run Z8E via cpmsim on my
   Apple iMac. I only need to run cpmaux when I will be using the
   auxiliary console.

**Version 4.3.2**

1. Fixes error when command line includes an unknown symbol filename
   with bias.  Code use to process ",bias" as next filename.  It now
   skips bias when unknown filename.

2. Added 'SLL' opcode (Shift Left Logical).

3. Can toggle display of main 8-bit registers in binary on 'J'
   full-screen via the new command "+" (plus symbol).

4. Fixed 'kdmp' (Display memory window on 'J' full-screen) to actually
   allow a maximum count value of 256 decimal.  Previously, the max
   was 255 even though the documentation said 256.  Note that the
   number of bytes displayed depends on the symbol size and the size
   of your screen.  The default count (if not given) is now 100h
   (256#) as documented.

5. If "virtterm equ true", then include xterm escape sequences in
   initial banner message to setup window size, title, location.
   Mac Terminal configured to emulate "xterm-256color".

6. Extensive cleanup of WordStar document (Z8E.WS), used to produce
   Z8E.MAN file.  Many comment lines added at beginning of file (using
   ".." comment dot-commands), for those intending to modify it.


**Version 4.2**

1. Fixes an obscure bug that affected reading of symbol table files
   that ended exactly at a record boundary without an explicit
   end-of-file character (a valid CP/M condition).

More details [here](http://p112.sourceforge.net/index.php?z8e).
