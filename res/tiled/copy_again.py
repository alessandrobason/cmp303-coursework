import subprocess

pre = "%SystemRoot%\system32\WindowsPowerShell\\v1.0\powershell.exe "

subprocess.call(pre + "rm -r ../../build/res", shell=True)
subprocess.call(pre + "cp -r ../../res ../../build", shell=True)
