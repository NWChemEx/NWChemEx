.. Copyright 2022 NWChemEx-Project
..
.. Licensed under the Apache License, Version 2.0 (the "License");
.. you may not use this file except in compliance with the License.
.. You may obtain a copy of the License at
..
.. http://www.apache.org/licenses/LICENSE-2.0
..
.. Unless required by applicable law or agreed to in writing, software
.. distributed under the License is distributed on an "AS IS" BASIS,
.. WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
.. See the License for the specific language governing permissions and
.. limitations under the License.

Running NWChemEx
================

Although could be made possible, NWChemEx usually does not run in the same 
way as other conventional quantum chemistry packages do: obtaining all 
calculation settings from a text input file. To increase the flexibility of
setting calculation routes and facilitate post-processing of calculation 
results, we recommend running NWChemEx with Python scripts. Once the NWChemEx 
package is successfully compiled, the python bindings of all the C++ modules 
are generated with the tool 
`cppyy <https://cppyy.readthedocs.io/en/latest/>`_. 
An example of running an SCF calculation with the Python bindings can be 
found in ``NWChemEx/tests/test_python.py``. We encourage our users to write 
their own python scripts for the special needs in their calculations.


