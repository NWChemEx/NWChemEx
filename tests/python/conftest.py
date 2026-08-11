#
# Copyright 2026 NWChemEx-Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import parallelzone as pz
import pytest


@pytest.fixture(scope="session", autouse=True)
def _session_runtime_view():
    """
    Holds a single RuntimeView for the whole pytest session.

    MPI may only be initialized/finalized once per process. The first
    RuntimeView constructed owns that responsibility; individual test
    modules construct their own RuntimeView per test (e.g. in setUp),
    which is safe only as long as this session-scoped instance is still
    alive to keep MPI initialized in between. Without this, pytest would
    run each test module independently and MPI would be finalized after
    the first module's tests finished, breaking every module after it.
    """
    rv = pz.runtime.RuntimeView()
    yield rv
