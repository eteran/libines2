
#include "iNES/Error.h"
#include "iNES/Rom.h"
#include <cstdio>

int main() {
	auto c1 = iNES::Rom("mm2.nes.gz");
	printf("PRG HASH: %08x\n", c1.prg_hash());

	auto c2 = iNES::Rom("mm2.nes");
	printf("PRG HASH: %08x\n", c2.prg_hash());
}
