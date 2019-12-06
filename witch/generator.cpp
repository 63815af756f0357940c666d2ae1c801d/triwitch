#include "generator.h"
#include "layers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void setupLayer(int scale, Layer *l, Layer *p, int s, void (*getMap)(Layer *layer, int *out, int x, int z, int w, int h))
{
    setBaseSeed(l, s);
    l->scale = scale;
    l->p = p;
    l->p2 = NULL;
    l->getMap = getMap;
    l->oceanRnd = NULL;
}

void setupMultiLayer(int scale, Layer *l, Layer *p1, Layer *p2, int s, void (*getMap)(Layer *layer, int *out, int x, int z, int w, int h))
{
    setBaseSeed(l, s);
    l->scale = scale;
    l->p = p1;
    l->p2 = p2;
    l->getMap = getMap;
    l->oceanRnd = NULL;
}



LayerStack setupGenerator(const int mcversion)
{
    if (biomes[plains].id == 0)
    {
        fprintf(stderr, "Warning: The biomes have to be initialised first using initBiomes() before any generator can be used.\n");
    }

    LayerStack g;
    g.layerCnt = L_NUM;
    g.layers = (Layer *) calloc(g.layerCnt, sizeof(Layer));
    Layer *l = g.layers;

    //        SCALE  LAYER                      PARENT                      SEED  LAYER_FUNCTION
    setupLayer(4096, &l[L_ISLAND_4096],         NULL,                       1,    mapIsland);
    setupLayer(2048, &l[L_ZOOM_2048],           &l[L_ISLAND_4096],          2000, mapZoom);
    setupLayer(2048, &l[L_ADD_ISLAND_2048],     &l[L_ZOOM_2048],            1,    mapAddIsland);
    setupLayer(1024, &l[L_ZOOM_1024],           &l[L_ADD_ISLAND_2048],      2001, mapZoom);
    setupLayer(1024, &l[L_ADD_ISLAND_1024A],    &l[L_ZOOM_1024],            2,    mapAddIsland);
    setupLayer(1024, &l[L_ADD_ISLAND_1024B],    &l[L_ADD_ISLAND_1024A],     50,   mapAddIsland);
    setupLayer(1024, &l[L_ADD_ISLAND_1024C],    &l[L_ADD_ISLAND_1024B],     70,   mapAddIsland);
    setupLayer(1024, &l[L_REMOVE_OCEAN_1024],   &l[L_ADD_ISLAND_1024C],     2,    mapRemoveTooMuchOcean);

    setupLayer(1024, &l[L_ADD_SNOW_1024],       &l[L_REMOVE_OCEAN_1024],    2,    mapAddSnow);
    setupLayer(1024, &l[L_ADD_ISLAND_1024D],    &l[L_ADD_SNOW_1024],        3,    mapAddIsland);
    setupLayer(1024, &l[L_COOL_WARM_1024],      &l[L_ADD_ISLAND_1024D],     2,    mapCoolWarm);
    setupLayer(1024, &l[L_HEAT_ICE_1024],       &l[L_COOL_WARM_1024],       2,    mapHeatIce);
    setupLayer(1024, &l[L_SPECIAL_1024],        &l[L_HEAT_ICE_1024],        3,    mapSpecial);
    setupLayer(512,  &l[L_ZOOM_512],            &l[L_SPECIAL_1024],         2002, mapZoom);
    setupLayer(256,  &l[L_ZOOM_256],            &l[L_ZOOM_512],             2003, mapZoom);
    setupLayer(256,  &l[L_ADD_ISLAND_256],      &l[L_ZOOM_256],             4,    mapAddIsland);
    setupLayer(256,  &l[L_ADD_MUSHROOM_256],    &l[L_ADD_ISLAND_256],       5,    mapAddMushroomIsland);
    setupLayer(256,  &l[L_DEEP_OCEAN_256],      &l[L_ADD_MUSHROOM_256],     4,    mapDeepOcean);
    // biome layer chain
    setupLayer(256,  &l[L_BIOME_256],           &l[L_DEEP_OCEAN_256],       200,
            mcversion != MCBE ? mapBiome : mapBiomeBE);

    if (mcversion <= MC_1_13)
        setupLayer(128, &l[L_ZOOM_128],         &l[L_BIOME_256],            1000, mapZoom);
    else
    {
        setupLayer(256, &l[L14_BAMBOO_256],     &l[L_BIOME_256],            1001, mapAddBamboo);
        setupLayer(128, &l[L_ZOOM_128],         &l[L14_BAMBOO_256],         1000, mapZoom);
    }

    setupLayer(64,   &l[L_ZOOM_64],             &l[L_ZOOM_128],             1001, mapZoom);
    setupLayer(64,   &l[L_BIOME_EDGE_64],       &l[L_ZOOM_64],              1000, mapBiomeEdge);

    // basic river layer chain, used to determine where hills generate
    setupLayer(256,  &l[L_RIVER_INIT_256],      &l[L_DEEP_OCEAN_256],       100,  mapRiverInit);
    setupLayer(128,  &l[L_ZOOM_128_HILLS],      &l[L_RIVER_INIT_256],       1000, mapZoom);
    setupLayer(64,   &l[L_ZOOM_64_HILLS],       &l[L_ZOOM_128_HILLS],       1001, mapZoom);

    setupMultiLayer(64, &l[L_HILLS_64], &l[L_BIOME_EDGE_64], &l[L_ZOOM_64_HILLS], 1000,
            (mcversion & 0xff) <= MC_1_12 ? mapHills : mapHills113);

    setupLayer(64,   &l[L_RARE_BIOME_64],       &l[L_HILLS_64],             1001, mapRareBiome);
    setupLayer(32,   &l[L_ZOOM_32],             &l[L_RARE_BIOME_64],        1000, mapZoom);
    setupLayer(32,   &l[L_ADD_ISLAND_32],       &l[L_ZOOM_32],              3,    mapAddIsland);
    setupLayer(16,   &l[L_ZOOM_16],             &l[L_ADD_ISLAND_32],        1001, mapZoom);
    setupLayer(16,   &l[L_SHORE_16],            &l[L_ZOOM_16],              1000, mapShore);
    setupLayer(8,    &l[L_ZOOM_8],              &l[L_SHORE_16],             1002, mapZoom);
    setupLayer(4,    &l[L_ZOOM_4],              &l[L_ZOOM_8],               1003, mapZoom);
    setupLayer(4,    &l[L_SMOOTH_4],            &l[L_ZOOM_4],               1000, mapSmooth);

    // river layer chain
    setupLayer(128,  &l[L_ZOOM_128_RIVER],      &l[L_RIVER_INIT_256],       1000, mapZoom);
    setupLayer(64,   &l[L_ZOOM_64_RIVER],       &l[L_ZOOM_128_RIVER],       1001, mapZoom);
    setupLayer(32,   &l[L_ZOOM_32_RIVER],       &l[L_ZOOM_64_RIVER],        1000, mapZoom);
    setupLayer(16,   &l[L_ZOOM_16_RIVER],       &l[L_ZOOM_32_RIVER],        1001, mapZoom);
    setupLayer(8,    &l[L_ZOOM_8_RIVER],        &l[L_ZOOM_16_RIVER],        1002, mapZoom);
    setupLayer(4,    &l[L_ZOOM_4_RIVER],        &l[L_ZOOM_8_RIVER],         1003, mapZoom);
    setupLayer(4,    &l[L_RIVER_4],             &l[L_ZOOM_4_RIVER],         1,    mapRiver);
    setupLayer(4,    &l[L_SMOOTH_4_RIVER],      &l[L_RIVER_4],              1000, mapSmooth);

    setupMultiLayer(4, &l[L_RIVER_MIX_4], &l[L_SMOOTH_4], &l[L_SMOOTH_4_RIVER], 100, mapRiverMix);

    if (mcversion <= MC_1_12)
    {
        setupLayer(1,   &l[L_VORONOI_ZOOM_1],   &l[L_RIVER_MIX_4],          10,   mapVoronoiZoom);
    }
    else
    {
        // ocean variants
        setupLayer(256, &l[L13_OCEAN_TEMP_256], NULL,                       2,    mapOceanTemp);
        l[L13_OCEAN_TEMP_256].oceanRnd = (OceanRnd *) malloc(sizeof(OceanRnd));
        setupLayer(128, &l[L13_ZOOM_128],       &l[L13_OCEAN_TEMP_256],     2001, mapZoom);
        setupLayer(64,  &l[L13_ZOOM_64],        &l[L13_ZOOM_128],           2002, mapZoom);
        setupLayer(32,  &l[L13_ZOOM_32],        &l[L13_ZOOM_64],            2003, mapZoom);
        setupLayer(16,  &l[L13_ZOOM_16],        &l[L13_ZOOM_32],            2004, mapZoom);
        setupLayer(8,   &l[L13_ZOOM_8],         &l[L13_ZOOM_16],            2005, mapZoom);
        setupLayer(4,   &l[L13_ZOOM_4],         &l[L13_ZOOM_8],             2006, mapZoom);

        setupMultiLayer(4, &l[L13_OCEAN_MIX_4], &l[L_RIVER_MIX_4], &l[L13_ZOOM_4], 100, mapOceanMix);

        setupLayer(1,   &l[L_VORONOI_ZOOM_1],   &l[L13_OCEAN_MIX_4],        10,   mapVoronoiZoom);
    }

    return g;
}


void freeGenerator(LayerStack g)
{
    int i;
    for(i = 0; i < g.layerCnt; i++)
    {
        if (g.layers[i].oceanRnd != NULL)
            free(g.layers[i].oceanRnd);
    }

    free(g.layers);
}


/* Recursively calculates the minimum buffer size required to generate an area
 * of the specified size from the current layer onwards.
 */
static void getMaxArea(Layer *layer, int areaX, int areaZ, int *maxX, int *maxZ)
{
    if (layer == NULL)
        return;

    if (layer->getMap == mapZoom)
    {
        areaX = (areaX >> 1) + 2;
        areaZ = (areaZ >> 1) + 2;
    }
    else if (layer->getMap == mapVoronoiZoom)
    {
        areaX = (areaX >> 2) + 2;
        areaZ = (areaZ >> 2) + 2;
    }
    else if (layer->getMap == mapOceanMix)
    {
        areaX += 17;
        areaZ += 17;
    }
    else
    {
        if (layer->getMap != mapNull &&
            layer->getMap != mapSkip &&
            layer->getMap != mapIsland &&
            layer->getMap != mapSpecial &&
            layer->getMap != mapBiome &&
            layer->getMap != mapRiverInit &&
            layer->getMap != mapRiverMix &&
            layer->getMap != mapOceanTemp)
        {
            areaX += 2;
            areaZ += 2;
        }
    }

    if (areaX > *maxX) *maxX = areaX;
    if (areaZ > *maxZ) *maxZ = areaZ;

    getMaxArea(layer->p, areaX, areaZ, maxX, maxZ);
    getMaxArea(layer->p2, areaX, areaZ, maxX, maxZ);
}

int calcRequiredBuf(Layer *layer, int areaX, int areaZ)
{
    int maxX = areaX, maxZ = areaZ;
    getMaxArea(layer, areaX, areaZ, &maxX, &maxZ);

    return maxX * maxZ;
}

int *allocCache(Layer *layer, int sizeX, int sizeZ)
{
    int size = calcRequiredBuf(layer, sizeX, sizeZ);

    int *ret = (int *) malloc(sizeof(*ret)*size);
    memset(ret, 0, sizeof(*ret)*size);

    return ret;
}


void applySeed(LayerStack *g, int64_t seed)
{
    // the seed has to be applied recursively
    setWorldSeed(&g->layers[L_VORONOI_ZOOM_1], seed);
}

void genArea(Layer *layer, int *out, int areaX, int areaZ, int areaWidth, int areaHeight)
{
    memset(out, 0, areaWidth*areaHeight*sizeof(*out));
    layer->getMap(layer, out, areaX, areaZ, areaWidth, areaHeight);
}





