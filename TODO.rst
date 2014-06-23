To-Do Lists
===========
A note about priorities.
The most pressing issues (those that hold this project back from being daily usable) are the following:

#. Improving and standardizing internal error handling (``libweather`` and ``shaman``)
#. Implmenting caching and freshening (``shaman``)

Expect GitHub milestone markers to start popping up, as I intend to get this project to be fairly functional in the not-too-distant future.

General
-------

* Update documentation
* Update ``_shaman`` (zsh completion)
   * Add Bash completion?

libweather
----------

* Add support for formatting time variables
* Split ``fetch_data_file()``, ``fetch_data_owm()`` and ``read_weather()`` into a separate header (tentatively ``owm.h``)
   * Probably rename to avoid name conflicts (tentatively, ``fetch_owm_local()``, ``fetch_owm_remote()`` and ``read_owm()``)
   * *Note*: this will likely signal the end of ``libweather`` being replaced by a library per-provider
* Add function for declaring an API key

shaman
------

* Implement the rest of the planned initial feature set:
   * Caching and Freshening

test suite
----------

* Add unit tests for ``shaman``
