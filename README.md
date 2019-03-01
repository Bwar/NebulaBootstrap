English | [中文](/README_cn.md)    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Bwar's blog](https://www.bwar.tech).
# NebulaBootstrap: a distributed systems solution build on nebula.
[![Author](https://img.shields.io/badge/author-@Bwar-blue.svg?style=flat)](cqc@vip.qq.com)  ![Platform](https://img.shields.io/badge/platform-Linux-green.svg?style=flat) [![License](https://img.shields.io/github/license/mashape/apistatus.svg)](LICENSE)<br/>

1. [Overview](#Overview)
2. [License](#License)
3. [Getting start](#GettingStart)
4. [Documentation](#Documentation)
5. [Depend on](#DependOn)
6. [Todo list](#TODO)
7. [Change log](#ChangeLog)

<a name="Overview"></a>
## Overview 

NebulaBootstrap provides service for developers to quickly build a distributed systems (including configuration management, service discovery, routing, load balancing, leadership election, distributed sessions, cluster state and so on). 
![nebula_cluster](image/nebula_cluster.png)

<a name="License"></a>
## License 

MIT License

>  Copyright (c) 2018 Bwar
>
>  Permission is hereby granted, free of charge, to any person obtaining a copy
>  of this software and associated documentation files (the "Software"), to deal
>  in the Software without restriction, including without limitation the rights
>  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
>  copies of the Software, and to permit persons to whom the Software is
>  furnished to do so, subject to the following conditions:
>
>  The above copyright notice and this permission notice shall be included in
>  all copies or substantial portions of the Software.
>
>  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
>  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
>  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
>  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
>  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
>  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
>  THE SOFTWARE.

<a name="GettingStart"></a>
## Getting start
   NebulaBootstrap was consist of [NebulaBeacon](https://github.com/Bwar/NebulaBeacon), [NebulaInterface](https://github.com/Bwar/NebulaHttp), [NebulaAccess](https://github.com/Bwar/NebulaAccess), [NebulaLogic](https://github.com/Bwar/NebulaLogic), NebulaMydis, NebulaDbAgent and [NebulaLogger](https://github.com/Bwar/NebulaLogger), and they were builded passing with gcc6.4 on centos6.5(upgrade binutils to 2.22 or later) and centos7.4.  
   NebulaBootstrap provides automate building and deploying.
   
   automate building and deploying follow these step：
   1. wget https://github.com/Bwar/NebulaBootstrap/archive/master.zip
   2. unzip master.zip; rm master.zip; mv NebulaBootstrap-master NebulaBootstrap
   3. cd NebulaBootstrap
   4. chmod u+x deploy.sh
   5. ./deploy.sh
   
   Run deploy.sh, the NebulaBootstrap distributed services were build completed. The reliance of Nebula was also automatically downloaded and compiled by deploy from the Internet before the construction of Nebula. The deploy path as follows:
* NebulaBootstrap
  + bin &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; server bin location。
  + build &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; build path，created by deploy.sh, if you do not need to build again, just delete it.(optional)。
  + conf &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; configuration path.
  + data &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; application data path. e.g. [Nebio](https://github.com/Bwar/Nebio) which is a data collect and real-time analysis application, write it's data to this path (optional).
  + lib &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; library path.
  + log &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; program logs path.
  + plugins &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; plugins path.
    - logic &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; plugins for logic server(optional).
  + script &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; script path. deploy.sh, startup.sh and shutdown.sh were depend on this path.
  + temp &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; temp file path(optional).
  - configure.sh &nbsp;&nbsp;&nbsp; run configure.sh for a simple configuration when deploy for the first time.
  - deploy.sh &nbsp;&nbsp;&nbsp;&nbsp; auto build and deploy.
  - shutdown.sh &nbsp;&nbsp;&nbsp;&nbsp; shutdown one or more server.
  - startup.sh &nbsp;&nbsp;&nbsp;&nbsp; startup one or more server.
  - README_cn.md              
  - README.md  

  build completed, you can start the server:
```
./configure.sh
./startup.s
```

<a name="Documentation"></a>
## Documentation 
   The complete documentation for NebulaBootstrap is coming...
    
<a name="DependOn"></a>
## Depend on 
   * [protobuf](https://github.com/google/protobuf)
   * [libev](http://software.schmorp.de/pkg/libev.html) or [libev](https://github.com/kindy/libev)
   * [hiredis](https://github.com/redis/hiredis)
   * [crypto++](http://www.cryptopp.com)
   * [http_parse](https://github.com/nodejs/http-parser) integrate into Nebula/src/util/http 
   * [CJsonObject](https://github.com/Bwar/CJsonObject) integrate into Nebula/src/util/json

<a name="TODO"></a>
## Todo list 
   - NebulaMydis Data Agency Service.
   - Developing an IM with the Nebula.

<a name="ChangeLog"></a>
## Change log 
#### v0.6
   - NebulaBeacon adds node status information query, registration center leader-fllower election.
   - NebulaInterface adds hello demo.
#### v0.5
   - add node info to worker the worker process terminated unexpectedly and restarted by the Manager.
   - ipv6 support.
#### v0.4
   - distributed log service test passing.
   - add https support.
   - add keep alive settings to http channel.
   - replace repeated http headers with proto3 map.
   - provides a symmetric encryption key setup interface for channel.
   - bug fix.
#### v0.3
   - rewrite with C++14
   - create actors by reflection (using template)
   - add distributed trace log
#### v0.2
   - the first runable version

<br/>

