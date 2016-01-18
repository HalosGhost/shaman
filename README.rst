Shaman
======

Originally inspired by a tool called weatherman, ``shaman`` is a small, native C utilty relying on a small, native C library (``libweather``) to fetch current weather conditions.
For the moment, ``libweather`` and ``shaman`` support grabbing weather from `OpenWeatherMap <http://openweathermap.org>`_ (which draws data from various sources accounting for nearly 80000 cities).
However, because the library uses a source-agnostic data structure, other weather providers can be easily added.
The only requirement for a new provider (which may become less stringent) is that they have a JSON API.

If you would like to see support for a particular weather provider added to ``shaman`` and ``libweather``, please open a bug report listing the name of the provider, along with a link to their JSON API.
*NOTE*: Implementing other providers is a lower priority than getting ``shaman`` and ``libweather`` usable with current functionality.

Dependencies
------------
``shaman`` and ``libweather`` are being written to use as few dependencies as possible while still trying to keep the utilities themselves light-weight.
To that end, they now both require only two external dependencies:

* `curl <http://curl.haxx.se/libcurl/>`_, which handles all network interaction very gracefully
* `jansson <http://www.digip.org/jansson/>`_, which allows for very efficient JSON Parsing
* `tup <http://gittup.org/tup/>`_, the build system that I will use going forward
* `sphinx <http://sphinx-doc.org/>`_, for building the man-page from reStructuredText

For a variety of reasons, mainly because ``shaman`` does not rely on parallelization at the moment and because of ``-Weverything``, I will no longer be officially supporting ``gcc``.
Resultingly, to use the official build system, you will need ``clang`` installed.
Ideally, when I get a continuous integration setup again, I will have the build tested against both compilers.

Status & Documentation
----------------------
``shaman`` is still under heavy active development.
For the moment, the manual documentation is quite out-of-date.
The best ways to follow its status and functionality are to track the commit list and keep up with the `To-do lists <http://github.com/HalosGhost/shaman/blob/master/TODO.rst>`_.

Contributions
-------------
All contributions are welcome (pull requests, constructive criticism, feature requests and musings), but pull requests should attempt to follow `these guidelines <http://github.com/HalosGhost/styleguides/blob/master/C.rst>`_ for consistency.
I am likely to reformat any commit that should deviate from these guidelines.
