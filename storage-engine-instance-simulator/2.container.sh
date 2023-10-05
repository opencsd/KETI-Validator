#!/usr/bin/env bash

registry="ketidevit2"
imageName="simulator_container" #이미지명
version="v0.1"
containerName="simulator_container"
portOptions="-p 40001:40001 -p 40003:40003"

arg=$1 #create/start/run/stop/kill/remove

while :
do
    if [ "$arg" == "create" ]; then   
        echo "docker creat $portOptions --name $containerName $registry/$imageName:$version"
        docker creat $portOptions --name $containerName $registry/$imageName:$version
        break
    elif [ "$arg" == "run" ]; then   
        echo "docker run --name $containerName $portOptions $imageName:$version"
        docker run -i --name $containerName $portOptions $imageName:$version
        break
    elif [ "$arg" == "log" ]; then   
        echo "docker logs $containerName"
        docker logs --follow $containerName
        break
    else
        ContainerID=`docker ps -aqf "name=^${containerName}"`
        if [ "$arg" == "start" ]; then   
            echo "docker start $ContainerID"
            docker start $ContainerID
            break  
        elif [ "$arg" == "stop" ]; then   
            echo "docker stop $ContainerID"
            docker stop $ContainerID
            break
        elif [ "$arg" == "kill" ]; then   
            echo "docker kill $ContainerID"
            docker kill $ContainerID
            break
        elif [ "$arg" == "remove" ]; then   
            echo "docker rm -f $ContainerID"
            docker rm -f $ContainerID
            break
        else 
            read -p "Enter Container Work(create/start/run/stop/kill/remove/log): " arg
        fi
    fi
done