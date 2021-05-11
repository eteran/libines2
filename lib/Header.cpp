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

#include "iNES/Header.h"

#include <cassert>
#include <cstdlib>
#include <cstring>

namespace iNES {
namespace {

/* Flags in Header.ctrl1 */
constexpr int INES_MIRROR  = 0x01;
constexpr int INES_SRAM    = 0x02;
constexpr int INES_TRAINER = 0x04;
constexpr int INES_4SCREEN = 0x08;

}

/*-----------------------------------------------------------------------------
// isValid
//---------------------------------------------------------------------------*/
bool Header::isValid() const {
	return memcmp(ines_signature_, "NES\x1a", 4) == 0;
}

/*-----------------------------------------------------------------------------
// isDirty
//---------------------------------------------------------------------------*/
bool Header::isDirty() const {
	if (version() != 2) {
		if (extended_.ines1.reserved_2 != 0 || extended_.ines1.reserved_1 != 0) {
			return true;
		}
	}

	return false;
}

/*-----------------------------------------------------------------------------
// version
//---------------------------------------------------------------------------*/
int Header::version() const {
	return ((ctrl2_ & 0xc) == 0x8) ? 2 : 1;
}

/*-----------------------------------------------------------------------------
// Name: mapper
//---------------------------------------------------------------------------*/
uint32_t Header::mapper() const {

	switch (version()) {
	case 2:
		return (((static_cast<uint32_t>(extended_.ines2.byte8) & 0x0f)) << 8) | (ctrl1_ >> 4) | (ctrl2_ & 0xf0);
	default:
		return (ctrl1_ >> 4) | (ctrl2_ & 0xf0);
	}
}

/*-----------------------------------------------------------------------------
// Name: submapper
//---------------------------------------------------------------------------*/
uint32_t Header::submapper() const {

	switch (version()) {
	case 2:
		return (extended_.ines2.byte8 & 0xf0) >> 4;
	default:
		return 0;
	}
}

/*-----------------------------------------------------------------------------
// Name: ppu
//---------------------------------------------------------------------------*/
Ppu Header::ppu() const {

	switch (version()) {
	case 2:
		switch (extended_.ines2.byte13 & 0x0f) {
		case 0x00:
			return Ppu::RP2C03B;
		case 0x01:
			return Ppu::RP2C03G;
		case 0x02:
			return Ppu::RP2C04_0001;
		case 0x03:
			return Ppu::RP2C04_0002;
		case 0x04:
			return Ppu::RP2C04_0003;
		case 0x05:
			return Ppu::RP2C04_0004;
		case 0x06:
			return Ppu::RC2C03B;
		case 0x07:
			return Ppu::RC2C03C;
		case 0x08:
			return Ppu::RC2C05_01;
		case 0x09:
			return Ppu::RC2C05_02;
		case 0x0a:
			return Ppu::RC2C05_03;
		case 0x0b:
			return Ppu::RC2C05_04;
		case 0x0c:
			return Ppu::RC2C05_05;
		default:
			return Ppu::UNKNOWN;
		}
	default:
		return Ppu::UNKNOWN;
	}
}

/*-----------------------------------------------------------------------------
// Name: display
//---------------------------------------------------------------------------*/
Display Header::display() const {

	switch (version()) {
	case 2:
		switch (extended_.ines2.byte12 & 0x03) {
		case 0x00:
			return Display::NTSC;
		case 0x01:
			return Display::PAL;
		case 0x02:
			return Display::BOTH;
		case 0x03:
			return Display::BOTH;
		default:
			return Display::BOTH;
		}
	default:
		return Display::BOTH;
	}
}

/*-----------------------------------------------------------------------------
// Name: system
//---------------------------------------------------------------------------*/
System Header::system() const {

	switch (ctrl2_ & 0x03) {
	case 0x01:
		return System::VS;
	case 0x02:
		return System::P10;
	default:
		return System::NES;
	}
}

/*-----------------------------------------------------------------------------
// Name: mirroring
//---------------------------------------------------------------------------*/
Mirroring Header::mirroring() const {

	switch (ctrl1_ & 0x09) {
	case 0x09:
	case 0x08:
		return Mirroring::FOUR_SCREEN;
	case 0x01:
		return Mirroring::VERTICAL;
	case 0x00:
	default:
		return Mirroring::HORIZONTAL;
	}
}

/*-----------------------------------------------------------------------------
// Name: trainer_present
//---------------------------------------------------------------------------*/
bool Header::trainer_present() const {
	return ((ctrl1_ & INES_TRAINER) != 0);
}

/*-----------------------------------------------------------------------------
// Name: prg_size
//---------------------------------------------------------------------------*/
uint32_t Header::prg_size() const {

	switch (version()) {
	case 2:
		return prg_size_ | ((static_cast<uint32_t>(extended_.ines2.byte9) & 0x0f) << 8);
	default:
		return prg_size_;
	}
}

/*-----------------------------------------------------------------------------
// Name: chr_size
//---------------------------------------------------------------------------*/
uint32_t Header::chr_size() const {

	switch (version()) {
	case 2:
		return chr_size_ | ((static_cast<uint32_t>(extended_.ines2.byte9) & 0xf0) << 4);
	default:
		return chr_size_;
	}
}

}
