# WeeChat Docker Image

This repository contains the Dockerfile to create a Docker image for WeeChat, a popular IRC client. Run WeeChat inside a Docker container with ease.

## Requirements

Ensure Docker is installed on your machine. If not, follow the installation instructions on the [official Docker website](https://docs.docker.com/get-docker/).

## Build the Image

To build the WeeChat Docker image, execute the following command in the directory containing the Dockerfile:

```sh
create:	docker build -t weechat-image .
```

## Run WeeChat

To run the WeeChat Docker image, execute the following command in the directory containing the Dockerfile:

```sh
use:	docker run -it --rm weechat-image
```



