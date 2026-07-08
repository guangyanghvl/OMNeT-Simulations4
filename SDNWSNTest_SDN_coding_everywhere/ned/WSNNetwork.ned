import WSNNode;
import SDNController;

network WSNNetwork
{
    parameters:
        int numSensors = default(20);
        int numNodes = default(numSensors + 1);
        //@display("bgb=300,300"); --20
         @display("bgb=500,500"); 
    submodules:
        node[numNodes]: WSNNode {
            parameters:
                nodeId = index;
                isSink = (index == parent.numSensors);
                sinkId = parent.numSensors;

                //xCoord = index == parent.numSensors ? 90 : uniform(0, 100); --20
                //yCoord = index == parent.numSensors ? 90 : uniform(0, 100); --20
                
                xCoord = index == parent.numSensors ? 145 : uniform(0, 160);
                yCoord = index == parent.numSensors ? 145 : uniform(0, 160);

                @display("p=$xCoord,$yCoord");
        }

        controller: SDNController {
            @display("p=20,20");
        }
}