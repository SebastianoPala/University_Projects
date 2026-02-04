package train5G.rail.train;


simple Passenger
{
    parameters:
        double packet_size @unit(B) = default(1000B);
        double generation_delay @unit(s) = default(1s);
}
