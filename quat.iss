; -- quat.iss --

[Setup]
AppName=Quat
AppVerName=Quat 1.20
AppCopyright=Copyright (C) 1997-2002 Dirk Meyer
OutputBaseFilename=quat-1.20-setup
DefaultDirName={pf}\Quat
DefaultGroupName=Quat
UninstallDisplayIcon={app}\Quat.exe
Compression=bzip
LicenseFile=COPYING
ChangesAssociations=yes
; uncomment the following line if you want your installation to run on NT 3.51 too.
; MinVersion=4,3.51

[Files]
Source: "quat.exe"; DestDir: "{app}"
Source: "quat-txt.exe"; DestDir: "{app}"
Source: "COPYING"; DestDir: "{app}"
Source: "ChangeLog"; DestDir: "{app}"

Source: "doc\ce.png"; DestDir: "{app}\doc"
Source: "doc\chart_de.png"; DestDir: "{app}\doc"
Source: "doc\chart_us.png"; DestDir: "{app}\doc"
Source: "doc\Dendr_st.jpg"; DestDir: "{app}\doc"
Source: "doc\ex_1.jpg"; DestDir: "{app}\doc"
Source: "doc\ex_2.jpg"; DestDir: "{app}\doc"
Source: "doc\ex_3.jpg"; DestDir: "{app}\doc"
Source: "doc\gpl.html"; DestDir: "{app}\doc"
Source: "doc\ie.png"; DestDir: "{app}\doc"
Source: "doc\oe.png"; DestDir: "{app}\doc"
Source: "doc\ote.png"; DestDir: "{app}\doc"
Source: "doc\quat.png"; DestDir: "{app}\doc"
Source: "doc\quat-de.html"; DestDir: "{app}\doc"
Source: "doc\quat-de-1.html"; DestDir: "{app}\doc"
Source: "doc\quat-de-2.html"; DestDir: "{app}\doc"
Source: "doc\quat-de-3.html"; DestDir: "{app}\doc"
Source: "doc\quat-de-4.html"; DestDir: "{app}\doc"
Source: "doc\quat-de-5.html"; DestDir: "{app}\doc"
Source: "doc\quat-us.html"; DestDir: "{app}\doc"
Source: "doc\quat-us-1.html"; DestDir: "{app}\doc"
Source: "doc\quat-us-2.html"; DestDir: "{app}\doc"
Source: "doc\quat-us-3.html"; DestDir: "{app}\doc"
Source: "doc\quat-us-4.html"; DestDir: "{app}\doc"
Source: "doc\quat-us-5.html"; DestDir: "{app}\doc"
Source: "doc\ve.png"; DestDir: "{app}\doc"

Source: "examples\cut\complex.cut"; DestDir: "{app}\examples\cut"
Source: "examples\cut\cut.col"; DestDir: "{app}\examples\cut"
Source: "examples\cut\cut.ini"; DestDir: "{app}\examples\cut"
Source: "examples\cut\cut.obj"; DestDir: "{app}\examples\cut"
Source: "examples\dendr_aa\dendr_aa.col"; DestDir: "{app}\examples\dendr_aa"
Source: "examples\dendr_aa\dendr_aa.ini"; DestDir: "{app}\examples\dendr_aa"
Source: "examples\dendr_aa\dendr_aa.obj"; DestDir: "{app}\examples\dendr_aa"
Source: "examples\dendrit1\dendr_st.ini"; DestDir: "{app}\examples\dendrit1"
Source: "examples\dendrit1\dendrit1.col"; DestDir: "{app}\examples\dendrit1"
Source: "examples\dendrit1\dendrit1.ini"; DestDir: "{app}\examples\dendrit1"
Source: "examples\dendrit1\dendrit1.obj"; DestDir: "{app}\examples\dendrit1"
Source: "examples\dezent\dezent.ini"; DestDir: "{app}\examples\dezent"
Source: "examples\drache\drache.ini"; DestDir: "{app}\examples\drache"
Source: "examples\drache\drache.obj"; DestDir: "{app}\examples\drache"
Source: "examples\drache\reds.col"; DestDir: "{app}\examples\drache"
Source: "examples\fetz_aa\fetz_aa.col"; DestDir: "{app}\examples\fetz_aa"
Source: "examples\fetz_aa\fetz_aa.ini"; DestDir: "{app}\examples\fetz_aa"
Source: "examples\fetz_aa\fetz_aa.obj"; DestDir: "{app}\examples\fetz_aa"
Source: "examples\fetzen\fetzen.col"; DestDir: "{app}\examples\fetzen"
Source: "examples\fetzen\fetzen.ini"; DestDir: "{app}\examples\fetzen"
Source: "examples\fetzen\fetzen.obj"; DestDir: "{app}\examples\fetzen"
Source: "examples\filigran\filigran.col"; DestDir: "{app}\examples\filigran"
Source: "examples\filigran\filigran.ini"; DestDir: "{app}\examples\filigran"
Source: "examples\filigran\filigran.obj"; DestDir: "{app}\examples\filigran"
Source: "examples\icon\icon.ini"; DestDir: "{app}\examples\icon"
Source: "examples\nice\nice.col"; DestDir: "{app}\examples\nice"
Source: "examples\nice\nice.ini"; DestDir: "{app}\examples\nice"
Source: "examples\nice\nice.obj"; DestDir: "{app}\examples\nice"
Source: "examples\nocut\nocut.col"; DestDir: "{app}\examples\nocut"
Source: "examples\nocut\nocut.ini"; DestDir: "{app}\examples\nocut"
Source: "examples\nocut\nocut.obj"; DestDir: "{app}\examples\nocut"
Source: "examples\sleipnir\sleipnir.ini"; DestDir: "{app}\examples\sleipnir"
Source: "examples\title1\title1.ini"; DestDir: "{app}\examples\title1"

[Icons]
Name: "{group}\Quat 1.20"; Filename: "{app}\quat.exe"; WorkingDir: "{app}"
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

