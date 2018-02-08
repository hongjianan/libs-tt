#ifndef EVENT_TRAFFIC_STAT_H
#define EVENT_TRAFFIC_STAT_H

#include <stdio.h>
#include <event2/util.h>


#define KBPS (1024)
#define MBPS (1024 * 1024)

class TrafficStat
{
public:
	int64_t rx_bytes_total;
	int64_t rx_packets_total;
	int64_t rx_bytes_period;
	int64_t rx_packets_period;

	int64_t tx_bytes_total;
	int64_t tx_packets_total;
	int64_t tx_bytes_period;
	int64_t tx_packets_period;

	struct timeval begin_time_total;
	struct timeval begin_time_period;

	TrafficStat(struct timeval begin) : rx_bytes_total(0), rx_packets_total(0), rx_bytes_period(0), rx_packets_period(0),
										tx_bytes_total(0), tx_packets_total(0), tx_bytes_period(0), tx_packets_period(0),
										begin_time_total(begin), begin_time_period(begin)
	{
	}


	void output_period(int bps)
	{
		struct timeval newtime, difference;

		evutil_gettimeofday(&newtime, NULL);
		evutil_timersub(&newtime, &begin_time_period, &difference);
		double elapsed = difference.tv_sec + (difference.tv_usec / 1.0e6);

		begin_time_period = newtime;

        printf("elapsed time:%lf, rx_bytes:%llu rx_packets:%llu, tx_bytes:%llu tx_packets:%llu\n",
                elapsed, rx_bytes_period, rx_packets_period, tx_bytes_period, tx_packets_period);

        double rx_speed = 0, tx_speed = 0;
        if (elapsed > 0.000001)
        {
            rx_speed = rx_bytes_period / elapsed / bps;
            tx_speed = tx_bytes_period / elapsed / bps;
        }

        printf("rx_speed:%.3f , tx_speed:%.3f %s\n",
                rx_speed, tx_speed, (bps == KBPS) ? "KBPS" : "MBPS");

        calc_total();
        clear_period();
	}

    void output_total()
    {
        struct timeval newtime, difference;

        evutil_gettimeofday(&newtime, NULL);
        evutil_timersub(&newtime, &begin_time_total, &difference);
        double elapsed = difference.tv_sec + (difference.tv_usec / 1.0e6);

        printf("total elapsed time:%lf, rx_bytes:%llu rx_packets:%llu, tx_bytes:%llu tx_packets:%llu\n",
                elapsed, rx_bytes_total + rx_bytes_period, rx_packets_total + rx_packets_period,
            tx_bytes_total + tx_bytes_period, tx_packets_total + tx_packets_period);
    }

private:
	inline void calc_total()
	{
		rx_bytes_total    += rx_bytes_period;
		rx_packets_total  += rx_packets_period;
		tx_bytes_total    += tx_bytes_period;
		tx_packets_total  += tx_packets_period;
	}

	inline void clear_period()
	{
		rx_bytes_period   = 0;
		rx_packets_period = 0;
		tx_bytes_period   = 0;
		tx_packets_period = 0;
	}

};

#endif /* EVENT_TRAFFIC_STAT_H */
