<#
PowerShell script to backup and replace XheyCameraSDKAssets with a Vue build.
Usage: run from repo root or execute this script in PowerShell:
  ./scripts/replace_assets.ps1
#>

$ts = Get-Date -Format "yyyyMMddHHmmss"
$root = Resolve-Path (Join-Path $PSScriptRoot "..")
$root = $root.Path

$androidAssets = Join-Path $root "src\android\assets\XheyCameraSDKAssets"
$backupRoot = Join-Path $root "backups"
New-Item -ItemType Directory -Force -Path $backupRoot | Out-Null
$backupDir = Join-Path $backupRoot ("XheyCameraSDKAssets_backup_" + $ts)

function Backup-And-Remove($src, $destBackup) {
    if (Test-Path $src) {
        Write-Host "Backing up`n  $src`n -> $destBackup"
        Copy-Item -Path $src -Destination $destBackup -Recurse -Force
        Write-Host "Removing original: $src"
        Remove-Item -Path $src -Recurse -Force
    } else {
        Write-Host "Source not found: $src" -ForegroundColor Yellow
    }
}

# Backup Android assets
Backup-And-Remove -src $androidAssets -destBackup $backupDir

# Copy Vue build output (expecting build in www/MyXheyVue/dist)
$vueDist = Join-Path $root "www\MyXheyVue\dist"
if (Test-Path $vueDist) {
    Write-Host "Copying Vue build from $vueDist to $androidAssets"
    Copy-Item -Path $vueDist -Destination $androidAssets -Recurse -Force
} else {
    Write-Host "Vue build not found at $vueDist. Place your built files there (dist contents)." -ForegroundColor Yellow
}

# iOS bundle replacement (if present)
$iosBundleData = Join-Path $root "src\ios\libs\XheyCameraSDK-v1.2.327.9c500eba.iOS.dynamic\XheyCameraSDKResource.bundle\data"
$iosBackup = Join-Path $backupRoot ("XheyCameraSDKResource_bundle_backup_" + $ts)
Backup-And-Remove -src $iosBundleData -destBackup $iosBackup
if (Test-Path $vueDist) {
    Write-Host "Copying Vue build into iOS bundle data: $iosBundleData"
    Copy-Item -Path $vueDist -Destination $iosBundleData -Recurse -Force
} else {
    Write-Host "Skipping iOS bundle replace because Vue build not found." -ForegroundColor Yellow
}

Write-Host "Done. Backups stored under: $backupRoot"
Write-Host "If you want the plugin to load the custom assets by default, call the plugin with options.resourceDir = 'file:///android_asset/www/MyXheyVue' or rebuild platforms to include files in assets."