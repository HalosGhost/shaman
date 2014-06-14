Shaman
======
Originally inspired by `weatherman <http://darkhorse.nu/weatherman/>`_, ``shaman`` is a small command-line application written in C for fetching current weather conditions from the `National Weather Service <http://forecast.weather.gov>`_.

**NOTE**: for anyone following ``shaman``'s development, there have been some new developments which will mean a significant change to both its goal, style and function.
For a variety of reasons, ``shaman`` will no longer use the National Weather Service of the US government for weather data. 
In its place, I have decided to use `OpenWeatherMap <http://openweathermap.org>`_.

A large portion of the recode is finished, and over the next few days, the repo will have some significant changes.
Hopefully, ``shaman`` 2.x will be ready for testing shortly.

Status & Documentation
----------------------

.. image:: https://travis-ci.org/HalosGhost/shaman.png

``shaman`` is being actively developed and re-coded from the ground-up to be all in native C.
The best ways to follow its status and functionality are to track the commit list and keep up with the `To-do lists <http://github.com/HalosGhost/shaman/blob/master/TODO.rst>`_.

Contributions
-------------
All contributions are welcome (pull requests, constructive criticism, feature requests and musings).
All pull requests should follow my `C style guide <http://github.com/HalosGhost/styleguides/blob/master/C.rst>`_ so as to keep code formatting consistent and readable.
Any proposed commit which does not follow the style guide may be subject to rejection or reformatting for conformance before being merged.

**NOTE**: With the recode, my C style has changed significantly.
The styleguide mentioned above is thus, out-of-date.
It will be updated after the repository is fully transitioned to the new code.
