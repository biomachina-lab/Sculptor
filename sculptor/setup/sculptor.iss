; Sculptor Setup Script
[Setup]
AppName=Sculptor
AppVersion=2.1.1
AppPublisher=Biomachina.org
OutputBaseFilename=sculptor
AppPublisherURL=http://sculptor.biomachina.org
AppSupportURL=http://sculptor.biomachina.org
AppUpdatesURL=http://sculptor.biomachina.org
DefaultDirName={pf}\Sculptor
DefaultGroupName=Sculptor
OutputDir=..\setup
; AlwaysCreateUninstallIcon=yes

[Tasks]
Name: "desktopicon"; Description: "Create &desktop icons"; GroupDescription: "Desktop icons:"; MinVersion: 4,4

[Dirs]
; real subdirs
Name: "{app}\bin"
; Name: "{app}\phantom"
; seperate groups in the program menu
; Name: "{group}\PHANToM"

[Files]
; sensitus executable and default configuration files
Source: "..\release\sculptor.exe"; DestDir: "{app}\bin";
Source: "..\sculptor.ico"; DestDir: "{app}\bin";

; readme files
Source: "..\Readme.txt"; DestDir: "{app}"; CopyMode: alwaysoverwrite

; dynamic link libraries
; Source: "..\..\svt\lib\svt_basics.dll"; DestDir: "{app}\bin";
; Source: "..\..\svt\lib\svt_file_io.dll"; DestDir: "{app}\bin";
; Source: "..\..\svt\lib\svt_system.dll"; DestDir: "{app}\bin";
; Source: "..\..\svt\lib\svt_core.dll"; DestDir: "{app}\bin";
; Source: "..\..\svt\lib\live.dll"; DestDir: "{app}\bin";
; Source: "..\..\svt\lib\svt_pdb.dll"; DestDir: "{app}\bin";
; Qt and Qwt
; Source: "C:\Devel\qwt4\lib\qwt.dll"; DestDir: "{app}\bin";
; Cg
Source: "C:\Devel\Cg\bin\cg.dll"; DestDir: "{app}\bin";
Source: "C:\Devel\Cg\bin\cgGL.dll"; DestDir: "{app}\bin";
; MinGW
Source: "C:\Devel\mingw\bin\mingwm10.dll"; DestDir: "{app}\bin";
; libgomp
; Source: "C:\Users\M\mingw\bin\libgomp-1.dll"; DestDir: "{app}\bin";
; legacy pthreadGC2.dll
Source: "C:\Devel\mingw\bin\pthreadGC2.dll"; DestDir:  "{app}\bin";

; phantom support files
; Source: "..\..\svt\lib\live_phantom.dll"; DestDir: "{app}\bin";
; Source: "..\..\lib\PHANToMIOLib.dll"; DestDir: "{app}\bin";
; Source: "..\..\lib\GHOST40.dll"; DestDir: "{app}\bin";

; falcon support files
Source: "..\..\svt\lib\live_falcon.dll"; DestDir: {app}\bin

; configuration files
; Source: "..\phantom\.svtrc"; DestDir: "{app}\phantom";
; Source: "..\.svtrc"; DestDir: "{app}";

[Icons]
Name: "{group}\Sculptor"; Filename: "{app}\bin\sculptor.exe"; WorkingDir: "{app}\bin";
Name: "{group}\Sculptor Config"; Filename: "notepad.exe"; Parameters: "{app}\.svtrc";
Name: "{userdesktop}\Sculptor"; Filename: "{app}\bin\sculptor.exe"; WorkingDir: "{app}\bin"; MinVersion: 4,4; Tasks: desktopicon
Name: "{group}\Readme"; Filename: "{app}\Readme.txt";
Name: "{group}\Uninstall"; Filename: "{uninstallexe}"

; Name: "{group}\PHANToM\Sculptor (PHANToM)"; IconFilename: "{app}\bin\sculptor.ico"; Filename: "{app}\bin\sculptor.exe"; Parameters: "-c ""{app}\phantom\.svtrc"""; WorkingDir: "{app}\data";
; Name: "{group}\PHANToM\Sculptor (PHANToM) Config"; Filename: "notepad.exe"; Parameters: "{app}\phantom\.svtrc";
