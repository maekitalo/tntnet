Name:		tntnet
Summary:	Server and framework for c++-webapplications
Version:	1.6.0
Release:	1
License:	GPL
Group:		Productivity/Networking/Web/Servers
Url:		http://www.tntnet.org
Source:		http://www.tntnet.org/download/tntnet-%{version}.tar.gz
BuildRoot:	%{_tmppath}/%{name}-%{version}-build
Requires:	cxxtools >= 1.4.4

%description
tntnet is a Webserver for c++-webapplications including tools
for creating these applications.

%package devel
Summary: Include Files and Libraries mandatory for Development.
Group: Productivity/Networking/Web/Servers
Requires:     tntnet = %{version} cxxtools-devel >= 1.4.4

%description devel
This package contains all necessary include files and libraries needed
to develop applications that require the provided includes and
libraries.

%prep
%setup

%build
CFLAGS="$RPM_OPT_FLAGS" \
./configure --prefix=%{_prefix} --libdir=%{_libdir} --sysconfdir=/etc  --disable-final
make

%install
make DESTDIR=$RPM_BUILD_ROOT \
     LIBDIR=$RPM_BUILD_ROOT/%{_libdir} \
     MANDIR=%{buildroot}%{_mandir} \
     CONFDIR=$RPM_BUILD_ROOT/etc/tntnet \
     INITDIR=$RPM_BUILD_ROOT/etc/init.d \
     install
install -d -m 755 %{buildroot}/etc/init.d
install -m 644 etc/init.d/tntnet %{buildroot}/etc/init.d/tntnet
install -d -m 755 %{buildroot}/etc/tntnet
install -m 644 etc/tntnet/tntnet.conf %{buildroot}/etc/tntnet/tntnet.conf
install -m 644 etc/tntnet/mime.conf %{buildroot}/etc/tntnet/mime.conf
install -m 644 etc/tntnet/tntnet.properties %{buildroot}/etc/tntnet/tntnet.properties

install -m 644 doc/man/*.1 %{buildroot}%{_mandir}/man1
install -m 644 doc/man/*.7 %{buildroot}%{_mandir}/man7
install -m 644 doc/man/*.8 %{buildroot}%{_mandir}/man8

rm %{buildroot}/usr/lib/*.la
rm %{buildroot}/usr/lib/tntnet/*.la


%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_libdir}/libtntnet*.so*
%{_libdir}/tntnet/*.so*
/usr/bin/tntnet
%{_mandir}/man8/*
%{_mandir}/man7/tntnet*
/etc/init.d/tntnet
/etc/tntnet/tntnet.conf
/etc/tntnet/mime.conf
/etc/tntnet/tntnet.properties

%files devel
%{_includedir}/tnt/*.h
/usr/%{_lib}/libtntnet_sdk.so*
/usr/bin/tntnet-config
/usr/bin/ecppc
/usr/bin/ecppl
/usr/bin/ecppll
%{_mandir}/man1/*
%{_mandir}/man7/ecpp*


%changelog
