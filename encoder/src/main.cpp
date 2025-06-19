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
#include <bitset>

// Structure to hold the encoder's outputs.
struct EncoderResult {
    uint16_t data; // 10-bit output packed in the lower 10 bits.
    bool     disp;
};

// Decoder struct
struct DecodeResult {
    uint16_t data;        // Decoded 8-bit data
    bool disp;            // Output running disparity (false = negative, true = positive)    
    bool codeError;       // True if illegal 10b pattern
    bool disparityError;  // True if disparity error
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

  // uint16_t dataout = 0;
  // dataout |= (bit0 ? 1 : 0) << 9;
  // dataout |= (bit1 ? 1 : 0) << 8;
  // dataout |= (bit2 ? 1 : 0) << 7;
  // dataout |= (bit3 ? 1 : 0) << 6;
  // dataout |= (bit4 ? 1 : 0) << 5;
  // dataout |= (bit5 ? 1 : 0) << 4;
  // dataout |= (bit6 ? 1 : 0) << 3;
  // dataout |= (bit7 ? 1 : 0) << 2;
  // dataout |= (bit8 ? 1 : 0) << 1;
  // dataout |= (bit9 ? 1 : 0) << 0;

  uint16_t dataout = 0;
  dataout |= (bit9 ? 1 : 0) << 9;
  dataout |= (bit8 ? 1 : 0) << 8;
  dataout |= (bit7 ? 1 : 0) << 7;
  dataout |= (bit6 ? 1 : 0) << 6;
  dataout |= (bit5 ? 1 : 0) << 5;
  dataout |= (bit4 ? 1 : 0) << 4;
  dataout |= (bit3 ? 1 : 0) << 3;
  dataout |= (bit2 ? 1 : 0) << 2;
  dataout |= (bit1 ? 1 : 0) << 1;
  dataout |= (bit0 ? 1 : 0) << 0;

  return EncoderResult{ dataout, dispout };
}

DecodeResult decode(uint16_t datin, bool disprtin) {
  bool ai = (datin >> 0) & 1;
  bool bi = (datin >> 1) & 1;
  bool ci = (datin >> 2) & 1;
  bool di = (datin >> 3) & 1;
  bool ei = (datin >> 4) & 1;
  bool ii = (datin >> 5) & 1;
  bool fi = (datin >> 6) & 1;
  bool gi = (datin >> 7) & 1;
  bool hi = (datin >> 8) & 1;
  bool ji = (datin >> 9) & 1;

  bool aeqb = (ai && bi) || (!ai && !bi);
  bool ceqd = (ci && di) || (!ci && !di);
  bool p22 = (ai && bi && !ci && !di) ||
             (ci && di && !ai && !bi) ||
             (!aeqb && !ceqd);
  bool p13 = (!aeqb && !ci && !di) ||
             (!ceqd && !ai && !bi);
  bool p31 = (!aeqb && ci && di) ||
             (!ceqd && ai && bi);

  bool p40 = ai && bi && ci && di;
  bool p04 = !ai && !bi && !ci && !di;

  bool disp6a = p31 || (p22 && disprtin);
  bool disp6a2 = p31 && disprtin;
  bool disp6a0 = p13 && !disprtin;

  bool disp6b = (((ei && ii && !disp6a0) || (disp6a && (ei || ii)) || disp6a2 ||
                (ei && ii && di)) && (ei || ii || di));

  bool p22bceeqi = p22 && bi && ci && (ei == ii);
  bool p22bncneeqi = p22 && !bi && !ci && (ei == ii);
  bool p13in = p13 && !ii;
  bool p31i = p31 && ii;
  bool p13dei = p13 && di && ei && ii;
  bool p22aceeqi = p22 && ai && ci && (ei == ii);
  bool p22ancneeqi = p22 && !ai && !ci && (ei == ii);
  bool p13en = p13 && !ei;
  bool anbnenin = !ai && !bi && !ei && !ii;
  bool abei = ai && bi && ei && ii;
  bool cdei = ci && di && ei && ii;
  bool cndnenin = !ci && !di && !ei && !ii;

  bool p22enin = p22 && !ei && !ii;
  bool p22ei = p22 && ei && ii;
  bool p31dnenin = p31 && !di && !ei && !ii;
  bool p31e = p31 && ei;

  bool compa = p22bncneeqi || p31i || p13dei || p22ancneeqi || 
             p13en || abei || cndnenin;
  bool compb = p22bceeqi || p31i || p13dei || p22aceeqi || 
             p13en || abei || cndnenin;
  bool compc = p22bceeqi || p31i || p13dei || p22ancneeqi || 
             p13en || anbnenin || cndnenin;
  bool compd = p22bncneeqi || p31i || p13dei || p22aceeqi ||
             p13en || abei || cndnenin;
  bool compe = p22bncneeqi || p13in || p13dei || p22ancneeqi || 
             p13en || anbnenin || cndnenin;

  bool a_o = ai ^ compa;
  bool b_o = bi ^ compb;
  bool c_o = ci ^ compc;
  bool d_o = di ^ compd;
  bool e_o = ei ^ compe;

  bool feqg = (fi && gi) || (!fi && !gi);
  bool heqj = (hi && ji) || (!hi && !ji);
  bool fghj22 = (fi && gi && !hi && !ji) ||
              (!fi && !gi && hi && ji) ||
              (!feqg && !heqj);
  bool fghjp13 = (!feqg && !hi && !ji) ||
               (!heqj && !fi && !gi);
  bool fghjp31 = ((!feqg) && hi && ji) ||
               (!heqj && fi && gi);

  bool dispout = (fghjp31 || (disp6b && fghj22) || (hi && ji)) && (hi || ji);

  bool ko = ((ci && di && ei && ii) || (!ci && !di && !ei && !ii) ||
          (p13 && !ei && ii && gi && hi && ji) ||
          (p31 && ei && !ii && !gi && !hi && !ji));

  bool alt7 = (fi && !gi && !hi &&
             ((disprtin && ci && di && !ei && !ii) || ko ||
              (disprtin && !ci && di && !ei && !ii))) ||
            (!fi && gi && hi &&
             ((!disprtin && !ci && !di && ei && ii) || ko ||
              (!disprtin && ci && !di && ei && ii)));

  bool k28 = (ci && di && ei && ii) || !(ci || di || ei || ii);
  bool k28p = !(ci || di || ei || ii);
  bool fo = (ji && !fi && (hi || !gi || k28p)) ||
          (fi && !ji && (!hi || gi || !k28p)) ||
          (k28p && gi && hi) ||
          (!k28p && !gi && !hi);
  bool go = (ji && !fi && (hi || !gi || !k28p)) ||
          (fi && !ji && (!hi || gi || k28p)) ||
          (!k28p && gi && hi) ||
          (k28p && !gi && !hi);
  bool ho = ((ji ^ hi) && !((!fi && gi && !hi && ji && !k28p) || (!fi && gi && hi && !ji && k28p) || 
                          (fi && !gi && !hi && ji && !k28p) || (fi && !gi && hi && !ji && k28p))) ||
          (!fi && gi && hi && ji) || (fi && !gi && !hi && !ji);

  bool disp6p = (p31 && (ei || ii)) || (p22 && ei && ii);
  bool disp6n = (p13 && !(ei && ii)) || (p22 && !ei && !ii);
  bool disp4p = fghjp31;
  bool disp4n = fghjp13;

  bool coderror = p40 || p04 ||
                (fi && gi && hi && ji) || (!fi && !gi && !hi && !ji) ||
                (p13 && !ei && !ii) || (p31 && ei && ii) ||
                (ei && ii && fi && gi && hi) || (!ei && !ii && !fi && !gi && !hi) ||
                (ei && !ii && gi && hi && ji) || (!ei && ii && !gi && !hi && !ji) ||
                (!p31 && ei && !ii && !gi && !hi && !ji) ||
                (!p13 && !ei && ii && gi && hi && ji) ||
                (((ei && ii && !gi && !hi && !ji) || (!ei && !ii && gi && hi && ji)) &&
                 !( (ci && di && ei) || (!ci && !di && !ei) )) ||
                (disp6p && disp4p) || (disp6n && disp4n) ||
                (ai && bi && ci && !ei && !ii && ((!fi && !gi) || fghjp13)) ||
                (!ai && !bi && !ci && ei && ii && ((fi && gi) || fghjp31)) ||
                (fi && gi && !hi && !ji && disp6p) ||
                (!fi && !gi && hi && ji && disp6n) ||
                (ci && di && ei && ii && !fi && !gi && !hi) ||
                (!ci && !di && !ei && !ii && fi && gi && hi);
  
  uint16_t datout = (ko  << 8) |
                     (ho  << 7) |
                     (go  << 6) |
                     (fo  << 5) |
                     (e_o << 4) |
                     (d_o << 3) |
                     (c_o << 2) |
                     (b_o << 1) |
                     (a_o << 0);

  bool disperror = ((disprtin && disp6p) || (disp6n && !disprtin) ||
                  (disprtin && !disp6n && fi && gi) ||
                  (disprtin && ai && bi && ci) ||
                  (disprtin && !disp6n && disp4p) ||
                  (!disprtin && !disp6p && !fi && !gi) ||
                  (!disprtin && !ai && !bi && !ci) ||
                  (!disprtin && !disp6p && disp4n) ||
                  (disp6p && disp4p) || (disp6n && disp4n));

  return DecodeResult{datout, dispout, coderror, disperror};
}

int main() {

  EncoderResult result = {0b0, false};
  DecodeResult  decode_result = {0b0, false, false, false};

  uint16_t data_test[] = {0b100111100, 0b0, 0b111, 0b1010, 0b000111100, 0b011111111};

  for (int j = 0; j < 6; j++) {
    std::cout << "EVENT: " << j << "\n" << std::endl;
    std::cout << "ENCODER" <<"\n";     
    std::cout << "input: " << data_test[j] << ", disp: " << result.disp << "\n";

    result = encode(data_test[j], result.disp);
    decode_result = decode(result.data, decode_result.disp);
   
    // Display the dataout as a 10-bit binary string.
    std::cout << "dataout: 0b";
    for (int i = 9; i >= 0; i--) {
        std::cout << ((result.data >> i) & 1);
    }
    std::cout << "\ndispout: " << result.disp << "\n" << std::endl;

    std::cout << "DECODER" <<"\n";  
    std::cout << "input: 0b";
    for (int i = 9; i >= 0; i--) {
        std::cout << ((result.data >> i) & 1);
    }      
    std::cout << "\ndataout: "         << decode_result.data;    
    std::cout << "\ndisp: "            << decode_result.disp;
    std::cout << "\ncode error: "      << decode_result.codeError;
    std::cout << "\ndisparity error: " << decode_result.disparityError << "\n" << std::endl; 
    std::cout << "==========*==========*==========*==========*" << "\n" << std::endl;;    


  }

  return 0;
}

