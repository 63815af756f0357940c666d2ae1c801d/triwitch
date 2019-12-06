#include <iostream>
using namespace std;



#include "generator.h"
#include "layers.h"



long long fullSeed = 1234567890123456789;// change here to your seed
unsigned long long currentSeed = ((1ULL << 48) - 1)&fullSeed;


STRUCT(Pos)
{
	int x, z;
};

STRUCT(BiomeFilter)
{
	// bitfield for required temperature categories, including special variants
	uint64_t tempCat;
	// bitfield for the required ocean types
	uint64_t oceansToFind;
	// bitfield of required biomes without modification bit
	uint64_t biomesToFind;
	// bitfield of required modified biomes
	uint64_t modifiedToFind; // TODO: add checks for bamboo_jungle*

							 // check that there is a minimum of both special and normal temperatures
	int tempNormal, tempSpecial;
	// check for the temperatures specified by tempCnt (1:1024)
	int doTempCheck;
	// check for mushroom potential
	int requireMushroom;
	// combine a more detailed mushroom and temperature check (1:256)
	int doShroomAndTempCheck;
	// early check for 1.13 ocean types (1:256)
	int doOceanTypeCheck;
	//
	int doMajorBiomeCheck;
	// pre-generation biome checks in layer L_BIOME_256
	int checkBiomePotential;
	//
	int doScale4Check;
};


//==============================================================================
// Moving Structures
//==============================================================================

/* Transposes a base seed such that structures are moved by the specified region
* vector, (regX, regZ).
*/
static inline int64_t moveStructure(const int64_t baseSeed,
	const int regX, const int regZ)
{
	return (baseSeed - regX * 341873128712 - regZ * 132897987541) & 0xffffffffffff;
}

//enum versions {
//	MC_1_7, MC_1_8, MC_1_9, MC_1_10, MC_1_11, MC_1_12, MC_1_13, MC_1_13_2, MC_1_14, MC_LEG
//};

class Structure {
public:
	explicit Structure(long long chunkX, long long chunkZ, long long incompleteRand, int modulus, char typeStruct) {
		this->chunkX = chunkX;
		this->chunkZ = chunkZ;
		this->incompleteRand = incompleteRand;
		this->modulus = modulus;
		this->typeStruct = typeStruct;
	};
	long long chunkX;
	long long chunkZ;
	long long incompleteRand;
	int modulus;
	char typeStruct;
};

class Random {
private:
	unsigned long long seed;
public:
	explicit Random(unsigned long long seed) : seed((seed ^ (unsigned long long) 0x5deece66d) & ((1LLU << 48u) - 1)) {};

	void setSeed(unsigned long long seed) {
		this->seed = (seed ^ (unsigned long long) 0x5deece66d) & ((1LLU << 48u) - 1);
	}

	unsigned long long getSeed() {
		return this->seed;
	}

	long signed next(unsigned int bits) {

		if (bits < 1) { bits = 1; }
		else if (bits > 32) { bits = 32; }
		seed = (seed * 0x5deece66d + 0xb);
		seed &= ((1LLU << 48u) - 1);
		return (long)(seed >> (48u - bits));
	}

	long signed nextInt(long unsigned bound) {
		if (bound <= 0) {
			std::cerr << "Invalid bound";
		}
		if ((bound & (bound - 1)) == 0) {
			return (long signed)((bound * (unsigned long long) next(31)) >> 31u);
		}
		long signed bits = next(31);
		long signed val = bits % bound;
		while ((bits - val + bound - 1) < 0) {
			bits = next(31);
			val = bits % bound;
		}
		return val;
	}

	float nextFloat() {
		return next(24) / (float)(1 << 24);
	}
};

bool newtestxz(unsigned long long currentSeed, long long cx, long long cz) {
	long long incompleteRand = (cx >> 5LL) * 341873128712LL + (cz >> 5LL) * 132897987541LL + 14357620;
	Random r = Random(currentSeed + incompleteRand);
	long signed k, m;
	k = r.nextInt(24);
	m = r.nextInt(24);
	return ((cx & 31) == k && m == (cz & 31));
}

//==============================================================================
// Checking Biomes & Biome Helper Functions
//==============================================================================


int getBiomeAtPos(const LayerStack g, int x,int z)
{
	int *map = allocCache(&g.layers[L_VORONOI_ZOOM_1], 1, 1);
	genArea(&g.layers[L_VORONOI_ZOOM_1], map,x, z, 1, 1);
	int biomeID = map[0];
	free(map);
	return biomeID;
}

int main() {
	
	// Always initialize the biome list before starting any seed finder or
	// biome generator.
	initBiomes();
	LayerStack g;

	g = setupGenerator(MC_1_7);

	Layer *lFilterBiome = &g.layers[L_BIOME_256];
	int *biomeCache = allocCache(lFilterBiome, 3, 3);

	Layer layerBiomeDummy;
	setupLayer(256, &layerBiomeDummy, NULL, 200, NULL);
	
	int64_t base;
	int regPosX, regPosZ;

	
	setWorldSeed(&layerBiomeDummy, fullSeed);

	for (long long cx = -1000000;cx < 1000000;cx++)
		for (long long cz = -1000000;cz < 1000000;cz++) {// change here to your search size
			if (newtestxz(currentSeed, cx, cz)) {
				//cout << cx << " " << cz << endl;
			}
			else continue;

			regPosX = cx;
			regPosZ = cz;

			int areaX = (regPosX << 1) + 1;
			int areaZ = (regPosZ << 1) + 1;

			base = moveStructure(currentSeed, regPosX, regPosZ);

			setChunkSeed(&layerBiomeDummy, areaX + 1, areaZ + 1);
			if (mcNextInt(&layerBiomeDummy, 6) != 5)
				continue;
			setWorldSeed(lFilterBiome, fullSeed);
			genArea(lFilterBiome, biomeCache, (regPosX << 1) + 2, (regPosZ << 1) + 2, 1, 1);
			
			applySeed(&g, fullSeed);
			int x = (regPosX << 4) + 9;
			int z = (regPosZ << 4) + 9;
			if (getBiomeAtPos(g, x,z) != swamp) continue;
			cout << cx << " " << cz << endl;
		}

	return 0;
}

