#syntax=docker/dockerfile:1
FROM alpine AS build
RUN apk add build-base
WORKDIR /app
COPY ./server.c ./Makefile ./
RUN make
RUN ls -lh /app/server && \
    file /app/server && \
    readelf -S /app/server | grep -E '(.text|.data|Size)'

FROM scratch
LABEL org.opencontainers.image.authors="Bartłomiej Deska"
LABEL org.opencontainers.image.title="SMOLL-HTTP"
LABEL org.opencontainers.image.description="Very small http server for a contest"
EXPOSE 3000

COPY --from=build /app/server /
HEALTHCHECK --interval=10s --timeout=5s CMD ["/server", "healthcheck"]

ENTRYPOINT [ "/server" ]
