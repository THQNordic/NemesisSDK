echo Cleaning sdk folders...
attrib -r -h -s /s sdk\sln\*.suo
del /s /q sdk\sln\*.ncb
del /s /q sdk\sln\*.suo
del /s /q sdk\sln\*.user
del /s /q sdk\sln\*.sdf
del /s /q sdk\sln\*.VC.db
del /s /q sdk\tmp\*.*
rd /s /q sdk\sln\.vs
rd /s /q sdk\tmp
rd /s /q sdk\lib
rd /s /q sdk\dll
rd /s /q sdk\bin
