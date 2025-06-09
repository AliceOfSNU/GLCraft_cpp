struct GameStateHeader{
    float playerPosition[3];
    int numChunks;
};

struct ChunkHeader{
    float basepos[3];
    float chunkIdx[3];
};