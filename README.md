# JiangRpc

## 简介
 - 基于Linux的rpc，由c++进行编写，目前仅支持简单RPC模式，将持续迭代流模式
 - 可作为http服务器

## 工具
 - 安装boost（使用boost_context和uncopyable）
 - 安装protobuf (需要protobuf进行序列化)
 - 内存泄露检测工具 (valgrind)

## 使用 
 - cmake .  // make 
 - examples 里有作为http server的使用和(rpc server(需要protobuf), python client)的使用样例
 - 将项目挂载为子模块，并在上级目录进行引用 

## 工作流程
 - 初始化rpc server后，将服务进行注册(函数指针，lamada，function，成员变量), 由于其是由http server进行实现，所以会将其方法名加入到前缀树（可优化为共享前缀树）。
 - 在由客户端连接到来时，主线程在没有连接时会进行阻塞，当连接到来会创建一个协程（task结构体），来控制这个连接，通过轮询的方式，将其关注事件注册到全局epoll（单独线程进行监控）里，并初始化一系列的相关结构（polldesc netfd conn)，首先会读取文件描述符，若读取到EAGIN则表明没有数据，会进行yield操作，放弃对线程的控制权，写操作同理。在processor没有任务时，会向其他线程进行steal操作，来保证线程在有连接的情况下始终处于工作状态。由于使用的是协程模型，没有像传统的server一样，靠回调来实现，能够很清晰的通过同步代码实现异步操作，所以代码是非常清晰易懂。

## 项目实现
 - 采用M:N协程模型，而不是传统的多进程或者reactor模型，使各个连接能够更好得到调度（避免出现有的线程空着有的线程满载的情况）和提高代码的可读性等
 - 采用EPOLL边沿触发的IO多路复用技术而不是SELECT和POLL，避免出现随着连接数变高时服务器性能出现大幅下降的情况
 - 使用基于红黑树的定时器，采用延时删除的策略，在牺牲少量内存的情况，能够最大化的提高性能（如即使删除带来的损耗，如果监听的请求在删除前再次到来减少了一次delete和一次new的时间）
 - 使用双缓冲（异步）机制实现日志，大部分时间前台线程与后台线程不会操作临界区，最大化降低锁的时间，同时也更好实现逻辑上的解耦。
 - 在协程切换与调度使用基于状态+原子操作，相比大粒度的锁操作极大提高模块的性能
 - 采用Http+Protobuf的形式实现rpc，采用Http是由于Http很成熟不用为每个语言客户端重新制定协议，采用Protobuf是由于脚手架的形式更利于跨语言代码的结合，且Protobuf对内容有着很好的压缩
