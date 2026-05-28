# Zadanie 3

# Tworzenie buildera

```bash
docker buildx create --name smol-builder --driver docker-container --use --bootstrap
```

# Budowanie obrazu

```bash
eval $(ssh-agent -s) # lub -c dla fish shell
# set -Ux SSH_AUTH_SOCK $SSH_AUTH_SOCK # fish
# set -Ux SSH_AGENT_PID $SSH_AGENT_PID # fish
ssh-add ~/.ssh/github
sudo docker buildx build -f Dockerfile_dodatkowe \
			--push --platform linux/amd64,linux/arm64 \
			-t pan1jan1/smol-server:v2.0 \
			--ssh github=$HOME/.ssh/github \
			--cache-to type=registry,ref=pan1jan1/smol-server:cache,mode=max \
			--cache-from type=registry,ref=pan1jan1/smol-server:cache .
```

# Uruchomienie serwera

```bash
docker run -d -p 3000:3000 --name smol-server pan1jan1/smol-server:v2.0
```

# Sposób uzyskiwania informacji o platformach

```bash
sudo docker buildx imagetools inspect pan1jan1/smol-server:v2.0
```

```
Name:      docker.io/pan1jan1/smol-server:v2.0
MediaType: application/vnd.oci.image.index.v1+json
Digest:    sha256:76377743feeecdbd1778856f894d05820ddb478d79ef73d106eccf82db23a791

Manifests:
  Name:        docker.io/pan1jan1/smol-server:v2.0@sha256:3c06914d4fe1d698b2f5d4619bd23ef5dd8f9e83963a35c187ab2df23db3b751
  MediaType:   application/vnd.oci.image.manifest.v1+json
  Platform:    linux/amd64

  Name:        docker.io/pan1jan1/smol-server:v2.0@sha256:994c4fe2513fc948081ea37643ef06cc733b00b9e766d443099f9639d6cac677
  MediaType:   application/vnd.oci.image.manifest.v1+json
  Platform:    linux/arm64

  Name:        docker.io/pan1jan1/smol-server:v2.0@sha256:27f25dee9046f1abee8e70eaf2eea4a4a2491b3daa955f5ebc64007f0d828c05
  MediaType:   application/vnd.oci.image.manifest.v1+json
  Platform:    unknown/unknown
  Annotations:
    vnd.docker.reference.digest: sha256:3c06914d4fe1d698b2f5d4619bd23ef5dd8f9e83963a35c187ab2df23db3b751
    vnd.docker.reference.type:   attestation-manifest

  Name:        docker.io/pan1jan1/smol-server:v2.0@sha256:fbbd7465ae7b40dcedec1c6948eef72d8623950d50f06d63ea2b3ab3d0dd2e1d
  MediaType:   application/vnd.oci.image.manifest.v1+json
  Platform:    unknown/unknown
  Annotations:
    vnd.docker.reference.digest: sha256:994c4fe2513fc948081ea37643ef06cc733b00b9e766d443099f9639d6cac677
    vnd.docker.reference.type:   attestation-manifest
```
