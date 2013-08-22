Shaman
======
Originally inspired by `weatherman <http://darkhorse.nu/weatherman/>`_, ``shaman`` is a small command-line application written in C for fetching current weather conditions (and perhaps, eventually, fetching forecasts) from the `NOAA <http://forecast.weather.gov>`_.

For those who aren't familiar with the concept of a forecast stone, think of it like this. This software's most basic function is, essentially, a digital form of this:

.. image:: http://www.theweatherstone.co.uk/images/slide1.jpg

Status
------
``shaman`` now correctly grabs and parses current conditions, temperature, humidity and dew point.

To-Do
-----
#. Add wind-speed, type and direction to extended info
#. Correctly allow use of metric units (may involve a work-around until upstream fixes a bug)
#. Add 7-day forecasts
