To-Do Lists
===========
A note about priorities.
The most pressing issue (that holds this project back from being daily usable) is the following:

#. Improving and standardizing internal error handling (``libweather`` and ``shaman``)

Expect GitHub milestone markers to start popping up, as I intend to get this project to be fairly functional in the not-too-distant future.

General
-------

* Switch to `argp <https://www.gnu.org/software/libc/manual/html_node/Argp.html>`_ instead of ``getopt``
* Update documentation
   * Switch to rst2man instead of latex2man and generate docs in-build
* Update ``_shaman`` (zsh completion)
   * Add Bash completion?

libweather
----------

* Create a ``setopt`` interface rather than passing a huge number of arguments
* Add support for formatting time variables
* Split ``fetch_data_file()``, ``fetch_data_owm()`` and ``read_weather()`` into a separate header (tentatively ``owm.h``)
   * Probably rename to avoid name conflicts (tentatively, ``fetch_owm_local()``, ``fetch_owm_remote()`` and ``read_owm()``)

shaman
------

* Implement a placeholder structure (and option) to specify a provider

test suite
----------

* Add unit tests for ``shaman``
* Stop doing silly things in test suite (e.g., testing for equality on floats)
