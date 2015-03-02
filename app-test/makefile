
#CC	= /usr/local/arm/4.2.2-eabi/usr/bin/arm-linux-gcc
#LD	= /usr/local/arm/4.2.2-eabi/usr/bin/arm-linux-gcc
#CC	= /opt/arm/usr/bin/arm-linux-gcc
#LD	= /opt/arm/usr/bin/arm-linux-gcc
CC	=/opt/4.4.3/bin/arm-linux-gcc 
LD	=/opt/4.4.3/bin/arm-linux-gcc 

CFLAGS =  -Wall -g 
SRC    = fbtest.c 
OBJ    = fbtest.o 
EXE    = fb_test


all:$(OBJ)
	$(CC)  -o $(EXE) $(OBJ)
	#cp -f $(EXE) /tftpboot
	cp -f $(EXE) /home/lee/8500-working/rootfs/root-nfs 
	
%o:%c 
	$(CC) $(CFLAGS) -c $<
		
clean:
	rm -f *.o  $(EXE)
