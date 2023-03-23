#include "GpsCalculator.h"

namespace rlc
{
    GpsCalculator::GpsCalculator(GpsPoint &pt1, GpsPoint &pt2)
    {
        is_valid = false;
        distance_in_feet = 0;
        distance_in_miles = 0;
        velocity_in_feet_per_second = 0;
        velocity_in_miles_per_hour = 0;

        if (pt1.is_valid && pt2.is_valid)
        {
            distance_in_miles = haversine_calculation(pt1.latitude, pt1.longitude, pt2.latitude, pt2.longitude, MyMath::earth_radius_in_miles);
            distance_in_feet = rlc::MyMath::convert_miles_to_feet(distance_in_miles);

            time_diff_in_seconds = pt1.datetime.diff_in_seconds(pt2.datetime);
            time_diff_in_hours = time_diff_in_seconds / (60 * 60);

            velocity_in_feet_per_second = time_diff_in_seconds == 0 ? 0 : distance_in_feet / time_diff_in_seconds;
            velocity_in_miles_per_hour = time_diff_in_hours == 0 ? 0 : distance_in_miles / time_diff_in_hours;

            is_valid = true;
        }

        // Ideally we are sampling at least once per 50 yards = 150 feet
        //
        recommended_gps_refresh_period_ms = velocity_in_feet_per_second == 0 ? rlc::Config::gps_refresh_period_default_ms : 1000 * 150.0 / velocity_in_feet_per_second;
        if (recommended_gps_refresh_period_ms < rlc::Config::gps_refresh_period_smallest_ms)
        {
            recommended_gps_refresh_period_ms = rlc::Config::gps_refresh_period_smallest_ms;
        }
        if (recommended_gps_refresh_period_ms > rlc::Config::gps_refresh_period_default_ms)
        {
            recommended_gps_refresh_period_ms = rlc::Config::gps_refresh_period_default_ms;
        }
    }

    /*------------------------------------------------------------------------------------------
            PRIVATE METHODS
    ------------------------------------------------------------------------------------------*/
    double GpsCalculator::haversine_calculation(double lat1, double lng1, double lat2, double lng2, double earth_radius)
    {
        double lat_delta = MyMath::convert_degrees_to_radians(lat2 - lat1);
        double lng_delta = MyMath::convert_degrees_to_radians(lng2 - lng1);

        double lat1_rad = MyMath::convert_degrees_to_radians(lat1);
        double lat2_rad = MyMath::convert_degrees_to_radians(lat2);

        double a = pow(sin(lat_delta / 2), 2) + cos(lat1_rad) * cos(lat2_rad) * pow(sin(lng_delta / 2), 2);

        double c = 2 * atan2(sqrt(a), sqrt(1 - a));

        double d = earth_radius * c;

        return d;
    }
}