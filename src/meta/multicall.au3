#include "../installer/au3/installer.au3"
#include "../installer/au3/uninstaller.au3"


; To save disk space, the autoit part of G2HR is a multicall binary now.
; It can act as the "meta" component, the installer or the uninstaller.

If $CmdLine[0] >= 1 Then
	If $CmdLine[1] == "install" Then
		Exit installer_main($CmdLine[2]) ; FIXME: use environment variable instead!
	ElseIf $CmdLine[1] == "uninstall" Then
		Exit uninstaller_main()
	Endif
Endif
