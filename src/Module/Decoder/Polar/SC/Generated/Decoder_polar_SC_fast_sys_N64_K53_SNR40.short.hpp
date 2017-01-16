#ifndef DECODER_POLAR_SC_FAST_SYS_N64_K53_SNR40_HPP_
#define DECODER_POLAR_SC_FAST_SYS_N64_K53_SNR40_HPP_

#include <cassert>

#include "../Decoder_polar_SC_fast_sys.hpp"

static const char Decoder_polar_SC_fast_sys_fb_64_53_40[64] = {
1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

template <typename B, typename R, class API_polar>
class Decoder_polar_SC_fast_sys_N64_K53_SNR40 : public Decoder_polar_SC_fast_sys<B, R, API_polar>
{
public:
	Decoder_polar_SC_fast_sys_N64_K53_SNR40(const int& K, const int& N, const mipp::vector<B>& frozen_bits, const int n_frames = 1)
	: Decoder_polar_SC_fast_sys<B, R, API_polar>(K, N, frozen_bits, n_frames)
	{
		assert(N == 64);
		assert(K == 53);
		
		auto i = 0;
		while (i < 64 && Decoder_polar_SC_fast_sys_fb_64_53_40[i] == frozen_bits[i]) i++;
		assert(i == 64);
	}

	virtual ~Decoder_polar_SC_fast_sys_N64_K53_SNR40()
	{
	}

	void decode()
	{
		auto &l = this->l;
		auto &s = this->s;

		API_polar::template f  <32>(   l,   0+  0,   0+ 32,            0+ 64, 32);
		API_polar::template f  <16>(   l,  64+  0,  64+ 16,           64+ 32, 16);
		API_polar::template f  < 8>(   l,  96+  0,  96+  8,           96+ 16,  8);
		API_polar::template rep< 8>(s, l, 112+  0,                     0+  0,  8);
		API_polar::template gr < 8>(s, l,  96+  0,  96+  8,   0+  0,  96+ 16,  8);
		API_polar::template f  < 4>(   l, 112+  0, 112+  4,          112+  8,  4);
		API_polar::template g0 < 2>(   l, 120+  0, 120+  2,          120+  4,  2);
		API_polar::template h  < 2>(s, l, 124+  0,                    10+  0,  2);
		API_polar::template xo0< 2>(s,      8+  2,                     8+  0,  2);
		API_polar::template g  < 4>(s, l, 112+  0, 112+  4,   8+  0, 112+  8,  4);
		API_polar::template h  < 4>(s, l, 120+  0,                    12+  0,  4);
		API_polar::template xo < 4>(s,      8+  0,   8+  4,            8+  0,  4);
		API_polar::template xo < 8>(s,      0+  0,   0+  8,            0+  0,  8);
		API_polar::template g  <16>(s, l,  64+  0,  64+ 16,   0+  0,  64+ 32, 16);
		API_polar::template spc<16>(s, l,  96+  0,                    16+  0, 16);
		API_polar::template xo <16>(s,      0+  0,   0+ 16,            0+  0, 16);
		API_polar::template g  <32>(s, l,   0+  0,   0+ 32,   0+  0,   0+ 64, 32);
		API_polar::template spc<32>(s, l,  64+  0,                    32+  0, 32);
		API_polar::template xo <32>(s,      0+  0,   0+ 32,            0+  0, 32);
	}
};
#endif
