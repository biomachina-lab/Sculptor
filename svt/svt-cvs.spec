# rpm spec file for svt - the scientific visualization toolkit
#
# 1) Set QTDIR to point to the qt release you want to build svt for (svt actually does only use qmake as build system, the qt library is not used)
#
# 2) The spec file builds a svt and a svt-devel package. The svt-devel
#    package includes also the documentation. 
#
# 3) You find the following packages:
#	RPM-BUILD-TREE/RPMS/i386/svt-X.X.X-N.i386.rpm
#       RPM-BUILD-TREE/RPMS/i386/svt-devel-X.X.X-N.i386.rpm
#       RPM-BUILD-TREE/SRPMS/svt-X.X.X-N.src.rpm


%define name	svt
%define version	cvs
%define release	%(date +'%m%d%y')

Summary:	scientific visualization toolkit (svt)
Name:		%{name}
Version:	%{version}
Release:	%{release}
License:	GNU Lesser GPL with restrictions for software not written by us
URL:		http://sculptor.biomachina.org
Group:		System/Libraries
BuildRoot:	%{_tmppath}/%{name}-buildroot

Requires:	Cg

%description
SVT is a C++ toolkit with a focus on the visualization of biophysical datasets. It can visualize a variety of data types (molecular structures, volume-data) and can be used on standard workstations/PC and full virtual reality environments.

%package devel
Summary:	Scientifc Visualization Toolkit (SVT)
Group:		Development/C++

%description devel
SVT is a C++ toolkit with a focus on the visualization of biophysical datasets. It can visualize a variety of data types (molecular structures, volume-data) and can be used on standard workstations/PC and full virtual reality environments.

###############################################################################
# Prepare
###############################################################################

%prep

# remove old source
rm -rf svt

# set repository variables
export CVSROOT=:ext:sbirmanns@129.106.149.105:/cvs/repository
export CVS_RSH=ssh

# export current code
cvs export -D 2011 %{name}

###############################################################################
# Build
###############################################################################

%build
export SVT_DIR=${RPM_BUILD_DIR}/svt

# build the library
cd %{name}
$QTDIR/bin/qmake svt.pro -o Makefile
make
doxygen svt.dox

###############################################################################
# Install
###############################################################################

%install
cd svt
rm -rf %{buildroot}
mkdir -p %{buildroot}/%{_includedir}/svt
mkdir -p %{buildroot}/%{_libdir}
mkdir -p %{buildroot}/%{_libdir}/svt
mkdir -p %{buildroot}/%{_libdir}/svt/qmake
mkdir -p %{buildroot}/%{_defaultdocdir}/svt

# install the header files
for n in basics/include/*.h ; do
    install -m 644 $n %{buildroot}/%{_includedir}/svt
done
for n in system/include/*.h ; do
    install -m 644 $n %{buildroot}/%{_includedir}/svt
done
for n in file_io/include/*.h ; do
    install -m 644 $n %{buildroot}/%{_includedir}/svt
done
for n in live/include/*.h ; do
    install -m 644 $n %{buildroot}/%{_includedir}/svt
done
for n in core/include/*.h ; do
    install -m 644 $n %{buildroot}/%{_includedir}/svt
done
for n in pdb/include/*.h ; do
    install -m 644 $n %{buildroot}/%{_includedir}/svt
done

# install doxygen documentation
for n in doc/html/* ; do
    install -m 644 $n %{buildroot}/%{_defaultdocdir}/svt
done

# install qmake files
for n in qmake/*.pri ; do
    install -m 644 $n %{buildroot}/%{_libdir}/svt/qmake
done

# install the libs, preserving links
chmod 644 lib/lib*.so*
for n in lib/lib*.so* ; do
    cp -d $n %{buildroot}/%{_libdir}
done

# the %%post and %%postun directives are superfluous on SuSE, but harmless
%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%clean
rm -rf %{buildroot}

%files
%defattr(-, root, root)
# %doc CHANGES COPYING README
%{_libdir}/lib*.so*

%files devel
%defattr(-, root, root)
%dir %{_defaultdocdir}/svt
%doc %{_defaultdocdir}/svt/*
%dir %{_includedir}/svt
%{_includedir}/svt/*
%{_libdir}/svt/qmake/*.pri

%changelog
* Fri Jan 13 2006 Stefan Birmanns <sculptor@biomachina.org>
- spec file for svt-1.0.0 - updated and simplified
* Mon Oct 20 2003 Stefan Birmanns <sculptor@biomachina.org>
- spec file for svt-0.9.1
