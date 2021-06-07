Name: clockwork
Summary: Customize your Sailfish OS launcher icons
Version: 0.1.0
Release: 1
License: MIT
URL: https://github.com/dseight/clockwork
Source0: %{name}-%{version}.tar.bz2
BuildRequires: pkgconfig(Qt5Core)
BuildRequires: pkgconfig(Qt5Quick)
BuildRequires: pkgconfig(Qt5Qml)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(sailfishsilica)
BuildRequires: qt5-qttools-linguist
BuildRequires: systemd
Provides: harbour-themepacksupport = 0.8.9-1
Conflicts: harbour-dyncal
Conflicts: harbour-dynclock

%define __provides_exclude_from ^%{_datadir}/.*$

%description
%{summary}.

%package devel
Summary: Development files for creating custom clockwork modules
Requires: Requires: %{name} = %{version}-%{release}

%description devel
%{summary}.

%prep
%autosetup

%build
%qmake5 -r \
    USERUNITDIR=%{_userunitdir} \
    VERSION=%{version}
%make_build

%install
%make_install

install -dm755 %{buildroot}%{_userunitdir}/user-session.target.wants/
ln -s ../clockwork.service %{buildroot}%{_userunitdir}/user-session.target.wants/

# Prepare directory for storing generated icons, so it will be owned by package
mkdir -p %{buildroot}%{_datadir}/%{name}/icons

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/jolla-settings/pages/%{name}
%{_datadir}/jolla-settings/entries/%{name}.json
%{_libdir}/*.so.*
%{_libdir}/qt5/qml/com/dseight/%{name}
%{_userunitdir}/%{name}.service
%{_userunitdir}/user-session.target.wants/%{name}.service

%files devel
%defattr(-,root,root,-)
%{_libdir}/*.so
%{_libdir}/pkgconfig/%{name}.pc
%{_includedir}/%{name}/*.h

%post
/sbin/ldconfig

# Daemon must be able to write into desktop files
setcap cap_dac_override+ep %{_bindir}/%{name}

if [ "$1" -ge 1 ]; then
    systemctl-user daemon-reload || :
    systemctl-user restart %{name}.service || :
fi

%preun
if [ "$1" -eq 0 ]; then
    systemctl-user stop %{name}.service || :
    systemctl-user daemon-reload || :
fi

%postun
/sbin/ldconfig

# Restart daemon when new icon pack is installed
%transfiletriggerin -- %{_datadir}
if grep -qF %{_datadir}/harbour-themepack- ; then
    systemctl-user try-restart %{name}.service
fi

# Restart daemon when icon pack is removed
%transfiletriggerun -- %{_datadir}
if grep -qF %{_datadir}/harbour-themepack- ; then
    systemctl-user try-restart %{name}.service
fi
