Name:       peripheral-bus
Summary:    Tizen Peripheral Input & Output Service Daemon
Version:    0.0.4
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
BuildRequires:  pkgconfig(iniparser)

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
Tizen Peripheral Input & Output Service Daemon

%prep
%setup -q
cp %{SOURCE1} ./
cp %{SOURCE2} ./

%build
export CFLAGS="$CFLAGS -DSYSCONFDIR=\\\"%{_sysconfdir}\\\""
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
%cmake . -DMAJORVER=${MAJORVER} -DFULLVER=%{version}

%install

%make_install
mkdir -p %{buildroot}%{_unitdir}/multi-user.target.wants
install -m 0644 %SOURCE2 %{buildroot}%{_unitdir}/peripheral-bus.service
%install_service multi-user.target.wants peripheral-bus.service

mkdir -p %{buildroot}/etc/peripheral-bus
cp %{_builddir}/%{name}-%{version}/data/*.ini %{buildroot}/etc/%{name}

%post
systemctl daemon-reload
if [ $1 == 1 ]; then
	# install
	systemctl start %{name}.service
elif [ $1 == 2 ]; then
	# upgrade
	systemctl restart %{name}.service
fi

%preun
if [ $1 == 0 ]; then
	# uninstall
	systemctl stop %{name}.service
fi

%postun
systemctl daemon-reload

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%license LICENSE.APLv2
%{_bindir}/peripheral-bus
%{_unitdir}/peripheral-bus.service
%{_unitdir}/multi-user.target.wants/peripheral-bus.service
/etc/peripheral-bus/*.ini
