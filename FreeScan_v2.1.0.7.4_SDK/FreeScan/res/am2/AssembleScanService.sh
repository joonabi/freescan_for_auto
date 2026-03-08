#!/bin/bash

cd /home/zz
pkill AirProcessManager
pkill scanhub
pkill ScanService
pkill SnSyncService
echo "kill process finish"
rm -rf ./scanService/*
mkdir ./scanService
echo "start unpack"
tar -zxvf ./scanService.tar.gz -C ./scanService
echo "finish unpack"