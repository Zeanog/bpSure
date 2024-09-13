#***********************************************************************
#	Makefile script to generate bpSure object files - February 17, 2020
#***********************************************************************
ARMGNU ?= arm-none-eabi

AOPS = --warn --fatal-warnings -mcpu=arm1176jzf-s -march=armv6 \
	-mfpu=vfpv3 -mfloat-abi=hard

COPS = -Wall -O3 -nostdlib -nostartfiles -ffreestanding \
	-mcpu=arm1176jzf-s -mtune=arm1176jzf-s -mhard-float -mfpu=vfp
	
all : kernel.bin kernel.hex kernel.lst

GCC.OBJ = startup.o kernel.o library.o display.o peripheral.o math.o
	
startup.o : startup.s makefile
	$(ARMGNU)-as $(AOPS) startup.s -o $@

kernel.o : kernel.c makefile
	$(ARMGNU)-gcc $(COPS) -c kernel.c -o $@

library.o : library.c library.h makefile
	$(ARMGNU)-gcc $(COPS) -c library.c -o $@
	
display.o : OLED_display.c OLED_display.h makefile
	$(ARMGNU)-gcc $(COPS) -c OLED_display.c -o $@

peripheral.o : peripheral.c peripheral.h makefile
	$(ARMGNU)-gcc $(COPS) -c peripheral.c -o $@
	
math.o : math.c math.h makefile
	$(ARMGNU)-gcc $(COPS) -c math.c -o $@

kernel.elf : memmap $(GCC.OBJ)
	$(ARMGNU)-ld $(GCC.OBJ) -T memmap -o $@
	$(ARMGNU)-objdump -D kernel.elf > kernel.list

kernel.bin : kernel.elf
	$(ARMGNU)-objcopy kernel.elf -O binary $@
	
kernel.hex : kernel.elf
	$(ARMGNU)-objcopy kernel.elf -O ihex $@ 

kernel.lst : kernel.elf
	$(ARMGNU)-objdump -dSt $^ > $@
	@echo "----- RAM/Flash Usage -----"
	$(ARMGNU)-size $^
	
.PHONY : clean

clean : 
	-rm -rf $(BUILD)
	-rm -f $(TARGET)
	-rm -f $(LIST)
	-rm -f $(MAP)
