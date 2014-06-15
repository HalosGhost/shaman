Shaman |jenkins|
================
.. |jenkins| image:: https://praetorian.erebor.buhman.org:2421/job/shaman/badge/icon
   :alt: Jenkins Build Status
   :target: https://praetorian.erebor.buhman.org:2421/job/shaman/

Originally inspired by `weatherman <http://darkhorse.nu/weatherman/>`_, ``shaman`` is a small, native C utilty relying on a small, native C library (``libweather``) to fetch current weather conditions.
For the moment, ``libweather`` and ``shaman`` support grabbing weather from `OpenWeatherMap <http://openweathermap.org>`_ (which draws data from various sources accounting for nearly 80000 cities). 
However, because the library uses a source-agnostic data structure, other weather providers can be easily added.
The only requirement for a new provider (which may become less stringent) is that they have a JSON API.

If you would like to see support for a particular weather provider added to ``shaman`` and ``libweather``, please open a bug report listing the name of the provider, along with a link to their JSON API.
*NOTE*: Implementing other providers is a lower priority than getting ``shaman`` and ``libweather`` usable with current functionality.

Status & Documentation
----------------------
``shaman`` is still under heavy active development.
For the moment, the manual documentation is quite out-of-date.
The best ways to follow its status and functionality are to track the commit list and keep up with the `To-do lists <http://github.com/HalosGhost/shaman/blob/master/TODO.rst>`_.

Contributions
-------------
All contributions are welcome (pull requests, constructive criticism, feature requests and musings), but pull requests should attempt to follow `these guidelines <http://github.com/HalosGhost/styleguides/blob/master/C.rst>`_ for consistency.
I am likely to reformat any commit that should deviate from these guidelines.

**NOTE**: With the recode, my C style has changed significantly.
The styleguide mentioned above is thus, out-of-date.
It will be updated after the repository is fully transitioned to the new code.
