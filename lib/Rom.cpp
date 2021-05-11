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

#include "iNES/Rom.h"
#include "iNES/Error.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

#ifndef ZLIB_NOT_FOUND
#include <zlib.h>
#endif

namespace iNES {
namespace {

constexpr int PrgBlockSize = 0x4000;
constexpr int ChrBlockSize = 0x2000;
constexpr int TrainerSize  = 512;

/*------------------------------------------------------------------------------
// Name: next_power
//----------------------------------------------------------------------------*/
uint32_t next_power(uint32_t size) {

	/* returns 1 less than closest fitting power of 2
	 * is this number not a power of two or 0?
	 */
	if ((size & (size - 1)) != 0) {
		--size;
		size |= size >> 1;
		size |= size >> 2;
		size |= size >> 4;
		size |= size >> 8;
		size |= size >> 16;
		++size;
	} else if (size == 0) {
		++size;
	}

	return size;
}

/* this table generated for poly = $edb88320 */
static const uint32_t crc_table[256] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
	0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
	0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
	0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
	0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
	0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
	0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
	0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
	0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
	0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
	0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
	0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
	0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
	0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
	0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
	0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
	0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
	0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
	0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
	0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
	0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
	0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
	0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
	0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
	0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
	0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
	0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
	0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
	0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
	0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
	0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
	0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
	0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
	0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
	0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
	0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
	0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
	0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d};

/*------------------------------------------------------------------------------
// Name: ines_crc32
//----------------------------------------------------------------------------*/
uint32_t ines_crc32(const void *data, size_t length, uint32_t initial_value) {

	const uint8_t *ptr = static_cast<const uint8_t *>(data);

	/* start out with all bits set */
	uint32_t crc = ~initial_value;

	if (ptr != nullptr) {
		while (length-- != 0) {
			/* accumulate crc */
			crc = (crc >> 8) ^ crc_table[(crc & 0x000000ff) ^ *ptr++];
		}
	}

	/* invert all bits, and we're done */
	return ~crc;
}

}

/*-----------------------------------------------------------------------------
// Name: write
//---------------------------------------------------------------------------*/
void Rom::write(const char *filename) const {

	assert(filename != nullptr);

	std::ofstream os(filename, std::ifstream::binary);

	if (!os) {
		throw ines_open_failed();
	}

	/* write the header data */
	if (!os.write(reinterpret_cast<char *>(header_.get()), sizeof(Header))) {
		throw ines_write_failed();
	}

	if (trainer_) {
		if (!os.write(reinterpret_cast<char *>(trainer_.get()), TrainerSize)) {
			throw ines_write_failed();
		}
	}

	if (prg_size_ > 0) {
		assert(prg_rom_);
		if (!os.write(reinterpret_cast<char *>(prg_rom_.get()), prg_size_)) {
			throw ines_write_failed();
		}
	}

	if (chr_size_ > 0) {
		assert(chr_rom_);
		if (!os.write(reinterpret_cast<char *>(chr_rom_.get()), chr_size_)) {
			throw ines_write_failed();
		}
	}
}

/*-----------------------------------------------------------------------------
// Name: Cart
//---------------------------------------------------------------------------*/
Rom::Rom(const char *filename) {
#ifndef ZLIB_NOT_FOUND
	gzFile file = gzopen(filename, "rb");
#else
	FILE *file = fopen(filename, "rb");
#endif
	try {
		auto header_ptr = std::make_unique<Header>();

		/* read the header data */
#ifndef ZLIB_NOT_FOUND
		if (gzread(file, header_ptr.get(), sizeof(Header)) != sizeof(Header)) {
			throw ines_read_failed();
		}
#else
		if (fread(header_ptr.get(), 1, sizeof(Header), file) != sizeof(Header)) {
			throw ines_read_failed();
		}
#endif

		if (!header_ptr->isValid()) {
			throw ines_bad_header();
		}

		const bool has_trainer = header_ptr->trainer_present();

		const uint32_t prg_size = header_ptr->prg_size() * PrgBlockSize;
		const uint32_t chr_size = header_ptr->chr_size() * ChrBlockSize;

		const uint32_t prg_alloc_size = next_power(prg_size);
		const uint32_t chr_alloc_size = next_power(chr_size);

		/* allocate memory for the cart */
		auto prg_rom_ptr = prg_size ? std::make_unique<uint8_t[]>(prg_alloc_size) : nullptr;
		auto chr_rom_ptr = chr_size ? std::make_unique<uint8_t[]>(chr_alloc_size) : nullptr;
		auto trainer_ptr = has_trainer ? std::make_unique<uint8_t[]>(TrainerSize) : nullptr;

		if (has_trainer) {
#ifndef ZLIB_NOT_FOUND
			if (gzread(file, trainer_ptr.get(), TrainerSize) != TrainerSize) {
				throw ines_read_failed();
			}
#else
			if (fread(trainer_ptr.get(), 1, TrainerSize, file) != TrainerSize) {
				throw ines_read_failed();
			}
#endif
		}

		if (prg_size != 0) {
#ifndef ZLIB_NOT_FOUND
			if (gzread(file, prg_rom_ptr.get(), prg_size) != static_cast<int>(prg_size)) {
				throw ines_read_failed();
			}
#else
			if (fread(prg_rom_ptr.get(), 1, prg_size, file) != prg_size) {
				throw ines_read_failed();
			}
#endif

			if ((prg_alloc_size - prg_size) > 0x2000 && prg_size >= 0x2000) {
				/* replicate the last bank if necessary */
				uint8_t *const last_8k = prg_rom_ptr.get() + prg_size - 0x2000;
				uint8_t *p             = prg_rom_ptr.get() + prg_size;
				while (p < prg_rom_ptr.get() + prg_alloc_size) {
					memcpy(p, last_8k, 0x2000);
					p += 0x2000;
				}
			}
		}

		if (chr_size != 0) {
#ifndef ZLIB_NOT_FOUND
			if (gzread(file, chr_rom_ptr.get(), chr_size) != static_cast<int>(chr_size)) {
				throw ines_read_failed();
			}
#else
			if (fread(chr_rom_ptr.get(), 1, chr_size, file) != chr_size) {
				throw ines_read_failed();
			}
#endif

			uint8_t *p = chr_rom_ptr.get() + chr_size;
			while (p != chr_rom_ptr.get() + chr_alloc_size) {
				*p++ = 0xff;
			}
		}

		header_   = std::move(header_ptr);
		prg_rom_  = std::move(prg_rom_ptr);
		chr_rom_  = std::move(chr_rom_ptr);
		trainer_  = std::move(trainer_ptr);
		prg_size_ = prg_size;
		chr_size_ = chr_size;
	} catch (const ines_error &) {
#ifndef ZLIB_NOT_FOUND
		gzclose(file);
#else
		fclose(file);
#endif
		throw;
	}
#ifndef ZLIB_NOT_FOUND
	gzclose(file);
#else
	fclose(file);
#endif
}

/*-----------------------------------------------------------------------------
// Name: prg_hash
//---------------------------------------------------------------------------*/
uint32_t Rom::prg_hash() const {
	return ines_crc32(prg_rom_.get(), prg_size_, 0);
}

/*-----------------------------------------------------------------------------
// Name: chr_hash
//---------------------------------------------------------------------------*/
uint32_t Rom::chr_hash() const {
	return ines_crc32(chr_rom_.get(), chr_size_, 0);
}

/*-----------------------------------------------------------------------------
// Name: rom_hash
//---------------------------------------------------------------------------*/
uint32_t Rom::rom_hash() const {

	const uint32_t hash1 = trainer_ ? ines_crc32(trainer_.get(), TrainerSize, 0) : 0;
	const uint32_t hash2 = prg_rom_ ? ines_crc32(prg_rom_.get(), prg_size_, hash1) : hash1;
	const uint32_t hash3 = chr_rom_ ? ines_crc32(chr_rom_.get(), chr_size_, hash2) : hash2;

	return hash3;
}

/*-----------------------------------------------------------------------------
// Name: header
//---------------------------------------------------------------------------*/
Header *Rom::header() const {
	return header_.get();
}

/*-----------------------------------------------------------------------------
// Name: trainer
//---------------------------------------------------------------------------*/
uint8_t *Rom::trainer() const {
	return trainer_.get();
}

/*-----------------------------------------------------------------------------
// Name: prg_size
//---------------------------------------------------------------------------*/
uint32_t Rom::prg_size() const {
	return prg_size_;
}

/*-----------------------------------------------------------------------------
// Name: chr_size
//---------------------------------------------------------------------------*/
uint32_t Rom::chr_size() const {
	return chr_size_;
}

/*-----------------------------------------------------------------------------
// Name: prg_rom
//---------------------------------------------------------------------------*/
uint8_t *Rom::prg_rom() const {
	return prg_rom_.get();
}

/*-----------------------------------------------------------------------------
// Name: chr_rom
//---------------------------------------------------------------------------*/
uint8_t *Rom::chr_rom() const {
	return chr_rom_.get();
}

}
