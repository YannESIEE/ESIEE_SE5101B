#generate .o
obj-m	   := any.o

PATH := /opt/x86_64/gcc/gcc-3.4.4/bin:$(PATH)
KIT = arcom1.esiee.fr

#KIT = arcom17.esiee.fr =BANC 2 ???
#ARCOM17 : butée +35° : 2771 // butée -35° 1318
#ARCOM17 : butée +.6m : 3772 // butée -.6m 421

#Gauche, proche bord = 2

#3
#Table de gauche, celui du le plus loin du bord 12/10
#ARCOM7 : butée +35° : 2819 // butée -35° 1217		
#ARCOM7 : butée +.6m : 4094 // butée -.6m 3
 
#4
#Table de droite, celui du le plus loin du bord 12/10
#ARCOM5 : butée +35° : 2760 // butée -35° 1291
#ARCOM5 : butée +.6m : 4095 // butée -.6m 0--11

#5
#Table de droite, celui du le plus proche du bord 12/10
#ARCOM11 : butée +35° : 2800 // butée -35° 1235		
#ARCOM11 : butée +.6m : 4095 // butée -.6m 0



CURRENT = 2.6.16.14-arcom1
KDIR = /usr/lib/arcom/lib/modules/${CURRENT}/build
PWD = $(shell pwd)
EXTRA_CFLAGS += -I/usr/lib/arcom/realtime/include -Wall -ffast-math -mhard-float
default:
	@echo --------------------------------------
	@echo PWD = $(PWD)
	@echo CURRENT = $(CURRENT)
	@echo KDIR = $(KDIR)
	i386 -B --uname-2.6 make -C $(KDIR) M=$(PWD) modules

clean:
	-rm -f *.o any*.ko .*.cmd .*.flags *.mod.c *~
	-rm -r .tmp*
send:
	scp *.ko runarcom arcom@$(KIT):/home/arcom
	ssh arcom@$(KIT)

a:
	-rm -f *.o any*.ko .*.cmd .*.flags *.mod.c *~
	-rm -r .tmp*
	@echo Cleaned
	@echo

	@echo PWD = $(PWD)
	@echo CURRENT = $(CURRENT)
	@echo KDIR = $(KDIR)
	i386 -B --uname-2.6 make -C $(KDIR) M=$(PWD) modules
	@echo Built
	@echo
	
	
	scp *.ko runarcom arcom@$(KIT):/home/arcom
	@echo
	ssh arcom@$(KIT)

b:
	-rm -f *.o any*.ko .*.cmd .*.flags *.mod.c *~
	-rm -r .tmp*
	@echo
	@echo PWD = $(PWD)
	@echo CURRENT = $(CURRENT)
	@echo KDIR = $(KDIR)
	i386 -B --uname-2.6 make -C $(KDIR) M=$(PWD) modules
	@echo
	
	
s:
	scp *.ko runarcom arcom@$(KIT):/home/arcom
	@echo
	ssh arcom@$(KIT)

a18:
	-rm -f *.o any*.ko .*.cmd .*.flags *.mod.c *~
	-rm -r .tmp*
	@echo Cleaned
	@echo

	@echo PWD = $(PWD)
	@echo CURRENT = $(CURRENT)
	@echo KDIR = $(KDIR)
	i386 -B --uname-2.6 make -C $(KDIR) M=$(PWD) modules
	@echo Built
	@echo
	
	
	scp *.ko runarcom arcom@arcom18.esiee.fr:/home/arcom
	@echo
	ssh arcom@arcom18.esiee.fr

a16:
	-rm -f *.o any*.ko .*.cmd .*.flags *.mod.c *~
	-rm -r .tmp*
	@echo Cleaned
	@echo

	@echo PWD = $(PWD)
	@echo CURRENT = $(CURRENT)
	@echo KDIR = $(KDIR)
	i386 -B --uname-2.6 make -C $(KDIR) M=$(PWD) modules
	@echo Built
	@echo
	
	
	scp *.ko runarcom arcom@arcom16.esiee.fr:/home/arcom
	@echo
	ssh arcom@arcom16.esiee.fr

a1:
	-rm -f *.o any*.ko .*.cmd .*.flags *.mod.c *~
	-rm -r .tmp*
	@echo Cleaned
	@echo

	@echo PWD = $(PWD)
	@echo CURRENT = $(CURRENT)
	@echo KDIR = $(KDIR)
	i386 -B --uname-2.6 make -C $(KDIR) M=$(PWD) modules
	@echo Built
	@echo
	
	
	scp *.ko runarcom arcom@arcom1.esiee.fr:/home/arcom
	@echo
	ssh arcom@arcom1.esiee.fr
