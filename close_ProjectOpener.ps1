# Close ProjectOpener.exe if running
$proc = Get-Process -Name ProjectOpener -ErrorAction SilentlyContinue
if ($proc) {
    Write-Host "Closing ProjectOpener.exe..."
    $proc | Stop-Process -Force
    Start-Sleep -Seconds 1
}