$buildDir = "C:\Users\stupi\Desktop\Projects\Image-Editor\build\Debug"
$qtPlugins = "C:\dev\vcpkg\installed\x64-windows\Qt6\plugins\platforms"

Copy-Item "$qtPlugins" -Destination "$buildDir" -Recurse -Force
Write-Host "Qt plugins copied successfully!"
