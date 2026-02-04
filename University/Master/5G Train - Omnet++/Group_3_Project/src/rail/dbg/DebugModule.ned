package train5G.rail.dbg;

simple DebugModule
{
    parameters:
        bool debug = default(false); // enables/disables on-screen text
        double update_rate @unit(s) = default(0.05s); // how fast the module updates the on-screen text
}
