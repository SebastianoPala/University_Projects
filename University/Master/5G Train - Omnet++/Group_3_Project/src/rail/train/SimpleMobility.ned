package train5G.rail.train;

simple SimpleMobility
{
    parameters:
        double initialX @unit(m) = default(0m);
        double initialY @unit(m) = default(0m);

        double speed @unit(mps) = default(0mps);

        double constraintAreaMinX @unit(m) = default(0m); // starting point of the railway
        double constraintAreaMaxX @unit(m) = default(10km); // wrap around point of the railway
        double updateInterval @unit(s) = default(0.05s); // how often the position is updated
}
