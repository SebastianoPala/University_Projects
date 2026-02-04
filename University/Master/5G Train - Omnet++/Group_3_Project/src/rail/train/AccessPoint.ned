
package train5G.rail.train;

simple AccessPoint
{
    parameters:
    	@signal[throughput_sig](type=double);
    	@statistic[throughput](source="throughput_sig";record=last);
    	
    	@signal[throughput_vect_sig](type=double);
    	@statistic[throughput_vect](source="throughput_vect_sig";record=vector(mean));
    	
    	@signal[response_time_sig](type=double);
    	@statistic[response_time](source="response_time_sig";record=vector(mean));
    	
    	@signal[response_time_avg_sig](type=double);
    	@statistic[avg_response_time](source="response_time_avg_sig";record=avg);
    	
        double max_data_rate = default(1000000);

        double handover_interval @unit(s) = default(0.4s); // the uniform variable's width
        double handover_mean @unit(s) = default(1.5s);

        double sampling_period @unit(s) = default(1.5s);
    gates:
        input in @directIn;
}
