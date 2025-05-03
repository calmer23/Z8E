/* **************************************************
   Compile and create executable into  ~/bin/cpmaux

   Define shell aliases like the following so that your terminal
   session is using RAW mode for STDIN and STDOUT:

      alias cpmaux='(stty raw && ~/bin/cpmaux) ; stty sane ; echo Resetting terminal to SANE'
      alias cpmsim='~/z80pack1.38/cpmsim/cpmsim'

   Run cpmaux first (before you launch the cpmsim program).

   Run cpmsim to start your CP/M simulation.  (You created the executable already, right?)
   The simulation will pause before the "A> " prompt if cpmaux is not running already.

   To terminate the cpmaux program, type:  CNTL-\  (0x1C, file separator).

 */

// #define VOL2		/* show me the status as we go */

// #include "unp.h"

/* Miscellaneous constants */
#define	MAXLINE		4096	/* max text line length */
#define	BUFFSIZE	8192	/* buffer size for reads and writes */


#define	max(a,b)	((a) > (b) ? (a) : (b))

// /* from iosim
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
//#include "sim.h"
//#include "simglb.h"
//#include "simbdos.h"
//#include "memsim.h"
// */

static int auxin;		/* fd for pipe "auxin" */
static int auxout;		/* fd for pipe "auxout" */


int
main(int argc, char **argv)
{
  struct stat sbuf;

  if (stat("/tmp/.z80pack", &sbuf) != 0)	/* check if /tmp/.z80pack exists */
    mkdir("/tmp/.z80pack", 0777);		/* no, create it */
  /* and then the pipes */
  if (stat("/tmp/.z80pack/cpmsim.auxin", &sbuf) != 0)
    mkfifo("/tmp/.z80pack/cpmsim.auxin", 0666);
  if (stat("/tmp/.z80pack/cpmsim.auxout", &sbuf) != 0)
    mkfifo("/tmp/.z80pack/cpmsim.auxout", 0666);

  if ((auxout = open("/tmp/.z80pack/cpmsim.auxout", O_RDONLY)) == -1) {
    perror("can't open pipe auxout");
    exit(1);
  }

  if ((auxin = open("/tmp/.z80pack/cpmsim.auxin", O_WRONLY)) == -1) {
    perror("can't open pipe auxin");
    exit(1);
  }

  fprintf(stderr, "\n\n\r\tTo EXIT this program, press  CNTL-\\  \n\n\r");


  int	maxfdp1, val, stdineof;
  ssize_t	n, nwritten;
  fd_set	rset, wset;
  char	to[MAXLINE], fr[MAXLINE];
  char	*toiptr, *tooptr, *friptr, *froptr;

  val = fcntl(auxin, F_GETFL, 0);
  fcntl(auxin, F_SETFL, val | O_NONBLOCK);

  val = fcntl(auxout, F_GETFL, 0);
  fcntl(auxout, F_SETFL, val | O_NONBLOCK);

  val = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, val | O_NONBLOCK);

  val = fcntl(STDOUT_FILENO, F_GETFL, 0);
  fcntl(STDOUT_FILENO, F_SETFL, val | O_NONBLOCK);

  toiptr = tooptr = to;	/* initialize buffer pointers */
  friptr = froptr = fr;
  stdineof = 0;

  maxfdp1 = max(max(max(STDIN_FILENO, STDOUT_FILENO), auxin), auxout) + 1;

  for ( ; ; ) {
    FD_ZERO(&rset);
    FD_ZERO(&wset);

    if (stdineof == 0 && toiptr < &to[MAXLINE])
      FD_SET(STDIN_FILENO, &rset);	/* read from stdin */
    if (friptr < &fr[MAXLINE])
      FD_SET(auxout, &rset);		/* read from FIFO auxout */

    if (tooptr != toiptr)
      FD_SET(auxin, &wset);		/* write to FIFO auxin */
    if (froptr != friptr)
      FD_SET(STDOUT_FILENO, &wset);	/* data to write to stdout */

    select(maxfdp1, &rset, &wset, NULL, NULL);
    /* ====================== */
    if (FD_ISSET(STDIN_FILENO, &rset)) {
      if ( (n = read(STDIN_FILENO, toiptr, &to[MAXLINE] - toiptr)) < 0) {
	if (errno != EWOULDBLOCK)
	  perror("read error on stdin");
      } else if (n == 0) {
#ifdef	VOL2
fprintf(stderr, "EOF on stdin\n");
#endif
	stdineof = 1;		/* all done with stdin */
	if (tooptr == to)
	  break;
      } else {
#ifdef	VOL2
fprintf(stderr, "read %ld bytes from stdin\n", n);
#endif
	if (*toiptr == 0x1c) {	/* CNTRL-\  is terminate program */
	  break;
	}
	toiptr += n;		/* # just read */
	FD_SET(auxin, &wset);	/* try and write to socket below */
      }
    }
    /* ====================== */
    if (FD_ISSET(auxout, &rset)) {
      if ( (n = read(auxout, friptr, &fr[MAXLINE] - friptr)) < 0) {
	if (errno != EWOULDBLOCK)
	  perror("read error on auxout");
      } else if (n == 0) {
#ifdef	VOL2
fprintf(stderr, "EOF on auxout\n");
#endif
	if (stdineof)
	  break;
	else {
	  perror("server terminated prematurely");
	  break;
	}
      } else {
#ifdef	VOL2
fprintf(stderr, "read %ld bytes from auxout\n", n);
#endif
	friptr += n;			/* # just read */
	FD_SET(STDOUT_FILENO, &wset);	/* try and write below */
      }
    }
    /* ====================== */
    if (FD_ISSET(STDOUT_FILENO, &wset) && ( (n = friptr - froptr) > 0)) {
      if ( (nwritten = write(STDOUT_FILENO, froptr, n)) < 0) {
	if (errno != EWOULDBLOCK)
	  perror("write error to stdout");
      } else {
#ifdef	VOL2
fprintf(stderr, "wrote %ld bytes to stdout\n", nwritten);
#endif
	froptr += nwritten;	/* # just written */
	if (froptr == friptr)
	  froptr = friptr = fr;	/* back to beginning of buffer */
      }
    }
    /* ====================== */
    if (FD_ISSET(auxin, &wset) && ( (n = toiptr - tooptr) > 0)) {
      if ( (nwritten = write(auxin, tooptr, n)) < 0) {
	if (errno != EWOULDBLOCK)
	  perror("write error to auxin");
      } else {
#ifdef	VOL2
fprintf(stderr, "wrote %ld bytes to auxin\n", nwritten);
#endif
	tooptr += nwritten;	/* # just written */
	if (tooptr == toiptr) {
	  toiptr = tooptr = to;	/* back to beginning of buffer */
	  if (stdineof)
	    break;
	}
      }
    }
  }
  close(auxin);
  close(auxout);
  fprintf(stderr,"\n\n\rProgram terminated.\n\r");
  exit(0);	/* normal termination */
}
