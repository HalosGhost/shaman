Shaman
======
Originally inspired by `weatherman <http://darkhorse.nu/weatherman/>`_, ``shaman`` is a small command-line application written in C for fetching current weather conditions from the `National Weather Service <http://forecast.weather.gov>`_.

**NOTE**: for anyone following ``shaman``'s development, there have been some new developments which will mean a significant change to both its goal, style and function.
Stay tuned for a big announcement.

Status & Documentation
----------------------
``shaman`` is being actively developed and re-coded from the ground-up to be all in native C.
The best ways to follow its status and functionality are to track the commit list and keep up with the To-Do lists below.

Short Term To-Do
----------------
These are tasks that must be done in order for ``shaman`` to be considered functional and stable:

#. Parse user-supplied ``formatString``
#. Fetch dwml file with ``libcurl`` according to the NWS API and user-supplied ``formatString``
#. Parse fetched dwml with ``libxml``
#. Re-parse ``formatString`` and do the conversions with the fetched data

Long Term To-Do
---------------
These are tasks that offer some very nice functionality or usability features but do not make sense to pursue until the short-term tasks have been completed:

#. Update ``_shaman`` (zsh completion) for the re-code
#. Provide a native C library and interface (rework ``shaman`` to use this interface)
#. Support non-Linux platforms? (some may already be supported)

Contributions
-------------
All contributions are welcome (pull requests, constructive criticism, feature requests and musings).
All pull requests should follow my `C style guide <http://github.com/HalosGhost/styleguides/blob/master/C.rst>`_ so as to keep code formatting consistent and readable.
Any proposed commit which does not follow the style guide may be subject to rejection or reformatting for conformance before being merged.
