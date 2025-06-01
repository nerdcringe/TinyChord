CC=avr-gcc
OBJCOPY=avr-objcopy
PORT=COM3
TARGET=attiny85

IDIR = include
CFLAGS = -I$(IDIR) -Os -mmcu=attiny85 -DF_CPU=16000000LU
ODIR = build

main.hex: main.elf
	$(OBJCOPY) -O ihex main.elf main.hex

main.elf: include/*.h src/*.c 
	$(CC) $(CFLAGS) include/*.h src/*.c -o main.elf 

flash: main.hex
	avrdude -c arduino_as_isp -p $(TARGET) -U -P $(PORT) flash:w:'main.hex':a 


#avr-gcc -I${workspaceFolder}/include src/*.c include/*.h -o main.elf -mmcu=attiny85 -DF_CPU=8000000LU -Os;
#avr-objcopy main.elf -O ihex main.hex;
#avrdude -c arduino_as_isp -p attiny85 -U flash:w:'main.hex':a -P PORT3",