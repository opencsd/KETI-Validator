#!/usr/bin/env bash

registry="ketidevit2"
imageName="simulator_csd_container6" #이미지명
version="v0.1" #버전
dockerFilePath="." #도커파일경로

arg=$1 # make/remove

while :
do
    if [ "$arg" == "make" ]; then   
        # make image (dockerfile 경로 지정)
        docker build -t $imageName:$version $dockerFilePath && \
        # add tag
        #docker tag $imageName:$version $registry/$imageName:$version && \
        # login
        #docker login && \
        # push image
        #docker push $registry/$imageName:$version
        break
    elif [ "$arg" == "remove" ]; then 
        # get image id
        ImageIDs=`docker images | grep $registry/$imageName | awk '{print $3}'`
        echo "docker rmi -f $ImageIDs"
        docker rmi -f $ImageIDs 
        break
    else
        read -p "Enter Image Work(make/remove): " arg
    fi
done




  