import subprocess

pre = "%SystemRoot%\system32\WindowsPowerShell\\v1.0\powershell.exe "

subprocess.call(pre + "rm -r ../../build2/res", shell=True)
subprocess.call(pre + "cp -r ../../res ../../build2", shell=True)
