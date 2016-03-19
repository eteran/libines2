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

#ifndef INES_ROM_20160318_H_
#define INES_ROM_20160318_H_

#include "iNES/Header.h"

namespace iNES {

/* abstract description of a iNES file */
class Rom {
public:
	struct FromGzip {};
	struct FromiNES {};

public:
	Rom(const char *filename);
	Rom(const char *filename, const FromGzip &);
	Rom(const char *filename, const FromiNES &);
	Rom(const Rom &) = delete;
	Rom& operator=(const Rom &) = delete;
	Rom(Rom &&) = default;
	Rom& operator=(Rom &&) = default;
	~Rom();

public:
	/* API access to iNES data, works with version 2.0 ROMs as well */
	uint32_t prg_size() const;
	uint32_t chr_size() const;
	uint32_t prg_hash() const;
	uint32_t chr_hash() const;
	uint32_t rom_hash() const;
	Header *header() const;
	uint8_t *trainer() const;
	uint8_t *prg_rom() const;
	uint8_t *chr_rom() const;
	
public:
	/* functions for writing an iNES file */
	void write(const char *filename) const;

private:
	Header  *header_;   /* raw iNES header */
	uint8_t *trainer_;  /* pointer to 512 byte trainer data or NULL */
	uint8_t *prg_rom_;  /* pointer to PRG data */
	uint8_t *chr_rom_;  /* pointer to CHR data or NULL */
	uint32_t prg_size_; /* size of PRG data */
	uint32_t chr_size_; /* size of CHR data or 0 */
};

}

#endif

