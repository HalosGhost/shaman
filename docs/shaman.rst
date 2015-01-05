.. Copyright (C) 2012-2014, Sam Stuewe
   Copyright (C) 2014, Johannes Löthberg

shaman
======

Synopsis
--------

**shaman** [*options*]

Description
-----------

:program:`shaman` is a very lightweight application to fetch weather data.

If shaman is not given a location, it will take the data from the cache file. If shaman is not given a format string, it will default to "%c (%t°U)" (where "U" is either "F" or "C" depending on the units requested).

Caching and Refreshing
----------------------

Shaman will look for the cache in the following locations stopping upon the first file found:

:file:`$XDG_CONFIG_HOME/shaman/SCALE.json`

:file:`$HOME/.shaman/SCALE.json`

In both of the paths listed above, :option:`SCALE` may be either :option:`imperial` or :option:`metric` depending on the units requested. In accordance with OpenWeatherMap's API, a call for new data should be made no more often than once per 10 minutes per device. Shaman will, therefore, automatically use data from the cache (assuming the requested location and units are the same) until the file's last-modified timestamp is over 10 minutes old (or the :option:`--refresh` flag has been passed).

Options
-------

-h, --help
    Print usage information and exit

-c CACHE, --cache=CACHE
    Use the file located at :option:`CACHE` for data caching

-f FORMAT, --format=FORMAT
    Specify an output format string

-i, --imperial
    Print information using Imperial units

-l LOCATION, --location=LOCATION
    Specify location to query. Note that :option:`LOCATION` should be formatted as "City Name,CC" where "CC" is the ISO 3166 two-digit Country Code.

-m, --metric
    Print information using Metric units

-q, --quiet
    Silence all output. See the section on Return Values for more information

-r, --refresh
    Force updating the cache

-v, --verbose
    Show more output

Format string
-------------

The following format specifiers may be used in the format string::

    %%    A literal percent sign
    %a    Time of data receipt       (unixtime)
    %b    Barometric pressure        (hectoPascals)
    %c    Weather condition string
    %C    Weather condition code
    %d    Cloud coverage             (percent)
    %h    Low temperature            (°F | °C)
    %H    High temperature           (°F | °C)
    %i    City ID number
    %I    City name
    %j    Country Code               (ISO3166)
    %l    Latitude                   (degrees)
    %L    Longitude                  (degrees)
    %p    Precipitation              (in | mm)
    %P    Relative humidity          (percent)
    %s    Time of sunrise            (unixtime)
    %S    Time of sunset             (unixtime)
    %t    Current temperature        (°F | °C)
    %w    Sustained wind speed       (mph|mps)
    %W    Gusting wind speed         (mph|mps)
    %x    Wind heading               (degrees)
    %X    Wind heading               (Cardinal direction)

All other valid escape characters may also be used (e.g., \\n and \\\\).

Return value
------------

When called without the :option:`-q` flag, shaman will return 0 on success else it will return 1. However, when called with :option:`-q`, shaman's return code will be set from 1 to 9 (1 indicating failure, and each other code representing one of the following groups of weather conditions)::

    2     Thunderstorm
    3     Drizzle
    5     Rain
    6     Snow
    7     Reduced Visibility
    8     Clouds (includes "Clear Skies")
    9     Extreme Hazards or Miscellaneous

See also
--------

:manpage:`libcurl(3)`, :manpage:`libweather(3)`
