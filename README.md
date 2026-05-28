# Bartłomiej Deska

## Zadanie 2

## Jak obsługiwane są wersje i cache

Obrazy są budowane w przypadku tagu v\* (v1.0, v1, v2, v2.4.1) i nie są one usuwane, aby można było szybko przywrócić już zbudowane obrazy.

Cache za to jest oznaczany tylko jako pan1jan1/cache:gha_cache ponieważ cache zajmuje dużo miejsca na dysku,
a nowe obrazy będą budowane na podstawie tych najnowszych
