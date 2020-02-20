#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
(c) Copyright 2018 Sensirion AG, Switzerland
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of Sensirion AG nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
"""

import math


# Generate a look-up table for the interpolation of the relative humidity to
# absolute humidity conversion within a specifig temperature region (e.g. -20°C
# to 70°C in steps of 10°C)
# As part of the output, the mean error over each discrete (t, rh) point within
# a region of interest is printed (See `quantify_ah_lut_error`)

def calc_ah(t, rh):
    """ Mathematically correct AH computation """
    return 216.7 * (
            (rh / 100. * 6.112 * math.exp(17.62 * t / (243.12 + t))) /
            (273.15 + t))


def gen_ah_lut(t_range):
    """ Generate the AH Look Up Table at 100%RH (0..100 scales linearly) """
    return [calc_ah(t, 100) for t in t_range]


def ah_lookup(ah_lut, t_lo, t_hi, temp, rh):
    if rh == 0:
        return 0

    t_step = (t_hi - t_lo) / (len(ah_lut) - 1)
    t = temp - t_lo
    i = int(t / t_step)
    rem = t % t_step

    if i >= len(ah_lut) - 1:
        return ah_lut[-1] * (rh / 100.)

    if rem == 0:
        return ah_lut[i] * (rh / 100.)
    return (ah_lut[i] + (ah_lut[i + 1] - ah_lut[i]) * rem / t_step) * rh / 100.


def c_ah_lookup(ah_lut, t_lo, t_hi, temp, rh):
    """ Fixed point implementation (for C conversion)
    The only non-fixed point aspect is the final division by 1000. for
    comparison with the floating point version """
    if rh == 0:
        return 0

    rh = int(rh * 1000)
    norm_humi = (rh * 82) >> 13
    temp = int(temp * 1000)
    t_lo = int(t_lo * 1000)
    t_hi = int(t_hi * 1000)

    t_step = int((t_hi - t_lo) / (len(ah_lut) - 1))
    t = temp - t_lo
    i = int(t / t_step)
    rem = t % t_step

    if i >= len(ah_lut) - 1:
        return (ah_lut[-1] * norm_humi) / 1000.
    if rem == 0:
        return (ah_lut[i] * norm_humi) / 1000.

    return ((ah_lut[i] + (ah_lut[i + 1] - ah_lut[i]) * rem / t_step) * norm_humi) / 1000.


def quantify_ah_lut_error(ah_lut, t_lo, t_hi, t_range, rh_range):
    s_float = 0
    s_int = 0
    for t in t_range:
        for rh in rh_range:
            s_float += abs(calc_ah(t, rh) - ah_lookup(ah_lut, t_lo, t_hi, t, rh))
            s_int += abs(calc_ah(t, rh) - c_ah_lookup(ah_lut, t_lo, t_hi, t, rh))
    div = (len(t_range) * len(rh_range))
    return (s_float / div, s_int / div)


if __name__ == '__main__':
    T_LO = -20
    T_HI = 70
    T_STEP = 10
    lut = gen_ah_lut(range(T_LO, T_HI+1, T_STEP))
    print("The average absolute error over the range in 1°C steps is:")
    print("error avg(abs(ah(t,rh) - lookup(t,rh))) for T: -20..45, RH: 20..80 (float, int)")
    print(quantify_ah_lut_error(lut, T_LO, T_HI, range(T_LO, 45, 1), range(20, 80, 1)))

    # print("Comparison of the absolute humdity to the lookup-table value over a
    #       "selected set of (T, %RH) pairs")
    # for t, rh in [(-20, 0), (-20, 100), (-19.9, 100), (-15.1, 90), (-15, 90),
    #         (-14.9, 90), (20, 50), (20, 100), (22.5, 50),  (55, 70),  (65, 70), (75, 70), (140, 50)]:
    #     print("AH({}, {}) = {} ~ {} ~ {}".format(t, rh, calc_ah(t, rh),
    #                                         ah_lookup(lut, T_LO, T_HI, t, rh),
    #                                         c_ah_lookup(lut, T_LO, T_HI, t, rh)))

    print("")
    print("C Source:")
    print("""
#define T_LO ({t_low})
#define T_HI ({t_high})
static const uint32_t AH_LUT_100RH[] = {{{ah_lut_100rh}}};"""
          .format(t_low=T_LO, t_high=T_HI,
                  ah_lut_100rh=', '.join(['{:.0f}'.format(ah * 1000)
                                          for ah in lut])))
