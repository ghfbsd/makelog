FC=g77
FC=gfortran
FFLAGS = -g -fbounds-check
GROUP = seismo

OBJS = iosubs.o possubs.o timesubs.o datasubs.o


makelog: makelog.o datspan.o ${OBJS}
	$(FC) ${FFLAGS} -o makelog makelog.o datspan.o ${OBJS}
	chgrp ${GROUP} makelog ; chmod go+rx makelog

cdscan: cdscan.o iosubs.o possubs.o
	$(FC) -o cdscan cdscan.o iosubs.o possubs.o

seedscan: seedscan.o iosubs.o possubs.o
	$(FC) -o seedscan seedscan.o iosubs.o possubs.o

mssplit: mssplit.o
	$(CC) ${FFLAGS} -o mssplit mssplit.o

all: cdscan makelog mssplit

cdscan.o iosubs.o possubs.o makelog.o: cdscan.io.com
makelog.o: cdscan.sta.com

distclean: clean
	/bin/rm -f makelog cdscan mssplit .gdb_history makelog.log

clean:
	/bin/rm -f *.o core
