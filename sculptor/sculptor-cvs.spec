# rpm spec file for Sculptor.
#
# 1) Set QTDIR to point to the qt release you want to build sculptor for.
# 
# 2) You find the following packages:
#	RPM-BUILD-TREE/RPMS/i386/sculptor-cvs-1.i386.rpm
#       RPM-BUILD-TREE/SRPMS/sculptor-cvs-1.src.rpm

%define name	sculptor
%define version	cvs
%define release	%(date +'%m%d%y')

Summary:	Sculptor
Name:		%{name}
Version:	%{version}
Release:	%{release}
License:	GNU Lesser GPL with restrictions for software not written by us
URL:		http://sculptor.biomachina.org
Group:		Application/Scientific
BuildRoot:	%{_tmppath}/%{name}-buildroot

Requires:	qwt
Requires:	svt
BuildRequires:	qwt-devel svt-devel

%description
Sculptor is an interactive molecular graphics program for modeling high-resolution
molecular data and low-resolution density maps.

###############################################################################
# Prepare
###############################################################################

%prep

# remove old source
rm -rf %{name}

export CVSROOT=:ext:sbirmanns@129.106.149.105:/cvs/repository
export CVS_RSH=ssh

# export current code
cvs export -D 2011 %{name}

###############################################################################
# Build
###############################################################################

%build

# build the application
cd %{name}
export SVT_PLATFORM=fc2
export SVT_DIR=/usr/lib/svt
$QTDIR/bin/qmake %{name}.pro -o Makefile
make

###############################################################################
# Install
###############################################################################

%install
cd %{name}
rm -rf %{buildroot}

# install, preserving links
mkdir -p %{buildroot}/%{_bindir}
cp %{name} %{buildroot}/%{_bindir}

%clean
rm -rf %{buildroot}

%files
%defattr(-, root, root)
%{_bindir}/%{name}

%changelog
* Fri Jan 13 2006 Stefan Birmanns <sculptor@biomachina.org>
- spec file for sculptor-1.0.0 - updated and simplified
* Wed Oct 22 2003 Stefan Birmanns <sculptor@biomachina.org>
- spec file for sensitus-0.9.0
