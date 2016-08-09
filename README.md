#[Boost Process (Boost.Process)](https://github.com/klemens-morgenstern/boost-process)

Boost.process is not yet part of the [Boost C++ Libraries](http://github.com/boostorg). It is a library for comfortable management of processes.

### Test results

Branches        | Build         | Tests coverage | 
----------------|-------------- | -------------- |
Develop:        | [![Build Status](https://travis-ci.org/klemens-morgenstern/boost-process.svg?branch=develop)](https://travis-ci.org/klemens-morgenstern/boost-process)  [![Build status](https://ci.appveyor.com/api/projects/status/peup7e6m0e1bb5ba?svg=true)](https://ci.appveyor.com/project/klemens-morgenstern/boost-process) | [![Coverage Status](https://coveralls.io/repos/github/klemens-morgenstern/boost-process/badge.svg?branch=develop)](https://coveralls.io/github/klemens-morgenstern/boost-process?branch=develop) |
Master:         | [![Build Status](https://travis-ci.org/klemens-morgenstern/boost-process.svg?branch=master)] (https://travis-ci.org/klemens-morgenstern/boost-process) [![Build status](https://ci.appveyor.com/api/projects/status/peup7e6m0e1bb5ba/branch/master?svg=true)](https://ci.appveyor.com/project/klemens-morgenstern/boost-process/branch/master)
  | [![Coverage Status](https://coveralls.io/repos/github/klemens-morgenstern/boost-process/badge.svg?branch=master)](https://coveralls.io/github/klemens-morgenstern/boost-process?branch=master)   |

[Open Issues](https://github.com/klemens-morgenstern/boost-process/issues)

[Latest developer documentation](http://klemens-morgenstern.github.io/process/)

### About
This library is the current result of a long attempt to get a boost.process library, which is going on since 2006.

### License
Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/LICENSE_1_0.txt).

### Dependency

This library requires boost 1.62. Since this is not released yet you can clone the winapi module from [here](https://github.com/boostorg/winapi) to get it to work. You will need to overwrite the current code in boost/libs/winapi.
