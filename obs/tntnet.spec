#
# spec file for package tntnet
#
# Copyright (c) 2014 SUSE LINUX Products GmbH, Nuernberg, Germany.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#


%define major   9
Name:           tntnet
Version:        2.2.1
Release:        etn4
Summary:        Tntnet is a web application server for web applications written in C++
License:        GPL-2.0
Group:          Productivity/Networking/Web/Servers
Url:            http://www.tntnet.org/index.html
Source0:        %{name}-%{version}.tar.gz
Source1:        debian.tntnet@.service
#Patch0:         libz_compress_713693.patch
#Patch1:         alldemos_path.patch
#Patch2:         xml-camelCase-fixes.patch
#Patch3:         fix_gcc47.patch
#Patch4:         tntnet.xml.7.section.patch
#Patch5:         documentroot-730374.patch
#Patch6:         0001-Use-monotonic-time-when-possible.patch
#Patch7:         tntnet_ECDHE_support.patch
#Patch8:         sslCipherList.patch
#Patch9:         0002-tntnet-2.2.1-allUserGroups.patch
BuildRequires:  automake
BuildRequires:  autoconf
BuildRequires:  findutils
BuildRequires:  gcc-c++
BuildRequires:  glibc-devel
BuildRequires:  pkgconfig(gnutls)
BuildRequires:  pkgconfig(cxxtools)
BuildRequires:  lzo
BuildRequires:  lzo-devel
BuildRequires:  pkgconfig(openssl)
BuildRequires:  libtool
BuildRequires:  systemd-devel
BuildRequires:  zip
BuildRequires:  pkgconfig(zlib)
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
%{systemd_requires}

%description
Tntnet is a web application server for web applications written in C++.

You can write a Web-page with HTML and with special tags you embed
C++-code into the page for active contents. These pages, called
components are compiled into C++-classes with the ecpp-compilier
"ecppc", then compiled into objectcode and linked into a shared library.
This shared library is loaded by the webserver "tntnet" on request and
executed.

%package -n libtntnet%{major}
Summary:        Shared library part of tntnet
Group:          System/Libraries

%description -n libtntnet%{major}
Tntnet is a web application server for web applications written in C++.

Shared library part of tntnet

%package demos
Summary:        Example applications for tntnet
Group:          Productivity/Networking/Web/Servers
Requires:       tntnet

%description demos
Tntnet is a web application server for web applications written in C++.

This package provides few examples written in it.

%package devel
Summary:        Development files of tntnet
Group:          Development/Libraries/C and C++
Requires:       glibc-devel
Requires:       pkgconfig(gnutls)
Requires:       pkgconfig(cxxtools)
Requires:       libtntnet%{major} = %{version}
Requires:       lzo
Requires:       lzo-devel
Requires:       pkgconfig(openssl)
Requires:       pkgconfig(zlib)
# various home projects does use spurious lib prefix for devel files, lets be compatible
Provides:       lib%{name}-devel = %{version}

%description devel
Tntnet is a web application server for web applications written in C++.
Development files

%package runtime
Summary:        Tntdb is a c++-class-library for easy database-access
Group:          System/Libraries
Requires:       glibc-devel

%description runtime
Tntnet is a web application server for web applications written in C++.
Runtime library (tntnet.so).

%prep
%setup -q -n tntnet-%{version}
cp -r sdk/demos sdk/demos-pure
#%patch0 -p1
#%patch1 -p1
#%patch2 -p1
#%patch3 -p1
#%patch4 -p1
#%patch5 -p1
#%patch6 -p1
#%patch7 -p1
#%patch8 -p1
#%patch9 -p1

%build
./autogen.sh
%configure
make %{?_smp_mflags}

%install
make DESTDIR=%{buildroot} install %{?_smp_mflags}
install -D -m 0644 %{SOURCE1} %{buildroot}%{_libexecdir}/systemd/system/%{name}@.service
rm -rf %{buildroot}%{_sysconfdir}/init.d
mkdir -p %{buildroot}%{_datadir}/tntnet
cp -r sdk/demos-pure %{buildroot}%{_datadir}/tntnet/demos
rm -f %{buildroot}/%{_libdir}/*.*a

%post -n libtntnet%{major} -p /sbin/ldconfig

%postun -n libtntnet%{major} -p /sbin/ldconfig

%files
%defattr (-, root, root)
%doc AUTHORS ChangeLog COPYING README
%{_bindir}/*
%{_libexecdir}/systemd
%dir %{_sysconfdir}/tntnet
%{_datadir}/tntnet
%exclude %{_datadir}/tntnet/demos
%doc %{_mandir}/man1/*.1.gz
%doc %{_mandir}/man7/*.7.gz
%doc %{_mandir}/man8/*.8.gz

%files demos
%defattr(-,root,root)
%config(noreplace)%{_sysconfdir}/tntnet/tntnet.xml
%dir %{_libdir}/tntnet
%dir %{_datadir}/tntnet
%{_datadir}/tntnet/demos

%files -n libtntnet%{major}
%defattr(-,root,root)
%{_libdir}/*.so.*

%files devel
%defattr (-, root, root)
%dir %{_includedir}/tnt/
%{_includedir}/tnt/*.h
%{_libdir}/*.so
/usr/lib*/pkgconfig/*.pc

%files runtime
%defattr(-,root,root)
%{_libdir}/tntnet/*

%changelog

