#!/bin/bash

which docker > /dev/null 2>&1
if [ $? -eq 1 ]; then
    echo "don't installed docker"
fi

which tar > /dev/null 2>&1
if [ $? -eq 1 ]; then
    echo "don't installed tar"
fi 

docker pull alpine
docker run --name alpine alpine
docker export alpine > alpine.tar
docker rm alpine

mkdir rootfs
tar -C rootfs -xf alpine.tar

echo "Complete!"