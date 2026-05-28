# Bartłomiej Deska

## Zadanie 2

## a. Obraz wspierać ma dwie architektury: linux/arm64 oraz linux/amd64.

Dodałem:

```
platforms: linux/amd64,linux/arm64
```

w build-push-action

## b. Wykorzystywane mają być (wysyłanie i pobieranie) dane cache (eksporter: registry oraz backend-u registry w trybie max). Te dane cache powinny być przechowywane w dedykowanym, publicznym repozytorium autora na DockerHub.

Dodałem:

```
cache-from: |
	type=registry,ref=${{ vars.DOCKERHUB_USERNAME }}/cache:cache
cache-to: |
	type=registry,ref=${{ vars.DOCKERHUB_USERNAME }}/cache:cache,mode=max
```

w build-push-action

## c. Ma być wykonany test CVE obrazu, który zapewni, że obraz zostanie przesłany do publicznego repozytorium obrazów na GitHub TYLKO WTEDY gdy nie będzie zawierał zagrożeń sklasyfikowanych jako krytyczne lub wysokie.

1. Zbudowałem obraz lokalnie
2. Zeskanowałem go za pomocą aquasecurity/trivy-action@master
   1. Jeżeli się nie powiedzie to przerwie wysłanie obrazu do ghcr.io
3. Wysłałem raport do sekcji "Code scanning" na github

## Jak obsługiwane są wersje i cache

Obrazy są budowane w przypadku tagu v\* (v1.0, v1, v2, v2.4.1) i nie są one usuwane, aby można było szybko przywrócić już zbudowane obrazy.

Cache za to jest oznaczany tylko jako pan1jan1/cache:gha_cache ponieważ cache zajmuje dużo miejsca na dysku,
a nowe obrazy będą budowane na podstawie tych najnowszych

```

```
