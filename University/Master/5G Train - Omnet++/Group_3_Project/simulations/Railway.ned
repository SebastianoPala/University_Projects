package train5G.simulations;

import train5G.rail.train.Train;
import train5G.rail.BaseStation;
import train5G.rail.Mapper;

network Railway
{
    parameters:
        @display("bgb=10000,2000,,,0;bgg=1000,1,black");
        double rail_length @unit(m) = default(10km);
        int num_base = default(10); // number of base stations

    submodules:
        train: Train {
            @display("p=0,1000;r=0;is=n;i=custom/train,-");
            wrapAroundDistance = parent.rail_length;
        }
        baseStations[num_base]: BaseStation {
            @display("i=device/antennatower");
        }
        map: Mapper {
            @display("p=9763,1775;i=old/srouter");
        }
}
