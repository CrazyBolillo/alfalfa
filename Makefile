CC = xc8-cc
OUTDIR = build
MCPU = 16f1823
CFLAGS = -mcpu=$(MCPU) -O2 -mwarn=-9
OBJS = $(addprefix $(OUTDIR)/, main.p1 si5351.p1 lcd.p1)

all: $(OUTDIR)/alfalfa.hex
.PHONY: all clean

$(OUTDIR)/alfalfa.hex: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(OUTDIR)/alfalfa.hex

$(OUTDIR)/%.p1 : %.c %.h | $(OUTDIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(OUTDIR):
	mkdir $(OUTDIR)

clean:
	-rm -dr $(OUTDIR)
