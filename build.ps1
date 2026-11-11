param([ValidateRange(1, 64)][int]$Jobs = 8)

$ErrorActionPreference = 'Stop'

$runner = Join-Path $PSScriptRoot 'build-in-container.sh'
$context = Join-Path $PSScriptRoot 'builder-context'
$image = 'elite-redux-cn-builder:bd7acbdd'
$results = Join-Path $PSScriptRoot ('build-results/' + (Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $results | Out-Null
$startupLog = Join-Path $results 'startup.txt'

function Fail-Startup([string]$Message) {
    Set-Content -LiteralPath $startupLog -Encoding UTF8 -Value $Message
    throw $Message
}

$docker = Get-Command docker.exe -CommandType Application -ErrorAction SilentlyContinue | Select-Object -First 1
if (-not $docker) {
    Fail-Startup 'Docker CLI was not found. A running Docker engine with Linux containers is required.'
}
$serverOs = & $docker.Source version --format '{{.Server.Os}}'
if ($LASTEXITCODE -ne 0 -or $serverOs -ne 'linux') {
    Fail-Startup 'Docker did not report a reachable Linux engine. Check Docker Desktop and its Linux-container mode.'
}
# Archive the committed source tree to preserve executable modes on Windows.
$git = Get-Command git.exe -CommandType Application -ErrorAction Stop | Select-Object -First 1
$sourceChanges = & $git.Source -C $PSScriptRoot status --porcelain -- game
if ($LASTEXITCODE -ne 0 -or $sourceChanges) {
    Fail-Startup 'Commit source changes in game/ before building.'
}
$inputArchive = Join-Path $results 'source.tar'
& $git.Source -C $PSScriptRoot archive --format=tar --output=$inputArchive HEAD:game
if ($LASTEXITCODE -ne 0) {
    Fail-Startup 'Could not archive game/ from the current commit.'
}

& $docker.Source build --platform linux/amd64 --target builder -f (Join-Path $context 'docker/Dockerfile') -t $image $context
if ($LASTEXITCODE -ne 0) {
    Fail-Startup 'Docker builder image creation failed. See the Docker build output.'
}

& $docker.Source run --rm --platform linux/amd64 `
    --mount "type=bind,source=$inputArchive,target=/inputs/source.tar,readonly" `
    --mount "type=bind,source=$runner,target=/inputs/build.sh,readonly" `
    --mount "type=bind,source=$results,target=/out" `
    --env "ER_BUILD_JOBS=$Jobs" $image /bin/sh /inputs/build.sh
if ($LASTEXITCODE -ne 0) {
    Fail-Startup "Container build or verification failed. Review $results."
}
Write-Output "Build output: $results"
