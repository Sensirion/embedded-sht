include default_config.inc

.PHONY: clean

all: $(sensirion_humidity_conversion_sources:.c=.o)

%.o: %.c %.h
	$(CC) $(CFLAGS) -shared -o $@ $<

clean:
	$(RM) $(obj)
