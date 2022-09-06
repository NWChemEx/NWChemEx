/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

namespace madness {

class RMI {
    static thread_local bool is_server_thread;
    static bool* _cling_is_server_thread();
};

thread_local bool RMI::is_server_thread = false;

bool* RMI::_cling_is_server_thread() { return &is_server_thread; }

} // namespace madness
