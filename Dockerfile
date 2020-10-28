FROM        ubuntu:20.04

LABEL       maintainer "Bert de Jong, wadejong@lbl.gov"

ENV         LC_ALL C.UTF-8
ENV         LANG C.UTF-8
ENV         DEBIAN_FRONTEND "noninteractive" 
ENV         TZ "US"

RUN         apt-get update \
            && apt-get -y install software-properties-common

RUN         apt-get update \
            && apt-get -y upgrade \
            && apt-get -y install git \
            && apt-get -y install libboost-all-dev libgslcblas0 libgsl-dev \
            && apt-get -y install -f clang-format-9 \
            && apt-get -y install -f doxygen \
            && apt-get -y install libeigen3-dev wget vim

ENV         cmake_v "3.16.3"
ENV         cmake_url_base "https://github.com/Kitware/CMake/releases/download"
ENV         arch "Linux-x86_64"
ENV         script_name "cmake-${cmake_v}-${arch}.sh"

RUN         wget "${cmake_url_base}/v${cmake_v}/${script_name}" \
            && yes | /bin/sh "${script_name}"

ENV         gnu_v 9
ENV         gcc_no_v "/usr/bin/gcc"
ENV         gcc_v "${gcc_no_v}-${gnu_v}"
ENV         gxx_no_v "/usr/bin/g++"
ENV         gxx_v "${gxx_no_v}-${gnu_v}"
ENV         gfort_no_v "/usr/bin/gfortran"
ENV         gfort_v "${gfort_no_v}-${gnu_v}"
ENV         gcov_no_v "/usr/bin/gcov"
ENV         gcov_v "${gcov_no_v}-${gnu_v}"

RUN         add-apt-repository ppa:ubuntu-toolchain-r/test \
            && apt-get update \
            && apt-get -y install "gcc-${gnu_v}" "g++-${gnu_v}" "gfortran-${gnu_v}" \
            && update-alternatives --install "${gcc_no_v}" gcc "${gcc_v}" 95 \
                           --slave "${gxx_no_v}" g++ "${gxx_v}" \
                           --slave "${gfort_no_v}" gfortran "${gfort_v}" \
                           --slave "${gcov_no_v}" gcov "${gcov_v}"

RUN         apt-get update \
            && apt-get -y install liblapacke liblapacke-dev \
            && apt-get -y install libopenblas-base libopenblas-dev \
            && apt-get -y install openmpi-bin libopenmpi-dev \
            && apt-get -y install libscalapack-openmpi-dev 

RUN         apt-get -y install python3-pip

RUN         python3 -m pip install cppyy

RUN         apt-get update \
            && apt-get -y install -f python3-venv \
            && python3 -m venv venv \
            && . venv/bin/activate \
            && python -m pip install sphinx sphinx_rtd_theme

ENV         arch Linux-x86_64
ENV         cmake_root ${pwd}/cmake-"${cmake_v}"-"${arch}"
ENV         cmake_command "${cmake_root}/bin/cmake"
ENV         ctest_command "${cmake_root}/bin/ctest"

ENV         toolchain_file ${pwd}/toolchain.cmake

WORKDIR     /app
RUN         wget https://github.com/evaleev/libint/releases/download/v2.6.0/libint-2.6.0.tgz \
            && tar -zxf libint-2.6.0.tgz
WORKDIR     /app/libint-2.6.0
RUN         ${cmake_command} -H. -Bbuild -DCMAKE_INSTALL_PREFIX=/app/install \
            -DCMAKE_CXX_FLAGS="-std=c++17" -DBUILD_SHARED_LIBS=ON
WORKDIR     /app/libint-2.6.0/build
RUN         make ; make install

RUN         echo "set(BUILD_TESTING ON)" > "${toolchain_file}" \
            && echo "set(CMAKE_CXX_STANDARD 17)" >> "${toolchain_file}" \
            && echo 'set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)' >> "${toolchain_file}" \
            && echo "set(BUILD_SHARED_LIBS ON)" >> "${toolchain_file}" \
            && echo "set(BUILD_TESTING ON)" >> "${toolchain_file}" \
            && echo "set(CATCH_ENABLE_COVERAGE ON)" >> "${toolchain_file}" \
            && echo "set(CMAKE_PREFIX_PATH /app/install)" >> "${toolchain_file}" \
            && echo 'set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -std=c++17")' >> "${toolchain_file}" \
            && echo 'set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOMPI_SKIP_MPICXX")' >> "${toolchain_file}" \
            && echo 'set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")' >> "${toolchain_file}" \
            && echo 'set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs")' >> "${toolchain_file}" \
            && echo 'set(CPP_GITHUB_TOKEN "${CPP_GITHUB_TOKEN}")' >> "${toolchain_file}" \
            && echo 'set(CMAKE_BUILD_TYPE Debug)' >> "${toolchain_file}" \
            && echo 'set(ENABLE_SCALAPACK ON)' >> "${toolchain_file}" \
            && echo 'set(LIBDIR "/usr/lib/x86_64-linux-gnu")' >> "${toolchain_file}" \
            && echo 'set(LAPACK_LIBRARIES "-L${LIBDIR} -lblas -llapack")' >> "${toolchain_file}" \
            && echo 'set(SCALAPACK_LIBRARIES  "-L${LIBDIR} -lscalapack-openmpi")' >> "${toolchain_file}" \
            && echo 'set(blacs_LIBRARIES ${SCALAPACK_LIBRARIES})' >> "${toolchain_file}" \
            && echo 'set(scalapack_LIBRARIES ${SCALAPACK_LIBRARIES})' >> "${toolchain_file}" \
            && echo 'set(lapack_LIBRARIES ${LAPACK_LIBRARIES})' >> "${toolchain_file}"

WORKDIR     /app
RUN         git clone --branch python https://${CPP_GITHUB_USER}:${CPP_GITHUB_TOKEN}@github.com/NWChemEx-Project/NWChemEx.git 
WORKDIR     /app/NWChemEx
RUN         ${cmake_command} -H. -Bbuild -DCMAKE_TOOLCHAIN_FILE="${toolchain_file}" ; ${cmake_command} --build build
WORKDIR     /app/NWChemEx/build 
RUN         ${ctest_command} -VV 
WORKDIR     /app
