# Preferred language and keyboard layout
$lang = "fi-FI"

# Reset eveluation timer if necessary. This can be repeated six times and each
# gives 180 extra days of evaluation use.
if ((Get-Ciminstance -Class SoftwareLicensingProduct).LicenseStatus -ne 1) {
    Write-Host "Resetting license evaluation timer"
    & slmgr.vbs //B -rearm
    $need_restart = $true
}

Write-Host "Setting keyboard layouts"
$languages = New-WinUserLanguageList -Language $lang
$languages.Add($lang)
Set-WinUserLanguageList $languages -Force

if (-Not (Test-Path C:\ProgramData\chocolatey\bin\choco.exe)) {
    Write-Host "Installing Chocolatey"
    Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))
}

Write-Host "Installing software"
& choco.exe install -y git --params "/GitAndUnixToolsOnPath"
& choco.exe install -y wixtoolset

if ($need_restart) {
    Write-Host "Rebooting to finish the license evaluation timer reset"
    # We need to schedule the restart or Vagrant will loose the WinRM
    # connection and destroy the instance as a result.
    shutdown /r /t 10
}
