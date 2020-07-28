FROM ubuntu:18.04

RUN apt-get update --fix-missing && \
  apt-get install -y \
  build-essential \
  pkg-config \
  libsdl2-dev \
  libsdl2-mixer-dev \
  libsdl2-image-dev \
  libglew-dev \
  libglm-dev \
  mesa-common-dev \
  git \
  cmake

RUN mkdir -p /build