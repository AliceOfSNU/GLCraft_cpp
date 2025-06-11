#ifndef PERSISTENCE_H
#define PERSISTENCE_H
struct GameStateHeader{
    float playerPosition[3];
    int numChunks;
};

struct ChunkHeader{
    float basepos[3];
    float chunkIdx[3];
};
#endif