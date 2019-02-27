/*
Copyright (C) 2000 - 2016 Evan Teran
                          evan.teran@gmail.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef INES_HEADER_20160318_H_
#define INES_HEADER_20160318_H_

#include <cstdint>

namespace iNES {

enum class Mirroring {
	HORIZONTAL,
	VERTICAL,
	FOUR_SCREEN
};

enum class System {
	NES,
	P10,
	VS
};

enum class Display {
	NTSC,
	PAL,
	BOTH
};

enum class Ppu {
	UNKNOWN,
	RP2C03B,     /* (bog standard RGB palette) */
	RP2C03G,     /* (similar pallete to above, might have 1 changed colour) */
	RP2C04_0001, /* (scrambled palette + new colours) */
	RP2C04_0002, /* (same as above, different scrambling, diff new colours) */
	RP2C04_0003, /* (similar to above) */
	RP2C04_0004, /* (similar to above) */
	RC2C03B,     /* (bog standard palette, seems identical to RP2C03B) */
	RC2C03C,     /* (similar to above, but with 1 changed colour or so) */
	RC2C05_01,   /* (all five of these have the normal palette... */
	RC2C05_02,   /* (...but with different bits returned on 2002) */
	RC2C05_03,
	RC2C05_04,
	RC2C05_05,
	RESERVED_1,
	RESERVED_2,
	RESERVED_3
};

/* layout of first sixteen bytes of nes cartridge in ines format */
class Header {
public:
	int version() const;
	bool isValid() const;
	bool isDirty() const;
	
public:
	uint32_t mapper() const;    /* iNES mapper number */
	uint32_t submapper() const; /* iNES sub-mapper number */
	Ppu ppu() const;
	Display display() const;
	System system() const;
	Mirroring mirroring() const;
	bool trainer_present() const;
	uint32_t prg_size() const;
	uint32_t chr_size() const;

public:
	char    ines_signature_[4]; /* 0x1A53454E (NES file signature) */
	uint8_t prg_size_;          /* in 16k banks */
	uint8_t chr_size_;          /* in 8k banks */
	uint8_t ctrl1_;             /* %NNNN.FTBM */
	uint8_t ctrl2_;             /* %NNNN.SSPV (version 2.0 when SS = 10b*/

	/* in iNES 2.0, these mean something, otherwise, should be 0 */
	union {
		struct ines2_t {
			uint8_t byte8;  /* %SSSS.MMMM */
			uint8_t byte9;  /* %CCCC.PPPP */
			uint8_t byte10; /* %pppp.PPPP */
			uint8_t byte11; /* %cccc.CCCC */
			uint8_t byte12; /* %xxxx.xxBP */
			uint8_t byte13; /* %MMMM.PPPP */
			uint8_t byte14; /* reserved */
			uint8_t byte15; /* reserved */
		} ines2;

		struct ines1_t {
			uint32_t reserved_1;
			uint32_t reserved_2;
		} ines1;
	} extended_;
};

}

#endif

