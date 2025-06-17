//=============================================================================
// [Filename]       encoder.cpp
// [Project]        -
// [Author]         -
// [Language]       C++ 20
// [Created]        March 2025
// [Modified]       -
// [Description]    C++ code emulate encoder
// [Notes]          -
// [Status]         devel
// [Revisions]      -
//=============================================================================

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdint>

// Structure to hold the encoder's outputs.
struct EncoderResult {
    uint16_t data; // 10-bit output packed in the lower 10 bits.
    bool     disp;
};

// The encoder function implementing the 5B/6B encoding.
EncoderResult encode(uint16_t datain, bool dispin) {
  // Extract individual bits from datain (only lower 9 bits are used).
  bool ai = (datain >> 0) & 1;
  bool bi = (datain >> 1) & 1;
  bool ci = (datain >> 2) & 1;
  bool di = (datain >> 3) & 1;
  bool ei = (datain >> 4) & 1;
  bool fi = (datain >> 5) & 1;
  bool gi = (datain >> 6) & 1;
  bool hi = (datain >> 7) & 1;
  bool ki = (datain >> 8) & 1;

  // Intermediate signals from the first section.
  bool aeqb = (ai && bi) || (!ai && !bi);
  bool ceqd = (ci && di) || (!ci && !di);
  bool l22   = (ai && bi && !ci && !di) ||
    (ci && di && !ai && !bi) ||
    (!aeqb && !ceqd);
  bool l40   = ai && bi && ci && di;
  bool l04   = !ai && !bi && !ci && !di;
  bool l13   = (!aeqb && !ci && !di) ||
    (!ceqd && !ai && !bi);
  bool l31   = (!aeqb && ci && di) ||
    (!ceqd && ai && bi);

  // 5B/6B encoding signals.
  bool ao = ai;
  bool bo = (bi && !l40) || l04;
  bool co = l04 || ci || (ei && di && !ci && !bi && !ai);
  // "do" is a reserved word in C++, so we use do_ instead.
  bool do_ = di && !(ai && bi && ci);
  bool eo = (ei || l13) && !(ei && di && !ci && !bi && !ai);
  bool io = (l22 && !ei) ||
    (ei && !di && !ci && !(ai && bi)) || // D16, D17, D18
    (ei && l40) ||
    (ki && ei && di && ci && !bi && !ai) || // K.28
    (ei && !di && ci && !bi && !ai);

  // Disparity and encoding decisions.
  bool pd1s6 = (ei && di && !ci && !bi && !ai) || (!ei && !l22 && !l31);
  bool nd1s6 = ki || (ei && !l22 && !l13) || (!ei && !di && ci && bi && ai);
  bool ndos6 = pd1s6;
  bool pdos6 = ki || (ei && !l22 && !l13);

  // The alternate coding decision.
  bool alt7 = fi && gi && hi &&
    (ki || (dispin ? (!ei && di && l31) : (ei && !di && l13)));

  bool fo = fi && !alt7;
  bool go = gi || (!fi && !gi && !hi);
  bool ho = hi;
  bool jo = (!hi && (gi ^ fi)) || alt7;

  bool nd1s4 = fi && gi;
  bool pd1s4 = (!fi && !gi) || (ki && ((fi && !gi) || (!fi && gi)));
  bool ndos4 = (!fi && !gi);
  bool pdos4 = fi && gi && hi;

  // The illegal K-code flag (not used further in this example).
  bool illegalk = ki &&
    (ai || bi || !ci || !di || !ei) &&  // not K28.0->7
    (!fi || !gi || !hi || !ei || !l31);   // not K23/27/29/30.7

  // Determine whether to complement for 6-bit encoding.
  bool compls6 = (pd1s6 && !dispin) || (nd1s6 && dispin);
  bool disp6    = dispin ^ (ndos6 || pdos6);
  bool compls4 = (pd1s4 && !disp6) || (nd1s4 && disp6);
  bool dispout  = disp6 ^ (ndos4 || pdos4);

  // Build the 10-bit dataout.
  // The order is: { (jo^compls4), (ho^compls4), (go^compls4), (fo^compls4),
  //                (io^compls6), (eo^compls6), (do_^compls6), (co^compls6),
  //                (bo^compls6), (ao^compls6) }
  bool bit9 = jo ^ compls4;
  bool bit8 = ho ^ compls4;
  bool bit7 = go ^ compls4;
  bool bit6 = fo ^ compls4;
  bool bit5 = io ^ compls6;
  bool bit4 = eo ^ compls6;
  bool bit3 = do_ ^ compls6;
  bool bit2 = co ^ compls6;
  bool bit1 = bo ^ compls6;
  bool bit0 = ao ^ compls6;

  uint16_t dataout = 0;
  dataout |= (bit0 ? 1 : 0) << 9;
  dataout |= (bit1 ? 1 : 0) << 8;
  dataout |= (bit2 ? 1 : 0) << 7;
  dataout |= (bit3 ? 1 : 0) << 6;
  dataout |= (bit4 ? 1 : 0) << 5;
  dataout |= (bit5 ? 1 : 0) << 4;
  dataout |= (bit6 ? 1 : 0) << 3;
  dataout |= (bit7 ? 1 : 0) << 2;
  dataout |= (bit8 ? 1 : 0) << 1;
  dataout |= (bit9 ? 1 : 0) << 0;

  return EncoderResult{ dataout, dispout };
}


int main() {

  EncoderResult result = {0b0, false};

  int i = 0;
  for (uint16_t datain = 0b0; datain <= 0b1010; ++datain) {
    std::cout << "EVENT: " << i << "\n";
    std::cout << "input: " << datain << ", disp: " << result.disp << "\n";

    result = encode(datain, result.disp);
    // Display the dataout as a 10-bit binary string.
    std::cout << "dataout: 0b";
    for (int i = 9; i >= 0; i--) {
        std::cout << ((result.data >> i) & 1);
    }
    std::cout << "\ndispout: " << result.disp << "\n" << std::endl;
    i++;
  }

  return 0;
}

