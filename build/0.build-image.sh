registry="ketidevit2"
image_name="validator"
version="v1.0"

(
    cd cmake/build
    make -j 100
)

# make image
docker build -t $image_name:$version . && \

# add tag
docker tag $image_name:$version $registry/$image_name:$version && \

# login
docker login && \

# push image
docker push $registry/$image_name:$version 