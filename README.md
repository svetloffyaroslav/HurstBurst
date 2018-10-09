![logo](https://github.com/svetloffyaroslav/HurstBurst/blob/master/tcp_hurst/mainlogo.ico)
# HurstBurst - Self-Similar Traffic Software Generator
## About HurstBurst
This software is part of my Master's thesis **"Development of traffic generator with given degree of self-similar"**.
It uses [client-server model](http://qt-doc.ru/model-klient-server.html), so in this way it consits of two parts:
+ [server part](https://github.com/svetloffyaroslav/HurstBurst/tree/master/tcp_server);
+ [client part](https://github.com/svetloffyaroslav/HurstBurst/tree/master/tcp_hurst).

Both programms are used to organize connection between two computers, and after than start to send traffic with different parameters, like:
* number of tcp-packages;
* size of every package;
* types of distributions and their criterions (Like [Pareto](https://en.wikipedia.org/wiki/Pareto_distribution), [Weibull](https://en.wikipedia.org/wiki/Weibull_distribution), etc);

The scheme of expirements:

![picture-one-scheme](https://github.com/svetloffyaroslav/HurstBurst/blob/master/tcp_hurst/Images/scheme.png)

### Server part
---
![server-interface](https://github.com/svetloffyaroslav/HurstBurst/blob/master/tcp_server/Images/Interface.png)

First of all, when you run server-program -  you should create a server(press button under red arrow). In text area ***"IP-adress"*** you will get all IP-adress, which your server-computer has. Also, you can chose number of port in text line below (***"Port"***). [Here](http://www.steves-internet-guide.com/tcpip-ports-sockets/) you can read more about how TCP/IP network works. 

Information about time between packeges will be displayed in text area ***"Delta,nanosec"*** . Futhermore, you can get information about *size of each packege*, *current total data count*,*current packages number* in group-box ***"Accepted packeges"****.

### Client part
---


### Some information about code
---


### Build with
---

### Authors
---
* Svetlov Yaroslav - [git](https://github.com/svetloffyaroslav),[twitter](https://twitter.com/whensunraises)
### License

### Acknowledgments
---
* [QCustomPlot](https://www.qcustomplot.com/index.php/introduction);


