/*
 * Copyright © 2015 Denys Petrovnin <dipcore@gmail.com>
 */

#include"gps.h"

void notifier_svs_append(char talker[3], int prn, float elevation, float azimuth, float snr) {
	// Skip empty
	if (prn != 0) {

		for (int i=0; i<12; i++){
	        if (svs_used_ids[i] == prn){
				sv_status.used_in_fix_mask |= (1ul << (prn-1));
	    	}
		}

		sv_status.sv_list[sv_counter].prn = prn;
		sv_status.sv_list[sv_counter].elevation = elevation;
		sv_status.sv_list[sv_counter].azimuth = azimuth;
		sv_status.sv_list[sv_counter].snr = snr;
		sv_status.num_svs = sv_counter + 1;
		sv_counter ++;
	}
}

void notifier_svs_inview(char talker[3], int num_svs) {
	// Do nothing here
	// We calculate number of svs in notifier_append_sv
	// sv_status.num_svs = num_svs;
}

void notifier_svs_used_ids(int ids[12]) {
	memcpy(svs_used_ids, ids, 12);
}

void notifier_svs_update_status(char talker[3], int msg_nr, int total_msgs) {
	if (msg_nr == 1 && !strcmp(talker, "GP")) {

		// update
		int64_t diff = location.timestamp - last_svs_cycle_timestamp;
		if ( (location.flags & GPS_LOCATION_HAS_ACCURACY) && (diff >= 1000 / max_refresh_rate)){
			update_gps_svstatus(&sv_status);
			last_svs_cycle_timestamp = location.timestamp;
		}

		// start new cycle to collect svs
		sv_counter = 0;
		sv_status.used_in_fix_mask = 0ul;

	}
}

void notifier_set_speed(float speed_knots) {
	location.flags   |= GPS_LOCATION_HAS_SPEED;
	location.speed    = speed_knots * 1.852 / 3.6; // knots to m/s
}

void notifier_set_bearing(float bearing) {
	location.flags   |= GPS_LOCATION_HAS_BEARING;
    location.bearing  = bearing;
}

void notifier_set_latlong(double lat, double lon) {
	location.flags    |= GPS_LOCATION_HAS_LAT_LONG;
	location.latitude  = lat;
	location.longitude = lon;
}

void notifier_set_altitude(double altitude, char units) {
	location.flags   |= GPS_LOCATION_HAS_ALTITUDE;
	location.altitude = altitude;
}

void notifier_set_accuracy(float accuracy) {
	location.flags   |= GPS_LOCATION_HAS_ACCURACY;
	location.accuracy = accuracy;
}

void notifier_set_date_time(struct minmea_date date, struct minmea_time time_){

	D("hours: %d", time_.hours);
	D("minutes: %d", time_.minutes);
	D("seconds: %d", time_.seconds);
	D("year: %d", date.year);
	D("month: %d", date.month);
	D("day: %d", date.day);

	struct timespec ts;
	minmea_gettime(&ts, &date, &time_);
	location.timestamp = (long long) ts.tv_sec * 1000 + (long long) (ts.tv_nsec / 1e6);
}

void notifier_push_location() {

	int64_t diff = location.timestamp - last_location_cycle_timestamp;

	if ( (location.flags & GPS_LOCATION_HAS_ACCURACY) && (diff >= 1000 / max_refresh_rate)) {

		update_gps_location(&location);

		location.flags = 0;
		last_location_cycle_timestamp = location.timestamp;

		D("Update location time");
	}

}


