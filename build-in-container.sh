#!/bin/sh
set -eu

cd /src
tar -xf /inputs/source.tar
test ! -e pokeemerald_modern.gba

# Restore the pinned upstream resource excluded from the distribution.
wget -nv -O data/pokemon_colosseum.mb 'https://raw.githubusercontent.com/Elite-Redux/eliteredux-source/bd7acbdd355f3ee6fa0ac862add1f51933305b52/data/pokemon_colosseum.mb'
echo 'd133d340ce7507932bcb3a83f9ee1dd9905072763d493b66db20e1bde4304572  data/pokemon_colosseum.mb' | sha256sum -c -
/bin/sh /usr/local/bin/er-prepare-tree /src

if ! make -j"${ER_BUILD_JOBS:-8}" modern > /out/build.log 2>&1; then
    tail -n 80 /out/build.log >&2
    exit 1
fi
if ! test -d build/modern; then
    echo 'Expected build/modern was not produced.' >&2
    exit 1
fi
find build/modern -type f -name '*.o' -size 0c > /out/empty-objects.txt
if test -s /out/empty-objects.txt; then
    echo 'Build produced empty object files. See empty-objects.txt.' >&2
    exit 1
fi
test "$(wc -c < pokeemerald_modern.gba)" -eq 33554432
cp pokeemerald_modern.gba /out/
cd /out
sha256sum pokeemerald_modern.gba > SHA256SUMS.txt
cat SHA256SUMS.txt
