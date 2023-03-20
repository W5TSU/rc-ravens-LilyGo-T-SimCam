#include "GpsPoint.h"

namespace rlc
{
    double GpsPoint::convert_nmea_to_degrees(String val_str)
    {
        // NMEA Format: DDDMM.MMMMM
        //
        int period_pos = val_str.indexOf(".");
        if(period_pos < 0)
        {
            return 99999.99;
        }

        double degrees = val_str.substring(0, period_pos-2).toDouble();
        double minutes = val_str.substring(period_pos-2).toDouble();

        return degrees + minutes/60;
    }

    GpsPoint GpsPoint::from_gps_str(String &str)
    {
        unsigned int year = 0;
        unsigned int month = 0;
        unsigned int day = 0;
        unsigned int hour = 0;
        unsigned int minute = 0;
        unsigned int second = 0;
        double lat = 0;
        double lng = 0;
        double alt = 0.0;

        if (str.indexOf("+CGPSINFO: ") == 0)
        {
            // Parsing: +CGPSINFO: 4300.471406,N,08932.266537,W,200323,183805.0,79.2,0.0,0.0
            //
            int start = str.indexOf(":") + 2;
            int end = str.indexOf(",", start);
            String lat_str = str.substring(start, end);
            lat = GpsPoint::convert_nmea_to_degrees(lat_str);

            start = end + 1;
            end = str.indexOf(",", start);
            String lat_hemisphere = str.substring(start, end);
            if (lat_hemisphere == "S")
            {
                lat = -lat;
            }

            start = end + 1;
            end = str.indexOf(",", start);
            String lng_str = str.substring(start, end);
            lng = GpsPoint::convert_nmea_to_degrees(lng_str);

            start = end + 1;
            end = str.indexOf(",", start);
            String lng_hemisphere = str.substring(start, end);
            if (lng_hemisphere == "W")
            {
                lng = -lng;
            }

            start = end + 1;
            end = str.indexOf(",", start);
            String ddmmyy = str.substring(start, end);
            day = ddmmyy.substring(0, 2).toInt();
            month = ddmmyy.substring(2, 4).toInt();
            year = ddmmyy.substring(4).toInt() + 2000;

            start = end + 1;
            end = str.indexOf(".", start);
            String hhmmss = str.substring(start, end);
            hour = hhmmss.substring(0, 2).toInt();
            minute = hhmmss.substring(2, 4).toInt();
            second = hhmmss.substring(4).toInt();

            start = end + 3;
            end = str.indexOf(",", start);
            String alt_str = str.substring(start, end);
            alt = alt_str.toDouble();
        }

        DateTime dt(year, month, day, hour, minute, second);

        GpsPoint pt(dt, lat, lng, alt);

        return pt;
    }

    GpsPoint::GpsPoint() : latitude(0.0), longitude(0.0), altitude(0.0), datetime()
    {
    }

    GpsPoint::GpsPoint(DateTime &datetime, double lat, double lng) : latitude(lat), longitude(lng), altitude(0.0), datetime(datetime)
    {
    }

    GpsPoint::GpsPoint(DateTime &datetime, double lat, double lng, double alt) : latitude(lat), longitude(lng), altitude(alt), datetime(datetime)
    {
    }

    bool GpsPoint::copy(GpsPoint &pt)
    {
        datetime.set(pt.datetime.year, pt.datetime.month, pt.datetime.day, pt.datetime.hour, pt.datetime.minute, pt.datetime.second);
        latitude = pt.latitude;
        longitude = pt.longitude;
        altitude = pt.altitude;

        return validate();
    }

    String GpsPoint::to_string()
    {
        if (!is_valid)
        {
            return "Invalid GPS Point";
        }

        return "Date/Time: " + datetime.to_date_time_string() + ", Latitude: " + String(latitude, 6) + ", Longitude: " + String(longitude, 6) + ", Altitude: " + String(altitude, 6);
    }

    double GpsPoint::distance_in_miles(GpsPoint &from)
    {
        return haversine_calculation(latitude, longitude, from.latitude, from.longitude, MyMath::earth_radius_in_miles);
    }

    /*------------------------------------------------------------------------------------------
            PRIVATE METHODS
    ------------------------------------------------------------------------------------------*/

    double GpsPoint::haversine_calculation(double lat1, double lng1, double lat2, double lng2, double earth_radius)
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


    bool GpsPoint::validate()
    {
        is_valid = true;

        if (!datetime.is_valid)
        {
            is_valid = false;
        }

        if (latitude < -90 || latitude > 90)
        {
            is_valid = false;
        }

        if (longitude < -180 || longitude > 180)
        {
            is_valid = false;
        }

        return is_valid;
    }
}