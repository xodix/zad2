# Budowanie obrazu

```bash
docker buildx use smol-builder
docker buildx build \
			--push --platform linux/amd64,linux/arm64 \
			-t pan1jan1/smol-server:v1.0 .
```

# Uruchomienie serwera

```bash
docker run -d -p 3000:3000 --name smol-server pan1jan1/smol-server:v1.0
```

# Sposób uzyskiwania informacji z logów

```bash
docker logs smol-server
```

Wynik:

```
30.04.2026
Autor: Bartłomiej Deska
Aplikacja nasłuchuje na porcie 3000
```

# Sposób sprawdzenia ile warstw posiada obraz

```bash
docker image history pan1jan1/smol-server:v1.0
```

```
IMAGE          CREATED         CREATED BY                                      SIZE      COMMENT
b713d08b0256   3 minutes ago   ENTRYPOINT ["/server"]                          0B        buildkit.dockerfile.v0
<missing>      3 minutes ago   HEALTHCHECK &{["CMD" "/server" "healthcheck"…   0B        buildkit.dockerfile.v0
<missing>      3 minutes ago   COPY /app/server / # buildkit                   2.34kB    buildkit.dockerfile.v0
<missing>      3 minutes ago   EXPOSE [3000/tcp]                               0B        buildkit.dockerfile.v0
<missing>      3 minutes ago   LABEL org.opencontainers.image.description=V…   0B        buildkit.dockerfile.v0
<missing>      3 minutes ago   LABEL org.opencontainers.image.title=SMOLL-H…   0B        buildkit.dockerfile.v0
<missing>      3 minutes ago   LABEL org.opencontainers.image.authors=Bartł…   0B        buildkit.dockerfile.v0
```

Obraz składa się z tylko jednej warstwy.

# Potwierdzenie działania serwera

![Potwierdzenie](proof.png)
