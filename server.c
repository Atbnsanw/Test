 /*********************************************
 
 NAME:server.c
 TIME:2019/04/14
 
 **********************************************/

#include <stdio.h>
#include <sys/sysinfo.h> //获取系统相关信息
#include <sys/statfs.h> //获取磁盘容量信息
#include <sys/stat.h>
#include <stdlib.h>   //执行shell
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <arpa/inet.h>
#include "errno.h"
#include "sys/types.h"
#include "netinet/in.h"
#include "sys/socket.h"
#include "sys/msg.h"
#include "sys/ipc.h"
#include "signal.h"
//#include <sys/msgbuf.h>
#include <mysql.h> 
#define KB 1024.0
#define MB 1048576.0
#define GB 1073741824.0
#define MESGSIZE 1024 //udp报文的最大字节数
#define SPORT 7772 //服务器端口号
#define SIP "127.0.0.1" //服务器IP地址
#define QKEY 2
#define MSGTYPE  1
int pid;
MYSQL *conn_prt;     //MYSQL句柄
int conFlag = 0;
typedef struct messagestruct
{
    long type;
    char msgbuf[MESGSIZE];
}msgstc;
void recmsg(int sockfd)
{
    int n,qid;
    long mtype = 1;
    char msg[MESGSIZE];
    char rmsg[MESGSIZE] = "yes";//用来回送确认信息。
    int addrlen;
    char clinfo[128];
    struct sockaddr_in addr;
    msgstc msgstruct;
    msgstruct.type = MSGTYPE;
    qid = msgget(QKEY, 0666|IPC_CREAT);
    while(1){
        n=recvfrom(sockfd, msg, MESGSIZE, 0, (struct sockaddr*)&addr, &addrlen);
        //printf("received mesg%s\n",msg);
        sendto(sockfd, rmsg, sizeof(rmsg), 0, (struct sockaddr*)&addr, addrlen);
        sprintf(clinfo,"%d %s",ntohs(addr.sin_port),inet_ntoa(addr.sin_addr));
        sprintf(msgstruct.msgbuf,"%s %s",clinfo,msg);
        n = msgsnd(qid,&msgstruct , MESGSIZE, 0);
        memset(msg,0,sizeof(msg));
        memset(clinfo,0,sizeof(msg));
        memset(msgstruct.msgbuf,0,sizeof(msg));
    }
}


/*初始化mysql句柄*/
void self_init() {
	conn_prt = mysql_init(NULL);
}
 
/*连接数据库*/
void self_connect() {
	if(!mysql_real_connect(conn_prt,"localhost","root","sharon813","RM",0,NULL,0))/*password  db_name*/ {
		printf("failed to connect:%s\n",mysql_error(conn_prt));
		kill(pid,SIGKILL);
		exit(0) ;
	} else {
		conFlag = 1;
	}
	printf("connect success!\n");
}

 
/*往load表中插入数据*/
void insert_load(char * ip,int a,float b,float c,float d, float e, float f) {
	char buf[1024];
	int len = snprintf(buf, sizeof(buf), "INSERT INTO SYSLOAD (server_IP,cpu_core,cpu_usage,avgload_1min,avgload_5min,avgload_15min,mem_usage) VALUES('%s','%d','%f','%f','%f','%f','%f') ;",ip,a,b,c,d,e,f);
	if(len<0) {
		printf("insert fail\n");
		mysql_close(conn_prt);
	}
	if(0==mysql_query(conn_prt,buf)) {
		printf("insert succeed\n");
	}
	else {
		printf("insert failed\n");
        conFlag--;
		//mysql_error(conn_prt);
		mysql_close(conn_prt);
	}
}
/*往disk表中插入数据*/
void insert_disk(char * ip,float a,float b,float c, float d) {
	char buf[1024];
	int len = snprintf(buf, sizeof(buf), "INSERT INTO DISK (server_IP,disk_usage,disk_remain,disk_IOwait,disk_IObusy) VALUES('%s','%f','%f','%f','%f');",ip,a,b,c,d);
	if(len<0) {
		printf("insert fail\n");
		mysql_close(conn_prt);
	}
	if(0==mysql_query(conn_prt,buf)) {
		printf("insert succeed\n");
	}
	else {
		printf("insert failed\n");
        conFlag--;
		mysql_close(conn_prt);
	}
	
}

/*往net表中插入数据*/
void insert_net(char * ip,float a,float b,float c) {
	char buf[1024];
	int len = snprintf(buf, sizeof(buf), "INSERT INTO NET (server_IP,net_delay,uploadRate,downloadRate) VALUES('%s','%f','%f','%f');",ip,a,b,c);
	if(len<0) {
		printf("insert fail\n");
		mysql_close(conn_prt);
	}
	if(0==mysql_query(conn_prt,buf)) {
		printf("insert succeed\n");
	}
	else {
		printf("insert failed\n");
        conFlag--;
		mysql_close(conn_prt);
	}
}

/*往SERVER_INFO表中插入数据*/
void insert_server(int port, char * ip) {
	char buf[1024];
	int len = snprintf(buf, sizeof(buf), "INSERT INTO SERVER_INFO (port, server_IP, monitor_stat) VALUES('%d','%s','True') ON DUPLICATE KEY UPDATE id = id;",port,ip);
	if(len<0) {
		printf("insert fail\n");
		mysql_close(conn_prt);
	}
	if(0==mysql_query(conn_prt,buf)) {
		printf("insert succeed\n");
	}
	else {
		printf("insert failed\n");
        conFlag--;
		mysql_close(conn_prt);
	}
}


void informationprocess(char msg[MESGSIZE]) {
	/*sscanf(msg,"%d %lf %s %s %s %lf %lf %lf %lf %lf %lf %lf %lf",
			&cpu_num,&cpu_usage,&avgload_1,&avgload_5,&avgload_15,&mem_usage,&used_rate,&remain,&disk_wait,&disk_busy,&delay,&uploadRate,&downloadRate);*/
	int port,cpu_num;
	char server_ip[128];
	int flag = 0;
	float disk_usage,disk_remain,cpu_usage,avgload_1,avgload_5,avgload_15,mem_usage,disk_wait,disk_busy,net_delay,net_IO,uploadRate,downloadRate;
	sscanf(msg,"%d %s %f %f %d %f %f %f %f %f %f %f %f %f %f", &port,server_ip,&disk_usage,&disk_remain,&cpu_num,&cpu_usage,&avgload_1,&avgload_5,&avgload_15,&mem_usage,&disk_wait,&disk_busy,&uploadRate,&net_IO,&downloadRate);
	if(conFlag == 0){
		self_init();
		self_connect();
	}else {
		insert_load(server_ip,cpu_num,cpu_usage,avgload_1,avgload_5,avgload_15,mem_usage);
		insert_disk(server_ip,disk_usage,disk_remain,disk_wait,disk_busy);
		insert_net(server_ip,net_delay,uploadRate,downloadRate);
	    insert_server(port, server_ip);
	}
}

void storedata(void){
    while(1)
    {
        char msg[MESGSIZE];
        msgstc msgstruct;
        int qid;
        msgstruct.type = MSGTYPE;
        qid = msgget(QKEY,0666|IPC_CREAT);
        msgrcv(qid,&msgstruct,MESGSIZE,MSGTYPE,0);
        if(strlen(msgstruct.msgbuf)==0)
        	continue;
		informationprocess(msgstruct.msgbuf);
        printf("%s\n\n",msgstruct.msgbuf);
    }
}
int main(){
    struct sockaddr_in myaddr;
    int sockfd;
    if((sockfd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
    {
        perror("socket create error");
        exit(1);
    }
    bzero(&myaddr, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(SPORT);
    if(bind(sockfd,(struct sockaddr*)&myaddr,sizeof(myaddr)))
    {
        perror("bind error!");
        exit(-1);
    }
    pid = fork();
    if(pid>0)
    {
        storedata();
    }
    else if (pid==0)
    {
        recmsg(sockfd);
    }
    return 0;
 }
/*int main(){
    char udpmesg[1024];
    int sockfd;
    memset(udpmesg,0,sizeof(udpmesg));
    Data_Integration(udpmesg);
    printf("udpmesg:%s",udpmesg);
    //磁盘使用率 磁盘剩余空间 CPU核数 CPU利用率 CPU1、3、5分钟平均负载
    //内存利用率 磁盘IO等待时间  磁盘IO繁忙程度 网络延迟 网络IO 上行下行流量
}
*/
