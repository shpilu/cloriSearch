[English Version](README.md)

cloriSearch<div id="top"></div>
==========

cloriSearch是一套检索引擎内核，致力于解决常规业务开发中的策略检索与在线广告匹配问题，为中小互联网公司商业化提供一套开源的广告检索技术方案。

* [主要特性](#features)
* [设计思想](#design)
* [实例](#usage)
* [安装](#install)
* [待完成列表](#todo)
* [作者](#authors)

## 主要特性<div id="features"></div>

cloriSearch目前支持简单倒排检索、范围检索、LBS检索等三种检索方式(基本能cover除搜索广告之外的其他广告定向条件)，这三类检索方式的具体内涵是指

* 简单倒排检索 - 最基础的检索方式，term的值是一个有限集合，比如 city ∈ {北京, 上海, 深圳}，gender ∈ {male}或者device ∈ {iOS}
* 范围检索 - 广告定向条件是一个数值区间，比如"时间从2018-12-01 00:00 ~ 2018-12-20 00:00"，"年龄在18 ~ 25之间的用户"，"流量序号在20 ~ 50之间"
* LBS检索 - 基于地理位置(经纬度)的广告定向，比如检索出以某经纬度为圆心、某距离为半径圈定的圆形范围内的定投广告

## 设计思想<div id="design"></div>

cloriSearch在设计与工程实现上参考了多个开源项目与相关论文，包括

* *[Indexing Boolean Expressions](http://theory.stanford.edu/~sergei/papers/vldb09-indexing.pdf)* - 斯坦福大学发表的布尔表达式检索论文，是cloriSearch所采用的核心广告检索算法
* [Lucene](http://lucene.apache.org/) - 倒排检索的工程实现 
* [Redis](https://github.com/antirez/redis.git) - LBS检索参考并引入了redis对geohash的实现
* [brpc](https://github.com/brpc/brpc.git) - json与protobuf的互相转换
* [cloriSkip](https://github.com/shpilu/cloriSkip.git) - 跳跃表实现

此外，还部分参考了大型互联网公司的广告检索技术实现，包括

* 阿里妈妈 *[通用广告引擎的索引设计和性能优化](http://60.205.189.117/%E9%80%9A%E7%94%A8%E5%B9%BF%E5%91%8A%E5%BC%95%E6%93%8E%E7%9A%84%E7%B4%A2%E5%BC%95%E8%AE%BE%E8%AE%A1%E5%92%8C%E6%80%A7%E8%83%BD%E4%BC%98%E5%8C%96.pdf)* 
* 广点通 *[下一代的全能广告系统架构](http://djt.qq.com/article/view/1556)*
* 美团 *[美团点评广告实时索引的设计与实现](https://blog.csdn.net/MeituanTech/article/details/80415658)*
* 360 *[如何打造高质量的SSP广告引擎](https://blog.csdn.net/ZVAyIVqt0UFji/article/details/78934524)*

## 实例<div id="usage"></div>

* 1.定义索引结构

在使用cloriSearch之前，需要定义索引结构(具体proto定义参考src/proto/inverted_index.proto)，比如要支持城市、性别、年龄、地理位置等四个定向维度的索引结构定义为

```json
{
    "terms": [{
            "name": "city",
            "key_type": "string",
            "index_type": "simple"
        },
        {
            "name": "age",
            "key_type": "int32",
            "index_type": "interval"
        },
        {
            "name": "gender",
            "key_type": "string",
            "index_type": "simple"
        },
        {
            "name": "location",
            "key_type": "double",
            "index_type": "geo"
        }
    ]
}
```
可以根据业务需求随意增加定向维度。

* 2.写入倒排数据

cloriSearch目前只支持析取范式格式的倒排写入(具体proto定义参考src/proto/inverted_index.proto)，比如一条docid=1，定向北上广深、年龄在[18, 25]之间的女性用户投放的广告倒排描述为

```json
// docid=1，定向北上广深、年龄在[18, 25]之间的女性用户
{
    "mode": "stanard",
    "docid": 1,
    "disjunctions": [{
        "conjunctions": [{
            "name": "city",
            "value": {
                "sval": ["beijing", "shanghai", "guangzhou", "shenzhen"]
            }
        }, {
            "name": "age",
            "value": {
                "int32_intvl": [{
                    "left": 20,
                    "right": 30,
                    "flag": 3
                }]
            }
        }, {
            "name": "gendor",
            "value": {
                "sval": ["female"]
            }
        }]
    }]
}

// docid=3, 定投国贸周边的男性用户
{
    "mode": "stanard",
    "docid": 3,
    "disjunctions": [{
        "conjunctions": [{
            "name": "location",
            "value": {
                "geo": {
                    "lon": 116.461805,
                    "lat": 39.909005
                }
            }
        }, {
            "name": "gender",
            "value": {
                "sval": ["male"]
            }
        }]
    }]
}
```

3. 检索匹配的广告

比如要检索一位年龄在20岁、位于北京市经纬度坐标(116.400693, 39.907688)的女学生所命中的广告(LBS定向半径1km)，其检索式为

```C++
Query query;
query["city"] = "beijing";
query["age"] = 20; 
query["gendor"] = "female";
query["is_student"] = true;
query["location"] = GeoRange(116.400693, 39.907688, 1000);

std::vector<int> res = sch->Search(query, 10);
for (auto &p : res) {                                                                                                                 
    std::cout << "docid=" << p << std::endl;
} 

```
具体代码实现可以参考src/example/tutorial_xxx.cc中的例子

## 安装<div id="install"></div>

cloriSearch是用C++写的检索引擎内核，你可以直接拷贝代码到你的项目中，或者按照以下方式安装

```C++
mkdir build && cd build
cmake ..
make
sudo make install
```
## 待完成列表

cloriSearch目前只支持全量索引与倒排检索，正排索引、增量索引、广告Rank与过滤等功能尚在开发中，另外还没考虑程序性能问题，将在后续优化改进。

## 作者 <div id="authors"></div>

* James Wei (weijianlhp@163.com)   

[返回顶部](#top)

