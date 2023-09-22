/*
 * Copyright 2023 NWChemEx-Project
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

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <iostream>

int print_and_return(int ii) {
    std::cout << "This is only a blank model for an integration test."
              << std::endl;
    return 0;
}

TEST_CASE("Blank test", "[classic]") { REQUIRE(print_and_return(0) == 0); }
