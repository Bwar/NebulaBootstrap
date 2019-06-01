[English](/README.md) | 中文    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Bwar的技术博客](https://www.bwar.tech).
# NebulaBootstrap : 基于Nebula的分布式集群
[![Author](https://img.shields.io/badge/author-@Bwar-blue.svg?style=flat)](cqc@vip.qq.com)  ![Platform](https://img.shields.io/badge/platform-Linux-green.svg?style=flat) [![License](https://img.shields.io/github/license/mashape/apistatus.svg)](LICENSE)<br/>

1. [概述](#Overview)
2. [许可证](#License)
3. [编译](#Building)
4. [文档](#Documentation)
5. [依赖](#DependOn)
6. [开发任务](#TODO)
7. [版本历史](#ChangeLog)

<a name="Overview"></a>
## 概述 

NebulaBootstrap是为开发人员提供快速构建分布式系统（包括配置管理，服务发现，路由，负载平衡，领导选举，分布式会话，集群状态等）的服务解决方案。
![nebula_cluster](image/nebula_cluster.png)

<a name="License"></a>
## 许可证
> Copyright（c）2018 Bwar
>
> 特此免费授予任何人获得本软件及相关文档文件（“软件”）的副本，以无限制地处理本软件，包括但不限于使用，复制，修改和合并，发布，分发，再许可和/或销售本软件的副本，并允许本软件提供给其的人员遵守以下条件：
>
> 上述版权声明和本许可声明应包含在本软件的所有副本或主要部分中。
>
> 本软件按“原样”提供，不附有任何形式的明示或暗示保证，包括但不限于适销性，适用于特定用途和不侵权的保证。在任何情况下，作者或版权所有者都不承担任何索赔，损害或其他责任，无论是在合同，侵权或其他方面的行为，不论是由软件或其使用或其他交易引起或与之相关的行为。

<a name="Building"></a>
## 编译
Nebula需要gcc4.8以上的编译器。

自动构建和部署步骤：
   1. wget https://github.com/Bwar/NebulaBootstrap/archive/master.zip
   2. unzip master.zip; rm master.zip; mv NebulaBootstrap-master NebulaBootstrap
   3. cd NebulaBootstrap
   4. chmod u+x deploy.sh
   5. ./deploy.sh
                  
&emsp;&emsp;执行deploy脚本后即完成了Nebula及NebulaBootstrap分布式服务的编译和部署，Nebula的依赖也由deploy在构建Nebula前自动从网上下载并编译部署。虽然不像autoconf、automake那样众所周知，但deploy脚本完成的不止是autoconf、automake的工作。deploy之后的目录结构如下：
* NebulaBootstrap
  + bin &emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&nbsp; server的bin文件存放路径。
  + build &emsp;&emsp;&emsp;&emsp;&emsp;&nbsp; 构建路径，由deploy.sh生成，如果部署完后不需要再构建，可以直接删掉(可选)。
  + conf &emsp;&emsp;&emsp;&emsp;&emsp;&emsp; 配置文件存放路径。
  + data &emsp;&emsp;&emsp;&emsp;&emsp;&emsp; 数据文件存放路径，比如基于Nebula开发的页面埋点数据采集和实时分析[Nebio](https://github.com/Bwar/Nebio)项目，将采集的数据落地到这个路径（可选）。
  + lib &emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&nbsp; 运行所需的库文件存放路径。
  + log &emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&nbsp; 程序日志存放路径。
  + plugins &emsp;&emsp;&emsp;&emsp;&nbsp; 插件（动态加载的业务逻辑so）存放路径。
    - logic &emsp;&emsp;&emsp;&emsp;&nbsp; 逻辑Server插件存放路径，插件按server存放只是为了好区分，也可直接存放在plugins下，具体规则可自定义（可选）。
  + script &emsp;&emsp;&emsp;&emsp;&emsp; 脚本库存放路径，deploy.sh startup.sh shutdown.sh等脚本都需要依赖这个路径。
  + temp &emsp;&emsp;&emsp;&emsp;&emsp;&emsp; 临时文件存放路径(可选)。
  - configure.sh &emsp;&emsp;&emsp; 配置脚本，deploy之后第一次启动server之前先执行该脚本做简单的配置修改，也可以逐个配置文件打开直接修改。
  - deploy.sh &emsp;&emsp;&emsp;&emsp; 自动构建和部署脚本，自动下载并安装依赖，自动构建和部署，执行./deploy.sh --help查看帮助。
  - shutdown.sh &emsp;&emsp;&emsp;&nbsp; 关闭server，可以指定关闭一个或多个server，也可关闭所有server，不带参数时关闭所有server（需用户确认）。
  - startup.sh &emsp;&emsp;&emsp;&emsp; 启动server，可以指定启动一个或多个server，也可启动所有server。
  - README_cn.md              
  - README.md              

&emsp;&emsp;构建完成后，可以开始启动服务集群了：
```
./configure.sh
./startup.sh
```
&emsp;&emsp;server应该已经启动成功了，startup.sh会打印已启动的server。如果没有启动成功，可以到log目录查看原因。执行grep "ERROR" log/*和grep "FATAL" log/* 先看看是否有错误，再到具体日志文件查看错误详情。注意，Nebula的默认配置文件对IP单位时间连接次数做了限制，如果在测试量较大发生莫名奇妙的问题，可以修改配置限制，通过查看日志中的WARNING信息通常有助于定位这种不是错误的“错误”。如果server已启动成功，那么可以用postman、curl等做测试，看看结果。
```
# 只启动NebulaInterface即可完成http的hello测试
curl -H "Content-Type:application/json" -X POST -d '{"name": "Nebula", "address":"https://github.com/Bwar/Nebula"}' http://${your_ip}:16003/hello

# 启动NebulaInterface、NebulaLogic和NebulaBeacon完成分布式服务http的hello测试。
curl -H "Content-Type:application/json" -X POST -d '{"name": "Nebula", "address":"https://github.com/Bwar/Nebula"}' http://${your_ip}:16003/hello_nebula
```
&emsp;&emsp;这个简单的测试可以只启动一个NebulaInterface即可完成，也可以启动分布式服务完成。NebulaBootstrap提供基于集群和单个Server的HelloWorld，基于集群的HelloWorld启动了NebulaBeacon、NebulaInterface、NebulaLogic三个server。下面是一张集群架构图：

<a name="Documentation"></a>
## 文档
Nebula 完成的文档在 [Nebula documentation](https://github.com/Bwar/Nebula/wiki)，持续更新中。

<a name="DependOn"></a>
## 依赖 
   * [protobuf](https://github.com/google/protobuf)
   * [libev](http://software.schmorp.de/pkg/libev.html) 或 [libev](https://github.com/kindy/libev)
   * [hiredis](https://github.com/redis/hiredis)
   * [crypto++](http://www.cryptopp.com)
   * [http_parse](https://github.com/nodejs/http-parser) 已集成到 Nebula/src/util/http
   * [CJsonObject](https://github.com/Bwar/CJsonObject) 已集成到 Nebula/src/util/json

<a name="TODO"></a>
## 开发任务
   - NebulaMydis数据代理服务
   - 应用Nebula开发IM项目

<a name="ChangeLog"></a>
## 版本历史
#### v0.8
   - 兼容gcc4.8编译器（从这个版本起无须另行安装5以上gcc版本，可以无障碍无等待地在个人机器上部署和测试，也为应用于生产铺平道路。之前Bwar的埋点数据采集和实时分析的生产项目Nebio是在服务器上安装了gcc6才部署的。）
   - 增加CPU亲和度设置以支持将Worker进程绑定CPU功能。(有人测试过繁忙的多核服务器，绑定CPU比不绑定CPU有20%左右的性能提升，现在Nebua可以让开发者自行选择是否绑定CPU)
   - 增加动态库（业务插件）卸载功能。（支持不停服务升级的重要功能）
#### v0.7
   - 添加配置管理，NebulaBeacon为配置中心，使用说明见命令行管理工具[Nebcli](https://github.com/Bwar/Nebcli)的get和set命令。
#### v0.6
   - NebulaBeacon增加节点状态信息查询，注册中心主从高可用选举
   - NebulaInterface提供HelloWorld示例。
#### v0.5
   - 增加worker进程意外终止并被Manager重新拉起时的节点信息下发
   - 增加ipv6支持
#### v0.4
   - 分布式日志服务测试通过
   - 增加https支持
   - http通道增加keep alive设置
   - 用proto3中的map将http header替换掉原repeated数据类型
   - Channel增加设置对称加密密钥接口
   - 缺陷修复
#### v0.3
   - 用C++14标准改写整个项目
   - 用模板实现反射机制创建actor实例
   - 增加分布式追踪日志
#### v0.2
   - 第一个可运行并http测试通过的版本
<br/>

