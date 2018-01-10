# embedded-sht Embedded SHT Drivers

## Clone this repository
```
 git clone --recursive https://github.com/Sensirion/embedded-sht.git
```

## Repistory content
* embedded-common (submodule repository for common embedded driver stuff)
* *.c files (sources to build both sht3x and shtc1 drivers)
* *.h files (headers to build both sht3x and shtc1 drivers)
* *.py (helpers to restructure some files before release)

## Files to adjust (from embedded-common) You only need to touch the following
files:

* `sensirion_arch_config.h` (architecture specifics, you need to specify the 
integer sizes)

and depending on your i2c implementation one of the following:

* `sensirion_hw_i2c_implementation.c` (functions for hardware i2c
  communication if your platform supports that)
* `sensirion_sw_i2c_implementation.c` (functions for software i2c
  communication via GPIOs)

## Building the driver
1. Adjust sensirion_arch_config.h if you don't have the `<stdint.h>` header
file available.
2. Implement necessary functions in the `*_implementation.c` files:
3. make
4. make release (to get squashed files in a separate folder)

---

Please check the [embedded-common](https://github.com/Sensirion/embedded-common)
repository for further information and sample implementations.

---
