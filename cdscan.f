C     Program to scan SEED files and print out blockette numbers.  The name
C     comes from the fact that it was developed to dump the GEOSCOPE SEED CDs
C     that IPGP started sending out in the 1990s.
C
C     As command line argument, give SEED file path name.
C     Options:  -d - dump blockettes
C
C     By George Helffrich, U. Bristol, 1994-2006
      program cdscan
      integer getvol,getbkt
      character posstr*16
      include 'cdscan.io.com'
      character cdname*256
      character bkette*(MAXBL)
      integer pos(2), bktype, bklen
      character posn*16, timbeg*23, timend*23
      logical odat

      cdname = ' '
      odat = .false.
      do i=1,iargc()
	 call getarg(i,posn)
	 if (posn(1:1) .eq. '-') then
	    if (posn .eq. '-d') then
	       odat = .true.
	    else
	       write(0,*) '**Bad option: ',posn(1:index(posn,' ')-1)
	       stop
	    endif
	 else
            call getarg(i,cdname)
	 endif
      enddo
      if (cdname .eq. ' ') stop '**No CD or file provided.'

      if (getvol(cdname) .ne. 0) then
	 stop '**Unable to open CD SEED volume.'
      endif

      call posset(pos,1,8)
      if (getbkt(pos,bkette) .le. 0) stop '**Can''t read vol. hdr.'
      timbeg = bkette(14:14+len(timbeg))
      timend = bkette(14+index(timbeg,'~'):)
      write(6,9001) bkette(1:3),bkette(8:11),
     +   lrecl,
     +   timbeg(1:index(timbeg,'~')-1),
     +   timend(1:index(timend,'~')-1)

10    continue
	 ios = getbkt(pos,bkette)
	 if (ios.eq.0) go to 9000
	 posn = posstr(pos)
	 if (getbkt(pos,bkette) .lt. 0) then
	    write(*,*) '**Error reading block ',posn
	    go to 9000
	 endif
	 if (0 .ne. index('DRQM',bkette(7:7))) then
	    bklen = ios
	    write(6,9004) posn,bklen
	 else
	    read(bkette,'(i3,i4)',iostat=ios) bktype,bklen
	    if (ios .ne. 0) write(0,9003) posn,bkette(1:8)
	    write(6,9002) posn,bktype,bklen
	    if (odat) then
	       lenb = getbkt(pos,bkette)
	       if (lenb .le. 0) stop '**Error/EOF reading blockette data'
	       ioff = 0
	       do i=1,lenb/64
		  write(*,'(i5,1x,a)') ioff,bkette(1+ioff:64+ioff)
		  ioff = ioff + 64
	       enddo
	       if (ioff .lt. lenb) then
		  write(*,'(i5,1x,a)') ioff,bkette(1+ioff:lenb)
	       endif
	    endif
	 endif
	 call posadd(pos,bklen)
      go to 10

9000  continue
      close(iucd)

9001  format(1x,'Vol. header: Type: ',a3,' Version ',a,' lrecl ',i5,/,
     +   1x,'Time span: ',2(a,1x))
9002  format(1x,a,' type ',i3,' length ',i4)
9003  format(1x,'**Funny blockette format (',a,'): ',a)
9004  format(1x,a,' type dat length ',i4)
      end
