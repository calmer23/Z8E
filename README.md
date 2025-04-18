# Z8E Debugger


Z8E is a full-screen symbolic debugger for the Z80 CPU, known from the
former CP/M User Groups.

Main features:

* Screen-oriented.
* Animated debugging.
* Z80 and Z180 versions.
* Auxiliary terminal support.


**Version 4.3.2**

1. Fixes error when command line includes an unknown symbol filename
   with bias.  Code use to process ",bias" as next filename.  It now
   skips bias when unknown filename.

2. Added 'SLL' opcode (Shift Left Logical).

3. Can toggle display of main 8-bit registers in binary on 'J'
   full-screen.

4. Fixed 'kdmp' (Display memory window on 'J' full-screen) to actually
   allow a maximum count value of 256 decimal.  Previously, the max
   was 255 even though the documentation said 256.  Note that the
   number of bytes displayed depends on the symbol size and the size
   of your screen.  The default count (if not given) is now 100h
   (256#) as documented.

5. If "virtterm equ true", then include x-term escape sequences in
   initial banner message to setup window size, title, location.

6. Extensive cleanup of WordStar document (Z8E.WS), used to produce
   Z8E.MAN file.  Many comment lines added at beginning of file (using
   ".." comment dot-commands), for those intending to modify it.


**Version 4.2**

1. Fixes an obscure bug that affected reading of symbol table files
   that ended exactly at a record boundary without an explicit
   end-of-file character (a valid CP/M condition).

More details [here](http://p112.sourceforge.net/index.php?z8e).
