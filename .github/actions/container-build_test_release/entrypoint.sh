#!/bin/sh -l
# Copyright 2023 NWChemEx-Project
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

set -e

CR_PAT=$1
USER=$2
BASE_TAG=$3
CMAKE_VERSION=$4
GCC_VRESION=$5
CLANG_VERSION=$6
NINJA_BUILD=$7
USE_CLANG=$8
CMAIZE_GITHUB_TOKEN=$9
INSTALL=${10}
TEST=${11}
INTEGRATION_TEST=${12}
BRANCH_NAME=${13}
DEP_REPO_TAG=${14}

cd /docker-action

echo $CR_PAT | docker login ghcr.io -u $USER --password-stdin

docker build -t docker-action --build-arg cr_pat=$CR_PAT --build-arg user=$USER --build-arg base_tag=$BASE_TAG --build-arg gcc_version=$GCC_VRESION --build-arg clang_version=$CLANG_VERSION --build-arg ninja_build=$NINJA_BUILD --build-arg use_clang=$USE_CLANG --build-arg cmaize_github_token=$CMAIZE_GITHUB_TOKEN --build-arg install=$INSTALL --build-arg unit_test=$TEST --build-arg int_test=$INTEGRATION_TEST --build-arg branch_name=$BRANCH_NAME --build-arg dep_repo_tag=${DEP_REPO_TAG} . && docker run docker-action 

container_id="$(docker ps -a  | grep 'docker-action')"
container_id="${container_id%% *}"
docker cp ${container_id}:/install ./install

cat <<EOF>> release.Dockerfile
FROM scratch
ADD install /install
EOF

if [ "${USE_CLANG}" = true ]; then
   docker build -t ghcr.io/nwchemex-project/release_nwchemex:clang-latest -f release.Dockerfile .
   docker push ghcr.io/nwchemex-project/release_nwchemex:clang-latest
else
   docker build -t ghcr.io/nwchemex-project/release_nwchemex:gcc-latest -f release.Dockerfile .
   docker push ghcr.io/nwchemex-project/release_nwchemex:gcc-latest
fi
