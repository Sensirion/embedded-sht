include default_config.inc

.PHONY: clean

obj = sensirion_temperature_unit_conversion.o \
      sensirion_humidity_conversion.o

all: $(obj)

sensirion_temperature_unit_conversion.o: $(sensirion_temperature_unit_conversion_sources)
	$(CC) $(CFLAGS) -shared -o $@ $<

sensirion_humidity_conversion.o: $(sensirion_humidity_conversion_sources)
	$(CC) $(CFLAGS) -shared -o $@ $<

clean:
	$(RM) $(obj)
