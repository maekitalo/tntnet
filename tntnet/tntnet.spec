Summary: Server and framework for c++-webapplications
Name: tntnet
Version: 1.0.0
Release: 2
License: GPL
Group: Productivity/Networking/Web/Servers
Source: tntnet-1.0.0.tgz
BuildRoot: %{_tmppath}/%{name}-%{version}-build
Requires: log4cplus cxxtools

%description
tntnet is a Webserver for c++-webapplications including tools
for creating these applications.

%package devel
Summary: Include Files and Libraries mandatory for Development.
Group: Productivity/Networking/Web/Servers
Requires:     tntnet = %{version} log4cplus-devel cxxtools-devel

%description devel
This package contains all necessary include files and libraries needed
to develop applications that require the provided includes and
libraries.

%prep
%setup

%build
make

%install
make DESTDIR=$RPM_BUILD_ROOT/usr LIBDIR=$RPM_BUILD_ROOT/usr/%{_lib} CONFDIR=$RPM_BUILD_ROOT/etc/tntnet INITDIR=$RPM_BUILD_ROOT/etc/init.d install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
/usr/%{_lib}/libtntnet.so*
/usr/%{_lib}/libecpp.so*
/usr/%{_lib}/tntnet.so
/usr/bin/tntnet
/etc/tntnet/*
/etc/init.d/tntnet

%files devel
/usr/include/tnt/*.h
/usr/%{_lib}/libtntnet_sdk.so*
/usr/bin/ecppc
/usr/bin/ecppl
/usr/bin/ecppll

%changelog
