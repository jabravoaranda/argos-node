param(
    [Parameter(Position = 0)]
    [string]$RelayOrCommand,

    [Parameter(Position = 1)]
    [string]$Action,

    [string]$NodeIp = "192.168.1.138"
)

$ErrorActionPreference = "Stop"

function Show-Usage {
    Write-Host "Usage:"
    Write-Host "  .\tools\relay.ps1 <relay> on"
    Write-Host "  .\tools\relay.ps1 <relay> off"
    Write-Host "  .\tools\relay.ps1 status"
    Write-Host ""
    Write-Host "Options:"
    Write-Host "  -NodeIp <ip>    Defaults to 192.168.1.138"
}

function Get-Outputs {
    Invoke-RestMethod "http://$NodeIp/outputs"
}

if ([string]::IsNullOrWhiteSpace($RelayOrCommand)) {
    Show-Usage
    exit 1
}

if ($RelayOrCommand -eq "status") {
    if (-not [string]::IsNullOrWhiteSpace($Action)) {
        Write-Error "The status command does not accept an action."
    }

    $outputs = Get-Outputs
    Write-Host "ARGOS Node relay states at $NodeIp"
    $outputs.relays | Format-Table id, state
    exit 0
}

$relay = 0
if (-not [int]::TryParse($RelayOrCommand, [ref]$relay) -or $relay -lt 1 -or $relay -gt 8) {
    Write-Error "Invalid relay id '$RelayOrCommand'. Use a relay id from 1 to 8."
}

if ($Action -ne "on" -and $Action -ne "off") {
    Write-Error "Invalid action '$Action'. Use 'on' or 'off'."
}

$state = $Action -eq "on"
$body = if ($state) { '{"state":true}' } else { '{"state":false}' }
$uri = "http://$NodeIp/outputs/relays/$relay"

$response = Invoke-RestMethod `
    -Uri $uri `
    -Method Put `
    -ContentType "application/json" `
    -Body $body

$stateText = if ($response.state) { "ON" } else { "OFF" }
Write-Host "Relay $($response.relay) -> $stateText ($($response.result))"
