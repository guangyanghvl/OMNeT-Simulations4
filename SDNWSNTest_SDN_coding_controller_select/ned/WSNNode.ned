simple WSNNode
{
    parameters:
        int nodeId;
        bool isSink = default(false);
        int sinkId = default(-1);
        double xCoord;
        double yCoord;
        double radioRange @unit(m) = default(35m);
        double sendInterval @unit(s) = default(5s);
        int initialTtl = default(10);
        double codingWaitTime @unit(s) = default(1s);
        
        double generationEnergyCost = default(0.1);
        double rxEnergyCost = default(0.5);
        double txEnergyCost = default(1.0);
        double codingEnergyCost = default(0.2);
        
        @display("i=device/wifilaptop");
    gates:
        input in @directIn;
}