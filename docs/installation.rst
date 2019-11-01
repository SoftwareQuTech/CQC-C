Installation
============

.. note::

   This library can be used to write applications which send CQC messages to a backend which can listen to these messages.
   This library does not come with a backend but you can use the simulator `SimulaQron <http://www.simulaqron.org/>`_ as a backend.

Install from source
-------------------

To build from source (including tests), clone the `repo <https://github.com/SoftwareQuTech/CQC-C>`_, cd into the folder and do:

.. code-block:: bash
   
   cd test
   make all

To rum the tests, do:

.. code-block:: bash

   ./test_using_simulaqron.sh
