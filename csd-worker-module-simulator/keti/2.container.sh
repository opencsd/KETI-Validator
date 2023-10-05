#!/usr/bin/env bash

registry="ketidevit2"
imageName="simulator_csd_container2" #이미지명
version="v0.1"
containerName="simulator_csd_container2"
portOptions="-p 40013:40013"

arg=$1 #create/start/run/stop/kill/remove

while :
do
    if [ "$arg" == "create" ]; then   
        echo "docker create $portOptions --name $containerName $imageName:$version"
        docker create $portOptions --name $containerName $imageName:$version
        break
    elif [ "$arg" == "run" ]; then   
        # echo "docker run --net=host --name $containerName $imageName:$version"
        # docker run --net=host --name $containerName $imageName:$version
        echo "docker run $portOptions --name $containerName  $imageName:$version"
        docker run $portOptions -i --name $containerName $imageName:$version
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