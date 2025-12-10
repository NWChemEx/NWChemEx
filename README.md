<!--
  ~ Copyright 2022 NWChemEx-Project
  ~
  ~ Licensed under the Apache License, Version 2.0 (the "License");
  ~ you may not use this file except in compliance with the License.
  ~ You may obtain a copy of the License at
  ~
  ~ http://www.apache.org/licenses/LICENSE-2.0
  ~
  ~ Unless required by applicable law or agreed to in writing, software
  ~ distributed under the License is distributed on an "AS IS" BASIS,
  ~ WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  ~ See the License for the specific language governing permissions and
  ~ limitations under the License.
-->

![NWChemEx Logo](https://github.com/NWChemEx/.github/blob/master/assets/logos/banners/Graphic_LogoChemSymbolsLarge.png)

[![Nightly Workflow](https://github.com/NWChemEx/NWChemEx/actions/workflows/nightly.yaml/badge.svg)](https://github.com/NWChemEx/NWChemEx/actions/workflows/nightly.yaml)

[Documentation](https://nwchemex.github.io/NWChemEx)

NWChemEx is an electronic structure package focusing on high-performance
computing and reduced scaling methodologies. NWChemEx attempts to break from the
traditional "monolithic" electronic structure package by decomposing the overall
package into smaller, independent packages called plugins. Each plugin is its
own GitHub repository and can be used as is, without the rest of NWChemEx (aside
from any needed dependencies).

# Features

The NWChemEx repository is essentially a meta-target which brings together the
many modular components developed by the NWChemEx community.

## Plugins

- [SCF](https://github.com/NWChemEx/SCF) - a plugin containing GPU-enabled
  implementations of Hartree-Fock and Density Functional Theory with a focus
  on reduced scaling variants.
- [FriendZone](https://github.com/NWChemEx/FriendZone) - a plugin containing
  interfaces to software packages maintained by the broader computational
  chemistry community.

## Productivity

- [SimDE](https://github.com/NWChemEx/SimDE) - a software developer kit which
  facilitates writing modular computational chemistry plugins compatible with
  PluginPlay and NWChemEx.

# Installation

NWChemEx ultimately uses CMake for configuring and building. This means that
installation is usually achieved via a variation on:

```.sh
git clone https://github.com/NWChemEx/NWChemEx
cd NWChemEx
cmake -H. -Bbuild -D...
cmake --build build --target install
```

More detailed install instructions can be found
[here](https://nwchemex.github.io/NWChemEx/installation/building.html).

# Acknowledgments

This research was supported by the Exascale Computing Project (17-SC-20-SC), a
collaborative effort of the U.S. Department of Energy Office of Science and the
National Nuclear Security Administration.
