; -- quat.iss --

[Setup]
AppName=Quat
AppVerName=Quat 1.30
AppCopyright=Copyright (C) 1997-2022 Dirk Meyer
OutputBaseFilename=quat-1.30-setup
DefaultDirName={pf}\Quat
DefaultGroupName=Quat
UninstallDisplayIcon={app}\Quat.exe
Compression=lzma2
SolidCompression=yes
;Compression=bzip
LicenseFile=..\COPYING
ChangesAssociations=yes
;ArchitecturesInstallIn64BitMode=x64

[Files]
;Source: "quat-x64.exe"; DestDir: "{app}"; DestName: "quat.exe"; Check: Is64BitInstallMode
Source: "quat-x86.exe"; DestDir: "{app}"; DestName: "quat.exe"; Check: not Is64BitInstallMode
Source: "..\COPYING"; DestDir: "{app}"

Source: "..\docs\manual\ce.png"; DestDir: "{app}\doc"
Source: "..\docs\manual\chart_de.png"; DestDir: "{app}\doc"
Source: "..\docs\manual\chart_us.png"; DestDir: "{app}\doc"
Source: "..\docs\manual\Dendr_st.jpg"; DestDir: "{app}\doc"
Source: "..\docs\manual\\ex_1.jpg"; DestDir: "{app}\doc"
Source: "..\docs\manual\ex_2.jpg"; DestDir: "{app}\doc"
Source: "..\docs\manual\ex_3.jpg"; DestDir: "{app}\doc"
Source: "..\docs\manual\gpl.html"; DestDir: "{app}\doc"
Source: "..\docs\manual\ie.png"; DestDir: "{app}\doc"
Source: "..\docs\manual\oe.png"; DestDir: "{app}\doc"
Source: "..\docs\manual\ote.png"; DestDir: "{app}\doc"
Source: "..\docs\manual\quat.png"; DestDir: "{app}\doc"
Source: "..\docs\manual\quat-de.html"; DestDir: "{app}\doc"
Source: "..\docs\manual\quat-de-1.html"; DestDir: "{app}\doc"
Source: "..\docs\manual\quat-de-2.html"; DestDir: "{app}\doc"
Source: "..\docs\manual\quat-de-3.html"; DestDir: "{app}\doc"
Source: "..\docs\manual\quat-de-4.html"; DestDir: "{app}\doc"
Source: "..\docs\manual\quat-de-5.html"; DestDir: "{app}\doc"
Source: "..\docs\manual\quat-us.html"; DestDir: "{app}\doc"
Source: "..\docs\manual\quat-us-1.html"; DestDir: "{app}\doc"
Source: "..\docs\manual\quat-us-2.html"; DestDir: "{app}\doc"
Source: "..\docs\manual\quat-us-3.html"; DestDir: "{app}\doc"
Source: "..\docs\manual\quat-us-4.html"; DestDir: "{app}\doc"
Source: "..\docs\manual\quat-us-5.html"; DestDir: "{app}\doc"
Source: "..\docs\manual\ve.png"; DestDir: "{app}\doc"

Source: "..\docs\examples\cut\complex.cut"; DestDir: "{app}\examples\cut"
Source: "..\docs\examples\cut\cut.col"; DestDir: "{app}\examples\cut"
Source: "..\docs\examples\cut\cut.ini"; DestDir: "{app}\examples\cut"
Source: "..\docs\examples\cut\cut.obj"; DestDir: "{app}\examples\cut"
Source: "..\docs\examples\dendr_aa\dendr_aa.col"; DestDir: "{app}\examples\dendr_aa"
Source: "..\docs\examples\dendr_aa\dendr_aa.ini"; DestDir: "{app}\examples\dendr_aa"
Source: "..\docs\examples\dendr_aa\dendr_aa.obj"; DestDir: "{app}\examples\dendr_aa"
Source: "..\docs\examples\dendrit1\dendr_st.ini"; DestDir: "{app}\examples\dendrit1"
Source: "..\docs\examples\dendrit1\dendrit1.col"; DestDir: "{app}\examples\dendrit1"
Source: "..\docs\examples\dendrit1\dendrit1.ini"; DestDir: "{app}\examples\dendrit1"
Source: "..\docs\examples\dendrit1\dendrit1.obj"; DestDir: "{app}\examples\dendrit1"
Source: "..\docs\examples\dezent\dezent.ini"; DestDir: "{app}\examples\dezent"
Source: "..\docs\examples\drache\drache.ini"; DestDir: "{app}\examples\drache"
Source: "..\docs\examples\drache\drache.obj"; DestDir: "{app}\examples\drache"
Source: "..\docs\examples\drache\reds.col"; DestDir: "{app}\examples\drache"
Source: "..\docs\examples\fetz_aa\fetz_aa.col"; DestDir: "{app}\examples\fetz_aa"
Source: "..\docs\examples\fetz_aa\fetz_aa.ini"; DestDir: "{app}\examples\fetz_aa"
Source: "..\docs\examples\fetz_aa\fetz_aa.obj"; DestDir: "{app}\examples\fetz_aa"
Source: "..\docs\examples\fetzen\fetzen.col"; DestDir: "{app}\examples\fetzen"
Source: "..\docs\examples\fetzen\fetzen.ini"; DestDir: "{app}\examples\fetzen"
Source: "..\docs\examples\fetzen\fetzen.obj"; DestDir: "{app}\examples\fetzen"
Source: "..\docs\examples\filigran\filigran.col"; DestDir: "{app}\examples\filigran"
Source: "..\docs\examples\filigran\filigran.ini"; DestDir: "{app}\examples\filigran"
Source: "..\docs\examples\filigran\filigran.obj"; DestDir: "{app}\examples\filigran"
Source: "..\docs\examples\icon\icon.ini"; DestDir: "{app}\examples\icon"
Source: "..\docs\examples\nice\nice.col"; DestDir: "{app}\examples\nice"
Source: "..\docs\examples\nice\nice.ini"; DestDir: "{app}\examples\nice"
Source: "..\docs\examples\nice\nice.obj"; DestDir: "{app}\examples\nice"
Source: "..\docs\examples\nocut\nocut.col"; DestDir: "{app}\examples\nocut"
Source: "..\docs\examples\nocut\nocut.ini"; DestDir: "{app}\examples\nocut"
Source: "..\docs\examples\nocut\nocut.obj"; DestDir: "{app}\examples\nocut"
Source: "..\docs\examples\sleipnir\sleipnir.ini"; DestDir: "{app}\examples\sleipnir"
Source: "..\docs\examples\title1\title1.ini"; DestDir: "{app}\examples\title1"

[Icons]
Name: "{group}\Quat 1.30"; Filename: "{app}\quat.exe"; WorkingDir: "{app}"
Name: "{group}\Quat Documentation"; Filename: "{app}\doc\quat-us.html"; WorkingDir: "{app}\doc"
Name: "{group}\Uninstall Quat"; Filename: "{app}\unins000.exe"
Name: "{userdesktop}\Quat"; Filename: "{app}\quat.exe"; WorkingDir: "{app}"; Tasks: desktopicon

[Tasks]
Name: desktopicon; Description: "Create a &desktop icon"; GroupDescription: "Additional tasks:";
Name: associate; Description: "&Add to context menus of png, zpn and ini"; GroupDescription: "Additional tasks:";

[Registry]
Tasks: associate; Root: HKCR; Subkey: ".png"; ValueType: string; ValueName: ""; ValueData: "pngfile"; 
;Flags: createvalueifdoesntexist
Tasks: associate; Root: HKCR; Subkey: "pngfile\shell\Quat"; Flags: uninsdeletekey
Tasks: associate; Root: HKCR; Subkey: "pngfile\shell\Quat\command"; ValueType: string; ValueName: ""; ValueData: """{app}\quat.exe"" ""%1"""; Flags: uninsdeletekey

Tasks: associate; Root: HKCR; Subkey: ".ini"; ValueType: string; ValueName: ""; ValueData: "inifile"; 
;Flags: createvalueifdoesntexist
Tasks: associate; Root: HKCR; Subkey: "inifile\shell\Quat"; Flags: uninsdeletekey
Tasks: associate; Root: HKCR; Subkey: "inifile\shell\Quat\command"; ValueType: string; ValueName: ""; ValueData: """{app}\quat.exe"" ""%1"""; Flags: uninsdeletekey

Tasks: associate; Root: HKCR; Subkey: ".zpn"; ValueType: string; ValueName: ""; ValueData: "zpnfile"; Flags: uninsdeletevalue
Tasks: associate; Root: HKCR; Subkey: "zpnfile"; ValueType: string; ValueName: ""; ValueData: "Quat zbuffer file"; Flags: uninsdeletevalue
Tasks: associate; Root: HKCR; Subkey: "zpnfile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\quat.exe,0"
Tasks: associate; Root: HKCR; Subkey: "zpnfile\shell\Quat\command"; ValueType: string; ValueName: ""; ValueData: """{app}\quat.exe"" ""%1"""

