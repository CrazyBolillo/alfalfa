CC ::= xc8-cc
MCPU ::= 16f1823
CFLAGS ::= -mcpu=$(MCPU) -O2 -mwarn=-9
OBJS ::= main.p1 si5351.p1 lcd.p1

alfalfa.hex: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS)

main.p1: main.c config.h
	$(CC) $(CFLAGS) -c main.c

si5351.p1: si5351.c si5351.h
	$(CC) $(CFLAGS) -c si5351.c

lcd.p1: lcd.c lcd.h
	$(CC) $(CFLAGS) -c lcd.c

.PHONY: clean

clean:
	-rm *.hex *.hxl *.lst *.rlf *.s *.sym *.p1 *.d *.cmf *.sdb *.o *.elf *.coff
