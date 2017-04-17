MCQMCIntegration
================
Library to calculate Monte-Carlo and Quasi Monte-Carlo Integration.

REQUIREMENT
===========
* C++11 or C++14
* LITTLE ENDIAN System

INSTALL
=======

    ./configure --prefix=YOUR_INSTALL_DIR
    make
    make install

PREPARE
=======
set environment variable SOBOL_PATH to point a directory whici include
sobolbase.dat. for example;

    export SOBOL_PATH=$HOME/QMCIntegration/Sobol/data

set LIBRARY_PATH and CPLUS_INCLUDE_PATH properly.

DOCUMENT
========
- [doc/index.html](./doc/index.html)
- [GitHub Project](https://github.com/MersenneTwister-Lab/MCQMCIntegration)
- [GitHub Web Page](https://mersennetwister-lab.github.io/MCQMCIntegration/)

LICENSE
=======
    Copyright (c) 2008, Frances Y. Kuo and Stephen Joe.
    All rights reserved.

    Copyright (C) 2017 Shinsuke Mori, Makoto Matsutmoto, Mutsuo Saito
    and Hiroshima University.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
