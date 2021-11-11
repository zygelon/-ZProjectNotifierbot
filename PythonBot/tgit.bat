@ECHO OFF
rem https://tortoisegit.org/docs/tortoisegit/tgit-automation.html
rem gitk 
rem https://ayende.com/blog/4749/executing-tortoisegit-from-the-command-line
rem https://stackoverflow.com/questions/4562210/how-to-open-the-tortoisegit-log-window-from-command-line
rem https://stackoverflow.com/questions/357315/get-list-of-passed-arguments-in-windows-batch-script-bat/382312

IF NOT "%~1"=="" GOTO PARAM_COMMAND

:DEFAULT_COMMAND
rem if no command is passed via parameter, fallback to "commit" as default command
TortoiseGitProc.exe /command:commit /closeonend:0
GOTO EXIT

:PARAM_COMMAND
rem as having command passed via parameter, use this as command
TortoiseGitProc.exe /command:%~1 /closeonend:0

:EXIT