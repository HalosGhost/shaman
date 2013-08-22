Shaman
======
Originally inspired by `weatherman <http://darkhorse.nu/weatherman/>`_, ``shaman`` is a small command-line application written in C for fetching current weather conditions (and perhaps, eventually, fetching forecasts) from the `NOAA <http://forecast.weather.gov>`_.

For those who aren't familiar with the concept of a forecast stone, think of it like this. This software's most basic function is, essentially, a digital form of this:

.. image:: http://www.theweatherstone.co.uk/images/slide1.jpg

Status
------
``shaman`` now correctly grabs and parses current conditions, temperature, humidity, dew point, visibility and pressure. It is also capable of displaying its information with metric units.

To-Do
-----
#. Add wind-speed, type and direction to extended info
#. Allow specifying a location by "City, ST" and not just through a zipcode
#. Do more intelligent rounding for metric unit printing *or* print as a ``float``, not an ``int``
#. Add 7-day forecasts

Possible Additions
------------------
These are features that might take a disproportionate amount of work to implement for their gain, or detriment the final product because of how they might have to be implemented. As a result, they are not planned features, but rather, things I am mulling over for inclusion:

* Hazardous/Severe Weather Warnings
* Allowing compound arguments (e.g., ``-cprt`` as opposed to ``-c -p -r -t``)
