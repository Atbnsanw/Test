DROP DATABASE RM;
CREATE DATABASE RM;
USE RM;
CREATE TABLE `SERVER_INFO` (
`port` varchar(255) NOT NULL,
`server_IP` varchar(255) NOT NULL unique key,
`monitor_stat` varchar(255) NOT NULL,
`access_time` timestamp default CURRENT_TIMESTAMP,
PRIMARY KEY (`server_IP`, `port`) 
);
CREATE TABLE `DATAITEM` (
`name` varchar(255) NOT NULL,
`data_item` varchar(255) NOT NULL,
`data_update_period` varchar(255) NOT NULL,
`visualized` bool NOT NULL,
PRIMARY KEY (`name`, `data_item`) 
);
CREATE TABLE `DISK` (
`server_IP` varchar(255) NOT NULL,
`disk_usage` float(255,3) NOT NULL,
`disk_remain` float(255,6) NOT NULL,
`disk_IOwait` float NOT NULL,
`disk_IObusy` float(255,6) NOT NULL,
`id` int(255) NOT NULL AUTO_INCREMENT,
`ins_time` timestamp default current_timestamp,
PRIMARY KEY (`id`) 
);
CREATE TABLE `NET` (
`server_IP` varchar(255) NOT NULL,
`net_delay` float(255,6) NOT NULL,
`uploadRate` float(255,6) NOT NULL,
`downloadRate` float(255,6) NOT NULL,
`id` int(255) NOT NULL AUTO_INCREMENT,
`ins_time` timestamp default current_timestamp,
PRIMARY KEY (`id`) 
);
CREATE TABLE `SYSLOAD` (
`server_IP` varchar(255) NOT NULL,
`cpu_core` int(255) NOT NULL,
`cpu_usage` float(255,6) NOT NULL,
`avgload_1min` float(255,6) NOT NULL,
`avgload_5min` float(255,6) NOT NULL,
`avgload_15min` float(255,6) NOT NULL,
`mem_usage` float(255,6) NOT NULL,
`id` int(255) NOT NULL AUTO_INCREMENT,
`ins_time` timestamp default current_timestamp,
PRIMARY KEY (`id`) 
);
CREATE TABLE `FAULT` (
`id` int(255) NOT NULL AUTO_INCREMENT,
`server_IP` varchar(255) NOT NULL,
`fault_time` timestamp(6) NOT NULL ON UPDATE CURRENT_TIMESTAMP(6),
`fault_info` varchar(255) NOT NULL,
`report` bool NOT NULL,
`report_mode` varchar(255) NOT NULL,
PRIMARY KEY (`ID`) 
);



////////////监测项目



INSERT INTO DATAITEM (name,data_item, data_update_period, visualized) VALUES ('SYSLOAD','cpu_core','1min','1');
INSERT INTO DATAITEM (name,data_item, data_update_period, visualized) VALUES ('SYSLOAD','cpu_usage','1min','1');
INSERT INTO DATAITEM (name,data_item, data_update_period, visualized) VALUES ('SYSLOAD','avgload_1min','1min','1');
INSERT INTO DATAITEM (name,data_item, data_update_period, visualized) VALUES ('SYSLOAD','avgload_5min','1min','1');
INSERT INTO DATAITEM (name,data_item, data_update_period, visualized) VALUES ('SYSLOAD','avgload_15min','1min','1');
INSERT INTO DATAITEM (name,data_item, data_update_period, visualized) VALUES ('SYSLOAD','mem_usage','1min','1');

INSERT INTO DATAITEM (name,data_item, data_update_period, visualized) VALUES ('DISK','disk_usage','1min','1');
INSERT INTO DATAITEM (name,data_item, data_update_period, visualized) VALUES ('DISK','disk_remain','1min','1');
INSERT INTO DATAITEM (name,data_item, data_update_period, visualized) VALUES ('DISK','disk_IOwait','1min','1');
INSERT INTO DATAITEM (name,data_item, data_update_period, visualized) VALUES ('DISK','disk_IObusy','1min','1');

INSERT INTO DATAITEM (name,data_item, data_update_period, visualized) VALUES ('NET','net_delay','1min','1');
INSERT INTO DATAITEM (name,data_item, data_update_period, visualized) VALUES ('NET','uploadRate','1min','1');
INSERT INTO DATAITEM (name,data_item, data_update_period, visualized) VALUES ('NET','downloadRate','1min','1');

