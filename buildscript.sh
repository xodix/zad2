#!/usr/bin/bash
VERSION=1.0
sudo docker stop tiny2
sudo docker rm tiny2
sudo docker buildx build \
		--push --platform linux/amd64,linux/arm64 \
		-t pan1jan1/smol-server:v$VERSION .
sudo docker run -d --name tiny2 \
		--platform linux/arm64 \
		-p 3000:3000 \
		pan1jan1/smol-server:v$VERSION