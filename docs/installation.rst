Installation
============

.. note::

   This library can be used to write applications which send CQC messages to a backend which can listen to these messages.
   This library does not come with a backend but you can use the simulator `SimulaQron <http://www.simulaqron.org/>`_ as a backend.

Installation
------------

The library is available by cloning `repo <https://github.com/SoftwareQuTech/CQC-C>`_.

The library is available in two files `cqc.h` and `cqc.c` and it is up to you how to integrate them into your build system.

To verify that the library is able to compile and run on your machine you can run the tests included in this repository:

.. code-block:: bash

   test/test_using_simulaqron.sh

This will start SimulaQron and build and run the tests.
