Name:       peripheral-bus
Summary:    Tizen Peripheral Input & Output Service Daemon
Version:    0.0.1
Release:    0
Group:      System & System Tools
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1:    %{name}.manifest
Source2:    %{name}.service
BuildRequires:  cmake
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(gio-2.0)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(capi-system-peripheral-io)

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
Tizen Peripheral Input & Output Service Daemon

%prep
%setup -q
cp %{SOURCE1} ./
cp %{SOURCE2} ./

%build
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
%cmake . -DMAJORVER=${MAJORVER} -DFULLVER=%{version}

%install

%make_install
mkdir -p %{buildroot}%{_unitdir}/multi-user.target.wants
install -m 0644 %SOURCE2 %{buildroot}%{_unitdir}/peripheral-bus.service
%install_service multi-user.target.wants peripheral-bus.service

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%license LICENSE.APLv2
%{_bindir}/peripheral-bus
%{_unitdir}/peripheral-bus.service
%{_unitdir}/multi-user.target.wants/peripheral-bus.service
