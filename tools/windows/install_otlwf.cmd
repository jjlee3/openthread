copy.exe otlwf.sys %SystemRoot%\System32\drivers\otlwf.sys
copy.exe otapi.dll %SystemRoot%\System32\otapi.dll
reg.exe import ThreadEtw.reg
sc.exe create otlwf binPath= \SystemRoot\system32\drivers\otlwf.sys type= kernel start= system group= NDIS displayname= "OpenThread NDIS LightWeight Filter"
otInstall.exe -i
shutdown.exe /r /f