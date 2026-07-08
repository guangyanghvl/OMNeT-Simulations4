message WSNPacket
{
    int srcId = -1;
    int lastHopId = -1;
    int finalDstId = -1;
    int seq = 0;
    int hopCount = 0;
    int ttl = 10;
    simtime_t createdAt = 0;
    
    bool isCoded = false;
    int srcId2 = -1;
    int seq2 = -1;
}
