package train5G.rail.train;

import train5G.rail.train.SimpleMobility;
import train5G.rail.train.AccessPoint;

import train5G.rail.dbg.DebugModule;

module Train
{
    parameters:
        @display("i=custom/train,bg=400,34;bgb=1542,835,,,0;is=vs"); // graphical setup
        double wrapAroundDistance @unit(m);
        double trainSpeed @unit(kmph) = default(200kmph);
        double trainUpdateInterval @unit(s) = default(0.05s); // how often the train updates its position

        int passenger_count = default(4);

        bool randomness = default(true); // determines if the simple modules use random variables or deterministic ones (deterministic test)

    submodules:
        mobility: SimpleMobility {
            parameters:
                initialX = 0m;
                initialY = 1000m;

                speed = parent.trainSpeed;

                constraintAreaMinX = 0m;
                constraintAreaMaxX = parent.wrapAroundDistance;

                updateInterval = parent.trainUpdateInterval;
                @display("p=1465,86;i=misc/node,red");
        }
        ap: AccessPoint {
            @display("p=326,171");
        }
        passengers[passenger_count]: Passenger {
            @display("p=526,280");
        }

        //debug module
        dbg: DebugModule;
}
