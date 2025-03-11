# IM
一款以qt作为客户端，linux c/c++为后端的即时通讯应用，使用protobuf协议进行tcp明文传输，服务器使用asio进行数据接收与发送，配备任务池，mysql、redis连接池辅助,还有fmt库进行字符串的格式化
由于鼠鼠能力有限😔，憋不出更好的代码，所以客户端的代码不怎么样，服务端代码还算看得过去.
# 目前支持的功能如下
1.记住密码，自动登录  
2.添加好友  
3.发送emoji,消息,图片(图片在主线程解析发送，目前还没进行优化)  
4.好友搜索添加，验证  
5.离线消息存储拉取  
6.消息都是存储在本地的(除了离线消息)  

# 代码的部署
可以在ubuntu下使用apt安装libmysqlclient,mysql  
sudo apt update
sudo apt install mysql-server  (8.0版本)  
sudo apt install libmysqlclient-dev  
sudo apt install libhiredis-dev  
sudo apt install redis-server  
sudo apt install libfmt-dev  
sudo apt install libasio-dev  
protobuf的话可以去网上搜索安装编译的方法

CMakeList.txt里面  
set(MYSQL_INCLUDE_DIR /usr/include/mysql)  
set(MYSQL_LIBRARY /usr/lib/x86_64-linux-gnu/libmysqlclient.so)  
  
set(REDIS_INCLUDE_DIR /usr/include/hiredis)  
set(REDIS_LIBRARY /usr/lib/x86_64-linux-gnu/libhiredis.so)  
**记得改为自己的路径**  

客户端的话建议使用Qt6.3以上版本，因为使用到一些6.2 6.3后才有的特性，我使用的是6.72  
qt使用protobuf可以参考  
https://blog.csdn.net/qq_44667165/article/details/120620032

