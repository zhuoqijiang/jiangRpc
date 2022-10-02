# 测试及改进

## 测试环境
* OS：Linux version 4.15.0-118-generic (buildd@lgw01-amd64-039) (gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04))Ubuntu 14.04
* 内存：4G
* CPU：4核

## 测试对象 
* http server 

## 测试方法
* 使用webbench进行测压，开启800客户端进程，时间为60s
* 测试短链接情况
* 关闭所有的输出和Log
* 测试为必要的http头部，和“hello world!” 内容
* 线程池统一开启4个工作线程
* 测试对象为go语言官方http包 后期会与其他著名c++http框架做对比，还有长连接

## 测试结果
* go:        3840880 byte/s success request: 1786449
* jiangRpc:  4542547 byte/s success request: 2163118


* go短链接测试结果
![shortgo](https://github.com/zhuoqijiang/jiangRpc/blob/main/datum/go_short_800_60.jpg)

* jiangRpc短链接测试结果
![shortjiangRpc](https://github.com/zhuoqijiang/jiangRpc/blob/main/datum/jiangRpc_short_800_60.jpg)
